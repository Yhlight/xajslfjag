#pragma once
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLIOStream/CHTLIOStream.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <sstream>

namespace CHTL {

/**
 * 生成选项
 */
struct GenerateOptions {
    bool prettyPrint = true;                // 美化输出
    std::string indentation = "  ";         // 缩进字符
    std::string lineEnding = "\n";          // 行结束符
    bool generateComments = false;          // 生成注释
    bool minify = false;                    // 压缩输出
    bool generateSourceMap = false;         // 生成源映射
    std::string outputEncoding = "UTF-8";   // 输出编码
    
    // HTML特定选项
    bool html5Mode = true;                  // HTML5模式
    bool selfClosingTags = true;            // 自闭合标签
    bool quoteAttributes = true;            // 属性加引号
    
    // CSS特定选项
    bool compressCSS = false;               // 压缩CSS
    bool autoprefixer = false;              // 自动添加前缀
    
    // JavaScript特定选项
    bool compressJS = false;                // 压缩JavaScript
    bool es6Mode = true;                    // ES6模式
};

/**
 * 生成结果
 */
struct GenerateResult {
    std::string html;                       // 生成的HTML
    std::string css;                        // 生成的CSS
    std::string javascript;                 // 生成的JavaScript
    std::string sourceMap;                  // 源映射
    bool success;                          // 是否成功
    size_t nodesProcessed;                 // 处理的节点数
    std::vector<std::string> warnings;     // 警告信息
    
    GenerateResult() : success(false), nodesProcessed(0) {}
};

/**
 * CHTL代码生成器基类
 */
class CHTLGenerator {
public:
    /**
     * 构造函数
     */
    explicit CHTLGenerator(const GenerateOptions& options = GenerateOptions());
    
    /**
     * 虚析构函数
     */
    virtual ~CHTLGenerator() = default;
    
    /**
     * 生成代码
     */
    virtual GenerateResult generate(const NodePtr& root) = 0;
    
    /**
     * 设置生成选项
     */
    void setGenerateOptions(const GenerateOptions& options);
    
    /**
     * 获取生成选项
     */
    const GenerateOptions& getGenerateOptions() const;
    
    /**
     * 设置错误报告器
     */
    void setErrorReporter(ErrorReporter* errorReporter);

protected:
    GenerateOptions m_options;              // 生成选项
    ErrorReporter* m_errorReporter;         // 错误报告器
    size_t m_currentIndentLevel;            // 当前缩进级别
    
    /**
     * 获取缩进字符串
     */
    std::string getIndent(size_t level = 0) const;
    
    /**
     * 增加缩进
     */
    void increaseIndent();
    
    /**
     * 减少缩进
     */
    void decreaseIndent();
    
    /**
     * 报告警告
     */
    void reportWarning(const std::string& message);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
};

/**
 * HTML生成器
 */
class HTMLGenerator : public CHTLGenerator {
public:
    /**
     * 构造函数
     */
    explicit HTMLGenerator(const GenerateOptions& options = GenerateOptions());
    
    /**
     * 生成HTML
     */
    GenerateResult generate(const NodePtr& root) override;
    
    /**
     * 生成单个节点的HTML
     */
    std::string generateNode(const NodePtr& node);

private:
    std::stringstream m_output;             // 输出流
    std::unordered_map<std::string, size_t> m_classCounter; // 类名计数器
    std::unordered_map<std::string, size_t> m_idCounter;    // ID计数器
    
    /**
     * 生成元素节点
     */
    std::string generateElement(const ElementNode* element);
    
    /**
     * 生成文本节点
     */
    std::string generateText(const TextNode* text);
    
    /**
     * 生成开始标签
     */
    std::string generateStartTag(const ElementNode* element);
    
    /**
     * 生成结束标签
     */
    std::string generateEndTag(const ElementNode* element);
    
    /**
     * 生成属性
     */
    std::string generateAttributes(const ElementNode* element);
    
    /**
     * 转义HTML特殊字符
     */
    std::string escapeHtml(const std::string& text);
    
    /**
     * 处理自动生成的类名和ID
     */
    void processAutoClasses(ElementNode* element);
    
    /**
     * 生成唯一类名
     */
    std::string generateUniqueClassName(const std::string& base);
    
    /**
     * 生成唯一ID
     */
    std::string generateUniqueId(const std::string& base);
};

/**
 * CSS生成器
 */
class CSSGenerator : public CHTLGenerator {
public:
    /**
     * 构造函数
     */
    explicit CSSGenerator(const GenerateOptions& options = GenerateOptions());
    
    /**
     * 生成CSS
     */
    GenerateResult generate(const NodePtr& root) override;
    
    /**
     * 从样式节点生成CSS
     */
    std::string generateFromStyleNodes(const std::vector<NodePtr>& styleNodes);

private:
    std::stringstream m_output;             // 输出流
    std::vector<std::string> m_globalStyles; // 全局样式
    std::unordered_map<std::string, std::string> m_variables; // CSS变量
    
    /**
     * 收集样式节点
     */
    void collectStyleNodes(const NodePtr& node, std::vector<NodePtr>& styleNodes);
    
    /**
     * 生成样式节点
     */
    std::string generateStyleNode(const StyleNode* style);
    
    /**
     * 生成CSS规则
     */
    std::string generateCSSRule(const std::string& selector, 
                               const std::unordered_map<std::string, std::string>& properties);
    
