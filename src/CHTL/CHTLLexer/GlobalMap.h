#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <optional>

namespace CHTL {

// 前向声明
class TemplateInfo;
class CustomInfo;
class OriginInfo;
class NamespaceInfo;
class ConfigurationInfo;

// 全局符号类型
enum class SymbolType {
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    ORIGIN_EMBED,
    NAMESPACE,
    CONFIGURATION,
    IMPORTED_SYMBOL
};

// 符号信息基类
class SymbolInfo {
public:
    SymbolInfo(const std::string& name, SymbolType type, const std::string& file)
        : name_(name), type_(type), sourceFile_(file) {}
    virtual ~SymbolInfo() = default;
    
    const std::string& getName() const { return name_; }
    SymbolType getType() const { return type_; }
    const std::string& getSourceFile() const { return sourceFile_; }
    
protected:
    std::string name_;
    SymbolType type_;
    std::string sourceFile_;
};

// 模板信息
class TemplateInfo : public SymbolInfo {
public:
    enum class TemplateKind { STYLE, ELEMENT, VAR };
    
    TemplateInfo(const std::string& name, TemplateKind kind, const std::string& file)
        : SymbolInfo(name, getSymbolType(kind), file), kind_(kind) {}
    
    TemplateKind getKind() const { return kind_; }
    
    // 添加继承的模板
    void addInheritedTemplate(const std::string& templateName) {
        inheritedTemplates_.push_back(templateName);
    }
    
    const std::vector<std::string>& getInheritedTemplates() const {
        return inheritedTemplates_;
    }
    
private:
    static SymbolType getSymbolType(TemplateKind kind) {
        switch (kind) {
            case TemplateKind::STYLE: return SymbolType::TEMPLATE_STYLE;
            case TemplateKind::ELEMENT: return SymbolType::TEMPLATE_ELEMENT;
            case TemplateKind::VAR: return SymbolType::TEMPLATE_VAR;
        }
        return SymbolType::TEMPLATE_STYLE;
    }
    
    TemplateKind kind_;
    std::vector<std::string> inheritedTemplates_;
};

// 自定义信息
class CustomInfo : public SymbolInfo {
public:
    enum class CustomKind { STYLE, ELEMENT, VAR };
    
    CustomInfo(const std::string& name, CustomKind kind, const std::string& file)
        : SymbolInfo(name, getSymbolType(kind), file), kind_(kind) {}
    
    CustomKind getKind() const { return kind_; }
    
    // 无值样式组支持
    void addUnvaluedProperty(const std::string& prop) {
        unvaluedProperties_.insert(prop);
    }
    
    bool hasUnvaluedProperty(const std::string& prop) const {
        return unvaluedProperties_.find(prop) != unvaluedProperties_.end();
    }
    
private:
    static SymbolType getSymbolType(CustomKind kind) {
        switch (kind) {
            case CustomKind::STYLE: return SymbolType::CUSTOM_STYLE;
            case CustomKind::ELEMENT: return SymbolType::CUSTOM_ELEMENT;
            case CustomKind::VAR: return SymbolType::CUSTOM_VAR;
        }
        return SymbolType::CUSTOM_STYLE;
    }
    
    CustomKind kind_;
    std::unordered_set<std::string> unvaluedProperties_;
};

// 原始嵌入信息
class OriginInfo : public SymbolInfo {
public:
    OriginInfo(const std::string& name, const std::string& type, const std::string& file)
        : SymbolInfo(name, SymbolType::ORIGIN_EMBED, file), originType_(type) {}
    
    const std::string& getOriginType() const { return originType_; }
    
private:
    std::string originType_;  // @Html, @Style, @JavaScript, @Vue等
};

// 命名空间信息
class NamespaceInfo : public SymbolInfo {
public:
    NamespaceInfo(const std::string& name, const std::string& file)
        : SymbolInfo(name, SymbolType::NAMESPACE, file) {}
    
    // 添加子命名空间
    void addChildNamespace(const std::string& child) {
        childNamespaces_.insert(child);
    }
    
    // 添加符号到命名空间
    void addSymbol(const std::string& symbol, SymbolType type) {
        symbols_[symbol] = type;
    }
    
    // 获取符号
    std::optional<SymbolType> getSymbol(const std::string& symbol) const {
        auto it = symbols_.find(symbol);
        if (it != symbols_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
private:
    std::unordered_set<std::string> childNamespaces_;
    std::unordered_map<std::string, SymbolType> symbols_;
};

// 配置信息
class ConfigurationInfo : public SymbolInfo {
public:
    ConfigurationInfo(const std::string& name, const std::string& file)
        : SymbolInfo(name, SymbolType::CONFIGURATION, file) {}
    
    void setProperty(const std::string& key, const std::string& value) {
        properties_[key] = value;
    }
    
    std::optional<std::string> getProperty(const std::string& key) const {
        auto it = properties_.find(key);
        if (it != properties_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    const std::unordered_map<std::string, std::string>& getProperties() const {
        return properties_;
    }
    
private:
    std::unordered_map<std::string, std::string> properties_;
};

// 全局符号映射表
class GlobalMap {
public:
    static GlobalMap& getInstance() {
        static GlobalMap instance;
        return instance;
    }
    
    // 注册模板
    void registerTemplate(const std::string& name, TemplateInfo::TemplateKind kind, 
                         const std::string& file);
    
    // 注册自定义
    void registerCustom(const std::string& name, CustomInfo::CustomKind kind,
                       const std::string& file);
    
    // 注册原始嵌入
    void registerOrigin(const std::string& name, const std::string& type,
                       const std::string& file);
    
    // 注册命名空间
    void registerNamespace(const std::string& name, const std::string& file);
    
    // 注册配置
    void registerConfiguration(const std::string& name, const std::string& file);
    
    // 查找符号
    std::shared_ptr<SymbolInfo> lookupSymbol(const std::string& name) const;
    
    // 查找带命名空间的符号
    std::shared_ptr<SymbolInfo> lookupSymbol(const std::string& name, 
                                            const std::string& namespacePath) const;
    
    // 获取当前文件的默认命名空间
    std::string getDefaultNamespace(const std::string& file) const;
    
    // 设置当前文件的默认命名空间
    void setDefaultNamespace(const std::string& file, const std::string& ns);
    
    // 添加导入的符号
    void addImportedSymbol(const std::string& symbol, const std::string& fromFile,
                          const std::string& toFile, const std::string& alias = "");
    
    // 检查循环依赖
    bool hasCircularDependency(const std::string& file1, const std::string& file2) const;
    
    // 清空全局映射表
    void clear();
    
    // 获取所有注册的符号（用于调试）
    std::vector<std::string> getAllSymbols() const;

private:
    GlobalMap() = default;
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 符号表
    std::unordered_map<std::string, std::shared_ptr<SymbolInfo>> symbols_;
    
    // 命名空间映射
    std::unordered_map<std::string, std::shared_ptr<NamespaceInfo>> namespaces_;
    
    // 文件默认命名空间映射
    std::unordered_map<std::string, std::string> fileNamespaces_;
    
    // 导入关系图（用于检测循环依赖）
    std::unordered_map<std::string, std::unordered_set<std::string>> importGraph_;
    
    // 别名映射
    std::unordered_map<std::string, std::string> aliasMap_;
};

} // namespace CHTL

#endif // CHTL_GLOBAL_MAP_H