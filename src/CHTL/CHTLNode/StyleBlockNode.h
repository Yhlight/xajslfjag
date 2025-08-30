#pragma once

#include "BaseNode.h"
#include "../../Util/Common.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

// 样式选择器类型
enum class StyleSelectorType {
    CLASS,          // .className
    ID,             // #idName
    ELEMENT,        // elementName
    ATTRIBUTE,      // [attr=value]
    PSEUDO_CLASS,   // :hover, :active
    PSEUDO_ELEMENT, // ::before, ::after
    SELF_REFERENCE, // & (父级引用)
    DESCENDANT,     // 后代选择器
    CHILD,          // > 子选择器
    SIBLING,        // + 相邻兄弟
    GENERAL_SIBLING, // ~ 一般兄弟
    UNIVERSAL,      // * 通用选择器
    GROUP           // 选择器组合
};

// 样式属性
struct StyleProperty {
    String name;            // 属性名
    String value;           // 属性值
    bool isImportant;       // 是否 !important
    String originalValue;   // 原始值 (未处理变量替换前)
    bool isVariable;        // 是否为变量引用
    bool isCalculated;      // 是否为计算值
    
    StyleProperty() : isImportant(false), isVariable(false), isCalculated(false) {}
    StyleProperty(const String& n, const String& v) : name(n), value(v), 
                 isImportant(false), isVariable(false), isCalculated(false) {}
};

// 样式选择器
class StyleSelector {
public:
    StyleSelectorType type;     // 选择器类型
    String selector;            // 选择器字符串
    String pseudoClass;         // 伪类
    String pseudoElement;       // 伪元素
    StringVector classes;       // 类名列表
    StringVector ids;           // ID列表
    String element;             // 元素名
    String combinator;          // 组合器 (>, +, ~, space)
    std::shared_ptr<StyleSelector> parent; // 父级选择器
    std::vector<std::shared_ptr<StyleSelector>> children; // 子选择器
    
    explicit StyleSelector(StyleSelectorType t = StyleSelectorType::ELEMENT, const String& sel = "");
    ~StyleSelector() = default;
    
    // 选择器构建
    void addClassName(const String& className);
    void addId(const String& idName);
    void setPseudoClass(const String& pseudo);
    void setPseudoElement(const String& pseudo);
    void setCombinator(const String& comb);
    void setParent(std::shared_ptr<StyleSelector> parentSelector);
    void addChild(std::shared_ptr<StyleSelector> childSelector);
    
    // 选择器解析
    static std::shared_ptr<StyleSelector> parseSelector(const String& selectorString);
    static StyleSelectorType getSelectorType(const String& selectorString);
    
    // 选择器生成
    String toString() const;
    String toCSS() const;
    String getSpecificityString() const;
    int getSpecificity() const;
    
    // 选择器匹配
    bool matches(const String& element, const StringVector& classes = {}, const String& id = "") const;
    bool isDescendantOf(const StyleSelector& ancestor) const;
    
    // 选择器优化
    void optimize();
    bool canBeMerged(const StyleSelector& other) const;
    std::shared_ptr<StyleSelector> merge(const StyleSelector& other) const;
    
private:
    // 内部解析方法
    void parseSimpleSelector(const String& selector);
    void parsePseudoClasses(const String& selector);
    void parseCombinators(const String& selector);
    
    // 特异性计算
    int calculateSpecificity() const;
};

// 样式规则
class StyleRule {
public:
    std::shared_ptr<StyleSelector> selector;    // 选择器
    std::vector<StyleProperty> properties;      // 属性列表
    StringVector mediaQueries;                  // 媒体查询
    String namespace_;                          // 命名空间
    bool isNested;                             // 是否嵌套规则
    int specificity;                           // 特异性
    size_t sourceOrder;                        // 源码顺序
    
    explicit StyleRule(std::shared_ptr<StyleSelector> sel = nullptr);
    ~StyleRule() = default;
    
    // 属性管理
    void addProperty(const String& name, const String& value, bool important = false);
    void addProperty(const StyleProperty& property);
    void removeProperty(const String& name);
    StyleProperty* getProperty(const String& name);
    const StyleProperty* getProperty(const String& name) const;
    bool hasProperty(const String& name) const;
    size_t getPropertyCount() const { return properties.size(); }
    
