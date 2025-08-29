#pragma once

#include "../Util/Common.h"
#include "../Scanner/UnifiedScanner.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTLJS/CHTLJSParser/EnhancedParser.h"
// TODO: CSS和JS解析器将在后续阶段实现
// #include "../CSS/CSSParser/Parser.h"
// #include "../JS/JSParser/Parser.h"

namespace CHTL {

// 编译结果
struct CompilationResult {
    bool success = false;
    String htmlOutput;
    String cssOutput; 
    String jsOutput;
    StringVector errors;
    StringVector warnings;
    StringUnorderedMap metadata;
    
    void clear() {
        success = false;
        htmlOutput.clear();
        cssOutput.clear();
        jsOutput.clear();
        errors.clear();
        warnings.clear();
        metadata.clear();
    }
    
    void addError(const String& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const String& warning) {
        warnings.push_back(warning);
    }
};

// 编译器调度配置
struct DispatcherConfig {
    bool enableCHTLCompiler = true;
    bool enableCHTLJSCompiler = true;
    bool enableCSSCompiler = true;
    bool enableJSCompiler = true;
    bool enableParallelCompilation = false;
    bool enableErrorRecovery = true;
    bool enableOptimizations = true;
    int maxErrorCount = 50;
    String outputFormat = "html5";
};

// 编译器调度器
class CompilerDispatcher {
private:
    DispatcherConfig config;
    std::unique_ptr<CHTLUnifiedScanner> scanner;
    
    // 编译器实例
    std::unique_ptr<CHTL::Parser> chtlParser;
    std::unique_ptr<CHTLJS::CHTLJSEnhancedParser> chtljsParser;
    // TODO: CSS和JS解析器将在后续阶段实现
    // std::unique_ptr<CSS::Parser> cssParser;
    // std::unique_ptr<JS::Parser> jsParser;
    
    // 内部状态
    StringVector currentErrors;
    StringVector currentWarnings;
    
    // 编译方法
    CompilationResult compileCHTLFragments(const std::vector<CodeFragment>& fragments);
    CompilationResult compileCHTLJSFragments(const std::vector<CodeFragment>& fragments);
    CompilationResult compileCSSFragments(const std::vector<CodeFragment>& fragments);
    CompilationResult compileJSFragments(const std::vector<CodeFragment>& fragments);
    
    // 结果合并
    CompilationResult mergeResults(const std::vector<CompilationResult>& results);
    String generateFinalHTML(const CompilationResult& result);
    
    // 错误处理
    void handleCompilationError(const String& error, FragmentType type);
    bool shouldContinueAfterError() const;
    
    // 优化
    void optimizeOutput(CompilationResult& result);
    void minifyCSS(String& css);
    void minifyJS(String& js);
    
public:
    // 构造函数
    explicit CompilerDispatcher(const DispatcherConfig& cfg = DispatcherConfig());
    ~CompilerDispatcher() = default;
    
    // 禁用拷贝构造
    CompilerDispatcher(const CompilerDispatcher&) = delete;
    CompilerDispatcher& operator=(const CompilerDispatcher&) = delete;
    
    // 移动构造
    CompilerDispatcher(CompilerDispatcher&&) = default;
    CompilerDispatcher& operator=(CompilerDispatcher&&) = default;
    
    // 主要编译接口
    CompilationResult compile(const String& sourceCode);
    CompilationResult compileFromFile(const String& filePath);
    CompilationResult compileFragments(const std::vector<CodeFragment>& fragments);
    
    // 分步编译
    std::vector<CodeFragment> scanSource(const String& sourceCode);
    CompilationResult dispatchFragments(const std::vector<CodeFragment>& fragments);
    
    // 配置管理
    void setConfig(const DispatcherConfig& cfg);
    DispatcherConfig getConfig() const;
    
    // 编译器管理
    void initializeCompilers();
    void resetCompilers();
    
    // 状态查询
    StringVector getErrors() const { return currentErrors; }
    StringVector getWarnings() const { return currentWarnings; }
    bool hasErrors() const { return !currentErrors.empty(); }
    bool hasWarnings() const { return !currentWarnings.empty(); }
    
    // 工具方法
    void printCompilationStats(const CompilationResult& result) const;
    String getCompilerVersion() const;
    
private:
    // 初始化方法
    void setupCHTLParser();
    void setupCHTLJSParser();
    void setupCSSParser();
    void setupJSParser();
};

// 编译器工厂
class CompilerFactory {
public:
    static std::unique_ptr<CHTL::Parser> createCHTLParser(const CHTL::ParserConfig& config);
    static std::unique_ptr<CHTLJS::CHTLJSEnhancedParser> createCHTLJSParser(const CHTLJS::CHTLJSParserConfig& config);
    // TODO: CSS和JS解析器将在后续阶段实现
    // static std::unique_ptr<CSS::Parser> createCSSParser(const CSS::ParserConfig& config);
    // static std::unique_ptr<JS::Parser> createJSParser(const JS::ParserConfig& config);
};

// 辅助函数
namespace Dispatcher {
    // 片段分类
    std::vector<CodeFragment> filterFragmentsByType(const std::vector<CodeFragment>& fragments, FragmentType type);
    
    // 依赖分析
    StringVector analyzeDependencies(const std::vector<CodeFragment>& fragments);
    bool hasCircularDependency(const StringVector& dependencies);
    
    // 输出优化
    String optimizeHTML(const String& html);
    String combineCSS(const StringVector& cssFragments);
    String combineJS(const StringVector& jsFragments);
    
    // 错误格式化
    String formatError(const String& error, FragmentType type, size_t position);
    String formatWarning(const String& warning, FragmentType type, size_t position);
}

} // namespace CHTL