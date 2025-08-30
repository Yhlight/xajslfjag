#pragma once
#include "../../CHTL/CHTLLexer/Token.h"
#include "../../CHTL JS/CHTLJSLexer/Token.h"
#include <string>
#include <vector>

namespace CHTL {
namespace Test {

/**
 * Token打印工具
 * 提供Token序列的格式化输出
 */
class TokenPrint {
public:
    /**
     * 打印CHTL Token序列
     */
    static void printCHTLTokens(const CHTL::TokenSequence& tokens);
    
    /**
     * 打印CHTL JS Token序列
     */
    static void printCHTLJSTokens(const CHTLJS::TokenSequence& tokens);
    
    /**
     * 生成Token表格
     */
    static std::string generateTokenTable(const CHTL::TokenSequence& tokens);
    static std::string generateTokenTable(const CHTLJS::TokenSequence& tokens);
    
    /**
     * 导出Token到文件
     */
    static bool exportTokensToFile(const CHTL::TokenSequence& tokens, const std::string& filename);
    static bool exportTokensToFile(const CHTLJS::TokenSequence& tokens, const std::string& filename);

private:
    static std::string formatTokenInfo(const CHTL::Token& token);
    static std::string formatTokenInfo(const CHTLJS::Token& token);
};

} // namespace Test
} // namespace CHTL