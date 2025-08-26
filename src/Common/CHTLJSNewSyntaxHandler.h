#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <optional>

namespace CHTL {
namespace Common {

// 新语法函数类型
enum class NewSyntaxFunctionType {
    LISTEN,              // listen { ... }
    DELEGATE,            // delegate { ... }
    ANIMATE,             // animate { ... }
    VIR_INEVERAWAY,      // vir obj = iNeverAway { ... } (珂朵莉模块)
    VIR_LISTEN,          // vir obj = listen { ... }
    UNKNOWN
};

// 新语法上下文
enum class NewSyntaxContext {
    SCRIPT_BLOCK,        // 在script块中
    NESTED_FUNCTION,     // 在嵌套函数中
    VIR_DECLARATION,     // 在vir声明中
    KEYFRAME_BLOCK,      // 在when数组的关键帧中
    CSS_BLOCK,           // 在begin/end CSS块中
    UNKNOWN_CONTEXT
};

// 语法块类型
enum class SyntaxBlockType {
    FUNCTION_PARAMETER_BLOCK,  // 函数参数块 { key: value, ... }
    CSS_STYLE_BLOCK,          // CSS样式块 { property: value; ... }
    KEYFRAME_ARRAY,           // 关键帧数组 [{ at: 0.4; ... }, ...]
    EVENT_HANDLER_BLOCK,      // 事件处理器块
    UNKNOWN_BLOCK
};

// 新语法解析结果
struct NewSyntaxParseResult {
    NewSyntaxFunctionType function_type;        // 函数类型
    std::string function_name;                  // 函数名
    std::unordered_map<std::string, std::string> parameters; // 参数键值对
    std::vector<std::string> nested_blocks;     // 嵌套块内容
    bool is_valid;                             // 是否有效
    std::vector<std::string> errors;           // 错误信息
    std::vector<std::string> warnings;         // 警告信息
    
    NewSyntaxParseResult() : function_type(NewSyntaxFunctionType::UNKNOWN), is_valid(false) {}
    
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

// 大括号块信息
struct BraceBlockInfo {
    SyntaxBlockType block_type;                 // 块类型
    std::string content;                        // 块内容
    size_t start_position;                      // 开始位置
    size_t end_position;                        // 结束位置
    int nesting_level;                         // 嵌套层级
    bool is_css_block;                         // 是否是CSS块
    std::unordered_map<std::string, std::string> parsed_content; // 解析后的内容
    
    BraceBlockInfo() : block_type(SyntaxBlockType::UNKNOWN_BLOCK), 
                      start_position(0), end_position(0), 
                      nesting_level(0), is_css_block(false) {}
    
    bool isValid() const { return start_position < end_position; }
};

// CHTL JS新语法处理器
class CHTLJSNewSyntaxHandler {
public:
    CHTLJSNewSyntaxHandler();
    ~CHTLJSNewSyntaxHandler();
    
    // 检测新语法函数
    bool isNewSyntaxFunction(const std::string& code, size_t position);
    
    // 解析新语法函数
    NewSyntaxParseResult parseNewSyntaxFunction(const std::string& code, size_t start_position);
    
    // 检测函数类型
    NewSyntaxFunctionType detectFunctionType(const std::string& function_name);
    
    // 解析大括号块
    BraceBlockInfo parseBraceBlock(const std::string& code, size_t start_position);
    
    // 验证新语法的正确性
    bool validateNewSyntax(const std::string& code, NewSyntaxContext context);
    
    // 转换为旧语法（向后兼容）
    std::string convertToOldSyntax(const NewSyntaxParseResult& parse_result);
    
    // 生成JavaScript代码
    std::string generateJavaScript(const NewSyntaxParseResult& parse_result);
    
    // 配置处理器
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setAllowNestedFunctions(bool allow) { allow_nested_functions = allow; }
    void setValidateCSSSyntax(bool validate) { validate_css_syntax = validate; }

private:
    // 配置选项
    bool strict_mode;
    bool allow_nested_functions;
    bool validate_css_syntax;
    
    // 函数名映射
    std::unordered_map<std::string, NewSyntaxFunctionType> function_map;
    
    // 支持的CSS属性
    std::unordered_set<std::string> css_properties;
    
    // 支持的事件类型
    std::unordered_set<std::string> event_types;
    
    // 解析方法
    
    // 解析listen函数
    NewSyntaxParseResult parseListenFunction(const std::string& code, size_t start_pos);
    
    // 解析delegate函数
    NewSyntaxParseResult parseDelegateFunction(const std::string& code, size_t start_pos);
    
