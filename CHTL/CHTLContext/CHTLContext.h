#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <memory>
#include <stack>
#include <vector>
#include <unordered_map>
#include "../CHTLState/CHTLState.h"
#include "../CHTLManage/CHTLGlobalMap.h"

namespace CHTL {

// 前向声明
class CHTLNode;
class CHTLLexer;
class CHTLParser;

// 上下文类型
enum class ContextType {
    FILE,           // 文件上下文
    NAMESPACE,      // 命名空间上下文
    ELEMENT,        // 元素上下文
    STYLE,          // 样式上下文
    SCRIPT,         // 脚本上下文
    IMPORT,         // 导入上下文
    CONFIGURATION   // 配置上下文
};

// 上下文信息
struct ContextInfo {
    ContextType type;
    std::string name;
    std::string file_path;
    std::shared_ptr<CHTLNode> root_node;
    std::unordered_map<std::string, std::string> attributes;
    
    // 局部符号表
    std::unordered_map<std::string, SymbolInfo> local_symbols;
    
    // 上下文特定数据
    std::unordered_map<std::string, std::string> metadata;
    
    ContextInfo(ContextType t, const std::string& n = "")
        : type(t), name(n) {}
};

// RAII上下文管理器
class ContextGuard {
private:
    class CHTLContext* context;
    bool released;
    
public:
    ContextGuard(CHTLContext* ctx, ContextType type, const std::string& name = "");
    ~ContextGuard();
    
    // 禁止拷贝
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
    
    // 允许移动
    ContextGuard(ContextGuard&& other) noexcept;
    ContextGuard& operator=(ContextGuard&& other) noexcept;
    
    void release() { released = true; }
};

// 编译上下文管理器
class CHTLContext {
public:
    // 编译选项
    struct CompileOptions {
        bool enable_debug = false;
        bool strict_mode = false;
        bool generate_source_map = false;
        std::string output_directory = "./";
        std::string module_directory = "./module/";
        std::string official_module_directory = "";
    };

private:
    // 上下文栈
    std::stack<ContextInfo> context_stack;
    
    // 当前文件信息
    std::string current_file;
    std::string current_directory;
    
    // 状态管理器
    std::unique_ptr<CHTLState> state;
    
    // 全局符号表指针（改为指针）
    CHTLGlobalMap* global_map;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 编译选项
    CompileOptions options;

public:
    CHTLContext();
    ~CHTLContext() = default;
    
    // 上下文管理
    void pushContext(ContextType type, const std::string& name = "");
    void popContext();
    ContextInfo& getCurrentContext();
    const ContextInfo& getCurrentContext() const;
    
    // RAII上下文管理
    [[nodiscard]] ContextGuard enterContext(ContextType type, const std::string& name = "");
    
    // 文件管理
    void setCurrentFile(const std::string& file);
    std::string getCurrentFile() const { return current_file; }
    std::string getCurrentDirectory() const { return current_directory; }
    
    // 状态访问
    CHTLState& getState() { return *state; }
    const CHTLState& getState() const { return *state; }
    
    // 全局符号表访问
    CHTLGlobalMap& getGlobalMap() { return *global_map; }
    const CHTLGlobalMap& getGlobalMap() const { return *global_map; }
    
    // 局部符号管理
    bool addLocalSymbol(const std::string& name, const SymbolInfo& info);
    bool hasLocalSymbol(const std::string& name) const;
    std::optional<SymbolInfo> getLocalSymbol(const std::string& name) const;
    
    // 符号查找（先局部后全局）
    std::optional<SymbolInfo> findSymbol(const std::string& name) const;
    
    // 属性管理（用于元素上下文）
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    std::unordered_map<std::string, std::string> getAttributes() const;
    
    // 元数据管理
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    
    // 错误和警告管理
    void addError(const std::string& message, int line = -1, int column = -1);
    void addWarning(const std::string& message, int line = -1, int column = -1);
    std::vector<std::string> getErrors() const { return errors; }
    std::vector<std::string> getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // 编译选项
    void setCompileOptions(const CompileOptions& opts) { options = opts; }
    CompileOptions getCompileOptions() const { return options; }
    void setDebugMode(bool enable) { options.enable_debug = enable; }
    bool isDebugMode() const { return options.enable_debug; }
    void setStrictMode(bool enable) { options.strict_mode = enable; }
    bool isStrictMode() const { return options.strict_mode; }
    
    // 模块路径解析
    std::string resolveModulePath(const std::string& module_name, const std::string& type) const;
    std::string resolveImportPath(const std::string& path, const std::string& type) const;
    
    // 上下文查询
    bool isInContext(ContextType type) const;
    int getContextDepth() const;
    std::vector<ContextType> getContextStack() const;
    
    // 特殊查询
    bool isInFileContext() const;
    bool isInNamespaceContext() const;
    bool isInElementContext() const;
    bool isInStyleContext() const;
    bool isInScriptContext() const;
    
    // 获取特定上下文
    std::optional<ContextInfo> findContext(ContextType type) const;
    std::string getCurrentNamespace() const;
    std::string getCurrentElement() const;
    
    // 清理
    void clear();
    void clearContextStack();
    
    // 调试
    void dumpContext() const;
    std::string getContextString() const;
    
    friend class ContextGuard;
};

// 获取当前线程的上下文
CHTLContext& getCurrentContext();

} // namespace CHTL

#endif // CHTL_CONTEXT_H