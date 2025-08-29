#pragma once
#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

/**
 * 脚本节点类型
 */
enum class ScriptType {
    LOCAL_SCRIPT,       // 局部脚本 script {}
    GLOBAL_SCRIPT,      // 全局脚本
    CHTL_JS,           // CHTL JS扩展语法
    VANILLA_JS          // 原生JavaScript
};

/**
 * CHTL JS选择器类型
 */
enum class SelectorType {
    TAG,               // {{tag}}
    CLASS,             // {{.className}}
    ID,                // {{#idName}}
    COMPLEX,           // {{.class tag}}
    INDEXED,           // {{tag[0]}}
    REFERENCE          // {{&}}
};

/**
 * CHTL JS选择器结构
 */
struct CHTLJSSelector {
    SelectorType type;
    std::string selector;
    int index;         // 用于索引选择器
    
    CHTLJSSelector() : type(SelectorType::TAG), index(-1) {}
    CHTLJSSelector(SelectorType t, const std::string& sel, int idx = -1) 
        : type(t), selector(sel), index(idx) {}
    
    std::string toString() const;
};

/**
 * 脚本节点
 * 表示CHTL中的script{}块和CHTL JS扩展语法
 */
class ScriptNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    ScriptNode(ScriptType type = ScriptType::LOCAL_SCRIPT);
    
    /**
     * 析构函数
     */
    virtual ~ScriptNode() = default;
    
    /**
     * 获取脚本类型
     */
    ScriptType getScriptType() const;
    
    /**
     * 设置脚本类型
     */
    void setScriptType(ScriptType type);
    
    /**
     * 获取脚本内容
     */
    const std::string& getContent() const;
    
    /**
     * 设置脚本内容
     */
    void setContent(const std::string& content);
    
    /**
     * 添加脚本行
     */
    void addLine(const std::string& line);
    
    /**
     * 获取引用的选择器
     */
    const std::vector<CHTLJSSelector>& getReferencedSelectors() const;
    
    /**
     * 添加引用的选择器
     */
    void addReferencedSelector(const CHTLJSSelector& selector);
    
    /**
     * 检查是否包含CHTL JS语法
     */
    bool containsCHTLJS() const;
    
    /**
     * 设置是否包含CHTL JS语法
     */
    void setContainsCHTLJS(bool contains);
    
    /**
     * 获取自动添加的类名/ID
     */
    const std::vector<std::string>& getAutoAddedClasses() const;
    const std::vector<std::string>& getAutoAddedIds() const;
    
    /**
     * 添加自动类名/ID
     */
    void addAutoClass(const std::string& className);
    void addAutoId(const std::string& idName);
    
    /**
     * 处理CHTL JS选择器自动化
     */
    void processSelectorAutomation();
    
    /**
     * 转换CHTL JS语法为标准JavaScript
     */
    std::string convertToStandardJS() const;
    
    // BaseNode 接口实现
    CHTLNodeType getNodeType() const { return CHTLNodeType::SCRIPT_NODE; }
    std::string toString() const override;
    NodePtr clone() const override;
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

private:
    ScriptType m_scriptType;                              // 脚本类型
    std::string m_content;                                // 脚本内容
    std::vector<CHTLJSSelector> m_referencedSelectors;   // 引用的选择器
    bool m_containsCHTLJS;                                // 是否包含CHTL JS语法
    std::vector<std::string> m_autoAddedClasses;         // 自动添加的类名
    std::vector<std::string> m_autoAddedIds;             // 自动添加的ID
    
    /**
     * 解析CHTL JS选择器
     */
    void parseCHTLJSSelectors();
    
    /**
     * 解析单个选择器
     */
    CHTLJSSelector parseSingleSelector(const std::string& selectorText) const;
    
public:
    /**
     * 转换选择器为JavaScript DOM选择器
     */
    std::string convertSelectorToJS(const CHTLJSSelector& selector) const;
};

/**
 * CHTL JS监听器节点
 * 表示listen {}语法
 */
class CHTLJSListenerNode : public ScriptNode {
public:
    /**
     * 构造函数
     */
    CHTLJSListenerNode();
    
    /**
     * 添加事件监听器
     */
    void addEventListener(const std::string& event, const std::string& handler);
    
    /**
     * 获取事件监听器
     */
    const std::unordered_map<std::string, std::string>& getEventListeners() const;
    
    /**
     * 转换为标准JavaScript事件绑定
     */
    std::string convertToStandardEventBinding() const;
    
    CHTLNodeType getNodeType() const { return CHTLNodeType::CHTLJS_LISTENER_NODE; }

private:
    std::unordered_map<std::string, std::string> m_eventListeners;
};

/**
 * CHTL JS动画节点
 * 表示animate {}语法
 */
class CHTLJSAnimationNode : public ScriptNode {
public:
    /**
     * 构造函数
     */
    CHTLJSAnimationNode();
    
    /**
     * 设置动画属性
     */
    void setTarget(const CHTLJSSelector& target);
    void setDuration(int duration);
    void setEasing(const std::string& easing);
    void setLoop(int loop);
    void setDelay(int delay);
    
    /**
     * 添加关键帧
     */
    void addKeyframe(double at, const std::unordered_map<std::string, std::string>& styles);
    
    /**
     * 转换为标准JavaScript动画
     */
    std::string convertToStandardAnimation() const;
    
    CHTLNodeType getNodeType() const { return CHTLNodeType::CHTLJS_ANIMATION_NODE; }

private:
    CHTLJSSelector m_target;
    int m_duration;
    std::string m_easing;
    int m_loop;
    int m_delay;
    std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> m_keyframes;
};

/**
 * CHTL JS委托节点
 * 表示delegate {}语法
 */
class CHTLJSDelegateNode : public ScriptNode {
public:
    /**
     * 构造函数
     */
    CHTLJSDelegateNode();
    
    /**
     * 设置委托目标
     */
    void setDelegateTarget(const std::vector<CHTLJSSelector>& targets);
    
    /**
     * 添加委托事件
     */
    void addDelegateEvent(const std::string& event, const std::string& handler);
    
    /**
     * 转换为标准JavaScript事件委托
     */
    std::string convertToStandardDelegate() const;
    
    CHTLNodeType getNodeType() const { return CHTLNodeType::CHTLJS_DELEGATE_NODE; }

private:
    std::vector<CHTLJSSelector> m_delegateTargets;
    std::unordered_map<std::string, std::string> m_delegateEvents;
};

} // namespace CHTL