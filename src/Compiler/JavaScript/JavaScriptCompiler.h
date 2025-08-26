#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// ANTLR4生成的头文件
#include "JavaScriptLexer.h"
#include "JavaScriptParser.h" 
#include "JavaScriptParserBaseListener.h"

namespace CHTL {
namespace Compiler {

/**
 * JavaScriptCompiler - JavaScript编译器
 * 利用现有的ANTLR4基础设施和语法文件
 */
class JavaScriptCompiler {
public:
    /**
     * JavaScript编译结果
     */
    struct CompileResult {
        bool success;
        std::string compiled_js;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        CompileResult() : success(false) {}
    };
    
    /**
     * JavaScript代码片段信息
     */
    struct JSFragment {
        std::string content;
        std::string source_file;
        int line_number;
        bool is_global;                 // 是否为全局脚本
        std::string parent_element;     // 父元素（局部脚本时）
        bool contains_chtljs;           // 是否包含CHTL JS语法
        
        JSFragment() : line_number(0), is_global(true), contains_chtljs(false) {}
    };

public:
    /**
     * 编译JavaScript代码片段
     * @param js_fragments JavaScript片段列表
     * @return 编译结果
     */
    static CompileResult compile(const std::vector<JSFragment>& js_fragments);
    
    /**
     * 编译单个JavaScript片段
     * @param fragment JavaScript片段
     * @return 编译结果
     */
    static CompileResult compileSingle(const JSFragment& fragment);
    
    /**
     * 验证JavaScript语法
     * @param js_content JavaScript内容
     * @return 是否有效
     */
    static bool validateSyntax(const std::string& js_content);
    
    /**
     * 格式化JavaScript代码
     * @param js_content JavaScript内容
     * @param minify 是否压缩
     * @return 格式化后的JavaScript
     */
    static std::string formatJS(const std::string& js_content, bool minify = false);
    
    /**
     * 合并JavaScript片段
     * @param fragments JavaScript片段列表
     * @return 合并后的JavaScript
     */
    static std::string mergeFragments(const std::vector<JSFragment>& fragments);
    
    /**
     * 检测CHTL JS语法
     * @param js_content JavaScript内容
     * @return 是否包含CHTL JS语法
     */
    static bool detectCHTLJSSyntax(const std::string& js_content);
    
    /**
     * 分离纯JavaScript和CHTL JS代码
     * @param mixed_content 混合内容
     * @return [纯JavaScript, CHTL JS部分]
     */
    static std::pair<std::string, std::string> separateJSAndCHTLJS(const std::string& mixed_content);

private:
    /**
     * 初始化ANTLR4解析器
     * @param js_content JavaScript内容
     * @return [lexer, parser, tree]
     */
    static std::tuple<std::shared_ptr<JavaScriptLexer>, 
                     std::shared_ptr<JavaScriptParser>, 
                     JavaScriptParser::ProgramContext*> initializeParser(const std::string& js_content);
    
    /**
     * 收集解析错误
     * @param parser 解析器
     * @return 错误列表
     */
    static std::vector<std::string> collectParseErrors(JavaScriptParser* parser);
    
    /**
     * 生成JavaScript代码
     * @param tree 解析树
     * @return 生成的JavaScript代码
     */
    static std::string generateJS(JavaScriptParser::ProgramContext* tree);
};

/**
 * JavaScriptListener - JavaScript解析监听器
 * 继承自ANTLR4生成的BaseListener
 */
class JavaScriptListener : public JavaScriptParserBaseListener {
public:
    /**
     * 构造函数
     * @param parent_context 父上下文（用于局部脚本）
     */
    explicit JavaScriptListener(const std::string& parent_context = "");
    
    /**
     * 获取生成的JavaScript
     * @return JavaScript字符串
     */
    std::string getGeneratedJS() const;
    
    /**
     * 获取错误列表
     * @return 错误列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * 获取警告列表
     * @return 警告列表
     */
    const std::vector<std::string>& getWarnings() const;
    
    /**
     * 获取检测到的CHTL JS语法
     * @return CHTL JS代码片段列表
     */
    const std::vector<std::string>& getCHTLJSFragments() const;

    // ANTLR4监听器方法重写
    void enterProgram(JavaScriptParser::ProgramContext* ctx) override;
    void exitProgram(JavaScriptParser::ProgramContext* ctx) override;
    
    void enterStatement(JavaScriptParser::StatementContext* ctx) override;
    void exitStatement(JavaScriptParser::StatementContext* ctx) override;
    
