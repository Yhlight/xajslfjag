#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"

namespace CHTL {

// CHTL JS节点基类
class CHTLJSNode : public BaseNode {
public:
    String jsContent;
    StringUnorderedMap jsMetadata;
    bool isVirtualObject;
    
    CHTLJSNode(NodeType type, const String& content, const Position& pos = Position())
        : BaseNode(type, content, pos), jsContent(content), isVirtualObject(false) {}
    
    // 基础操作
    void setJSContent(const String& content) { jsContent = content; value = content; }
    String getJSContent() const { return jsContent; }
    
    void setJSMetadata(const String& key, const String& value) { jsMetadata[key] = value; }
    String getJSMetadata(const String& key, const String& defaultValue = "") const {
        auto it = jsMetadata.find(key);
        return (it != jsMetadata.end()) ? it->second : defaultValue;
    }
    
    void setVirtualObject(bool isVirtual) { isVirtualObject = isVirtual; }
    bool getVirtualObject() const { return isVirtualObject; }
    
    // 生成JavaScript代码
    virtual String generateJavaScript() const = 0;
    
    // 验证CHTL JS语法
    virtual bool validateCHTLJSSyntax() const = 0;
    virtual StringVector getCHTLJSValidationErrors() const = 0;
    
    // 重写基类方法
    String toString() const override;
    bool validate() const override;
};

// CHTL JS选择器节点 {{...}}
class CHTLJSSelectorNode : public CHTLJSNode {
public:
    String selectorContent;
    String selectorType;  // class, id, tag, complex
    bool isArrayAccess;   // {{button[0]}}
    int arrayIndex;
    
    explicit CHTLJSSelectorNode(const String& selector, const Position& pos = Position())
        : CHTLJSNode(NodeType::CHTLJS_SELECTOR, selector, pos), selectorContent(selector), 
          isArrayAccess(false), arrayIndex(-1) {
        parseSelectorContent();
    }
    
    // 选择器解析
    void parseSelectorContent();
    String getSelectorName() const;
    String getSelectorPrefix() const;  // ., #, 或空
    
    // 数组访问
    void setArrayAccess(int index) { isArrayAccess = true; arrayIndex = index; }
    bool hasArrayAccess() const { return isArrayAccess; }
    int getArrayIndex() const { return arrayIndex; }
    
    // 选择器类型检查
    bool isClassSelector() const { return selectorType == "class"; }
    bool isIdSelector() const { return selectorType == "id"; }
    bool isTagSelector() const { return selectorType == "tag"; }
    bool isComplexSelector() const { return selectorType == "complex"; }
    
    // JavaScript生成
    String generateJavaScript() const override;
    String generateDOMQuery() const;
    String generateElementReference() const;
    
    // 验证
    bool validateCHTLJSSyntax() const override;
    StringVector getCHTLJSValidationErrors() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// CHTL JS函数节点
class CHTLJSFunctionNode : public CHTLJSNode {
public:
    String functionName;
    StringUnorderedMap parameters;
    StringUnorderedMap keyValuePairs;
    bool hasUnorderedKeys;
    bool hasOptionalKeys;
    
    CHTLJSFunctionNode(const String& name, const String& content, const Position& pos = Position())
        : CHTLJSNode(NodeType::CHTLJS_FUNCTION, content, pos), functionName(name),
          hasUnorderedKeys(false), hasOptionalKeys(false) {}
    
    // 函数信息
    void setFunctionName(const String& name) { functionName = name; }
    String getFunctionName() const { return functionName; }
    
    // 参数管理
    void setParameter(const String& key, const String& value);
    String getParameter(const String& key, const String& defaultValue = "") const;
    bool hasParameter(const String& key) const;
    void removeParameter(const String& key);
    StringVector getParameterNames() const;
    
    // 键值对管理（CHTL JS特殊语法）
    void setKeyValuePair(const String& key, const String& value, bool isOptional = false);
    String getKeyValuePair(const String& key) const;
    bool hasKeyValuePair(const String& key) const;
    bool isOptionalKey(const String& key) const;
    
    // 无序和可选功能
    void enableUnorderedKeys() { hasUnorderedKeys = true; }
    void enableOptionalKeys() { hasOptionalKeys = true; }
    bool supportsUnorderedKeys() const { return hasUnorderedKeys; }
    bool supportsOptionalKeys() const { return hasOptionalKeys; }
    
    // JavaScript生成
    String generateJavaScript() const override;
    String generateFunctionCall() const;
    String generateParameterList() const;
    
