#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <optional>

namespace CHTL {

// 前向声明
class CHTLNode;

// 符号类型
enum class SymbolType {
    TEMPLATE_VAR,          // 模板变量
    CUSTOM_VAR,            // 自定义变量
    TEMPLATE_STYLE_GROUP,  // 模板样式组
    CUSTOM_STYLE_GROUP,    // 自定义样式组
    NO_VALUE_STYLE_GROUP,  // 无值样式组
    NAMESPACE,             // 命名空间
    IMPORT,                // 导入项
    ORIGIN_EMBED,          // 原始嵌入
    ELEMENT,               // HTML元素
    CLASS_NAME,            // 类名
    ID_NAME                // ID名
};

// 符号信息
struct SymbolInfo {
    SymbolType type;
    std::string name;
    std::string value;
    std::string namespace_name;  // 所属命名空间
    std::string source_file;      // 来源文件
    int line;
    int column;
    std::shared_ptr<CHTLNode> node;  // 关联的AST节点
    
    SymbolInfo() : line(0), column(0) {}
};

// 导入信息
struct ImportInfo {
    std::string type;        // @Chtl, @CJmod, @Html等
    std::string path;        // 导入路径
    std::string alias;       // as别名
    bool is_origin;          // 是否是[Origin]导入
    bool is_wildcard;        // 是否是通配符导入
    std::string namespace_name;  // 目标命名空间
    int line;
    int column;
    
    ImportInfo() : is_origin(false), is_wildcard(false), line(0), column(0) {}
};

// 命名空间信息
struct NamespaceInfo {
    std::string name;
    std::string parent;      // 父命名空间
    std::vector<std::string> children;  // 子命名空间
    std::unordered_map<std::string, SymbolInfo> symbols;  // 命名空间内的符号
    std::string source_file;
    bool is_merged;          // 是否是合并的命名空间
    
    NamespaceInfo() : is_merged(false) {}
};

// 配置项
struct ConfigurationInfo {
    bool disable_style_auto_add_class = false;
    bool disable_style_auto_add_id = false;
    bool disable_script_auto_add_class = true;
    bool disable_script_auto_add_id = true;
    bool disable_default_namespace = false;
};

class CHTLGlobalMap {
private:
    // 符号表
    std::unordered_map<std::string, SymbolInfo> symbols;
    
    // 命名空间表
    std::unordered_map<std::string, NamespaceInfo> namespaces;
    
    // 导入表
    std::vector<ImportInfo> imports;
    
    // 已导入文件集合（防止循环依赖）
    std::unordered_set<std::string> imported_files;
    
    // 配置信息
    ConfigurationInfo configuration;
    
    // 当前命名空间栈
    std::vector<std::string> namespace_stack;
    
    // 类名和ID使用统计
    std::unordered_map<std::string, int> class_usage;
    std::unordered_map<std::string, int> id_usage;

public:
    CHTLGlobalMap() = default;
    ~CHTLGlobalMap() = default;
    
    // 符号管理
    bool addSymbol(const std::string& name, const SymbolInfo& info);
    bool hasSymbol(const std::string& name) const;
    std::optional<SymbolInfo> getSymbol(const std::string& name) const;
    bool removeSymbol(const std::string& name);
    
    // 命名空间管理
    bool addNamespace(const std::string& name, const NamespaceInfo& info);
    bool hasNamespace(const std::string& name) const;
    std::optional<NamespaceInfo> getNamespace(const std::string& name) const;
    void enterNamespace(const std::string& name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    std::string getFullyQualifiedName(const std::string& name) const;
    
    // 导入管理
    void addImport(const ImportInfo& info);
    std::vector<ImportInfo> getImports() const { return imports; }
    bool isFileImported(const std::string& file) const;
    void markFileImported(const std::string& file);
    
    // 配置管理
    void setConfiguration(const ConfigurationInfo& config) { configuration = config; }
    ConfigurationInfo getConfiguration() const { return configuration; }
    
    // 类名和ID管理
    void registerClassName(const std::string& className);
    void registerIdName(const std::string& idName);
    int getClassUsageCount(const std::string& className) const;
    int getIdUsageCount(const std::string& idName) const;
    std::string getMostUsedClass() const;
    std::string getMostUsedId() const;
    
    // 符号查找（支持命名空间）
    std::optional<SymbolInfo> findSymbol(const std::string& name, 
                                         const std::string& namespace_hint = "") const;
    
    // 合并命名空间（同名命名空间合并）
    bool mergeNamespace(const std::string& name, const NamespaceInfo& info);
    
    // 冲突检测
    bool hasConflict(const std::string& name, SymbolType type) const;
    std::vector<std::string> detectConflicts() const;
    
    // 清理
    void clear();
    void clearNamespace(const std::string& name);
    
    // 调试输出
    void dump() const;
};

// 全局单例访问
CHTLGlobalMap& getGlobalMap();

} // namespace CHTL

#endif // CHTL_GLOBAL_MAP_H