#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include "Core/Common.h"
#include <any>

namespace CHTL {

// 符号类型
enum class SymbolType {
    // 元素
    HTMLElement,            // HTML元素
    
    // 模板
    StyleTemplate,          // 样式组模板
    ElementTemplate,        // 元素模板
    VariableTemplate,       // 变量组模板
    
    // 自定义
    CustomStyle,            // 自定义样式组
    CustomElement,          // 自定义元素
    CustomVariable,         // 自定义变量组
    
    // 原始嵌入
    OriginHTML,             // 原始HTML
    OriginStyle,            // 原始CSS
    OriginJavaScript,       // 原始JavaScript
    OriginCustomType,       // 自定义原始类型
    
    // 配置
    Configuration,          // 配置组
    ConfigurationValue,     // 配置值
    
    // 命名空间
    Namespace,              // 命名空间
    
    // 导入
    ImportedModule,         // 导入的模块
    ImportedSymbol,         // 导入的符号
    
    // 其他
    Unknown                 // 未知类型
};

// 符号信息
struct SymbolInfo {
    String name;                    // 符号名称
    SymbolType type;               // 符号类型
    String fullName;               // 全限定名（包含命名空间）
    String sourcefile;             // 定义所在文件
    SourceLocation location;       // 定义位置
    bool isPublic = true;          // 是否公开
    bool isImported = false;       // 是否是导入的
    String importedFrom;           // 导入来源
    HashMap<String, String> metadata;  // 额外元数据
    std::any data;                // 符号数据（可以存储任意类型）
    
    SymbolInfo() : type(SymbolType::Unknown) {}
    
    SymbolInfo(const String& n, SymbolType t)
        : name(n), type(t), fullName(n) {}
};

// 全局映射表
class GlobalMap {
private:
    // 符号表（名称 -> 符号信息）
    HashMap<String, SymbolInfo> symbols_;
    
    // 分类索引（类型 -> 符号名称列表）
    HashMap<SymbolType, Vector<String>> typeIndex_;
    
    // 命名空间索引（命名空间 -> 符号名称列表）
    HashMap<String, Vector<String>> namespaceIndex_;
    
    // 文件索引（文件名 -> 符号名称列表）
    HashMap<String, Vector<String>> fileIndex_;
    
    // 当前命名空间栈
    Vector<String> namespaceStack_;
    
    // 配置值缓存
    HashMap<String, String> configCache_;
    
public:
    GlobalMap() = default;
    ~GlobalMap() = default;
    
    // 添加符号
    Result<void> AddSymbol(const SymbolInfo& symbol);
    Result<void> AddSymbol(const String& name, SymbolType type, const std::any& data = {});
    
    // 查找符号
    Optional<SymbolInfo> FindSymbol(const String& name) const;
    Optional<SymbolInfo> FindSymbolInNamespace(const String& name, const String& ns) const;
    
    // 获取符号
    Vector<SymbolInfo> GetSymbolsByType(SymbolType type) const;
    Vector<SymbolInfo> GetSymbolsInNamespace(const String& ns) const;
    Vector<SymbolInfo> GetSymbolsInFile(const String& file) const;
    
    // 检查符号是否存在
    bool HasSymbol(const String& name) const;
    bool HasSymbolInNamespace(const String& name, const String& ns) const;
    
    // 删除符号
    Result<void> RemoveSymbol(const String& name);
    
    // 命名空间管理
    void PushNamespace(const String& ns);
    void PopNamespace();
    String GetCurrentNamespace() const;
    String BuildFullName(const String& name) const;
    
    // 导入管理
    Result<void> ImportSymbol(const SymbolInfo& symbol, const String& fromModule);
    Result<void> ImportAllFromModule(const String& moduleName, const Vector<SymbolInfo>& symbols);
    
    // 配置管理
    void SetConfig(const String& key, const String& value);
    Optional<String> GetConfig(const String& key) const;
    HashMap<String, String> GetAllConfigs() const;
    
    // 合并另一个GlobalMap
    Result<void> Merge(const GlobalMap& other, const String& prefix = "");
    
    // 清空
    void Clear();
    
    // 统计信息
    size_t GetSymbolCount() const { return symbols_.size(); }
    size_t GetSymbolCount(SymbolType type) const;
    
    // 调试输出
    String DumpSymbols() const;
    String DumpNamespaces() const;
    
private:
    // 更新索引
    void UpdateIndices(const String& name, const SymbolInfo& symbol);
    void RemoveFromIndices(const String& name, const SymbolInfo& symbol);
    
    // 解析全限定名
    std::pair<String, String> ParseQualifiedName(const String& fullName) const;
};

// 全局映射表管理器（单例）
class GlobalMapManager {
private:
    static UniquePtr<GlobalMap> instance_;
    
public:
    static GlobalMap& GetInstance();
    static void Reset();
};

// 符号作用域守卫（RAII）
class SymbolScopeGuard {
private:
    GlobalMap& map_;
    Vector<String> addedSymbols_;
    
public:
    explicit SymbolScopeGuard(GlobalMap& map) : map_(map) {}
    
    ~SymbolScopeGuard() {
        // 离开作用域时删除添加的符号
        for (const auto& symbol : addedSymbols_) {
            map_.RemoveSymbol(symbol);
        }
    }
    
    void AddSymbol(const String& name) {
        addedSymbols_.push_back(name);
    }
};

// 命名空间作用域守卫（RAII）
class NamespaceScopeGuard {
private:
    GlobalMap& map_;
    
public:
    NamespaceScopeGuard(GlobalMap& map, const String& ns) : map_(map) {
        map_.PushNamespace(ns);
    }
    
    ~NamespaceScopeGuard() {
        map_.PopNamespace();
    }
};

} // namespace CHTL

#endif // CHTL_GLOBAL_MAP_H