#pragma once

#include "NodeType.h"
#include "../../Core/Types.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace AST {

/**
 * 前向声明
 */
class ASTVisitor;

/**
 * CHTL AST基础节点
 * 所有AST节点的基类
 */
class ASTNode {
protected:
    NodeType type_;
    SourceRange range_;
    std::vector<std::shared_ptr<ASTNode>> children_;
    std::unordered_map<std::string, std::string> attributes_;
    
public:
    explicit ASTNode(NodeType type) : type_(type) {}
    virtual ~ASTNode() = default;
    
    // 基本访问器
    NodeType getType() const { return type_; }
    const SourceRange& getRange() const { return range_; }
    void setRange(const SourceRange& range) { range_ = range; }
    
    // 子节点管理
    const std::vector<std::shared_ptr<ASTNode>>& getChildren() const { return children_; }
    void addChild(std::shared_ptr<ASTNode> child) { children_.push_back(child); }
    void removeChild(size_t index) { 
        if (index < children_.size()) {
            children_.erase(children_.begin() + index);
        }
    }
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value) {
        attributes_[name] = value;
    }
    std::string getAttribute(const std::string& name) const {
        auto it = attributes_.find(name);
        return it != attributes_.end() ? it->second : "";
    }
    bool hasAttribute(const std::string& name) const {
        return attributes_.find(name) != attributes_.end();
    }
    const std::unordered_map<std::string, std::string>& getAttributes() const {
        return attributes_;
    }
    
    // 访问者模式
    virtual void accept(ASTVisitor& visitor) = 0;
    
    // 调试输出
    virtual std::string toString() const;
    std::string toTreeString(int indent = 0) const;
    
    // 类型检查便利方法
    bool isElement() const { return type_ == NodeType::ELEMENT; }
    bool isText() const { return type_ == NodeType::TEXT; }
    bool isStyle() const { return type_ == NodeType::STYLE_BLOCK; }
    bool isScript() const { return type_ == NodeType::SCRIPT_BLOCK; }
    bool isTemplate() const { 
        return type_ == NodeType::TEMPLATE_ELEMENT || 
               type_ == NodeType::TEMPLATE_STYLE || 
               type_ == NodeType::TEMPLATE_VAR; 
    }
    bool isCustom() const { 
        return type_ == NodeType::CUSTOM_ELEMENT || 
               type_ == NodeType::CUSTOM_STYLE || 
               type_ == NodeType::CUSTOM_VAR; 
    }
    bool isImport() const {
        return type_ == NodeType::IMPORT_HTML ||
               type_ == NodeType::IMPORT_STYLE ||
               type_ == NodeType::IMPORT_JAVASCRIPT ||
               type_ == NodeType::IMPORT_CHTL ||
               type_ == NodeType::IMPORT_CJMOD ||
               type_ == NodeType::IMPORT_SPECIFIC;
    }
};

/**
 * 访问者模式接口
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 基础节点访问
    virtual void visitRoot(ASTNode* node) {}
    virtual void visitElement(ASTNode* node) {}
    virtual void visitText(ASTNode* node) {}
    virtual void visitComment(ASTNode* node) {}
    
    // 属性访问
    virtual void visitAttribute(ASTNode* node) {}
    virtual void visitAttributeList(ASTNode* node) {}
    
    // 样式访问
    virtual void visitStyleBlock(ASTNode* node) {}
    virtual void visitStyleRule(ASTNode* node) {}
    virtual void visitStyleProperty(ASTNode* node) {}
    virtual void visitCSSSelector(ASTNode* node) {}
    
    // 脚本访问
    virtual void visitScriptBlock(ASTNode* node) {}
    
    // 模板访问
    virtual void visitTemplateElement(ASTNode* node) {}
    virtual void visitTemplateStyle(ASTNode* node) {}
    virtual void visitTemplateVar(ASTNode* node) {}
    
    // 自定义访问
    virtual void visitCustomElement(ASTNode* node) {}
    virtual void visitCustomStyle(ASTNode* node) {}
    virtual void visitCustomVar(ASTNode* node) {}
    
    // 导入访问
    virtual void visitImportHTML(ASTNode* node) {}
    virtual void visitImportStyle(ASTNode* node) {}
    virtual void visitImportJavaScript(ASTNode* node) {}
    virtual void visitImportCHTL(ASTNode* node) {}
    virtual void visitImportCJMOD(ASTNode* node) {}
    virtual void visitImportSpecific(ASTNode* node) {}
    
    // 其他访问
    virtual void visitNamespace(ASTNode* node) {}
    virtual void visitConfiguration(ASTNode* node) {}
    virtual void visitOriginBlock(ASTNode* node) {}
    virtual void visitVarGroup(ASTNode* node) {}
    virtual void visitLiteral(ASTNode* node) {}
    virtual void visitNodeList(ASTNode* node) {}
    
    // 通用访问方法
    virtual void visit(ASTNode* node);
};

} // namespace AST
} // namespace CHTL