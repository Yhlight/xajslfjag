#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

namespace CHTL {
namespace Common {

// 位置关键字类型
enum class PositionKeywordType {
    AFTER,              // after - 在指定元素后面插入
    BEFORE,             // before - 在指定元素前面插入
    REPLACE,            // replace - 替换指定元素
    AT_TOP,             // at top - 在容器顶部插入
    AT_BOTTOM,          // at bottom - 在容器底部插入
    UNKNOWN
};

// 位置操作类型
enum class PositionOperationType {
    INSERT,             // insert操作
    DELETE,             // delete操作
    MODIFY,             // 修改操作
    UNKNOWN_OPERATION
};

// 位置关键字上下文
enum class PositionKeywordContext {
    CUSTOM_ELEMENT_USAGE,   // 自定义元素使用时
    TEMPLATE_ELEMENT_USAGE, // 模板元素使用时
    STYLE_BLOCK,            // 样式块中
    SCRIPT_BLOCK,           // 脚本块中
    ROOT_LEVEL,             // 根级别
    UNKNOWN_CONTEXT
};

// 位置解析结果
struct PositionParseResult {
    PositionKeywordType position_type;          // 位置类型
    PositionOperationType operation_type;       // 操作类型
    std::string target_selector;               // 目标选择器
    std::string target_element;                // 目标元素名
    std::optional<int> target_index;           // 目标索引（如果有）
    std::vector<std::string> content_to_insert; // 要插入的内容
    bool is_valid;                             // 是否有效
    std::vector<std::string> errors;           // 错误信息
    std::vector<std::string> warnings;         // 警告信息
    
    PositionParseResult() : position_type(PositionKeywordType::UNKNOWN),
                           operation_type(PositionOperationType::UNKNOWN_OPERATION),
                           is_valid(false) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        is_valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// 插入位置信息
struct InsertionPosition {
    PositionKeywordType position;               // 插入位置
    std::string reference_element;              // 参考元素
    std::optional<int> reference_index;        // 参考索引
    int calculated_position;                   // 计算出的实际位置
    bool is_absolute;                          // 是否绝对位置
    
    InsertionPosition() : position(PositionKeywordType::UNKNOWN), 
                         calculated_position(-1), is_absolute(false) {}
    
    bool isValid() const { return calculated_position >= 0; }
};

// 位置关键字管理器
class PositionKeywordManager {
public:
    PositionKeywordManager();
    ~PositionKeywordManager();
    
    // 解析位置关键字语法
    PositionParseResult parsePositionKeyword(const std::string& statement);
    
    // 解析插入语句
    PositionParseResult parseInsertStatement(const std::string& statement);
    
    // 解析删除语句
    PositionParseResult parseDeleteStatement(const std::string& statement);
    
    // 验证位置关键字语法
    bool validatePositionSyntax(const std::string& statement, 
                               PositionKeywordContext context);
    
    // 计算插入位置
    InsertionPosition calculateInsertionPosition(
        const PositionParseResult& parse_result,
        const std::vector<std::string>& existing_elements);
    
    // 生成插入后的元素列表
    std::vector<std::string> generateUpdatedElementList(
        const std::vector<std::string>& original_elements,
        const PositionParseResult& parse_result);
    
    // 生成HTML插入代码
    std::string generateHTMLInsertion(const PositionParseResult& parse_result);
    
    // 生成CSS选择器用于定位
    std::string generatePositionSelector(const PositionParseResult& parse_result);
    
    // 生成JavaScript插入代码
    std::string generateJavaScriptInsertion(const PositionParseResult& parse_result);
    
    // 配置选项
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setAllowComplexSelectors(bool allow) { allow_complex_selectors = allow; }
    void setValidateTargetExists(bool validate) { validate_target_exists = validate; }

private:
    // 配置选项
    bool strict_mode;
    bool allow_complex_selectors;
    bool validate_target_exists;
    
    // 关键字映射
    std::unordered_map<std::string, PositionKeywordType> keyword_map;
    std::unordered_map<std::string, PositionOperationType> operation_map;
    
    // 解析方法
    PositionKeywordType parsePositionType(const std::string& keyword);
    PositionOperationType parseOperationType(const std::string& operation);
    std::string parseTargetSelector(const std::string& selector_part);
    std::optional<int> parseTargetIndex(const std::string& selector_part);
    
    // 验证方法
    bool isValidPositionKeyword(const std::string& keyword);
    bool isValidOperationKeyword(const std::string& operation);
    bool isValidTargetSelector(const std::string& selector);
    bool isValidInsertionContent(const std::vector<std::string>& content);
    
    // 位置计算
    int calculateAfterPosition(const std::string& target, int index, 
                              const std::vector<std::string>& elements);
    int calculateBeforePosition(const std::string& target, int index, 
                               const std::vector<std::string>& elements);
    int calculateReplacePosition(const std::string& target, int index, 
                                const std::vector<std::string>& elements);
    int calculateTopPosition();
    int calculateBottomPosition(const std::vector<std::string>& elements);
    
