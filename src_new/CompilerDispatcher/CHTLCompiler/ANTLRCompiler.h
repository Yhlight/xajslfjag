#pragma once
#include "../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// 前向声明 ANTLR4 类型（避免直接依赖ANTLR4头文件）
namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
    namespace tree {
        class ParseTree;
        template<class T> class ParseTreeWalker;
    }
}

namespace CHTL {

/**
 * CSS语法树节点
 */
struct CSSASTNode {
    std::string type;                           // 节点类型
    std::string value;                          // 节点值
    std::vector<std::shared_ptr<CSSASTNode>> children; // 子节点
    std::unordered_map<std::string, std::string> attributes; // 属性
    
    CSSASTNode(const std::string& nodeType = "") : type(nodeType) {}
};

/**
 * JavaScript语法树节点
 */
struct JSASTNode {
    std::string type;                           // 节点类型
    std::string value;                          // 节点值
    std::vector<std::shared_ptr<JSASTNode>> children; // 子节点
    std::unordered_map<std::string, std::string> attributes; // 属性
    
    JSASTNode(const std::string& nodeType = "") : type(nodeType) {}
};

/**
 * ANTLR4 CSS编译器
 */
class ANTLRCSSCompiler {
public:
    ANTLRCSSCompiler();
    ~ANTLRCSSCompiler();
    
    /**
     * 解析CSS代码
     */
    std::shared_ptr<CSSASTNode> parseCSS(const std::string& cssCode);
    
    /**
     * 生成CSS代码
     */
    std::string generateCSS(std::shared_ptr<CSSASTNode> astRoot);
    
    /**
     * 验证CSS语法
     */
    bool validateCSS(const std::string& cssCode, std::vector<std::string>& errors);
    
    /**
     * 优化CSS代码
     */
    std::string optimizeCSS(const std::string& cssCode);
    
    /**
     * 提取CSS选择器
     */
    std::vector<std::string> extractSelectors(const std::string& cssCode);
    
    /**
     * 提取CSS属性
     */
    std::unordered_map<std::string, std::string> extractProperties(const std::string& ruleContent);
    
    /**
     * 格式化CSS代码
     */
    std::string formatCSS(const std::string& cssCode);

private:
    class Impl; // PIMPL模式隐藏ANTLR4细节
    std::unique_ptr<Impl> m_impl;
    
    /**
     * 构建AST节点
     */
    std::shared_ptr<CSSASTNode> buildASTFromParseTree(void* parseTree);
    
    /**
     * 遍历AST生成代码
     */
    void generateFromAST(std::shared_ptr<CSSASTNode> node, std::ostringstream& output);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
};

/**
 * ANTLR4 JavaScript编译器
 */
class ANTLRJavaScriptCompiler {
public:
    ANTLRJavaScriptCompiler();
    ~ANTLRJavaScriptCompiler();
    
    /**
     * 解析JavaScript代码
     */
    std::shared_ptr<JSASTNode> parseJavaScript(const std::string& jsCode);
    
    /**
     * 生成JavaScript代码
     */
    std::string generateJavaScript(std::shared_ptr<JSASTNode> astRoot);
    
    /**
     * 验证JavaScript语法
     */
    bool validateJavaScript(const std::string& jsCode, std::vector<std::string>& errors);
    
    /**
     * 优化JavaScript代码
     */
    std::string optimizeJavaScript(const std::string& jsCode);
    
    /**
     * 提取函数声明
     */
    std::vector<std::string> extractFunctions(const std::string& jsCode);
    
    /**
     * 提取变量声明
     */
    std::vector<std::string> extractVariables(const std::string& jsCode);
    
    /**
     * 格式化JavaScript代码
     */
    std::string formatJavaScript(const std::string& jsCode);
    
    /**
     * 转换ES6到ES5
     */
    std::string transpileToES5(const std::string& es6Code);

private:
    class Impl; // PIMPL模式隐藏ANTLR4细节
    std::unique_ptr<Impl> m_impl;
    
    /**
     * 构建AST节点
     */
    std::shared_ptr<JSASTNode> buildASTFromParseTree(void* parseTree);
    
    /**
     * 遍历AST生成代码
     */
    void generateFromAST(std::shared_ptr<JSASTNode> node, std::ostringstream& output);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
};

/**
 * ANTLR4错误监听器
 */
class ANTLRErrorListener {
public:
    /**
     * 语法错误处理
     */
    void syntaxError(const std::string& recognizerName,
                    size_t line, size_t charPositionInLine,
                    const std::string& message);
    
    /**
     * 获取错误列表
     */
    const std::vector<std::string>& getErrors() const { return m_errors; }
    
    /**
     * 清理错误
     */
    void clearErrors() { m_errors.clear(); }
    
    /**
     * 是否有错误
     */
    bool hasErrors() const { return !m_errors.empty(); }

private:
    std::vector<std::string> m_errors;
};

/**
 * ANTLR4集成管理器
 */
class ANTLRIntegrationManager {
public:
    ANTLRIntegrationManager();
    ~ANTLRIntegrationManager() = default;
    
    /**
     * 初始化ANTLR4系统
     */
    bool initialize();
    
    /**
     * 获取CSS编译器
     */
    ANTLRCSSCompiler* getCSSCompiler() { return m_cssCompiler.get(); }
    
    /**
     * 获取JavaScript编译器
     */
    ANTLRJavaScriptCompiler* getJavaScriptCompiler() { return m_jsCompiler.get(); }
    
    /**
     * 编译CSS片段
     */
    std::string compileCSS(const std::string& cssCode);
    
    /**
     * 编译JavaScript片段
     */
    std::string compileJavaScript(const std::string& jsCode);
    
    /**
     * 验证语法
     */
    bool validateSyntax(const std::string& code, const std::string& type, 
                       std::vector<std::string>& errors);
    
    /**
     * 获取编译统计
     */
    std::string getCompilationStats() const;
    
    /**
     * 清理状态
     */
    void cleanup();
    
    /**
     * 设置调试模式
     */
    void setDebugMode(bool enabled);

private:
    std::unique_ptr<ANTLRCSSCompiler> m_cssCompiler;
    std::unique_ptr<ANTLRJavaScriptCompiler> m_jsCompiler;
    std::unique_ptr<ANTLRErrorListener> m_errorListener;
    
    bool m_initialized;
    bool m_debugMode;
    std::unordered_map<std::string, int> m_stats;
    
    /**
     * 更新统计信息
     */
    void updateStats(const std::string& category);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
};

} // namespace CHTL