#ifndef CHTL_SELECTOR_AUTOMATION_H
#define CHTL_SELECTOR_AUTOMATION_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <optional>

namespace CHTL {

// 选择器类型
enum class SelectorType {
    Class,      // .classname
    Id,         // #idname
    Tag,        // tagname
    Attribute,  // [attribute]
    Pseudo,     // :pseudo
    Universal,  // *
    Reference,  // & 引用选择器
    Unknown
};

// 选择器信息
struct SelectorInfo {
    SelectorType type;
    std::string value;
    std::string raw;       // 原始选择器字符串
    size_t line;
    size_t column;
};

// 配置选项
struct AutomationConfig {
    bool disableStyleAutoAddClass = false;
    bool disableStyleAutoAddId = false;
    bool disableScriptAutoAddClass = true;
    bool disableScriptAutoAddId = true;
    bool disableDefaultNamespace = false;
    
    // 从配置块解析
    static AutomationConfig fromConfigBlock(const std::unordered_map<std::string, std::string>& config);
};

// 选择器自动化管理器
class SelectorAutomation {
public:
    SelectorAutomation();
    ~SelectorAutomation() = default;
    
    // 设置配置
    void setConfig(const AutomationConfig& config) { config_ = config; }
    const AutomationConfig& getConfig() const { return config_; }
    
    // 解析选择器
    std::vector<SelectorInfo> parseSelectors(const std::string& css);
    
    // 从局部样式块提取选择器
    std::vector<SelectorInfo> extractFromStyleBlock(const std::string& styleContent);
    
    // 从局部脚本块提取选择器
    std::vector<SelectorInfo> extractFromScriptBlock(const std::string& scriptContent);
    
    // 获取第一个类选择器
    std::optional<std::string> getFirstClassSelector(const std::vector<SelectorInfo>& selectors);
    
    // 获取第一个ID选择器
    std::optional<std::string> getFirstIdSelector(const std::vector<SelectorInfo>& selectors);
    
    // 判断是否需要自动添加类
    bool shouldAutoAddClass(bool hasClassAttribute, bool hasStyleClassSelector) const;
    
    // 判断是否需要自动添加ID
    bool shouldAutoAddId(bool hasIdAttribute, bool hasStyleIdSelector) const;
    
    // 判断脚本是否需要自动添加类
    bool shouldScriptAutoAddClass(bool hasClassAttribute, bool hasStyleAutoAdded) const;
    
    // 判断脚本是否需要自动添加ID
    bool shouldScriptAutoAddId(bool hasIdAttribute, bool hasStyleAutoAdded) const;
    
    // 解析引用选择器 &
    std::string resolveReferenceSelector(const std::string& selector, 
                                       const std::string& contextClass,
                                       const std::string& contextId,
                                       bool isInStyle = true);
    
    // 处理选择器转换
    std::string processSelector(const std::string& selector,
                              const std::string& autoClass = "",
                              const std::string& autoId = "");

private:
    AutomationConfig config_;
    
    // 辅助方法
    SelectorType identifySelectorType(const std::string& selector);
    std::string extractSelectorValue(const std::string& selector, SelectorType type);
    bool isValidSelector(const std::string& selector);
    std::string normalizeSelector(const std::string& selector);
};

// 选择器引用解析器
class SelectorReferenceResolver {
public:
    SelectorReferenceResolver();
    ~SelectorReferenceResolver() = default;
    
    // 设置上下文
    void setContext(const std::string& elementClass, const std::string& elementId);
    
    // 解析引用
    std::string resolve(const std::string& selector);
    
    // 解析复合选择器
    std::string resolveCompound(const std::string& selector);
    
    // 是否包含引用
    bool hasReference(const std::string& selector) const;
    
    // 获取引用类型（优先级）
    SelectorType getReferencePreference(bool isInStyle) const;

private:
    std::string contextClass_;
    std::string contextId_;
    
    // 解析单个引用
    std::string resolveSingleReference(const std::string& ref, bool isInStyle);
    
    // 分割选择器
    std::vector<std::string> splitSelector(const std::string& selector);
};

// CHTL JS选择器处理器
class CHTLJSSelectorProcessor {
public:
    CHTLJSSelectorProcessor();
    ~CHTLJSSelectorProcessor() = default;
    
    // 处理CHTL JS选择器 {{selector}}
    std::string processSelector(const std::string& selector);
    
    // 提取CHTL JS选择器
    std::vector<std::string> extractSelectors(const std::string& code);
    
    // 判断是否是CHTL JS选择器
    bool isCHTLJSSelector(const std::string& str) const;
    
    // 转换为JavaScript代码
    std::string toJavaScript(const std::string& selector);
    
    // 支持的选择器格式
    // {{.class}}
    // {{#id}}
    // {{tag}}
    // {{tag[index]}}
    
    // 判断选择器类型
    SelectorType getType(const std::string& selector) const;
    
    // 获取选择器值
    std::string getValue(const std::string& selector) const;
    
    // 获取索引（如果有）
    std::optional<int> getIndex(const std::string& selector) const;

private:
    // 解析选择器
    struct ParsedSelector {
        SelectorType type;
        std::string value;
        std::optional<int> index;
    };
    
    ParsedSelector parseSelector(const std::string& selector) const;
    
    // 生成querySelector代码
    std::string generateQuerySelector(const ParsedSelector& parsed);
};

// 引用规则管理器
class ReferenceRuleManager {
public:
    static ReferenceRuleManager& getInstance() {
        static ReferenceRuleManager instance;
        return instance;
    }
    
    // 注册引用规则
    void registerRule(const std::string& name, const std::string& pattern);
    
    // 获取引用规则
    std::optional<std::string> getRule(const std::string& name) const;
    
    // 应用引用规则
    std::string applyRule(const std::string& name, const std::string& context);
    
    // 检查规则是否存在
    bool hasRule(const std::string& name) const;
    
    // 删除规则
    void removeRule(const std::string& name);
    
    // 清空所有规则
    void clearRules();

private:
    ReferenceRuleManager() = default;
    ReferenceRuleManager(const ReferenceRuleManager&) = delete;
    ReferenceRuleManager& operator=(const ReferenceRuleManager&) = delete;
    
    std::unordered_map<std::string, std::string> rules_;
};

} // namespace CHTL

#endif // CHTL_SELECTOR_AUTOMATION_H