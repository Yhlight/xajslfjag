#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "CHTLNode.h"

namespace CHTL {

// 约束类型
enum class ConstraintType {
    ELEMENT,        // HTML元素约束
    TYPE,           // 类型约束
    GLOBAL          // 全局约束
};

// 约束信息
struct ConstraintInfo {
    ConstraintType type;
    std::string value;
    size_t line;
    size_t column;
    std::string sourceFile;
    
    ConstraintInfo(ConstraintType type, const std::string& value,
                  size_t line = 0, size_t column = 0, const std::string& sourceFile = "")
        : type(type), value(value), line(line), column(column), sourceFile(sourceFile) {}
};

// 约束组
struct ConstraintGroup {
    std::vector<ConstraintInfo> constraints;
    size_t line;
    size_t column;
    std::string sourceFile;
    
    ConstraintGroup(size_t line = 0, size_t column = 0, const std::string& sourceFile = "")
        : line(line), column(column), sourceFile(sourceFile) {}
};

// 约束解析器
class ConstraintParser {
public:
    ConstraintParser();
    ~ConstraintParser() = default;
    
    // 解析约束语句
    std::vector<ConstraintGroup> parse(const std::string& source, const std::string& sourceFile = "");
    
    // 解析单个约束语句
    ConstraintGroup parseSingleConstraintStatement(const std::string& line, size_t lineNumber,
                                                 size_t columnNumber = 0, const std::string& sourceFile = "");
    
    // 解析except关键字
    std::vector<ConstraintInfo> parseExceptClause(const std::string& text, size_t line,
                                                 size_t column, const std::string& sourceFile);
    
    // 验证约束
    bool validateConstraint(const ConstraintInfo& constraint);
    std::vector<std::string> getValidationErrors(const ConstraintInfo& constraint);
    
    // 获取约束类型
    ConstraintType getConstraintType(const std::string& text);
    
    // 检查是否为约束语句
    bool isConstraintStatement(const std::string& text);
    bool isExceptClause(const std::string& text);
    
    // 提取约束值
    std::string extractConstraintValue(const std::string& text, ConstraintType type);
    
    // 生成约束节点
    std::shared_ptr<ConstraintNode> createConstraintNode(const ConstraintGroup& group);
    
    // 获取解析错误
    const std::vector<std::string>& getErrors() const { return errors_; }
    bool hasErrors() const { return !errors_.empty(); }
    void clearErrors();
    
    // 获取解析统计
    struct ParseStatistics {
        size_t totalConstraints;
        size_t elementConstraints;
        size_t typeConstraints;
        size_t globalConstraints;
        size_t parsingErrors;
        
        ParseStatistics() : totalConstraints(0), elementConstraints(0), 
                           typeConstraints(0), globalConstraints(0), parsingErrors(0) {}
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
    ConstraintInfo parseElementConstraint(const std::string& text, size_t line,
                                        size_t column, const std::string& sourceFile);
    ConstraintInfo parseTypeConstraint(const std::string& text, size_t line,
                                     size_t column, const std::string& sourceFile);
    ConstraintInfo parseGlobalConstraint(const std::string& text, size_t line,
                                       size_t column, const std::string& sourceFile);
    
    // 验证函数
    bool validateElementConstraint(const ConstraintInfo& constraint);
    bool validateTypeConstraint(const ConstraintInfo& constraint);
    bool validateGlobalConstraint(const ConstraintInfo& constraint);
    
    // 错误处理
    void addError(const std::string& error, size_t line = 0, size_t column = 0);
    
    // 统计更新
    void updateStatistics(const std::string& type, size_t value = 1);
    
    // 文本处理
    std::string normalizeText(const std::string& text);
    bool startsWith(const std::string& text, const std::string& prefix);
    bool endsWith(const std::string& text, const std::string& suffix);
    std::string extractBetween(const std::string& text, const std::string& start, const std::string& end);
    
    // 约束特定处理
    std::vector<std::string> splitConstraintList(const std::string& text);
    bool isValidElementName(const std::string& name);
    bool isValidTypeName(const std::string& name);
    bool isValidGlobalConstraint(const std::string& constraint);
    
    // 关键字检测
    bool isKeyword(const std::string& text, const std::vector<std::string>& keywords);
    std::vector<std::string> getElementKeywords();
    std::vector<std::string> getTypeKeywords();
    std::vector<std::string> getGlobalConstraintKeywords();
};

} // namespace CHTL