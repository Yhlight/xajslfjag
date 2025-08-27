#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <memory>

namespace CHTL {
namespace Common {

// 键值对类型
enum class KeyValueType {
    STRING,         // 字符串值
    NUMBER,         // 数字值
    BOOLEAN,        // 布尔值
    FUNCTION,       // 函数值
    OBJECT,         // 对象值
    ARRAY,          // 数组值
    SELECTOR,       // 选择器值
    EVENT_HANDLER,  // 事件处理器
    UNQUOTED_LITERAL, // 无修饰字面量
    AUTO_DETECT     // 自动检测类型
};

// 键值对约束
enum class KeyConstraint {
    REQUIRED,       // 必需的
    OPTIONAL,       // 可选的
    CONDITIONAL,    // 条件性的（依赖其他键）
    DEPRECATED,     // 已弃用的
    EXPERIMENTAL    // 实验性的
};

// 键值对验证状态
enum class ValidationStatus {
    VALID,          // 有效
    MISSING,        // 缺失必需键
    INVALID_TYPE,   // 类型无效
    INVALID_VALUE,  // 值无效
    UNKNOWN_KEY,    // 未知键
    CONSTRAINT_VIOLATION, // 约束违反
    DEPRECATED_WARNING    // 弃用警告
};

// 键值对定义
struct KeyValueDefinition {
    std::string key;                        // 键名
    KeyValueType type;                      // 值类型
    KeyConstraint constraint;               // 约束类型
    std::string default_value;              // 默认值
    std::vector<std::string> allowed_values; // 允许的值（枚举）
    std::string description;                // 描述
    std::vector<std::string> dependencies;  // 依赖的其他键
    std::string validation_pattern;         // 验证模式（正则表达式）
    
    KeyValueDefinition() : type(KeyValueType::AUTO_DETECT), 
                          constraint(KeyConstraint::OPTIONAL) {}
    
    KeyValueDefinition(const std::string& k, KeyValueType t, KeyConstraint c = KeyConstraint::OPTIONAL)
        : key(k), type(t), constraint(c) {}
    
    bool isRequired() const { return constraint == KeyConstraint::REQUIRED; }
    bool isOptional() const { return constraint == KeyConstraint::OPTIONAL; }
    bool hasDefault() const { return !default_value.empty(); }
    bool hasAllowedValues() const { return !allowed_values.empty(); }
    bool hasDependencies() const { return !dependencies.empty(); }
};

// 键值对实例
struct KeyValuePair {
    std::string key;                // 键名
    std::string value;              // 值
    KeyValueType detected_type;     // 检测到的类型
    ValidationStatus status;        // 验证状态
    size_t position;                // 在源码中的位置
    bool is_provided;               // 是否由用户提供
    std::string error_message;      // 错误消息
    
    KeyValuePair() : detected_type(KeyValueType::AUTO_DETECT), 
                    status(ValidationStatus::VALID), position(0), is_provided(false) {}
    
    KeyValuePair(const std::string& k, const std::string& v)
        : key(k), value(v), detected_type(KeyValueType::AUTO_DETECT), 
          status(ValidationStatus::VALID), position(0), is_provided(true) {}
    
    bool isValid() const { return status == ValidationStatus::VALID; }
    bool hasError() const { return !error_message.empty(); }
    std::string toString() const { return key + ": " + value; }
};

// 函数签名定义（支持键值对的CHTL JS函数）
struct FunctionSignature {
    std::string function_name;                          // 函数名
    std::vector<KeyValueDefinition> key_definitions;    // 键定义
    bool allows_unordered;                              // 是否允许无序
    bool allows_optional;                               // 是否允许可选
    bool allows_extra_keys;                             // 是否允许额外的键
    std::string usage_example;                          // 使用示例
    std::string description;                            // 描述
    
    FunctionSignature() : allows_unordered(true), allows_optional(true), allows_extra_keys(false) {}
    
    FunctionSignature(const std::string& name) 
        : function_name(name), allows_unordered(true), allows_optional(true), allows_extra_keys(false) {}
    
    void addKey(const KeyValueDefinition& key_def) {
        key_definitions.push_back(key_def);
    }
    
    std::optional<KeyValueDefinition> findKeyDefinition(const std::string& key) const {
        for (const auto& def : key_definitions) {
            if (def.key == key) {
                return def;
            }
        }
        return std::nullopt;
    }
    
    std::vector<KeyValueDefinition> getRequiredKeys() const {
        std::vector<KeyValueDefinition> required;
        for (const auto& def : key_definitions) {
            if (def.isRequired()) {
                required.push_back(def);
            }
        }
        return required;
    }
    