    // 媒体查询
    void addMediaQuery(const String& query);
    void setMediaQueries(const StringVector& queries);
    bool hasMediaQueries() const { return !mediaQueries.empty(); }
    
    // 规则生成
    String toString() const;
    String toCSS(int indentLevel = 0) const;
    String toCSSWithMediaQuery() const;
    
    // 规则合并和优化
    bool canBeMerged(const StyleRule& other) const;
    std::shared_ptr<StyleRule> merge(const StyleRule& other) const;
    void optimize();
    void deduplicateProperties();
    
    // 规则验证
    bool isValid() const;
    StringVector getValidationErrors() const;
    
    // 特异性管理
    void updateSpecificity();
    int getSpecificity() const { return specificity; }
    
private:
    void calculateSpecificity();
    bool isValidProperty(const String& name, const String& value) const;
};

// 样式块节点
class StyleBlockNode : public BaseNode {
public:
    std::vector<std::shared_ptr<StyleRule>> rules;  // 样式规则列表
    String blockType;                               // 块类型 (inline, external)
    bool isInline;                                  // 是否内联样式
    bool autoAddClass;                              // 是否自动添加类名
    bool autoAddId;                                 // 是否自动添加ID
    String scopeSelector;                           // 作用域选择器
    StringUnorderedMap variables;                   // CSS变量
    StringVector imports;                           // CSS导入
    StringVector fontFaces;                         // 字体定义
    StringVector keyframes;                         // 动画关键帧
    
    explicit StyleBlockNode(const Position& pos = Position());
    ~StyleBlockNode() override = default;
    
    // 规则管理
    void addRule(std::shared_ptr<StyleRule> rule);
    void removeRule(size_t index);
    void removeRule(const String& selectorString);
    std::shared_ptr<StyleRule> getRule(size_t index) const;
    std::shared_ptr<StyleRule> getRule(const String& selectorString) const;
    std::vector<std::shared_ptr<StyleRule>> getRulesBySelector(const String& selectorPattern) const;
    size_t getRuleCount() const { return rules.size(); }
    
    // 快速样式添加
    void addStyle(const String& selector, const String& property, const String& value);
    void addClassStyle(const String& className, const String& property, const String& value);
    void addIdStyle(const String& idName, const String& property, const String& value);
    void addElementStyle(const String& element, const String& property, const String& value);
    void addPseudoStyle(const String& selector, const String& pseudo, const String& property, const String& value);
    
    // 自引用样式 (&)
    void addSelfReferenceStyle(const String& pseudoClass, const String& property, const String& value);
    void addSelfReferenceStyle(const String& property, const String& value);
    std::shared_ptr<StyleRule> createSelfReferenceRule(const String& pseudoClass = "");
    String resolveSelfReference(const String& selector, const String& parentSelector) const;
    
    // 嵌套样式处理
    void processNestedStyles();
    void flattenNestedRules();
    void expandSelfReferences(const String& parentSelector = "");
    std::vector<std::shared_ptr<StyleRule>> expandRule(std::shared_ptr<StyleRule> rule, const String& parentSelector = "") const;
    
    // CSS变量支持
    void addVariable(const String& name, const String& value);
    void removeVariable(const String& name);
    String getVariable(const String& name) const;
    bool hasVariable(const String& name) const;
    void expandVariables();
    String expandVariableReferences(const String& value) const;
    
    // 导入和外部资源
    void addImport(const String& url, const String& mediaQuery = "");
    void addFontFace(const String& fontFamily, const String& src, const StringUnorderedMap& properties = {});
    void addKeyframes(const String& name, const String& keyframeRules);
    
    // 自动化功能
    void enableAutoAddClass(bool enable = true) { autoAddClass = enable; }
    void enableAutoAddId(bool enable = true) { autoAddId = enable; }
    void setScopeSelector(const String& selector) { scopeSelector = selector; }
    void applyAutoClassNames(const String& baseSelector);
    void applyAutoIds(const String& baseSelector);
    void applyScopeSelector();
    
    // 样式优化
    void optimize();
    void mergeCompatibleRules();
    void removeEmptyRules();
    void sortRulesBySpecificity();
    void deduplicateRules();
    void minifyPropertyValues();
    
