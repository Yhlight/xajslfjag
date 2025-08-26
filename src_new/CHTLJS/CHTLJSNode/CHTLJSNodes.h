#pragma once

#include "../Common/ASTNode.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace AST {

// 前向声明
class Visitor;

// ============================================================================
// CHTL JS特有节点类型
// ============================================================================

// CHTL JS节点类型扩展
enum class CHTLJSNodeType {
    // 增强选择器
    ENHANCED_SELECTOR,   // {{selector}}
    
    // 虚对象
    VIR_OBJECT,          // vir
    
    // 增强监听器
    LISTEN_CALL,         // listen()
    
    // 事件委托
    DELEGATE_CALL,       // delegate()
    
    // 动画
    ANIMATE_CALL,        // animate()
    
    // 特殊函数（珂朵莉模块）
    I_NEVER_AWAY_CALL,   // iNeverAway()
    PRINT_MY_LOVE_CALL,  // printMylove()
    
    // 选择器引用
    SELECTOR_REFERENCE,  // &
    
    // 箭头操作符
    ARROW_OPERATOR,      // ->
    
    // 事件对象
    EVENT_OBJECT,        // { click: function(){}, ... }
    
    // 动画配置对象
    ANIMATION_CONFIG,    // animate的配置对象
    ANIMATION_KEYFRAME,  // 动画关键帧 (when数组中的对象)
    
    // 事件绑定表达式
    EVENT_BIND_EXPR,     // &-> 事件绑定表达式
    
    // vir声明
    VIR_DECLARATION,     // vir虚对象声明
    
    // 键值对（用于事件和动画配置）
    KEY_VALUE_PAIR,
    
    // CHTL JS表达式
    CHTLJS_EXPRESSION,
    
    UNKNOWN_CHTLJS
};

// ============================================================================
// 增强选择器系统
// ============================================================================

// 增强选择器节点 {{selector}}
class EnhancedSelectorNode : public ASTNodeImpl {
public:
    enum SelectorType { 
        GENERAL,         // {{box}}
        CLASS,           // {{.box}}
        ID,              // {{#box}}
        TAG,             // {{button}}
        COMPLEX,         // {{.box button}}
        INDEXED          // {{button[0]}}
    };
    
    SelectorType selector_type;
    std::string selector_value;
    std::string index_value;     // 用于indexed选择器
    bool is_auto_resolved;       // 是否自动解析
    
    explicit EnhancedSelectorNode(const std::string& selector)
        : ASTNodeImpl(NodeType::UNKNOWN), selector_value(selector), is_auto_resolved(false) {
        selector_type = parseSelectorType(selector);
    }
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 解析选择器类型
    SelectorType parseSelectorType(const std::string& selector);
    
    // 获取完整的选择器字符串
    std::string getFullSelector() const;
    
    // 是否需要自动解析为class/id
    bool needsAutoResolution() const { return selector_type == GENERAL; }
    
    void setIndex(const std::string& index) { index_value = index; }
    bool hasIndex() const { return !index_value.empty(); }
};

// ============================================================================
// 虚对象系统
// ============================================================================

// 虚对象节点
class VirObjectNode : public ASTNodeImpl {
public:
    std::string object_name;
    std::string function_assignment;  // 赋值的函数名
    std::unordered_map<std::string, std::string> key_mappings; // 键映射
    
    VirObjectNode(const std::string& name, const std::string& func_name)
        : ASTNodeImpl(NodeType::UNKNOWN), object_name(name), function_assignment(func_name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void addKeyMapping(const std::string& key, const std::string& type) {
        key_mappings[key] = type;
    }
    
    bool hasKey(const std::string& key) const {
        return key_mappings.find(key) != key_mappings.end();
    }
    
    std::string getKeyType(const std::string& key) const {
        auto it = key_mappings.find(key);
        return it != key_mappings.end() ? it->second : "";
    }
};

// ============================================================================
// 增强监听器系统
// ============================================================================

// 监听器调用节点
class ListenCallNode : public ASTNodeImpl {
public:
    std::shared_ptr<EnhancedSelectorNode> target_selector;
    std::unordered_map<std::string, std::string> event_handlers; // event -> handler
    bool is_method_call; // 是否是方法调用形式 (selector->listen())
    bool uses_new_syntax; // 是否使用新的大括号语法 listen { } 而不是 listen({ })
    
    ListenCallNode() : ASTNodeImpl(NodeType::UNKNOWN), is_method_call(false), uses_new_syntax(true) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void setTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector) {
        target_selector = selector;
    }
    
