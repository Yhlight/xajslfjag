#include "TokenPrint.h"
#include <sstream>
#include <algorithm>
#include <map>
#include <stack>
#include <set>

namespace CHTL {
namespace Test {

// TokenPrinter implementation
void TokenPrinter::print(const CHTLToken& token) {
    switch (format_) {
        case Format::SIMPLE:
            printSimple(token);
            break;
        case Format::DETAILED:
            printDetailed(token);
            break;
        default:
            printSimple(token);
    }
}

void TokenPrinter::print(const std::vector<CHTLToken>& tokens) {
    switch (format_) {
        case Format::TABLE:
            printTable(tokens);
            break;
        case Format::JSON:
            printJSON(tokens);
            break;
        case Format::XML:
            printXML(tokens);
            break;
        default:
            for (const auto& token : tokens) {
                print(token);
            }
    }
}

void TokenPrinter::printSimple(const CHTLToken& token) {
    *os_ << tokenTypeToString(token.type) << ": " << token.value << std::endl;
}

void TokenPrinter::printDetailed(const CHTLToken& token) {
    *os_ << "Token {" << std::endl;
    *os_ << "  Type: " << tokenTypeToString(token.type) << std::endl;
    *os_ << "  Value: \"" << token.value << "\"" << std::endl;
    *os_ << "  Line: " << token.line << std::endl;
    *os_ << "  Column: " << token.column << std::endl;
    *os_ << "  Offset: " << token.offset << std::endl;
    *os_ << "  Length: " << token.length << std::endl;
    *os_ << "}" << std::endl;
}

void TokenPrinter::printTable(const std::vector<CHTLToken>& tokens) {
    // 计算列宽
    size_t maxTypeLen = 10;
    size_t maxValueLen = 20;
    
    for (const auto& token : tokens) {
        maxTypeLen = std::max(maxTypeLen, tokenTypeToString(token.type).length());
        maxValueLen = std::max(maxValueLen, token.value.length());
    }
    
    // 打印表头
    *os_ << std::string(80, '-') << std::endl;
    *os_ << "| " << std::setw(5) << "Index" 
         << " | " << std::setw(maxTypeLen) << "Type"
         << " | " << std::setw(maxValueLen) << "Value";
    
    if (showLineNumbers_) {
        *os_ << " | " << std::setw(5) << "Line"
             << " | " << std::setw(5) << "Col";
    }
    
    if (showPosition_) {
        *os_ << " | " << std::setw(6) << "Offset"
             << " | " << std::setw(6) << "Length";
    }
    
    *os_ << " |" << std::endl;
    *os_ << std::string(80, '-') << std::endl;
    
    // 打印Token
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::string typeStr = tokenTypeToString(token.type);
        
        if (colorOutput_) {
            typeStr = colorize(typeStr, getColorForTokenType(token.type));
        }
        
        *os_ << "| " << std::setw(5) << i
             << " | " << std::setw(maxTypeLen) << typeStr
             << " | " << std::setw(maxValueLen) << token.value;
        
        if (showLineNumbers_) {
            *os_ << " | " << std::setw(5) << token.line
                 << " | " << std::setw(5) << token.column;
        }
        
        if (showPosition_) {
            *os_ << " | " << std::setw(6) << token.offset
                 << " | " << std::setw(6) << token.length;
        }
        
        *os_ << " |" << std::endl;
    }
    
