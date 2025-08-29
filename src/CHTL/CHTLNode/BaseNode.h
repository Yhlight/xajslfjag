#pragma once

#include "../../Util/Common.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

// AST节点类型枚举
enum class NodeType {
    // 基础节点
    DOCUMENT,           // 文档根节点
    ELEMENT,            // HTML元素节点
    TEXT,               // 文本节点
    COMMENT,            // 注释节点
    
    // 样式和脚本
    STYLE,              // 样式节点
    SCRIPT,             // 脚本节点
    
    // 模板系统
    TEMPLATE,           // 模板节点
    TEMPLATE_STYLE,     // 模板样式组
    TEMPLATE_ELEMENT,   // 模板元素
    TEMPLATE_VAR,       // 模板变量组
    
    // 自定义系统
    CUSTOM,             // 自定义节点
    CUSTOM_STYLE,       // 自定义样式组
    CUSTOM_ELEMENT,     // 自定义元素
    CUSTOM_VAR,         // 自定义变量组
    
    // 原始嵌入
    ORIGIN,             // 原始嵌入节点
    ORIGIN_HTML,        // HTML原始嵌入
    ORIGIN_STYLE,       // CSS原始嵌入
    ORIGIN_JAVASCRIPT,  // JavaScript原始嵌入
    ORIGIN_CUSTOM,      // 自定义类型原始嵌入
    
    // 导入系统
    IMPORT,             // 导入节点
    IMPORT_HTML,        // HTML导入
    IMPORT_STYLE,       // CSS导入
    IMPORT_JAVASCRIPT,  // JavaScript导入
    IMPORT_CHTL,        // CHTL导入
    IMPORT_CMOD,        // CMOD导入
    IMPORT_CJMOD,       // CJMOD导入
    IMPORT_CONFIG,      // 配置导入
    IMPORT_ORIGIN,      // 原始嵌入导入
    
    // 配置系统
    CONFIGURATION,      // 配置节点
    CONFIG_NAME,        // 名称配置
    CONFIG_ORIGIN_TYPE, // 原始类型配置
    
    // 命名空间
    NAMESPACE,          // 命名空间节点
    
    // 操作符
    INHERIT,            // 继承操作
    DELETE,             // 删除操作
    INSERT,             // 插入操作
    USE,                // 使用声明
    
    // 属性和选择器
    ATTRIBUTE,          // 属性节点
    SELECTOR,           // 选择器节点
    CLASS_SELECTOR,     // 类选择器
    ID_SELECTOR,        // ID选择器
    PSEUDO_SELECTOR,    // 伪选择器
    
    // 值和表达式
    LITERAL,            // 字面量
    IDENTIFIER,         // 标识符
    VARIABLE_REF,       // 变量引用
    FUNCTION_CALL,      // 函数调用
    INDEX_ACCESS,       // 索引访问
    
    // CHTL JS特殊节点
    CHTLJS_SELECTOR,    // CHTL JS选择器 {{...}}
    CHTLJS_FUNCTION,    // CHTL JS函数
    CHTLJS_LISTEN,      // 监听器
    CHTLJS_ANIMATE,     // 动画
    CHTLJS_DELEGATE,    // 事件委托
    CHTLJS_VIR_OBJECT,  // 虚对象
    CHTLJS_MODULE,      // CHTL JS模块
    
    // 约束系统
    CONSTRAINT,         // 约束节点
    CONSTRAINT_PRECISE, // 精确约束
    CONSTRAINT_TYPE,    // 类型约束
    CONSTRAINT_GLOBAL,  // 全局约束
    
    // 特例化系统
    DELETE_NODE,        // 删除操作节点
    INSERT_NODE,        // 插入操作节点
    INDEX_ACCESS_NODE,  // 索引访问节点
    NO_VALUE_STYLE,     // 无值样式组节点
    
    // 特殊
    UNKNOWN             // 未知节点
};

// 基础AST节点
class BaseNode {
public:
    NodeType type;
    String value;
    Position position;
    std::vector<std::unique_ptr<BaseNode>> children;
    StringUnorderedMap attributes;
    BaseNode* parent;
    
    explicit BaseNode(NodeType t, const String& v = "", const Position& pos = Position())
        : type(t), value(v), position(pos), parent(nullptr) {}
    
    virtual ~BaseNode() = default;
    
    // 基础访问方法
    NodeType getType() const { return type; }
    const String& getValue() const { return value; }
    const Position& getPosition() const { return position; }
    
    // 节点操作
    void addChild(std::unique_ptr<BaseNode> child);
    void insertChild(size_t index, std::unique_ptr<BaseNode> child);
    void removeChild(size_t index);
    void removeChild(BaseNode* child);
    BaseNode* getChild(size_t index) const;
    size_t getChildCount() const { return children.size(); }
    
