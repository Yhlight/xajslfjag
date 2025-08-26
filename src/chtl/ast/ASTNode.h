#ifndef CHTL_AST_NODE_H
#define CHTL_AST_NODE_H

#include <memory>
#include <vector>
#include <string>
#include <variant>
#include "../CHTLToken.h"

namespace chtl {
namespace ast {

// AST节点类型
enum class ASTNodeType {
    // 根节点
    ROOT,
    
    // 元素相关
    ELEMENT,
    ATTRIBUTE,
    TEXT,
    
    // 样式相关
    STYLE_BLOCK,         // 局部样式块
    STYLE_RULE,          // 样式规则
    STYLE_PROPERTY,      // 样式属性
    SELECTOR,            // 选择器
    
    // 脚本相关
    SCRIPT_BLOCK,        // 局部脚本块
    
    // 模板和自定义
    TEMPLATE,            // [Template]
    CUSTOM,              // [Custom]
    
    // 原始嵌入
    ORIGIN,              // [Origin]
    
    // 导入和命名空间
    IMPORT,              // [Import]
    NAMESPACE,           // [Namespace]
    
    // 配置
    CONFIGURATION,       // [Configuration]
    CONFIG_PROPERTY,     // 配置属性
    
    // 特殊节点
    USE_STATEMENT,       // use语句
    EXCEPT_STATEMENT,    // except语句
    
    // 操作节点
    INHERIT,             // 继承
    DELETE,              // 删除
    INSERT,              // 插入
    
    // 引用节点
    REFERENCE,           // 引用（@Style、@Element等）
    FROM_CLAUSE,         // from子句
    
    // 表达式
    IDENTIFIER,          // 标识符
    LITERAL,             // 字面量
    EXPRESSION,          // 表达式
    
    // 注释
    COMMENT,             // 注释节点
    GENERATOR_COMMENT    // 生成器注释
};

// 前向声明
class ASTVisitor;

// AST节点基类
class ASTNode {
public:
    ASTNode(ASTNodeType type, const TokenLocation& location = TokenLocation())
        : nodeType(type), location(location) {}
    
    virtual ~ASTNode() = default;
    
    // 获取节点类型
    ASTNodeType getType() const { return nodeType; }
    
    // 获取位置信息
    const TokenLocation& getLocation() const { return location; }
    void setLocation(const TokenLocation& loc) { location = loc; }
    
    // 子节点管理
    void addChild(std::shared_ptr<ASTNode> child);
    void removeChild(size_t index);
    void insertChild(size_t index, std::shared_ptr<ASTNode> child);
    std::shared_ptr<ASTNode> getChild(size_t index) const;
    size_t getChildCount() const { return children.size(); }
    const std::vector<std::shared_ptr<ASTNode>>& getChildren() const { return children; }
    
    // 父节点管理
    void setParent(ASTNode* parent) { this->parent = parent; }
    ASTNode* getParent() const { return parent; }
    
    // 访问者模式
    virtual void accept(ASTVisitor& visitor) = 0;
    
    // 克隆节点
    virtual std::shared_ptr<ASTNode> clone() const = 0;
    
    // 调试支持
    virtual std::string toString() const;
    virtual void print(int indent = 0) const;
    
protected:
    ASTNodeType nodeType;
    TokenLocation location;
    std::vector<std::shared_ptr<ASTNode>> children;
    ASTNode* parent = nullptr;
};

// 根节点
class RootNode : public ASTNode {
public:
    RootNode() : ASTNode(ASTNodeType::ROOT) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 根节点特有方法
    void setFileName(const std::string& name) { fileName = name; }
    const std::string& getFileName() const { return fileName; }
    
private:
    std::string fileName;
};

// 元素节点
class ElementNode : public ASTNode {
public:
    ElementNode(const std::string& tagName, const TokenLocation& location = TokenLocation())
        : ASTNode(ASTNodeType::ELEMENT, location), tagName(tagName) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 元素特有方法
    const std::string& getTagName() const { return tagName; }
    void setTagName(const std::string& name) { tagName = name; }
    
    // 属性管理
    void addAttribute(std::shared_ptr<class AttributeNode> attr);
    std::vector<std::shared_ptr<class AttributeNode>> getAttributes() const;
    std::shared_ptr<class AttributeNode> getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    
    // 样式和脚本块
    void setStyleBlock(std::shared_ptr<class StyleBlockNode> style);
    std::shared_ptr<class StyleBlockNode> getStyleBlock() const;
    void setScriptBlock(std::shared_ptr<class ScriptBlockNode> script);
    std::shared_ptr<class ScriptBlockNode> getScriptBlock() const;
    
    // 自动化支持
    std::string getAutoClass() const { return autoClass; }
    void setAutoClass(const std::string& className) { autoClass = className; }
    std::string getAutoId() const { return autoId; }
    void setAutoId(const std::string& id) { autoId = id; }
    
private:
    std::string tagName;
    std::string autoClass;  // 自动添加的类名
    std::string autoId;     // 自动添加的ID
};

// 属性节点
class AttributeNode : public ASTNode {
public:
    AttributeNode(const std::string& name, const std::string& value,
                  const TokenLocation& location = TokenLocation())
        : ASTNode(ASTNodeType::ATTRIBUTE, location), name(name), value(value) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 属性特有方法
    const std::string& getName() const { return name; }
    const std::string& getValue() const { return value; }
    void setValue(const std::string& val) { value = val; }
    
    // CE对等式支持
    bool isColonSyntax() const { return useColon; }
    void setColonSyntax(bool colon) { useColon = colon; }
    
private:
    std::string name;
    std::string value;
    bool useColon = true;  // 默认使用冒号语法
};

// 文本节点
class TextNode : public ASTNode {
public:
    TextNode(const std::string& content, const TokenLocation& location = TokenLocation())
        : ASTNode(ASTNodeType::TEXT, location), content(content) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    const std::string& getContent() const { return content; }
    void setContent(const std::string& text) { content = text; }
    
    // 是否是无修饰字面量
    bool isUnquotedLiteral() const { return unquoted; }
    void setUnquotedLiteral(bool unquoted) { this->unquoted = unquoted; }
    
private:
    std::string content;
    bool unquoted = false;
};

// 访问者接口
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(RootNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    
    // 更多visit方法将在具体节点类中定义
};

} // namespace ast
} // namespace chtl

#endif // CHTL_AST_NODE_H