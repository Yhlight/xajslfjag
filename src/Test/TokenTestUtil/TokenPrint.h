#pragma once
#include "../../CHTL/CHTLLexer/Token.h"
#include "../../CHTL/CHTLLexer/Lexer.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

namespace CHTL {
namespace Test {

/**
 * Token打印工具类
 * 提供格式化的Token输出功能
 */
class TokenPrint {
public:
    /**
     * 打印单个Token
     */
    static void printToken(const Token& token, bool detailed = false);
    
    /**
     * 打印Token序列
     */
    static void printTokenSequence(const TokenSequence& tokens, bool detailed = false);
    
    /**
     * 打印Token序列（带行号）
     */
    static void printTokenSequenceWithLineNumbers(const TokenSequence& tokens);
    
    /**
     * 打印Token统计信息
     */
    static void printTokenStatistics(const TokenSequence& tokens);
    
    /**
     * 按类型分组打印Token
     */
    static void printTokensByType(const TokenSequence& tokens);
    
    /**
     * 打印Token的十六进制表示
     */
    static void printTokenHex(const Token& token);
    
    /**
     * 打印Token序列到文件
     */
    static bool printTokenSequenceToFile(const TokenSequence& tokens, const std::string& filename);
    
    /**
     * 生成Token序列的HTML报告
     */
    static std::string generateTokenHtmlReport(const TokenSequence& tokens);
    
    /**
     * 生成Token序列的CSV报告
     */
    static std::string generateTokenCsvReport(const TokenSequence& tokens);

private:
    /**
     * 获取Token类型的字符串表示
     */
    static std::string getTokenTypeString(TokenType type);
    
    /**
     * 获取Token类型的颜色代码（用于控制台输出）
     */
    static std::string getTokenTypeColor(TokenType type);
    
    /**
     * 格式化Token值（处理特殊字符）
     */
    static std::string formatTokenValue(const std::string& value);
    
    /**
     * 获取Token的优先级（用于排序）
     */
    static int getTokenTypePriority(TokenType type);
    
    /**
     * 重置控制台颜色
     */
    static std::string resetColor();
};

/**
 * Token比较工具类
 */
class TokenComparator {
public:
    /**
     * 比较两个Token序列
     */
    static bool compareTokenSequences(const TokenSequence& seq1, const TokenSequence& seq2, bool ignorePosition = false);
    
    /**
     * 查找Token序列的差异
     */
    static std::vector<std::pair<size_t, std::string>> findTokenDifferences(
        const TokenSequence& expected, 
        const TokenSequence& actual
    );
    
    /**
     * 打印Token序列差异
     */
    static void printTokenDifferences(const TokenSequence& expected, const TokenSequence& actual);
    
    /**
     * 验证Token序列是否符合预期模式
     */
    static bool validateTokenPattern(const TokenSequence& tokens, const std::vector<TokenType>& expectedPattern);
    
    /**
     * 查找特定类型的Token
     */
    static std::vector<size_t> findTokensOfType(const TokenSequence& tokens, TokenType type);
    
    /**
     * 检查Token序列的完整性
     */
    static bool checkTokenSequenceIntegrity(const TokenSequence& tokens);
};

/**
 * Token过滤器类
 */
class TokenFilter {
public:
    /**
     * 过滤指定类型的Token
     */
    static TokenSequence filterByType(const TokenSequence& tokens, TokenType type);
    
    /**
     * 过滤多种类型的Token
     */
    static TokenSequence filterByTypes(const TokenSequence& tokens, const std::vector<TokenType>& types);
    
    /**
     * 移除注释Token
     */
    static TokenSequence removeComments(const TokenSequence& tokens);
    
    /**
     * 移除空白Token
     */
    static TokenSequence removeWhitespace(const TokenSequence& tokens);
    
    /**
     * 按行过滤Token
     */
    static TokenSequence filterByLine(const TokenSequence& tokens, size_t lineNumber);
    
    /**
     * 按行范围过滤Token
     */
    static TokenSequence filterByLineRange(const TokenSequence& tokens, size_t startLine, size_t endLine);
    
    /**
     * 按值过滤Token
     */
    static TokenSequence filterByValue(const TokenSequence& tokens, const std::string& value);
    
    /**
     * 按正则表达式过滤Token值
     */
    static TokenSequence filterByValuePattern(const TokenSequence& tokens, const std::string& pattern);
};

} // namespace Test
} // namespace CHTL