    // 解析animate函数
    NewSyntaxParseResult parseAnimateFunction(const std::string& code, size_t start_pos);
    
    // 块内容解析
    std::unordered_map<std::string, std::string> parseParameterBlock(const std::string& block_content);
    std::unordered_map<std::string, std::string> parseCSSBlock(const std::string& block_content);
    std::vector<std::unordered_map<std::string, std::string>> parseKeyframeArray(const std::string& array_content);
    
    // 大括号匹配
    size_t findMatchingBrace(const std::string& code, size_t start_pos);
    std::vector<std::pair<size_t, size_t>> findNestedBlocks(const std::string& code, size_t start_pos, size_t end_pos);
    
    // 语法验证
    bool isValidParameterName(const std::string& name, NewSyntaxFunctionType function_type);
    bool isValidParameterValue(const std::string& value, const std::string& parameter_name);
    bool isValidCSSProperty(const std::string& property);
    bool isValidCSSValue(const std::string& value);
    bool isValidEventType(const std::string& event);
    
    // 上下文检测
    NewSyntaxContext detectContext(const std::string& surrounding_code, size_t position);
    bool isInCSSContext(const std::string& code, size_t position);
    bool isInParameterContext(const std::string& code, size_t position);
    
    // 工具方法
    std::string trimWhitespace(const std::string& str);
    std::string extractFunctionName(const std::string& code, size_t position);
    std::vector<std::string> splitParameters(const std::string& parameter_string);
    std::pair<std::string, std::string> parseKeyValuePair(const std::string& pair_string);
    
    // 代码生成
    std::string generateListenJS(const NewSyntaxParseResult& result);
    std::string generateDelegateJS(const NewSyntaxParseResult& result);
    std::string generateAnimateJS(const NewSyntaxParseResult& result);
    
    // 错误处理
    void reportError(NewSyntaxParseResult& result, const std::string& error);
    void reportWarning(NewSyntaxParseResult& result, const std::string& warning);
    
    // 初始化
    void initializeFunctionMap();
    void initializeCSSProperties();
    void initializeEventTypes();
};

// 新语法工具类
class NewSyntaxUtils {
public:
    // 检测语法模式
    static bool isListenPattern(const std::string& code, size_t position);
    static bool isDelegatePattern(const std::string& code, size_t position);
    static bool isAnimatePattern(const std::string& code, size_t position);
    
    // 提取语法组件
    static std::string extractSelector(const std::string& code);
    static std::string extractFunctionName(const std::string& code);
    static std::string extractParameterBlock(const std::string& code);
    
    // 验证语法组件
    static bool isValidSelectorChain(const std::string& chain);
    static bool isValidFunctionBlock(const std::string& block);
    static bool isBalancedBraces(const std::string& code);
    
    // 转换工具
    static std::string newToOldSyntax(const std::string& new_syntax);
    static std::string oldToNewSyntax(const std::string& old_syntax);
    
    // 格式化工具
    static std::string formatNewSyntax(const std::string& code, int indent_level = 0);
    static std::string minifyNewSyntax(const std::string& code);
    
    // 调试工具
    static std::string analyzeNewSyntax(const std::string& code);
    static void dumpParseResult(const NewSyntaxParseResult& result);
};

// 语法迁移助手
class SyntaxMigrationHelper {
public:
    SyntaxMigrationHelper();
    ~SyntaxMigrationHelper();
    
    // 批量转换文件
    bool migrateFile(const std::string& file_path, bool backup_original = true);
    
    // 转换代码片段
    std::string migrateCodeSnippet(const std::string& code);
    
    // 检测需要迁移的代码
    std::vector<std::string> detectOldSyntax(const std::string& code);
    
    // 生成迁移报告
    std::string generateMigrationReport(const std::string& old_code, const std::string& new_code);
    
    // 验证迁移结果
    bool validateMigration(const std::string& old_code, const std::string& new_code);
    
    // 配置迁移选项
    void setPreserveFunctionality(bool preserve) { preserve_functionality = preserve; }
    void setGenerateComments(bool generate) { generate_comments = generate; }

private:
    bool preserve_functionality;
    bool generate_comments;
    
    // 迁移模式
    std::unordered_map<std::string, std::string> migration_patterns;
    
    // 初始化迁移模式
    void initializeMigrationPatterns();
    
    // 迁移辅助方法
    std::string migrateListenSyntax(const std::string& code);
    std::string migrateDelegateSyntax(const std::string& code);
    std::string migrateAnimateSyntax(const std::string& code);
};

} // namespace Common
} // namespace CHTL