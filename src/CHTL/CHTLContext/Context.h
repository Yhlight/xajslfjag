#pragma once
#include "../CHTLLexer/Type.h"
#include "../CHTLState/State.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <stack>

namespace CHTL {

/**
 * 符号类型枚举
 */
enum class SymbolType {
    TEMPLATE_STYLE,         // 样式组模板
    TEMPLATE_ELEMENT,       // 元素模板
    TEMPLATE_VAR,           // 变量组模板
    CUSTOM_STYLE,           // 自定义样式组
    CUSTOM_ELEMENT,         // 自定义元素
    CUSTOM_VAR,             // 自定义变量组
    ORIGIN_BLOCK,           // 原始嵌入块
    NAMESPACE,              // 命名空间
    CONFIGURATION,          // 配置项
    VARIABLE,               // 变量
    SELECTOR,               // 选择器
    UNKNOWN_SYMBOL          // 未知符号
};

/**
 * 符号信息类
 */
class SymbolInfo {
public:
    SymbolType type;            // 符号类型
    std::string name;           // 符号名称
    std::string fullName;       // 完整名称（包含命名空间）
    std::string namespace_;     // 所属命名空间
    TypeInfo typeInfo;          // 类型信息
    std::vector<std::string> inherits; // 继承的符号
    bool isExported;            // 是否导出
    
    SymbolInfo(SymbolType t = SymbolType::UNKNOWN_SYMBOL,
               const std::string& n = "",
               const std::string& ns = "",
               const TypeInfo& ti = TypeInfo())
        : type(t), name(n), namespace_(ns), typeInfo(ti), isExported(false) {
        updateFullName();
    }
    
    // 更新完整名称
    void updateFullName();
    
    // 检查是否为模板符号
    bool isTemplateSymbol() const;
    
    // 检查是否为自定义符号
    bool isCustomSymbol() const;
    
    // 获取字符串表示
    std::string toString() const;
};

/**
 * 上下文作用域类型（避免与Type.h中的ScopeType冲突）
 */
enum class ContextScopeType {
    GLOBAL_SCOPE,           // 全局作用域
    NAMESPACE_SCOPE,        // 命名空间作用域
    ELEMENT_SCOPE,          // 元素作用域
    STYLE_SCOPE,            // 样式作用域
    TEMPLATE_SCOPE,         // 模板作用域
    CUSTOM_SCOPE,           // 自定义作用域
    CONFIGURATION_SCOPE,    // 配置作用域
    LOCAL_SCOPE             // 局部作用域
};

/**
 * 作用域信息类
 */
class ScopeInfo {
public:
    ContextScopeType type;          // 作用域类型
    std::string name;               // 作用域名称
    std::string namespace_;         // 命名空间
    size_t depth;                   // 嵌套深度
    std::unordered_map<std::string, SymbolInfo> symbols; // 符号表
    std::unordered_set<std::string> constraints;         // 约束集合
    
    ScopeInfo(ContextScopeType t = ContextScopeType::LOCAL_SCOPE,
              const std::string& n = "",
              const std::string& ns = "",
              size_t d = 0)
        : type(t), name(n), namespace_(ns), depth(d) {}
    
    // 添加符号
    bool addSymbol(const SymbolInfo& symbol);
    
    // 查找符号
    SymbolInfo* findSymbol(const std::string& name);
    const SymbolInfo* findSymbol(const std::string& name) const;
    
    // 检查是否存在符号
    bool hasSymbol(const std::string& name) const;
    
    // 添加约束
    void addConstraint(const std::string& constraint);
    
    // 检查是否被约束
    bool isConstrained(const std::string& item) const;
    
    // 获取所有符号
    std::vector<SymbolInfo> getAllSymbols() const;
};

/**
 * 选择器上下文
 * 管理当前元素的类名和ID信息
 */
class SelectorContext {
public:
    std::string currentClass;       // 当前类名
    std::string currentId;          // 当前ID
    std::vector<std::string> autoClasses; // 自动添加的类名
    std::vector<std::string> autoIds;      // 自动添加的ID
    bool hasExplicitClass;          // 是否有显式类名
    bool hasExplicitId;             // 是否有显式ID
    
    SelectorContext() : hasExplicitClass(false), hasExplicitId(false) {}
    
    // 设置类名
    void setClass(const std::string& className, bool isExplicit = true);
    
    // 设置ID
    void setId(const std::string& idName, bool isExplicit = true);
    
    // 添加自动类名
    void addAutoClass(const std::string& className);
    
    // 添加自动ID
    void addAutoId(const std::string& idName);
    
    // 获取有效的类名
    std::string getEffectiveClass() const;
    
    // 获取有效的ID
    std::string getEffectiveId() const;
    
    // 重置上下文
    void reset();
};

/**
 * RAII作用域管理器
 */
class RAIIScopeManager {
public:
    /**
     * 构造函数 - 自动进入作用域
     */
    RAIIScopeManager(class CHTLContext* context, 
                     ContextScopeType scopeType,
                     const std::string& name = "",
                     const std::string& namespace_ = "");
    
    /**
     * 析构函数 - 自动退出作用域
     */
    ~RAIIScopeManager();
    
