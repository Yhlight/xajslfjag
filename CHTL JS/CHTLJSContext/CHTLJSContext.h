#ifndef CHTLJSCONTEXT_H
#define CHTLJSCONTEXT_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <variant>

namespace CHTLJS {

/**
 * @brief CHTL JS值类型枚举
 */
enum class CHTLJSValueType {
    UNDEFINED,
    NULL_VALUE,
    BOOLEAN,
    NUMBER,
    STRING,
    FUNCTION,
    OBJECT,
    ARRAY,
    DOM_ELEMENT,
    ENHANCED_SELECTOR,
    VIRTUAL_OBJECT
};

/**
 * @brief CHTL JS值结构
 */
struct CHTLJSValue {
    CHTLJSValueType type;
    std::variant<bool, double, std::string, std::function<void()>, void*> value;
    
    CHTLJSValue() : type(CHTLJSValueType::UNDEFINED) {}
    CHTLJSValue(CHTLJSValueType t) : type(t) {}
    
    template<typename T>
    CHTLJSValue(CHTLJSValueType t, const T& v) : type(t), value(v) {}
};

/**
 * @brief 增强选择器结构
 */
struct EnhancedSelector {
    std::string selector;           // 选择器文本
    std::string elementType;        // 元素类型（tag、class、id）
    std::string elementValue;       // 元素值
    bool isIndexed;                 // 是否带索引
    int index;                      // 索引值
    std::string parentSelector;     // 父选择器（用于后代选择）
    
    EnhancedSelector() : isIndexed(false), index(0) {}
};

/**
 * @brief 事件监听器结构
 */
struct EventListener {
    std::string eventType;          // 事件类型（click、mouseenter等）
    std::string callback;           // 回调函数
    bool isFunction;                // 是否为函数定义
    bool isReference;               // 是否为函数引用
    
    EventListener() : isFunction(false), isReference(false) {}
};

/**
 * @brief 事件委托结构
 */
struct EventDelegation {
    std::string parentSelector;     // 父元素选择器
    std::vector<std::string> targetSelectors; // 目标选择器列表
    std::unordered_map<std::string, std::string> eventHandlers; // 事件处理器映射
    
    EventDelegation() = default;
};

/**
 * @brief 动画关键帧结构
 */
struct AnimationKeyframe {
    double at;                      // 时间点（0.0-1.0）
    std::unordered_map<std::string, std::string> cssProperties; // CSS属性
    
    AnimationKeyframe() : at(0.0) {}
};

/**
 * @brief 动画配置结构
 */
struct AnimationConfig {
    std::string target;             // 目标选择器
    int duration;                   // 持续时间（毫秒）
    std::string easing;             // 缓动函数
    std::unordered_map<std::string, std::string> beginState; // 起始状态
    std::vector<AnimationKeyframe> keyframes; // 关键帧
    std::unordered_map<std::string, std::string> endState; // 结束状态
    int loop;                       // 循环次数（-1为无限）
    std::string direction;          // 播放方向
    int delay;                      // 延迟时间
    std::string callback;           // 回调函数
    
    AnimationConfig() : duration(100), loop(1), delay(0) {}
};

/**
 * @brief 虚对象结构
 */
struct VirtualObject {
    std::string name;               // 虚对象名称
    std::unordered_map<std::string, CHTLJSValue> properties; // 属性映射
    std::unordered_map<std::string, std::string> methods; // 方法映射
    std::string sourceCode;         // 源代码
    
    VirtualObject() = default;
};

/**
 * @brief 模块导入结构
 */
struct ModuleImport {
    std::string path;               // 模块路径
    std::string alias;              // 模块别名
    bool isLoaded;                  // 是否已加载
    
    ModuleImport() : isLoaded(false) {}
};

/**
 * @brief CHTL JS上下文管理器
 * 负责管理CHTL JS的所有核心功能
 */
class CHTLJSContext {
public:
    /**
     * @brief 构造函数
     */
    CHTLJSContext();
    
