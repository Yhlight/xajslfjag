#ifndef CHTL_GLOBALMAP_H
#define CHTL_GLOBALMAP_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTL {

// 符号类型
enum class SymbolType {
    TEMPLATE_STYLE,     // 模板样式组
    TEMPLATE_ELEMENT,   // 模板元素
    TEMPLATE_VAR,       // 模板变量组
    CUSTOM_STYLE,       // 自定义样式组
    CUSTOM_ELEMENT,     // 自定义元素
    CUSTOM_VAR,         // 自定义变量组
    ORIGIN_HTML,        // 原始HTML嵌入
    ORIGIN_STYLE,       // 原始样式嵌入
    ORIGIN_JAVASCRIPT,  // 原始JavaScript嵌入
    ORIGIN_CUSTOM,      // 自定义原始嵌入
    NAMESPACE,          // 命名空间
    CONFIGURATION,      // 配置组
    HTML_ELEMENT,       // HTML元素
    CSS_CLASS,          // CSS类
    CSS_ID,             // CSS ID
    IMPORT_MODULE,      // 导入的模块
    CMOD_MODULE,        // CMOD模块
    CJMOD_MODULE        // CJMOD模块
};

// 符号信息
struct SymbolInfo {
    SymbolType type;
    std::string name;
    std::string namespace_name;  // 所属命名空间
    std::string file_path;       // 定义文件路径
    size_t line;                 // 定义行号
    size_t column;               // 定义列号
    std::unordered_map<std::string, std::string> attributes;  // 属性
    std::vector<std::string> dependencies;  // 依赖项
    bool is_exported;            // 是否导出
    
    SymbolInfo(SymbolType t = SymbolType::HTML_ELEMENT, 
               const std::string& n = "",
               const std::string& ns = "",
               const std::string& fp = "",
               size_t ln = 0,
               size_t col = 0,
               bool exported = false)
        : type(t), name(n), namespace_name(ns), file_path(fp), 
          line(ln), column(col), is_exported(exported) {}
};

// 全局符号表管理器
class GlobalMap {
private:
    // 符号表 - namespace -> symbol_name -> SymbolInfo
    std::unordered_map<std::string, std::unordered_map<std::string, SymbolInfo>> symbols;
    
    // HTML标签关键字集合
    std::unordered_set<std::string> html_tags;
    
    // CHTL关键字集合
    std::unordered_map<std::string, TokenType> keywords;
    
    // 配置组映射
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> configurations;
    
    // 当前活动的命名空间
    std::string current_namespace;
    
    // 当前文件路径
    std::string current_file;
    
    // 导入的模块
    std::unordered_map<std::string, std::vector<std::string>> imported_modules;
    
    void initializeHtmlTags();
    void initializeKeywords();
    
public:
    GlobalMap();
    ~GlobalMap() = default;
    
    // 符号管理
    bool addSymbol(const std::string& name, const SymbolInfo& info);
    bool addSymbol(const std::string& namespace_name, const std::string& name, const SymbolInfo& info);
    SymbolInfo* findSymbol(const std::string& name);
    SymbolInfo* findSymbol(const std::string& namespace_name, const std::string& name);
    bool removeSymbol(const std::string& name);
    bool removeSymbol(const std::string& namespace_name, const std::string& name);
    
    // 命名空间管理
    void setCurrentNamespace(const std::string& namespace_name);
    std::string getCurrentNamespace() const;
    void createNamespace(const std::string& namespace_name);
    bool namespaceExists(const std::string& namespace_name) const;
    std::vector<std::string> getAllNamespaces() const;
    
    // 关键字检查
    bool isHtmlTag(const std::string& tag) const;
    bool isKeyword(const std::string& word) const;
    TokenType getKeywordType(const std::string& word) const;
    
    // 配置管理
    void setConfiguration(const std::string& config_name, const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& config_name, const std::string& key) const;
    bool hasConfiguration(const std::string& config_name) const;
    
    // 文件管理
    void setCurrentFile(const std::string& file_path);
    std::string getCurrentFile() const;
    
    // 模块导入管理
    void addImportedModule(const std::string& module_name, const std::vector<std::string>& symbols);
    bool isModuleImported(const std::string& module_name) const;
    std::vector<std::string> getImportedSymbols(const std::string& module_name) const;
    
    // 依赖检查
    bool hasCyclicDependency(const std::string& symbol_name, const std::string& dependency) const;
    std::vector<std::string> getDependencies(const std::string& symbol_name) const;
    
    // 导出管理
    void markAsExported(const std::string& symbol_name);
    bool isExported(const std::string& symbol_name) const;
    std::vector<SymbolInfo> getExportedSymbols() const;
    
    // 清理
    void clear();
    void clearNamespace(const std::string& namespace_name);
    
    // 调试支持
    void printSymbolTable() const;
    size_t getSymbolCount() const;
    size_t getNamespaceCount() const;
};

} // namespace CHTL

#endif // CHTL_GLOBALMAP_H