    // 验证
    bool validateCHTLJSSyntax() const override;
    StringVector getCHTLJSValidationErrors() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 监听器节点
class CHTLJSListenNode : public CHTLJSFunctionNode {
public:
    StringUnorderedMap eventHandlers;
    StringVector supportedEvents;
    
    explicit CHTLJSListenNode(const String& content, const Position& pos = Position())
        : CHTLJSFunctionNode("listen", content, pos) {
        initializeSupportedEvents();
    }
    
    // 事件处理器管理
    void setEventHandler(const String& event, const String& handler);
    String getEventHandler(const String& event) const;
    bool hasEventHandler(const String& event) const;
    void removeEventHandler(const String& event);
    StringVector getEventNames() const;
    
    // 支持的事件
    void initializeSupportedEvents();
    bool isSupportedEvent(const String& event) const;
    StringVector getSupportedEvents() const { return supportedEvents; }
    
    // JavaScript生成
    String generateJavaScript() const override;
    String generateEventListeners() const;
    String generateEventHandler(const String& event, const String& handler) const;
    
    // 验证
    bool validateCHTLJSSyntax() const override;
    StringVector getCHTLJSValidationErrors() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 动画节点
class CHTLJSAnimateNode : public CHTLJSFunctionNode {
public:
    String target;
    int duration;
    String easing;
    StringUnorderedMap beginState;
    StringUnorderedMap endState;
    std::vector<KeyFrame> keyFrames;
    int loopCount;
    String direction;
    int delay;
    String callback;
    
    struct KeyFrame {
        float at;  // 0.0 to 1.0
        StringUnorderedMap properties;
    };
    
    explicit CHTLJSAnimateNode(const String& content, const Position& pos = Position())
        : CHTLJSFunctionNode("animate", content, pos), duration(1000), loopCount(1), delay(0),
          easing("ease") {}
    
    // 动画属性
    void setTarget(const String& targetSelector) { target = targetSelector; }
    String getTarget() const { return target; }
    
    void setDuration(int durationMs) { duration = durationMs; }
    int getDuration() const { return duration; }
    
    void setEasing(const String& easingFunction) { easing = easingFunction; }
    String getEasing() const { return easing; }
    
    // 状态管理
    void setBeginState(const StringUnorderedMap& state) { beginState = state; }
    void setEndState(const StringUnorderedMap& state) { endState = state; }
    StringUnorderedMap getBeginState() const { return beginState; }
    StringUnorderedMap getEndState() const { return endState; }
    
    // 关键帧管理
    void addKeyFrame(float at, const StringUnorderedMap& properties);
    void removeKeyFrame(float at);
    std::vector<KeyFrame> getKeyFrames() const { return keyFrames; }
    void clearKeyFrames() { keyFrames.clear(); }
    
    // 动画控制
    void setLoop(int count) { loopCount = count; }
    void setDirection(const String& dir) { direction = dir; }
    void setDelay(int delayMs) { delay = delayMs; }
    void setCallback(const String& callbackFunc) { callback = callbackFunc; }
    
    // JavaScript生成
    String generateJavaScript() const override;
    String generateKeyFramesCSS() const;
    String generateAnimationCSS() const;
    String generateRequestAnimationFrame() const;
    
    // 验证
    bool validateCHTLJSSyntax() const override;
    StringVector getCHTLJSValidationErrors() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 事件委托节点
class CHTLJSDelegateNode : public CHTLJSFunctionNode {
public:
    String parentSelector;
    StringVector targetSelectors;
    StringUnorderedMap eventHandlers;
    
    explicit CHTLJSDelegateNode(const String& content, const Position& pos = Position())
        : CHTLJSFunctionNode("delegate", content, pos) {}
    
    // 委托设置
    void setParentSelector(const String& parent) { parentSelector = parent; }
    String getParentSelector() const { return parentSelector; }
    
    void addTargetSelector(const String& target);
    void removeTargetSelector(const String& target);
    bool hasTargetSelector(const String& target) const;
    StringVector getTargetSelectors() const { return targetSelectors; }
    
    // 事件处理器
    void setEventHandler(const String& event, const String& handler);
    String getEventHandler(const String& event) const;
    bool hasEventHandler(const String& event) const;
    StringVector getEventNames() const;
    
    // JavaScript生成
    String generateJavaScript() const override;
    String generateDelegationSetup() const;
    String generateEventDelegation(const String& event, const String& handler) const;
    
