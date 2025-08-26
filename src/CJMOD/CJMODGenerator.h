#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>
#include "Arg.h"

namespace CHTL {
namespace CJMOD {

/**
 * GeneratorOptions - 生成器选项
 */
struct GeneratorOptions {
    bool minify;                    // 是否压缩代码
    bool add_comments;              // 是否添加注释
    bool strict_mode;               // 是否使用严格模式
    bool use_modules;               // 是否使用ES6模块
    std::string indent;             // 缩进字符
    std::string line_ending;        // 行结束符
    std::string target_version;     // 目标JavaScript版本
    bool enable_sourcemap;          // 是否生成sourcemap
    
    GeneratorOptions()
        : minify(false)
        , add_comments(true)
        , strict_mode(false)
        , use_modules(false)
        , indent("  ")
        , line_ending("\n")
        , target_version("ES5")
        , enable_sourcemap(false) {}
};

/**
 * CodeFragment - 代码片段
 */
struct CodeFragment {
    std::string content;            // 代码内容
    std::string type;               // 代码类型
    std::unordered_map<std::string, std::string> metadata; // 元数据
    int line_number;                // 行号
    int column_number;              // 列号
    
    CodeFragment() : line_number(0), column_number(0) {}
    CodeFragment(const std::string& content, const std::string& type)
        : content(content), type(type), line_number(0), column_number(0) {}
};

/**
 * GenerationResult - 生成结果
 */
struct GenerationResult {
    std::string generated_code;     // 生成的代码
    std::vector<std::string> warnings; // 警告信息
    std::vector<std::string> errors;   // 错误信息
    std::unordered_map<std::string, std::string> metadata; // 元数据
    bool success;                   // 是否成功
    
    GenerationResult() : success(false) {}
};

/**
 * CJMODGenerator - CJMOD生成器
 * 负责将CJMOD API处理的结果导出为最终的JavaScript代码
 */
class CJMODGenerator {
public:
    /**
     * 导出最终的JS代码
     * @param args 处理后的参数
     * @return 生成的JavaScript代码
     */
    static std::string exportResult(const Arg& args);
    
    /**
     * 导出最终的JS代码（带选项）
     * @param args 处理后的参数
     * @param options 生成选项
     * @return 生成结果
     */
    static GenerationResult exportResult(const Arg& args, const GeneratorOptions& options);
    
    /**
     * 生成函数调用代码
     * @param function_name 函数名
     * @param args 参数
     * @return JavaScript函数调用代码
     */
    static std::string generateFunctionCall(const std::string& function_name, const Arg& args);
    
    /**
     * 生成表达式代码
     * @param args 表达式参数
     * @return JavaScript表达式代码
     */
    static std::string generateExpression(const Arg& args);
    
    /**
     * 生成变量声明代码
     * @param variable_name 变量名
     * @param value 值
     * @param type 声明类型（var, let, const）
     * @return JavaScript变量声明代码
     */
    static std::string generateVariableDeclaration(const std::string& variable_name, const std::string& value, const std::string& type = "var");
    
    /**
     * 生成对象字面量代码
     * @param properties 属性映射
     * @return JavaScript对象字面量代码
     */
    static std::string generateObjectLiteral(const std::unordered_map<std::string, std::string>& properties);
    
    /**
     * 生成数组字面量代码
     * @param elements 元素列表
     * @return JavaScript数组字面量代码
     */
    static std::string generateArrayLiteral(const std::vector<std::string>& elements);
    
    /**
     * 生成CHTL JS特定代码
     * @param args CHTL JS参数
     * @param function_type 函数类型（listen, animate, delegate等）
     * @return 转换后的JavaScript代码
     */
    static std::string generateCHTLJSCode(const Arg& args, const std::string& function_type);
    
    /**
     * 生成虚对象处理代码
     * @param vir_name 虚对象名称
     * @param args 参数
     * @return JavaScript代码
     */
    static std::string generateVirtualObjectCode(const std::string& vir_name, const Arg& args);
    
    /**
     * 生成模块导出代码
     * @param exports 导出项
     * @return JavaScript模块导出代码
     */
    static std::string generateModuleExports(const std::unordered_map<std::string, std::string>& exports);
    
    /**
     * 批量生成代码
     * @param arg_list 参数列表
     * @param options 生成选项
     * @return 生成结果列表
     */
    static std::vector<GenerationResult> batchGenerate(const std::vector<Arg>& arg_list, const GeneratorOptions& options);
    
    /**
     * 设置全局生成选项
     * @param options 生成选项
     */
    static void setGlobalOptions(const GeneratorOptions& options);
    
    /**
     * 获取全局生成选项
     * @return 生成选项
     */
    static GeneratorOptions getGlobalOptions();

private:
    static GeneratorOptions global_options;
    
    // 代码生成方法
    static std::string generateBasicExpression(const Arg& args);
    static std::string generateComplexExpression(const Arg& args);
    static std::string generateFunctionExpression(const Arg& args);
    static std::string generateOperatorExpression(const Arg& args);
    
    // CHTL JS特定生成方法
    static std::string generateListenCode(const Arg& args);
    static std::string generateAnimateCode(const Arg& args);
    static std::string generateDelegateCode(const Arg& args);
    static std::string generateEnhancedSelectorCode(const std::string& selector);
    
    // 代码格式化方法
    static std::string formatCode(const std::string& code, const GeneratorOptions& options);
    static std::string minifyCode(const std::string& code);
    static std::string addIndentation(const std::string& code, const std::string& indent);
    static std::string addComments(const std::string& code);
    
