#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// CHTL AST节点类型 - 严格按照语法文档
enum class CHTLNodeType {
    // === 文档结构 ===
    DOCUMENT,                     // 根文档节点
    
    // === HTML元素 ===
    HTML_ELEMENT,                 // HTML元素 div, span 等
    TEXT_NODE,                    // text {} 文本节点
    ATTRIBUTE,                    // 属性 id: value
    
    // === CHTL声明 ===
    USE_DECLARATION,              // use 声明
    NAMESPACE_DECLARATION,        // [Namespace] 声明
    
    // === 模板系统 ===
    TEMPLATE_DEFINITION,          // [Template] 模板定义
    TEMPLATE_USAGE,               // @Style Name; 模板使用
    
    // === 自定义系统 ===
    CUSTOM_DEFINITION,            // [Custom] 自定义定义
    CUSTOM_USAGE,                 // @Style Name {} 自定义使用
    
    // === 原始嵌入 ===
    ORIGIN_DEFINITION,            // [Origin] 原始嵌入定义
    ORIGIN_USAGE,                 // [Origin] @Html name; 原始嵌入使用
    
    // === 配置系统 ===
    CONFIGURATION_DEFINITION,     // [Configuration] 配置定义
    CONFIGURATION_BLOCK,          // [Name] [OriginType] 配置块
    
    // === 导入系统 ===
    IMPORT_STATEMENT,             // [Import] 导入语句
    
    // === 样式和脚本 ===
    STYLE_BLOCK,                  // style {} 局部样式块
    STYLE_RULE,                   // .class { property: value; }
    STYLE_PROPERTY,               // property: value;
    SCRIPT_BLOCK,                 // script {} 局部脚本块
    
    // === 继承和特化 ===
    INHERIT_STATEMENT,            // inherit @Style Parent;
    DELETE_STATEMENT,             // delete property;
    INSERT_STATEMENT,             // insert after target { ... }
    
    // === 约束系统 ===
    CONSTRAINT_CLAUSE,            // except span, @Element Box;
    
    // === 变量系统 ===
    VARIABLE_DEFINITION,          // [Template] @Var 变量定义
    VARIABLE_USAGE,               // ThemeColor(param)
    
    // === 注释 ===
    LINE_COMMENT,                 // // 注释
    BLOCK_COMMENT,                // /* 注释 */
    GENERATOR_COMMENT,            // -- 注释
    
    // === 字面量 ===
    STRING_LITERAL,               // "string"
    NUMBER_LITERAL,               // 123
    UNQUOTED_LITERAL,             // CSS风格字面量
    IDENTIFIER,                   // 标识符
    
    // === 选择器 ===
    CLASS_SELECTOR,               // .className
    ID_SELECTOR,                  // #idName
    REFERENCE_SELECTOR,           // &
    
    // === CMOD相关 ===
    INFO_BLOCK,                   // [Info] 信息块
    EXPORT_BLOCK                  // [Export] 导出块
};

// CHTL AST基础节点
class CHTLNode : public std::enable_shared_from_this<CHTLNode> {
public:
    CHTLNodeType type;
    std::string value;                    // 节点值
    std::string name;                     // 节点名称
    std::unordered_map<std::string, std::string> attributes; // 属性
    std::vector<std::shared_ptr<CHTLNode>> children;         // 子节点
    std::weak_ptr<CHTLNode> parent;       // 父节点
    
    // 位置信息
    size_t line;
    size_t column;
    size_t position;
    
    // 构造函数
    explicit CHTLNode(CHTLNodeType nodeType, 
                      const std::string& nodeName = "",
                      const std::string& nodeValue = "");
    
    // 虚析构函数
    virtual ~CHTLNode() = default;
    
    // 子节点操作
    void addChild(std::shared_ptr<CHTLNode> child);
    void removeChild(std::shared_ptr<CHTLNode> child);
    std::shared_ptr<CHTLNode> findChild(const std::string& name, CHTLNodeType type = CHTLNodeType::HTML_ELEMENT);
    std::vector<std::shared_ptr<CHTLNode>> findChildren(CHTLNodeType type);
    
    // 属性操作
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    
    // 位置设置
    void setPosition(size_t line, size_t column, size_t position);
    
    // 树遍历
    void traverse(std::function<void(std::shared_ptr<CHTLNode>)> visitor);
    
    // 节点类型判断
    bool isDefinition() const;
    bool isUsage() const;
    bool isBlock() const;
    bool isLeaf() const;
    bool isComment() const;
    
    // 序列化
    std::string toString(int indent = 0) const;
    std::string toXML(int indent = 0) const;
    
    // 获取节点类型名称
    std::string getTypeName() const;
};

// CHTL文档节点
class CHTLDocumentNode : public CHTLNode {
public:
    CHTLDocumentNode();
    
    // 文档级别的特殊操作
    std::vector<std::shared_ptr<CHTLNode>> getUseDeclarations();
    std::vector<std::shared_ptr<CHTLNode>> getNamespaceDeclarations();
    std::vector<std::shared_ptr<CHTLNode>> getTemplateDefinitions();
    std::vector<std::shared_ptr<CHTLNode>> getCustomDefinitions();
    std::vector<std::shared_ptr<CHTLNode>> getImportStatements();
    std::vector<std::shared_ptr<CHTLNode>> getConfigurationDefinitions();
};

