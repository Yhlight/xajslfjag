#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

namespace CHTL {
namespace Common {

// 原始嵌入类型
enum class OriginType {
    HTML,           // @Html
    STYLE,          // @Style  
    JAVASCRIPT,     // @JavaScript
    CUSTOM          // 自定义类型（如@Vue等）
};

// 原始嵌入定义模式
enum class OriginDefinitionMode {
    BLOCK_DEFINITION,   // 块定义模式：[Origin] @Type name { content }
    INLINE_USAGE,       // 内联使用模式：[Origin] @Type name;
    IMPORT_REFERENCE    // 导入引用模式：[Import] [Origin] @Type from file
};

// 原始嵌入位置类型（任意位置使用）
enum class OriginUsageContext {
    ROOT_LEVEL,         // 根级别
    HTML_ELEMENT,       // HTML元素内
    TEMPLATE_BLOCK,     // 模板块内
    CUSTOM_BLOCK,       // 自定义块内
    STYLE_BLOCK,        // 样式块内（局部/全局）
    SCRIPT_BLOCK,       // 脚本块内（局部/全局）
    TEXT_NODE,          // 文本节点内
    NAMESPACE,          // 命名空间内
    CONFIGURATION,      // 配置块内
    ANY_CONTEXT         // 任意上下文（核心特性）
};

// 原始嵌入节点信息
struct OriginEmbedNode {
    OriginType type;                        // 类型（Html/Style/JavaScript/Custom）
    std::string type_name;                  // 类型名称（如"Vue"用于@Vue）
    std::string name;                       // 节点名称（可选，用于带名原始嵌入）
    std::string content;                    // 原始内容
    OriginDefinitionMode mode;              // 定义模式
    OriginUsageContext context;             // 使用上下文
    std::string file_path;                  // 源文件路径
    size_t line_number;                     // 行号
    size_t column_number;                   // 列号
    bool has_name;                          // 是否有名称
    bool is_exported;                       // 是否可导出
    bool is_imported;                       // 是否是导入的
    std::string import_source;              // 导入源文件（如果是导入的）
    
    OriginEmbedNode() : type(OriginType::HTML), mode(OriginDefinitionMode::BLOCK_DEFINITION),
                       context(OriginUsageContext::ROOT_LEVEL), line_number(0), column_number(0),
                       has_name(false), is_exported(false), is_imported(false) {}
    
    OriginEmbedNode(OriginType t, const std::string& type_n, const std::string& n = "")
        : type(t), type_name(type_n), name(n), mode(OriginDefinitionMode::BLOCK_DEFINITION),
          context(OriginUsageContext::ROOT_LEVEL), line_number(0), column_number(0),
          has_name(!n.empty()), is_exported(false), is_imported(false) {}
    
    std::string getFullTypeName() const {
        return "@" + type_name;
    }
    
    std::string getIdentifier() const {
        return has_name ? (getFullTypeName() + " " + name) : getFullTypeName();
    }
    
    bool isCustomType() const {
        return type == OriginType::CUSTOM;
    }
    
    bool isNamedEmbed() const {
        return has_name && !name.empty();
    }
    
    std::string getLocationInfo() const {
        return file_path + ":" + std::to_string(line_number) + ":" + std::to_string(column_number);
    }
};

// 原始嵌入解析结果
struct OriginParseResult {
    std::vector<OriginEmbedNode> nodes;     // 解析得到的节点
    std::vector<std::string> errors;       // 错误信息
    std::vector<std::string> warnings;     // 警告信息
    bool success;                           // 是否成功
    
    OriginParseResult() : success(false) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    size_t getNodeCount() const { return nodes.size(); }
};

// 原始嵌入验证结果
struct OriginValidationResult {
    bool is_valid;                              // 是否有效
    std::vector<std::string> violations;       // 违反规则
    std::vector<std::string> suggestions;      // 建议
    std::string corrected_syntax;              // 修正后的语法（如果可能）
    
    OriginValidationResult() : is_valid(true) {}
    
    void addViolation(const std::string& violation) {
        violations.push_back(violation);
        is_valid = false;
    }
    
