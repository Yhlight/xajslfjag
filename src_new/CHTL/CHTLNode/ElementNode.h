#pragma once
#include "BaseNode.h"
#include <unordered_set>

namespace CHTL {

/**
 * HTML元素节点
 * 表示CHTL中的HTML元素
 */
class ElementNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    ElementNode(const std::string& tagName);
    
    /**
     * 析构函数
     */
    virtual ~ElementNode() = default;
    
    // ========== 元素特定属性 ==========
    
    /**
     * 获取标签名
     */
    const std::string& getTagName() const;
    
    /**
     * 设置标签名
     */
    void setTagName(const std::string& tagName);
    
    /**
     * 检查是否为自闭合标签
     */
    bool isSelfClosing() const;
    
    /**
     * 检查是否为块级元素
     */
    bool isBlockElement() const;
    
    /**
     * 检查是否为行内元素
     */
    bool isInlineElement() const;
    
    // ========== HTML属性管理 ==========
    
    /**
     * 设置ID属性
     */
    void setId(const std::string& id);
    
    /**
     * 获取ID属性
     */
    std::string getId() const;
    
    /**
     * 设置class属性
     */
    void setClass(const std::string& className);
    
    /**
     * 获取class属性
     */
    std::string getClass() const;
    
    /**
     * 添加CSS类
     */
    void addClass(const std::string& className);
    
    /**
     * 移除CSS类
     */
    bool removeClass(const std::string& className);
    
    /**
     * 检查是否有指定CSS类
     */
    bool hasClass(const std::string& className) const;
    
    /**
     * 获取所有CSS类
     */
    std::vector<std::string> getClasses() const;
    
    // ========== 样式和脚本节点 ==========
    
    /**
     * 获取样式节点
     */
    BaseNode* getStyleNode() const;
    
    /**
     * 添加样式节点
     */
    void setStyleNode(NodePtr styleNode);
    
    /**
     * 获取脚本节点
     */
    BaseNode* getScriptNode() const;
    
    /**
     * 添加脚本节点
     */
    void setScriptNode(NodePtr scriptNode);
    
    /**
     * 检查是否有内联样式
     */
    bool hasInlineStyle() const;
    
    /**
     * 检查是否有局部脚本
     */
    bool hasLocalScript() const;
    
    // ========== 特殊属性 ==========
    
    /**
     * 设置是否导出
     */
    void setExported(bool exported);
    
    /**
     * 检查是否导出
     */
    bool isExported() const;
    
    /**
     * 设置命名空间
     */
    void setNamespace(const std::string& namespace_);
    
    /**
     * 获取命名空间
     */
    const std::string& getNamespace() const;
    
    // ========== 重写基类方法 ==========
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 深度克隆节点
     */
    std::shared_ptr<BaseNode> deepClone() const override;
    
    /**
     * 转换为字符串
     */
    std::string toString() const override;
    
    /**
     * 转换为HTML
     */
    std::string toHtml() const;
    
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
    std::string m_tagName;          // 标签名
    std::string m_namespace;        // 命名空间
    bool m_exported;                // 是否导出
    
    // 辅助方法
    std::vector<std::string> splitClasses(const std::string& classes) const;
    std::string joinClasses(const std::vector<std::string>& classes) const;
    
    // 静态HTML元素信息
    static const std::unordered_set<std::string>& getSelfClosingTags();
    static const std::unordered_set<std::string>& getBlockElements();
    static const std::unordered_set<std::string>& getInlineElements();
};

} // namespace CHTL