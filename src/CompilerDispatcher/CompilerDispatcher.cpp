#include "CompilerDispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../Scanner/FragmentCollector.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTLJS/CHTLJSParser/Parser.h"
#include "../CHTLJS/CHTLJSGenerator/Generator.h"
#include "../CHTL/CHTLIOStream/CHTLFileSystem.h"
#include "../Error/ErrorReport.h"
#include <chrono>
#include <sstream>
#include <algorithm>

namespace CHTL {

// 内部实现类
class CHTLCompilerImpl : public CHTLCompiler {
public:
    CHTLCompilerImpl() {
        parser_ = std::make_shared<CHTLParser>();
        generator_ = std::make_shared<CHTLGenerator>();
    }
    
    CompileResult compile(const std::string& code, const CompileOptions& options) override {
        CompileResult result;
        
        try {
            // 词法分析
            CHTLLexer lexer;
            auto tokens = lexer.tokenize(code);
            
            // 语法分析
            auto ast = parser_->parse(tokens);
            
            // 代码生成
            generator_->setOptions(options);
            auto output = generator_->generate(ast);
            
            result.success = true;
            result.htmlOutput = output.html;
            result.cssOutput = output.css;
            result.jsOutput = output.javascript;
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errors.push_back(e.what());
        }
        
        return result;
    }
    