    void addSuggestion(const std::string& suggestion) {
        suggestions.push_back(suggestion);
    }
    
    bool hasViolations() const { return !violations.empty(); }
    bool hasSuggestions() const { return !suggestions.empty(); }
};

// 原始嵌入管理器
class OriginEmbedManager {
public:
    OriginEmbedManager();
    ~OriginEmbedManager();
    
    // 核心解析功能
    
    // 解析源代码中的所有原始嵌入
    OriginParseResult parseAllOriginEmbeds(const std::string& source_code, 
                                          const std::string& file_path = "");
    
    // 解析单个原始嵌入语句
    std::optional<OriginEmbedNode> parseSingleOriginEmbed(const std::string& statement, 
                                                         OriginUsageContext context);
    
    // 验证原始嵌入语法
    OriginValidationResult validateOriginEmbed(const std::string& statement);
    
    // 任意位置使用规则
    
    // 检查原始嵌入是否可以在指定上下文中使用（总是返回true，这是核心特性）
    bool canUseInContext(OriginUsageContext context) const { return true; }
    
    // 检查原始嵌入在当前位置的有效性
    bool isValidAtPosition(const std::string& source, size_t position) const;
    
    // 获取位置的使用上下文
    OriginUsageContext detectUsageContext(const std::string& source, size_t position) const;
    
    // 类型管理
    
    // 注册自定义原始嵌入类型
    void registerCustomType(const std::string& type_name);
    
    // 检查类型是否存在
    bool isValidType(const std::string& type_name) const;
    
    // 获取所有支持的类型
    std::vector<std::string> getSupportedTypes() const;
    
    // 检查是否是标准类型
    bool isStandardType(const std::string& type_name) const;
    
    // 带名原始嵌入管理
    
    // 注册带名原始嵌入
    void registerNamedEmbed(const OriginEmbedNode& node);
    
    // 查找带名原始嵌入
    std::optional<OriginEmbedNode> findNamedEmbed(const std::string& type_name, 
                                                 const std::string& name) const;
    
    // 检查名称是否已存在
    bool namedEmbedExists(const std::string& type_name, const std::string& name) const;
    
    // 获取所有带名原始嵌入
    std::vector<OriginEmbedNode> getAllNamedEmbeds() const;
    
    // 内容处理
    
    // 提取原始嵌入的内容
    std::string extractContent(const std::string& statement) const;
    
    // 验证原始内容的有效性
    bool isValidContent(const std::string& content, OriginType type) const;
    
    // 清理原始内容（去除多余空格等）
    std::string cleanContent(const std::string& content) const;
    
    // 转义特殊字符（如果需要）
    std::string escapeContent(const std::string& content, OriginType type) const;
    
    // 导入和导出
    
    // 处理原始嵌入的导入
    bool processOriginImport(const std::string& import_statement);
    
    // 生成原始嵌入的导出
    std::string generateOriginExport(const OriginEmbedNode& node) const;
    
    // 检查原始嵌入是否可导出
    bool canExport(const OriginEmbedNode& node) const;
    
    // 合并导入的原始嵌入
    void mergeImportedEmbeds(const std::vector<OriginEmbedNode>& imported_nodes);
    
    // 生成和输出
    
    // 生成原始嵌入的最终输出
    std::string generateOutput(const OriginEmbedNode& node) const;
    
    // 生成HTML原始嵌入
    std::string generateHTMLEmbed(const OriginEmbedNode& node) const;
    
    // 生成CSS原始嵌入
    std::string generateCSSEmbed(const OriginEmbedNode& node) const;
    
    // 生成JavaScript原始嵌入
    std::string generateJavaScriptEmbed(const OriginEmbedNode& node) const;
    
    // 生成自定义类型原始嵌入
    std::string generateCustomEmbed(const OriginEmbedNode& node) const;
    
    // 语法检测和分析
    
    // 检测原始嵌入语句的模式
    OriginDefinitionMode detectDefinitionMode(const std::string& statement) const;
    
