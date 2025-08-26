#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {
namespace Common {

// Use语法类型
enum class UseSyntaxType {
    HTML5_DECLARATION,      // use html5;
    CONFIGURATION_USE,      // use @Config Basic;
    FULL_CONFIGURATION_USE, // use [Configuration] @Config Basic;
    UNKNOWN_USE
};

// Use语法解析结果
struct UseSyntaxParseResult {
    bool success;                          // 解析是否成功
    UseSyntaxType type;                    // Use语法类型
    std::string target_name;               // 目标名称
    std::string full_target;               // 完整目标字符串
    bool has_full_prefix;                  // 是否有完整前缀
    std::string error_message;             // 错误信息
    
    UseSyntaxParseResult() : success(false), type(UseSyntaxType::UNKNOWN_USE), has_full_prefix(false) {}
};

// HTML5声明信息
struct HTML5Declaration {
    bool enabled;                          // 是否启用
    std::string doctype;                   // DOCTYPE声明
    std::string html_attributes;           // HTML元素属性
    std::string head_meta;                 // 头部元信息
    std::vector<std::string> required_elements;  // 必需元素
    
    HTML5Declaration() : enabled(false) {
        doctype = "<!DOCTYPE html>";
        html_attributes = R"(lang="en")";
        head_meta = R"(<meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0">)";
        required_elements = {"html", "head", "body"};
    }
};

// 配置组使用信息
struct ConfigurationUse {
    std::string config_name;               // 配置组名称
    std::string resolved_path;             // 解析后的路径
    bool is_valid;                         // 是否有效
    bool is_applied;                       // 是否已应用
    std::unordered_map<std::string, std::string> config_values;  // 配置值
    std::vector<std::string> dependencies; // 依赖的其他配置
    
    ConfigurationUse() : is_valid(false), is_applied(false) {}
};

// Use语法管理器
class UseSyntaxManager {
private:
    HTML5Declaration html5_declaration;
    std::unordered_map<std::string, ConfigurationUse> configurations_in_use;
    std::string current_file_path;
    bool use_statement_processed;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 内部方法
    UseSyntaxParseResult parseHTML5Use(const std::string& statement);
    UseSyntaxParseResult parseConfigurationUse(const std::string& statement);
    bool validateUseStatement(const std::string& statement);
    bool isValidConfigurationName(const std::string& name);
    
public:
    UseSyntaxManager();
    ~UseSyntaxManager() = default;
    
    // Use语句解析
    UseSyntaxParseResult parseUseStatement(const std::string& statement);
    bool processUseStatement(const std::string& statement);
    bool isValidUseStatement(const std::string& statement);
    
    // HTML5声明管理
    bool enableHTML5Declaration();
    bool disableHTML5Declaration();
    bool isHTML5Enabled() const;
    HTML5Declaration getHTML5Declaration() const;
    void setHTML5Attributes(const std::string& attributes);
    void setHTML5Meta(const std::string& meta);
    std::string generateHTML5Doctype() const;
    std::string generateHTML5HtmlTag() const;
    std::string generateHTML5Head() const;
    
    // 配置组使用管理
    bool useConfiguration(const std::string& config_name);
    bool useConfiguration(const std::string& config_name, const std::string& file_path);
    bool removeConfigurationUse(const std::string& config_name);
    bool hasConfigurationInUse(const std::string& config_name) const;
    const ConfigurationUse* getConfigurationUse(const std::string& config_name) const;
    std::vector<std::string> getAllConfigurationsInUse() const;
    
    // 配置值解析
    bool loadConfigurationValues(const std::string& config_name, const std::string& file_path);
    std::string getConfigurationValue(const std::string& config_name, const std::string& key, 
                                     const std::string& default_val = "") const;
    std::unordered_map<std::string, std::string> getAllConfigurationValues(const std::string& config_name) const;
    bool applyConfiguration(const std::string& config_name);
    
    // 文件管理
    void setCurrentFile(const std::string& file_path);
    std::string getCurrentFile() const;
    bool hasUseStatement() const;
    void resetForNewFile();
    
    // 验证和检查
    bool validateFileStructure() const;
    bool checkUseStatementPosition(const std::string& file_content, size_t statement_position) const;
    std::vector<std::string> validateUseStatements(const std::vector<std::string>& statements) const;
    bool hasConflictingUseStatements() const;
    
    // 代码生成
    std::string generateDocumentStructure() const;
    std::string generateHTMLHeader() const;
    std::string generateConfigurationScript() const;
    std::vector<std::string> generateRequiredElements() const;
    
    // 依赖管理
    void addConfigurationDependency(const std::string& config_name, const std::string& dependency);
    std::vector<std::string> getConfigurationDependencies(const std::string& config_name) const;
    std::vector<std::string> resolveConfigurationOrder() const;
    bool hasCircularConfigurationDependency() const;
    
