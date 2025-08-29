#pragma once

#include "../../Util/Common.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLConfig/ConfigurationManager.h"

namespace CHTL {

// 选择器自动化配置
struct SelectorAutomationConfig {
    bool disableStyleAutoAddClass = false;
    bool disableStyleAutoAddId = false;
    bool disableScriptAutoAddClass = true;
    bool disableScriptAutoAddId = true;
    
    // 从配置管理器中加载配置
    void loadFromConfiguration(const ConfigurationManager& configManager);
};

// 选择器类型
enum class SelectorType {
    CLASS,      // .box
    ID,         // #box
    TAG,        // div
    ATTRIBUTE,  // [data-id]
    PSEUDO,     // :hover
    COMPLEX     // .box div
};

// 选择器信息
struct SelectorInfo {
    SelectorType type;
    String name;
    String fullSelector;
    Position position;
    
    SelectorInfo(SelectorType t, const String& n, const String& full, const Position& pos)
        : type(t), name(n), fullSelector(full), position(pos) {}
};

// 引用规则类型
enum class ReferenceType {
    STYLE_AMPERSAND,    // & 在样式块中引用选择器
    SCRIPT_AMPERSAND,   // & 在脚本块中引用选择器
    SCRIPT_SELECTOR,    // {{.box}}, {{#box}} 在脚本中引用选择器
    VARIABLE_REFERENCE  // ThemeColor(tableColor) 变量引用
};

// 引用信息
struct ReferenceInfo {
    ReferenceType type;
    String referenceName;
    String resolvedValue;
    Position position;
    
    ReferenceInfo(ReferenceType t, const String& name, const String& value, const Position& pos)
        : type(t), referenceName(name), resolvedValue(value), position(pos) {}
};

// 选择器自动化管理器
class SelectorAutomationManager {
public:
    explicit SelectorAutomationManager(const SelectorAutomationConfig& config = SelectorAutomationConfig{});
    
    // 分析元素中的选择器
    StringVector analyzeElementSelectors(const BaseNode* elementNode);
    
    // 自动添加类名/ID到元素
    void autoAddSelectorsToElement(BaseNode* elementNode);
    
    // 处理局部样式块中的选择器
    void processLocalStyleSelectors(BaseNode* elementNode, BaseNode* styleNode);
    
    // 处理局部脚本块中的选择器
    void processLocalScriptSelectors(BaseNode* elementNode, BaseNode* scriptNode);
    
    // 解析& 引用
    String resolveAmpersandReference(const BaseNode* contextNode, const String& referenceContext) const;
    
    // 解析{{selector}}引用
    String resolveSelectorReference(const String& selectorRef, const BaseNode* contextNode) const;
    
    // 验证选择器的有效性
    bool validateSelector(const String& selector);
    
    // 获取第一个类选择器
    String getFirstClassSelector(const StringVector& selectors);
    
    // 获取第一个ID选择器
    String getFirstIdSelector(const StringVector& selectors);
    
    // 配置管理
    void updateConfig(const SelectorAutomationConfig& newConfig) { config = newConfig; }
    const SelectorAutomationConfig& getConfig() const { return config; }
    
private:
    SelectorAutomationConfig config;
    
    // 解析选择器字符串
    SelectorInfo parseSelector(const String& selectorString, const Position& position);
    
    // 提取样式块中的选择器
    StringVector extractStyleSelectors(const BaseNode* styleNode);
    
    // 提取脚本块中的选择器引用
    StringVector extractScriptSelectorReferences(const BaseNode* scriptNode);
    
    // 检查元素是否已有指定属性
    bool elementHasAttribute(const BaseNode* element, const String& attributeName) const;
    
    // 为元素添加属性
    void addAttributeToElement(BaseNode* element, const String& attributeName, const String& attributeValue);
    
    // 清理选择器名称（去掉. # 等前缀）
    String cleanSelectorName(const String& selector);
};

// 引用规则管理器
class ReferenceRuleManager {
public:
    // 处理所有引用规则
    void processReferences(BaseNode* documentNode, const SelectorAutomationManager& selectorManager);
    
    // 处理变量引用 ThemeColor(tableColor)
    String resolveVariableReference(const String& variableRef, const BaseNode* contextNode);
    
    // 处理命名空间引用 from space.room2
    String resolveNamespaceReference(const String& namespaceRef, const BaseNode* contextNode);
    
private:
    // 查找变量定义
    String findVariableDefinition(const String& variableName, const BaseNode* contextNode);
    
    // 查找命名空间定义
    BaseNode* findNamespaceDefinition(const String& namespaceName, const BaseNode* contextNode);
    
    // 递归处理节点中的引用
    void processNodeReferences(BaseNode* node, const SelectorAutomationManager& selectorManager);
};

// 全局选择器自动化实例
SelectorAutomationManager& getGlobalSelectorAutomation();
ReferenceRuleManager& getGlobalReferenceRuleManager();

} // namespace CHTL