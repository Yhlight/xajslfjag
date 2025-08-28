#ifndef CONSTRAINTPARSER_H
#define CONSTRAINTPARSER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "../CHTLContext/ConfigurationManager.h"

namespace CHTL {

/**
 * @brief 约束类型枚举
 */
enum class ConstraintType {
    EXACT,      // 精确约束：HTML元素、自定义对象、模板对象
    TYPE,       // 类型约束：@Html、[Custom]、[Template]
    GLOBAL      // 全局约束：在命名空间内使用
};

/**
 * @brief 约束目标类型枚举
 */
enum class ConstraintTargetType {
    HTML_ELEMENT,    // HTML元素
    CUSTOM_ELEMENT,  // 自定义元素
    TEMPLATE_VAR,    // 模板变量
    TEMPLATE_ELEMENT, // 模板元素
    TEMPLATE_STYLE,  // 模板样式
    ORIGIN_HTML,     // 原始HTML
    CUSTOM_BLOCK,    // 自定义块
    TEMPLATE_BLOCK   // 模板块
};

/**
 * @brief 约束目标结构
 */
struct ConstraintTarget {
    ConstraintTargetType type;
    std::string name;        // 元素名称或类型名称
    std::string fullPath;    // 完整路径（如 [Custom] @Element Box）
    int lineNumber;          // 行号
    int columnNumber;        // 列号
    
    ConstraintTarget() : type(ConstraintTargetType::HTML_ELEMENT), lineNumber(0), columnNumber(0) {}
};

/**
 * @brief 约束语句结构
 */
struct ConstraintStatement {
    ConstraintType type;
    std::vector<ConstraintTarget> targets;  // 约束目标列表
    std::string scope;                      // 作用域（元素名称或命名空间）
    int lineNumber;                         // 行号
    int columnNumber;                       // 列号
    
    ConstraintStatement() : type(ConstraintType::EXACT), lineNumber(0), columnNumber(0) {}
};

/**
 * @brief 约束解析器
 * 负责解析CHTL中的except约束语句
 */
class ConstraintParser {
public:
    /**
     * @brief 构造函数
     * @param configManager 配置管理器指针
     */
    ConstraintParser(std::shared_ptr<ConfigurationManager> configManager);
    
    /**
     * @brief 析构函数
     */
    ~ConstraintParser();
    
    /**
     * @brief 解析约束语句
     * @param source 源代码
     * @param scope 作用域（元素名称或命名空间）
     * @return 解析结果列表
     */
    std::vector<std::shared_ptr<ConstraintStatement>> parse(const std::string& source, const std::string& scope = "");
    
    /**
     * @brief 解析except语句
     * @param line except语句行
     * @param lineNumber 行号
     * @param scope 作用域
     * @return 解析结果
     */
    std::shared_ptr<ConstraintStatement> parseExceptStatement(const std::string& line, int lineNumber, const std::string& scope);
    
    /**
     * @brief 解析约束目标
     * @param targetText 目标文本
     * @param lineNumber 行号
     * @return 约束目标
     */
    ConstraintTarget parseConstraintTarget(const std::string& targetText, int lineNumber);
    
    /**
     * @brief 验证约束语句
     * @param statement 约束语句
     * @return 是否有效
     */
    bool validateConstraintStatement(const std::shared_ptr<ConstraintStatement>& statement);
    
    /**
     * @brief 应用约束语句
     * @param statement 约束语句
     * @return 是否成功应用
     */
    bool applyConstraintStatement(const std::shared_ptr<ConstraintStatement>& statement);
    
    /**
     * @brief 检查元素是否被约束
     * @param elementName 元素名称
     * @param scope 作用域
     * @return 是否被约束
     */
    bool isElementConstrained(const std::string& elementName, const std::string& scope) const;
    
    /**
     * @brief 检查类型是否被约束
     * @param typeName 类型名称
     * @param scope 作用域
     * @return 是否被约束
     */
    bool isTypeConstrained(const std::string& typeName, const std::string& scope) const;
    
    /**
     * @brief 获取错误信息
     * @return 错误信息列表
     */
    std::vector<std::string> getErrors() const;
    
    /**
     * @brief 清除错误信息
     */
    void clearErrors();

private:
    std::shared_ptr<ConfigurationManager> configManager_;
    std::vector<std::string> errors_;
    
    // 约束存储：作用域 -> 约束目标集合
    std::unordered_map<std::string, std::unordered_set<std::string>> elementConstraints_;
    std::unordered_map<std::string, std::unordered_set<std::string>> typeConstraints_;
    
    /**
     * @brief 添加错误信息
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * @brief 跳过空白字符
     * @param line 行内容
     * @param position 位置
     */
    void skipWhitespace(const std::string& line, size_t& position);
    
    /**
     * @brief 提取标识符
     * @param line 行内容
     * @param position 位置
     * @return 标识符
     */
    std::string extractIdentifier(const std::string& line, size_t& position);
    
    /**
     * @brief 检查是否匹配关键字
     * @param line 行内容
     * @param position 位置
     * @param keyword 关键字
     * @return 是否匹配
     */
    bool matchKeyword(const std::string& line, size_t& position, const std::string& keyword);
    
    /**
     * @brief 解析HTML元素名称
     * @param elementName 元素名称
     * @return 约束目标
     */
    ConstraintTarget parseHTMLElement(const std::string& elementName, int lineNumber);
    
    /**
     * @brief 解析自定义元素
     * @param customText 自定义文本
     * @param lineNumber 行号
     * @return 约束目标
     */
    ConstraintTarget parseCustomElement(const std::string& customText, int lineNumber);
    
    /**
     * @brief 解析模板对象
     * @param templateText 模板文本
     * @param lineNumber 行号
     * @return 约束目标
     */
    ConstraintTarget parseTemplateObject(const std::string& templateText, int lineNumber);
    
    /**
     * @brief 解析类型约束
     * @param typeText 类型文本
     * @param lineNumber 行号
     * @return 约束目标
     */
    ConstraintTarget parseTypeConstraint(const std::string& typeText, int lineNumber);
    
    /**
     * @brief 标准化作用域名称
     * @param scope 作用域
     * @return 标准化后的作用域
     */
    std::string normalizeScope(const std::string& scope) const;
};

} // namespace CHTL

#endif // CONSTRAINTPARSER_H