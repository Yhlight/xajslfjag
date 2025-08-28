#ifndef USESTATEMENTPARSER_H
#define USESTATEMENTPARSER_H

#include <string>
#include <memory>
#include <vector>
#include "../CHTLContext/ConfigurationManager.h"

namespace CHTL {

/**
 * @brief use语句类型枚举
 */
enum class UseStatementType {
    HTML5_TYPE,         // use html5;
    CONFIG_GROUP,       // use @Config Basic;
    FULL_CONFIG_GROUP   // use [Configuration] @Config Basic;
};

/**
 * @brief use语句结构
 */
struct UseStatement {
    UseStatementType type;
    std::string configGroupName;  // 配置组名称（仅对CONFIG_GROUP和FULL_CONFIG_GROUP有效）
    std::string html5Type;        // HTML类型（仅对HTML5_TYPE有效）
    int lineNumber;               // 行号
    int columnNumber;             // 列号
    
    UseStatement() : type(UseStatementType::HTML5_TYPE), lineNumber(0), columnNumber(0) {}
};

/**
 * @brief use语句解析器
 * 负责解析CHTL文件开头的use语句
 */
class UseStatementParser {
public:
    /**
     * @brief 构造函数
     * @param configManager 配置管理器指针
     */
    UseStatementParser(std::shared_ptr<ConfigurationManager> configManager);
    
    /**
     * @brief 析构函数
     */
    ~UseStatementParser();
    
    /**
     * @brief 解析use语句
     * @param source 源代码
     * @return 解析结果，如果解析失败返回nullptr
     */
    std::shared_ptr<UseStatement> parse(const std::string& source);
    
    /**
     * @brief 解析HTML5类型use语句
     * @param line use语句行
     * @param lineNumber 行号
     * @return 解析结果
     */
    std::shared_ptr<UseStatement> parseHTML5Type(const std::string& line, int lineNumber);
    
    /**
     * @brief 解析配置组use语句
     * @param line use语句行
     * @param lineNumber 行号
     * @return 解析结果
     */
    std::shared_ptr<UseStatement> parseConfigGroup(const std::string& line, int lineNumber);
    
    /**
     * @brief 解析全缀名配置组use语句
     * @param line use语句行
     * @param lineNumber 行号
     * @return 解析结果
     */
    std::shared_ptr<UseStatement> parseFullConfigGroup(const std::string& line, int lineNumber);
    
    /**
     * @brief 验证use语句
     * @param statement use语句
     * @return 是否有效
     */
    bool validateUseStatement(const std::shared_ptr<UseStatement>& statement);
    
    /**
     * @brief 应用use语句
     * @param statement use语句
     * @return 是否成功应用
     */
    bool applyUseStatement(const std::shared_ptr<UseStatement>& statement);
    
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
    
    /**
     * @brief 添加错误信息
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * @brief 检查use语句是否在文件开头
     * @param source 源代码
     * @param lineNumber 行号
     * @return 是否在文件开头
     */
    bool isAtFileBeginning(const std::string& source, int lineNumber);
    
    /**
     * @brief 提取行号
     * @param source 源代码
     * @param position 位置
     * @return 行号
     */
    int getLineNumber(const std::string& source, size_t position);
    
    /**
     * @brief 提取列号
     * @param source 源代码
     * @param position 位置
     * @return 列号
     */
    int getColumnNumber(const std::string& source, size_t position);
    
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
};

} // namespace CHTL

#endif // USESTATEMENTPARSER_H