    // 工具方法
    std::string trimWhitespace(const std::string& str);
    std::vector<std::string> splitStatement(const std::string& statement);
    std::string extractContentBlock(const std::string& statement);
    
    // 索引处理
    bool hasIndexSuffix(const std::string& selector);
    std::pair<std::string, int> parseElementWithIndex(const std::string& selector);
    
    // 错误处理
    void reportError(PositionParseResult& result, const std::string& error);
    void reportWarning(PositionParseResult& result, const std::string& warning);
    
    // 初始化
    void initializeKeywordMaps();
};

// 位置关键字工具类
class PositionKeywordUtils {
public:
    // 检测位置关键字语句
    static bool isPositionKeywordStatement(const std::string& statement);
    
    // 检测插入语句
    static bool isInsertStatement(const std::string& statement);
    
    // 检测删除语句
    static bool isDeleteStatement(const std::string& statement);
    
    // 提取位置关键字
    static std::string extractPositionKeyword(const std::string& statement);
    
    // 提取操作关键字
    static std::string extractOperationKeyword(const std::string& statement);
    
    // 提取目标选择器
    static std::string extractTargetSelector(const std::string& statement);
    
    // 验证位置关键字组合
    static bool isValidKeywordCombination(PositionKeywordType position, 
                                         PositionOperationType operation);
    
    // 生成位置描述
    static std::string describePosition(PositionKeywordType position);
    static std::string describeOperation(PositionOperationType operation);
    
    // 位置计算工具
    static int calculateRelativePosition(int base_position, 
                                        PositionKeywordType position_type);
    
    // HTML生成工具
    static std::string generateInsertionComment(const PositionParseResult& result);
    static std::string generatePositionMarker(const PositionParseResult& result);
    
    // CSS生成工具
    static std::string generatePositionPseudoSelector(PositionKeywordType position, int index);
    static std::string generateInsertionCSS(const PositionParseResult& result);
    
    // JavaScript生成工具
    static std::string generateInsertAdjacentHTML(PositionKeywordType position);
    static std::string generatePositionBasedInsertion(const PositionParseResult& result);
    
    // 验证工具
    static std::vector<std::string> validatePositionStatement(const std::string& statement);
    static bool canApplyPosition(PositionKeywordType position, 
                                const std::string& target_element);
    
    // 调试工具
    static std::string analyzePositionStatement(const std::string& statement);
    static void dumpPositionParseResult(const PositionParseResult& result);
};

// 位置操作执行器
class PositionOperationExecutor {
public:
    PositionOperationExecutor();
    ~PositionOperationExecutor();
    
    // 执行插入操作
    std::vector<std::string> executeInsert(
        const std::vector<std::string>& original_elements,
        const PositionParseResult& insert_operation);
    
    // 执行删除操作
    std::vector<std::string> executeDelete(
        const std::vector<std::string>& original_elements,
        const PositionParseResult& delete_operation);
    
    // 执行替换操作
    std::vector<std::string> executeReplace(
        const std::vector<std::string>& original_elements,
        const PositionParseResult& replace_operation);
    
    // 批量执行位置操作
    std::vector<std::string> executeBatchOperations(
        const std::vector<std::string>& original_elements,
        const std::vector<PositionParseResult>& operations);
    
    // 验证操作可执行性
    bool canExecuteOperation(const std::vector<std::string>& elements,
                            const PositionParseResult& operation);
    
    // 预览操作结果
    std::vector<std::string> previewOperation(
        const std::vector<std::string>& original_elements,
        const PositionParseResult& operation);
    
    // 生成操作说明
    std::string generateOperationDescription(const PositionParseResult& operation);
    
    // 配置执行器
    void setValidateBeforeExecution(bool validate) { validate_before_execution = validate; }
    void setPreserveIndentation(bool preserve) { preserve_indentation = preserve; }

private:
    bool validate_before_execution;
    bool preserve_indentation;
    
    // 内部执行方法
    std::vector<std::string> insertElements(
        const std::vector<std::string>& elements,
        const std::vector<std::string>& new_elements,
        int position);
    
    std::vector<std::string> deleteElements(
        const std::vector<std::string>& elements,
        int start_position, int count = 1);
    
    std::vector<std::string> replaceElements(
        const std::vector<std::string>& elements,
        const std::vector<std::string>& new_elements,
        int position, int count = 1);
    
    // 位置查找
    int findElementPosition(const std::vector<std::string>& elements,
                           const std::string& target_element,
                           int target_index = 0);
    
    // 验证辅助
    bool validateElementExists(const std::vector<std::string>& elements,
                              const std::string& target_element,
                              int target_index);
    
    // 错误处理
    void handleExecutionError(const std::string& error);
};

} // namespace Common
} // namespace CHTL