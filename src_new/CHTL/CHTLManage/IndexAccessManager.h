#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

namespace CHTL {
namespace Common {

// 索引访问类型
enum class IndexAccessType {
    NUMERIC_INDEX,          // 数字索引 [0], [1], [2]
    NEGATIVE_INDEX,         // 负数索引 [-1], [-2] (从末尾开始)
    RANGE_INDEX,            // 范围索引 [1:3], [0:-1]
    NAMED_INDEX,            // 命名索引 [first], [last]
    CONDITIONAL_INDEX,      // 条件索引 [class="box"]
    UNKNOWN
};

// 索引访问上下文
enum class IndexAccessContext {
    CUSTOM_ELEMENT_USAGE,   // 使用自定义元素时
    TEMPLATE_ELEMENT_USAGE, // 使用模板元素时
    STYLE_SELECTOR,         // 在样式选择器中
    SCRIPT_ELEMENT_ACCESS,  // 在脚本中访问元素
    UNKNOWN_CONTEXT
};

// 索引解析结果
struct IndexParseResult {
    IndexAccessType type;               // 索引类型
    std::vector<int> numeric_indices;   // 数字索引值
    std::string range_start;            // 范围开始
    std::string range_end;              // 范围结束
    std::string named_index;            // 命名索引
    std::string condition_attribute;    // 条件属性
    std::string condition_value;        // 条件值
    bool is_valid;                      // 是否有效
    std::vector<std::string> errors;    // 错误信息
    
    IndexParseResult() : type(IndexAccessType::UNKNOWN), is_valid(false) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        is_valid = false;
    }
    
    bool hasErrors() const { return !errors.empty(); }
};

// 元素索引信息
struct ElementIndexInfo {
    std::string element_type;           // 元素类型 (div, span等)
    int index_in_type;                  // 在同类型中的索引
    int global_index;                   // 全局索引
    std::string element_id;             // 元素ID（如果有）
    std::string element_class;          // 元素类（如果有）
    std::unordered_map<std::string, std::string> attributes; // 元素属性
    
    ElementIndexInfo() : index_in_type(-1), global_index(-1) {}
    
    bool isValid() const { return global_index >= 0; }
};

// 索引访问管理器
class IndexAccessManager {
public:
    IndexAccessManager();
    ~IndexAccessManager();
    
    // 解析索引访问语法
    IndexParseResult parseIndexAccess(const std::string& index_expression);
    
    // 验证索引访问是否有效
    bool validateIndexAccess(const std::string& index_expression, 
                            IndexAccessContext context);
    
    // 解析自定义元素的索引访问
    IndexParseResult parseCustomElementIndex(const std::string& element_name, 
                                            const std::string& index_expression);
    
    // 解析模板元素的索引访问
    IndexParseResult parseTemplateElementIndex(const std::string& element_name, 
                                              const std::string& index_expression);
    
    // 获取元素列表中指定索引的元素
    std::optional<ElementIndexInfo> getElementAtIndex(
        const std::vector<ElementIndexInfo>& elements, 
        const IndexParseResult& index_result);
    
    // 获取元素列表中满足条件的元素
    std::vector<ElementIndexInfo> getElementsMatchingIndex(
        const std::vector<ElementIndexInfo>& elements, 
        const IndexParseResult& index_result);
    
    // 生成索引访问的CSS选择器
    std::string generateCSSSelector(const std::string& base_selector, 
                                   const IndexParseResult& index_result);
    
    // 生成索引访问的JavaScript代码
    std::string generateJavaScriptAccess(const std::string& base_selector, 
                                        const IndexParseResult& index_result);
    
    // 配置选项
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setAllowNegativeIndex(bool allow) { allow_negative_index = allow; }
    void setAllowRangeIndex(bool allow) { allow_range_index = allow; }
    void setAllowConditionalIndex(bool allow) { allow_conditional_index = allow; }

private:
    // 配置选项
    bool strict_mode;
    bool allow_negative_index;
    bool allow_range_index;
    bool allow_conditional_index;
    
    // 预定义的命名索引
    std::unordered_map<std::string, int> named_indices;
    
    // 解析方法
    IndexParseResult parseNumericIndex(const std::string& expression);
    IndexParseResult parseRangeIndex(const std::string& expression);
    IndexParseResult parseNamedIndex(const std::string& expression);
    IndexParseResult parseConditionalIndex(const std::string& expression);
    