    // 验证
    bool validateCHTLJSSyntax() const override;
    StringVector getCHTLJSValidationErrors() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 虚对象节点
class CHTLJSVirObjectNode : public CHTLJSNode {
public:
    String objectName;
    String objectType;  // listen, animate, delegate, iNeverAway等
    StringUnorderedMap objectMembers;
    StringUnorderedMap memberTypes;  // function, object, array等
    
    CHTLJSVirObjectNode(const String& name, const String& type, const String& content, const Position& pos = Position())
        : CHTLJSNode(NodeType::CHTLJS_VIR_OBJECT, content, pos), objectName(name), objectType(type) {
        isVirtualObject = true;
    }
    
    // 虚对象信息
    void setObjectName(const String& name) { objectName = name; }
    String getObjectName() const { return objectName; }
    
    void setObjectType(const String& type) { objectType = type; }
    String getObjectType() const { return objectType; }
    
    // 成员管理
    void addMember(const String& name, const String& content, const String& type = "function");
    void removeMember(const String& name);
    bool hasMember(const String& name) const;
    String getMember(const String& name) const;
    String getMemberType(const String& name) const;
    StringVector getMemberNames() const;
    
    // 成员访问
    String resolveMemberAccess(const String& memberName) const;
    bool isFunctionMember(const String& memberName) const;
    bool isObjectMember(const String& memberName) const;
    
    // JavaScript生成
    String generateJavaScript() const override;
    String generateMemberAccess(const String& memberName) const;
    String generateObjectDefinition() const;
    
    // 验证
    bool validateCHTLJSSyntax() const override;
    StringVector getCHTLJSValidationErrors() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 事件绑定操作符节点 &->
class CHTLJSEventBindNode : public CHTLJSNode {
public:
    String targetSelector;
    String eventType;
    String eventHandler;
    
    explicit CHTLJSEventBindNode(const String& content, const Position& pos = Position())
        : CHTLJSNode(NodeType::CHTLJS_FUNCTION, content, pos) {
        parseEventBinding();
    }
    
    // 解析事件绑定
    void parseEventBinding();
    
    // 事件绑定信息
    void setTarget(const String& target) { targetSelector = target; }
    String getTarget() const { return targetSelector; }
    
    void setEventType(const String& event) { eventType = event; }
    String getEventType() const { return eventType; }
    
    void setEventHandler(const String& handler) { eventHandler = handler; }
    String getEventHandler() const { return eventHandler; }
    
    // JavaScript生成
    String generateJavaScript() const override;
    String generateEventBinding() const;
    
    // 验证
    bool validateCHTLJSSyntax() const override;
    StringVector getCHTLJSValidationErrors() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// CHTL JS工厂
class CHTLJSNodeFactory {
public:
    static std::unique_ptr<CHTLJSSelectorNode> createSelectorNode(const String& selector);
    static std::unique_ptr<CHTLJSFunctionNode> createFunctionNode(const String& functionName, const String& content);
    static std::unique_ptr<CHTLJSListenNode> createListenNode(const String& content);
    static std::unique_ptr<CHTLJSAnimateNode> createAnimateNode(const String& content);
    static std::unique_ptr<CHTLJSDelegateNode> createDelegateNode(const String& content);
    static std::unique_ptr<CHTLJSVirObjectNode> createVirObjectNode(const String& name, const String& type, const String& content);
    static std::unique_ptr<CHTLJSEventBindNode> createEventBindNode(const String& content);
    
    // 从内容解析创建
    static std::unique_ptr<CHTLJSNode> createFromContent(const String& content);
    static NodeType detectCHTLJSNodeType(const String& content);
    
    // 批量创建
    static std::vector<std::unique_ptr<CHTLJSNode>> createFromScript(const String& scriptContent);
};

// CHTL JS验证器
class CHTLJSValidator {
public:
    // 语法验证
    static bool validateSelectorSyntax(const String& selector);
    static bool validateFunctionSyntax(const String& functionName, const String& content);
    static bool validateEventSyntax(const String& event);
    static bool validateJavaScriptExpression(const String& expression);
    
    // 详细验证
    static StringVector getSelectorValidationErrors(const String& selector);
    static StringVector getFunctionValidationErrors(const String& functionName, const String& content);
    static StringVector getEventValidationErrors(const String& event);
    
    // 安全验证
    static bool hasSafetyIssues(const String& jsContent);
    static StringVector getSafetyWarnings(const String& jsContent);
    
private:
    static bool isValidSelector(const String& selector);
    static bool isValidJavaScriptIdentifier(const String& identifier);
    static bool isValidEventName(const String& eventName);
};

} // namespace CHTL