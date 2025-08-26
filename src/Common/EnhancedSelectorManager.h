#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <regex>

namespace CHTL {
namespace Common {

// 选择器类型
enum class SelectorType {
    TAG_SELECTOR,           // 标签选择器 button
    CLASS_SELECTOR,         // 类选择器 .box
    ID_SELECTOR,            // ID选择器 #box
    UNIVERSAL_SELECTOR,     // 通用选择器 box (先判断tag，再找class/id)
    PRECISE_SELECTOR,       // 精确选择器 {{选择器}}
    INDEXED_SELECTOR,       // 索引选择器 button[0]
    DESCENDANT_SELECTOR,    // 后代选择器 .box button
    MIXED_SELECTOR,         // 混合选择器
    UNKNOWN_SELECTOR
};

// 精确访问类型
enum class PreciseAccessType {
    FIRST_MATCH,           // 第一个匹配元素
    LAST_MATCH,            // 最后一个匹配元素
    INDEX_MATCH,           // 指定索引的元素 [0], [1]
    RANGE_MATCH,           // 范围匹配 [0-2]
    ATTRIBUTE_MATCH,       // 属性匹配 [attr=value]
    ALL_MATCHES            // 所有匹配元素
};

// 选择器解析结果
struct SelectorParseResult {
    bool success;                          // 解析是否成功
    SelectorType type;                     // 选择器类型
    std::string base_selector;             // 基础选择器
    std::vector<std::string> components;   // 选择器组件
    PreciseAccessType access_type;         // 精确访问类型
    std::string index_expression;          // 索引表达式
    std::unordered_map<std::string, std::string> attributes;  // 属性条件
    std::vector<std::string> pseudo_classes;   // 伪类
    std::vector<std::string> pseudo_elements;  // 伪元素
    std::string error_message;             // 错误信息
    
    SelectorParseResult() : success(false), type(SelectorType::UNKNOWN_SELECTOR), 
                           access_type(PreciseAccessType::FIRST_MATCH) {}
};

// DOM元素信息
struct DOMElementInfo {
    std::string tag_name;                  // 标签名
    std::string id;                        // ID
    std::vector<std::string> classes;      // 类名列表
    std::unordered_map<std::string, std::string> attributes;  // 属性
    std::vector<std::shared_ptr<DOMElementInfo>> children;    // 子元素
    std::weak_ptr<DOMElementInfo> parent;  // 父元素
    size_t index_in_parent;                // 在父元素中的索引
    
    DOMElementInfo() : index_in_parent(0) {}
    DOMElementInfo(const std::string& tag) : tag_name(tag), index_in_parent(0) {}
    
    bool hasClass(const std::string& class_name) const {
        return std::find(classes.begin(), classes.end(), class_name) != classes.end();
    }
    
    bool hasAttribute(const std::string& attr_name) const {
        return attributes.find(attr_name) != attributes.end();
    }
    
    std::string getAttribute(const std::string& attr_name, const std::string& default_val = "") const {
        auto it = attributes.find(attr_name);
        return (it != attributes.end()) ? it->second : default_val;
    }
};

// 选择器匹配结果
struct SelectorMatchResult {
    bool has_matches;                      // 是否有匹配
    std::vector<std::shared_ptr<DOMElementInfo>> elements;  // 匹配的元素
    std::string generated_selector;        // 生成的CSS选择器
    std::string generated_js_code;         // 生成的JavaScript代码
    std::vector<std::string> warnings;     // 警告信息
    
    SelectorMatchResult() : has_matches(false) {}
    
    size_t count() const { return elements.size(); }
    std::shared_ptr<DOMElementInfo> first() const { 
        return has_matches ? elements[0] : nullptr; 
    }
    std::shared_ptr<DOMElementInfo> last() const { 
        return has_matches ? elements.back() : nullptr; 
    }
};

// 增强选择器管理器
class EnhancedSelectorManager {
private:
    std::unordered_map<std::string, std::shared_ptr<DOMElementInfo>> dom_tree;
    std::unordered_map<std::string, std::vector<std::string>> auto_generated_classes;
    std::unordered_map<std::string, std::vector<std::string>> auto_generated_ids;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 配置选项
    bool enable_smart_matching;            // 启用智能匹配
    bool enable_performance_optimization;  // 启用性能优化
    bool strict_matching_mode;             // 严格匹配模式
    