    // 获取所有子节点（为了兼容性）
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children; }
    std::vector<std::unique_ptr<BaseNode>>& getChildren() { return children; }
    
    // 属性操作
    void setAttribute(const String& name, const String& value);
    String getAttribute(const String& name, const String& defaultValue = "") const;
    bool hasAttribute(const String& name) const;
    void removeAttribute(const String& name);
    StringVector getAttributeNames() const;
    
    // 查找方法
    BaseNode* findChild(NodeType type) const;
    std::vector<BaseNode*> findChildren(NodeType type) const;
    BaseNode* findDescendant(NodeType type) const;
    std::vector<BaseNode*> findDescendants(NodeType type) const;
    BaseNode* findByValue(const String& value) const;
    BaseNode* findByAttribute(const String& name, const String& value) const;
    
    // 遍历方法
    template<typename Visitor>
    void traverse(Visitor& visitor);
    
    template<typename Visitor>
    void traverseDepthFirst(Visitor& visitor);
    
    template<typename Visitor>
    void traverseBreadthFirst(Visitor& visitor);
    
    // 树操作
    BaseNode* getRoot();
    const BaseNode* getRoot() const;
    BaseNode* getParent() const { return parent; }
    bool isRoot() const { return parent == nullptr; }
    bool isLeaf() const { return children.empty(); }
    size_t getDepth() const;
    StringVector getPath() const;
    
    // 类型检查
    bool isElementNode() const;
    bool isTextNode() const;
    bool isStyleNode() const;
    bool isScriptNode() const;
    bool isTemplateNode() const;
    bool isCustomNode() const;
    bool isOriginNode() const;
    bool isImportNode() const;
    bool isConfigNode() const;
    bool isNamespaceNode() const;
    bool isOperatorNode() const;
    bool isCHTLJSNode() const;
    
    // 序列化
    virtual String toString() const;
    virtual String toXML() const;
    virtual String toJSON() const;
    
    // 调试支持
    void dumpTree(size_t indent = 0) const;
    String getNodeInfo() const;
    
    // 克隆支持
    virtual std::unique_ptr<BaseNode> clone() const;
    
    // 验证
    virtual bool validate() const;
    virtual StringVector getValidationErrors() const;
    
protected:
    virtual void onChildAdded(BaseNode* child) {}
    virtual void onChildRemoved(BaseNode* child) {}
    virtual void onAttributeChanged(const String& name, const String& oldValue, const String& newValue) {}
    
private:
    void setParentForChild(BaseNode* child);
    void removeParentForChild(BaseNode* child);
};

// 元素节点
class ElementNode : public BaseNode {
public:
    String tagName;
    bool isSelfClosing;
    
    explicit ElementNode(const String& tag, const Position& pos = Position())
        : BaseNode(NodeType::ELEMENT, tag, pos), tagName(tag), isSelfClosing(false) {}
    
    // 元素特定方法
    void setTagName(const String& name) { tagName = name; value = name; }
    String getTagName() const { return tagName; }
    
    bool isBlockElement() const;
    bool isInlineElement() const;
    bool isSingleTag() const;
    
    // 样式和脚本子节点
    BaseNode* getStyleBlock() const;
    BaseNode* getScriptBlock() const;
    std::vector<BaseNode*> getTextNodes() const;
    
    // HTML生成
    String toHTML() const;
    String getOpenTag() const;
    String getCloseTag() const;
    
    // 重写基类方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    bool validate() const override;
    
protected:
    void onChildAdded(BaseNode* child) override;
};

// 文本节点
class TextNode : public BaseNode {
public:
    String textContent;
    
    explicit TextNode(const String& text, const Position& pos = Position())
        : BaseNode(NodeType::TEXT, text, pos), textContent(text) {}
    
    void setText(const String& text) { textContent = text; value = text; }
    String getText() const { return textContent; }
    
    bool isEmpty() const { return textContent.empty(); }
    size_t getLength() const { return textContent.length(); }
    
    // 文本处理
    void trim();
    void normalize(); // 规范化空白字符
    
    // HTML生成
    String toHTML() const { return textContent; }
    
    // 重写基类方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    bool validate() const override;
};

// 模板实现（因为是模板函数）
template<typename Visitor>
void BaseNode::traverse(Visitor& visitor) {
    visitor(*this);
    for (auto& child : children) {
        child->traverse(visitor);
    }
}

template<typename Visitor>
void BaseNode::traverseDepthFirst(Visitor& visitor) {
    for (auto& child : children) {
        child->traverseDepthFirst(visitor);
    }
    visitor(*this);
}

template<typename Visitor>
void BaseNode::traverseBreadthFirst(Visitor& visitor) {
    std::queue<BaseNode*> queue;
    queue.push(this);
    
    while (!queue.empty()) {
        BaseNode* current = queue.front();
        queue.pop();
        
        visitor(*current);
        
        for (auto& child : current->children) {
            queue.push(child.get());
        }
    }
}

// 节点工厂
class NodeFactory {
public:
    static std::unique_ptr<BaseNode> createDocumentNode();
    static std::unique_ptr<ElementNode> createElementNode(const String& tagName);
    static std::unique_ptr<TextNode> createTextNode(const String& text);
    static std::unique_ptr<BaseNode> createStyleNode();
    static std::unique_ptr<BaseNode> createScriptNode();
    static std::unique_ptr<BaseNode> createTemplateNode(const String& type, const String& name);
    static std::unique_ptr<BaseNode> createCustomNode(const String& type, const String& name);
    static std::unique_ptr<BaseNode> createOriginNode(const String& type, const String& name = "");
    static std::unique_ptr<BaseNode> createImportNode(const String& type, const String& source);
    static std::unique_ptr<BaseNode> createConfigNode();
    static std::unique_ptr<BaseNode> createNamespaceNode(const String& name);
    static std::unique_ptr<BaseNode> createCHTLJSNode(NodeType type, const String& content);
    
    // 约束节点创建
    static std::unique_ptr<class ConstraintNode> createConstraintNode(const String& constraintType, const String& targets, const String& scope = "");
    
    // 特例化节点创建
    static std::unique_ptr<class DeleteNode> createDeleteNode(const String& operationType, const String& targets);
    static std::unique_ptr<class InsertNode> createInsertNode(const String& position, const String& target);
    static std::unique_ptr<class IndexAccessNode> createIndexAccessNode(const String& elementName, size_t index);
    static std::unique_ptr<class NoValueStyleNode> createNoValueStyleNode(const String& properties);
};

} // namespace CHTL