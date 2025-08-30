#pragma once

#include "../../Util/Common.h"
#include "../../CHTL/CHTLNode/BaseNode.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTLJS {

// 引入CHTL的类型定义
using namespace CHTL;

// CHTL JS 增强语法节点类型
enum class CHTLJSNodeType {
    LISTEN,           // listen事件监听
    DELEGATE,         // delegate事件委托
    ANIMATE,          // animate动画
    VIR,              // vir虚对象
    MODULE,           // module模块加载
    SELECTOR,         // {{selector}}增强选择器
    EVENT_BIND,       // &->事件绑定
    TWO_WAY_BIND,     // <->双向绑定
    COMPUTED,         // =>计算属性
    CONDITIONAL,      // ?:条件渲染
    LIST_RENDER,      // *:列表渲染
    WATCHER,          // ~>监听器
    STREAM            // |>事件流
};

// CHTL JS 增强语法基础节点
class CHTLJSEnhancedNode : public CHTL::BaseNode {
public:
    CHTLJSNodeType jsType;
    
    explicit CHTLJSEnhancedNode(CHTLJSNodeType type, const String& value = "", const CHTL::Position& pos = CHTL::Position());
    virtual ~CHTLJSEnhancedNode() = default;
    
    // 获取CHTL JS节点类型
    CHTLJSNodeType getCHTLJSType() const { return jsType; }
    
    // 生成JS代码
    virtual String generateJavaScript() const = 0;
    
    // 检查语法有效性
    virtual bool isValidSyntax() const = 0;
    
    // 工具方法
    String jsTypeToString() const;
    static String nodeTypeToString(CHTLJSNodeType type);
};

// Listen 节点 - 事件监听器
class ListenNode : public CHTLJSEnhancedNode {
public:
    String targetSelector;                     // 目标选择器
    std::unordered_map<String, String> events; // 事件映射
    bool isDeclarative;                       // 是否声明式语法
    
    explicit ListenNode(const CHTL::Position& pos = CHTL::Position());
    ~ListenNode() override = default;
    
    // 添加事件
    void addEvent(const String& eventType, const String& handler);
    void setTargetSelector(const String& selector);
    void setDeclarative(bool declarative) { isDeclarative = declarative; }
    
    // 检查事件
    bool hasEvent(const String& eventType) const;
    size_t getEventCount() const { return events.size(); }
    
    // 实现接口
    String generateJavaScript() const override;
    bool isValidSyntax() const override;
    std::unique_ptr<CHTL::BaseNode> clone() const override;
    
    // 静态工厂方法
    static std::unique_ptr<ListenNode> createListen(const String& selector, const CHTL::Position& pos = CHTL::Position());
    
    // 解析语法
    static std::unordered_map<String, String> parseEventMap(const String& eventMapString);
    static bool isListenSyntax(const String& input);
    
private:
    bool isValidEventType(const String& eventType) const;
    bool isValidHandler(const String& handler) const;
};

// Delegate 节点 - 事件委托
class DelegateNode : public CHTLJSEnhancedNode {
public:
    String parentSelector;                     // 父元素选择器
    StringVector targetSelectors;              // 目标子元素选择器列表
    std::unordered_map<String, String> events; // 事件映射
    
    explicit DelegateNode(const CHTL::Position& pos = CHTL::Position());
    ~DelegateNode() override = default;
    
    // 设置委托信息
    void setParentSelector(const String& selector);
    void addTargetSelector(const String& selector);
    void setTargetSelectors(const StringVector& selectors);
    void addEvent(const String& eventType, const String& handler);
    
    // 实现接口
    String generateJavaScript() const override;
    bool isValidSyntax() const override;
    std::unique_ptr<CHTL::BaseNode> clone() const override;
    
    // 静态工厂方法
    static std::unique_ptr<DelegateNode> createDelegate(const String& parentSelector, const CHTL::Position& pos = CHTL::Position());
    
    // 解析语法
    static StringVector parseTargetList(const String& targetString);
    static bool isDelegateSyntax(const String& input);
    
private:
    String generateDelegateRegistration() const;
    bool isValidSelector(const String& selector) const;
};

// Animate 节点 - 动画
class AnimateNode : public CHTLJSEnhancedNode {
public:
    String targetSelector;                     // 目标选择器
    int duration;                             // 持续时间 (ms)
    String easing;                            // 缓动函数
    std::unordered_map<String, String> beginState;  // 起始状态
    std::unordered_map<String, String> endState;    // 结束状态
    std::vector<std::pair<double, std::unordered_map<String, String>>> keyframes; // 关键帧
    int loop;                                 // 循环次数 (-1为无限)
    String direction;                         // 播放方向
    int delay;                                // 延迟 (ms)
    String callback;                          // 完成回调
    
    explicit AnimateNode(const CHTL::Position& pos = CHTL::Position());
    ~AnimateNode() override = default;
    
    // 设置动画属性
    void setTarget(const String& selector);
    void setDuration(int ms) { duration = ms; }
    void setEasing(const String& easingFunc) { easing = easingFunc; }
    void setLoop(int loopCount) { loop = loopCount; }
    void setDirection(const String& dir) { direction = dir; }
    void setDelay(int ms) { delay = ms; }
    void setCallback(const String& cb) { callback = cb; }
    
