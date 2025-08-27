#pragma once

#include "BaseNode.h"
#include <unordered_set>

namespace CHTL {

/// 脚本类型枚举
enum class ScriptType {
    JAVASCRIPT,     // 普通JavaScript
    CHTL_JS        // CHTL JS语法
};

/// CHTL JS增强选择器节点 {{selector}}
class EnhancedSelectorNode : public BaseNode {
public:
    /// 选择器类型
    enum class SelectorType {
        TAG,            // {{button}} - 标签选择器
        CLASS,          // {{.box}} - 类选择器
        ID,             // {{#box}} - ID选择器
        COMPOSITE,      // {{.box button}} - 复合选择器
        INDEXED         // {{button[0]}} - 索引访问
    };

private:
    SelectorType selectorType;
    std::string selectorValue;  // 选择器值
    int index;                  // 索引值（用于INDEXED类型）
    bool isAutoAdded;          // 是否为自动添加的选择器

public:
    EnhancedSelectorNode(SelectorType type, const std::string& value, const NodePosition& pos = NodePosition());
    
    /// 获取选择器类型
    SelectorType GetSelectorType() const { return selectorType; }
    
    /// 获取选择器值
    const std::string& GetSelectorValue() const { return selectorValue; }
    
    /// 设置索引值
    void SetIndex(int idx) { index = idx; }
    
    /// 获取索引值
    int GetIndex() const { return index; }
    
    /// 设置是否为自动添加
    void SetAutoAdded(bool autoAdded) { isAutoAdded = autoAdded; }
    
    /// 检查是否为自动添加
    bool IsAutoAdded() const { return isAutoAdded; }
    
    /// 转换为JavaScript选择器代码
    std::string ToJavaScriptSelector() const;
    
    /// 克隆节点
    std::shared_ptr<BaseNode> Clone() const override;
    
    /// 验证节点
    bool Validate() const override;
    
    /// 获取节点描述
    std::string GetDescription() const override;
    
    /// 接受访问者
    void Accept(NodeVisitor& visitor) override;
};

/// CHTL JS listen函数节点
class ListenNode : public BaseNode {
private:
    std::shared_ptr<EnhancedSelectorNode> targetSelector;  // 目标选择器
    std::unordered_map<std::string, std::string> eventHandlers;  // 事件处理器映射

public:
    ListenNode(const NodePosition& pos = NodePosition());
    
    /// 设置目标选择器
    void SetTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector);
    
    /// 获取目标选择器
    std::shared_ptr<EnhancedSelectorNode> GetTargetSelector() const { return targetSelector; }
    
    /// 添加事件处理器
    void AddEventHandler(const std::string& eventType, const std::string& handler);
    
    /// 获取事件处理器映射
    const std::unordered_map<std::string, std::string>& GetEventHandlers() const { return eventHandlers; }
    
    /// 转换为JavaScript代码
    std::string ToJavaScript() const;
    
    /// 克隆节点
    std::shared_ptr<BaseNode> Clone() const override;
    
    /// 验证节点
    bool Validate() const override;
    
    /// 获取节点描述
    std::string GetDescription() const override;
    
    /// 接受访问者
    void Accept(NodeVisitor& visitor) override;
};

/// CHTL JS delegate函数节点
class DelegateNode : public BaseNode {
private:
    std::shared_ptr<EnhancedSelectorNode> parentSelector;    // 父元素选择器
    std::vector<std::shared_ptr<EnhancedSelectorNode>> targetSelectors;  // 目标选择器列表
    std::unordered_map<std::string, std::string> eventHandlers;  // 事件处理器映射

public:
    DelegateNode(const NodePosition& pos = NodePosition());
    
    /// 设置父元素选择器
    void SetParentSelector(std::shared_ptr<EnhancedSelectorNode> selector);
    
    /// 获取父元素选择器
    std::shared_ptr<EnhancedSelectorNode> GetParentSelector() const { return parentSelector; }
    
    /// 添加目标选择器
    void AddTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector);
    
    /// 获取目标选择器列表
    const std::vector<std::shared_ptr<EnhancedSelectorNode>>& GetTargetSelectors() const { return targetSelectors; }
    
    /// 添加事件处理器
    void AddEventHandler(const std::string& eventType, const std::string& handler);
    
    /// 获取事件处理器映射
    const std::unordered_map<std::string, std::string>& GetEventHandlers() const { return eventHandlers; }
    
    /// 转换为JavaScript代码
    std::string ToJavaScript() const;
    
    /// 克隆节点
    std::shared_ptr<BaseNode> Clone() const override;
    
    /// 验证节点
    bool Validate() const override;
    
    /// 获取节点描述
    std::string GetDescription() const override;
    
    /// 接受访问者
    void Accept(NodeVisitor& visitor) override;
};

/// CHTL JS animate函数节点
class AnimateNode : public BaseNode {
private:
    std::vector<std::shared_ptr<EnhancedSelectorNode>> targetSelectors;  // 目标选择器
    std::unordered_map<std::string, std::string> animationProperties;    // 动画属性
    std::string beginState;     // 起始状态
    std::string endState;       // 结束状态
    std::vector<std::pair<double, std::string>> keyframes;  // 关键帧

public:
    AnimateNode(const NodePosition& pos = NodePosition());
    