    // 分析原始嵌入的使用统计
    std::unordered_map<OriginType, size_t> analyzeUsageStatistics(const std::string& source) const;
    
    // 检查原始嵌入的语法完整性
    bool checkSyntaxCompleteness(const std::string& statement) const;
    
    // 查找所有原始嵌入位置
    std::vector<std::pair<size_t, std::string>> findAllOriginEmbeds(const std::string& source) const;
    
    // 配置和选项
    
    // 设置是否允许自定义类型
    void setAllowCustomTypes(bool allow) { allow_custom_types = allow; }
    
    // 设置是否严格验证内容
    void setStrictContentValidation(bool strict) { strict_content_validation = strict; }
    
    // 设置默认类型
    void setDefaultType(OriginType type) { default_type = type; }
    
    // 调试和诊断
    
    // 获取所有注册的原始嵌入
    std::vector<OriginEmbedNode> getAllRegisteredEmbeds() const;
    
    // 转储原始嵌入管理状态
    void dumpManagerState() const;
    
    // 生成使用报告
    std::string generateUsageReport(const std::string& source) const;
    
    // 清理
    void clearAll();
    void clearNamedEmbeds();
    void clearCustomTypes();

private:
    // 配置
    bool allow_custom_types;
    bool strict_content_validation;
    OriginType default_type;
    
    // 存储
    std::unordered_map<std::string, OriginEmbedNode> named_embeds;           // 带名原始嵌入
    std::unordered_set<std::string> custom_types;                          // 自定义类型
    std::unordered_map<std::string, OriginEmbedNode> imported_embeds;       // 导入的原始嵌入
    
    // 内部方法
    
    // 解析相关
    OriginType parseOriginType(const std::string& type_string) const;
    std::string parseTypeName(const std::string& type_string) const;
    std::string parseName(const std::string& statement) const;
    std::string parseContent(const std::string& statement) const;
    
    // 验证相关
    bool isValidTypeName(const std::string& type_name) const;
    bool isValidEmbedName(const std::string& name) const;
    bool isValidOriginSyntax(const std::string& statement) const;
    
    // 上下文分析
    OriginUsageContext analyzeContext(const std::string& source, size_t position) const;
    std::pair<size_t, size_t> findContextBoundaries(const std::string& source, size_t position) const;
    
    // 内容处理
    std::string trimContent(const std::string& content) const;
    bool containsInvalidCharacters(const std::string& content, OriginType type) const;
    std::string normalizeContent(const std::string& content) const;
    
    // 生成辅助
    std::string wrapContent(const std::string& content, OriginType type) const;
    std::string addTypeSpecificHeaders(const std::string& content, OriginType type) const;
    
    // 工具方法
    std::string getTypeName(OriginType type) const;
    std::string getContextName(OriginUsageContext context) const;
    std::string generateUniqueId() const;
    
    // 错误处理
    void logError(const std::string& error) const;
    void logWarning(const std::string& warning) const;
    
    // 计数器
    mutable size_t next_id_counter;
};

// 原始嵌入工具类
class OriginEmbedUtils {
public:
    // 快速检查是否是原始嵌入语句
    static bool isOriginEmbedStatement(const std::string& statement);
    
    // 快速提取类型名称
    static std::string extractTypeName(const std::string& statement);
    
    // 快速提取内容
    static std::string extractContent(const std::string& statement);
    
    // 生成标准原始嵌入语法
    static std::string generateSyntax(OriginType type, const std::string& name = "", 
                                     const std::string& content = "");
    
    // 检查是否是自定义类型语法
    static bool isCustomTypeSyntax(const std::string& statement);
    
    // 规范化原始嵌入语句
    static std::string normalizeSyntax(const std::string& statement);
    
    // 验证原始嵌入的完整性
    static bool isCompleteSyntax(const std::string& statement);
    
    // 获取建议的语法修正
    static std::string suggestCorrection(const std::string& statement);
    
    // 检查语法兼容性
    static bool isCompatibleSyntax(const std::string& statement);
    
    // 生成使用示例
    static std::vector<std::string> generateUsageExamples(OriginType type);
};

} // namespace Common
} // namespace CHTL