#pragma once
#include "BaseNode.h"
#include <unordered_map>

namespace CHTL {

/**
 * 样式节点类型
 */
enum class StyleNodeType {
    LOCAL_STYLE,        // 局部样式块
    INLINE_STYLE,       // 内联样式
    SELECTOR_STYLE      // 选择器样式
};

/**
 * 样式节点
 * 表示CHTL中的style块
 */
class StyleNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    StyleNode(StyleNodeType styleType = StyleNodeType::LOCAL_STYLE);
    
    /**
     * 析构函数
     */
    virtual ~StyleNode() = default;
    
    // ========== 样式属性 ==========
    
    /**
     * 获取样式类型
     */
    StyleNodeType getStyleType() const;
    
    /**
     * 设置样式类型
     */
    void setStyleType(StyleNodeType type);
    
    /**
     * 添加CSS属性
     */
    void addCssProperty(const std::string& property, const std::string& value);
    
    /**
     * 获取CSS属性
     */
    std::string getCssProperty(const std::string& property) const;
    
    /**
     * 检查是否有CSS属性
     */
    bool hasCssProperty(const std::string& property) const;
    
    /**
     * 移除CSS属性
     */
    bool removeCssProperty(const std::string& property);
    
    /**
     * 获取所有CSS属性
     */
    const std::unordered_map<std::string, std::string>& getCssProperties() const;
    
    /**
     * 清空所有CSS属性
     */
    void clearCssProperties();
    
    // ========== 选择器管理 ==========
    
    /**
     * 设置选择器
     */
    void setSelector(const std::string& selector);
    
    /**
     * 获取选择器
     */
    const std::string& getSelector() const;
    
    /**
     * 检查是否有选择器
     */
    bool hasSelector() const;
    
    /**
     * 检查是否为类选择器
     */
    bool isClassSelector() const;
    
    /**
     * 检查是否为ID选择器
     */
    bool isIdSelector() const;
    
    /**
     * 检查是否为伪类选择器
     */
    bool isPseudoClassSelector() const;
    
    /**
     * 检查是否为伪元素选择器
     */
    bool isPseudoElementSelector() const;
    
    // ========== 自动化功能 ==========
    
    /**
     * 获取自动添加的类名
     */
    std::string getAutoClassName() const;
    
    /**
     * 获取自动添加的ID
     */
    std::string getAutoId() const;
    
    /**
     * 检查是否需要自动添加类名
     */
    bool needsAutoClass() const;
    
    /**
     * 检查是否需要自动添加ID
     */
    bool needsAutoId() const;
    
    // ========== 重写基类方法 ==========
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 转换为字符串
     */
    std::string toString() const override;
    
    /**
     * 转换为CSS
     */
    std::string toCss() const;
    
    /**
     * 验证节点
     */
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

protected:
    /**
     * 内部验证方法
     */
    bool internalValidate(ErrorReporter* errorReporter) const override;

private:
    StyleNodeType m_styleType;                                  // 样式类型
    std::string m_selector;                                     // 选择器
    std::unordered_map<std::string, std::string> m_cssProperties; // CSS属性
    
    // 辅助方法
    std::string extractSelectorName(const std::string& selector) const;
};

} // namespace CHTL