    void enterExpression(JavaScriptParser::ExpressionContext* ctx) override;
    void exitExpression(JavaScriptParser::ExpressionContext* ctx) override;
    
    void enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext* ctx) override;
    void exitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext* ctx) override;

private:
    std::string parent_context_;
    std::string generated_js_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    std::vector<std::string> chtljs_fragments_;
    
    /**
     * 检查表达式是否为CHTL JS语法
     * @param expression 表达式文本
     * @return 是否为CHTL JS语法
     */
    bool isCHTLJSExpression(const std::string& expression);
    
    /**
     * 处理CHTL JS语法
     * @param chtljs_code CHTL JS代码
     * @return 处理后的JavaScript代码
     */
    std::string processCHTLJSSyntax(const std::string& chtljs_code);
};

/**
 * JavaScriptOptimizer - JavaScript优化器
 * 提供JavaScript压缩、优化功能
 */
class JavaScriptOptimizer {
public:
    /**
     * 优化选项
     */
    struct OptimizeOptions {
        bool minify = false;                    // 是否压缩
        bool remove_comments = false;           // 是否移除注释
        bool optimize_variables = false;        // 是否优化变量名
        bool remove_dead_code = false;          // 是否移除死代码
        bool inline_functions = false;          // 是否内联函数
        std::string target_version = "ES5";     // 目标版本
        
        OptimizeOptions() = default;
    };
    
    /**
     * 优化JavaScript
     * @param js_content JavaScript内容
     * @param options 优化选项
     * @return 优化后的JavaScript
     */
    static std::string optimize(const std::string& js_content, 
                               const OptimizeOptions& options = OptimizeOptions());
    
    /**
     * 压缩JavaScript
     * @param js_content JavaScript内容
     * @return 压缩后的JavaScript
     */
    static std::string minify(const std::string& js_content);
    
    /**
     * 美化JavaScript
     * @param js_content JavaScript内容
     * @param indent_size 缩进大小
     * @return 美化后的JavaScript
     */
    static std::string beautify(const std::string& js_content, int indent_size = 2);
    
    /**
     * 转换ES6+语法到ES5
     * @param modern_js 现代JavaScript代码
     * @return ES5兼容的JavaScript
     */
    static std::string transpileToES5(const std::string& modern_js);

private:
    /**
     * 移除注释
     * @param js_content JavaScript内容
     * @return 移除注释后的JavaScript
     */
    static std::string removeComments(const std::string& js_content);
    
    /**
     * 移除多余空白
     * @param js_content JavaScript内容
     * @return 移除空白后的JavaScript
     */
    static std::string removeWhitespace(const std::string& js_content);
    
    /**
     * 优化变量名
     * @param js_content JavaScript内容
     * @return 优化后的JavaScript
     */
    static std::string optimizeVariables(const std::string& js_content);
};

/**
 * CHTLJSDetector - CHTL JS语法检测器
 * 检测和分离CHTL JS语法
 */
class CHTLJSDetector {
public:
    /**
     * CHTL JS语法模式
     */
    struct CHTLJSPattern {
        std::string pattern;
        std::string description;
        bool is_function_call;
        
        CHTLJSPattern(const std::string& p, const std::string& d, bool func = false)
            : pattern(p), description(d), is_function_call(func) {}
    };
    
    /**
     * 检测结果
     */
    struct DetectionResult {
        bool has_chtljs;
        std::vector<std::string> chtljs_fragments;
        std::vector<std::string> pure_js_fragments;
        std::vector<CHTLJSPattern> detected_patterns;
        
        DetectionResult() : has_chtljs(false) {}
    };
    
    /**
     * 检测CHTL JS语法
     * @param js_content JavaScript内容
     * @return 检测结果
     */
    static DetectionResult detect(const std::string& js_content);
    
    /**
     * 获取CHTL JS语法模式列表
     * @return 模式列表
     */
    static std::vector<CHTLJSPattern> getCHTLJSPatterns();

private:
    /**
     * 初始化CHTL JS模式
     * @return 模式列表
     */
    static std::vector<CHTLJSPattern> initializePatterns();
    
    /**
     * 匹配模式
     * @param content 内容
     * @param pattern 模式
     * @return 匹配结果
     */
    static std::vector<std::string> matchPattern(const std::string& content, 
                                                const CHTLJSPattern& pattern);
};

} // namespace Compiler
} // namespace CHTL