#pragma once
#include "../BaseNode.h"
#include <unordered_set>

namespace CHTL {

/**
 * HTML元素节点
 * 表示CHTL中的HTML元素，如div, span, p等
 */
class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName, const Position& pos = Position{});
    ~ElementNode() override = default;
    
    // 标签名管理
    const std::string& getTagName() const { return tagName_; }
    void setTagName(const std::string& tagName) { tagName_ = tagName; }
    
    // 类名管理
    void addClass(const std::string& className);
    void removeClass(const std::string& className);
    bool hasClass(const std::string& className) const;
    std::unordered_set<std::string> getClasses() const;
    void clearClasses();
    
    // ID管理
    void setId(const std::string& id);
    std::string getId() const;
    bool hasId() const;
    
    // 自动化类名/ID管理
    void enableAutoClass(bool enable = true) { autoClass_ = enable; }
    void enableAutoId(bool enable = true) { autoId_ = enable; }
    bool isAutoClassEnabled() const { return autoClass_; }
    bool isAutoIdEnabled() const { return autoId_; }
    
    // 样式块管理
    void addStyleBlock(std::shared_ptr<BaseNode> styleNode);
    std::vector<std::shared_ptr<BaseNode>> getStyleBlocks() const;
    bool hasStyleBlocks() const;
    
    // 脚本块管理
    void addScriptBlock(std::shared_ptr<BaseNode> scriptNode);
    std::vector<std::shared_ptr<BaseNode>> getScriptBlocks() const;
    bool hasScriptBlocks() const;
    
    // 文本内容管理
    void addTextNode(std::shared_ptr<BaseNode> textNode);
    std::vector<std::shared_ptr<BaseNode>> getTextNodes() const;
    std::string getAllText() const;
    
    // 内联样式管理
    void setInlineStyle(const std::string& property, const std::string& value);
    std::string getInlineStyle(const std::string& property) const;
    std::unordered_map<std::string, std::string> getAllInlineStyles() const;
    void clearInlineStyles();
    
    // HTML特性检查
    bool isSelfClosing() const;
    bool isBlockElement() const;
    bool isInlineElement() const;
    bool isVoidElement() const;
    
    // 验证
    bool validate() const override;
    std::vector<std::string> getValidationErrors() const override;
    
    // BaseNode接口实现
    std::string toString() const override;
    std::shared_ptr<BaseNode> clone() const override;
    void accept(CHTLGenerator* generator) override;
    
protected:
    void onChildAdded(std::shared_ptr<BaseNode> child) override;
    void onChildRemoved(std::shared_ptr<BaseNode> child) override;
    void onAttributeChanged(const std::string& name, const std::string& value) override;
    
private:
    std::string tagName_;
    std::unordered_set<std::string> classes_;
    std::unordered_map<std::string, std::string> inlineStyles_;
    bool autoClass_ = false;
    bool autoId_ = false;
    
    // 缓存的子节点引用
    std::vector<std::shared_ptr<BaseNode>> styleBlocks_;
    std::vector<std::shared_ptr<BaseNode>> scriptBlocks_;
    std::vector<std::shared_ptr<BaseNode>> textNodes_;
    
    // 辅助方法
    void updateClassAttribute();
    void updateStyleAttribute();
    void rebuildChildCaches();
    
    // HTML元素信息
    static const std::unordered_set<std::string> SELF_CLOSING_TAGS;
    static const std::unordered_set<std::string> BLOCK_ELEMENTS;
    static const std::unordered_set<std::string> INLINE_ELEMENTS;
    static const std::unordered_set<std::string> VOID_ELEMENTS;
};

} // namespace CHTL