#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>

namespace CHTLJS {

// 前向声明
class BaseNode;
class VirtualObjectNode;
class EnhancedSelectorNode;
class EventBindingNode;
class ListenBlockNode;
class DelegateBlockNode;
class AnimateBlockNode;
class INeverAwayNode;
class PrintMyloveNode;
class ModuleNode;
class ScriptBlockNode;
class FunctionNode;
class ObjectNode;
class ArrayNode;

// 节点类型枚举
enum class NodeType {
    BASE,
    VIRTUAL_OBJECT,
    ENHANCED_SELECTOR,
    EVENT_BINDING,
    LISTEN_BLOCK,
    DELEGATE_BLOCK,
    ANIMATE_BLOCK,
    I_NEVER_AWAY,
    PRINT_MY_LOVE,
    MODULE,
    SCRIPT_BLOCK,
    FUNCTION,
    OBJECT,
    ARRAY,
    ROOT
};

// 基础节点类
class BaseNode {
public:
    BaseNode(NodeType type, size_t line = 0, size_t column = 0);
    virtual ~BaseNode() = default;
    
    NodeType getType() const { return type_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    void setLine(size_t line) { line_ = line; }
    void setColumn(size_t column) { column_ = column; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children_; }
    
    // 父节点管理
    void setParent(std::shared_ptr<BaseNode> parent);
    std::shared_ptr<BaseNode> getParent() const { return parent_.lock(); }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getAllAttributes() const { return attributes_; }
    
    // 虚拟函数
    virtual std::string toString() const;
    virtual std::string toJavaScript() const = 0;
    
protected:
    NodeType type_;
    size_t line_;
    size_t column_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::weak_ptr<BaseNode> parent_;
    std::unordered_map<std::string, std::string> attributes_;
};

// 根节点
class RootNode : public BaseNode {
public:
    RootNode();
    std::string toJavaScript() const override;
    std::string toString() const override;
};

// 虚对象节点
class VirtualObjectNode : public BaseNode {
public:
    VirtualObjectNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    std::string getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // 属性管理
    void addProperty(const std::string& key, const std::string& value);
    void addMethod(const std::string& key, const std::string& value);
    const std::unordered_map<std::string, std::string>& getProperties() const { return properties_; }
    const std::unordered_map<std::string, std::string>& getMethods() const { return methods_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::string name_;
    std::unordered_map<std::string, std::string> properties_;
    std::unordered_map<std::string, std::string> methods_;
};

// 增强选择器节点
class EnhancedSelectorNode : public BaseNode {
public:
    EnhancedSelectorNode(const std::string& selector, size_t line = 0, size_t column = 0);
    
    std::string getSelector() const { return selector_; }
    void setSelector(const std::string& selector) { selector_ = selector; }
    
    // 选择器类型分析
    std::string getElementType() const;  // tag, class, id
    std::string getElementValue() const;
    bool isIndexed() const;
    int getIndex() const;
    std::string getParentSelector() const;
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::string selector_;
    
    // 解析选择器
    void parseSelector();
    std::string elementType_;
    std::string elementValue_;
    bool isIndexed_;
    int index_;
    std::string parentSelector_;
};

// 事件绑定节点
class EventBindingNode : public BaseNode {
public:
    EventBindingNode(const std::string& selector, const std::string& eventType, 
                    const std::string& callback, size_t line = 0, size_t column = 0);
    
    std::string getSelector() const { return selector_; }
    std::string getEventType() const { return eventType_; }
    std::string getCallback() const { return callback_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::string selector_;
    std::string eventType_;
    std::string callback_;
};

// 监听块节点
class ListenBlockNode : public BaseNode {
public:
    ListenBlockNode(const std::string& selector, size_t line = 0, size_t column = 0);
    
    std::string getSelector() const { return selector_; }
    
    // 事件监听器管理
    void addEventListener(const std::string& eventType, const std::string& callback);
    const std::unordered_map<std::string, std::string>& getEventListeners() const { return eventListeners_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::string selector_;
    std::unordered_map<std::string, std::string> eventListeners_;
};

// 委托块节点
class DelegateBlockNode : public BaseNode {
public:
    DelegateBlockNode(const std::string& parentSelector, size_t line = 0, size_t column = 0);
    
    std::string getParentSelector() const { return parentSelector_; }
    
    // 目标选择器管理
    void addTargetSelector(const std::string& targetSelector);
    void setTargetSelectors(const std::vector<std::string>& targets);
    const std::vector<std::string>& getTargetSelectors() const { return targetSelectors_; }
    
    // 事件处理器管理
    void addEventHandler(const std::string& eventType, const std::string& handler);
    const std::unordered_map<std::string, std::string>& getEventHandlers() const { return eventHandlers_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::string parentSelector_;
    std::vector<std::string> targetSelectors_;
    std::unordered_map<std::string, std::string> eventHandlers_;
};

// 动画块节点
class AnimateBlockNode : public BaseNode {
public:
    AnimateBlockNode(size_t line = 0, size_t column = 0);
    
