#ifndef CHTL_IMPORT_NODE_H
#define CHTL_IMPORT_NODE_H

#include "BaseNode.h"
#include <optional>

namespace CHTL {

// 导入类型
enum class ImportType {
    HTML,           // @Html
    STYLE,          // @Style
    JAVASCRIPT,     // @JavaScript
    CHTL,           // @Chtl
    CJMOD,          // @CJmod
    CONFIG,         // @Config
    TEMPLATE_STYLE, // [Template] @Style
    TEMPLATE_ELEMENT, // [Template] @Element
    TEMPLATE_VAR,   // [Template] @Var
    CUSTOM_STYLE,   // [Custom] @Style
    CUSTOM_ELEMENT, // [Custom] @Element
    CUSTOM_VAR,     // [Custom] @Var
    ORIGIN,         // [Origin] @Type
    ALL_TEMPLATE,   // [Template]
    ALL_CUSTOM,     // [Custom]
    ALL_ORIGIN      // [Origin]
};

// 导入节点
class ImportNode : public ASTNode {
public:
    ImportNode(ImportType importType, const std::string& fromPath, const TokenLocation& location)
        : ASTNode(NodeType::IMPORT, location), importType_(importType), fromPath_(fromPath) {}
    
    ImportType getImportType() const { return importType_; }
    
    // 设置导入的具体项（可选）
    void setImportItem(const std::string& item) {
        importItem_ = item;
    }
    
    void setTargetName(const std::string& targetName) {
        importItem_ = targetName;
    }
    
    const std::optional<std::string>& getImportItem() const {
        return importItem_;
    }
    
    // 设置源路径
    void setFromPath(const std::string& path) {
        fromPath_ = path;
    }
    
    const std::string& getFromPath() const { return fromPath_; }
    
    // 设置别名（可选）
    void setAlias(const std::string& alias) {
        alias_ = alias;
    }
    
    void setAsName(const std::string& asName) {
        alias_ = asName;
    }
    
    const std::optional<std::string>& getAlias() const {
        return alias_;
    }
    
    // 设置except列表（用于排除某些项）
    void addExcept(const std::string& item) {
        exceptItems_.push_back(item);
    }
    
    const std::vector<std::string>& getExceptItems() const {
        return exceptItems_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    ImportType importType_;
    std::optional<std::string> importItem_;  // 具体导入项
    std::string fromPath_;                   // 源路径
    std::optional<std::string> alias_;       // 别名
    std::vector<std::string> exceptItems_;   // 排除项
};

// 扩展访问者接口
class ImportVisitor : public Visitor {
public:
    virtual void visitImportNode(ImportNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_IMPORT_NODE_H