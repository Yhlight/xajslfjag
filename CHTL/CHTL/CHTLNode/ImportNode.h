#ifndef CHTL_IMPORT_NODE_H
#define CHTL_IMPORT_NODE_H

#include "BaseNode.h"

namespace CHTL {

// 导入类型枚举
enum class ImportCategory {
    FILE,       // 文件导入 (@Html, @Style, @JavaScript, @Chtl, @CJmod)
    CUSTOM,     // 自定义导入 ([Custom])
    TEMPLATE,   // 模板导入 ([Template])
    ORIGIN,     // 原始嵌入导入 ([Origin])
    CONFIG      // 配置导入 ([Configuration])
};

// 导入节点类
class ImportNode : public BaseNode {
private:
    ImportCategory category;
    std::string importType;      // @Html, @Style, @Element等
    std::string importName;      // 要导入的具体名称（可选）
    std::string fromPath;        // 导入路径
    std::string asName;          // 重命名（可选）
    bool importAll;              // 是否导入所有（如[Import] [Custom] from path）
    std::string prefix;          // 前缀（如[Custom], [Template]等）
    
public:
    ImportNode(ImportCategory cat, const std::string& type, int line = 0, int col = 0)
        : BaseNode(NodeType::IMPORT, line, col), category(cat), importType(type), importAll(false) {}
    
    // 类别访问
    ImportCategory getCategory() const { return category; }
    void setCategory(ImportCategory cat) { category = cat; }
    
    // 类型访问
    const std::string& getImportType() const { return importType; }
    void setImportType(const std::string& type) { importType = type; }
    
    // 名称访问
    const std::string& getImportName() const { return importName; }
    void setImportName(const std::string& name) { importName = name; }
    bool hasImportName() const { return !importName.empty(); }
    
    // 路径访问
    const std::string& getFromPath() const { return fromPath; }
    void setFromPath(const std::string& path) { fromPath = path; }
    
    // 重命名访问
    const std::string& getAsName() const { return asName; }
    void setAsName(const std::string& name) { asName = name; }
    bool hasAsName() const { return !asName.empty(); }
    
    // 导入所有标志
    bool getImportAll() const { return importAll; }
    void setImportAll(bool all) { importAll = all; }
    
    // 前缀访问
    const std::string& getPrefix() const { return prefix; }
    void setPrefix(const std::string& p) { prefix = p; }
    
    // 解析路径（处理'.'到'/'的转换）
    std::string getResolvedPath() const;
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class ImportNodeVisitor : public NodeVisitor {
public:
    virtual void visitImportNode(ImportNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_IMPORT_NODE_H