    // 工具方法
    static std::string escapeString(const std::string& str);
    static std::string sanitizeName(const std::string& name);
    static bool isValidJavaScriptIdentifier(const std::string& name);
    static std::string generateUniqueId(const std::string& prefix = "cjmod_");
    
    // 验证方法
    static bool validateGeneratedCode(const std::string& code);
    static std::vector<std::string> checkSyntaxErrors(const std::string& code);
    
    // 错误处理
    static void addError(GenerationResult& result, const std::string& error);
    static void addWarning(GenerationResult& result, const std::string& warning);
};

/**
 * CodeBuilder - 代码构建器
 */
class CodeBuilder {
public:
    CodeBuilder();
    
    /**
     * 添加代码行
     * @param line 代码行
     * @return 构建器引用
     */
    CodeBuilder& addLine(const std::string& line);
    
    /**
     * 添加代码块
     * @param block 代码块
     * @return 构建器引用
     */
    CodeBuilder& addBlock(const std::string& block);
    
    /**
     * 添加注释
     * @param comment 注释内容
     * @return 构建器引用
     */
    CodeBuilder& addComment(const std::string& comment);
    
    /**
     * 添加函数
     * @param name 函数名
     * @param params 参数列表
     * @param body 函数体
     * @return 构建器引用
     */
    CodeBuilder& addFunction(const std::string& name, const std::vector<std::string>& params, const std::string& body);
    
    /**
     * 添加变量声明
     * @param name 变量名
     * @param value 初始值
     * @param type 声明类型
     * @return 构建器引用
     */
    CodeBuilder& addVariable(const std::string& name, const std::string& value, const std::string& type = "var");
    
    /**
     * 增加缩进级别
     * @return 构建器引用
     */
    CodeBuilder& indent();
    
    /**
     * 减少缩进级别
     * @return 构建器引用
     */
    CodeBuilder& unindent();
    
    /**
     * 添加空行
     * @return 构建器引用
     */
    CodeBuilder& newLine();
    
    /**
     * 构建最终代码
     * @return JavaScript代码
     */
    std::string build();
    
    /**
     * 构建最终代码（带选项）
     * @param options 生成选项
     * @return 生成结果
     */
    GenerationResult build(const GeneratorOptions& options);
    
    /**
     * 清空内容
     */
    void clear();

private:
    std::vector<CodeFragment> fragments;
    int current_indent_level;
    GeneratorOptions builder_options;
    
    std::string getCurrentIndent() const;
    void addFragment(const std::string& content, const std::string& type);
};

/**
 * TemplateEngine - 模板引擎
 */
class TemplateEngine {
public:
    /**
     * 注册模板
     * @param name 模板名称
     * @param template_string 模板字符串
     */
    static void registerTemplate(const std::string& name, const std::string& template_string);
    
    /**
     * 移除模板
     * @param name 模板名称
     */
    static void removeTemplate(const std::string& name);
    
    /**
     * 渲染模板
     * @param name 模板名称
     * @param variables 变量映射
     * @return 渲染结果
     */
    static std::string renderTemplate(const std::string& name, const std::unordered_map<std::string, std::string>& variables);
    
    /**
     * 直接渲染模板字符串
     * @param template_string 模板字符串
     * @param variables 变量映射
     * @return 渲染结果
     */
    static std::string renderString(const std::string& template_string, const std::unordered_map<std::string, std::string>& variables);
    
    /**
     * 检查模板是否存在
     * @param name 模板名称
     * @return 是否存在
     */
    static bool hasTemplate(const std::string& name);
    
    /**
     * 获取所有模板名称
     * @return 模板名称列表
     */
    static std::vector<std::string> getTemplateNames();

private:
    static std::unordered_map<std::string, std::string> templates;
    
    static std::string processTemplate(const std::string& template_string, const std::unordered_map<std::string, std::string>& variables);
    static std::string replaceVariables(const std::string& str, const std::unordered_map<std::string, std::string>& variables);
};

/**
 * CJMODGeneratorUtils - CJMOD生成器工具类
 */
class CJMODGeneratorUtils {
public:
    /**
     * 格式化JavaScript代码
     * @param code 原始代码
     * @return 格式化后的代码
     */
    static std::string formatJavaScript(const std::string& code);
    
    /**
     * 压缩JavaScript代码
     * @param code 原始代码
     * @return 压缩后的代码
     */
    static std::string minifyJavaScript(const std::string& code);
    
    /**
     * 验证JavaScript语法
     * @param code JavaScript代码
     * @return 是否有效
     */
    static bool validateJavaScript(const std::string& code);
    
    /**
     * 检测JavaScript语法错误
     * @param code JavaScript代码
     * @return 错误列表
     */
    static std::vector<std::string> detectSyntaxErrors(const std::string& code);
    
    /**
     * 优化JavaScript代码
     * @param code 原始代码
     * @return 优化后的代码
     */
    static std::string optimizeJavaScript(const std::string& code);
    
    /**
     * 转换为指定JavaScript版本
     * @param code 原始代码
     * @param target_version 目标版本
     * @return 转换后的代码
     */
    static std::string transpileToVersion(const std::string& code, const std::string& target_version);
    
    /**
     * 添加严格模式声明
     * @param code 原始代码
     * @return 添加严格模式后的代码
     */
    static std::string addStrictMode(const std::string& code);
    
    /**
     * 生成sourcemap
     * @param original_code 原始代码
     * @param generated_code 生成的代码
     * @return sourcemap JSON
     */
    static std::string generateSourceMap(const std::string& original_code, const std::string& generated_code);
};

} // namespace CJMOD
} // namespace CHTL