    /// 添加目标选择器
    void AddTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector);
    
    /// 获取目标选择器列表
    const std::vector<std::shared_ptr<EnhancedSelectorNode>>& GetTargetSelectors() const { return targetSelectors; }
    
    /// 设置动画属性
    void SetAnimationProperty(const std::string& key, const std::string& value);
    
    /// 获取动画属性
    const std::unordered_map<std::string, std::string>& GetAnimationProperties() const { return animationProperties; }
    
    /// 设置起始状态
    void SetBeginState(const std::string& state) { beginState = state; }
    
    /// 获取起始状态
    const std::string& GetBeginState() const { return beginState; }
    
    /// 设置结束状态
    void SetEndState(const std::string& state) { endState = state; }
    
    /// 获取结束状态
    const std::string& GetEndState() const { return endState; }
    
    /// 添加关键帧
    void AddKeyframe(double at, const std::string& state);
    
    /// 获取关键帧列表
    const std::vector<std::pair<double, std::string>>& GetKeyframes() const { return keyframes; }
    
    /// 转换为JavaScript代码
    std::string ToJavaScript() const;
    
    /// 克隆节点
    std::shared_ptr<BaseNode> Clone() const override;
    
    /// 验证节点
    bool Validate() const override;
    
    /// 获取节点描述
    std::string GetDescription() const override;
    
    /// 接受访问者
    void Accept(NodeVisitor& visitor) override;
};

/// CHTL JS虚对象节点 vir
class VirtualObjectNode : public BaseNode {
private:
    std::string objectName;     // 虚对象名称
    std::unordered_map<std::string, std::string> properties;  // 属性映射

public:
    VirtualObjectNode(const std::string& name, const NodePosition& pos = NodePosition());
    
    /// 获取对象名称
    const std::string& GetObjectName() const { return objectName; }
    
    /// 设置属性
    void SetProperty(const std::string& key, const std::string& value);
    
    /// 获取属性
    std::string GetProperty(const std::string& key) const;
    
    /// 获取所有属性
    const std::unordered_map<std::string, std::string>& GetProperties() const { return properties; }
    
    /// 转换为JavaScript代码
    std::string ToJavaScript() const;
    
    /// 克隆节点
    std::shared_ptr<BaseNode> Clone() const override;
    
    /// 验证节点
    bool Validate() const override;
    
    /// 获取节点描述
    std::string GetDescription() const override;
    
    /// 接受访问者
    void Accept(NodeVisitor& visitor) override;
};

/// 脚本块节点 script { }
class ScriptBlockNode : public BaseNode {
private:
    ScriptType scriptType;      // 脚本类型
    std::string rawContent;     // 原始JavaScript内容
    std::unordered_set<std::string> autoAddedClasses;  // 自动添加的类选择器
    std::unordered_set<std::string> autoAddedIds;      // 自动添加的ID选择器

public:
    ScriptBlockNode(ScriptType type = ScriptType::CHTL_JS, const NodePosition& pos = NodePosition());
    
    /// 获取脚本类型
    ScriptType GetScriptType() const { return scriptType; }
    
    /// 设置脚本类型
    void SetScriptType(ScriptType type) { scriptType = type; }
    
    /// 设置原始内容
    void SetRawContent(const std::string& content) { rawContent = content; }
    
    /// 获取原始内容
    const std::string& GetRawContent() const { return rawContent; }
    
    /// 添加listen节点
    void AddListenNode(std::shared_ptr<ListenNode> listenNode);
    
    /// 添加delegate节点
    void AddDelegateNode(std::shared_ptr<DelegateNode> delegateNode);
    
    /// 添加animate节点
    void AddAnimateNode(std::shared_ptr<AnimateNode> animateNode);
    
    /// 添加虚对象节点
    void AddVirtualObjectNode(std::shared_ptr<VirtualObjectNode> virNode);
    
    /// 获取listen节点列表
    std::vector<std::shared_ptr<ListenNode>> GetListenNodes() const;
    
    /// 获取delegate节点列表
    std::vector<std::shared_ptr<DelegateNode>> GetDelegateNodes() const;
    
    /// 获取animate节点列表
    std::vector<std::shared_ptr<AnimateNode>> GetAnimateNodes() const;
    
    /// 获取虚对象节点列表
    std::vector<std::shared_ptr<VirtualObjectNode>> GetVirtualObjectNodes() const;
    
    /// 记录自动添加的类选择器
    void AddAutoAddedClass(const std::string& className);
    
    /// 记录自动添加的ID选择器
    void AddAutoAddedId(const std::string& id);
    
    /// 获取自动添加的类选择器集合
    const std::unordered_set<std::string>& GetAutoAddedClasses() const { return autoAddedClasses; }
    
    /// 获取自动添加的ID选择器集合
    const std::unordered_set<std::string>& GetAutoAddedIds() const { return autoAddedIds; }
    
    /// 处理自动化选择器
    void ProcessAutomaticSelectors();
    
    /// 生成最终的JavaScript代码
    std::string GenerateJavaScript() const;
    
    /// 克隆节点
    std::shared_ptr<BaseNode> Clone() const override;
    
    /// 验证节点
    bool Validate() const override;
    
    /// 获取节点描述
    std::string GetDescription() const override;
    
    /// 接受访问者
    void Accept(NodeVisitor& visitor) override;
};

/// 脚本类型转换为字符串
std::string ScriptTypeToString(ScriptType type);

/// 增强选择器类型转换为字符串
std::string EnhancedSelectorTypeToString(EnhancedSelectorNode::SelectorType type);

} // namespace CHTL