    // 验证方法
    bool isValidNumericIndex(const std::string& expression);
    bool isValidRangeExpression(const std::string& expression);
    bool isValidNamedIndex(const std::string& expression);
    bool isValidConditionalExpression(const std::string& expression);
    
    // 工具方法
    std::string trimWhitespace(const std::string& str);
    bool isInteger(const std::string& str);
    int parseInteger(const std::string& str);
    
    // 范围处理
    std::pair<int, int> parseRange(const std::string& range_expression);
    std::vector<int> expandRange(int start, int end, int total_count);
    
    // 条件处理
    std::pair<std::string, std::string> parseCondition(const std::string& condition);
    bool matchesCondition(const ElementIndexInfo& element, 
                         const std::string& attribute, 
                         const std::string& value);
    
    // CSS/JS生成辅助
    std::string generateNthChildSelector(int index);
    std::string generateNthOfTypeSelector(const std::string& element_type, int index);
    std::string generateAttributeSelector(const std::string& attribute, 
                                         const std::string& value);
    
    // 错误处理
    void reportError(IndexParseResult& result, const std::string& error);
    
    // 初始化
    void initializeNamedIndices();
};

// 索引访问工具类
class IndexAccessUtils {
public:
    // 检测索引访问语法
    static bool isIndexAccessExpression(const std::string& expression);
    
    // 提取索引表达式
    static std::string extractIndexExpression(const std::string& full_expression);
    
    // 提取基础元素名
    static std::string extractBaseElementName(const std::string& full_expression);
    
    // 验证索引范围
    static bool isValidIndexRange(int index, int total_count);
    
    // 转换负数索引
    static int convertNegativeIndex(int negative_index, int total_count);
    
    // 生成索引范围
    static std::vector<int> generateIndexRange(int start, int end);
    
    // 检查元素是否匹配属性条件
    static bool elementMatchesCondition(const ElementIndexInfo& element, 
                                       const std::string& condition);
    
    // 解析属性条件
    static std::pair<std::string, std::string> parseAttributeCondition(const std::string& condition);
    
    // 生成CSS nth-child选择器
    static std::string generateNthChild(int index);
    static std::string generateNthChild(const std::string& formula);
    
    // 生成CSS nth-of-type选择器
    static std::string generateNthOfType(int index);
    static std::string generateNthOfType(const std::string& formula);
    
    // 错误检测
    static std::vector<std::string> validateIndexSyntax(const std::string& expression);
    
    // 性能优化
    static bool isSimpleNumericIndex(const std::string& expression);
    static int extractSimpleIndex(const std::string& expression);
    
    // 调试工具
    static std::string describeIndexAccess(const IndexParseResult& result);
    static void dumpElementList(const std::vector<ElementIndexInfo>& elements);
};

// 自定义元素索引管理器
class CustomElementIndexManager {
public:
    CustomElementIndexManager();
    ~CustomElementIndexManager();
    
    // 注册自定义元素定义
    void registerCustomElement(const std::string& element_name, 
                              const std::vector<ElementIndexInfo>& child_elements);
    
    // 获取自定义元素的子元素列表
    std::vector<ElementIndexInfo> getCustomElementChildren(const std::string& element_name);
    
    // 解析自定义元素的索引访问
    std::optional<ElementIndexInfo> resolveCustomElementIndex(
        const std::string& element_name, 
        const IndexParseResult& index_result);
    
    // 生成自定义元素索引访问的HTML
    std::string generateCustomElementHTML(const std::string& element_name, 
                                         const IndexParseResult& index_result);
    
    // 验证自定义元素索引访问
    bool validateCustomElementIndexAccess(const std::string& element_name, 
                                         const std::string& index_expression);
    
    // 获取可用的索引
    std::vector<int> getAvailableIndices(const std::string& element_name);
    
    // 获取元素类型统计
    std::unordered_map<std::string, int> getElementTypeCount(const std::string& custom_element_name);

private:
    // 自定义元素定义存储
    std::unordered_map<std::string, std::vector<ElementIndexInfo>> custom_elements;
    
    // 缓存优化
    std::unordered_map<std::string, std::unordered_map<std::string, int>> type_count_cache;
    
    // 辅助方法
    void updateTypeCountCache(const std::string& element_name);
    std::vector<ElementIndexInfo> filterElementsByType(
        const std::vector<ElementIndexInfo>& elements, 
        const std::string& type);
    
    // 错误处理
    void validateElementDefinition(const std::string& element_name, 
                                  const std::vector<ElementIndexInfo>& elements);
};

} // namespace Common
} // namespace CHTL