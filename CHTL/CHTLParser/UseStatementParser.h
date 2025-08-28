#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "CHTLNode.h"

namespace CHTL {

// use语句类型
enum class UseStatementType {
    HTML5,      // use html5;
    CONFIG      // use @Config Basic;
};

// use语句信息
struct UseStatementInfo {
    UseStatementType type;
    std::string value;      // 对于CONFIG类型，这里是配置组名称
    size_t line;
    size_t column;
    std::string sourceFile;
    
    UseStatementInfo(UseStatementType type, const std::string& value = "",
                    size_t line = 0, size_t column = 0, const std::string& sourceFile = "")
        : type(type), value(value), line(line), column(column), sourceFile(sourceFile) {}
};

// use语句解析器
class UseStatementParser {
public:
    UseStatementParser();
    ~UseStatementParser() = default;
    
    // 解析use语句
    std::vector<UseStatementInfo> parse(const std::string& source, const std::string& sourceFile = "");
    
    // 解析单个use语句
    UseStatementInfo parseSingleUseStatement(const std::string& line, size_t lineNumber, 
                                           size_t columnNumber = 0, const std::string& sourceFile = "");
    
    // 验证use语句
    bool validateUseStatement(const UseStatementInfo& useStatement);
    std::vector<std::string> getValidationErrors(const UseStatementInfo& useStatement);
    
    // 获取use语句类型
    UseStatementType getUseStatementType(const std::string& text);
    
    // 检查是否为use语句
    bool isUseStatement(const std::string& text);
    
    // 提取use语句值
    std::string extractUseStatementValue(const std::string& text, UseStatementType type);
    
    // 生成use语句节点
    std::shared_ptr<UseStatementNode> createUseStatementNode(const UseStatementInfo& info);
    
    // 获取解析错误
    const std::vector<std::string>& getErrors() const { return errors_; }
    bool hasErrors() const { return !errors_.empty(); }
    void clearErrors();
    
    // 获取解析统计
    struct ParseStatistics {
        size_t totalUseStatements;
        size_t html5Statements;
        size_t configStatements;
        size_t parsingErrors;
        
        ParseStatistics() : totalUseStatements(0), html5Statements(0), 
                           configStatements(0), parsingErrors(0) {}
    };
    
    const ParseStatistics& getStatistics() const { return statistics_; }
    void clearStatistics();
    
    // 调试信息
    std::string getDebugInfo() const;
    
    // 重置
    void reset();
    
private:
    std::vector<std::string> errors_;
    ParseStatistics statistics_;
    
    // 辅助函数
    std::vector<std::string> splitIntoLines(const std::string& source);
    std::string trimWhitespace(const std::string& text);
    bool isCommentLine(const std::string& text);
    bool isEmptyLine(const std::string& text);
    
    // 解析函数
    UseStatementInfo parseHtml5UseStatement(const std::string& text, size_t line, 
                                          size_t column, const std::string& sourceFile);
    UseStatementInfo parseConfigUseStatement(const std::string& text, size_t line, 
                                           size_t column, const std::string& sourceFile);
    
    // 验证函数
    bool validateHtml5UseStatement(const UseStatementInfo& useStatement);
    bool validateConfigUseStatement(const UseStatementInfo& useStatement);
    
    // 错误处理
    void addError(const std::string& error, size_t line = 0, size_t column = 0);
    
    // 统计更新
    void updateStatistics(const std::string& type, size_t value = 1);
    
    // 文本处理
    std::string normalizeText(const std::string& text);
    bool startsWith(const std::string& text, const std::string& prefix);
    bool endsWith(const std::string& text, const std::string& suffix);
    std::string extractBetween(const std::string& text, const std::string& start, const std::string& end);
};

} // namespace CHTL