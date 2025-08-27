#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include <unordered_map>
#include <vector>

namespace CHTL {

// CSS规则类型
enum class CSSRuleType {
    INLINE,         // 内联样式
    CLASS,          // 类选择器
    ID,             // ID选择器
    PSEUDO_CLASS,   // 伪类选择器
    PSEUDO_ELEMENT, // 伪元素选择器
    CONTEXT         // 上下文选择器(&)
};

// CSS规则结构
struct CSSRule {
    CSSRuleType type;
    std::string selector;
    std::unordered_map<std::string, std::string> properties;
    
    CSSRule(CSSRuleType t, const std::string& s) : type(t), selector(s) {}
};

// 样式节点类
class StyleNode : public BaseNode {
private:
    std::vector<CSSRule> rules;
    std::vector<std::string> importedStyles;  // 导入的样式组
    bool isGlobal;  // 是否为全局样式块
    
public:
    StyleNode(bool global = false, int line = 0, int col = 0)
        : BaseNode(NodeType::STYLE, line, col), isGlobal(global) {}
    
    // 规则管理
    void addRule(const CSSRule& rule);
    void addInlineProperty(const std::string& property, const std::string& value);
    void addClassRule(const std::string& className);
    void addIdRule(const std::string& idName);
    void addPseudoClassRule(const std::string& selector);
    void addPseudoElementRule(const std::string& selector);
    void addContextRule(const std::string& selector);
    
    // 获取规则
    const std::vector<CSSRule>& getRules() const { return rules; }
    std::vector<CSSRule> getRulesByType(CSSRuleType type) const;
    
    // 样式组导入
    void importStyle(const std::string& styleName);
    const std::vector<std::string>& getImportedStyles() const { return importedStyles; }
    
    // 全局/局部判断
    bool getIsGlobal() const { return isGlobal; }
    void setIsGlobal(bool global) { isGlobal = global; }
    
    // 获取需要自动添加的类名/ID
    std::string getAutoClassName() const;
    std::string getAutoIdName() const;
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class StyleNodeVisitor : public NodeVisitor {
public:
    virtual void visitStyleNode(StyleNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H