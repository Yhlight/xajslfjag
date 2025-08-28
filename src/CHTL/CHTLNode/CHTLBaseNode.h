#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../CHTLLexer/CHTLToken.h"

namespace CHTL {

// 前向声明
class CHTLBaseNode;
class CHTLElementNode;
class CHTLTextNode;

// AST节点类型枚举
enum class CHTLNodeType {
    BASE = 0,
    ELEMENT,
    TEXT,
    COMMENT,
    TEMPLATE,
    CUSTOM,
    STYLE,
    SCRIPT,
    ORIGIN,
    IMPORT,
    CONFIG,
    NAMESPACE,
    OPERATOR
};

// AST节点访问者接口
class CHTLNodeVisitor {
public:
    virtual ~CHTLNodeVisitor() = default;
    virtual void visit(class CHTLElementNode* node) = 0;
    virtual void visit(class CHTLTextNode* node) = 0;
    virtual void visit(class CHTLCommentNode* node) = 0;
    virtual void visit(class CHTLTemplateNode* node) = 0;
    virtual void visit(class CHTLCustomNode* node) = 0;
    virtual void visit(class CHTLStyleNode* node) = 0;
    virtual void visit(class CHTLScriptNode* node) = 0;
    virtual void visit(class CHTLOriginNode* node) = 0;
    virtual void visit(class CHTLImportNode* node) = 0;
    virtual void visit(class CHTLConfigNode* node) = 0;
    virtual void visit(class CHTLNamespaceNode* node) = 0;
    virtual void visit(class CHTLOperatorNode* node) = 0;
};

// AST基础节点类
class CHTLBaseNode : public std::enable_shared_from_this<CHTLBaseNode> {
public:
    CHTLBaseNode(CHTLNodeType type = CHTLNodeType::BASE);
    virtual ~CHTLBaseNode() = default;
    
    // 获取节点类型
    CHTLNodeType getNodeType() const { return nodeType; }
    
    // 获取节点类型字符串
    virtual std::string getNodeTypeName() const;
    
    // 父子关系管理
    void addChild(std::shared_ptr<CHTLBaseNode> child);
    void removeChild(std::shared_ptr<CHTLBaseNode> child);
    void removeChildAt(size_t index);
    void insertChild(size_t index, std::shared_ptr<CHTLBaseNode> child);
    
    // 获取子节点
    const std::vector<std::shared_ptr<CHTLBaseNode>>& getChildren() const { return children; }
    std::shared_ptr<CHTLBaseNode> getChild(size_t index) const;
    size_t getChildCount() const { return children.size(); }
    
    // 父节点管理
    void setParent(std::shared_ptr<CHTLBaseNode> parent) { this->parent = parent; }
    std::shared_ptr<CHTLBaseNode> getParent() const { return parent.lock(); }
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    void removeAttribute(const std::string& name);
    std::string getAttribute(const std::string& name, const std::string& defaultValue = "") const;
    bool hasAttribute(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getAttributes() const { return attributes; }
    
    // 位置信息
    void setSourcePosition(size_t line, size_t column);
    size_t getSourceLine() const { return sourceLine; }
    size_t getSourceColumn() const { return sourceColumn; }
    
    // Token信息
    void setToken(CHTLTokenPtr token) { this->token = token; }
    CHTLTokenPtr getToken() const { return token; }
    
    // 访问者模式
    virtual void accept(CHTLNodeVisitor* visitor) = 0;
    
    // 深度复制
    virtual std::shared_ptr<CHTLBaseNode> clone() const = 0;
    
    // 调试输出
    virtual std::string toString() const;
    std::string toTreeString(int indent = 0) const;
    
    // 查找节点
    std::vector<std::shared_ptr<CHTLBaseNode>> findNodesByType(CHTLNodeType type) const;
    std::shared_ptr<CHTLBaseNode> findFirstNodeByType(CHTLNodeType type) const;
    
    // 转换为特定类型
    template<typename T>
    std::shared_ptr<T> as() {
        return std::dynamic_pointer_cast<T>(shared_from_this());
    }
    
    template<typename T>
    std::shared_ptr<const T> as() const {
        return std::dynamic_pointer_cast<const T>(shared_from_this());
    }

protected:
    CHTLNodeType nodeType;
    std::vector<std::shared_ptr<CHTLBaseNode>> children;
    std::weak_ptr<CHTLBaseNode> parent;
    std::unordered_map<std::string, std::string> attributes;
    size_t sourceLine = 0;
    size_t sourceColumn = 0;
    CHTLTokenPtr token;
    
    // 内部辅助方法
    virtual void findNodesRecursive(CHTLNodeType type, std::vector<std::shared_ptr<CHTLBaseNode>>& result) const;
    virtual std::string getIndentString(int indent) const;

};

using CHTLNodePtr = std::shared_ptr<CHTLBaseNode>;
using CHTLNodeList = std::vector<CHTLNodePtr>;

} // namespace CHTL