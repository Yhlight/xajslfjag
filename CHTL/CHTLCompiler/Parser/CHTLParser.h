#pragma once
#include "../Lexer/CHTLLexer.h"
#include "../AST/CHTLNodes.h"
#include <vector>
#include <memory>
#include <stack>

namespace CHTL {

// CHTL解析状态 - 基于真实语法文档
enum class CHTLParsingState {
    GlobalScope,                    // 全局作用域
    InNamespace,                    // 在命名空间中
    InTemplateDefinition,           // 在模板定义中
    InCustomDefinition,             // 在自定义定义中
    InOriginDefinition,             // 在原始嵌入定义中
    InConfigurationDefinition,      // 在配置定义中
    InElement,                      // 在元素中
    InStyleBlock,                   // 在局部样式块中
    InScriptBlock,                  // 在局部脚本块中
    InTextNode,                     // 在文本节点中
};

// CHTL语法分析器 - 严格按照CHTL语法文档
class CHTLParser {
private:
    std::vector<CHTLToken> tokens;
    size_t current;
    std::vector<std::string> errors;
    std::stack<CHTLParsingState> stateStack;
    
    // 当前上下文
    std::string currentNamespace;
    
public:
    explicit CHTLParser(const std::vector<CHTLToken>& tokenList);
    
    // 主解析函数
    std::shared_ptr<CHTLDocumentNode> parseDocument();
    
    // 错误处理
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
private:
    // === 基础Token操作 ===
    CHTLToken peek(size_t offset = 0) const;
    CHTLToken advance();
    bool isAtEnd() const;
    bool match(CHTLTokenType type);
    bool check(CHTLTokenType type) const;
    CHTLToken consume(CHTLTokenType type, const std::string& message);
    
    // === 状态管理 ===
    void pushState(CHTLParsingState state);
    void popState();
    CHTLParsingState getCurrentState() const;
    
    // === 错误处理 ===
    void addError(const std::string& message);
    
    // === CHTL语法解析 - 严格按照语法文档 ===
    
    // 顶级声明
    std::shared_ptr<CHTLNode> parseDeclaration();
    std::shared_ptr<CHTLNode> parseUseDeclaration();           // use html5; use @Config Basic;
    std::shared_ptr<CHTLNode> parseNamespaceDeclaration();     // [Namespace] app { }
    std::shared_ptr<CHTLNode> parseConfigurationDefinition();  // [Configuration] { }
    std::shared_ptr<CHTLNode> parseImportStatement();          // [Import] 各种导入
    
    // 模板系统
    std::shared_ptr<CHTLNode> parseTemplateDefinition();       // [Template] @Style/@Element/@Var
    std::shared_ptr<CHTLNode> parseTemplateUsage();            // @Style Name; 或 @Style Name { }
    
    // 自定义系统
    std::shared_ptr<CHTLNode> parseCustomDefinition();         // [Custom] @Style/@Element/@Var
    std::shared_ptr<CHTLNode> parseCustomUsage();              // 自定义使用和特例化
    
    // 原始嵌入
    std::shared_ptr<CHTLNode> parseOriginDefinition();         // [Origin] @Html/@Style/@JavaScript
    std::shared_ptr<CHTLNode> parseOriginUsage();              // [Origin] @Html name;
    
    // HTML元素
    std::shared_ptr<CHTLNode> parseElement();                  // div { } 等HTML元素
    std::shared_ptr<CHTLNode> parseTextNode();                 // text { content }
    std::shared_ptr<CHTLNode> parseAttribute();                // id: value; class: name;
    
    // 样式块
    std::shared_ptr<CHTLNode> parseStyleBlock();               // style { }
    std::shared_ptr<CHTLNode> parseStyleRule();                // .class { } 或 property: value;
    std::shared_ptr<CHTLNode> parseStyleProperty();            // color: red;
    
    // 脚本块
    std::shared_ptr<CHTLNode> parseScriptBlock();              // script { }
    
    // 选择器自动化
    std::shared_ptr<CHTLNode> parseAutoSelector();             // .class { } 或 #id { }
    
    // 继承和修改
    std::shared_ptr<CHTLNode> parseInheritStatement();         // inherit @Style Base;
    std::shared_ptr<CHTLNode> parseDeleteStatement();          // delete property, @Style Base;
    std::shared_ptr<CHTLNode> parseInsertStatement();          // insert after/before/replace/at top/at bottom
    
    // 约束
    std::shared_ptr<CHTLNode> parseExceptClause();             // except span, @Element Box;
    
    // 变量系统
    std::shared_ptr<CHTLNode> parseVariableUsage();            // ThemeColor(primary)
    
    // 配置块
    std::shared_ptr<CHTLNode> parseConfigurationBlock();       // [Name] { } [OriginType] { }
    
    // 字面量
    std::shared_ptr<CHTLNode> parseLiteral();
    std::shared_ptr<CHTLNode> parseIdentifier();
    
    // === 辅助方法 ===
    
    // 类型检查
    bool isDeclaration() const;
    bool isElement() const;
    bool isTextNode() const;
    bool isStyleBlock() const;
    bool isScriptBlock() const;
    bool isAutoSelector() const;
    
    // 语法验证
    bool isValidHTMLElement(const std::string& name) const;
    bool isValidTemplateType(const std::string& type) const;
    bool isValidOriginType(const std::string& type) const;
    
    // 跳过空白和注释
    void skipWhitespaceAndComments();
    
    // 多词关键字处理
    bool checkMultiWordKeyword(const std::string& first, const std::string& second) const;
};

} // namespace CHTL