    /**
     * 处理局部样式块
     */
    std::string processLocalStyleBlock(const StyleNode* style, const std::string& elementContext);
    
    /**
     * 处理内联样式
     */
    std::string processInlineStyle(const StyleNode* style);
    
    /**
     * 处理选择器样式
     */
    std::string processSelectorStyle(const StyleNode* style);
    
    /**
     * 解析变量引用
     */
    std::string resolveVariables(const std::string& value);
    
    /**
     * 压缩CSS
     */
    std::string compressCSS(const std::string& css);
    
    /**
     * 添加CSS前缀
     */
    std::string addPrefixes(const std::string& css);
};

/**
 * 局部样式处理器
 * 专门处理CHTL局部样式块的特殊功能
 */
class LocalStyleProcessor {
public:
    /**
     * 样式上下文信息
     */
    struct StyleContext {
        std::string elementName;            // 元素名称
        std::string parentSelector;        // 父选择器
        std::unordered_map<std::string, std::string> attributes; // 元素属性
        bool hasClass;                     // 是否有class属性
        bool hasId;                        // 是否有id属性
        size_t depth;                      // 嵌套深度
        
        StyleContext() : hasClass(false), hasId(false), depth(0) {}
    };
    
    /**
     * 构造函数
     */
    LocalStyleProcessor();
    
    /**
     * 处理局部样式块
     */
    std::string processLocalStyle(const StyleNode* style, const StyleContext& context);
    
    /**
     * 处理自动化类名/ID
     */
    std::pair<std::string, std::string> processAutoClasses(const StyleNode* style, StyleContext& context);
    
    /**
     * 处理上下文推导(&符号)
     */
    std::string processContextSelector(const std::string& selector, const StyleContext& context);
    
    /**
     * 生成全局样式规则
     */
    std::vector<std::string> generateGlobalRules(const StyleNode* style, const StyleContext& context);
    
    /**
     * 生成内联样式
     */
    std::string generateInlineStyle(const StyleNode* style);

private:
    size_t m_classCounter;                  // 类名计数器
    size_t m_idCounter;                     // ID计数器
    std::unordered_map<std::string, std::string> m_selectorCache; // 选择器缓存
    
    /**
     * 解析选择器
     */
    std::string parseSelector(const std::string& selector, const StyleContext& context);
    
    /**
     * 生成自动类名
     */
    std::string generateAutoClassName(const std::string& base = "");
    
    /**
     * 生成自动ID
     */
    std::string generateAutoId(const std::string& base = "");
    
    /**
     * 检查是否为伪类选择器
     */
    bool isPseudoClassSelector(const std::string& selector);
    
    /**
     * 检查是否为伪元素选择器
     */
    bool isPseudoElementSelector(const std::string& selector);
    
    /**
     * 提取选择器基础名称
     */
    std::string extractSelectorBase(const std::string& selector);
};

/**
 * JavaScript生成器（简化版）
 */
class JavaScriptGenerator : public CHTLGenerator {
public:
    /**
     * 构造函数
     */
    explicit JavaScriptGenerator(const GenerateOptions& options = GenerateOptions());
    
    /**
     * 生成JavaScript
     */
    GenerateResult generate(const NodePtr& root) override;

private:
    std::stringstream m_output;             // 输出流
    
    /**
     * 收集脚本节点
     */
    void collectScriptNodes(const NodePtr& node, std::vector<NodePtr>& scriptNodes);
    
    /**
     * 生成脚本节点
     */
    std::string generateScriptNode(const NodePtr& script);
};

/**
 * 统一代码生成器
 * 协调HTML、CSS和JavaScript的生成
 */
class UnifiedGenerator {
public:
    /**
     * 构造函数
     */
    explicit UnifiedGenerator(const GenerateOptions& options = GenerateOptions());
    
    /**
     * 生成完整的Web项目
     */
    GenerateResult generateComplete(const NodePtr& root);
    
    /**
     * 分别生成HTML、CSS和JavaScript
     */
    GenerateResult generateSeparate(const NodePtr& root);
    
    /**
     * 设置生成选项
     */
    void setGenerateOptions(const GenerateOptions& options);
    
    /**
     * 获取HTML生成器
     */
    HTMLGenerator& getHTMLGenerator();
    
    /**
     * 获取CSS生成器
     */
    CSSGenerator& getCSSGenerator();
    
    /**
     * 获取JavaScript生成器
     */
    JavaScriptGenerator& getJavaScriptGenerator();

private:
    GenerateOptions m_options;              // 生成选项
    std::unique_ptr<HTMLGenerator> m_htmlGenerator;     // HTML生成器
    std::unique_ptr<CSSGenerator> m_cssGenerator;       // CSS生成器
    std::unique_ptr<JavaScriptGenerator> m_jsGenerator; // JavaScript生成器
    std::unique_ptr<LocalStyleProcessor> m_styleProcessor; // 样式处理器
    
    /**
     * 合并生成结果
     */
    GenerateResult mergeResults(const GenerateResult& html, 
                               const GenerateResult& css, 
                               const GenerateResult& js);
    
    /**
     * 生成完整的HTML文档
     */
    std::string generateCompleteHTML(const std::string& body, 
                                   const std::string& css, 
                                   const std::string& js);
};

} // namespace CHTL