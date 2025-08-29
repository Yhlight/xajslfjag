#ifndef CHTL_ORIGIN_NODE_H
#define CHTL_ORIGIN_NODE_H

#include "BaseNode.h"

namespace CHTL {

// 原始嵌入类型
enum class OriginType {
    HTML,
    STYLE,
    JAVASCRIPT,
    CUSTOM
};

// 原始嵌入节点
class OriginNode : public ASTNode {
public:
    OriginNode(OriginType type, const std::string& name,
               const TokenLocation& location)
        : ASTNode(NodeType::ORIGIN, location), type_(type), name_(name) {}
    
    OriginType getOriginType() const { return type_; }
    const std::string& getName() const { return name_; }
    
    // 设置原始内容
    void setContent(const std::string& content) {
        content_ = content;
    }
    
    const std::string& getContent() const { return content_; }
    
    // 设置自定义类型名称（仅用于CUSTOM类型）
    void setCustomType(const std::string& customType) {
        customType_ = customType;
    }
    
    const std::string& getCustomType() const { return customType_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    OriginType type_;      // 原始嵌入类型
    std::string name_;     // 命名（可选）
    std::string content_;  // 原始内容
    std::string customType_; // 自定义类型名称（如@Vue）
};

// 原始嵌入使用节点
class OriginUseNode : public ASTNode {
public:
    OriginUseNode(const std::string& type, const std::string& name,
                  const TokenLocation& location)
        : ASTNode(NodeType::FUNCTION_CALL, location), type_(type), name_(name) {}
    
    const std::string& getOriginType() const { return type_; }
    const std::string& getName() const { return name_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string type_;
    std::string name_;
};

// 扩展访问者接口
class OriginVisitor : public Visitor {
public:
    virtual void visitOriginNode(OriginNode* node) = 0;
    virtual void visitOriginUseNode(OriginUseNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_ORIGIN_NODE_H