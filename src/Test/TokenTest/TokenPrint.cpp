#include "TokenPrint.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace CHTL::Test {

// Colors定义
const String TokenPrint::Colors::RESET = "\033[0m";
const String TokenPrint::Colors::RED = "\033[31m";
const String TokenPrint::Colors::GREEN = "\033[32m";
const String TokenPrint::Colors::YELLOW = "\033[33m";
const String TokenPrint::Colors::BLUE = "\033[34m";
const String TokenPrint::Colors::MAGENTA = "\033[35m";
const String TokenPrint::Colors::CYAN = "\033[36m";
const String TokenPrint::Colors::WHITE = "\033[37m";
const String TokenPrint::Colors::BOLD = "\033[1m";

void TokenPrint::printToken(const CHTL::Token& token, std::ostream& os) {
    PrintOptions defaultOptions;
    printTokenWithOptions(token, defaultOptions, os);
}

void TokenPrint::printTokens(const std::vector<CHTL::Token>& tokens, std::ostream& os) {
    PrintOptions defaultOptions;
    printTokensWithOptions(tokens, defaultOptions, os);
}

String TokenPrint::tokenToString(const CHTL::Token& token) {
    std::ostringstream oss;
    printToken(token, oss);
    return oss.str();
}

String TokenPrint::tokensToString(const std::vector<CHTL::Token>& tokens) {
    std::ostringstream oss;
    printTokens(tokens, oss);
    return oss.str();
}

void TokenPrint::printTokenWithOptions(const CHTL::Token& token, 
                                      const PrintOptions& options, 
                                      std::ostream& os) {
    std::ostringstream line;
    
    if (options.showType) {
        line << colorize(getTokenTypeName(token.getType()), Colors::BLUE, options.colorOutput);
        line << options.separator;
    }
    
    if (options.showValue) {
        String value = formatTokenValue(token.getValue(), options.maxValueLength);
        line << colorize("\"" + value + "\"", Colors::GREEN, options.colorOutput);
        line << options.separator;
    }
    
    if (options.showPosition) {
        String posStr = formatPosition(token.getPosition());
        line << colorize(posStr, Colors::YELLOW, options.colorOutput);
    }
    
    os << line.str() << std::endl;
}

void TokenPrint::printTokensWithOptions(const std::vector<CHTL::Token>& tokens, 
                                       const PrintOptions& options, 
                                       std::ostream& os) {
    if (options.compactFormat) {
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (i > 0) os << " ";
            os << getTokenTypeName(tokens[i].getType()) << "(" << tokens[i].getValue() << ")";
        }
        os << std::endl;
        return;
    }
    
    int lineNum = 1;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (options.showLineNumbers) {
            os << std::setw(4) << lineNum << ": ";
            lineNum++;
        }
        
        os << options.indentation;
        printTokenWithOptions(tokens[i], options, os);
    }
}

void TokenPrint::printTokensAsTable(const std::vector<CHTL::Token>& tokens, std::ostream& os) {
    printTableHeader(os);
    printTableSeparator(os);
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        printTableRow(tokens[i], static_cast<int>(i), os);
    }
    
    printTableSeparator(os);
}

void TokenPrint::printTokensAsJSON(const std::vector<CHTL::Token>& tokens, std::ostream& os) {
    os << "{\n";
    os << "  \"tokens\": [\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        os << "    {\n";
        os << "      \"index\": " << i << ",\n";
        os << "      \"type\": \"" << getTokenTypeName(tokens[i].getType()) << "\",\n";
        os << "      \"value\": \"" << escapeJSON(tokens[i].getValue()) << "\",\n";
        os << "      \"position\": {\n";
        os << "        \"line\": " << tokens[i].getPosition().line << ",\n";
        os << "        \"column\": " << tokens[i].getPosition().column << ",\n";
        os << "        \"offset\": " << tokens[i].getPosition().offset << "\n";
        os << "      }\n";
        os << "    }";
        
        if (i < tokens.size() - 1) {
            os << ",";
        }
        os << "\n";
    }
    
    os << "  ]\n";
    os << "}\n";
}

