#pragma once
#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSState/State.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS符号类型
 */
enum class SymbolType {
    CHTLJS_FUNCTION,            // CHTL JS函数
    ENHANCED_SELECTOR,          // 增强选择器
    EVENT_LISTENER,             // 事件监听器
    EVENT_DELEGATE,             // 事件委托
    ANIMATION,                  // 动画
    VIRTUAL_OBJECT,             // 虚拟对象
    MODULE_IMPORT,              // 模块导入
    VARIABLE,                   // 变量
    UNKNOWN_SYMBOL              // 未知符号
};

/**
 * CHTL JS符号信息
 */
class SymbolInfo {
public:
    SymbolType type;            // 符号类型
    std::string name;           // 符号名称
    std::string value;          // 符号值
    Position position;          // 位置信息
    
    SymbolInfo(SymbolType t = SymbolType::UNKNOWN_SYMBOL,
               const std::string& n = "",
               const std::string& v = "",
               const Position& pos = Position())
        : type(t), name(n), value(v), position(pos) {}
    
    std::string toString() const;
};

/**
 * CHTL JS上下文管理器
 * 管理CHTL JS特有的上下文信息
 */
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext() = default;
    
    // 禁用拷贝和移动
    CHTLJSContext(const CHTLJSContext&) = delete;
    CHTLJSContext& operator=(const CHTLJSContext&) = delete;
    
    // ========== 符号管理 ==========
    
    /**
     * 添加符号
     */
    void addSymbol(const SymbolInfo& symbol);
    
    /**
     * 查找符号
     */
    SymbolInfo* findSymbol(const std::string& name);
    
    /**
     * 检查符号是否存在
     */
    bool hasSymbol(const std::string& name) const;
    
    // ========== 增强选择器管理 ==========
    
    /**
     * 注册增强选择器
     */
    void registerEnhancedSelector(const std::string& selector);
    
    /**
     * 获取所有增强选择器
     */
    std::vector<std::string> getEnhancedSelectors() const;
    
    // ========== 事件管理 ==========
    
    /**
     * 注册事件监听器
     */
    void registerEventListener(const std::string& event, const std::string& selector);
    
    /**
     * 注册事件委托
     */
    void registerEventDelegate(const std::string& event, const std::string& selector);
    
    // ========== 虚拟对象管理 ==========
    
    /**
     * 注册虚拟对象
     */
    void registerVirtualObject(const std::string& name, const std::string& definition);
    
    /**
     * 获取虚拟对象
     */
    std::string getVirtualObject(const std::string& name) const;
    
    // ========== 上下文控制 ==========
    
    /**
     * 重置上下文
     */
    void reset();
    
    /**
     * 获取上下文统计
     */
    std::string getContextStats() const;

private:
    std::unordered_map<std::string, SymbolInfo> m_symbols;
    std::unordered_set<std::string> m_enhancedSelectors;
    std::unordered_map<std::string, std::string> m_eventListeners;
    std::unordered_map<std::string, std::string> m_eventDelegates;
    std::unordered_map<std::string, std::string> m_virtualObjects;
};

} // namespace CHTLJS