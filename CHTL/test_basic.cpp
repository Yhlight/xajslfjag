#include <iostream>
#include <cassert>
#include "CHTLLexer/CHTLToken.h"

using namespace CHTL;

void testTokenTypes() {
    std::cout << "Testing Token Types..." << std::endl;
    
    // 测试Token创建
    Token tok1(TokenType::IDENTIFIER, "myVar", 1, 1, 0);
    assert(tok1.type == TokenType::IDENTIFIER);
    assert(tok1.value == "myVar");
    assert(tok1.line == 1);
    assert(tok1.column == 1);
    
    // 测试Token类型判断
    Token tok2(TokenType::STRING_LITERAL, "\"hello\"", 1, 5, 4);
    assert(tok2.isLiteral());
    assert(!tok2.isOperator());
    
    Token tok3(TokenType::COLON, ":", 1, 10, 9);
    assert(tok3.isOperator());
    assert(!tok3.isLiteral());
    
    Token tok4(TokenType::TEXT, "text", 2, 1, 10);
    assert(tok4.isKeyword());
    
    Token tok5(TokenType::DIV, "div", 3, 1, 15);
    assert(tok5.isHTMLTag());
    
    Token tok6(TokenType::SINGLE_LINE_COMMENT, "// comment", 4, 1, 20);
    assert(tok6.isComment());
    
    std::cout << "Token type tests passed!" << std::endl;
}

void testKeywordMap() {
    std::cout << "Testing Keyword Map..." << std::endl;
    
    // 初始化关键字映射
    initializeKeywordMap();
    
    // 测试CHTL关键字
    assert(keywordMap["text"] == TokenType::TEXT);
    assert(keywordMap["style"] == TokenType::STYLE);
    assert(keywordMap["script"] == TokenType::SCRIPT);
    assert(keywordMap["namespace"] == TokenType::NAMESPACE);
    assert(keywordMap["from"] == TokenType::FROM);
    assert(keywordMap["as"] == TokenType::AS);
    assert(keywordMap["delete"] == TokenType::DELETE);
    
    // 测试HTML标签
    assert(keywordMap["html"] == TokenType::HTML);
    assert(keywordMap["body"] == TokenType::BODY);
    assert(keywordMap["div"] == TokenType::DIV);
    assert(keywordMap["span"] == TokenType::SPAN);
    assert(keywordMap["p"] == TokenType::P);
    assert(keywordMap["a"] == TokenType::A);
    assert(keywordMap["h1"] == TokenType::H1);
    assert(keywordMap["h2"] == TokenType::H2);
    
    // 测试导入类型
    assert(keywordMap["@Chtl"] == TokenType::AT_CHTL);
    assert(keywordMap["@CJmod"] == TokenType::AT_CJMOD);
    assert(keywordMap["@Html"] == TokenType::AT_HTML);
    assert(keywordMap["@Style"] == TokenType::AT_STYLE);
    assert(keywordMap["@JavaScript"] == TokenType::AT_JAVASCRIPT);
    
    std::cout << "Keyword map tests passed!" << std::endl;
}

void testTokenTypeToString() {
    std::cout << "Testing Token Type to String..." << std::endl;
    
    assert(tokenTypeToString(TokenType::END_OF_FILE) == "EOF");
    assert(tokenTypeToString(TokenType::STRING_LITERAL) == "STRING_LITERAL");
    assert(tokenTypeToString(TokenType::IDENTIFIER) == "IDENTIFIER");
    assert(tokenTypeToString(TokenType::TEXT) == "TEXT");
    assert(tokenTypeToString(TokenType::COLON) == ":");
    assert(tokenTypeToString(TokenType::LEFT_BRACE) == "{");
    assert(tokenTypeToString(TokenType::RIGHT_BRACE) == "}");
    assert(tokenTypeToString(TokenType::DOUBLE_BRACE_OPEN) == "{{");
    assert(tokenTypeToString(TokenType::DOUBLE_BRACE_CLOSE) == "}}");
    assert(tokenTypeToString(TokenType::ARROW) == "->");
    assert(tokenTypeToString(TokenType::DIV) == "DIV");
    assert(tokenTypeToString(TokenType::AT_CHTL) == "@Chtl");
    
    std::cout << "Token type to string tests passed!" << std::endl;
}

int main() {
    std::cout << "Running CHTL Basic Tests..." << std::endl;
    std::cout << "===========================" << std::endl;
    
    try {
        testTokenTypes();
        testKeywordMap();
        testTokenTypeToString();
        
        std::cout << "\nAll tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}