    void addEventHandler(const std::string& event, const std::string& handler) {
        event_handlers[event] = handler;
    }
    
    std::vector<std::string> getSupportedEvents() const;
    bool hasEvent(const std::string& event) const {
        return event_handlers.find(event) != event_handlers.end();
    }
};

// ============================================================================
// 事件委托系统
// ============================================================================

// 事件委托调用节点
class DelegateCallNode : public ASTNodeImpl {
public:
    std::shared_ptr<EnhancedSelectorNode> parent_selector;
    std::vector<std::shared_ptr<EnhancedSelectorNode>> target_selectors;
    std::unordered_map<std::string, std::string> event_handlers;
    bool uses_new_syntax; // 是否使用新的大括号语法 delegate { } 而不是 delegate({ })
    
    DelegateCallNode() : ASTNodeImpl(NodeType::UNKNOWN), uses_new_syntax(true) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void setParentSelector(std::shared_ptr<EnhancedSelectorNode> parent) {
        parent_selector = parent;
    }
    
    void addTargetSelector(std::shared_ptr<EnhancedSelectorNode> target) {
        target_selectors.push_back(target);
    }
    
    void addEventHandler(const std::string& event, const std::string& handler) {
        event_handlers[event] = handler;
    }
    
    bool hasMultipleTargets() const { return target_selectors.size() > 1; }
};

// ============================================================================
// 动画系统
// ============================================================================

// 动画调用节点
class AnimateCallNode : public ASTNodeImpl {
public:
    std::shared_ptr<EnhancedSelectorNode> target_selector;
    std::vector<std::shared_ptr<EnhancedSelectorNode>> target_selectors; // 多目标
    std::shared_ptr<AnimationConfigNode> config;
    bool uses_new_syntax; // 是否使用新的大括号语法 animate { } 而不是 animate({ })
    
    AnimateCallNode() : ASTNodeImpl(NodeType::UNKNOWN), uses_new_syntax(true) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void setTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector) {
        target_selector = selector;
    }
    
    void addTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector) {
        target_selectors.push_back(selector);
    }
    
    void setConfig(std::shared_ptr<AnimationConfigNode> animation_config) {
        config = animation_config;
    }
    
    bool hasMultipleTargets() const { return target_selectors.size() > 1; }
};

// 动画配置节点
class AnimationConfigNode : public ASTNodeImpl {
public:
    int duration;                                           // 持续时间
    std::string easing;                                     // 缓动函数
    std::unordered_map<std::string, std::string> begin;    // 起始状态
    std::unordered_map<std::string, std::string> end;      // 终止状态
    std::vector<AnimationKeyframe> keyframes;              // 关键帧
    int loop;                                               // 循环次数
    std::string direction;                                  // 播放方向
    int delay;                                              // 延迟
    std::string callback;                                   // 回调函数
    
    AnimationConfigNode() : ASTNodeImpl(NodeType::UNKNOWN), duration(0), loop(1), delay(0) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    struct AnimationKeyframe {
        double at;  // 时间点 (0.0-1.0)
        std::unordered_map<std::string, std::string> styles; // CSS样式
        
        AnimationKeyframe(double time) : at(time) {}
        void addStyle(const std::string& property, const std::string& value) {
            styles[property] = value;
        }
    };
    
    void addKeyframe(double at, const std::unordered_map<std::string, std::string>& styles) {
        AnimationKeyframe frame(at);
        frame.styles = styles;
        keyframes.push_back(frame);
    }
    
    void setBeginStyle(const std::string& property, const std::string& value) {
        begin[property] = value;
    }
    
    void setEndStyle(const std::string& property, const std::string& value) {
        end[property] = value;
    }
    
    bool isValidEasing() const;
};

// ============================================================================
// 特殊函数系统（珂朵莉模块）
// ============================================================================

// iNeverAway调用节点
class INeverAwayCallNode : public ASTNodeImpl {
public:
    std::unordered_map<std::string, std::string> functions; // 函数映射
    std::unordered_map<std::string, std::string> objects;   // 对象映射
    
    INeverAwayCallNode() : ASTNodeImpl(NodeType::UNKNOWN) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void addFunction(const std::string& key, const std::string& func) {
        functions[key] = func;
    }
    
    void addObject(const std::string& key, const std::string& obj) {
        objects[key] = obj;
    }
    
    bool hasKey(const std::string& key) const {
        return functions.find(key) != functions.end() || objects.find(key) != objects.end();
    }
    
