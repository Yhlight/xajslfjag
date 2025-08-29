#pragma once
#include "../CHTLScanner/UnifiedScanner.h"
#include "../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

// 前置声明
class CHTLParser;
class UnifiedGenerator;

/**
 * 编译结果
 */
struct CompilationResult {
    bool success;                   // 编译是否成功
    std::string output;             // 编译输出
    std::string errorMessage;       // 错误信息
    FragmentType sourceType;        // 源片段类型
    
    CompilationResult(bool s = false, const std::string& o = "", 
                     const std::string& e = "", FragmentType t = FragmentType::UNKNOWN)
        : success(s), output(o), errorMessage(e), sourceType(t) {}
};

/**
 * 编译器接口
 */
class ICompiler {
public:
    virtual ~ICompiler() = default;
    
    /**
     * 编译代码片段
     */
    virtual CompilationResult compile(const CodeFragment& fragment) = 0;
    
    /**
     * 获取支持的片段类型
     */
    virtual FragmentType getSupportedType() const = 0;
    
    /**
     * 获取编译器名称
     */
    virtual std::string getName() const = 0;
};

/**
 * CHTL编译器
 */
class CHTLCompiler : public ICompiler {
public:
    CHTLCompiler();
    ~CHTLCompiler() override = default;
    
    CompilationResult compile(const CodeFragment& fragment) override;
    FragmentType getSupportedType() const override { return FragmentType::CHTL; }
    std::string getName() const override { return "CHTL Compiler"; }
    
private:
    std::unique_ptr<CHTLParser> m_parser;
    std::unique_ptr<UnifiedGenerator> m_generator;
};

/**
 * CHTL JS编译器
 */
class CHTLJSCompiler : public ICompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler() override = default;
    
    CompilationResult compile(const CodeFragment& fragment) override;
    FragmentType getSupportedType() const override { return FragmentType::CHTL_JS; }
    std::string getName() const override { return "CHTL JS Enhanced Compiler"; }
    
private:
    std::unique_ptr<class CHTLJSEnhancedCompiler> m_enhancedCompiler;
};

/**
 * CSS编译器（ANTLR4实现）
 */
class CSSCompiler : public ICompiler {
public:
    CSSCompiler();
    ~CSSCompiler() override = default;
    
    CompilationResult compile(const CodeFragment& fragment) override;
    FragmentType getSupportedType() const override { return FragmentType::CSS; }
    std::string getName() const override { return "CSS Compiler (ANTLR4)"; }
    
private:
    // ANTLR4 CSS编译器实现
};

/**
 * JavaScript编译器（ANTLR4实现）
 */
class JavaScriptCompiler : public ICompiler {
public:
    JavaScriptCompiler();
    ~JavaScriptCompiler() override = default;
    
    CompilationResult compile(const CodeFragment& fragment) override;
    FragmentType getSupportedType() const override { return FragmentType::JAVASCRIPT; }
    std::string getName() const override { return "JavaScript Compiler (ANTLR4)"; }
    
private:
    // ANTLR4 JavaScript编译器实现
};

/**
 * 编译器调度器
 * 负责将代码片段分发到对应的编译器，并合并编译结果
 */
class CompilerDispatcher {
public:
    /**
     * 构造函数
     */
    CompilerDispatcher();
    
    /**
     * 析构函数
     */
    ~CompilerDispatcher() = default;
    
    // ========== 编译器管理 ==========
    
    /**
     * 注册编译器
     */
    void registerCompiler(std::unique_ptr<ICompiler> compiler);
    
    /**
     * 获取支持指定类型的编译器
     */
    ICompiler* getCompiler(FragmentType type);
    
    /**
     * 获取所有编译器
     */
    std::vector<ICompiler*> getAllCompilers() const;
    
    // ========== 编译调度 ==========
    
    /**
     * 编译单个代码片段
     */
    CompilationResult compileFragment(const CodeFragment& fragment);
    
    /**
     * 编译多个代码片段
     */
    std::vector<CompilationResult> compileFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 编译完整源代码（包含扫描）
     */
    CompilationResult compileSource(const std::string& source);
    
    // ========== 结果合并 ==========
    
    /**
     * 合并编译结果为HTML输出
     */
    std::string mergeToHTML(const std::vector<CompilationResult>& results);
    
    /**
     * 合并CSS结果
     */
    std::string mergeCSS(const std::vector<CompilationResult>& results);
    
    /**
     * 合并JavaScript结果
     */
    std::string mergeJavaScript(const std::vector<CompilationResult>& results);
    
    // ========== 配置与优化 ==========
    
    /**
     * 设置统一扫描器
     */
    void setScanner(std::shared_ptr<CHTLUnifiedScanner> scanner);
    
    /**
     * 启用并行编译
     */
    void enableParallelCompilation(bool enabled = true);
    
    /**
     * 设置编译选项
     */
    void setCompilationOptions(const std::unordered_map<std::string, std::string>& options);
    
    // ========== 错误处理与报告 ==========
    
    /**
     * 获取编译错误
     */
    std::vector<std::string> getCompilationErrors() const;
    
    /**
     * 生成编译报告
     */
    std::string generateCompilationReport(const std::vector<CompilationResult>& results);
    
    /**
     * 清除错误记录
     */
    void clearErrors();
    
    // ========== 工具方法 ==========
    
    /**
     * 验证片段兼容性
     */
    bool validateFragmentCompatibility(const CodeFragment& fragment);
    
    /**
     * 优化编译顺序
     */
    std::vector<CodeFragment> optimizeCompilationOrder(const std::vector<CodeFragment>& fragments);

private:
    std::vector<std::unique_ptr<ICompiler>> m_compilers;       // 编译器列表
    std::shared_ptr<CHTLUnifiedScanner> m_scanner;            // 统一扫描器
    std::vector<std::string> m_errors;                        // 编译错误列表
    bool m_parallelCompilation;                               // 是否启用并行编译
    std::unordered_map<std::string, std::string> m_options;   // 编译选项
    
    /**
     * 初始化默认编译器
     */
    void initializeDefaultCompilers();
    
    /**
     * 处理编译错误
     */
    void handleCompilationError(const std::string& error, const CodeFragment& fragment);
    
    /**
     * 生成HTML模板
     */
    std::string generateHTMLTemplate();
    
    /**
     * 插入CSS到HTML
     */
    std::string insertCSSIntoHTML(const std::string& html, const std::string& css);
    
    /**
     * 插入JavaScript到HTML
     */
    std::string insertJavaScriptIntoHTML(const std::string& html, const std::string& js);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
};

} // namespace CHTL