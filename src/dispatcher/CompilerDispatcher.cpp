#include "CompilerDispatcher.h"
#include <algorithm>
#include <thread>
#include <future>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace chtl {

CompilerDispatcher::CompilerDispatcher() 
    : config(), scanner(std::make_unique<CHTLUnifiedScanner>()) {
    // 注册默认编译器
    registerCompiler(std::make_unique<CHTLCompilerWrapper>());
    registerCompiler(std::make_unique<CHTLJSCompilerWrapper>());
    registerCompiler(std::make_unique<CSSCompilerWrapper>());
    registerCompiler(std::make_unique<JSCompilerWrapper>());
}

CompilerDispatcher::CompilerDispatcher(const DispatcherConfig& config)
    : config(config), scanner(std::make_unique<CHTLUnifiedScanner>()) {
    // 注册默认编译器
    registerCompiler(std::make_unique<CHTLCompilerWrapper>());
    registerCompiler(std::make_unique<CHTLJSCompilerWrapper>());
    registerCompiler(std::make_unique<CSSCompilerWrapper>());
    registerCompiler(std::make_unique<JSCompilerWrapper>());
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::registerCompiler(std::unique_ptr<ICompiler> compiler) {
    if (!compiler) return;
    
    // 注册类型映射
    for (int i = 0; i < static_cast<int>(CodeFragmentType::UNKNOWN); ++i) {
        CodeFragmentType type = static_cast<CodeFragmentType>(i);
        if (compiler->canHandle(type)) {
            typeToCompiler[type] = compiler.get();
        }
    }
    
    compilers.push_back(std::move(compiler));
}

CompileResult CompilerDispatcher::dispatch(const std::vector<CodeFragment>& fragments) {
    CompileResult finalResult;
    
    if (fragments.empty()) {
        return finalResult;
    }
    
    // 检查缓存
    if (config.enableCaching) {
        std::string cacheKey = getCacheKey(fragments);
        if (getCachedResult(cacheKey, finalResult)) {
            return finalResult;
        }
    }
    
    // 按类型分组片段
    auto groupedFragments = groupFragmentsByType(fragments);
    
    // 编译各组片段
    std::vector<CompileResult> results;
    if (config.enableParallelCompilation && groupedFragments.size() > 1) {
        results = compileParallel(groupedFragments);
    } else {
        results = compileSerial(groupedFragments);
    }
    
    // 合并结果
    finalResult = mergeResults(results);
    
    // 缓存结果
    if (config.enableCaching && finalResult.success) {
        std::string cacheKey = getCacheKey(fragments);
        cacheResult(cacheKey, finalResult);
    }
    
    return finalResult;
}

CompileResult CompilerDispatcher::dispatchFile(const std::string& filePath) {
    auto fragments = scanner->scanFile(filePath);
    return dispatch(fragments);
}

ICompiler* CompilerDispatcher::getCompiler(const std::string& name) const {
    auto it = std::find_if(compilers.begin(), compilers.end(),
        [&name](const std::unique_ptr<ICompiler>& compiler) {
            return compiler->getName() == name;
        });
    
    return (it != compilers.end()) ? it->get() : nullptr;
}

ICompiler* CompilerDispatcher::getCompilerForType(CodeFragmentType type) const {
    auto it = typeToCompiler.find(type);
    return (it != typeToCompiler.end()) ? it->second : nullptr;
}

void CompilerDispatcher::clearCache() {
    cache.clear();
}

void CompilerDispatcher::setErrorHandler(std::function<void(const std::string&)> handler) {
    errorHandler = handler;
}

std::unordered_map<CodeFragmentType, std::vector<CodeFragment>> 
CompilerDispatcher::groupFragmentsByType(const std::vector<CodeFragment>& fragments) {
    std::unordered_map<CodeFragmentType, std::vector<CodeFragment>> grouped;
    
    for (const auto& fragment : fragments) {
        grouped[fragment.type].push_back(fragment);
    }
    
    return grouped;
}

CompileResult CompilerDispatcher::mergeResults(const std::vector<CompileResult>& results) {
    CompileResult merged;
    
    for (const auto& result : results) {
        if (!result.success) {
            merged.success = false;
        }
        
        // 合并HTML
        if (!result.html.empty()) {
            merged.html += result.html;
        }
        
        // 合并CSS
        if (!result.css.empty()) {
            if (!merged.css.empty()) {
                merged.css += "\n";
            }
            merged.css += result.css;
        }
        
        // 合并JavaScript
        if (!result.javascript.empty()) {
            if (!merged.javascript.empty()) {
                merged.javascript += "\n";
            }
            merged.javascript += result.javascript;
        }
        
        // 合并错误和警告
        merged.errors.insert(merged.errors.end(), 
                           result.errors.begin(), 
                           result.errors.end());
        merged.warnings.insert(merged.warnings.end(), 
                             result.warnings.begin(), 
                             result.warnings.end());
    }
    
    return merged;
}

std::vector<CompileResult> CompilerDispatcher::compileParallel(
    const std::unordered_map<CodeFragmentType, std::vector<CodeFragment>>& groupedFragments) {
    
    std::vector<std::future<CompileResult>> futures;
    
    for (const auto& [type, fragments] : groupedFragments) {
        auto compiler = getCompilerForType(type);
        if (compiler) {
            futures.push_back(std::async(std::launch::async, 
                [compiler, fragments]() {
                    return compiler->compile(fragments);
                }
            ));
        }
    }
    
    std::vector<CompileResult> results;
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    
    return results;
}

std::vector<CompileResult> CompilerDispatcher::compileSerial(
    const std::unordered_map<CodeFragmentType, std::vector<CodeFragment>>& groupedFragments) {
    
    std::vector<CompileResult> results;
    
    for (const auto& [type, fragments] : groupedFragments) {
        auto compiler = getCompilerForType(type);
        if (compiler) {
            try {
                auto result = compiler->compile(fragments);
                results.push_back(result);
            } catch (const std::exception& e) {
                CompileResult errorResult;
                errorResult.success = false;
                errorResult.errors.push_back(
                    "Compiler error in " + compiler->getName() + ": " + e.what()
                );
                results.push_back(errorResult);
                
                if (errorHandler) {
                    errorHandler(errorResult.errors.back());
                }
            }
        } else {
            CompileResult warningResult;
            warningResult.warnings.push_back(
                "No compiler found for fragment type: " + 
                std::to_string(static_cast<int>(type))
            );
            results.push_back(warningResult);
        }
    }
    
    return results;
}

std::string CompilerDispatcher::getCacheKey(const std::vector<CodeFragment>& fragments) const {
    std::stringstream ss;
    
    for (const auto& fragment : fragments) {
        ss << static_cast<int>(fragment.type) << ":"
           << fragment.content.length() << ":"
           << std::hash<std::string>{}(fragment.content) << ";";
    }
    
    return ss.str();
}

bool CompilerDispatcher::getCachedResult(const std::string& key, CompileResult& result) const {
    auto it = cache.find(key);
    if (it != cache.end()) {
        result = it->second;
        return true;
    }
    return false;
}

void CompilerDispatcher::cacheResult(const std::string& key, const CompileResult& result) {
    cache[key] = result;
}

// 编译器包装器实现（临时占位，后续会实现具体编译器）

CHTLCompilerWrapper::CHTLCompilerWrapper() {
    // 这里会创建实际的CHTL编译器
}

CHTLCompilerWrapper::~CHTLCompilerWrapper() = default;

CompileResult CHTLCompilerWrapper::compile(const std::vector<CodeFragment>& fragments) {
    CompileResult result;
    
    // 临时实现：直接将CHTL片段作为HTML返回
    for (const auto& fragment : fragments) {
        result.html += fragment.content + "\n";
    }
    
    return result;
}

bool CHTLCompilerWrapper::canHandle(CodeFragmentType type) const {
    return type == CodeFragmentType::CHTL || type == CodeFragmentType::HTML;
}

CHTLJSCompilerWrapper::CHTLJSCompilerWrapper() {
    // 这里会创建实际的CHTL JS编译器
}

CHTLJSCompilerWrapper::~CHTLJSCompilerWrapper() = default;

CompileResult CHTLJSCompilerWrapper::compile(const std::vector<CodeFragment>& fragments) {
    CompileResult result;
    
    // 临时实现：直接将CHTL JS片段作为JavaScript返回
    for (const auto& fragment : fragments) {
        result.javascript += fragment.content + "\n";
    }
    
    return result;
}

bool CHTLJSCompilerWrapper::canHandle(CodeFragmentType type) const {
    return type == CodeFragmentType::CHTL_JS;
}

CSSCompilerWrapper::CSSCompilerWrapper() : cssParser(nullptr) {
    // 这里会初始化ANTLR CSS解析器
}

CSSCompilerWrapper::~CSSCompilerWrapper() {
    // 清理ANTLR资源
}

CompileResult CSSCompilerWrapper::compile(const std::vector<CodeFragment>& fragments) {
    CompileResult result;
    
    // 临时实现：直接将CSS片段返回
    for (const auto& fragment : fragments) {
        result.css += fragment.content + "\n";
    }
    
    return result;
}

bool CSSCompilerWrapper::canHandle(CodeFragmentType type) const {
    return type == CodeFragmentType::CSS;
}

JSCompilerWrapper::JSCompilerWrapper() : jsParser(nullptr) {
    // 这里会初始化ANTLR JavaScript解析器
}

JSCompilerWrapper::~JSCompilerWrapper() {
    // 清理ANTLR资源
}

CompileResult JSCompilerWrapper::compile(const std::vector<CodeFragment>& fragments) {
    CompileResult result;
    
    // 临时实现：直接将JS片段返回
    for (const auto& fragment : fragments) {
        result.javascript += fragment.content + "\n";
    }
    
    return result;
}

bool JSCompilerWrapper::canHandle(CodeFragmentType type) const {
    return type == CodeFragmentType::JS;
}

} // namespace chtl