    // 内部方法
    SelectorParseResult parseBasicSelector(const std::string& selector);
    SelectorParseResult parseEnhancedSelector(const std::string& selector);
    SelectorParseResult parseIndexedSelector(const std::string& selector);
    SelectorParseResult parseDescendantSelector(const std::string& selector);
    
    std::vector<std::shared_ptr<DOMElementInfo>> findElementsByTag(const std::string& tag);
    std::vector<std::shared_ptr<DOMElementInfo>> findElementsByClass(const std::string& class_name);
    std::vector<std::shared_ptr<DOMElementInfo>> findElementsById(const std::string& id);
    std::vector<std::shared_ptr<DOMElementInfo>> findElementsByUniversal(const std::string& name);
    
    std::vector<std::shared_ptr<DOMElementInfo>> applyIndexFilter(
        const std::vector<std::shared_ptr<DOMElementInfo>>& elements,
        const std::string& index_expr);
    
    std::vector<std::shared_ptr<DOMElementInfo>> applyAttributeFilter(
        const std::vector<std::shared_ptr<DOMElementInfo>>& elements,
        const std::unordered_map<std::string, std::string>& attributes);
    
    bool matchesElement(const std::shared_ptr<DOMElementInfo>& element, const std::string& selector);
    
public:
    EnhancedSelectorManager();
    ~EnhancedSelectorManager() = default;
    
    // DOM树管理
    void addElement(const std::shared_ptr<DOMElementInfo>& element);
    void removeElement(const std::string& element_id);
    void updateElement(const std::string& element_id, const std::shared_ptr<DOMElementInfo>& element);
    std::shared_ptr<DOMElementInfo> getElement(const std::string& element_id) const;
    void clearDOM();
    
    // 选择器解析
    SelectorParseResult parseSelector(const std::string& selector);
    bool isEnhancedSelector(const std::string& selector);
    bool isValidSelector(const std::string& selector);
    std::string normalizeSelector(const std::string& selector);
    
    // 选择器匹配
    SelectorMatchResult matchSelector(const std::string& selector);
    SelectorMatchResult matchEnhancedSelector(const std::string& enhanced_selector);
    std::vector<std::shared_ptr<DOMElementInfo>> findElements(const std::string& selector);
    std::shared_ptr<DOMElementInfo> findFirstElement(const std::string& selector);
    
    // 精确访问功能
    std::shared_ptr<DOMElementInfo> accessElementByIndex(const std::string& base_selector, int index);
    std::vector<std::shared_ptr<DOMElementInfo>> accessElementsByRange(
        const std::string& base_selector, int start, int end);
    std::shared_ptr<DOMElementInfo> accessElementByAttribute(
        const std::string& base_selector, const std::string& attr_name, const std::string& attr_value);
    
    // 复杂选择器支持
    SelectorMatchResult processDescendantSelector(const std::string& ancestor, const std::string& descendant);
    SelectorMatchResult processChildSelector(const std::string& parent, const std::string& child);
    SelectorMatchResult processSiblingSelector(const std::string& first, const std::string& second);
    SelectorMatchResult processAdjacentSelector(const std::string& first, const std::string& second);
    
    // 代码生成
    std::string generateCSS Selector(const SelectorParseResult& parse_result);
    std::string generateJavaScriptCode(const SelectorParseResult& parse_result);
    std::string generateQuerySelector(const std::string& enhanced_selector);
    std::string generateQuerySelectorAll(const std::string& enhanced_selector);
    
    // 自动化功能
    void enableAutoClassGeneration(bool enable = true);
    void enableAutoIdGeneration(bool enable = true);
    std::string generateAutoClass(const std::string& base_name);
    std::string generateAutoId(const std::string& base_name);
    void addAutoGeneratedClass(const std::string& element_id, const std::string& class_name);
    void addAutoGeneratedId(const std::string& element_id, const std::string& id_name);
    
    // 性能优化
    void enablePerformanceOptimization(bool enable = true);
    void cacheSelector(const std::string& selector, const SelectorMatchResult& result);
    SelectorMatchResult getCachedResult(const std::string& selector);
    void clearCache();
    
    // 验证和检查
    bool validateSelectorSyntax(const std::string& selector);
    std::vector<std::string> checkSelectorPerformance(const std::string& selector);
    std::vector<std::string> suggestOptimizations(const std::string& selector);
    