    // 动画配置管理
    void setTarget(const std::string& target);
    void setDuration(int duration);
    void setEasing(const std::string& easing);
    void setLoop(int loop);
    void setDirection(const std::string& direction);
    void setDelay(int delay);
    void setCallback(const std::string& callback);
    
    // 状态管理
    void setBeginState(const std::unordered_map<std::string, std::string>& state);
    void setEndState(const std::unordered_map<std::string, std::string>& state);
    void addKeyframe(double at, const std::unordered_map<std::string, std::string>& cssProperties);
    
    std::string getTarget() const { return target_; }
    int getDuration() const { return duration_; }
    std::string getEasing() const { return easing_; }
    int getLoop() const { return loop_; }
    std::string getDirection() const { return direction_; }
    int getDelay() const { return delay_; }
    std::string getCallback() const { return callback_; }
    
    const std::unordered_map<std::string, std::string>& getBeginState() const { return beginState_; }
    const std::unordered_map<std::string, std::string>& getEndState() const { return endState_; }
    const std::vector<std::pair<double, std::unordered_map<std::string, std::string>>>& getKeyframes() const { return keyframes_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::string target_;
    int duration_;
    std::string easing_;
    int loop_;
    std::string direction_;
    int delay_;
    std::string callback_;
    std::unordered_map<std::string, std::string> beginState_;
    std::unordered_map<std::string, std::string> endState_;
    std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> keyframes_;
};

// iNeverAway节点
class INeverAwayNode : public BaseNode {
public:
    INeverAwayNode(size_t line = 0, size_t column = 0);
    
    // 函数定义管理
    void addFunction(const std::string& key, const std::string& function, const std::string& state = "");
    const std::unordered_map<std::string, std::string>& getFunctions() const { return functions_; }
    const std::unordered_map<std::string, std::string>& getStates() const { return states_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::unordered_map<std::string, std::string> functions_;  // key -> function
    std::unordered_map<std::string, std::string> states_;     // key -> state
};

// printMylove节点
class PrintMyloveNode : public BaseNode {
public:
    PrintMyloveNode(size_t line = 0, size_t column = 0);
    
    // 参数管理
    void setUrl(const std::string& url);
    void setMode(const std::string& mode);
    void setWidth(const std::string& width);
    void setHeight(const std::string& height);
    void setScale(double scale);
    
    std::string getUrl() const { return url_; }
    std::string getMode() const { return mode_; }
    std::string getWidth() const { return width_; }
    std::string getHeight() const { return height_; }
    double getScale() const { return scale_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::string url_;
    std::string mode_;
    std::string width_;
    std::string height_;
    double scale_;
};

// 模块节点
class ModuleNode : public BaseNode {
public:
    ModuleNode(size_t line = 0, size_t column = 0);
    
    // 模块加载管理
    void addLoadPath(const std::string& path);
    const std::vector<std::string>& getLoadPaths() const { return loadPaths_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::vector<std::string> loadPaths_;
};

// 脚本块节点
class ScriptBlockNode : public BaseNode {
public:
    ScriptBlockNode(size_t line = 0, size_t column = 0);
    
    // JavaScript代码管理
    void setJavaScriptCode(const std::string& code);
    std::string getJavaScriptCode() const { return jsCode_; }
    
    // 选择器管理
    void addSelector(const std::string& selector);
    const std::vector<std::string>& getSelectors() const { return selectors_; }
    
    // 自动添加类/ID
    void setAutoAddClass(bool enabled) { autoAddClass_ = enabled; }
    void setAutoAddId(bool enabled) { autoAddId_ = enabled; }
    bool shouldAutoAddClass() const { return autoAddClass_; }
    bool shouldAutoAddId() const { return autoAddId_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::string jsCode_;
    std::vector<std::string> selectors_;
    bool autoAddClass_;
    bool autoAddId_;
};

// 函数节点
class FunctionNode : public BaseNode {
public:
    FunctionNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    std::string getName() const { return name_; }
    
    // 参数管理
    void addParameter(const std::string& param);
    const std::vector<std::string>& getParameters() const { return parameters_; }
    
    // 函数体管理
    void setBody(const std::string& body);
    std::string getBody() const { return body_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::string name_;
    std::vector<std::string> parameters_;
    std::string body_;
};

// 对象节点
class ObjectNode : public BaseNode {
public:
    ObjectNode(size_t line = 0, size_t column = 0);
    
    // 属性管理
    void addProperty(const std::string& key, const std::string& value);
    const std::unordered_map<std::string, std::string>& getProperties() const { return properties_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::unordered_map<std::string, std::string> properties_;
};

// 数组节点
class ArrayNode : public BaseNode {
public:
    ArrayNode(size_t line = 0, size_t column = 0);
    
    // 元素管理
    void addElement(const std::string& element);
    const std::vector<std::string>& getElements() const { return elements_; }
    
    std::string toJavaScript() const override;
    std::string toString() const override;
    
private:
    std::vector<std::string> elements_;
};

// 节点工厂函数
std::shared_ptr<BaseNode> createNode(NodeType type, const std::string& name = "", 
                                    size_t line = 0, size_t column = 0);

} // namespace CHTLJS