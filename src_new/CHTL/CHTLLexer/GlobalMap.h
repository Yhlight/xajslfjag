#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace CHTL {
namespace Lexer {

// 前向声明
class CHTLContext;

// 符号类型
enum class SymbolType {
    TEMPLATE_STYLE,      // [Template] @Style
    TEMPLATE_ELEMENT,    // [Template] @Element  
    TEMPLATE_VAR,        // [Template] @Var
    CUSTOM_STYLE,        // [Custom] @Style
    CUSTOM_ELEMENT,      // [Custom] @Element
    CUSTOM_VAR,          // [Custom] @Var
    ORIGIN_HTML,         // [Origin] @Html
    ORIGIN_STYLE,        // [Origin] @Style
    ORIGIN_JAVASCRIPT,   // [Origin] @JavaScript
    ORIGIN_CUSTOM,       // [Origin] @CustomType
    CONFIGURATION,       // [Configuration]
    NAMESPACE,           // [Namespace]
    HTML_ELEMENT,        // HTML元素
    VARIABLE,            // 变量
    FUNCTION,            // 函数
    CLASS_NAME,          // 自动生成的类名
    ID_NAME,             // 自动生成的ID名
    UNKNOWN
};

// 符号信息
struct Symbol {
    std::string name;
    SymbolType type;
    std::string namespace_path;  // 命名空间路径
    std::string file_path;       // 定义文件路径
    size_t line;                 // 定义行号
    std::string content;         // 符号内容
    std::unordered_map<std::string, std::string> attributes; // 属性映射
    
    Symbol() : type(SymbolType::UNKNOWN), line(0) {}
    Symbol(const std::string& n, SymbolType t, const std::string& ns = "", 
           const std::string& file = "", size_t l = 0)
        : name(n), type(t), namespace_path(ns), file_path(file), line(l) {}
};

// 命名空间管理
struct Namespace {
    std::string name;
    std::string parent_path;  // 父命名空间路径
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols;
    std::vector<std::string> children; // 子命名空间
    
    Namespace() = default;
    Namespace(const std::string& n, const std::string& parent = "")
        : name(n), parent_path(parent) {}
        
    std::string getFullPath() const {
        return parent_path.empty() ? name : parent_path + "." + name;
    }
};

// 文件依赖信息
struct FileDependency {
    std::string file_path;
    std::vector<std::string> imports;      // 导入的文件
    std::vector<std::string> dependencies; // 依赖的文件
    bool is_processed;                     // 是否已处理
    
    FileDependency() : is_processed(false) {}
    FileDependency(const std::string& path) : file_path(path), is_processed(false) {}
};

// 全局映射管理器
class GlobalMap {
public:
    static GlobalMap& getInstance();
    
    // 符号管理
    void addSymbol(const std::string& name, SymbolType type, 
                   const std::string& namespace_path = "",
                   const std::string& file_path = "", size_t line = 0);
    
    std::shared_ptr<Symbol> getSymbol(const std::string& name, 
                                      const std::string& namespace_path = "") const;
    
    std::vector<std::shared_ptr<Symbol>> getSymbolsByType(SymbolType type) const;
    
    bool hasSymbol(const std::string& name, const std::string& namespace_path = "") const;
    
    void removeSymbol(const std::string& name, const std::string& namespace_path = "");
    
    // 命名空间管理
    void createNamespace(const std::string& name, const std::string& parent_path = "");
    
    std::shared_ptr<Namespace> getNamespace(const std::string& full_path) const;
    
    void mergeNamespace(const std::string& target_path, const std::string& source_path);
    
    std::vector<std::string> getAllNamespaces() const;
    
    // 文件依赖管理
    void addFileDependency(const std::string& file_path);
    
    void addImport(const std::string& file_path, const std::string& import_path);
    
    void addDependency(const std::string& file_path, const std::string& dependency_path);
    
    bool checkCircularDependency(const std::string& file_path) const;
    
    std::vector<std::string> getProcessingOrder() const;
    
    void markFileProcessed(const std::string& file_path);
    
    bool isFileProcessed(const std::string& file_path) const;
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    
    std::string getConfiguration(const std::string& key, const std::string& default_value = "") const;
    
    bool hasConfiguration(const std::string& key) const;
    
    // 自动生成名称管理
    std::string generateClassName(const std::string& prefix = "chtl-class");
    
    std::string generateIdName(const std::string& prefix = "chtl-id");
    
    void registerGeneratedName(const std::string& name, const std::string& element_path);
    
    std::string getGeneratedNameForElement(const std::string& element_path) const;
    
    // 错误管理
    void addError(const std::string& message, const std::string& file_path = "", 
                  size_t line = 0, size_t column = 0);
    
    void addWarning(const std::string& message, const std::string& file_path = "", 
                    size_t line = 0, size_t column = 0);
    
    std::vector<std::string> getErrors() const;
    
    std::vector<std::string> getWarnings() const;
    
    bool hasErrors() const;
    
    void clearErrors();
    
    void clearWarnings();
    
    // 清理
    void clear();
    
    void clearFile(const std::string& file_path);
    
    // 调试
    void dumpSymbols() const;
    
    void dumpNamespaces() const;
    
    void dumpDependencies() const;

private:
    GlobalMap() : classNameCounter(0), idNameCounter(0) {}
    ~GlobalMap() = default;
    
    // 禁止拷贝和赋值
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 内部数据
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> namespaces;
    std::unordered_map<std::string, FileDependency> file_dependencies;
    std::unordered_map<std::string, std::string> configurations;
    std::unordered_map<std::string, std::string> generated_names; // element_path -> generated_name
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 名称计数器
    size_t classNameCounter;
    size_t idNameCounter;
    
    // 线程安全
    mutable std::mutex mutex;
    
    // 内部辅助方法
    std::string buildSymbolKey(const std::string& name, const std::string& namespace_path) const;
    
    bool hasCyclicDependencyHelper(const std::string& file_path, 
                                   std::unordered_map<std::string, int>& visited) const;
};

} // namespace Lexer
} // namespace CHTL