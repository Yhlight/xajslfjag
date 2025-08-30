#pragma once
#include "../../CHTL/CHTLLexer/Token.h"
#include "../../CHTL JS/CHTLJSLexer/Token.h"
#include <string>
#include <vector>

namespace CHTL {
namespace Test {

/**
 * Token表格生成器
 * 生成详细的Token分析表格
 */
class TokenTable {
public:
    /**
     * 生成CHTL Token分析表格
     */
    static std::string generateCHTLTokenTable(const CHTL::TokenSequence& tokens);
    
    /**
     * 生成CHTL JS Token分析表格
     */
    static std::string generateCHTLJSTokenTable(const CHTLJS::TokenSequence& tokens);
    
    /**
     * 生成Token统计信息
     */
    static std::string generateTokenStatistics(const CHTL::TokenSequence& tokens);
    static std::string generateTokenStatistics(const CHTLJS::TokenSequence& tokens);
    
    /**
     * 生成Token类型分布
     */
    static std::string generateTokenTypeDistribution(const CHTL::TokenSequence& tokens);
    static std::string generateTokenTypeDistribution(const CHTLJS::TokenSequence& tokens);
    
    /**
     * 导出表格到HTML
     */
    static bool exportTableToHTML(const CHTL::TokenSequence& tokens, const std::string& filename);
    static bool exportTableToHTML(const CHTLJS::TokenSequence& tokens, const std::string& filename);

private:
    static std::string generateHTMLHeader();
    static std::string generateHTMLFooter();
    static std::string formatTokenRow(const CHTL::Token& token, size_t index);
    static std::string formatTokenRow(const CHTLJS::Token& token, size_t index);
};

} // namespace Test
} // namespace CHTL