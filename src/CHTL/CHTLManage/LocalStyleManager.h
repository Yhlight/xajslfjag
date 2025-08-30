#pragma once
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * 局部样式类型
 */
enum class LocalStyleType {
    INLINE_STYLE,           // 内联样式
    CLASS_SELECTOR,         // 类选择器
    ID_SELECTOR,            // ID选择器
    PSEUDO_CLASS,           // 伪类选择器
    PSEUDO_ELEMENT,         // 伪元素选择器
    CONTEXT_SELECTOR        // 上下文选择器（&）
};

/**
 * 样式规则信息
 */
struct StyleRule {
    std::string selector;                   // 选择器
    std::unordered_map<std::string, std::string> properties; // CSS属性
    LocalStyleType type;                    // 样式类型
    std::string originalSelector;           // 原始选择器
    ElementNode* targetElement;             // 目标元素
    bool isGlobal;                         // 是否为全局样式
    size_t priority;                       // 优先级
    
    StyleRule() : targetElement(nullptr), isGlobal(false), priority(0) {}
};

/**
 * 自动化规则配置
 */
struct AutomationRules {
    bool enableAutoClass = true;            // 启用自动类名
    bool enableAutoId = true;               // 启用自动ID
    bool disableStyleAutoAddClass = false;  // 禁用样式自动添加类选择器
    bool disableStyleAutoAddId = false;     // 禁用样式自动添加ID选择器
    bool disableScriptAutoAddClass = true;  // 禁用脚本自动添加类选择器
    bool disableScriptAutoAddId = true;     // 禁用脚本自动添加ID选择器
    std::string classPrefix = "";           // 类名前缀
    std::string idPrefix = "";              // ID前缀
};

/**
 * 局部样式块管理器
 * 按照CHTL语法文档实现局部样式块的所有功能
 */
class LocalStyleManager {
public:
    /**
     * 构造函数
     */
    LocalStyleManager();
    
    /**
     * 析构函数
     */
    ~LocalStyleManager() = default;
    
    // ========== 样式块处理 ==========
    
    /**
     * 处理局部样式块
     */
    void processLocalStyleBlock(StyleNode* styleNode, ElementNode* parentElement);
    
    /**
     * 处理内联样式
     * 将style{}内部的属性转换为元素的内联样式
     */
    std::string processInlineStyle(const StyleNode* styleNode);
    
    /**
     * 处理选择器样式
     * 将类/ID选择器的代码自动添加到全局样式块
     */
    std::vector<StyleRule> processSelectorStyles(const StyleNode* styleNode, ElementNode* parentElement);
    
    /**
     * 处理上下文推导
     * 处理&符号的上下文推导功能
     */
    std::string processContextSelector(const std::string& selector, ElementNode* element);
    
    // ========== 自动化类名/ID管理 ==========
    
    /**
     * 自动添加类名
     * 在局部样式块中使用类选择器时自动为元素添加类名
     */
    std::string autoAddClassName(ElementNode* element, const std::string& className);
    
    /**
     * 自动添加ID
     * 在局部样式块中使用ID选择器时自动为元素添加ID
     */
    std::string autoAddId(ElementNode* element, const std::string& idName);
    
    /**
     * 生成自动类名
     * 当没有指定类名时生成唯一的类名
     */
    std::string generateAutoClassName(const std::string& base = "auto");
    
    /**
     * 生成自动ID
     * 当没有指定ID时生成唯一的ID
     */
    std::string generateAutoId(const std::string& base = "auto");
    
    // ========== 选择器处理 ==========
    
    /**
     * 解析选择器
     */
    LocalStyleType parseSelector(const std::string& selector);
    
    /**
     * 提取选择器名称
     * 从.box, #container等选择器中提取名称
     */
    std::string extractSelectorName(const std::string& selector);
    
    /**
     * 验证选择器格式
     */
    bool validateSelector(const std::string& selector);
    
    /**
     * 处理伪类选择器
     * 如&:hover, &:focus等
     */
    std::string processPseudoClassSelector(const std::string& selector, ElementNode* element);
    
    /**
     * 处理伪元素选择器
     * 如&::before, &::after等
     */
    std::string processPseudoElementSelector(const std::string& selector, ElementNode* element);
    
    // ========== 全局样式管理 ==========
    
    /**
     * 添加全局样式规则
     */
    void addGlobalStyleRule(const StyleRule& rule);
    
    /**
     * 获取所有全局样式规则
     */
    const std::vector<StyleRule>& getGlobalStyleRules() const;
    
    /**
     * 生成全局CSS
     */
    std::string generateGlobalCSS();
    
    /**
     * 清空全局样式
     */
    void clearGlobalStyles();
    