    // 配置合并
    ConfigurationUse mergeConfigurations(const std::vector<std::string>& config_names) const;
    std::unordered_map<std::string, std::string> mergeConfigurationValues(
        const std::vector<std::string>& config_names) const;
    void resolveConfigurationConflicts();
    
    // 配置查找
    std::string findConfigurationFile(const std::string& config_name) const;
    std::vector<std::string> searchConfigurationPaths(const std::string& config_name) const;
    bool configurationExists(const std::string& config_name, const std::string& search_path = "") const;
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors; }
    std::vector<std::string> getWarnings() const { return warnings; }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // 调试和报告
    std::string generateUseReport() const;
    std::string generateConfigurationReport() const;
    void dumpUseState() const;
    
    // 配置文件处理
    bool loadConfigurationFile(const std::string& file_path);
    bool saveConfigurationFile(const std::string& config_name, const std::string& file_path) const;
    std::string serializeConfiguration(const std::string& config_name) const;
    bool deserializeConfiguration(const std::string& serialized_data);
    
private:
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    
    // 内部解析方法
    std::string extractConfigurationName(const std::string& statement);
    bool hasFullConfigurationPrefix(const std::string& statement);
    std::string normalizeConfigurationName(const std::string& name);
    
    // 内部验证方法
    bool isAtFileBeginning(const std::string& file_content, size_t position) const;
    bool hasOnlyCommentsAndWhitespaceBefore(const std::string& content, size_t position) const;
    
    // 配置解析方法
    std::unordered_map<std::string, std::string> parseConfigurationContent(const std::string& content);
    std::string resolveConfigurationPath(const std::string& config_name) const;
    
    // 循环依赖检测
    bool detectCircularDependency(const std::string& config_name, 
                                 std::unordered_set<std::string>& visited,
                                 std::unordered_set<std::string>& rec_stack) const;
    
    // 文件系统操作
    bool fileExists(const std::string& path) const;
    std::string readFile(const std::string& path) const;
    bool writeFile(const std::string& path, const std::string& content) const;
    
    // 搜索路径
    std::vector<std::string> getConfigurationSearchPaths() const;
};

// Use语法工具类
class UseSyntaxUtils {
public:
    // 语句检测
    static bool isUseStatement(const std::string& statement);
    static bool isHTML5Use(const std::string& statement);
    static bool isConfigurationUse(const std::string& statement);
    static UseSyntaxType detectUseSyntaxType(const std::string& statement);
    
    // 语句解析
    static std::string extractUseTarget(const std::string& statement);
    static std::string normalizeUseStatement(const std::string& statement);
    static bool hasValidUseSyntax(const std::string& statement);
    
    // 配置名称处理
    static std::string extractConfigurationName(const std::string& use_statement);
    static bool isValidConfigurationName(const std::string& name);
    static std::string normalizeConfigurationName(const std::string& name);
    
    // 验证
    static bool validateUseStatementSyntax(const std::string& statement);
    static std::vector<std::string> validateUseStatementPosition(const std::string& file_content);
    static bool isAtCorrectPosition(const std::string& file_content, const std::string& use_statement);
    
    // HTML5相关
    static std::string generateStandardHTML5Doctype();
    static std::string generateStandardHTML5Meta();
    static std::vector<std::string> getRequiredHTML5Elements();
    
    // 配置相关
    static std::vector<std::string> parseConfigurationKeys(const std::string& config_content);
    static std::unordered_map<std::string, std::string> parseConfigurationValues(const std::string& config_content);
    static bool isValidConfigurationContent(const std::string& content);
    
    // 字符串处理
    static std::string trim(const std::string& str);
    static std::string removeComments(const std::string& content);
    static bool isWhitespaceOrComment(const std::string& line);
    static std::vector<std::string> splitLines(const std::string& content);
    
    // 路径处理
    static std::string resolveConfigurationPath(const std::string& base_path, const std::string& config_name);
    static std::vector<std::string> getStandardConfigurationPaths();
    static bool isAbsolutePath(const std::string& path);
    static std::string makeAbsolutePath(const std::string& base_path, const std::string& relative_path);
};

// 预定义Use语法模式
namespace UseSyntaxPatterns {
    extern const std::string HTML5_USE_PATTERN;
    extern const std::string CONFIG_USE_PATTERN;
    extern const std::string FULL_CONFIG_USE_PATTERN;
    extern const std::string USE_STATEMENT_PATTERN;
}

// 预定义配置
namespace PredefinedConfigurations {
    extern const std::string STANDARD_CONFIG;
    extern const std::string MINIMAL_CONFIG;
    extern const std::string DEBUG_CONFIG;
    extern const std::string PRODUCTION_CONFIG;
}

} // namespace Common
} // namespace CHTL