    size_t getKeyCount() const { return key_definitions.size(); }
    bool hasKey(const std::string& key) const { return findKeyDefinition(key).has_value(); }
};

// 解析结果
struct ParseResult {
    std::vector<KeyValuePair> pairs;            // 解析得到的键值对
    std::vector<std::string> errors;           // 错误信息
    std::vector<std::string> warnings;         // 警告信息
    bool success;                               // 是否成功
    std::string normalized_syntax;              // 规范化后的语法
    
    ParseResult() : success(false) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    size_t getPairCount() const { return pairs.size(); }
    
    std::optional<KeyValuePair> findPair(const std::string& key) const {
        for (const auto& pair : pairs) {
            if (pair.key == key) {
                return pair;
            }
        }
        return std::nullopt;
    }
};

// 验证结果
struct ValidationResult {
    std::vector<KeyValuePair> validated_pairs;     // 验证后的键值对（包含默认值）
    std::vector<std::string> violations;           // 约束违反
    std::vector<std::string> suggestions;          // 建议
    bool is_valid;                                  // 是否有效
    std::string corrected_syntax;                  // 修正后的语法
    
    ValidationResult() : is_valid(true) {}
    
    void addViolation(const std::string& violation) {
        violations.push_back(violation);
        is_valid = false;
    }
    
    void addSuggestion(const std::string& suggestion) {
        suggestions.push_back(suggestion);
    }
    
    bool hasViolations() const { return !violations.empty(); }
    bool hasSuggestions() const { return !suggestions.empty(); }
    size_t getValidPairCount() const { return validated_pairs.size(); }
};

// CHTL JS键值对管理器
class CHTLJSKeyValueManager {
public:
    CHTLJSKeyValueManager();
    ~CHTLJSKeyValueManager();
    
    // 初始化预定义函数签名
    void initializeStandardSignatures();
    void initializeListenSignature();
    void initializeDelegateSignature();
    void initializeAnimateSignature();
    void initializeCustomSignatures();
    
    // 函数签名管理
    
    // 注册函数签名
    void registerFunctionSignature(const FunctionSignature& signature);
    
    // 获取函数签名
    std::optional<FunctionSignature> getFunctionSignature(const std::string& function_name) const;
    
    // 检查函数是否支持键值对
    bool supportsFunctionKeyValue(const std::string& function_name) const;
    
    // 获取所有支持的函数
    std::vector<std::string> getSupportedFunctions() const;
    
    // 键值对解析
    
    // 解析键值对语法
    ParseResult parseKeyValuePairs(const std::string& syntax);
    
    // 解析单个键值对
    std::optional<KeyValuePair> parseSinglePair(const std::string& pair_syntax);
    
    // 解析函数调用中的键值对
    ParseResult parseFunctionKeyValues(const std::string& function_name, 
                                      const std::string& parameters);
    
    // 验证和约束检查
    
    // 验证键值对是否符合函数签名
    ValidationResult validateKeyValues(const std::string& function_name, 
                                      const std::vector<KeyValuePair>& pairs);
    
    // 检查键值对约束
    bool checkKeyConstraints(const FunctionSignature& signature, 
                            const std::vector<KeyValuePair>& pairs);
    
    // 验证键值对类型
    bool validateKeyValueType(const KeyValuePair& pair, KeyValueType expected_type);
    
    // 检查必需键是否存在
    std::vector<std::string> findMissingRequiredKeys(const FunctionSignature& signature,
                                                     const std::vector<KeyValuePair>& pairs);
    
    // 类型检测和转换
    
    // 自动检测值类型
    KeyValueType detectValueType(const std::string& value) const;
    
    // 检查值是否符合类型
    bool isValidValueForType(const std::string& value, KeyValueType type) const;
    
    // 转换值类型
    std::string convertValueType(const std::string& value, KeyValueType from, KeyValueType to) const;
    
    // 规范化值
    std::string normalizeValue(const std::string& value, KeyValueType type) const;
    
    // 无序与可选功能
    
    // 支持无序键值对（任意顺序）
    std::vector<KeyValuePair> reorderKeyValues(const std::vector<KeyValuePair>& pairs,
                                               const FunctionSignature& signature);
    
    // 添加缺失的可选键的默认值
    std::vector<KeyValuePair> addDefaultValues(const std::vector<KeyValuePair>& pairs,
                                               const FunctionSignature& signature);
    
    // 检查键值对的完整性
    bool isComplete(const std::vector<KeyValuePair>& pairs, 
                   const FunctionSignature& signature);
    
    // 生成最小必需的键值对集合
    std::vector<KeyValuePair> generateMinimalSet(const FunctionSignature& signature);
    
    // 语法生成和格式化
    
    // 生成规范化的键值对语法
    std::string generateNormalizedSyntax(const std::vector<KeyValuePair>& pairs);
    
    // 生成函数调用语法
    std::string generateFunctionCall(const std::string& function_name,
                                    const std::vector<KeyValuePair>& pairs);
    
    // 格式化键值对（美化输出）
    std::string formatKeyValues(const std::vector<KeyValuePair>& pairs, 
                               bool multi_line = false);
    