    // 解析状态模板（如 Void<A>）
    std::string parseStateTemplate(const std::string& key) const;
};

// printMylove调用节点
class PrintMyLoveCallNode : public ASTNodeImpl {
public:
    std::string url;
    std::string mode;      // ASCII 或 Pixel
    std::string width;
    std::string height;
    std::string scale;
    
    PrintMyLoveCallNode() : ASTNodeImpl(NodeType::UNKNOWN) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void setUrl(const std::string& image_url) { url = image_url; }
    void setMode(const std::string& output_mode) { mode = output_mode; }
    void setWidth(const std::string& w) { width = w; }
    void setHeight(const std::string& h) { height = h; }
    void setScale(const std::string& s) { scale = s; }
    
    bool isValidMode() const { return mode == "ASCII" || mode == "Pixel"; }
    bool hasRequiredParams() const { return !url.empty() && !mode.empty(); }
};

// ============================================================================
// 选择器引用和箭头操作符
// ============================================================================

// 选择器引用节点 (&)
class SelectorReferenceNode : public ASTNodeImpl {
public:
    std::string reference_type; // class 或 id
    std::string resolved_value; // 解析后的值
    bool is_resolved;
    
    SelectorReferenceNode() : ASTNodeImpl(NodeType::UNKNOWN), is_resolved(false) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    void resolve(const std::string& value, const std::string& type) {
        resolved_value = value;
        reference_type = type;
        is_resolved = true;
    }
    
    bool needsResolution() const { return !is_resolved; }
};

// 箭头操作符节点 (->)
class ArrowOperatorNode : public ASTNodeImpl {
public:
    std::shared_ptr<ASTNode> left_operand;   // 左操作数
    std::shared_ptr<ASTNode> right_operand;  // 右操作数
    
    ArrowOperatorNode() : ASTNodeImpl(NodeType::UNKNOWN) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void setLeftOperand(std::shared_ptr<ASTNode> left) { left_operand = left; }
    void setRightOperand(std::shared_ptr<ASTNode> right) { right_operand = right; }
    
    bool isMethodCall() const; // 判断是否是方法调用
    bool isCHTLJSSpecific() const { return true; }
};

// ============================================================================
// 事件和配置对象
// ============================================================================

// 事件对象节点
class EventObjectNode : public ASTNodeImpl {
public:
    std::unordered_map<std::string, std::string> event_handlers;
    bool allows_unordered;  // 是否允许无序
    bool allows_optional;   // 是否允许可选
    
    EventObjectNode() : ASTNodeImpl(NodeType::UNKNOWN), allows_unordered(true), allows_optional(true) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void addEventHandler(const std::string& event, const std::string& handler) {
        event_handlers[event] = handler;
    }
    
    bool hasEvent(const std::string& event) const {
        return event_handlers.find(event) != event_handlers.end();
    }
    
    std::vector<std::string> getAllEvents() const;
    bool isValidEvent(const std::string& event) const;
};

// 键值对节点
class KeyValuePairNode : public ASTNodeImpl {
public:
    std::string key;
    std::string value;
    std::string value_type; // function, object, string等
    bool is_optional;
    
    KeyValuePairNode(const std::string& k, const std::string& v, const std::string& type = "")
        : ASTNodeImpl(NodeType::UNKNOWN), key(k), value(v), value_type(type), is_optional(false) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    void setOptional(bool optional) { is_optional = optional; }
    bool isFunction() const { return value_type == "function"; }
    bool isObject() const { return value_type == "object"; }
};

// ============================================================================
// CHTL JS表达式
// ============================================================================

// CHTL JS表达式节点
class CHTLJSExpressionNode : public ASTNodeImpl {
public:
    enum ExpressionType { 
        SELECTOR_CHAIN,      // {{selector}}->method()
        VIR_ASSIGNMENT,      // vir name = function()
        EVENT_BINDING,       // listen({...})
        ANIMATION_CALL,      // animate({...})
        VARIABLE_REFERENCE,  // 变量引用
        FUNCTION_CALL        // 函数调用
    };
    
    ExpressionType expression_type;
    std::string expression_content;
    
    CHTLJSExpressionNode(ExpressionType type, const std::string& content)
        : ASTNodeImpl(NodeType::UNKNOWN), expression_type(type), expression_content(content) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    bool isCHTLJSSpecific() const { return true; }
    std::string getExpressionTypeString() const;
};

// ============================================================================
// 工具类和辅助函数
// ============================================================================

