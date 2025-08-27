#pragma once

#include "BaseNode.h"
#include <unordered_map>
#include <vector>

namespace CHTL {

// 样式规则
struct StyleRule {
    std::string selector;      // 选择器（如 .box, #id, &:hover）
    std::unordered_map<std::string, std::string> properties;  // CSS属性
    bool isInlineStyle;       // 是否是内联样式
};

// 样式节点 - 处理局部样式块
class StyleNode : public BaseNode {
public:
    StyleNode(bool isLocal = false);
    
    bool IsLocalStyle() const { return isLocal; }
    void SetLocal(bool local) { isLocal = local; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 添加样式规则
    void AddRule(const StyleRule& rule);
    void AddInlineProperty(const std::string& name, const std::string& value);
    
    // 获取样式信息
    std::vector<StyleRule> GetRules() const { return rules; }
    std::unordered_map<std::string, std::string> GetInlineProperties() const;
    
    // 自动化选择器
    std::string GetFirstClassSelector() const;
    std::string GetFirstIdSelector() const;
    bool HasClassSelector(const std::string& className) const;
    bool HasIdSelector(const std::string& id) const;
    
    // 引用选择器处理
    void ProcessAmpersandSelector(const std::string& parentSelector);
    
    // 分离内联样式和全局样式
    std::pair<std::string, std::string> SeparateStyles() const;
    
private:
    bool isLocal;                    // 是否是局部样式块
    std::vector<StyleRule> rules;    // 样式规则列表
    
    // 解析选择器类型
    bool IsClassSelector(const std::string& selector) const;
    bool IsIdSelector(const std::string& selector) const;
    bool IsAmpersandSelector(const std::string& selector) const;
    
    // 提取选择器名称
    std::string ExtractClassName(const std::string& selector) const;
    std::string ExtractIdName(const std::string& selector) const;
};

} // namespace CHTL