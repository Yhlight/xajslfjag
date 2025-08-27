#pragma once
#include "BaseNode.h"
#include <unordered_set>

namespace CHTL {

/**
 * @brief 样式规则结构
 */
struct StyleRule {
    std::string selector;                                    // 选择器
    std::unordered_map<std::string, std::string> properties; // 属性映射
    bool is_inline;                                          // 是否为内联样式
    
    StyleRule() : is_inline(false) {}
    StyleRule(const std::string& sel, bool inline_style = false) 
        : selector(sel), is_inline(inline_style) {}
};

/**
 * @brief 样式节点类
 * 表示CHTL中的局部样式块
 */
class StyleNode : public BaseNode {
private:
    std::vector<StyleRule> rules_;              // 样式规则列表
    std::unordered_set<std::string> auto_classes_; // 自动生成的类名
    std::unordered_set<std::string> auto_ids_;      // 自动生成的ID
    bool is_global_;                            // 是否为全局样式
    std::string parent_selector_;               // 父级选择器（用于&引用）

public:
    /**
     * @brief 构造函数
     * @param is_global 是否为全局样式
     */
    StyleNode(bool is_global = false);

    /**
     * @brief 析构函数
     */
    virtual ~StyleNode() = default;

    // Getter方法
    const std::vector<StyleRule>& getRules() const { return rules_; }
    const std::unordered_set<std::string>& getAutoClasses() const { return auto_classes_; }
    const std::unordered_set<std::string>& getAutoIds() const { return auto_ids_; }
    bool isGlobal() const { return is_global_; }
    const std::string& getParentSelector() const { return parent_selector_; }

    // Setter方法
    void setGlobal(bool global) { is_global_ = global; }
    void setParentSelector(const std::string& selector) { parent_selector_ = selector; }

    /**
     * @brief 添加样式规则
     * @param rule 样式规则
     */
    void addRule(const StyleRule& rule);

    /**
     * @brief 添加内联样式属性
     * @param property 属性名
     * @param value 属性值
     */
    void addInlineProperty(const std::string& property, const std::string& value);

    /**
     * @brief 添加选择器规则
     * @param selector 选择器
     * @param properties 属性映射
     */
    void addSelectorRule(const std::string& selector, 
                        const std::unordered_map<std::string, std::string>& properties);

    /**
     * @brief 处理自动化类名
     * @param class_name 类名（不带.前缀）
     */
    void addAutoClass(const std::string& class_name);

    /**
     * @brief 处理自动化ID
     * @param id_name ID名（不带#前缀）
     */
    void addAutoId(const std::string& id_name);

    /**
     * @brief 处理&引用选择器
     * @param selector 包含&的选择器
     * @return 处理后的选择器
     */
    std::string resolveAmpersandSelector(const std::string& selector) const;

    /**
     * @brief 获取内联样式规则
     * @return 内联样式规则，如果没有返回nullptr
     */
    const StyleRule* getInlineRule() const;

    /**
     * @brief 获取非内联样式规则
     * @return 非内联样式规则列表
     */
    std::vector<StyleRule> getNonInlineRules() const;

    /**
     * @brief 生成内联样式字符串
     * @return 内联样式字符串
     */
    std::string generateInlineStyle() const;

    /**
     * @brief 生成CSS规则字符串
     * @param indent 缩进级别
     * @return CSS字符串
     */
    std::string generateCSS(int indent = 0) const;

    /**
     * @brief 合并另一个样式节点
     * @param other 另一个样式节点
     */
    void merge(const StyleNode& other);

    /**
     * @brief 克隆节点
     * @param deep 是否深度克隆
     * @return 克隆的节点
     */
    virtual NodePtr clone(bool deep = true) const override;

    /**
     * @brief 接受访问者
     * @param visitor 访问者对象
     */
    virtual void accept(NodeVisitor& visitor) override;

    /**
     * @brief 转换为字符串表示
     * @param indent 缩进级别
     * @return 字符串表示
     */
    virtual std::string toString(int indent = 0) const override;

    /**
     * @brief 验证样式节点的有效性
     * @return 验证结果消息
     */
    virtual std::string validate() const override;

private:
    /**
     * @brief 规范化选择器
     * @param selector 原始选择器
     * @return 规范化后的选择器
     */
    std::string normalizeSelector(const std::string& selector) const;

    /**
     * @brief 验证CSS属性
     * @param property 属性名
     * @param value 属性值
     * @return 是否有效
     */
    bool validateCSSProperty(const std::string& property, const std::string& value) const;

    /**
     * @brief 转义CSS值
     * @param value 原始值
     * @return 转义后的值
     */
    std::string escapeCSSValue(const std::string& value) const;
};

} // namespace CHTL