// CHTL JS AST工具类
class CHTLJSASTUtils {
public:
    // 创建增强选择器
    static std::shared_ptr<EnhancedSelectorNode> createSelector(const std::string& selector);
    
    // 创建虚对象
    static std::shared_ptr<VirObjectNode> createVirObject(const std::string& name, const std::string& func);
    
    // 创建监听器调用
    static std::shared_ptr<ListenCallNode> createListenCall();
    
    // 创建事件委托调用
    static std::shared_ptr<DelegateCallNode> createDelegateCall();
    
    // 创建动画调用
    static std::shared_ptr<AnimateCallNode> createAnimateCall();
    
    // 验证事件名称
    static bool isValidEventName(const std::string& event);
    
    // 验证缓动函数
    static bool isValidEasingFunction(const std::string& easing);
    
    // 解析选择器
    static EnhancedSelectorNode::SelectorType parseSelector(const std::string& selector);
    
    // 生成JavaScript代码
    static std::string generateJavaScript(std::shared_ptr<ASTNode> node);
    
    // 优化CHTL JS AST
    static void optimizeAST(std::shared_ptr<ASTNode> root);
};

// ============================================================================
// 扩展的CHTL JS AST节点类
// ============================================================================

// 事件绑定表达式节点 (&->)
class EventBindExpressionNode : public ASTNodeImpl {
public:
    std::shared_ptr<EnhancedSelectorNode> target_selector; // 目标选择器
    std::string event_type;                                // 事件类型 (click, hover等)
    std::string handler_code;                              // 处理器代码
    bool is_inline_handler;                                // 是否内联处理器
    
    EventBindExpressionNode() : ASTNodeImpl(NodeType::UNKNOWN), is_inline_handler(true) {}
    
    EventBindExpressionNode(const std::string& event, const std::string& handler)
        : ASTNodeImpl(NodeType::UNKNOWN), event_type(event), handler_code(handler), is_inline_handler(true) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 生成标准事件监听器代码
    std::string generateEventListenerCode() const;
};

// vir虚对象声明节点
class VirDeclarationNode : public ASTNodeImpl {
public:
    std::string vir_name;                                  // vir对象名称
    std::shared_ptr<ASTNode> target_function;             // 目标函数 (listen, delegate等)
    std::unordered_map<std::string, std::string> key_map; // 键值映射表
    std::unordered_map<std::string, std::string> key_types; // 键的类型映射 (function, object, array等)
    
    VirDeclarationNode() : ASTNodeImpl(NodeType::UNKNOWN) {}
    
    VirDeclarationNode(const std::string& name) 
        : ASTNodeImpl(NodeType::UNKNOWN), vir_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 解析目标函数中的键 - 创建C++编译期View对象
    void extractKeysFromFunction();
    
    // 生成vir访问代码 - 根据键值类型转换成不同内容
    std::string generateVirAccessCode(const std::string& key) const;
    
    // 检查键是否存在
    bool hasKey(const std::string& key) const;
    
    // 获取键的类型 (function, object, array等)
    std::string getKeyType(const std::string& key) const;
    
    // 缓存访问结果 - View对象的缓存机制
    void cacheAccessResult(const std::string& key, const std::string& result);
    std::string getCachedResult(const std::string& key) const;
    
    // 检测键值的实际类型
    enum class KeyValueType { FUNCTION_REF, OBJECT_REF, ARRAY_REF, PRIMITIVE_VALUE, UNKNOWN };
    KeyValueType detectKeyValueType(const std::string& key) const;

private:
    // C++编译期View对象的缓存
    mutable std::unordered_map<std::string, std::string> access_cache;
};

// 动画关键帧节点
class AnimationKeyframeNode : public ASTNodeImpl {
public:
    double at_time;                                        // 时间点 (0.0 - 1.0)
    std::unordered_map<std::string, std::string> styles;  // CSS样式属性
    
    AnimationKeyframeNode() : ASTNodeImpl(NodeType::UNKNOWN), at_time(0.0) {}
    
    AnimationKeyframeNode(double time) : ASTNodeImpl(NodeType::UNKNOWN), at_time(time) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 添加样式属性
    void addStyle(const std::string& property, const std::string& value) {
        styles[property] = value;
    }
    
    // 生成CSS关键帧代码
    std::string generateKeyframeCSS() const;
    
    // 验证时间点有效性
    bool isValidTimePoint() const { return at_time >= 0.0 && at_time <= 1.0; }
};

} // namespace AST
} // namespace CHTL