    // ========== 优先级和冲突处理 ==========
    
    /**
     * 计算样式优先级
     */
    size_t calculateStylePriority(const StyleRule& rule);
    
    /**
     * 解决样式冲突
     * 按照书写顺序进行值的替换
     */
    void resolveStyleConflicts();
    
    /**
     * 合并重复的选择器
     */
    void mergeDuplicateSelectors();
    
    // ========== 配置和规则 ==========
    
    /**
     * 设置自动化规则
     */
    void setAutomationRules(const AutomationRules& rules);
    
    /**
     * 获取自动化规则
     */
    const AutomationRules& getAutomationRules() const;
    
    /**
     * 设置错误报告器
     */
    void setErrorReporter(ErrorReporter* errorReporter);
    
    // ========== 上下文管理 ==========
    
    /**
     * 设置当前元素上下文
     */
    void setCurrentElement(ElementNode* element);
    
    /**
     * 获取当前元素上下文
     */
    ElementNode* getCurrentElement() const;
    
    /**
     * 推入元素上下文栈
     */
    void pushElementContext(ElementNode* element);
    
    /**
     * 弹出元素上下文栈
     */
    ElementNode* popElementContext();
    
    // ========== 统计和调试 ==========
    
    /**
     * 获取处理统计信息
     */
    std::unordered_map<std::string, size_t> getStatistics() const;
    
    /**
     * 获取自动生成的类名列表
     */
    std::vector<std::string> getGeneratedClassNames() const;
    
    /**
     * 获取自动生成的ID列表
     */
    std::vector<std::string> getGeneratedIds() const;
    
    /**
     * 重置管理器状态
     */
    void reset();

private:
    AutomationRules m_automationRules;      // 自动化规则
    ErrorReporter* m_errorReporter;         // 错误报告器
    
    // 样式管理
    std::vector<StyleRule> m_globalStyles;  // 全局样式规则
    std::unordered_map<std::string, StyleRule> m_selectorMap; // 选择器映射
    
    // 名称生成和管理
    size_t m_classCounter;                  // 类名计数器
    size_t m_idCounter;                     // ID计数器
    std::unordered_set<std::string> m_usedClassNames;  // 已使用的类名
    std::unordered_set<std::string> m_usedIds;         // 已使用的ID
    std::vector<std::string> m_generatedClasses;       // 生成的类名
    std::vector<std::string> m_generatedIds;           // 生成的ID
    
    // 上下文管理
    std::vector<ElementNode*> m_elementStack;   // 元素上下文栈
    ElementNode* m_currentElement;              // 当前元素
    
    // 统计信息
    std::unordered_map<std::string, size_t> m_statistics; // 处理统计
    
    // ========== 内部辅助方法 ==========
    
    /**
     * 处理单个CSS属性
     */
    void processCSSProperty(StyleRule& rule, const std::string& property, const std::string& value);
    
    /**
     * 验证CSS属性名
     */
    bool validateCSSProperty(const std::string& property);
    
    /**
     * 验证CSS属性值
     */
    bool validateCSSValue(const std::string& property, const std::string& value);
    
    /**
     * 生成唯一名称
     */
    std::string generateUniqueName(const std::string& base, 
                                 const std::unordered_set<std::string>& usedNames);
    
    /**
     * 检查名称是否已使用
     */
    bool isNameUsed(const std::string& name, bool isClass) const;
    
    /**
     * 更新统计信息
     */
    void updateStatistics(const std::string& key, size_t increment = 1);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
    
    /**
     * 报告警告
     */
    void reportWarning(const std::string& message);
    
    /**
     * 解析选择器组合
     * 处理复杂的选择器组合，如".box:hover"
     */
    std::vector<std::string> parseSelectorCombination(const std::string& selector);
    
    /**
     * 解析伪选择器
     */
    std::pair<std::string, std::string> parsePseudoSelector(const std::string& selector);
    
    /**
     * 格式化CSS规则
     */
    std::string formatCSSRule(const StyleRule& rule);
    
    /**
     * 优化CSS输出
     */
    std::string optimizeCSS(const std::string& css);
    
    /**
     * 处理CSS变量
     */
    std::string processCSSVariables(const std::string& value);
};

/**
 * 局部样式管理器工厂
 */
class LocalStyleManagerFactory {
public:
    /**
     * 创建标准管理器
     */
    static std::unique_ptr<LocalStyleManager> createStandardManager();
    
    /**
     * 创建严格模式管理器
     */
    static std::unique_ptr<LocalStyleManager> createStrictManager();
    
    /**
     * 创建自定义管理器
     */
    static std::unique_ptr<LocalStyleManager> createCustomManager(const AutomationRules& rules);
};

} // namespace CHTL