#include "TokenTable.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <unordered_map>

namespace CHTL {
namespace Test {

std::string TokenTable::generateCHTLTokenTable(const CHTL::TokenSequence& tokens) {
    std::ostringstream table;
    
    table << "<!DOCTYPE html>\n<html>\n<head>\n";
    table << "<title>CHTL Token分析表</title>\n";
    table << "<style>\n";
    table << "table { border-collapse: collapse; width: 100%; }\n";
    table << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    table << "th { background-color: #f2f2f2; }\n";
    table << "</style>\n";
    table << "</head>\n<body>\n";
    
    table << "<h1>CHTL Token分析表</h1>\n";
    table << "<p>总Token数: " << tokens.size() << "</p>\n";
    
    table << "<table>\n";
    table << "<tr><th>索引</th><th>类型</th><th>值</th><th>行号</th><th>列号</th></tr>\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        table << formatTokenRow(tokens[i], i);
    }
    
    table << "</table>\n";
    table << "</body>\n</html>";
    
    return table.str();
}

std::string TokenTable::generateCHTLJSTokenTable(const CHTLJS::TokenSequence& tokens) {
    std::ostringstream table;
    
    table << "<!DOCTYPE html>\n<html>\n<head>\n";
    table << "<title>CHTL JS Token分析表</title>\n";
    table << "<style>\n";
    table << "table { border-collapse: collapse; width: 100%; }\n";
    table << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    table << "th { background-color: #f2f2f2; }\n";
    table << "</style>\n";
    table << "</head>\n<body>\n";
    
    table << "<h1>CHTL JS Token分析表</h1>\n";
    table << "<p>总Token数: " << tokens.size() << "</p>\n";
    
    table << "<table>\n";
    table << "<tr><th>索引</th><th>类型</th><th>值</th><th>行号</th><th>列号</th></tr>\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        table << formatTokenRow(tokens[i], i);
    }
    
    table << "</table>\n";
    table << "</body>\n</html>";
    
    return table.str();
}

std::string TokenTable::generateTokenStatistics(const CHTL::TokenSequence& tokens) {
    std::unordered_map<std::string, int> typeCount;
    
    for (const auto& token : tokens) {
        typeCount[token.getTypeName()]++;
    }
    
    std::ostringstream stats;
    stats << "CHTL Token统计\n";
    stats << "==============\n";
    stats << "总Token数: " << tokens.size() << "\n\n";
    stats << "类型分布:\n";
    
    for (const auto& pair : typeCount) {
        stats << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    return stats.str();
}

std::string TokenTable::generateTokenStatistics(const CHTLJS::TokenSequence& tokens) {
    std::unordered_map<std::string, int> typeCount;
    
    for (const auto& token : tokens) {
        typeCount[token.getTypeName()]++;
    }
    
    std::ostringstream stats;
    stats << "CHTL JS Token统计\n";
    stats << "==================\n";
    stats << "总Token数: " << tokens.size() << "\n\n";
    stats << "类型分布:\n";
    
    for (const auto& pair : typeCount) {
        stats << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    return stats.str();
}

std::string TokenTable::generateTokenTypeDistribution(const CHTL::TokenSequence& tokens) {
    std::unordered_map<std::string, int> distribution;
    
    for (const auto& token : tokens) {
        std::string category = "其他";
        
        if (token.isKeyword()) category = "关键字";
        else if (token.isOperator()) category = "操作符";
        else if (token.isLiteral()) category = "字面量";
        else if (token.isSelector()) category = "选择器";
        
        distribution[category]++;
    }
    
    std::ostringstream dist;
    dist << "CHTL Token类型分布\n";
    dist << "==================\n";
    
    for (const auto& pair : distribution) {
        double percentage = (double)pair.second / tokens.size() * 100.0;
        dist << pair.first << ": " << pair.second << " (" 
             << std::fixed << std::setprecision(1) << percentage << "%)\n";
    }
    
    return dist.str();
}

std::string TokenTable::generateTokenTypeDistribution(const CHTLJS::TokenSequence& tokens) {
    std::unordered_map<std::string, int> distribution;
    
    for (const auto& token : tokens) {
        std::string category = "其他";
        
        if (token.isKeyword()) category = "关键字";
        else if (token.isOperator()) category = "操作符";
        else if (token.isLiteral()) category = "字面量";
        else if (token.isSelector()) category = "选择器";
        else if (token.isEvent()) category = "事件";
        
        distribution[category]++;
    }
    
    std::ostringstream dist;
    dist << "CHTL JS Token类型分布\n";
    dist << "====================\n";
    
    for (const auto& pair : distribution) {
        double percentage = (double)pair.second / tokens.size() * 100.0;
        dist << pair.first << ": " << pair.second << " (" 
             << std::fixed << std::setprecision(1) << percentage << "%)\n";
    }
    
    return dist.str();
}

bool TokenTable::exportTableToHTML(const CHTL::TokenSequence& tokens, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateCHTLTokenTable(tokens);
    return true;
}

bool TokenTable::exportTableToHTML(const CHTLJS::TokenSequence& tokens, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateCHTLJSTokenTable(tokens);
    return true;
}

std::string TokenTable::generateHTMLHeader() {
    return "<!DOCTYPE html>\n<html>\n<head>\n<style>\n"
           "table { border-collapse: collapse; width: 100%; }\n"
           "th, td { border: 1px solid #ddd; padding: 8px; }\n"
           "th { background-color: #f2f2f2; }\n"
           "</style>\n</head>\n<body>\n";
}

std::string TokenTable::generateHTMLFooter() {
    return "</body>\n</html>";
}

std::string TokenTable::formatTokenRow(const CHTL::Token& token, size_t index) {
    std::ostringstream row;
    row << "<tr>";
    row << "<td>" << index << "</td>";
    row << "<td>" << token.getTypeName() << "</td>";
    row << "<td>" << token.value << "</td>";
    row << "<td>" << token.position.line << "</td>";
    row << "<td>" << token.position.column << "</td>";
    row << "</tr>\n";
    return row.str();
}

std::string TokenTable::formatTokenRow(const CHTLJS::Token& token, size_t index) {
    std::ostringstream row;
    row << "<tr>";
    row << "<td>" << index << "</td>";
    row << "<td>" << token.getTypeName() << "</td>";
    row << "<td>" << token.value << "</td>";
    row << "<td>" << token.position.line << "</td>";
    row << "<td>" << token.position.column << "</td>";
    row << "</tr>\n";
    return row.str();
}

} // namespace Test
} // namespace CHTL