#ifndef CHTL_SELECTOR_MANAGER_H
#define CHTL_SELECTOR_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include "../CHTLContext/CHTLContext.h"

namespace CHTL {

// 选择器自动化规则
struct SelectorAutoRule {
    bool autoAddClass;          // 是否自动添加类选择器
    bool autoAddId;             // 是否自动添加ID选择器
    bool preferClassOverId;     // 是否优先使用类选择器
    bool onlyFirstSelector;     // 是否只使用第一个选择器
    
    SelectorAutoRule() : 
        autoAddClass(true), 
        autoAddId(true), 
        preferClassOverId(true), 
        onlyFirstSelector(true) {}
};

// 选择器管理器 - 负责选择器的解析、管理和自动化
class SelectorManager {
private:
    // 样式选择器自动化规则
    SelectorAutoRule styleAutoRule;
    
    // 脚本选择器自动化规则
    SelectorAutoRule scriptAutoRule;
    
    // 辅助方法
    SelectorType parseSelectorType(const std::string& selector) const;
    std::string extractSelectorValue(const std::string& selector) const;
    bool isValidSelector(const std::string& selector) const;
    
public:
    SelectorManager();
    ~SelectorManager() = default;
    
    // 规则配置
    void setStyleAutoRule(const SelectorAutoRule& rule);
    void setScriptAutoRule(const SelectorAutoRule& rule);
    SelectorAutoRule getStyleAutoRule() const { return styleAutoRule; }
    SelectorAutoRule getScriptAutoRule() const { return scriptAutoRule; }
    
    // 选择器解析
    SelectorInfo parseSelector(const std::string& selector) const;
    std::vector<SelectorInfo> parseSelectors(const std::string& selectorList) const;
    SelectorType getSelectorType(const std::string& selector) const;
    std::string getSelectorValue(const std::string& selector) const;
    
    // 选择器验证
    bool isClassSelector(const std::string& selector) const;
    bool isIdSelector(const std::string& selector) const;
    bool isElementSelector(const std::string& selector) const;
    bool isReferenceSelector(const std::string& selector) const;
    bool isPseudoClass(const std::string& selector) const;
    bool isPseudoElement(const std::string& selector) const;
    
    // 选择器自动化
    std::pair<std::string, std::string> autoGenerateSelectors(
        const std::vector<SelectorInfo>& styleSelectors,
        const std::vector<SelectorInfo>& scriptSelectors,
        bool hasExistingClass,
        bool hasExistingId,
        bool isInStyle
    ) const;
    
    std::string selectFirstClassSelector(const std::vector<SelectorInfo>& selectors) const;
    std::string selectFirstIdSelector(const std::vector<SelectorInfo>& selectors) const;
    
    // 引用解析
    std::string resolveAmpersandReference(
        const std::string& elementId,
        const std::vector<std::string>& elementClasses,
        bool isInStyle
    ) const;
    
    // 选择器转换
    std::string convertToCSS(const SelectorInfo& selector) const;
    std::string convertToJavaScript(const SelectorInfo& selector) const;
    std::string combineSectors(const std::vector<SelectorInfo>& selectors) const;
    
    // 选择器优先级
    int getSelectorSpecificity(const SelectorInfo& selector) const;
    std::vector<SelectorInfo> sortBySpecificity(const std::vector<SelectorInfo>& selectors) const;
    
    // 批量操作
    void applyAutoSelectors(ElementContext& element) const;
    void markAutoAddedSelectors(std::vector<SelectorInfo>& selectors, const std::string& addedValue) const;
    
    // 验证
    bool validateSelectors(const std::vector<SelectorInfo>& selectors) const;
    std::vector<std::string> getSelectorErrors(const std::vector<SelectorInfo>& selectors) const;
};

} // namespace CHTL

#endif // CHTL_SELECTOR_MANAGER_H