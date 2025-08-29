#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"

namespace CHTL {

// 脚本块类型
enum class ScriptBlockType {
    GLOBAL,     // 全局脚本块
    LOCAL       // 局部脚本块
};

// 脚本块节点
class ScriptNode : public ASTNode {
public:
    ScriptNode(ScriptBlockType blockType, const TokenLocation& location)
        : ASTNode(NodeType::SCRIPT_BLOCK, location), blockType_(blockType) {}
    
    ScriptBlockType getBlockType() const { return blockType_; }
    
    // 设置脚本内容
    void setContent(const std::string& content) {
        content_ = content;
    }
    
    const std::string& getContent() const { return content_; }
    
    // 添加语句节点（用于结构化的脚本）
    void addStatement(std::shared_ptr<ASTNode> statement) {
        statements_.push_back(statement);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getStatements() const {
        return statements_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return statements_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    ScriptBlockType blockType_;
    std::string content_;  // 原始脚本内容
    std::vector<std::shared_ptr<ASTNode>> statements_;  // 结构化语句
};

// 扩展访问者接口
class ScriptVisitor : public Visitor {
public:
    virtual void visitScriptNode(ScriptNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H