    // 智能匹配
    void enableSmartMatching(bool enable = true);
    std::vector<std::string> suggestSimilarSelectors(const std::string& selector);
    std::string findBestMatch(const std::string& intended_selector);
    double calculateSelectorSimilarity(const std::string& selector1, const std::string& selector2);
    
    // 调试和分析
    std::string analyzeSelectorComplexity(const std::string& selector);
    std::vector<std::string> getMatchingPath(const std::shared_ptr<DOMElementInfo>& element);
    std::string generateSelectorReport(const std::string& selector);
    void dumpDOMTree() const;
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors; }
    std::vector<std::string> getWarnings() const { return warnings; }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // 配置管理
    void setStrictMatchingMode(bool strict = true);
    bool isStrictMatchingMode() const { return strict_matching_mode; }
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key, const std::string& default_val = "") const;
    
private:
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    
    // 内部解析方法
    std::string extractSelectorContent(const std::string& enhanced_selector);
    std::vector<std::string> splitSelectorComponents(const std::string& selector);
    std::string parseIndexExpression(const std::string& expression);
    std::unordered_map<std::string, std::string> parseAttributeExpression(const std::string& expression);
    
    // 内部匹配方法
    bool isTagMatch(const std::shared_ptr<DOMElementInfo>& element, const std::string& tag);
    bool isClassMatch(const std::shared_ptr<DOMElementInfo>& element, const std::string& class_name);
    bool isIdMatch(const std::shared_ptr<DOMElementInfo>& element, const std::string& id);
    bool isAttributeMatch(const std::shared_ptr<DOMElementInfo>& element, 
                         const std::string& attr_name, const std::string& attr_value);
    
    // 缓存管理
    std::unordered_map<std::string, SelectorMatchResult> selector_cache;
    size_t cache_hit_count;
    size_t cache_miss_count;
    
    // 性能统计
    void updatePerformanceStats(const std::string& selector, bool cache_hit);
    
    // DOM树遍历
    void traverseDOM(const std::shared_ptr<DOMElementInfo>& root,
                    std::function<void(const std::shared_ptr<DOMElementInfo>&)> visitor);
    std::vector<std::shared_ptr<DOMElementInfo>> getAllElements();
    
    // 配置存储
    std::unordered_map<std::string, std::string> configuration;
};

// 选择器工具类
class SelectorUtils {
public:
    // 选择器类型检测
    static SelectorType detectSelectorType(const std::string& selector);
    static bool isEnhancedSelector(const std::string& selector);
    static bool hasIndexAccess(const std::string& selector);
    static bool hasAttributeFilter(const std::string& selector);
    static bool hasDescendantRelation(const std::string& selector);
    
    // 选择器转换
    static std::string convertToStandardCSS(const std::string& enhanced_selector);
    static std::string convertToQuerySelector(const std::string& enhanced_selector);
    static std::string optimizeSelector(const std::string& selector);
    
    // 选择器验证
    static bool isValidCSSSelector(const std::string& selector);
    static bool isValidEnhancedSelector(const std::string& selector);
    static std::vector<std::string> validateSelectorSyntax(const std::string& selector);
    
    // 选择器分析
    static std::vector<std::string> extractComponents(const std::string& selector);
    static std::string extractBaseSelector(const std::string& complex_selector);
    static std::string extractIndexPart(const std::string& selector);
    static std::unordered_map<std::string, std::string> extractAttributes(const std::string& selector);
    
    // 字符串处理
    static std::string trim(const std::string& str);
    static std::string removeExtraSpaces(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string escapeCSS(const std::string& str);
    
    // 性能分析
    static int calculateSelectorSpecificity(const std::string& selector);
    static bool isFastSelector(const std::string& selector);
    static std::vector<std::string> identifyPerformanceIssues(const std::string& selector);
};

// 预定义选择器模式
namespace SelectorPatterns {
    extern const std::string ENHANCED_SELECTOR_PATTERN;
    extern const std::string INDEX_ACCESS_PATTERN;
    extern const std::string ATTRIBUTE_FILTER_PATTERN;
    extern const std::string DESCENDANT_SELECTOR_PATTERN;
    extern const std::string PSEUDO_CLASS_PATTERN;
    extern const std::string PSEUDO_ELEMENT_PATTERN;
}

} // namespace Common
} // namespace CHTL