// CHTL HTML元素节点
class CHTLElementNode : public CHTLNode {
public:
    explicit CHTLElementNode(const std::string& tagName);
    
    // 元素特有操作
    std::shared_ptr<CHTLNode> getStyleBlock();
    std::shared_ptr<CHTLNode> getScriptBlock();
    std::vector<std::shared_ptr<CHTLNode>> getTextNodes();
    std::vector<std::shared_ptr<CHTLNode>> getChildElements();
    
    // HTML属性操作
    void setId(const std::string& id);
    void setClass(const std::string& className);
    void addClass(const std::string& className);
    void removeClass(const std::string& className);
    
    std::string getId() const;
    std::string getClass() const;
    std::vector<std::string> getClasses() const;
};

// CHTL模板定义节点
class CHTLTemplateNode : public CHTLNode {
public:
    std::string templateType;  // Style, Element, Var
    
    explicit CHTLTemplateNode(const std::string& name, const std::string& type);
    
    // 模板特有操作
    bool isStyleTemplate() const { return templateType == "Style"; }
    bool isElementTemplate() const { return templateType == "Element"; }
    bool isVariableTemplate() const { return templateType == "Var"; }
    
    // 继承操作
    void addInheritance(const std::string& parentName, const std::string& parentType);
    std::vector<std::pair<std::string, std::string>> getInheritances();
};

// CHTL自定义节点
class CHTLCustomNode : public CHTLNode {
public:
    std::string customType;    // Style, Element, Var
    
    explicit CHTLCustomNode(const std::string& name, const std::string& type);
    
    // 自定义特有操作
    bool isStyleCustom() const { return customType == "Style"; }
    bool isElementCustom() const { return customType == "Element"; }
    bool isVariableCustom() const { return customType == "Var"; }
    
    // 特化操作
    std::vector<std::shared_ptr<CHTLNode>> getDeleteStatements();
    std::vector<std::shared_ptr<CHTLNode>> getInsertStatements();
    std::vector<std::shared_ptr<CHTLNode>> getInheritStatements();
};

// CHTL样式块节点
class CHTLStyleNode : public CHTLNode {
public:
    bool isLocal;              // 是否为局部样式
    
    explicit CHTLStyleNode(bool local = true);
    
    // 样式特有操作
    std::vector<std::shared_ptr<CHTLNode>> getStyleRules();
    std::vector<std::shared_ptr<CHTLNode>> getStyleProperties();
    void addStyleRule(const std::string& selector, const std::vector<std::pair<std::string, std::string>>& properties);
    void addStyleProperty(const std::string& property, const std::string& value);
};

// CHTL脚本块节点
class CHTLScriptNode : public CHTLNode {
public:
    bool isLocal;              // 是否为局部脚本
    std::string scriptContent; // 脚本内容(将交给CHTL JS编译器处理)
    
    explicit CHTLScriptNode(bool local = true);
    
    // 脚本特有操作
    void setScriptContent(const std::string& content);
    std::string getScriptContent() const;
};

// CHTL导入节点
class CHTLImportNode : public CHTLNode {
public:
    std::string importType;    // Html, Style, JavaScript, Chtl, CJmod
    std::string importPath;
    std::string alias;
    std::vector<std::string> exceptItems;
    
    explicit CHTLImportNode(const std::string& type, const std::string& path);
    
    // 导入特有操作
    void setAlias(const std::string& aliasName);
    void addExceptItem(const std::string& item);
    void setFromPath(const std::string& path);
    
    bool hasAlias() const { return !alias.empty(); }
    bool hasExceptClause() const { return !exceptItems.empty(); }
};

// 节点工厂
class CHTLNodeFactory {
public:
    // 创建基础节点
    static std::shared_ptr<CHTLNode> createNode(CHTLNodeType type, const std::string& name = "", const std::string& value = "");
    
    // 创建特定节点
    static std::shared_ptr<CHTLDocumentNode> createDocument();
    static std::shared_ptr<CHTLElementNode> createElement(const std::string& tagName);
    static std::shared_ptr<CHTLNode> createTextNode(const std::string& text);
    static std::shared_ptr<CHTLTemplateNode> createTemplate(const std::string& name, const std::string& type);
    static std::shared_ptr<CHTLCustomNode> createCustom(const std::string& name, const std::string& type);
    static std::shared_ptr<CHTLStyleNode> createStyle(bool local = true);
    static std::shared_ptr<CHTLScriptNode> createScript(bool local = true);
    static std::shared_ptr<CHTLImportNode> createImport(const std::string& type, const std::string& path);
    
    // 创建声明节点
    static std::shared_ptr<CHTLNode> createUseDeclaration(const std::string& useType);
    static std::shared_ptr<CHTLNode> createNamespaceDeclaration(const std::string& namespaceName);
    static std::shared_ptr<CHTLNode> createConfigurationDefinition();
    
    // 创建注释节点
    static std::shared_ptr<CHTLNode> createLineComment(const std::string& comment);
    static std::shared_ptr<CHTLNode> createBlockComment(const std::string& comment);
    static std::shared_ptr<CHTLNode> createGeneratorComment(const std::string& comment);
};

} // namespace CHTL