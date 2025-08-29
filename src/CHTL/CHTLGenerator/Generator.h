#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include <string>
#include <memory>
#include <sstream>
#include <stack>
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ProgramNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/ConfigNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/OperatorNode.h"
#include "../CHTLContext/Context.h"

namespace CHTL {

// 生成器配置
struct GeneratorConfig {
    bool prettyPrint = true;            // 美化输出
    int indentSize = 2;                 // 缩进大小
    bool generateComments = true;       // 生成注释
    bool minify = false;                // 压缩输出
    std::string lineEnding = "\n";      // 行结束符
};

// HTML生成器
class Generator {
private:
    // 模板变量存储
    std::unordered_map<std::string, std::string> templateVars_;
    
public:
    Generator(std::shared_ptr<CompileContext> context,
              const GeneratorConfig& config = GeneratorConfig());
    ~Generator() = default;
    
    // 生成HTML
    std::string generate(std::shared_ptr<ProgramNode> program);
    
    // 访问者方法实现
    void visitProgramNode(ProgramNode* node);
    void visitElementNode(ElementNode* node);
    void visitTextNode(TextNode* node);
    void visitAttributeNode(AttributeNode* node);
    void visitCommentNode(CommentNode* node);
    void visitTemplateNode(TemplateNode* node);
    void visitTemplateUseNode(TemplateUseNode* node);
    void visitCustomNode(CustomNode* node);
    void visitCustomUseNode(CustomUseNode* node);
    void visitStyleNode(StyleNode* node);
    void visitSelectorNode(SelectorNode* node);
    void visitPropertyNode(PropertyNode* node);
    void visitScriptNode(ScriptNode* node);
    void visitOriginNode(OriginNode* node);
    void visitOriginUseNode(OriginUseNode* node);
    void visitImportNode(ImportNode* node);
    void visitConfigNode(ConfigNode* node);
    void visitInfoNode(InfoNode* node);
    void visitExportNode(ExportNode* node);
    void visitNamespaceNode(NamespaceNode* node);
    void visitFromNode(FromNode* node);
    void visitDeleteNode(DeleteNode* node);
    void visitInsertNode(InsertNode* node);
    void visitInheritNode(InheritNode* node);
    void visitExceptNode(ExceptNode* node);
    void visitUseNode(UseNode* node);

private:
    std::shared_ptr<CompileContext> context_;
    GeneratorConfig config_;
    std::stringstream output_;
    std::stringstream globalStyles_;
    std::stringstream globalScripts_;
    int indentLevel_ = 0;
    
    // 生成状态
    struct GeneratorState {
        bool inStyleBlock = false;
        bool inScriptBlock = false;
        bool inLocalStyle = false;
        bool inLocalScript = false;
        std::string currentElement;
        std::vector<std::string> autoAddedClasses;
        std::vector<std::string> autoAddedIds;
        std::string pendingInlineStyles;  // 待添加的内联样式
        ElementNode* currentElementNode = nullptr;  // 当前正在处理的元素节点
    };
    
    std::stack<GeneratorState> stateStack_;
    GeneratorState currentState_;
    
    // 输出辅助方法
    void write(const std::string& text);
    void writeLine(const std::string& text = "");
    void indent();
    void dedent();
    std::string getIndent() const;
    
    // HTML生成方法
    void generateHtmlElement(ElementNode* node);
    void generateAttributes(ElementNode* node);
    void generateHtml5Doctype();
    
    // 样式生成方法
    void generateGlobalStyles();
    void generateLocalStyles(StyleNode* node);
    void generateSelector(SelectorNode* node);
    void generateCSSRule(SelectorNode* node);
    void generateCssProperty(PropertyNode* node);
    std::string processSelectorReference(const std::string& selector);
    
    // 脚本生成方法
    void generateGlobalScripts();
    void generateLocalScripts(ScriptNode* node);
    
    // 选择器自动化
    void processAutoSelectors(StyleNode* node);
    void applyAutoClasses(ElementNode* element);
    void applyAutoIds(ElementNode* element);
    
    // 模板展开
    void expandTemplate(TemplateUseNode* node);
    void expandCustom(CustomUseNode* node);
    
    // 变量替换
    std::string replaceVariables(const std::string& value);
    
    // 注释生成
    void generateHtmlComment(const std::string& comment);
    void generateCssComment(const std::string& comment);
    void generateJsComment(const std::string& comment);
    
    // 状态管理
    void pushState();
    void popState();
    
    // 辅助方法
    bool isVoidElement(const std::string& tagName);
    std::string escapeHtml(const std::string& text);
    std::string escapeCss(const std::string& text);
    std::string escapeJs(const std::string& text);
};

// 生成器异常
class GeneratorException : public std::exception {
public:
    explicit GeneratorException(const std::string& message) : msg_(message) {}
    const char* what() const noexcept override { return msg_.c_str(); }
    
private:
    std::string msg_;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H