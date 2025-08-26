#ifndef CHTLJS_GLOBAL_MAP_H
#define CHTLJS_GLOBAL_MAP_H

#include "Core/Common.h"
#include <any>

namespace CHTLJS {

// CHTL JS符号类型
enum class SymbolType {
    // 选择器相关
    EnhancedSelector,       // 增强选择器 {{}}
    CachedSelector,         // 缓存的选择器结果
    
    // 事件相关
    EventListener,          // 事件监听器
    EventDelegation,        // 事件委托
    EventHandler,           // 事件处理函数
    
    // 动画相关
    Animation,              // 动画定义
    Keyframe,               // 关键帧
    EasingFunction,         // 缓动函数
    
    // 虚对象相关
    VirtualObject,          // 虚对象定义
    VirtualMethod,          // 虚对象方法
    VirtualProperty,        // 虚对象属性
    
    // DOM操作相关
    DOMQuery,               // DOM查询
    DOMManipulation,        // DOM操作
    
    // 变量和函数
    Variable,               // 变量
    Function,               // 函数
    ArrowFunction,          // 箭头函数（通过->创建）
    
    // 模块相关
    ImportedFunction,       // 导入的函数
    ImportedVariable,       // 导入的变量
    Module,                 // 模块
    
    // 其他
    Unknown                 // 未知类型
};

// 符号信息
struct SymbolInfo {
    String name;                    // 符号名称
    SymbolType type;               // 符号类型
    String selector;               // 相关的选择器（如果有）
    String eventName;              // 事件名（如果是事件相关）
    bool isAsync = false;          // 是否是异步操作
    bool isCached = false;         // 是否被缓存
    HashMap<String, String> metadata;  // 额外元数据
    std::any data;                // 符号数据
    SourceLocation location;       // 定义位置
    
    SymbolInfo() : type(SymbolType::Unknown) {}
    
    SymbolInfo(const String& n, SymbolType t)
        : name(n), type(t) {}
};

// 动画信息
struct AnimationInfo {
    String name;                   // 动画名称
    String target;                 // 目标选择器
    HashMap<String, String> from;  // 起始状态
    HashMap<String, String> to;    // 结束状态
    HashMap<String, String> by;    // 增量变化
    double duration = 1.0;         // 持续时间（秒）
    double delay = 0.0;            // 延迟时间（秒）
    String easing = "linear";      // 缓动函数
    int repeat = 1;                // 重复次数（-1表示无限）
    bool alternate = false;        // 是否交替
    String fill = "none";          // 填充模式
};

// 事件监听器信息
struct ListenerInfo {
    String selector;               // 目标选择器
    String eventName;              // 事件名
    String handlerName;            // 处理函数名
    bool useCapture = false;       // 是否在捕获阶段
    bool once = false;             // 是否只执行一次
    bool passive = false;          // 是否是被动监听器
    HashMap<String, String> options;  // 其他选项（支持无序键值对）
};

// CHTL JS全局映射表
class GlobalMap {
private:
    // 符号表
    HashMap<String, SymbolInfo> symbols_;
    
    // 选择器缓存
    HashMap<String, String> selectorCache_;
    
    // 事件监听器映射（选择器 -> 事件列表）
    HashMap<String, Vector<ListenerInfo>> listeners_;
    
    // 动画定义
    HashMap<String, AnimationInfo> animations_;
    
    // 虚对象定义
    HashMap<String, std::any> virtualObjects_;
    
    // 分类索引
    HashMap<SymbolType, Vector<String>> typeIndex_;
    
    // DOM查询优化索引
    HashMap<String, Vector<String>> domQueryIndex_;
    
public:
    GlobalMap() = default;
    ~GlobalMap() = default;
    
    // 符号管理
    Result<void> AddSymbol(const SymbolInfo& symbol);
    Optional<SymbolInfo> FindSymbol(const String& name) const;
    Result<void> RemoveSymbol(const String& name);
    bool HasSymbol(const String& name) const;
    
    // 选择器管理
    void CacheSelector(const String& original, const String& compiled);
    Optional<String> GetCachedSelector(const String& original) const;
    void InvalidateSelectorCache();
    
    // 事件监听器管理
    Result<void> AddListener(const ListenerInfo& listener);
    Vector<ListenerInfo> GetListeners(const String& selector) const;
    Vector<ListenerInfo> GetListenersByEvent(const String& eventName) const;
    Result<void> RemoveListener(const String& selector, const String& eventName);
    
    // 动画管理
    Result<void> AddAnimation(const String& name, const AnimationInfo& animation);
    Optional<AnimationInfo> FindAnimation(const String& name) const;
    Vector<String> GetAnimationNames() const;
    
    // 虚对象管理
    Result<void> AddVirtualObject(const String& name, const std::any& definition);
    Optional<std::any> FindVirtualObject(const String& name) const;
    bool IsVirtualObject(const String& name) const;
    
    // DOM查询优化
    void AddDOMQuery(const String& selector, const String& optimizedQuery);
    Optional<String> GetOptimizedQuery(const String& selector) const;
    
    // 获取符号
    Vector<SymbolInfo> GetSymbolsByType(SymbolType type) const;
    Vector<String> GetAllSelectors() const;
    Vector<String> GetAllEventNames() const;
    
    // 清空
    void Clear();
    void ClearCache();
    
    // 统计信息
    size_t GetSymbolCount() const { return symbols_.size(); }
    size_t GetListenerCount() const;
    size_t GetAnimationCount() const { return animations_.size(); }
    size_t GetCacheSize() const { return selectorCache_.size(); }
    
    // 调试输出
    String DumpSymbols() const;
    String DumpListeners() const;
    String DumpAnimations() const;
    
private:
    // 更新索引
    void UpdateIndices(const String& name, const SymbolInfo& symbol);
    void RemoveFromIndices(const String& name, const SymbolInfo& symbol);
};

// 全局映射表管理器（单例）
class GlobalMapManager {
private:
    static UniquePtr<GlobalMap> instance_;
    
public:
    static GlobalMap& GetInstance();
    static void Reset();
};

// 选择器缓存守卫（RAII）
class SelectorCacheGuard {
private:
    GlobalMap& map_;
    Vector<String> cachedSelectors_;
    
public:
    explicit SelectorCacheGuard(GlobalMap& map) : map_(map) {}
    
    ~SelectorCacheGuard() {
        // 清理临时缓存的选择器
        for (const auto& selector : cachedSelectors_) {
            // 可选：清理特定的缓存项
        }
    }
    
    void AddCached(const String& selector) {
        cachedSelectors_.push_back(selector);
    }
};

// 事件监听器批量管理
class ListenerBatch {
private:
    GlobalMap& map_;
    Vector<ListenerInfo> pendingListeners_;
    
public:
    explicit ListenerBatch(GlobalMap& map) : map_(map) {}
    
    void AddListener(const ListenerInfo& listener) {
        pendingListeners_.push_back(listener);
    }
    
    Result<void> Commit() {
        for (const auto& listener : pendingListeners_) {
            auto result = map_.AddListener(listener);
            if (!result.IsOk()) {
                return result;
            }
        }
        return Result<void>::Ok();
    }
    
    void Clear() {
        pendingListeners_.clear();
    }
};

} // namespace CHTLJS

#endif // CHTLJS_GLOBAL_MAP_H