    // 设置状态
    void setBeginState(const std::unordered_map<String, String>& state);
    void setEndState(const std::unordered_map<String, String>& state);
    void addKeyframe(double at, const std::unordered_map<String, String>& state);
    
    // 实现接口
    String generateJavaScript() const override;
    bool isValidSyntax() const override;
    std::unique_ptr<CHTL::BaseNode> clone() const override;
    
    // 静态工厂方法
    static std::unique_ptr<AnimateNode> createAnimate(const String& selector, const CHTL::Position& pos = CHTL::Position());
    
    // 解析语法
    static std::unordered_map<String, String> parseStyleState(const String& stateString);
    static bool isAnimateSyntax(const String& input);
    
private:
    String generateKeyframesCSS() const;
    String generateAnimationJS() const;
    bool isValidCSSProperty(const String& property) const;
};

// Vir 节点 - 虚对象
class VirNode : public CHTLJSEnhancedNode {
public:
    String objectName;                         // 虚对象名称
    CHTLJSNodeType sourceType;                // 源类型 (LISTEN, ANIMATE等)
    std::unordered_map<String, String> properties; // 属性映射
    std::unordered_map<String, String> methods;    // 方法映射
    bool isCompileTime;                       // 是否编译时处理
    
    explicit VirNode(const String& name, const CHTL::Position& pos = CHTL::Position());
    ~VirNode() override = default;
    
    // 设置虚对象信息
    void setSourceType(CHTLJSNodeType type) { sourceType = type; }
    void addProperty(const String& name, const String& value);
    void addMethod(const String& name, const String& signature);
    void setCompileTimeProcessing(bool compileTime) { isCompileTime = compileTime; }
    
    // 属性访问
    bool hasProperty(const String& name) const;
    bool hasMethod(const String& name) const;
    String getProperty(const String& name) const;
    String getMethod(const String& name) const;
    
    // 实现接口
    String generateJavaScript() const override;
    bool isValidSyntax() const override;
    std::unique_ptr<CHTL::BaseNode> clone() const override;
    
    // 静态工厂方法
    static std::unique_ptr<VirNode> createVir(const String& name, CHTLJSNodeType sourceType, const CHTL::Position& pos = CHTL::Position());
    
    // 解析语法
    static std::pair<String, CHTLJSNodeType> parseVirDeclaration(const String& declaration);
    static bool isVirSyntax(const String& input);
    
private:
    String generatePropertyAccess(const String& propertyName) const;
    String generateMethodCall(const String& methodName) const;
    bool isValidObjectName(const String& name) const;
};

// Module 节点 - 模块加载
class ModuleNode : public CHTLJSEnhancedNode {
public:
    StringVector loadPaths;                   // 加载路径列表
    bool isChainSyntax;                       // 是否链式语法
    String loadMode;                          // 加载模式 (parallel, sequential)
    
    explicit ModuleNode(const CHTL::Position& pos = CHTL::Position());
    ~ModuleNode() override = default;
    
    // 添加加载路径
    void addLoadPath(const String& path);
    void setLoadPaths(const StringVector& paths);
    void setChainSyntax(bool chain) { isChainSyntax = chain; }
    void setLoadMode(const String& mode) { loadMode = mode; }
    
    // 实现接口
    String generateJavaScript() const override;
    bool isValidSyntax() const override;
    std::unique_ptr<CHTL::BaseNode> clone() const override;
    
    // 静态工厂方法
    static std::unique_ptr<ModuleNode> createModule(const CHTL::Position& pos = CHTL::Position());
    
    // 解析语法
    static StringVector parseLoadPaths(const String& pathString);
    static bool isModuleSyntax(const String& input);
    
private:
    String generateAMDLoader() const;
    bool isValidPath(const String& path) const;
};

// Selector 节点 - 增强选择器
class SelectorNode : public CHTLJSEnhancedNode {
public:
    String selector;                          // CSS选择器
    String accessMethod;                      // 访问方法
    bool isIndexed;                          // 是否索引访问
    size_t index;                            // 索引值
    bool isPreciseAccess;                    // 是否精确访问
    
    explicit SelectorNode(const String& sel, const CHTL::Position& pos = CHTL::Position());
    ~SelectorNode() override = default;
    
    // 设置选择器信息
    void setAccessMethod(const String& method);
    void setIndexedAccess(size_t idx);
    void setPreciseAccess(bool precise) { isPreciseAccess = precise; }
    
    // 实现接口
    String generateJavaScript() const override;
    bool isValidSyntax() const override;
    std::unique_ptr<CHTL::BaseNode> clone() const override;
    
    // 静态工厂方法
    static std::unique_ptr<SelectorNode> createSelector(const String& selector, const CHTL::Position& pos = CHTL::Position());
    
    // 解析语法
    static std::pair<String, size_t> parseIndexedSelector(const String& selectorString);
    static bool isSelectorSyntax(const String& input);
    static bool isValidCSSSelector(const String& selector);
    
private:
    String generateDOMQuery() const;
    String optimizeSelector() const;
};

} // namespace CHTLJS