    *os_ << std::string(80, '-') << std::endl;
}

void TokenPrinter::printJSON(const std::vector<CHTLToken>& tokens) {
    *os_ << "{\n  \"tokens\": [\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        *os_ << "    {\n";
        *os_ << "      \"index\": " << i << ",\n";
        *os_ << "      \"type\": \"" << tokenTypeToString(token.type) << "\",\n";
        *os_ << "      \"value\": \"" << token.value << "\",\n";
        *os_ << "      \"line\": " << token.line << ",\n";
        *os_ << "      \"column\": " << token.column << ",\n";
        *os_ << "      \"offset\": " << token.offset << ",\n";
        *os_ << "      \"length\": " << token.length << "\n";
        *os_ << "    }";
        
        if (i < tokens.size() - 1) {
            *os_ << ",";
        }
        *os_ << "\n";
    }
    
    *os_ << "  ]\n}\n";
}

void TokenPrinter::printXML(const std::vector<CHTLToken>& tokens) {
    *os_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    *os_ << "<tokens count=\"" << tokens.size() << "\">\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        *os_ << "  <token index=\"" << i << "\">\n";
        *os_ << "    <type>" << tokenTypeToString(token.type) << "</type>\n";
        *os_ << "    <value><![CDATA[" << token.value << "]]></value>\n";
        *os_ << "    <position line=\"" << token.line 
             << "\" column=\"" << token.column
             << "\" offset=\"" << token.offset
             << "\" length=\"" << token.length << "\"/>\n";
        *os_ << "  </token>\n";
    }
    
    *os_ << "</tokens>\n";
}

void TokenPrinter::printStatistics(const std::vector<CHTLToken>& tokens) {
    std::map<CHTLTokenType, size_t> typeCounts;
    size_t totalChars = 0;
    size_t totalLines = 0;
    
    for (const auto& token : tokens) {
        typeCounts[token.type]++;
        totalChars += token.length;
        totalLines = std::max(totalLines, token.line);
    }
    
    *os_ << "\nToken Statistics:\n";
    *os_ << std::string(40, '-') << std::endl;
    *os_ << "Total tokens: " << tokens.size() << std::endl;
    *os_ << "Total characters: " << totalChars << std::endl;
    *os_ << "Total lines: " << totalLines << std::endl;
    *os_ << "\nToken type distribution:\n";
    
    for (const auto& [type, count] : typeCounts) {
        double percentage = (count * 100.0) / tokens.size();
        *os_ << std::setw(20) << tokenTypeToString(type) 
             << ": " << std::setw(5) << count
             << " (" << std::fixed << std::setprecision(1) << percentage << "%)\n";
    }
}

std::string TokenPrinter::tokenTypeToString(CHTLTokenType type) {
    static const std::map<CHTLTokenType, std::string> typeNames = {
        {CHTLTokenType::IDENTIFIER, "IDENTIFIER"},
        {CHTLTokenType::KEYWORD, "KEYWORD"},
        {CHTLTokenType::STRING, "STRING"},
        {CHTLTokenType::NUMBER, "NUMBER"},
        {CHTLTokenType::OPERATOR, "OPERATOR"},
        {CHTLTokenType::DELIMITER, "DELIMITER"},
        {CHTLTokenType::COMMENT, "COMMENT"},
        {CHTLTokenType::WHITESPACE, "WHITESPACE"},
        {CHTLTokenType::EOF_TOKEN, "EOF"},
        {CHTLTokenType::ERROR, "ERROR"}
    };
    
    auto it = typeNames.find(type);
    return it != typeNames.end() ? it->second : "UNKNOWN";
}

std::string TokenPrinter::colorize(const std::string& text, const std::string& color) {
    if (!colorOutput_) return text;
    return color + text + "\033[0m";
}

std::string TokenPrinter::getColorForTokenType(CHTLTokenType type) {
    switch (type) {
        case CHTLTokenType::KEYWORD:    return "\033[34m";  // Blue
        case CHTLTokenType::STRING:     return "\033[32m";  // Green
        case CHTLTokenType::NUMBER:     return "\033[33m";  // Yellow
        case CHTLTokenType::COMMENT:    return "\033[90m";  // Gray
        case CHTLTokenType::ERROR:      return "\033[31m";  // Red
        default:                        return "";
    }
}

