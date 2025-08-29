#ifndef TOKEN_PRINT_H
#define TOKEN_PRINT_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "../../CHTL/CHTLLexer/Token.h"
#include "../../CHTLJS/CHTLJSLexer/Token.h"

namespace CHTL {
namespace Test {

// Token打印器
class TokenPrinter {
public:
    enum class Format {
        SIMPLE,      // 简单格式
        DETAILED,    // 详细格式
        TABLE,       // 表格格式
        JSON,        // JSON格式
        XML          // XML格式
    };
    
    TokenPrinter(Format format = Format::TABLE) : format_(format) {}
    
    // 打印CHTL Token
    void print(const CHTLToken& token);
    void print(const std::vector<CHTLToken>& tokens);
    
    // 打印CHTL JS Token
    void print(const CHTLJSToken& token);
    void print(const std::vector<CHTLJSToken>& tokens);
    
    // 设置输出流
    void setOutputStream(std::ostream& os) { os_ = &os; }
    
    // 设置格式
    void setFormat(Format format) { format_ = format; }
    
    // 设置选项
    void setShowLineNumbers(bool show) { showLineNumbers_ = show; }
    void setShowPosition(bool show) { showPosition_ = show; }
    void setColorOutput(bool enable) { colorOutput_ = enable; }
    
    // 统计信息
    void printStatistics(const std::vector<CHTLToken>& tokens);
    void printStatistics(const std::vector<CHTLJSToken>& tokens);

private:
    Format format_;
    std::ostream* os_ = &std::cout;
    bool showLineNumbers_ = true;
    bool showPosition_ = true;
    bool colorOutput_ = true;
    
    // 格式化方法
    void printSimple(const CHTLToken& token);
    void printDetailed(const CHTLToken& token);
    void printTable(const std::vector<CHTLToken>& tokens);
    void printJSON(const std::vector<CHTLToken>& tokens);
    void printXML(const std::vector<CHTLToken>& tokens);
    
    // 辅助方法
    std::string tokenTypeToString(CHTLTokenType type);
    std::string tokenTypeToString(CHTLJSTokenType type);
    std::string colorize(const std::string& text, const std::string& color);
    std::string getColorForTokenType(CHTLTokenType type);
    std::string getColorForTokenType(CHTLJSTokenType type);
};

// Token比较器
class TokenComparator {
public:
    struct Difference {
        size_t index;
        std::string field;  // type, value, line, column
        std::string expected;
        std::string actual;
    };
    
    // 比较单个Token
    static bool compare(const CHTLToken& expected, const CHTLToken& actual, Difference& diff);
    static bool compare(const CHTLJSToken& expected, const CHTLJSToken& actual, Difference& diff);
    
    // 比较Token列表
    static std::vector<Difference> compare(const std::vector<CHTLToken>& expected, 
                                         const std::vector<CHTLToken>& actual);
    static std::vector<Difference> compare(const std::vector<CHTLJSToken>& expected,
                                         const std::vector<CHTLJSToken>& actual);
    
    // 打印差异
    static void printDifferences(const std::vector<Difference>& diffs);
};

// Token过滤器
class TokenFilter {
public:
    // 按类型过滤
    static std::vector<CHTLToken> filterByType(const std::vector<CHTLToken>& tokens, 
                                               CHTLTokenType type);
    static std::vector<CHTLJSToken> filterByType(const std::vector<CHTLJSToken>& tokens,
                                                CHTLJSTokenType type);
    
    // 按行号过滤
    static std::vector<CHTLToken> filterByLine(const std::vector<CHTLToken>& tokens,
                                              size_t startLine, size_t endLine);
    
    // 排除注释
    static std::vector<CHTLToken> excludeComments(const std::vector<CHTLToken>& tokens);
    
    // 排除空白
    static std::vector<CHTLToken> excludeWhitespace(const std::vector<CHTLToken>& tokens);
};

// Token验证器
class TokenValidator {
public:
    struct ValidationError {
        size_t tokenIndex;
        std::string message;
    };
    
    // 验证Token序列
    static std::vector<ValidationError> validate(const std::vector<CHTLToken>& tokens);
    static std::vector<ValidationError> validate(const std::vector<CHTLJSToken>& tokens);
    
    // 检查平衡性（括号、花括号等）
    static bool checkBalance(const std::vector<CHTLToken>& tokens);
    
    // 检查关键字使用
    static bool checkKeywords(const std::vector<CHTLToken>& tokens);
    
    // 检查标识符命名
    static bool checkIdentifiers(const std::vector<CHTLToken>& tokens);
};

// Token调试器
class TokenDebugger {
public:
    TokenDebugger(const std::string& source) : source_(source) {}
    
    // 显示Token在源代码中的位置
    void showTokenInContext(const CHTLToken& token, int contextLines = 2);
    void showTokenInContext(const CHTLJSToken& token, int contextLines = 2);
    
    // 高亮显示Token
    void highlightTokens(const std::vector<CHTLToken>& tokens);
    
    // 生成Token图
    void generateTokenGraph(const std::vector<CHTLToken>& tokens, const std::string& outputFile);
    
    // 交互式调试
    void startInteractiveDebug(const std::vector<CHTLToken>& tokens);

private:
    std::string source_;
    
    std::vector<std::string> getLines();
    void printLineWithHighlight(size_t lineNum, size_t colStart, size_t colEnd);
};

} // namespace Test
} // namespace CHTL

#endif // TOKEN_PRINT_H