#pragma once
#include "BaseNode.hpp"
#include <unordered_map>

namespace CHTL {

/**
 * 样式块类型枚举
 */
enum class StyleBlockType {
    GLOBAL,               // 全局样式块
    LOCAL,                // 局部样式块
    INLINE,               // 内联样式
    SELECTOR,             // 选择器样式
    MEDIA_QUERY,          // 媒体查询
    KEYFRAMES            // 关键帧动画
};

/**
 * CSS选择器类型
 */
enum class SelectorType {
    ELEMENT,              // 元素选择器
    CLASS,                // 类选择器
    ID,                   // ID选择器
    PSEUDO_CLASS,         // 伪类选择器
    PSEUDO_ELEMENT,       // 伪元素选择器
    REFERENCE,            // 引用选择器 (&)
    COMPOUND,             // 复合选择器
    DESCENDANT,           // 后代选择器
    CHILD,                // 子元素选择器
    SIBLING,              // 兄弟选择器
    ADJACENT              // 相邻选择器
};

/**
 * CSS属性结构
 */
struct CSSProperty {
    std::string name;
    std::string value;
    bool isImportant;
    bool isVariable;      // 是否是CSS变量
    bool isReference;     // 是否是CHTL变量引用
    
    CSSProperty() : isImportant(false), isVariable(false), isReference(false) {}
    CSSProperty(const std::string& n, const std::string& v, bool important = false)
        : name(n), value(v), isImportant(important), isVariable(false), isReference(false) {}
};

/**
 * CSS选择器结构
 */
struct CSSSelector {
    SelectorType type;
    std::string value;
    std::string pseudoElement;
    std::string pseudoClass;
    std::vector<std::string> combinators; // 组合器
    
    CSSSelector() : type(SelectorType::ELEMENT) {}
    CSSSelector(SelectorType t, const std::string& v) : type(t), value(v) {}
    
    std::string toString() const;
    bool isCompound() const;
};

/**
 * CHTL样式节点类
 * 处理CHTL的局部样式块和全局样式
 */
class StyleNode : public BaseNode {
private:
    StyleBlockType blockType;
    std::vector<CSSSelector> selectors;
    std::unordered_map<std::string, CSSProperty> properties;
    std::string mediaQuery;                    // 媒体查询条件
    std::string keyframeName;                  // 关键帧名称
    bool autoAddClass;                         // 是否自动添加类名
    bool autoAddId;                           // 是否自动添加ID
    std::string autoClassName;                 // 自动生成的类名
    std::string autoIdName;                    // 自动生成的ID名
    std::shared_ptr<BaseNode> targetElement;   // 目标元素（用于局部样式）

public:
    StyleNode(StyleBlockType type = StyleBlockType::LOCAL);
    
    // 样式块类型
    StyleBlockType getBlockType() const;
    void setBlockType(StyleBlockType type);
    
    // 选择器管理
    void addSelector(const CSSSelector& selector);
    void removeSelector(size_t index);
    void clearSelectors();
    const std::vector<CSSSelector>& getSelectors() const;
    size_t getSelectorCount() const;
    
    // 便捷选择器方法
    void addClassSelector(const std::string& className);
    void addIdSelector(const std::string& idName);
    void addElementSelector(const std::string& elementName);
    void addPseudoClassSelector(const std::string& className, const std::string& pseudoClass);
    void addPseudoElementSelector(const std::string& className, const std::string& pseudoElement);
    void addReferenceSelector(const std::string& reference = "&");
    
    // CSS属性管理
    void setProperty(const std::string& name, const std::string& value, bool important = false);
    void setProperty(const CSSProperty& property);
    CSSProperty getProperty(const std::string& name) const;
    bool hasProperty(const std::string& name) const;
    void removeProperty(const std::string& name);
    const std::unordered_map<std::string, CSSProperty>& getProperties() const;
    
    // 特殊属性处理
    void setVariableProperty(const std::string& name, const std::string& value);
    void setReferenceProperty(const std::string& name, const std::string& variableRef);
    
    // 媒体查询
    const std::string& getMediaQuery() const;
    void setMediaQuery(const std::string& query);
    bool hasMediaQuery() const;
    
    // 关键帧动画
    const std::string& getKeyframeName() const;
    void setKeyframeName(const std::string& name);
    bool isKeyframes() const;
    
    // 自动化功能
    bool getAutoAddClass() const;
    void setAutoAddClass(bool autoAdd);
    
    bool getAutoAddId() const;
    void setAutoAddId(bool autoAdd);
    
    const std::string& getAutoClassName() const;
    void setAutoClassName(const std::string& className);
    
    const std::string& getAutoIdName() const;
    void setAutoIdName(const std::string& idName);
    
    // 目标元素
    std::shared_ptr<BaseNode> getTargetElement() const;
    void setTargetElement(std::shared_ptr<BaseNode> element);
    
    // 样式处理
    std::string generateCSS() const;
    std::string generateInlineStyle() const;
    void processReferences(const std::unordered_map<std::string, std::string>& references);
    
    // 选择器解析
    std::vector<std::string> resolveSelectors() const;
    std::string resolveSingleSelector(const CSSSelector& selector) const;
    
    // 自动化处理
    void applyAutomation();
    void updateTargetElementClasses();
    void updateTargetElementId();
    
    // 重写基类方法
    std::string toString() const override;
    std::string toHTML() const override;
    std::string toCSS() const override;
    void validate(CHTLState& state) override;
    std::shared_ptr<BaseNode> clone() const override;
    
    // 样式合并
    void mergeWith(const StyleNode& other);
    bool isCompatibleWith(const StyleNode& other) const;
    
private:
    std::string getBlockTypeString() const;
    std::string formatProperty(const CSSProperty& property) const;
    std::string processVariableReferences(const std::string& value) const;
    bool isInlineStyleProperty(const std::string& name) const;
    std::string generateSelectorString() const;
};

/**
 * 样式处理器
 * 提供样式相关的工具方法
 */
class StyleProcessor {
public:
    // CSS选择器工具
    static CSSSelector parseSelector(const std::string& selectorString);
    static std::string combinedSelectors(const std::vector<CSSSelector>& selectors);
    static bool isValidSelector(const std::string& selector);
    
    // CSS属性工具
    static bool isValidProperty(const std::string& name);
    static bool isValidValue(const std::string& name, const std::string& value);
    static std::string normalizePropertyName(const std::string& name);
    static std::string normalizePropertyValue(const std::string& value);
    
    // CSS变量工具
    static bool isCSSVariable(const std::string& name);
    static std::string formatCSSVariable(const std::string& name);
    static std::string resolveCSSVariable(const std::string& variableRef, 
                                         const std::unordered_map<std::string, std::string>& variables);
    
    // 选择器自动化
    static std::string generateUniqueClassName(const std::string& prefix = "chtl");
    static std::string generateUniqueIdName(const std::string& prefix = "chtl");
    
    // 样式优化
    static std::string minifyCSS(const std::string& css);
    static std::string prettifyCSS(const std::string& css);
    
private:
    static size_t classCounter;
    static size_t idCounter;
};

}