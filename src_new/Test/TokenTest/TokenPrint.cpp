#include "TokenPrint.h"
#include <iostream>
#include <iomanip>
#include <fstream>

namespace CHTL {
namespace Test {

void TokenPrint::printCHTLTokens(const CHTL::TokenSequence& tokens) {
    std::cout << "\n=== CHTL Token序列 ===\n";
    std::cout << "Token数量: " << tokens.size() << "\n";
    std::cout << std::setw(5) << "索引" << " | " 
              << std::setw(20) << "类型" << " | "
              << std::setw(15) << "值" << " | "
              << std::setw(10) << "位置" << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << std::setw(5) << i << " | " 
                  << std::setw(20) << formatTokenInfo(tokens[i]) << " | "
                  << std::setw(15) << tokens[i].value << " | "
                  << std::setw(10) << tokens[i].position.toString() << "\n";
    }
}

void TokenPrint::printCHTLJSTokens(const CHTLJS::TokenSequence& tokens) {
    std::cout << "\n=== CHTL JS Token序列 ===\n";
    std::cout << "Token数量: " << tokens.size() << "\n";
    std::cout << std::setw(5) << "索引" << " | " 
              << std::setw(20) << "类型" << " | "
              << std::setw(15) << "值" << " | "
              << std::setw(10) << "位置" << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << std::setw(5) << i << " | " 
                  << std::setw(20) << formatTokenInfo(tokens[i]) << " | "
                  << std::setw(15) << tokens[i].value << " | "
                  << std::setw(10) << tokens[i].position.toString() << "\n";
    }
}

std::string TokenPrint::generateTokenTable(const CHTL::TokenSequence& tokens) {
    std::ostringstream table;
    
    table << "CHTL Token分析表\n";
    table << "================\n";
    table << "总Token数: " << tokens.size() << "\n\n";
    
    table << std::setw(5) << "索引" << " | " 
          << std::setw(20) << "类型" << " | "
          << std::setw(15) << "值" << " | "
          << std::setw(10) << "位置" << "\n";
    table << std::string(60, '-') << "\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        table << std::setw(5) << i << " | " 
              << std::setw(20) << formatTokenInfo(tokens[i]) << " | "
              << std::setw(15) << tokens[i].value << " | "
              << std::setw(10) << tokens[i].position.toString() << "\n";
    }
    
    return table.str();
}

std::string TokenPrint::generateTokenTable(const CHTLJS::TokenSequence& tokens) {
    std::ostringstream table;
    
    table << "CHTL JS Token分析表\n";
    table << "==================\n";
    table << "总Token数: " << tokens.size() << "\n\n";
    
    table << std::setw(5) << "索引" << " | " 
          << std::setw(20) << "类型" << " | "
          << std::setw(15) << "值" << " | "
          << std::setw(10) << "位置" << "\n";
    table << std::string(60, '-') << "\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        table << std::setw(5) << i << " | " 
              << std::setw(20) << formatTokenInfo(tokens[i]) << " | "
              << std::setw(15) << tokens[i].value << " | "
              << std::setw(10) << tokens[i].position.toString() << "\n";
    }
    
    return table.str();
}

bool TokenPrint::exportTokensToFile(const CHTL::TokenSequence& tokens, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateTokenTable(tokens);
    return true;
}

bool TokenPrint::exportTokensToFile(const CHTLJS::TokenSequence& tokens, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateTokenTable(tokens);
    return true;
}

std::string TokenPrint::formatTokenInfo(const CHTL::Token& token) {
    return token.getTypeName();
}

std::string TokenPrint::formatTokenInfo(const CHTLJS::Token& token) {
    return token.getTypeName();
}

} // namespace Test
} // namespace CHTL