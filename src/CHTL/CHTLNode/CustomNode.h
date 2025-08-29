#ifndef CHTL_CUSTOM_NODE_H
#define CHTL_CUSTOM_NODE_H

#include "BaseNode.h"
#include <unordered_set>

namespace CHTL {

// 自定义类型
enum class CustomType {
    STYLE,      // @Style
    ELEMENT,    // @Element
    VAR         // @Var
};

// 自定义节点
class CustomNode : public ASTNode {
public:
    CustomNode(CustomType customType, const std::string& name, 
               const TokenLocation& location)
        : ASTNode(NodeType::CUSTOM, location), 
          customType_(customType), name_(name) {}
    
    CustomType getCustomType() const { return customType_; }
    const std::string& getName() const { return name_; }
    
    // 内容管理
    void setContent(std::shared_ptr<ASTNode> content) {
        content_ = content;
    }
    
    std::shared_ptr<ASTNode> getContent() const { return content_; }
    
    // 无值属性（用于自定义样式组）
    void addUnvaluedProperty(const std::string& property) {
        unvaluedProperties_.insert(property);
    }
    
    const std::unordered_set<std::string>& getUnvaluedProperties() const {
        return unvaluedProperties_;
    }
    
    // 继承管理
    void addInheritedCustom(const std::string& customName) {
        inheritedCustoms_.insert(customName);
    }
    
    const std::unordered_set<std::string>& getInheritedCustoms() const {
        return inheritedCustoms_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        if (content_) {
            return {content_};
        }
        return {};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    CustomType customType_;
    std::string name_;
    std::shared_ptr<ASTNode> content_;
    std::unordered_set<std::string> unvaluedProperties_;
    std::unordered_set<std::string> inheritedCustoms_;
};

// 自定义使用节点
class CustomUseNode : public ASTNode {
public:
    CustomUseNode(CustomType customType, const std::string& name,
                  const TokenLocation& location)
        : ASTNode(NodeType::FUNCTION_CALL, location),
          customType_(customType), name_(name) {}
    
    CustomType getCustomType() const { return customType_; }
    const std::string& getName() const { return name_; }
    
    // 特例化内容
    void setSpecializationContent(std::shared_ptr<ASTNode> content) {
        specializationContent_ = content;
    }
    
    std::shared_ptr<ASTNode> getSpecializationContent() const {
        return specializationContent_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        if (specializationContent_) {
            return {specializationContent_};
        }
        return {};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    CustomType customType_;
    std::string name_;
    std::shared_ptr<ASTNode> specializationContent_;
};

// 扩展访问者接口
class CustomVisitor : public Visitor {
public:
    virtual void visitCustomNode(CustomNode* node) = 0;
    virtual void visitCustomUseNode(CustomUseNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_CUSTOM_NODE_H