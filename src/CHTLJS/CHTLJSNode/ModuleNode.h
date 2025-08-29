#ifndef CHTLJS_MODULE_NODE_H
#define CHTLJS_MODULE_NODE_H

#include "BaseNode.h"
#include <vector>
#include <map>
#include <string>

namespace CHTLJS {

// 模块节点（module {}）
class ModuleNode : public ASTNode {
public:
    ModuleNode(const TokenLocation& location)
        : ASTNode(NodeType::MODULE_BLOCK, location) {}
    
    // 加载项管理
    void addLoadItem(const std::string& path) {
        loadItems_.push_back(path);
    }
    
    const std::vector<std::string>& getLoadItems() const {
        return loadItems_;
    }
    
    // 导出管理
    void addExport(const std::string& name, const std::string& value);
    const std::map<std::string, std::string>& getExports() const {
        return exports_;
    }
    bool hasExports() const { return !exports_.empty(); }
    
    // 配置管理
    void setModuleType(const std::string& type);
    std::string getModuleType() const { return moduleType_; }
    
    void setAsync(bool async) { async_ = async; }
    bool isAsync() const { return async_; }
    
    void setBaseUrl(const std::string& url) { baseUrl_ = url; }
    std::string getBaseUrl() const { return baseUrl_; }
    
    // 工具方法
    static std::string toVariableName(const std::string& path);
    static bool isRelativePath(const std::string& path);
    static std::string normalizePath(const std::string& path);
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::vector<std::string> loadItems_;
    std::map<std::string, std::string> exports_;
    std::string moduleType_ = "amd";
    bool async_ = false;
    std::string baseUrl_ = "";
};

// 扩展访问者接口
class ModuleVisitor : public Visitor {
public:
    virtual void visitModuleNode(ModuleNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_MODULE_NODE_H