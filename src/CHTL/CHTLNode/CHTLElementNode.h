#pragma once

#include "CHTLBaseNode.h"

namespace CHTL {

// HTML元素节点
class CHTLElementNode : public CHTLBaseNode {
public:
    explicit CHTLElementNode(const std::string& tagName = "");
    virtual ~CHTLElementNode() = default;
    
    // 标签名管理
    void setTagName(const std::string& tagName) { this->tagName = tagName; }
    const std::string& getTagName() const { return tagName; }
    
    // 检查是否为自闭合标签
    bool isSelfClosing() const;
    void setSelfClosing(bool selfClosing) { this->selfClosing = selfClosing; }
    
    // ID和Class管理
    void setId(const std::string& id);
    std::string getId() const;
    
    void addClass(const std::string& className);
    void removeClass(const std::string& className);
    bool hasClass(const std::string& className) const;
    std::vector<std::string> getClasses() const;
    void setClasses(const std::vector<std::string>& classes);
    
    // 样式管理
    void setInlineStyle(const std::string& property, const std::string& value);
    void removeInlineStyle(const std::string& property);
    std::string getInlineStyle(const std::string& property) const;
    std::string getInlineStyleString() const;
    
    // 子节点的特殊管理
    std::shared_ptr<class CHTLStyleNode> getStyleNode() const;
    std::shared_ptr<class CHTLScriptNode> getScriptNode() const;
    std::vector<std::shared_ptr<CHTLTextNode>> getTextNodes() const;
    
    // 添加特殊子节点
    void addStyleNode(std::shared_ptr<class CHTLStyleNode> styleNode);
    void addScriptNode(std::shared_ptr<class CHTLScriptNode> scriptNode);
    void addTextNode(std::shared_ptr<CHTLTextNode> textNode);
    
    // 检查节点有效性
    bool isValidHtmlElement() const;
    
    // 访问者模式
    void accept(CHTLNodeVisitor* visitor) override;
    
    // 深度复制
    std::shared_ptr<CHTLBaseNode> clone() const override;
    
    // 调试输出
    std::string toString() const override;
    
    // HTML输出
    std::string toHTML(bool includeChildren = true, int indent = 0) const;

private:
    std::string tagName;
    bool selfClosing = false;
    std::unordered_map<std::string, std::string> inlineStyles;
    
    // 解析class属性
    void updateClassAttribute();
    void updateStyleAttribute();
    std::vector<std::string> parseClassAttribute() const;
};

} // namespace CHTL