void TokenPrint::printTokensAsXML(const std::vector<CHTL::Token>& tokens, std::ostream& os) {
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    os << "<tokens>\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        os << "  <token index=\"" << i << "\">\n";
        os << "    <type>" << escapeXML(getTokenTypeName(tokens[i].getType())) << "</type>\n";
        os << "    <value>" << escapeXML(tokens[i].getValue()) << "</value>\n";
        os << "    <position>\n";
        os << "      <line>" << tokens[i].getPosition().line << "</line>\n";
        os << "      <column>" << tokens[i].getPosition().column << "</column>\n";
        os << "      <offset>" << tokens[i].getPosition().offset << "</offset>\n";
        os << "    </position>\n";
        os << "  </token>\n";
    }
    
    os << "</tokens>\n";
}

void TokenPrint::printTokenStatistics(const std::vector<CHTL::Token>& tokens, std::ostream& os) {
    auto stats = getTokenStatistics(tokens);
    
    os << "Token Statistics:\n";
    os << "================\n";
    os << "Total Tokens: " << tokens.size() << "\n";
    os << "Token Types:\n";
    
    for (const auto& [type, count] : stats) {
        os << "  " << std::setw(20) << getTokenTypeName(type) 
           << ": " << std::setw(6) << count 
           << " (" << std::fixed << std::setprecision(1) 
           << (double)count / tokens.size() * 100.0 << "%)\n";
    }
}

String TokenPrint::getTokenTypeName(CHTL::TokenType type) {
    // 这里需要根据实际的TokenType枚举来实现
    // 暂时返回一个基础实现
    switch (type) {
        case CHTL::TokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTL::TokenType::LITERAL_STRING: return "LITERAL_STRING";
        case CHTL::TokenType::LITERAL_NUMBER: return "LITERAL_NUMBER";
        case CHTL::TokenType::OPERATOR: return "OPERATOR";
        case CHTL::TokenType::KEYWORD: return "KEYWORD";
        case CHTL::TokenType::PUNCTUATION: return "PUNCTUATION";
        case CHTL::TokenType::COMMENT: return "COMMENT";
        case CHTL::TokenType::WHITESPACE: return "WHITESPACE";
        case CHTL::TokenType::NEWLINE: return "NEWLINE";
        case CHTL::TokenType::EOF_TOKEN: return "EOF";
        default: return "UNKNOWN";
    }
}

String TokenPrint::getNodeTypeName(CHTL::NodeType type) {
    // 这里需要根据实际的NodeType枚举来实现
    switch (type) {
        case CHTL::NodeType::ELEMENT: return "ELEMENT";
        case CHTL::NodeType::TEXT: return "TEXT";
        case CHTL::NodeType::COMMENT: return "COMMENT";
        case CHTL::NodeType::TEMPLATE: return "TEMPLATE";
        case CHTL::NodeType::CUSTOM: return "CUSTOM";
        case CHTL::NodeType::STYLE: return "STYLE";
        case CHTL::NodeType::SCRIPT: return "SCRIPT";
        case CHTL::NodeType::ORIGIN: return "ORIGIN";
        case CHTL::NodeType::IMPORT: return "IMPORT";
        case CHTL::NodeType::CONFIG: return "CONFIG";
        default: return "UNKNOWN";
    }
}

String TokenPrint::formatPosition(const CHTL::Position& pos) {
    std::ostringstream oss;
    oss << pos.line << ":" << pos.column;
    if (pos.offset >= 0) {
        oss << "@" << pos.offset;
    }
    return oss.str();
}

String TokenPrint::formatTokenValue(const String& value, int maxLength) {
    if (value.length() <= static_cast<size_t>(maxLength)) {
        return value;
    }
    
    return value.substr(0, maxLength - 3) + "...";
}

String TokenPrint::colorize(const String& text, const String& color, bool useColor) {
    if (!useColor) {
        return text;
    }
    return color + text + Colors::RESET;
}

void TokenPrint::printTableHeader(std::ostream& os) {
    os << std::setw(6) << "Index" << " | "
       << std::setw(15) << "Type" << " | "
       << std::setw(30) << "Value" << " | "
       << std::setw(12) << "Position" << "\n";
}

void TokenPrint::printTableRow(const CHTL::Token& token, int index, std::ostream& os) {
    String value = formatTokenValue(token.getValue(), 30);
    String position = formatPosition(token.getPosition());
    
    os << std::setw(6) << index << " | "
       << std::setw(15) << getTokenTypeName(token.getType()) << " | "
       << std::setw(30) << value << " | "
       << std::setw(12) << position << "\n";
}

