#pragma once

#include "BaseNode.h"
#include <vector>

namespace CHTL {

// 导入类型
enum class ImportTargetType {
    HTML,           // @Html
    STYLE,          // @Style  
    JAVASCRIPT,     // @JavaScript
    CHTL,           // @Chtl
    CJMOD,          // @CJmod
    CONFIG,         // @Config
    CUSTOM,         // [Custom] @Type Name
    TEMPLATE,       // [Template] @Type Name
    ORIGIN          // [Origin] @Type Name
};

// 导入节点
class ImportNode : public BaseNode {
public:
    ImportNode();
    
    // 设置导入信息
    void SetTargetType(ImportTargetType type) { targetType = type; }
    void SetItemType(const std::string& type) { itemType = type; }  // @Style, @Element等
    void SetItemName(const std::string& name) { itemName = name; }
    void SetFromPath(const std::string& path) { fromPath = path; }
    void SetAsName(const std::string& name) { asName = name; }
    
    // 获取导入信息
    ImportTargetType GetTargetType() const { return targetType; }
    std::string GetItemType() const { return itemType; }
    std::string GetItemName() const { return itemName; }
    std::string GetFromPath() const { return fromPath; }
    std::string GetAsName() const { return asName; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 导入验证
    bool IsValid() const;
    bool RequiresAsName() const;
    
    // 路径处理
    std::string GetNormalizedPath() const;
    std::vector<std::string> GetSearchPaths() const;
    
    // 判断导入类型
    bool IsWildcardImport() const;  // 导入所有项
    bool IsSpecificImport() const;  // 导入特定项
    
private:
    ImportTargetType targetType;
    std::string itemType;    // 具体类型（如@Style, @Element）
    std::string itemName;    // 具体名称（可为空表示导入所有）
    std::string fromPath;    // 来源路径
    std::string asName;      // 重命名（可选）
    
    // 根据目标类型获取文件扩展名
    std::vector<std::string> GetExpectedExtensions() const;
};

} // namespace CHTL