    /**
     * @brief 析构函数
     */
    ~CHTLJSContext();
    
    /**
     * @brief 解析增强选择器
     * @param selectorText 选择器文本
     * @return 增强选择器指针
     */
    std::shared_ptr<EnhancedSelector> parseEnhancedSelector(const std::string& selectorText);
    
    /**
     * @brief 创建DOM元素引用
     * @param selector 增强选择器
     * @return DOM元素引用
     */
    std::string createDOMReference(const std::shared_ptr<EnhancedSelector>& selector);
    
    /**
     * @brief 解析事件绑定操作符
     * @param selector 选择器
     * @param eventType 事件类型
     * @param callback 回调函数
     * @return 生成的JavaScript代码
     */
    std::string parseEventBindingOperator(const std::string& selector, const std::string& eventType, const std::string& callback);
    
    /**
     * @brief 解析增强监听器
     * @param selector 选择器
     * @param listeners 监听器列表
     * @return 生成的JavaScript代码
     */
    std::string parseEnhancedListener(const std::string& selector, const std::vector<EventListener>& listeners);
    
    /**
     * @brief 解析事件委托
     * @param delegation 事件委托配置
     * @return 生成的JavaScript代码
     */
    std::string parseEventDelegation(const EventDelegation& delegation);
    
    /**
     * @brief 解析动画配置
     * @param config 动画配置
     * @return 生成的JavaScript代码
     */
    std::string parseAnimation(const AnimationConfig& config);
    
    /**
     * @brief 创建虚对象
     * @param name 虚对象名称
     * @param sourceCode 源代码
     * @return 虚对象指针
     */
    std::shared_ptr<VirtualObject> createVirtualObject(const std::string& name, const std::string& sourceCode);
    
    /**
     * @brief 解析虚对象访问
     * @param objectName 对象名称
     * @param propertyName 属性名称
     * @return 生成的JavaScript代码
     */
    std::string parseVirtualObjectAccess(const std::string& objectName, const std::string& propertyName);
    
    /**
     * @brief 解析模块导入
     * @param imports 导入列表
     * @return 生成的JavaScript代码
     */
    std::string parseModuleImports(const std::vector<ModuleImport>& imports);
    
    /**
     * @brief 验证选择器语法
     * @param selectorText 选择器文本
     * @return 是否有效
     */
    bool validateSelector(const std::string& selectorText) const;
    
    /**
     * @brief 获取选择器类型
     * @param selectorText 选择器文本
     * @return 选择器类型字符串
     */
    std::string getSelectorType(const std::string& selectorText) const;
    
    /**
     * @brief 生成JavaScript代码
     * @param context 上下文信息
     * @return 生成的JavaScript代码
     */
    std::string generateJavaScript(const std::string& context);
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;
    
    /**
     * @brief 清除统计信息
     */
    void clearStatistics();

private:
    // 统计信息
    int totalSelectorsProcessed_;
    int totalEventListeners_;
    int totalAnimations_;
    int totalVirtualObjects_;
    int totalModuleImports_;
    
    // 内部存储
    std::unordered_map<std::string, std::shared_ptr<VirtualObject>> virtualObjects_;
    std::vector<EventDelegation> eventDelegations_;
    std::vector<ModuleImport> moduleImports_;
    
    /**
     * @brief 解析选择器组件
     * @param selectorText 选择器文本
     * @return 解析结果
     */
    std::vector<std::string> parseSelectorComponents(const std::string& selectorText) const;
    
    /**
     * @brief 检查选择器是否为索引选择器
     * @param selectorText 选择器文本
     * @return 是否为索引选择器
     */
    bool isIndexedSelector(const std::string& selectorText) const;
    
    /**
     * @brief 提取索引值
     * @param selectorText 选择器文本
     * @return 索引值
     */
    int extractIndex(const std::string& selectorText) const;
    
    /**
     * @brief 更新统计信息
     * @param type 统计类型
     */
    void updateStatistics(const std::string& type);
};

} // namespace CHTLJS

#endif // CHTLJSCONTEXT_H