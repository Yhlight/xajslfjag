#pragma once

#include "../../Util/Common.h"
#include "../../CHTL/CHTLLexer/Token.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>

namespace CHTL::Test {

// Token打印器 - 用于调试和测试
class TokenPrint {
public:
    // 打印单个Token
    static void printToken(const CHTL::Token& token, std::ostream& os = std::cout);
    
    // 打印Token向量
    static void printTokens(const std::vector<CHTL::Token>& tokens, std::ostream& os = std::cout);
    
    // 打印Token到字符串
    static String tokenToString(const CHTL::Token& token);
    static String tokensToString(const std::vector<CHTL::Token>& tokens);
    
    // 格式化打印选项
    struct PrintOptions {
        bool showPosition = true;
        bool showType = true;
        bool showValue = true;
        bool showLineNumbers = true;
        bool colorOutput = false;
        bool compactFormat = false;
        int maxValueLength = 50;
        String indentation = "  ";
        String separator = " | ";
        
        PrintOptions() = default;
    };
    
    // 使用选项打印
    static void printTokenWithOptions(const CHTL::Token& token, 
                                     const PrintOptions& options, 
                                     std::ostream& os = std::cout);
    
    static void printTokensWithOptions(const std::vector<CHTL::Token>& tokens, 
                                      const PrintOptions& options, 
                                      std::ostream& os = std::cout);
    
    // 专用格式
    static void printTokensAsTable(const std::vector<CHTL::Token>& tokens, 
                                  std::ostream& os = std::cout);
    
    static void printTokensAsJSON(const std::vector<CHTL::Token>& tokens, 
                                 std::ostream& os = std::cout);
    
    static void printTokensAsXML(const std::vector<CHTL::Token>& tokens, 
                                std::ostream& os = std::cout);
    
    // 统计信息
    static void printTokenStatistics(const std::vector<CHTL::Token>& tokens, 
                                    std::ostream& os = std::cout);
    
    // 错误Token高亮显示
    static void printTokensWithErrors(const std::vector<CHTL::Token>& tokens,
                                     const std::vector<int>& errorIndices,
                                     std::ostream& os = std::cout);
    
    // 比较两组Token
    static void printTokenComparison(const std::vector<CHTL::Token>& expected,
                                    const std::vector<CHTL::Token>& actual,
                                    std::ostream& os = std::cout);
    
    // 工具方法
    static String getTokenTypeName(CHTL::TokenType type);
    static String getNodeTypeName(CHTL::NodeType type);
    static String formatPosition(const CHTL::Position& pos);
    static String formatTokenValue(const String& value, int maxLength = 50);
    
    // 颜色控制
    struct Colors {
        static const String RESET;
        static const String RED;
        static const String GREEN;
        static const String YELLOW;
        static const String BLUE;
        static const String MAGENTA;
        static const String CYAN;
        static const String WHITE;
        static const String BOLD;
    };
    
    static String colorize(const String& text, const String& color, bool useColor = true);
    
private:
    static void printTableHeader(std::ostream& os);
    static void printTableRow(const CHTL::Token& token, int index, std::ostream& os);
    static void printTableSeparator(std::ostream& os);
    
    static String escapeXML(const String& text);
    static String escapeJSON(const String& text);
    
    static std::map<CHTL::TokenType, int> getTokenStatistics(const std::vector<CHTL::Token>& tokens);
};

// Token差异分析器
class TokenDiffer {
public:
    struct Difference {
        enum Type {
            ADDED,
            REMOVED,
            MODIFIED,
            POSITION_CHANGED
        };
        
        Type type;
        int expectedIndex;
        int actualIndex;
        String description;
        CHTL::Token expectedToken;
        CHTL::Token actualToken;
    };
    
    static std::vector<Difference> compareTokens(const std::vector<CHTL::Token>& expected,
                                                const std::vector<CHTL::Token>& actual);
    
    static void printDifferences(const std::vector<Difference>& differences,
                                std::ostream& os = std::cout);
    
    static bool areTokensEqual(const CHTL::Token& token1, const CHTL::Token& token2,
                              bool ignorePosition = false);
    
    static String differencesToString(const std::vector<Difference>& differences);
    
private:
    static int calculateEditDistance(const std::vector<CHTL::Token>& seq1,
                                   const std::vector<CHTL::Token>& seq2);
    
    static std::vector<Difference> traceback(const std::vector<CHTL::Token>& expected,
                                           const std::vector<CHTL::Token>& actual,
                                           const std::vector<std::vector<int>>& dp);
};

// Token序列验证器
class TokenValidator {
public:
    struct ValidationRule {
        String name;
        std::function<bool(const CHTL::Token&)> predicate;
        String errorMessage;
        
        ValidationRule(const String& n, 
                      std::function<bool(const CHTL::Token&)> pred, 
                      const String& msg)
            : name(n), predicate(pred), errorMessage(msg) {}
    };
    
    struct ValidationResult {
        bool isValid;
        std::vector<String> errors;
        std::vector<String> warnings;
        std::vector<int> errorIndices;
        
        ValidationResult() : isValid(true) {}
    };
    
    // 添加验证规则
    void addRule(const ValidationRule& rule);
    void addSequenceRule(const String& name, 
                        std::function<bool(const std::vector<CHTL::Token>&)> predicate,
                        const String& errorMessage);
    
    // 执行验证
    ValidationResult validate(const std::vector<CHTL::Token>& tokens);
    
    // 预定义规则
    static ValidationRule createPositionValidationRule();
    static ValidationRule createTypeValidationRule();
    static ValidationRule createValueValidationRule();
    static ValidationRule createBraceMatchingRule();
    
    void addCommonRules();
    
private:
    std::vector<ValidationRule> tokenRules_;
    std::vector<std::pair<String, std::function<bool(const std::vector<CHTL::Token>&)>>> sequenceRules_;
    std::vector<String> sequenceErrorMessages_;
    
    bool validateBraceMatching(const std::vector<CHTL::Token>& tokens);
    bool validateParenMatching(const std::vector<CHTL::Token>& tokens);
    bool validatePositions(const std::vector<CHTL::Token>& tokens);
};

} // namespace CHTL::Test