    // 禁用拷贝和移动
    RAIIScopeManager(const RAIIScopeManager&) = delete;
    RAIIScopeManager& operator=(const RAIIScopeManager&) = delete;
    RAIIScopeManager(RAIIScopeManager&&) = delete;
    RAIIScopeManager& operator=(RAIIScopeManager&&) = delete;
    
    /**
     * 获取当前作用域
     */
    const ScopeInfo& getCurrentScope() const;
    
    /**
     * 检查是否有效
     */
    bool isValid() const;

private:
    class CHTLContext* m_context;
    bool m_valid;
};

/**
 * CHTL上下文管理器
 * 管理CHTL语法解析过程中的上下文信息
 */
class CHTLContext {
public:
    /**
     * 构造函数
     */
    CHTLContext();
    
    /**
     * 析构函数
     */
    ~CHTLContext() = default;
    
    // 禁用拷贝和移动
    CHTLContext(const CHTLContext&) = delete;
    CHTLContext& operator=(const CHTLContext&) = delete;
    
    // ========== 作用域管理 ==========
    
    /**
     * 进入新作用域
     */
    bool enterScope(ContextScopeType scopeType, 
                    const std::string& name = "",
                    const std::string& namespace_ = "");
    
    /**
     * 退出当前作用域
     */
    bool exitScope();
    
    /**
     * 获取当前作用域
     */
    const ScopeInfo& getCurrentScope() const;
    
    /**
     * 获取作用域深度
     */
    size_t getScopeDepth() const;
    
    /**
     * 创建RAII作用域管理器
     */
    std::unique_ptr<RAIIScopeManager> createScopeManager(ContextScopeType scopeType,
                                                          const std::string& name = "",
                                                          const std::string& namespace_ = "");
    
    // ========== 符号管理 ==========
    
    /**
     * 添加符号到当前作用域
     */
    bool addSymbol(const SymbolInfo& symbol);
    
    /**
     * 在当前作用域及父作用域中查找符号
     */
    SymbolInfo* findSymbol(const std::string& name);
    const SymbolInfo* findSymbol(const std::string& name) const;
    
    /**
     * 在指定命名空间中查找符号
     */
    SymbolInfo* findSymbolInNamespace(const std::string& name, const std::string& namespace_);
    
    /**
     * 检查符号是否存在
     */
    bool hasSymbol(const std::string& name) const;
    
    /**
     * 获取当前作用域的所有符号
     */
    std::vector<SymbolInfo> getCurrentScopeSymbols() const;
    
    // ========== 命名空间管理 ==========
    
    /**
     * 设置当前命名空间
     */
    void setCurrentNamespace(const std::string& namespace_);
    
    /**
     * 获取当前命名空间
     */
    std::string getCurrentNamespace() const;
    
    /**
     * 进入命名空间
     */
    void enterNamespace(const std::string& namespace_);
    
    /**
     * 退出命名空间
     */
    void exitNamespace();
    
    // ========== 选择器上下文管理 ==========
    
    /**
     * 获取选择器上下文
     */
    SelectorContext& getSelectorContext();
    const SelectorContext& getSelectorContext() const;
    
    /**
     * 重置选择器上下文
     */
    void resetSelectorContext();
    
    // ========== 约束管理 ==========
    
    /**
     * 添加约束
     */
    void addConstraint(const std::string& constraint);
    
    /**
     * 检查是否被约束
     */
    bool isConstrained(const std::string& item) const;
    
    /**
     * 清除约束
     */
    void clearConstraints();
    
    // ========== 继承关系管理 ==========
    
    /**
     * 添加继承关系
     */
    void addInheritance(const std::string& child, const std::string& parent);
    
    /**
     * 检查继承关系
     */
    bool isInherited(const std::string& child, const std::string& parent) const;
    
    /**
     * 获取继承链
     */
    std::vector<std::string> getInheritanceChain(const std::string& symbol) const;
    
    // ========== 状态管理 ==========
    
    /**
     * 重置上下文
     */
    void reset();
    
    /**
     * 设置错误回调
     */
    void setErrorCallback(std::function<void(const std::string&)> callback);

private:
    std::stack<ScopeInfo> m_scopeStack;                     // 作用域栈
    std::stack<std::string> m_namespaceStack;               // 命名空间栈
    SelectorContext m_selectorContext;                      // 选择器上下文
    std::unordered_map<std::string, std::vector<std::string>> m_inheritanceMap; // 继承关系映射
    std::function<void(const std::string&)> m_errorCallback; // 错误回调
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
    
    friend class RAIIScopeManager;
};

/**
 * 便捷宏定义
 */
#define CHTL_SCOPE_GUARD(context, scopeType) \
    auto scopeGuard = (context).createScopeManager(scopeType, __FUNCTION__)

#define CHTL_SCOPE_GUARD_WITH_NAME(context, scopeType, name) \
    auto scopeGuard = (context).createScopeManager(scopeType, name)

#define CHTL_SCOPE_GUARD_WITH_NAMESPACE(context, scopeType, name, namespace_) \
    auto scopeGuard = (context).createScopeManager(scopeType, name, namespace_)

} // namespace CHTL