// TokenComparator implementation
bool TokenComparator::compare(const CHTLToken& expected, const CHTLToken& actual, Difference& diff) {
    if (expected.type != actual.type) {
        diff.field = "type";
        diff.expected = std::to_string(static_cast<int>(expected.type));
        diff.actual = std::to_string(static_cast<int>(actual.type));
        return false;
    }
    
    if (expected.value != actual.value) {
        diff.field = "value";
        diff.expected = expected.value;
        diff.actual = actual.value;
        return false;
    }
    
    if (expected.line != actual.line) {
        diff.field = "line";
        diff.expected = std::to_string(expected.line);
        diff.actual = std::to_string(actual.line);
        return false;
    }
    
    if (expected.column != actual.column) {
        diff.field = "column";
        diff.expected = std::to_string(expected.column);
        diff.actual = std::to_string(actual.column);
        return false;
    }
    
    return true;
}

std::vector<TokenComparator::Difference> TokenComparator::compare(
    const std::vector<CHTLToken>& expected,
    const std::vector<CHTLToken>& actual) {
    
    std::vector<Difference> diffs;
    
    if (expected.size() != actual.size()) {
        Difference diff;
        diff.index = 0;
        diff.field = "count";
        diff.expected = std::to_string(expected.size());
        diff.actual = std::to_string(actual.size());
        diffs.push_back(diff);
        return diffs;
    }
    
    for (size_t i = 0; i < expected.size(); ++i) {
        Difference diff;
        diff.index = i;
        if (!compare(expected[i], actual[i], diff)) {
            diffs.push_back(diff);
        }
    }
    
    return diffs;
}

void TokenComparator::printDifferences(const std::vector<Difference>& diffs) {
    if (diffs.empty()) {
        std::cout << "No differences found.\n";
        return;
    }
    
    std::cout << "Found " << diffs.size() << " differences:\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& diff : diffs) {
        std::cout << "Token #" << diff.index << ", field: " << diff.field << "\n";
        std::cout << "  Expected: " << diff.expected << "\n";
        std::cout << "  Actual:   " << diff.actual << "\n";
        std::cout << std::string(60, '-') << "\n";
    }
}

// TokenFilter implementation
std::vector<CHTLToken> TokenFilter::filterByType(const std::vector<CHTLToken>& tokens, 
                                                CHTLTokenType type) {
    std::vector<CHTLToken> result;
    std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
                 [type](const CHTLToken& token) { return token.type == type; });
    return result;
}

std::vector<CHTLToken> TokenFilter::filterByLine(const std::vector<CHTLToken>& tokens,
                                                size_t startLine, size_t endLine) {
    std::vector<CHTLToken> result;
    std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
                 [startLine, endLine](const CHTLToken& token) {
                     return token.line >= startLine && token.line <= endLine;
                 });
    return result;
}

std::vector<CHTLToken> TokenFilter::excludeComments(const std::vector<CHTLToken>& tokens) {
    std::vector<CHTLToken> result;
    std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
                 [](const CHTLToken& token) { return token.type != CHTLTokenType::COMMENT; });
    return result;
}

std::vector<CHTLToken> TokenFilter::excludeWhitespace(const std::vector<CHTLToken>& tokens) {
    std::vector<CHTLToken> result;
    std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
                 [](const CHTLToken& token) { return token.type != CHTLTokenType::WHITESPACE; });
    return result;
}

// TokenValidator implementation
std::vector<TokenValidator::ValidationError> TokenValidator::validate(const std::vector<CHTLToken>& tokens) {
    std::vector<ValidationError> errors;
    
    // Check balance
    if (!checkBalance(tokens)) {
        errors.push_back({0, "Unbalanced brackets or braces"});
    }
    
    // Check keywords
    if (!checkKeywords(tokens)) {
        errors.push_back({0, "Invalid keyword usage"});
    }
    
    // Check identifiers
    if (!checkIdentifiers(tokens)) {
        errors.push_back({0, "Invalid identifier naming"});
    }
    
    return errors;
}

bool TokenValidator::checkBalance(const std::vector<CHTLToken>& tokens) {
    std::stack<char> brackets;
    
    for (const auto& token : tokens) {
        if (token.type == CHTLTokenType::DELIMITER) {
            if (token.value == "(" || token.value == "[" || token.value == "{") {
                brackets.push(token.value[0]);
            } else if (token.value == ")" || token.value == "]" || token.value == "}") {
                if (brackets.empty()) return false;
                
                char open = brackets.top();
                brackets.pop();
                
                if ((token.value == ")" && open != '(') ||
                    (token.value == "]" && open != '[') ||
                    (token.value == "}" && open != '{')) {
                    return false;
                }
            }
        }
    }
    
    return brackets.empty();
}

