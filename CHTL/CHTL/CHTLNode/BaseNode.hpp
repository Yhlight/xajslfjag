#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <any>

namespace CHTL {

// 前向声明
class CHTLState;

/**
 * CHTL节点类型枚举
 * 严格按照CHTL语法文档定义所有节点类型
 */
enum class NodeType {
    // 基础节点
    BASE,
    ELEMENT,              // 元素节点
    TEXT,                 // 文本节点
    COMMENT,              // 注释节点
    
    // 块节点
    TEMPLATE,             // 模板节点
    CUSTOM,               // 自定义节点
    ORIGIN,               // 原始嵌入节点
    IMPORT,               // 导入节点
    CONFIGURATION,        // 配置节点
    NAMESPACE,            // 命名空间节点
    
    // 样式和脚本节点
    STYLE,                // 样式节点
    SCRIPT,               // 脚本节点
    
    // 操作节点
    OPERATOR_DELETE,      // delete操作节点
    OPERATOR_INSERT,      // insert操作节点
    OPERATOR_USE,         // use操作节点
    
    // 特殊节点
    ATTRIBUTE,            // 属性节点
    SELECTOR,             // 选择器节点
    PROPERTY,             // 属性值节点
    VALUE,                // 值节点
    REFERENCE,            // 引用节点(&)
    
    // 根节点
    DOCUMENT              // 文档根节点
};

/**
 * 节点属性结构
 */
struct NodeAttribute {
    std::string name;
    std::string value;
    bool isLiteral;       // 是否是字面量
    bool isReference;     // 是否是引用
    
    NodeAttribute() : isLiteral(false), isReference(false) {}
    NodeAttribute(const std::string& n, const std::string& v, bool literal = false, bool ref = false)
        : name(n), value(v), isLiteral(literal), isReference(ref) {}
};

/**
 * CHTL AST基础节点类
 * 所有CHTL节点的基类
 */
class BaseNode : public std::enable_shared_from_this<BaseNode> {
protected:
    NodeType nodeType;
    std::string nodeName;
    std::vector<std::shared_ptr<BaseNode>> children;
    std::weak_ptr<BaseNode> parent;
    std::unordered_map<std::string, NodeAttribute> attributes;
    std::unordered_map<std::string, std::any> metadata;
    
    // 位置信息
    size_t line;
    size_t column;
    size_t position;
    
    // 状态信息
    bool isValid;
    std::string errorMessage;

public:
    BaseNode(NodeType type, const std::string& name = "");
    virtual ~BaseNode() = default;
    
    // 基本属性
    NodeType getType() const;
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // 层次结构管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    void insertChild(size_t index, std::shared_ptr<BaseNode> child);
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const;
    std::shared_ptr<BaseNode> getChild(size_t index) const;
    size_t getChildCount() const;
    
    std::shared_ptr<BaseNode> getParent() const;
    void setParent(std::shared_ptr<BaseNode> parent);
    
    // 查找功能
    std::shared_ptr<BaseNode> findChild(const std::string& name) const;
    std::shared_ptr<BaseNode> findChildByType(NodeType type) const;
    std::vector<std::shared_ptr<BaseNode>> findChildrenByType(NodeType type) const;
    std::shared_ptr<BaseNode> findDescendant(const std::string& name) const;
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value, bool isLiteral = false);
    void setAttribute(const NodeAttribute& attr);
    NodeAttribute getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    void removeAttribute(const std::string& name);
    const std::unordered_map<std::string, NodeAttribute>& getAttributes() const;
    
    // 元数据管理
    template<typename T>
    void setMetadata(const std::string& key, const T& value);
    template<typename T>
    T getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    
    // 位置信息
    void setPosition(size_t line, size_t column, size_t position);
    size_t getLine() const;
    size_t getColumn() const;
    size_t getPosition() const;
    
    // 状态管理
    bool getIsValid() const;
    void setValid(bool valid);
    const std::string& getErrorMessage() const;
    void setErrorMessage(const std::string& error);
    
    // 虚函数 - 子类可以重写
    virtual std::string toString() const;
    virtual std::string toHTML() const;
    virtual std::string toCSS() const;
    virtual std::string toJavaScript() const;
    virtual void validate(CHTLState& state);
    virtual std::shared_ptr<BaseNode> clone() const;
    
    // 遍历支持
    virtual void accept(class NodeVisitor& visitor);
    
    // 调试功能
    void dumpTree(int indent = 0) const;
    std::string getTreeString(int indent = 0) const;
    
    // 工厂方法
    static std::shared_ptr<BaseNode> createNode(NodeType type, const std::string& name = "");
};

/**
 * 元素节点类
 * 表示HTML元素，如div, span, html等
 */
class ElementNode : public BaseNode {
private:
    std::string tagName;
    bool isSelfClosing;
    bool hasStyleBlock;
    bool hasScriptBlock;
    std::vector<std::string> cssClasses;
    std::string idAttribute;

public:
    ElementNode(const std::string& tag);
    
    // 元素特定方法
    const std::string& getTagName() const;
    void setTagName(const std::string& tag);
    
    bool getIsSelfClosing() const;
    void setSelfClosing(bool selfClosing);
    
    bool getHasStyleBlock() const;
    void setHasStyleBlock(bool hasStyle);
    
    bool getHasScriptBlock() const;
    void setHasScriptBlock(bool hasScript);
    
    // CSS类管理
    void addClass(const std::string& className);
    void removeClass(const std::string& className);
    bool hasClass(const std::string& className) const;
    const std::vector<std::string>& getClasses() const;
    
    // ID管理
    void setId(const std::string& id);
    const std::string& getId() const;
    
    // 重写基类方法
    std::string toString() const override;
    std::string toHTML() const override;
    void validate(CHTLState& state) override;
    std::shared_ptr<BaseNode> clone() const override;

private:
    void updateClassAttribute();
};

/**
 * 文本节点类
 * 表示文本内容
 */
class TextNode : public BaseNode {
private:
    std::string content;
    bool isLiteral;
    bool needsEscaping;

public:
    TextNode(const std::string& text = "");
    
    // 文本特定方法
    const std::string& getContent() const;
    void setContent(const std::string& text);
    
    bool getIsLiteral() const;
    void setIsLiteral(bool literal);
    
    bool getNeedsEscaping() const;
    void setNeedsEscaping(bool escaping);
    
    // 文本处理
    std::string getProcessedContent() const;
    void appendContent(const std::string& text);
    
    // 重写基类方法
    std::string toString() const override;
    std::string toHTML() const override;
    void validate(CHTLState& state) override;
    std::shared_ptr<BaseNode> clone() const override;
};

// 模板函数实现
template<typename T>
void BaseNode::setMetadata(const std::string& key, const T& value) {
    metadata[key] = value;
}

template<typename T>
T BaseNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    if (it != metadata.end()) {
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast& e) {
            // 类型不匹配，返回默认值
            return T{};
        }
    }
    return T{};
}

}