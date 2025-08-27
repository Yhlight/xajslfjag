#include "CompilerDispatcher.h"
#include "../CHTL/CHTLContext/Context.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/HtmlGenerator.h"
#include "../Util/FileSystem/FileSystem.h"
#include <algorithm>
#include <thread>
#include <future>
#include <sstream>

namespace CHTL {

// CompilerDispatcher实现

CompilerDispatcher::CompilerDispatcher(const DispatcherConfig& cfg)
    : config(cfg) {
    InitializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::SetConfig(const DispatcherConfig& cfg) {
    config = cfg;
}

CompilationResult CompilerDispatcher::Compile(const std::string& sourceCode, std::shared_ptr<Context> context) {
    if (!context) {
        context = std::make_shared<Context>();
    }
    
    // 清空之前的错误和警告
    errors.clear();
    warnings.clear();
    
    try {
        // 创建统一扫描器
        if (!scanner) {
            scanner = ScannerFactory::CreateDefault();
        }
        
        // 设置源代码并扫描
        scanner->SetSourceCode(sourceCode);
        if (!scanner->Scan()) {
            CompilationResult result;
            result.success = false;
            result.errors = scanner->GetErrors();
            return result;
        }
        
        // 获取代码片段
        const auto& fragments = scanner->GetFragments();
        
        // 分发编译任务
        auto results = DispatchCompilation(fragments, context);
        
        // 合并结果
        auto finalResult = MergeResults(results);
        
        // 复制调度器的错误和警告
        finalResult.errors.insert(finalResult.errors.end(), errors.begin(), errors.end());
        finalResult.warnings.insert(finalResult.warnings.end(), warnings.begin(), warnings.end());
        
        return finalResult;
    }
    catch (const std::exception& e) {
        CompilationResult result;
        result.success = false;
        result.errors.push_back("编译过程中发生异常: " + std::string(e.what()));
        return result;
    }
}

CompilationResult CompilerDispatcher::CompileFile(const std::string& filePath, std::shared_ptr<Context> context) {
    if (!FileSystem::FileExists(filePath)) {
        CompilationResult result;
        result.success = false;
        result.errors.push_back("文件不存在: " + filePath);
        return result;
    }
    
    std::string sourceCode = FileSystem::ReadTextFile(filePath);
    if (sourceCode.empty()) {
        CompilationResult result;
        result.success = false;
        result.errors.push_back("无法读取文件或文件为空: " + filePath);
        return result;
    }
    
    if (!context) {
        context = std::make_shared<Context>();
    }
    context->SetCurrentFilePath(filePath);
    
    return Compile(sourceCode, context);
}

std::vector<CompilationResult> CompilerDispatcher::CompileFiles(const std::vector<std::string>& filePaths, 
                                                               std::shared_ptr<Context> context) {
    std::vector<CompilationResult> results;
    results.reserve(filePaths.size());
    
    for (const std::string& filePath : filePaths) {
        results.push_back(CompileFile(filePath, context));
    }
    
    return results;
}

const std::vector<std::string>& CompilerDispatcher::GetErrors() const {
    return errors;
}

const std::vector<std::string>& CompilerDispatcher::GetWarnings() const {
    return warnings;
}

void CompilerDispatcher::ClearCache() {
    resultCache.clear();
}

std::string CompilerDispatcher::GetStatistics() const {
    std::ostringstream oss;
    oss << "编译器调度器统计信息:\n";
    oss << "  缓存项数量: " << resultCache.size() << "\n";
    oss << "  错误数量: " << errors.size() << "\n";
    oss << "  警告数量: " << warnings.size() << "\n";
    oss << "  并行编译: " << (config.enableParallelCompilation ? "启用" : "禁用") << "\n";
    oss << "  交叉引用: " << (config.enableCrossReference ? "启用" : "禁用") << "\n";
    oss << "  优化: " << (config.enableOptimization ? "启用" : "禁用") << "\n";
    oss << "  最大并发数: " << config.maxConcurrency << "\n";
    
    if (scanner) {
        oss << "\n" << scanner->GetStatistics();
    }
    
    return oss.str();
}

void CompilerDispatcher::SetCHTLCompiler(std::unique_ptr<CHTLCompiler> compiler) {
    chtlCompiler = std::move(compiler);
}

void CompilerDispatcher::SetCHTLJSCompiler(std::unique_ptr<CHTLJSCompiler> compiler) {
    chtlJSCompiler = std::move(compiler);
}

void CompilerDispatcher::SetCSSCompiler(std::unique_ptr<CSSCompiler> compiler) {
    cssCompiler = std::move(compiler);
}

void CompilerDispatcher::SetJSCompiler(std::unique_ptr<JSCompiler> compiler) {
    jsCompiler = std::move(compiler);
}

void CompilerDispatcher::InitializeCompilers() {
    if (!chtlCompiler) {
        chtlCompiler = std::make_unique<CHTLCompiler>();
    }
    if (!chtlJSCompiler) {
        chtlJSCompiler = std::make_unique<CHTLJSCompiler>();
    }
    if (!cssCompiler) {
        cssCompiler = std::make_unique<CSSCompiler>();
    }
    if (!jsCompiler) {
        jsCompiler = std::make_unique<JSCompiler>();
    }
    
    if (!scanner) {
        scanner = ScannerFactory::CreateDefault();
    }
}

std::vector<CompilationResult> CompilerDispatcher::DispatchCompilation(const std::vector<CodeFragment>& fragments,
                                                                      std::shared_ptr<Context> context) {
    if (config.enableParallelCompilation) {
        return ParallelCompileFragments(fragments, context);
    } else {
        return SerialCompileFragments(fragments, context);
    }
}

CompilationResult CompilerDispatcher::CompileFragment(const CodeFragment& fragment, std::shared_ptr<Context> context) {
    // 生成缓存键
    std::string cacheKey = GenerateCacheKey(fragment.content, 
        fragment.type == FragmentType::CHTL ? CompilerType::CHTL :
        fragment.type == FragmentType::CHTL_JS ? CompilerType::CHTL_JS :
        fragment.type == FragmentType::CSS ? CompilerType::CSS :
        CompilerType::JAVASCRIPT);
    
    // 检查缓存
    auto cacheIt = resultCache.find(cacheKey);
    if (cacheIt != resultCache.end()) {
        return cacheIt->second;
    }
    
    CompilationResult result;
    
    try {
        switch (fragment.type) {
            case FragmentType::CHTL:
                if (chtlCompiler && chtlCompiler->CanCompile(fragment)) {
                    result = chtlCompiler->Compile(fragment.content, context);
                } else {
                    result.success = false;
                    result.errors.push_back("CHTL编译器不可用或无法编译此片段");
                }
                break;
                
            case FragmentType::CHTL_JS:
                if (chtlJSCompiler && chtlJSCompiler->CanCompile(fragment)) {
                    result = chtlJSCompiler->Compile(fragment.content, context);
                } else {
                    result.success = false;
                    result.errors.push_back("CHTL JS编译器不可用或无法编译此片段");
                }
                break;
                
            case FragmentType::CSS:
                if (cssCompiler && cssCompiler->CanCompile(fragment)) {
                    result = cssCompiler->Compile(fragment.content, context);
                } else {
                    result.success = false;
                    result.errors.push_back("CSS编译器不可用或无法编译此片段");
                }
                break;
                
            case FragmentType::JS:
                if (jsCompiler && jsCompiler->CanCompile(fragment)) {
                    result = jsCompiler->Compile(fragment.content, context);
                } else {
                    result.success = false;
                    result.errors.push_back("JavaScript编译器不可用或无法编译此片段");
                }
                break;
                
            case FragmentType::MIXED:
                // 对于混合片段，尝试使用CHTL编译器处理
                if (chtlCompiler && chtlCompiler->CanCompile(fragment)) {
                    result = chtlCompiler->Compile(fragment.content, context);
                } else {
                    result.success = false;
                    result.errors.push_back("混合片段无法被任何编译器处理");
                }
                break;
                
            case FragmentType::UNKNOWN:
                // 对于未知片段，也尝试使用CHTL编译器处理
                if (chtlCompiler) {
                    result = chtlCompiler->Compile(fragment.content, context);
                } else {
                    result.success = false;
                    result.errors.push_back("未知片段类型且CHTL编译器不可用");
                }
                break;
                
            default:
                result.success = false;
                result.errors.push_back("不支持的片段类型: " + FragmentTypeToString(fragment.type));
                break;
        }
    }
    catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("编译片段时发生异常: " + std::string(e.what()));
    }
    
    // 缓存结果
    resultCache[cacheKey] = result;
    
    return result;
}

CompilationResult CompilerDispatcher::MergeResults(const std::vector<CompilationResult>& results) {
    CompilationResult merged;
    merged.success = true;
    
    // 检查是否所有结果都成功
    for (const auto& result : results) {
        if (!result.success) {
            merged.success = false;
        }
        
        // 合并错误和警告
        merged.errors.insert(merged.errors.end(), result.errors.begin(), result.errors.end());
        merged.warnings.insert(merged.warnings.end(), result.warnings.begin(), result.warnings.end());
    }
    
    if (merged.success) {
        // 生成最终HTML
        merged.content = GenerateFinalHTML(results);
        merged.compilerType = CompilerType::CHTL; // 主编译器类型
        
        // 合并元数据
        for (const auto& result : results) {
            for (const auto& pair : result.metadata) {
                merged.metadata[pair.first] = pair.second;
            }
        }
    }
    
    return merged;
}

void CompilerDispatcher::ProcessCrossReferences(std::vector<CompilationResult>& results) {
    if (!config.enableCrossReference) {
        return;
    }
    
    // TODO: 实现交叉引用处理逻辑
}

void CompilerDispatcher::OptimizeResults(std::vector<CompilationResult>& results) {
    if (!config.enableOptimization) {
        return;
    }
    
    // TODO: 实现结果优化逻辑
}

std::string CompilerDispatcher::GenerateFinalHTML(const std::vector<CompilationResult>& results) {
    std::ostringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"zh-CN\">\n";
    html << "<head>\n";
    html << "    <meta charset=\"UTF-8\">\n";
    html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "    <title>CHTL编译结果</title>\n";
    
    // 合并CSS
    html << "    <style>\n";
    for (const auto& result : results) {
        if (result.compilerType == CompilerType::CSS && result.success) {
            html << result.content << "\n";
        }
    }
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    
    // 合并HTML内容
    for (const auto& result : results) {
        if (result.compilerType == CompilerType::CHTL && result.success) {
            html << result.content << "\n";
        }
    }
    
    // 合并JavaScript
    html << "    <script>\n";
    for (const auto& result : results) {
        if ((result.compilerType == CompilerType::CHTL_JS || result.compilerType == CompilerType::JAVASCRIPT) 
            && result.success) {
            html << result.content << "\n";
        }
    }
    html << "    </script>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

void CompilerDispatcher::AddError(const std::string& message) {
    errors.push_back(message);
}

void CompilerDispatcher::AddWarning(const std::string& message) {
    warnings.push_back(message);
}

std::vector<CompilationResult> CompilerDispatcher::ParallelCompileFragments(const std::vector<CodeFragment>& fragments,
                                                                          std::shared_ptr<Context> context) {
    std::vector<std::future<CompilationResult>> futures;
    std::vector<CompilationResult> results;
    
    // 限制并发数
    size_t concurrency = std::min(config.maxConcurrency, fragments.size());
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        // 如果已达到最大并发数，等待一个任务完成
        if (futures.size() >= concurrency) {
            results.push_back(futures.front().get());
            futures.erase(futures.begin());
        }
        
        // 启动新的异步任务
        futures.push_back(std::async(std::launch::async, 
            [this, &fragments, i, context]() {
                return CompileFragment(fragments[i], context);
            }));
    }
    
    // 等待剩余任务完成
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    
    return results;
}

std::vector<CompilationResult> CompilerDispatcher::SerialCompileFragments(const std::vector<CodeFragment>& fragments,
                                                                         std::shared_ptr<Context> context) {
    std::vector<CompilationResult> results;
    results.reserve(fragments.size());
    
    for (const auto& fragment : fragments) {
        results.push_back(CompileFragment(fragment, context));
    }
    
    return results;
}

bool CompilerDispatcher::ValidateResults(const std::vector<CompilationResult>& results) {
    for (const auto& result : results) {
        if (!result.success) {
            return false;
        }
    }
    return true;
}

std::string CompilerDispatcher::GenerateCacheKey(const std::string& content, CompilerType type) {
    // 简单的缓存键生成（实际项目中可能需要更复杂的哈希算法）
    return CompilerTypeToString(type) + "_" + std::to_string(std::hash<std::string>{}(content));
}

// 编译器实现（临时基础实现）

CHTLCompiler::CHTLCompiler() = default;
CHTLCompiler::~CHTLCompiler() = default;

CompilationResult CHTLCompiler::Compile(const std::string& source, std::shared_ptr<Context> context) {
    CompilationResult result(CompilerType::CHTL);
    
    try {
        // 创建解析器
        auto parser = ParserFactory::CreateCHTLParser();
        
        // 解析CHTL源代码
        auto rootNode = parser->Parse(source);
        
        if (!rootNode || parser->HasErrors()) {
            result.success = false;
            for (const auto& error : parser->GetErrors()) {
                result.errors.push_back(error);
            }
            
            // 生成错误页面
            std::ostringstream html;
            html << "<div style=\"color: red; border: 1px solid red; padding: 10px; margin: 10px;\">\n";
            html << "<h3>CHTL编译错误</h3>\n";
            for (const auto& error : result.errors) {
                html << "<p>" << error << "</p>\n";
            }
            html << "</div>\n";
            result.content = html.str();
            return result;
        }
        
        // 创建HTML生成器
        auto generator = HtmlGeneratorFactory::CreateStandardGenerator();
        
        // 生成HTML
        auto generationResult = generator->Generate(rootNode);
        
        if (generationResult.success) {
            result.success = true;
            result.content = generationResult.htmlContent;
            
            // 添加警告信息
            for (const auto& warning : generationResult.warnings) {
                result.warnings.push_back(warning);
            }
        } else {
            result.success = false;
            for (const auto& error : generationResult.errors) {
                result.errors.push_back(error);
            }
            
            // 生成错误页面
            std::ostringstream html;
            html << "<div style=\"color: red; border: 1px solid red; padding: 10px; margin: 10px;\">\n";
            html << "<h3>CHTL生成错误</h3>\n";
            for (const auto& error : result.errors) {
                html << "<p>" << error << "</p>\n";
            }
            html << "</div>\n";
            result.content = html.str();
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("CHTL编译异常: " + std::string(e.what()));
        
        // 生成异常页面
        std::ostringstream html;
        html << "<div style=\"color: red; border: 1px solid red; padding: 10px; margin: 10px;\">\n";
        html << "<h3>CHTL编译异常</h3>\n";
        html << "<p>" << e.what() << "</p>\n";
        html << "<pre>" << source << "</pre>\n";
        html << "</div>\n";
        result.content = html.str();
    }
    
    return result;
}

bool CHTLCompiler::CanCompile(const CodeFragment& fragment) const {
    return fragment.type == FragmentType::CHTL || 
           fragment.type == FragmentType::MIXED || 
           fragment.type == FragmentType::UNKNOWN;
}

CHTLJSCompiler::CHTLJSCompiler() = default;
CHTLJSCompiler::~CHTLJSCompiler() = default;

CompilationResult CHTLJSCompiler::Compile(const std::string& source, std::shared_ptr<Context> context) {
    CompilationResult result(CompilerType::CHTL_JS);
    
    // 临时实现：转换为基本JavaScript
    result.content = "// CHTL JS编译结果\n" + source;
    result.success = true;
    
    return result;
}

bool CHTLJSCompiler::CanCompile(const CodeFragment& fragment) const {
    return fragment.type == FragmentType::CHTL_JS;
}

CSSCompiler::CSSCompiler() = default;
CSSCompiler::~CSSCompiler() = default;

CompilationResult CSSCompiler::Compile(const std::string& source, std::shared_ptr<Context> context) {
    CompilationResult result(CompilerType::CSS);
    
    // 临时实现：直接返回CSS
    result.content = "/* CSS编译结果 */\n" + source;
    result.success = true;
    
    return result;
}

bool CSSCompiler::CanCompile(const CodeFragment& fragment) const {
    return fragment.type == FragmentType::CSS;
}

JSCompiler::JSCompiler() = default;
JSCompiler::~JSCompiler() = default;

CompilationResult JSCompiler::Compile(const std::string& source, std::shared_ptr<Context> context) {
    CompilationResult result(CompilerType::JAVASCRIPT);
    
    // 临时实现：直接返回JavaScript
    result.content = "// JavaScript编译结果\n" + source;
    result.success = true;
    
    return result;
}

bool JSCompiler::CanCompile(const CodeFragment& fragment) const {
    return fragment.type == FragmentType::JS;
}

// 工厂方法实现

std::unique_ptr<CompilerDispatcher> CompilerFactory::CreateDefault() {
    return std::make_unique<CompilerDispatcher>();
}

std::unique_ptr<CompilerDispatcher> CompilerFactory::CreateHighPerformance() {
    DispatcherConfig config;
    config.enableParallelCompilation = true;
    config.maxConcurrency = std::thread::hardware_concurrency();
    config.enableOptimization = true;
    
    return std::make_unique<CompilerDispatcher>(config);
}

std::unique_ptr<CompilerDispatcher> CompilerFactory::CreateDebug() {
    DispatcherConfig config;
    config.enableParallelCompilation = false;
    config.enableDebugInfo = true;
    config.enableCrossReference = true;
    
    return std::make_unique<CompilerDispatcher>(config);
}

std::unique_ptr<ICompiler> CompilerFactory::CreateCompiler(CompilerType type) {
    switch (type) {
        case CompilerType::CHTL:
            return std::make_unique<CHTLCompiler>();
        case CompilerType::CHTL_JS:
            return std::make_unique<CHTLJSCompiler>();
        case CompilerType::CSS:
            return std::make_unique<CSSCompiler>();
        case CompilerType::JAVASCRIPT:
            return std::make_unique<JSCompiler>();
        default:
            return nullptr;
    }
}

// 辅助函数

std::string CompilerTypeToString(CompilerType type) {
    switch (type) {
        case CompilerType::CHTL: return "CHTL";
        case CompilerType::CHTL_JS: return "CHTL_JS";
        case CompilerType::CSS: return "CSS";
        case CompilerType::JAVASCRIPT: return "JavaScript";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL