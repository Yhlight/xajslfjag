#pragma once

#include "../../Util/Common.h"
#include "../CHTLState/State.h"
#include "../CHTLContext/Context.h"

namespace CHTL {

// RAII状态管理器
class RAIIStateManager {
public:
    explicit RAIIStateManager(std::shared_ptr<StateManager> stateManager);
    ~RAIIStateManager() = default;
    
    // 禁止复制，允许移动
    RAIIStateManager(const RAIIStateManager&) = delete;
    RAIIStateManager& operator=(const RAIIStateManager&) = delete;
    RAIIStateManager(RAIIStateManager&&) = default;
    RAIIStateManager& operator=(RAIIStateManager&&) = default;
    
    // RAII状态守卫
    class StateGuard {
    public:
        StateGuard(RAIIStateManager& manager, CHTLState newState);
        ~StateGuard();
        
        // 禁止复制和移动
        StateGuard(const StateGuard&) = delete;
        StateGuard& operator=(const StateGuard&) = delete;
        StateGuard(StateGuard&&) = delete;
        StateGuard& operator=(StateGuard&&) = delete;
        
        // 手动恢复状态
        void restore();
        // 释放自动恢复
        void release();
        // 检查是否活跃
        bool isActive() const { return active; }
        
    private:
        RAIIStateManager& manager;
        CHTLState savedState;
        bool active;
    };
    
    // 状态栈守卫
    class StateStackGuard {
    public:
        StateStackGuard(RAIIStateManager& manager, CHTLState newState);
        ~StateStackGuard();
        
        // 禁止复制和移动
        StateStackGuard(const StateStackGuard&) = delete;
        StateStackGuard& operator=(const StateStackGuard&) = delete;
        StateStackGuard(StateStackGuard&&) = delete;
        StateStackGuard& operator=(StateStackGuard&&) = delete;
        
        void release();
        bool isActive() const { return active; }
        
    private:
        RAIIStateManager& manager;
        bool active;
    };
    
    // 条件状态守卫
    class ConditionalStateGuard {
    public:
        ConditionalStateGuard(RAIIStateManager& manager, CHTLState newState, bool condition);
        ~ConditionalStateGuard();
        
        // 禁止复制和移动
        ConditionalStateGuard(const ConditionalStateGuard&) = delete;
        ConditionalStateGuard& operator=(const ConditionalStateGuard&) = delete;
        ConditionalStateGuard(ConditionalStateGuard&&) = delete;
        ConditionalStateGuard& operator=(ConditionalStateGuard&&) = delete;
        
        void release();
        bool isActive() const { return active; }
        bool wasConditionMet() const { return conditionMet; }
        
    private:
        RAIIStateManager& manager;
        CHTLState savedState;
        bool active;
        bool conditionMet;
    };
    
    // 状态作用域
    template<typename Func>
    auto withState(CHTLState state, Func&& func) -> decltype(func()) {
        StateGuard guard(*this, state);
        return func();
    }
    
    template<typename Func>
    auto withStateStack(CHTLState state, Func&& func) -> decltype(func()) {
        StateStackGuard guard(*this, state);
        return func();
    }
    
    template<typename Func>
    void conditionalWithState(CHTLState state, bool condition, Func&& func) {
        ConditionalStateGuard guard(*this, state, condition);
        if (condition) {
            func();
        }
    }
    
    // 状态管理器访问
    std::shared_ptr<StateManager> getStateManager() const { return stateManager; }
    CHTLState getCurrentState() const { return stateManager->getCurrentState(); }
    
    // 便捷方法
    StateGuard enterState(CHTLState state) { return StateGuard(*this, state); }
    StateStackGuard pushState(CHTLState state) { return StateStackGuard(*this, state); }
    ConditionalStateGuard enterStateIf(CHTLState state, bool condition) { 
        return ConditionalStateGuard(*this, state, condition); 
    }
    
private:
    std::shared_ptr<StateManager> stateManager;
    
    friend class StateGuard;
    friend class StateStackGuard;
    friend class ConditionalStateGuard;
};

// RAII上下文管理器
class RAIIContextManager {
public:
    explicit RAIIContextManager(std::shared_ptr<Context> context);
    ~RAIIContextManager() = default;
    
    // 禁止复制，允许移动
    RAIIContextManager(const RAIIContextManager&) = delete;
    RAIIContextManager& operator=(const RAIIContextManager&) = delete;
    RAIIContextManager(RAIIContextManager&&) = default;
    RAIIContextManager& operator=(RAIIContextManager&&) = default;
    
    // 作用域守卫
    class ScopeGuard {
    public:
        ScopeGuard(RAIIContextManager& manager, const String& scopeName, ContextType type);
        ~ScopeGuard();
        
        // 禁止复制和移动
        ScopeGuard(const ScopeGuard&) = delete;
        ScopeGuard& operator=(const ScopeGuard&) = delete;
        ScopeGuard(ScopeGuard&&) = delete;
        ScopeGuard& operator=(ScopeGuard&&) = delete;
        
        void release();
        bool isActive() const { return active; }
        const String& getScopeName() const { return scopeName; }
        ContextType getScopeType() const { return scopeType; }
        
    private:
        RAIIContextManager& manager;
        String scopeName;
        ContextType scopeType;
        bool active;
    };
    
    // 命名空间守卫
    class NamespaceGuard {
    public:
        NamespaceGuard(RAIIContextManager& manager, const String& namespaceName);
        ~NamespaceGuard();
        
        // 禁止复制和移动
        NamespaceGuard(const NamespaceGuard&) = delete;
        NamespaceGuard& operator=(const NamespaceGuard&) = delete;
        NamespaceGuard(NamespaceGuard&&) = delete;
        NamespaceGuard& operator=(NamespaceGuard&&) = delete;
        
        void release();
        bool isActive() const { return active; }
        const String& getNamespaceName() const { return namespaceName; }
        
    private:
        RAIIContextManager& manager;
        String namespaceName;
        bool active;
    };
    
    // 变量守卫
    class VariableGuard {
    public:
        VariableGuard(RAIIContextManager& manager, const String& name, const Variable& var);
        ~VariableGuard();
        
        // 禁止复制和移动
        VariableGuard(const VariableGuard&) = delete;
        VariableGuard& operator=(const VariableGuard&) = delete;
        VariableGuard(VariableGuard&&) = delete;
        VariableGuard& operator=(VariableGuard&&) = delete;
        
        void release();
        bool isActive() const { return active; }
        const String& getVariableName() const { return variableName; }
        
    private:
        RAIIContextManager& manager;
        String variableName;
        bool hadVariable;
        Variable savedVariable;
        bool active;
    };
    
    // 配置守卫
    class ConfigGuard {
    public:
        ConfigGuard(RAIIContextManager& manager, const String& key, const String& value);
        ~ConfigGuard();
        
        // 禁止复制和移动
        ConfigGuard(const ConfigGuard&) = delete;
        ConfigGuard& operator=(const ConfigGuard&) = delete;
        ConfigGuard(ConfigGuard&&) = delete;
        ConfigGuard& operator=(ConfigGuard&&) = delete;
        
        void release();
        bool isActive() const { return active; }
        
    private:
        RAIIContextManager& manager;
        String configKey;
        String savedValue;
        bool hadConfig;
        bool active;
    };
    
    // 作用域操作
    template<typename Func>
    auto withScope(const String& scopeName, ContextType type, Func&& func) -> decltype(func()) {
        ScopeGuard guard(*this, scopeName, type);
        return func();
    }
    
    template<typename Func>
    auto withNamespace(const String& namespaceName, Func&& func) -> decltype(func()) {
        NamespaceGuard guard(*this, namespaceName);
        return func();
    }
    
    template<typename Func>
    auto withVariable(const String& name, const Variable& var, Func&& func) -> decltype(func()) {
        VariableGuard guard(*this, name, var);
        return func();
    }
    
    template<typename Func>
    auto withConfig(const String& key, const String& value, Func&& func) -> decltype(func()) {
        ConfigGuard guard(*this, key, value);
        return func();
    }
    
    // 上下文访问
    std::shared_ptr<Context> getContext() const { return context; }
    
    // 便捷方法
    ScopeGuard enterScope(const String& scopeName, ContextType type) { 
        return ScopeGuard(*this, scopeName, type); 
    }
    NamespaceGuard enterNamespace(const String& namespaceName) { 
        return NamespaceGuard(*this, namespaceName); 
    }
    VariableGuard setVariable(const String& name, const Variable& var) { 
        return VariableGuard(*this, name, var); 
    }
    ConfigGuard setConfig(const String& key, const String& value) { 
        return ConfigGuard(*this, key, value); 
    }
    
private:
    std::shared_ptr<Context> context;
    
    friend class ScopeGuard;
    friend class NamespaceGuard;
    friend class VariableGuard;
    friend class ConfigGuard;
};

// 组合RAII管理器
class CombinedRAIIManager {
public:
    CombinedRAIIManager(std::shared_ptr<StateManager> stateManager, 
                       std::shared_ptr<Context> context);
    ~CombinedRAIIManager() = default;
    
    // 获取子管理器
    RAIIStateManager& getStateManager() { return stateManager; }
    RAIIContextManager& getContextManager() { return contextManager; }
    
    // 组合守卫
    class CombinedGuard {
    public:
        CombinedGuard(CombinedRAIIManager& manager, CHTLState state, 
                     const String& scopeName, ContextType contextType);
        ~CombinedGuard();
        
        // 禁止复制和移动
        CombinedGuard(const CombinedGuard&) = delete;
        CombinedGuard& operator=(const CombinedGuard&) = delete;
        CombinedGuard(CombinedGuard&&) = delete;
        CombinedGuard& operator=(CombinedGuard&&) = delete;
        
        void release();
        bool isActive() const;
        
    private:
        std::unique_ptr<RAIIStateManager::StateGuard> stateGuard;
        std::unique_ptr<RAIIContextManager::ScopeGuard> scopeGuard;
        bool active;
    };
    
    // 组合操作
    template<typename Func>
    auto withStateAndScope(CHTLState state, const String& scopeName, 
                          ContextType contextType, Func&& func) -> decltype(func()) {
        CombinedGuard guard(*this, state, scopeName, contextType);
        return func();
    }
    
    // 便捷方法
    CombinedGuard enterStateAndScope(CHTLState state, const String& scopeName, ContextType contextType) {
        return CombinedGuard(*this, state, scopeName, contextType);
    }
    
private:
    RAIIStateManager stateManager;
    RAIIContextManager contextManager;
};

// 智能状态追踪器
class StateTracker {
public:
    struct StateTransition {
        CHTLState fromState;
        CHTLState toState;
        double timestamp;
        String description;
        
        StateTransition(CHTLState from, CHTLState to, const String& desc = "")
            : fromState(from), toState(to), description(desc) {
            timestamp = getCurrentTime();
        }
    };
    
    explicit StateTracker(std::shared_ptr<StateManager> stateManager);
    ~StateTracker() = default;
    
    // 追踪控制
    void startTracking();
    void stopTracking();
    void pauseTracking();
    void resumeTracking();
    bool isTracking() const { return tracking; }
    
    // 追踪数据
    std::vector<StateTransition> getTransitions() const { return transitions; }
    void clearTransitions() { transitions.clear(); }
    
    // 统计信息
    struct TrackingStats {
        size_t totalTransitions = 0;
        double totalTime = 0.0;
        double averageTransitionTime = 0.0;
        StringUnorderedMap stateDistribution;
        StringUnorderedMap transitionFrequency;
    };
    
    TrackingStats getStats() const;
    
    // 分析方法
    StringVector getMostFrequentStates(size_t count = 5) const;
    StringVector getMostFrequentTransitions(size_t count = 5) const;
    double getAverageTimeInState(CHTLState state) const;
    bool hasCircularTransitions() const;
    
    // 导出数据
    String exportToJSON() const;
    String exportToCSV() const;
    void exportToFile(const String& filePath, const String& format = "json") const;
    
private:
    std::shared_ptr<StateManager> stateManager;
    std::vector<StateTransition> transitions;
    bool tracking;
    bool paused;
    CHTLState lastState;
    double lastTransitionTime;
    
    void onStateChanged(CHTLState oldState, CHTLState newState);
    double getCurrentTime() const;
    String stateToString(CHTLState state) const;
};

// 性能监控RAII
class PerformanceMonitor {
public:
    struct PerformanceData {
        String operationName;
        double startTime;
        double endTime;
        double duration;
        String category;
        StringUnorderedMap metadata;
        
        PerformanceData(const String& name, const String& cat = "")
            : operationName(name), category(cat) {
            startTime = getCurrentTime();
            endTime = 0.0;
            duration = 0.0;
        }
    };
    
    class PerformanceScope {
    public:
        PerformanceScope(PerformanceMonitor& monitor, const String& operationName, 
                        const String& category = "");
        ~PerformanceScope();
        
        // 禁止复制和移动
        PerformanceScope(const PerformanceScope&) = delete;
        PerformanceScope& operator=(const PerformanceScope&) = delete;
        PerformanceScope(PerformanceScope&&) = delete;
        PerformanceScope& operator=(PerformanceScope&&) = delete;
        
        void addMetadata(const String& key, const String& value);
        void finish();
        bool isActive() const { return active; }
        
    private:
        PerformanceMonitor& monitor;
        PerformanceData data;
        bool active;
    };
    
    explicit PerformanceMonitor(bool enabled = true);
    ~PerformanceMonitor() = default;
    
    // 监控控制
    void enable() { enabled = true; }
    void disable() { enabled = false; }
    bool isEnabled() const { return enabled; }
    
    // 性能作用域
    template<typename Func>
    auto measure(const String& operationName, Func&& func) -> decltype(func()) {
        PerformanceScope scope(*this, operationName);
        return func();
    }
    
    template<typename Func>
    auto measureCategory(const String& operationName, const String& category, Func&& func) -> decltype(func()) {
        PerformanceScope scope(*this, operationName, category);
        return func();
    }
    
    // 数据访问
    std::vector<PerformanceData> getPerformanceData() const { return performanceData; }
    void clearPerformanceData() { performanceData.clear(); }
    
    // 统计分析
    struct PerformanceStats {
        double totalTime = 0.0;
        double averageTime = 0.0;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        size_t operationCount = 0;
        StringUnorderedMap categoryStats;
    };
    
    PerformanceStats getStats() const;
    PerformanceStats getStatsByCategory(const String& category) const;
    
    // 报告生成
    String generateReport() const;
    String generateCategoryReport(const String& category) const;
    void saveReport(const String& filePath) const;
    
    // 便捷方法
    PerformanceScope startMeasurement(const String& operationName, const String& category = "") {
        return PerformanceScope(*this, operationName, category);
    }
    
private:
    bool enabled;
    std::vector<PerformanceData> performanceData;
    std::mutex dataMutex;
    
    void recordPerformance(const PerformanceData& data);
    static double getCurrentTime();
    
    friend class PerformanceScope;
};

// 工厂函数
class RAIIManagerFactory {
public:
    static std::unique_ptr<RAIIStateManager> createStateManager(std::shared_ptr<StateManager> stateManager);
    static std::unique_ptr<RAIIContextManager> createContextManager(std::shared_ptr<Context> context);
    static std::unique_ptr<CombinedRAIIManager> createCombinedManager(
        std::shared_ptr<StateManager> stateManager, std::shared_ptr<Context> context);
    static std::unique_ptr<StateTracker> createStateTracker(std::shared_ptr<StateManager> stateManager);
    static std::unique_ptr<PerformanceMonitor> createPerformanceMonitor(bool enabled = true);
};

// 宏定义便捷使用
#define CHTL_STATE_GUARD(manager, state) \
    auto _state_guard = (manager).enterState(state)

#define CHTL_SCOPE_GUARD(manager, name, type) \
    auto _scope_guard = (manager).enterScope(name, type)

#define CHTL_NAMESPACE_GUARD(manager, name) \
    auto _namespace_guard = (manager).enterNamespace(name)

#define CHTL_PERFORMANCE_SCOPE(monitor, operation) \
    auto _perf_scope = (monitor).startMeasurement(operation)

#define CHTL_PERFORMANCE_CATEGORY_SCOPE(monitor, operation, category) \
    auto _perf_scope = (monitor).startMeasurement(operation, category)

} // namespace CHTL