    bool validate(const std::string& code) override {
        try {
            CHTLLexer lexer;
            auto tokens = lexer.tokenize(code);
            parser_->parse(tokens);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    std::string getName() const override { return "CHTL Compiler"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void setNamespaceManager(std::shared_ptr<void> manager) override {
        namespaceManager_ = manager;
    }
    
    void setImportResolver(std::shared_ptr<void> resolver) override {
        importResolver_ = resolver;
    }
    
    void setSelectorAutomation(std::shared_ptr<void> automation) override {
        selectorAutomation_ = automation;
    }
    
private:
    std::shared_ptr<CHTLParser> parser_;
    std::shared_ptr<CHTLGenerator> generator_;
    std::shared_ptr<void> namespaceManager_;
    std::shared_ptr<void> importResolver_;
    std::shared_ptr<void> selectorAutomation_;
};

// CHTL JS编译器实现
class CHTLJSCompilerImpl : public CHTLJSCompiler {
public:
    CHTLJSCompilerImpl() {
        parser_ = std::make_shared<CHTLJSParser>();
        generator_ = std::make_shared<CHTLJSGenerator>();
    }
    
    CompileResult compile(const std::string& code, const CompileOptions& options) override {
        CompileResult result;
        
        try {
            // 词法分析
            CHTLJSLexer lexer;
            auto tokens = lexer.tokenize(code);
            
            // 语法分析
            auto ast = parser_->parse(tokens);
            
            // 代码生成
            generator_->setOptions(options);
            result.jsOutput = generator_->generate(ast);
            result.success = true;
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errors.push_back(e.what());
        }
        
        return result;
    }
    
    bool validate(const std::string& code) override {
        try {
            CHTLJSLexer lexer;
            auto tokens = lexer.tokenize(code);
            parser_->parse(tokens);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    std::string getName() const override { return "CHTL JS Compiler"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void setCJMODLoader(std::shared_ptr<void> loader) override {
        cjmodLoader_ = loader;
    }
    
    void setVirtualObjectManager(std::shared_ptr<void> manager) override {
        virtualObjectManager_ = manager;
    }
    
private:
    std::shared_ptr<CHTLJSParser> parser_;
    std::shared_ptr<CHTLJSGenerator> generator_;
    std::shared_ptr<void> cjmodLoader_;
    std::shared_ptr<void> virtualObjectManager_;
};

// 简化的CSS编译器实现（实际应使用ANTLR）
class CSSCompilerImpl : public CSSCompiler {
public:
    CompileResult compile(const std::string& code, const CompileOptions& options) override {
        CompileResult result;
        result.cssOutput = code; // 暂时直接输出
        result.success = true;
        
        if (options.minify) {
            // 简单的压缩：移除多余空白
            result.cssOutput.erase(
                std::remove_if(result.cssOutput.begin(), result.cssOutput.end(),
                              [](char c) { return c == '\n' || c == '\r'; }),
                result.cssOutput.end()
            );
        }
        
        return result;
    }
    
    bool validate(const std::string& code) override {
        // 简单的CSS验证
        return code.find('{') != std::string::npos && 
               code.find('}') != std::string::npos;
    }
    
    std::string getName() const override { return "CSS Compiler"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void setPreprocessor(const std::string& preprocessor) override {
        preprocessor_ = preprocessor;
    }
    
    void setAutoprefixer(bool enable) override {
        autoprefixer_ = enable;
    }
    
private:
    std::string preprocessor_;
    bool autoprefixer_ = false;
};

// 简化的JavaScript编译器实现（实际应使用ANTLR）
class JavaScriptCompilerImpl : public JavaScriptCompiler {
public:
    CompileResult compile(const std::string& code, const CompileOptions& options) override {
        CompileResult result;
        result.jsOutput = code; // 暂时直接输出
        result.success = true;
        
        if (options.minify) {
            // 简单的压缩：移除注释和多余空白
            // 实际实现需要更复杂的处理
            std::string minified;
            bool inString = false;
            bool inComment = false;
            
            for (size_t i = 0; i < code.length(); ++i) {
                if (!inString && i + 1 < code.length() && 
                    code[i] == '/' && code[i + 1] == '/') {
                    // 单行注释
                    while (i < code.length() && code[i] != '\n') ++i;
                    continue;
                }
                
                if (!inString && i + 1 < code.length() && 
                    code[i] == '/' && code[i + 1] == '*') {
                    // 多行注释
                    i += 2;
                    while (i + 1 < code.length() && 
                           !(code[i] == '*' && code[i + 1] == '/')) ++i;
                    i += 1;
                    continue;
                }
                
                if (code[i] == '"' || code[i] == '\'') {
                    inString = !inString;
                }
                
                minified += code[i];
            }
            
            result.jsOutput = minified;
        }
        
        return result;
    }
    
    bool validate(const std::string& code) override {
        // 简单的JS验证
        return true;
    }
    
    std::string getName() const override { return "JavaScript Compiler"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void setTargetVersion(const std::string& version) override {
        targetVersion_ = version;
    }
    
    void setModuleSystem(const std::string& system) override {
        moduleSystem_ = system;
    }
    
private:
    std::string targetVersion_ = "ES6";
    std::string moduleSystem_ = "ESM";
};

// CompilerDispatcher 实现

CompilerDispatcher::CompilerDispatcher() {
    scanner_ = std::make_unique<CHTLUnifiedScanner>();
}

CompilerDispatcher::~CompilerDispatcher() = default;

bool CompilerDispatcher::initialize() {
    // 创建并注册编译器
    registerCompiler(CompilerType::CHTL, CompilerFactory::createCHTLCompiler());
    registerCompiler(CompilerType::CHTLJS, CompilerFactory::createCHTLJSCompiler());
    registerCompiler(CompilerType::CSS, CompilerFactory::createCSSCompiler());
    registerCompiler(CompilerType::JAVASCRIPT, CompilerFactory::createJavaScriptCompiler());
    
    // 设置默认片段路由规则
    setFragmentRoute("chtl", {CompilerType::CHTL, "", 1});
    setFragmentRoute("chtljs", {CompilerType::CHTLJS, "", 1});
    setFragmentRoute("css", {CompilerType::CSS, "", 1});
    setFragmentRoute("javascript", {CompilerType::JAVASCRIPT, "", 1});
    
    return true;
}

CompileResult CompilerDispatcher::compile(const std::string& inputFile) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // 读取文件内容
    auto contentOpt = File::readToString(inputFile);
    if (!contentOpt) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("Failed to read file: " + inputFile);
        return result;
    }
    
    CompileResult result = doCompile(*contentOpt, inputFile);
    
    auto end = std::chrono::high_resolution_clock::now();
    result.compilationTime = std::chrono::duration<double>(end - start).count();
    
    return result;
}

CompileResult CompilerDispatcher::compileString(const std::string& content, const std::string& filename) {
    auto start = std::chrono::high_resolution_clock::now();
    
    CompileResult result = doCompile(content, filename);
    
    auto end = std::chrono::high_resolution_clock::now();
    result.compilationTime = std::chrono::duration<double>(end - start).count();
    
    return result;
}

std::vector<CompileResult> CompilerDispatcher::compileBatch(const std::vector<std::string>& files) {
    std::vector<CompileResult> results;
    size_t total = files.size();
    size_t current = 0;
    
    for (const auto& file : files) {
        if (progressCallback_) {
            progressCallback_(current++, total);
        }
        
        results.push_back(compile(file));
    }
    
    if (progressCallback_) {
        progressCallback_(total, total);
    }
    
    return results;
}

void CompilerDispatcher::registerCompiler(CompilerType type, std::shared_ptr<void> compiler) {
    compilers_[type] = compiler;
}

void CompilerDispatcher::setFragmentRoute(const std::string& pattern, const FragmentRoute& route) {
    fragmentRoutes_[pattern] = route;
}

CompileResult CompilerDispatcher::doCompile(const std::string& content, const std::string& filename) {
    CompileResult result;
    
    try {
        // 使用统一扫描器分析代码
        auto fragments = scanner_->scan(content);
        
        // 使用FragmentCollector收集同类型片段
        FragmentCollector collector;
        collector.processFragments(fragments);
        
        // 记录处理的片段数
        result.processedFragments = fragments.size();
        
        // 处理CHTL片段（如果有）
        if (collector.hasContent(FragmentType::CHTL)) {
            auto chtlResult = compileCHTL(collector.getCompleteCode(FragmentType::CHTL));
            mergeResults(result, chtlResult);
        }
        
        // 处理CHTL JS片段（如果有）
        if (collector.hasContent(FragmentType::CHTLJS)) {
            auto cjtlJsResult = compileCHTLJS(collector.getCompleteCode(FragmentType::CHTLJS));
            mergeResults(result, cjtlJsResult);
        }
        
        // 处理CSS片段（如果有）- 传递完整的CSS代码给CSS编译器
        if (collector.hasContent(FragmentType::CSS)) {
            auto cssResult = compileCSS(collector.getCompleteCSS());
            mergeResults(result, cssResult);
        }
        
        // 处理JavaScript片段（如果有）- 传递完整的JS代码给JS编译器
        if (collector.hasContent(FragmentType::JavaScript)) {
            auto jsResult = compileJavaScript(collector.getCompleteJavaScript());
            mergeResults(result, jsResult);
        }
        
        // 生成最终输出
        generateOutput(result);
        
        result.success = true;
        result.includedFiles.push_back(filename);
        
    } catch (const std::exception& e) {
        reportError(e.what(), result);
        result.success = false;
    }
    
    return result;
}

void CompilerDispatcher::dispatchFragments(const std::vector<CodeFragment>& fragments, CompileResult& result) {
    result.processedFragments = fragments.size();
    size_t current = 0;
    
    for (const auto& fragment : fragments) {
        if (progressCallback_) {
            progressCallback_(current++, fragments.size());
        }
        
        CompilerType compilerType = determineCompiler(fragment);
        auto compiler = getCompiler<ICompiler>(compilerType);
        
        if (!compiler) {
            reportError("No compiler found for fragment type: " + fragment.type, result);
            continue;
        }
        
        CompileOptions fragmentOptions = options_;
        CompileResult fragmentResult = compiler->compile(fragment.code, fragmentOptions);
        
        mergeResults(result, fragmentResult);
    }
}

CompileResult CompilerDispatcher::compileCHTL(const std::string& code) {
    auto compiler = std::static_pointer_cast<CHTLCompiler>(compilers_[CompilerType::CHTL]);
    if (!compiler) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("CHTL compiler not initialized");
        return result;
    }
    return compiler->compile(code, options_);
}

CompileResult CompilerDispatcher::compileCHTLJS(const std::string& code) {
    auto compiler = std::static_pointer_cast<CHTLJSCompiler>(compilers_[CompilerType::CHTLJS]);
    if (!compiler) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("CHTL JS compiler not initialized");
        return result;
    }
    return compiler->compile(code, options_);
}

CompileResult CompilerDispatcher::compileCSS(const std::string& code) {
    auto compiler = std::static_pointer_cast<CSSCompiler>(compilers_[CompilerType::CSS]);
    if (!compiler) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("CSS compiler not initialized");
        return result;
    }
    
    // 记录CSS编译器接收到完整代码
    ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
        .withMessage("CSS Compiler processing complete CSS code")
        .withDetail("Code length: " + std::to_string(code.length()) + " characters")
        .report();
        
    return compiler->compile(code, options_);
}

CompileResult CompilerDispatcher::compileJavaScript(const std::string& code) {
    auto compiler = std::static_pointer_cast<JavaScriptCompiler>(compilers_[CompilerType::JAVASCRIPT]);
    if (!compiler) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("JavaScript compiler not initialized");
        return result;
    }
    
    // 记录JS编译器接收到完整代码
    ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
        .withMessage("JavaScript Compiler processing complete JS code")
        .withDetail("Code length: " + std::to_string(code.length()) + " characters")
        .report();
        
    return compiler->compile(code, options_);
}

CompilerType CompilerDispatcher::determineCompiler(const CodeFragment& fragment) {
    // 根据片段类型确定编译器
    auto it = fragmentRoutes_.find(fragment.type);
    if (it != fragmentRoutes_.end()) {
        return it->second.compiler;
    }
    
    // 默认路由规则
    if (fragment.type == "chtl") return CompilerType::CHTL;
    if (fragment.type == "chtljs" || fragment.type == "script") return CompilerType::CHTLJS;
    if (fragment.type == "css" || fragment.type == "style") return CompilerType::CSS;
    if (fragment.type == "javascript" || fragment.type == "js") return CompilerType::JAVASCRIPT;
    
    return CompilerType::CHTL; // 默认使用CHTL编译器
}

void CompilerDispatcher::mergeResults(CompileResult& mainResult, const CompileResult& fragmentResult) {
    // 合并HTML输出
    if (!fragmentResult.htmlOutput.empty()) {
        mainResult.htmlOutput += fragmentResult.htmlOutput + "\n";
    }
    
    // 合并CSS输出
    if (!fragmentResult.cssOutput.empty()) {
        mainResult.cssOutput += fragmentResult.cssOutput + "\n";
    }
    
    // 合并JS输出
    if (!fragmentResult.jsOutput.empty()) {
        mainResult.jsOutput += fragmentResult.jsOutput + "\n";
    }
    
    // 合并错误和警告
    mainResult.errors.insert(mainResult.errors.end(), 
                           fragmentResult.errors.begin(), 
                           fragmentResult.errors.end());
    
    mainResult.warnings.insert(mainResult.warnings.end(), 
                             fragmentResult.warnings.begin(), 
                             fragmentResult.warnings.end());
}

void CompilerDispatcher::generateOutput(CompileResult& result) {
    if (!options_.outputFile.empty()) {
        // 生成完整的HTML文件
        std::stringstream html;
        html << "<!DOCTYPE html>\n";
        html << "<html>\n";
        html << "<head>\n";
        html << "  <meta charset=\"UTF-8\">\n";
        
        if (!result.cssOutput.empty()) {
            html << "  <style>\n";
            html << result.cssOutput;
            html << "  </style>\n";
        }
        
        html << "</head>\n";
        html << "<body>\n";
        html << result.htmlOutput;
        
        if (!result.jsOutput.empty()) {
            html << "  <script>\n";
            html << result.jsOutput;
            html << "  </script>\n";
        }
        
        html << "</body>\n";
        html << "</html>\n";
        
        // 写入文件
        if (File::writeString(options_.outputFile, html.str())) {
            result.outputPath = options_.outputFile;
        } else {
            reportError("Failed to write output file: " + options_.outputFile, result);
        }
    }
}

void CompilerDispatcher::reportError(const std::string& error, CompileResult& result) {
    result.errors.push_back(error);
    if (errorHandler_) {
        errorHandler_(error);
    }
}

void CompilerDispatcher::reportWarning(const std::string& warning, CompileResult& result) {
    result.warnings.push_back(warning);
    if (warningHandler_) {
        warningHandler_(warning);
    }
}

// CompilerFactory 实现

std::shared_ptr<CompilerDispatcher> CompilerFactory::createDispatcher() {
    auto dispatcher = std::make_shared<CompilerDispatcher>();
    dispatcher->initialize();
    return dispatcher;
}

std::shared_ptr<CHTLCompiler> CompilerFactory::createCHTLCompiler() {
    return std::make_shared<CHTLCompilerImpl>();
}

std::shared_ptr<CHTLJSCompiler> CompilerFactory::createCHTLJSCompiler() {
    return std::make_shared<CHTLJSCompilerImpl>();
}

std::shared_ptr<CSSCompiler> CompilerFactory::createCSSCompiler() {
    return std::make_shared<CSSCompilerImpl>();
}

std::shared_ptr<JavaScriptCompiler> CompilerFactory::createJavaScriptCompiler() {
    return std::make_shared<JavaScriptCompilerImpl>();
}

// CompilerManager 实现

CompilerManager::CompilerManager() {
    defaultDispatcher_ = CompilerFactory::createDispatcher();
}

std::shared_ptr<CompilerDispatcher> CompilerManager::getDefaultDispatcher() {
    return defaultDispatcher_;
}

std::shared_ptr<CompilerDispatcher> CompilerManager::createDispatcher() {
    return CompilerFactory::createDispatcher();
}

void CompilerManager::registerDispatcher(const std::string& name, std::shared_ptr<CompilerDispatcher> dispatcher) {
    dispatchers_[name] = dispatcher;
}

std::shared_ptr<CompilerDispatcher> CompilerManager::getDispatcher(const std::string& name) {
    auto it = dispatchers_.find(name);
    if (it != dispatchers_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace CHTL