    // 样式生成
    String toCSS(int indentLevel = 0) const;
    String toInlineCSS() const;
    String toCSSWithMediaQueries() const;
    String generateScopedCSS(const String& scopePrefix) const;
    
    // 样式验证
    bool validateStyles() const;
    StringVector getStyleValidationErrors() const;
    bool hasConflictingRules() const;
    std::vector<std::pair<size_t, size_t>> getConflictingRulePairs() const;
    
    // 样式统计
    std::unordered_map<String, size_t> getPropertyUsageStats() const;
    std::unordered_map<String, size_t> getSelectorUsageStats() const;
    size_t getTotalDeclarations() const;
    double getAverageSpecificity() const;
    
    // BaseNode接口
    NodeType getType() const override { return NodeType::STYLE; }
    String getValue() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
    // 工厂方法
    static std::unique_ptr<StyleBlockNode> createInlineStyle();
    static std::unique_ptr<StyleBlockNode> createExternalStyle();
    static std::unique_ptr<StyleBlockNode> createScopedStyle(const String& scope);
    
    // 解析方法
    static std::shared_ptr<StyleRule> parseStyleRule(const String& ruleString);
    static StyleProperty parseStyleProperty(const String& propertyString);
    static StringVector parseMediaQuery(const String& mediaString);
    
private:
    // 内部辅助方法
    void processRule(std::shared_ptr<StyleRule> rule);
    void processNestedRule(std::shared_ptr<StyleRule> rule, const String& parentSelector);
    void processSelfReference(std::shared_ptr<StyleRule> rule, const String& parentSelector);
    
    // 优化内部方法
    bool canMergeRules(const StyleRule& rule1, const StyleRule& rule2) const;
    std::shared_ptr<StyleRule> mergeRules(const StyleRule& rule1, const StyleRule& rule2) const;
    
    // 验证内部方法
    bool isValidCSS(const String& css) const;
    bool isValidSelector(const String& selector) const;
    bool isValidProperty(const String& property, const String& value) const;
    
    // 工具方法
    String generateUniqueClassName() const;
    String generateUniqueId() const;
    String normalizeSelector(const String& selector) const;
    int calculateRuleSpecificity(const StyleRule& rule) const;
};

// 样式处理器
class StyleProcessor {
public:
    StyleProcessor() = default;
    ~StyleProcessor() = default;
    
    // 样式处理
    static String processInlineStyle(const String& styleContent, const String& elementSelector = "");
    static String processBlockStyle(const StyleBlockNode& styleBlock, const String& scopePrefix = "");
    static String expandSelfReferences(const String& css, const String& parentSelector);
    static String optimizeCSS(const String& css);
    
    // 选择器处理
    static String combineSelectors(const String& parent, const String& child, const String& combinator = " ");
    static String expandSelectorGroups(const String& selectorGroup);
    static StringVector splitSelectors(const String& selectorGroup);
    static String normalizeSelector(const String& selector);
    
    // 变量处理
    static String expandCSSVariables(const String& css, const StringUnorderedMap& variables);
    static StringUnorderedMap extractCSSVariables(const String& css);
    
    // 媒体查询处理
    static String wrapWithMediaQuery(const String& css, const String& mediaQuery);
    static String extractMediaQueries(const String& css);
    
    // 优化功能
    static String minifyCSS(const String& css);
    static String beautifyCSS(const String& css);
    static String removeComments(const String& css);
    static String removeDuplicateRules(const String& css);
    
    // 验证功能
    static bool validateCSS(const String& css);
    static StringVector getCSSErrors(const String& css);
    static bool isValidSelector(const String& selector);
    static bool isValidProperty(const String& property, const String& value);
    
    // 转换功能
    static String convertToSass(const String& css);
    static String convertToLess(const String& css);
    static String convertToStylus(const String& css);
    
private:
    // 内部处理方法
    static String processNestedRules(const String& css, const String& parentSelector = "");
    static String processSelfReferences(const String& css, const String& parentSelector);
    static String processVariableReferences(const String& css, const StringUnorderedMap& variables);
    
    // 解析辅助
    static std::pair<String, String> parsePropertyDeclaration(const String& declaration);
    static StringVector parseRuleBlock(const String& ruleBlock);
    static String extractSelectorFromRule(const String& rule);
    static String extractDeclarationsFromRule(const String& rule);
};

} // namespace CHTL