    // 生成使用示例
    std::vector<std::string> generateUsageExamples(const std::string& function_name);
    
    // 特殊功能
    
    // 处理条件性键（依赖其他键的存在）
    std::vector<std::string> resolveConditionalKeys(const FunctionSignature& signature,
                                                    const std::vector<KeyValuePair>& pairs);
    
    // 处理弃用键的警告
    std::vector<std::string> checkDeprecatedKeys(const std::vector<KeyValuePair>& pairs);
    
    // 建议替代键
    std::vector<std::string> suggestAlternativeKeys(const std::string& unknown_key,
                                                   const FunctionSignature& signature);
    
    // 配置和选项
    
    // 设置是否允许额外键
    void setAllowExtraKeys(bool allow) { allow_extra_keys = allow; }
    
    // 设置是否严格类型检查
    void setStrictTypeChecking(bool strict) { strict_type_checking = strict; }
    
    // 设置默认值处理策略
    void setDefaultValueStrategy(bool auto_add) { auto_add_defaults = auto_add; }
    
    // 统计和分析
    
    // 分析键值对使用统计
    std::unordered_map<std::string, size_t> analyzeKeyUsage(const std::string& source_code);
    
    // 检查键值对的复杂度
    size_t calculateComplexity(const std::vector<KeyValuePair>& pairs);
    
    // 获取函数签名覆盖率
    double getSignatureCoverage(const std::string& function_name,
                               const std::vector<KeyValuePair>& pairs);
    
    // 调试和诊断
    void dumpFunctionSignatures() const;
    void dumpKeyValuePairs(const std::vector<KeyValuePair>& pairs) const;
    std::string generateDiagnosticReport(const std::string& function_name,
                                        const std::vector<KeyValuePair>& pairs) const;
    
    // 清理
    void clearAll();
    void clearFunctionSignature(const std::string& function_name);

private:
    // 存储
    std::unordered_map<std::string, FunctionSignature> function_signatures;
    
    // 配置
    bool allow_extra_keys;
    bool strict_type_checking;
    bool auto_add_defaults;
    
    // 缓存
    std::unordered_map<std::string, KeyValueType> type_cache;
    std::unordered_map<std::string, std::string> normalized_value_cache;
    
    // 内部方法
    
    // 解析相关
    std::vector<std::string> splitKeyValuePairs(const std::string& syntax) const;
    std::pair<std::string, std::string> extractKeyValue(const std::string& pair) const;
    std::string cleanKeyValueSyntax(const std::string& syntax) const;
    
    // 类型检测
    bool isStringLiteral(const std::string& value) const;
    bool isNumberLiteral(const std::string& value) const;
    bool isBooleanLiteral(const std::string& value) const;
    bool isFunctionLiteral(const std::string& value) const;
    bool isSelectorLiteral(const std::string& value) const;
    bool isUnquotedLiteral(const std::string& value) const;
    
    // 验证辅助
    bool matchesPattern(const std::string& value, const std::string& pattern) const;
    bool isInAllowedValues(const std::string& value, const std::vector<std::string>& allowed) const;
    bool satisfiesDependencies(const KeyValueDefinition& key_def, 
                              const std::vector<KeyValuePair>& pairs) const;
    
    // 格式化辅助
    std::string quoteIfNeeded(const std::string& value, KeyValueType type) const;
    std::string addTypeHint(const std::string& value, KeyValueType type) const;
    std::string generateIndentation(size_t level) const;
    
    // 工具方法
    std::string getTypeName(KeyValueType type) const;
    std::string getConstraintName(KeyConstraint constraint) const;
    std::string getStatusName(ValidationStatus status) const;
    
    // 错误处理
    void logError(const std::string& error) const;
    void logWarning(const std::string& warning) const;
};

// CHTL JS键值对工具类
class CHTLJSKeyValueUtils {
public:
    // 快速解析键值对
    static std::vector<KeyValuePair> quickParse(const std::string& syntax);
    
    // 快速验证键值对
    static bool quickValidate(const std::string& function_name, const std::string& syntax);
    
    // 生成标准键值对语法
    static std::string generateStandardSyntax(const std::vector<KeyValuePair>& pairs);
    
    // 检查是否是有效的键名
    static bool isValidKeyName(const std::string& key);
    
    // 检查是否是有效的值
    static bool isValidValue(const std::string& value);
    
    // 规范化键名
    static std::string normalizeKeyName(const std::string& key);
    
    // 提取所有键名
    static std::vector<std::string> extractAllKeys(const std::string& syntax);
    
    // 计算键值对数量
    static size_t countKeyValuePairs(const std::string& syntax);
    
    // 检查语法完整性
    static bool isCompleteSyntax(const std::string& syntax);
    
    // 建议语法修正
    static std::string suggestCorrection(const std::string& syntax);
};

} // namespace Common
} // namespace CHTL