bool TokenValidator::checkKeywords(const std::vector<CHTLToken>& tokens) {
    // 实现关键字验证
    static const std::set<std::string> reservedKeywords = {
        "Template", "Custom", "Import", "Namespace", "Configuration",
        "Origin", "use", "except", "delete", "insert", "Style", "Script"
    };
    
    for (const auto& token : tokens) {
        if (token.type == CHTLTokenType::IDENTIFIER) {
            // 检查标识符是否误用了保留关键字
            if (reservedKeywords.find(token.value) != reservedKeywords.end()) {
                // 这应该是一个关键字token，不是标识符
                return false;
            }
        } else if (token.type == CHTLTokenType::KEYWORD) {
            // 检查关键字是否有效
            if (reservedKeywords.find(token.value) == reservedKeywords.end()) {
                // 无效的关键字
                return false;
            }
        }
    }
    
    return true;
}

bool TokenValidator::checkIdentifiers(const std::vector<CHTLToken>& tokens) {
    // 实现标识符验证
    for (const auto& token : tokens) {
        if (token.type == CHTLTokenType::IDENTIFIER) {
            const std::string& id = token.value;
            
            // 检查标识符是否为空
            if (id.empty()) {
                return false;
            }
            
            // 检查第一个字符是否是字母或下划线
            if (!std::isalpha(id[0]) && id[0] != '_') {
                return false;
            }
            
            // 检查其余字符是否是字母、数字或下划线
            for (size_t i = 1; i < id.length(); ++i) {
                if (!std::isalnum(id[i]) && id[i] != '_') {
                    return false;
                }
            }
            
            // 检查是否过长（例如超过255个字符）
            if (id.length() > 255) {
                return false;
            }
        }
    }
    
    return true;
}

// TokenDebugger implementation
void TokenDebugger::showTokenInContext(const CHTLToken& token, int contextLines) {
    auto lines = getLines();
    
    int startLine = std::max(0, static_cast<int>(token.line) - contextLines - 1);
    int endLine = std::min(static_cast<int>(lines.size()), 
                           static_cast<int>(token.line) + contextLines);
    
    std::cout << "\nToken in context:\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (int i = startLine; i < endLine; ++i) {
        bool isTokenLine = (i + 1 == static_cast<int>(token.line));
        
        std::cout << std::setw(4) << (i + 1) << " | ";
        
        if (isTokenLine) {
            printLineWithHighlight(i + 1, token.column, token.column + token.length);
        } else {
            std::cout << lines[i] << "\n";
        }
    }
    
    std::cout << std::string(60, '-') << "\n";
}

std::vector<std::string> TokenDebugger::getLines() {
    std::vector<std::string> lines;
    std::istringstream stream(source_);
    std::string line;
    
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

void TokenDebugger::printLineWithHighlight(size_t lineNum, size_t colStart, size_t colEnd) {
    auto lines = getLines();
    if (lineNum == 0 || lineNum > lines.size()) return;
    
    const std::string& line = lines[lineNum - 1];
    
    // Print line with highlighting
    for (size_t i = 0; i < line.length(); ++i) {
        if (i >= colStart - 1 && i < colEnd - 1) {
            std::cout << "\033[41m" << line[i] << "\033[0m";  // Red background
        } else {
            std::cout << line[i];
        }
    }
    std::cout << "\n";
    
    // Print marker line
    std::cout << "     | ";
    for (size_t i = 0; i < colStart - 1; ++i) {
        std::cout << " ";
    }
    for (size_t i = colStart - 1; i < colEnd - 1; ++i) {
        std::cout << "^";
    }
    std::cout << "\n";
}

} // namespace Test
} // namespace CHTL