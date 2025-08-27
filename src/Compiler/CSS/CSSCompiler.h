#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// ANTLR4生成的头文件
#include "css3Lexer.h"
#include "css3Parser.h"
#include "css3ParserBaseListener.h"

namespace CHTL {
namespace Compiler {

/**
 * CSSCompiler - CSS编译器
 * 利用现有的ANTLR4基础设施和语法文件
 */
class CSSCompiler {
public:
    /**
     * CSS编译结果
     */
    struct CompileResult {
        bool success;
        std::string compiled_css;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        CompileResult() : success(false) {}
    };
    
    /**
     * CSS代码片段信息
     */
    struct CSSFragment {
        std::string content;
        std::string source_file;
        int line_number;
        bool is_global;                 // 是否为全局样式
        std::string parent_element;     // 父元素（局部样式时）
        
        CSSFragment() : line_number(0), is_global(true) {}
    };

public:
    /**
     * 编译CSS代码片段
     * @param css_fragments CSS片段列表
     * @return 编译结果
     */
    static CompileResult compile(const std::vector<CSSFragment>& css_fragments);
    
    /**
     * 编译单个CSS片段
     * @param fragment CSS片段
     * @return 编译结果
     */
    static CompileResult compileSingle(const CSSFragment& fragment);
    
    /**
     * 验证CSS语法
     * @param css_content CSS内容
     * @return 是否有效
     */
    static bool validateSyntax(const std::string& css_content);
    
    /**
     * 格式化CSS代码
     * @param css_content CSS内容
     * @param minify 是否压缩
     * @return 格式化后的CSS
     */
    static std::string formatCSS(const std::string& css_content, bool minify = false);
    
    /**
     * 合并CSS片段
     * @param fragments CSS片段列表
     * @return 合并后的CSS
     */
    static std::string mergeFragments(const std::vector<CSSFragment>& fragments);
    
    /**
     * 处理局部样式块
     * @param local_css 局部CSS内容
     * @param parent_selector 父选择器
     * @return 处理后的全局CSS
     */
    static std::string processLocalStyle(const std::string& local_css, 
                                       const std::string& parent_selector);

private:
    /**
     * 初始化ANTLR4解析器
     * @param css_content CSS内容
     * @return [lexer, parser, tree]
     */
    static std::tuple<std::shared_ptr<css3Lexer>, 
                     std::shared_ptr<css3Parser>, 
                     css3Parser::StylesheetContext*> initializeParser(const std::string& css_content);
    
    /**
     * 收集解析错误
     * @param parser 解析器
     * @return 错误列表
     */
    static std::vector<std::string> collectParseErrors(css3Parser* parser);
    
    /**
     * 生成CSS代码
     * @param tree 解析树
     * @return 生成的CSS代码
     */
    static std::string generateCSS(css3Parser::StylesheetContext* tree);
};

/**
 * CSSListener - CSS解析监听器
 * 继承自ANTLR4生成的BaseListener
 */
class CSSListener : public css3ParserBaseListener {
public:
    /**
     * 构造函数
     * @param parent_selector 父选择器（用于局部样式）
     */
    explicit CSSListener(const std::string& parent_selector = "");
    
    /**
     * 获取生成的CSS
     * @return CSS字符串
     */
    std::string getGeneratedCSS() const;
    
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

    // ANTLR4监听器方法重写
    void enterStylesheet(css3Parser::StylesheetContext* ctx) override;
    void exitStylesheet(css3Parser::StylesheetContext* ctx) override;
    
    void enterRuleset(css3Parser::RulesetContext* ctx) override;
    void exitRuleset(css3Parser::RulesetContext* ctx) override;
    
    void enterSelector(css3Parser::SelectorContext* ctx) override;
    void exitSelector(css3Parser::SelectorContext* ctx) override;
    
    void enterDeclaration(css3Parser::DeclarationContext* ctx) override;
    void exitDeclaration(css3Parser::DeclarationContext* ctx) override;

private:
    std::string parent_selector_;
    std::string generated_css_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 临时存储
    std::string current_selector_;
    std::vector<std::string> current_declarations_;
    
    /**
     * 处理选择器（添加父选择器前缀）
     * @param selector 原始选择器
     * @return 处理后的选择器
     */
    std::string processSelector(const std::string& selector);
    
    /**
     * 添加CSS规则
     * @param selector 选择器
     * @param declarations 声明列表
     */
    void addCSSRule(const std::string& selector, const std::vector<std::string>& declarations);
};

/**
 * CSSOptimizer - CSS优化器
 * 提供CSS压缩、优化功能
 */
class CSSOptimizer {
public:
    /**
     * 优化选项
     */
    struct OptimizeOptions {
        bool minify = false;                    // 是否压缩
        bool remove_comments = false;           // 是否移除注释
        bool combine_selectors = false;         // 是否合并相同选择器
        bool remove_duplicates = false;         // 是否移除重复规则
        bool optimize_values = false;           // 是否优化值（如颜色、尺寸）
        
        OptimizeOptions() = default;
    };
    
    /**
     * 优化CSS
     * @param css_content CSS内容
     * @param options 优化选项
     * @return 优化后的CSS
     */
    static std::string optimize(const std::string& css_content, 
                               const OptimizeOptions& options = OptimizeOptions());
    
    /**
     * 压缩CSS
     * @param css_content CSS内容
     * @return 压缩后的CSS
     */
    static std::string minify(const std::string& css_content);
    
    /**
     * 美化CSS
     * @param css_content CSS内容
     * @param indent_size 缩进大小
     * @return 美化后的CSS
     */
    static std::string beautify(const std::string& css_content, int indent_size = 2);

private:
    /**
     * 移除注释
     * @param css_content CSS内容
     * @return 移除注释后的CSS
     */
    static std::string removeComments(const std::string& css_content);
    
    /**
     * 移除多余空白
     * @param css_content CSS内容
     * @return 移除空白后的CSS
     */
    static std::string removeWhitespace(const std::string& css_content);
    
    /**
     * 合并相同选择器
     * @param css_content CSS内容
     * @return 合并后的CSS
     */
    static std::string combineSelectors(const std::string& css_content);
};

} // namespace Compiler
} // namespace CHTL