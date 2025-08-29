#ifndef CHTL_CONFIG_NODE_H
#define CHTL_CONFIG_NODE_H

#include "BaseNode.h"
#include <unordered_map>

namespace CHTL {

// 配置节点
class ConfigNode : public ASTNode {
public:
    ConfigNode(const std::string& name, const TokenLocation& location)
        : ASTNode(NodeType::CONFIGURATION, location), name_(name) {}
    
    const std::string& getName() const { return name_; }
    
    // 添加配置项
    void addProperty(const std::string& key, const std::string& value) {
        properties_[key] = value;
    }
    
    const std::unordered_map<std::string, std::string>& getProperties() const {
        return properties_;
    }
    
    std::optional<std::string> getProperty(const std::string& key) const {
        auto it = properties_.find(key);
        if (it != properties_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    // 添加子配置块（如[Name], [OriginType]）
    void addSubConfig(const std::string& name, std::shared_ptr<ConfigNode> config) {
        subConfigs_[name] = config;
    }
    
    const std::unordered_map<std::string, std::shared_ptr<ConfigNode>>& getSubConfigs() const {
        return subConfigs_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        std::vector<std::shared_ptr<ASTNode>> children;
        for (const auto& [_, config] : subConfigs_) {
            children.push_back(config);
        }
        return children;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string name_;  // 配置名称（空字符串表示默认配置）
    std::unordered_map<std::string, std::string> properties_;
    std::unordered_map<std::string, std::shared_ptr<ConfigNode>> subConfigs_;
};

// Info节点（用于CMOD信息）
class InfoNode : public ASTNode {
public:
    InfoNode(const TokenLocation& location)
        : ASTNode(NodeType::INFO, location) {}
    
    // 添加信息项
    void addProperty(const std::string& key, const std::string& value) {
        properties_[key] = value;
    }
    
    const std::unordered_map<std::string, std::string>& getProperties() const {
        return properties_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::unordered_map<std::string, std::string> properties_;
};

// Export节点（用于CMOD导出）
class ExportNode : public ASTNode {
public:
    ExportNode(const TokenLocation& location)
        : ASTNode(NodeType::EXPORT, location) {}
    
    // 添加导出项
    void addExport(const std::string& type, const std::vector<std::string>& items) {
        exports_[type] = items;
    }
    
    const std::unordered_map<std::string, std::vector<std::string>>& getExports() const {
        return exports_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::unordered_map<std::string, std::vector<std::string>> exports_;
};

// 扩展访问者接口
class ConfigVisitor : public Visitor {
public:
    virtual void visitConfigNode(ConfigNode* node) = 0;
    virtual void visitInfoNode(InfoNode* node) = 0;
    virtual void visitExportNode(ExportNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_CONFIG_NODE_H