void TokenPrint::printTableSeparator(std::ostream& os) {
    os << std::string(6 + 3 + 15 + 3 + 30 + 3 + 12, '-') << "\n";
}

String TokenPrint::escapeXML(const String& text) {
    String result = text;
    size_t pos = 0;
    
    // 替换特殊字符
    while ((pos = result.find('&', pos)) != String::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find('<', pos)) != String::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('>', pos)) != String::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('"', pos)) != String::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    return result;
}

String TokenPrint::escapeJSON(const String& text) {
    String result = text;
    size_t pos = 0;
    
    // 替换反斜杠
    while ((pos = result.find('\\', pos)) != String::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    // 替换引号
    pos = 0;
    while ((pos = result.find('"', pos)) != String::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    return result;
}

std::map<CHTL::TokenType, int> TokenPrint::getTokenStatistics(const std::vector<CHTL::Token>& tokens) {
    std::map<CHTL::TokenType, int> stats;
    
    for (const auto& token : tokens) {
        stats[token.getType()]++;
    }
    
    return stats;
}

// TokenDiffer实现（基础版本）
std::vector<TokenDiffer::Difference> TokenDiffer::compareTokens(const std::vector<CHTL::Token>& expected,
                                                               const std::vector<CHTL::Token>& actual) {
    std::vector<Difference> differences;
    
    size_t minSize = std::min(expected.size(), actual.size());
    
    // 比较公共部分
    for (size_t i = 0; i < minSize; ++i) {
        if (!areTokensEqual(expected[i], actual[i])) {
            Difference diff;
            diff.type = Difference::MODIFIED;
            diff.expectedIndex = static_cast<int>(i);
            diff.actualIndex = static_cast<int>(i);
            diff.description = "Token mismatch";
            diff.expectedToken = expected[i];
            diff.actualToken = actual[i];
            differences.push_back(diff);
        }
    }
    
    // 处理不同长度的情况
    if (expected.size() > actual.size()) {
        for (size_t i = minSize; i < expected.size(); ++i) {
            Difference diff;
            diff.type = Difference::REMOVED;
            diff.expectedIndex = static_cast<int>(i);
            diff.actualIndex = -1;
            diff.description = "Token removed";
            diff.expectedToken = expected[i];
            differences.push_back(diff);
        }
    } else if (actual.size() > expected.size()) {
        for (size_t i = minSize; i < actual.size(); ++i) {
            Difference diff;
            diff.type = Difference::ADDED;
            diff.expectedIndex = -1;
            diff.actualIndex = static_cast<int>(i);
            diff.description = "Token added";
            diff.actualToken = actual[i];
            differences.push_back(diff);
        }
    }
    
    return differences;
}

bool TokenDiffer::areTokensEqual(const CHTL::Token& token1, const CHTL::Token& token2, bool ignorePosition) {
    if (token1.getType() != token2.getType()) {
        return false;
    }
    
    if (token1.getValue() != token2.getValue()) {
        return false;
    }
    
    if (!ignorePosition) {
        const auto& pos1 = token1.getPosition();
        const auto& pos2 = token2.getPosition();
        
        return pos1.line == pos2.line && 
               pos1.column == pos2.column && 
               pos1.offset == pos2.offset;
    }
    
    return true;
}

void TokenDiffer::printDifferences(const std::vector<Difference>& differences, std::ostream& os) {
    os << "Token Differences Found: " << differences.size() << "\n";
    os << "================================\n";
    
    for (const auto& diff : differences) {
        switch (diff.type) {
            case Difference::ADDED:
                os << "[ADDED] ";
                break;
            case Difference::REMOVED:
                os << "[REMOVED] ";
                break;
            case Difference::MODIFIED:
                os << "[MODIFIED] ";
                break;
            case Difference::POSITION_CHANGED:
                os << "[POSITION] ";
                break;
        }
        
        os << diff.description << "\n";
    }
}

String TokenDiffer::differencesToString(const std::vector<Difference>& differences) {
    std::ostringstream oss;
    printDifferences(differences, oss);
    return oss.str();
}

} // namespace CHTL::Test