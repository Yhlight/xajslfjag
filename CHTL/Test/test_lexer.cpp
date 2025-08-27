#include <iostream>
#include <cassert>
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"

using namespace CHTL;

void testBasicTokens() {
    std::cout << "Testing basic tokens...\n";
    
    Lexer lexer("div { id: box; }");
    auto tokens = lexer.getFilteredTokens();
    
    assert(tokens.size() >= 6);
    assert(tokens[0].type == TokenType::IDENTIFIER);
    assert(tokens[0].value == "div");
    assert(tokens[1].type == TokenType::LEFT_BRACE);
    assert(tokens[2].type == TokenType::IDENTIFIER);
    assert(tokens[2].value == "id");
    assert(tokens[3].type == TokenType::COLON);
    
    std::cout << "✓ Basic tokens test passed\n";
}

void testKeywords() {
    std::cout << "Testing keywords...\n";
    
    Lexer lexer("[Template] @Style text style script [Custom] [Import] use delete");
    auto tokens = lexer.getFilteredTokens();
    
    assert(tokens[0].type == TokenType::KEYWORD_TEMPLATE);
    assert(tokens[2].type == TokenType::KEYWORD_TEXT);
    assert(tokens[3].type == TokenType::KEYWORD_STYLE);
    assert(tokens[4].type == TokenType::KEYWORD_SCRIPT);
    assert(tokens[5].type == TokenType::KEYWORD_CUSTOM);
    assert(tokens[6].type == TokenType::KEYWORD_IMPORT);
    assert(tokens[7].type == TokenType::KEYWORD_USE);
    assert(tokens[8].type == TokenType::KEYWORD_DELETE);
    
    std::cout << "✓ Keywords test passed\n";
}

void testStrings() {
    std::cout << "Testing string literals...\n";
    
    Lexer lexer(R"("double quotes" 'single quotes' unquoted)");
    auto tokens = lexer.getFilteredTokens();
    
    assert(tokens[0].type == TokenType::STRING_LITERAL);
    assert(tokens[0].value == "double quotes");
    assert(tokens[1].type == TokenType::SINGLE_STRING_LITERAL);
    assert(tokens[1].value == "single quotes");
    assert(tokens[2].type == TokenType::IDENTIFIER);
    assert(tokens[2].value == "unquoted");
    
    std::cout << "✓ String literals test passed\n";
}

void testComments() {
    std::cout << "Testing comments...\n";
    
    std::string code = R"(
// Single line comment
div {
    /* Multi
       line
       comment */
    -- Generate comment
}
)";
    
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    int singleCount = 0, multiCount = 0, generateCount = 0;
    for (const auto& token : tokens) {
        if (token.type == TokenType::COMMENT_SINGLE) singleCount++;
        if (token.type == TokenType::COMMENT_MULTI) multiCount++;
        if (token.type == TokenType::COMMENT_GENERATE) generateCount++;
    }
    
    assert(singleCount == 1);
    assert(multiCount == 1);
    assert(generateCount == 1);
    
    std::cout << "✓ Comments test passed\n";
}

void testComplexCHTL() {
    std::cout << "Testing complex CHTL code...\n";
    
    std::string code = R"(
[Template] @Style DefaultText {
    color: "black";
    line-height: 1.6;
}

div {
    style {
        .box {
            width: 100px;
            height: 100px;
        }
        
        &:hover {
            background-color: red;
        }
    }
    
    text {
        "Hello, CHTL!"
    }
}
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    // 验证关键token存在
    bool hasTemplate = false, hasStyle = false, hasDiv = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::KEYWORD_TEMPLATE) hasTemplate = true;
        if (token.type == TokenType::KEYWORD_STYLE) hasStyle = true;
        if (token.type == TokenType::IDENTIFIER && token.value == "div") hasDiv = true;
    }
    
    assert(hasTemplate);
    assert(hasStyle);
    assert(hasDiv);
    
    std::cout << "✓ Complex CHTL test passed\n";
}

void testNumbers() {
    std::cout << "Testing numbers...\n";
    
    Lexer lexer("123 456.789 1.6 0 100px");
    auto tokens = lexer.getFilteredTokens();
    
    assert(tokens[0].type == TokenType::NUMBER);
    assert(tokens[0].value == "123");
    assert(tokens[1].type == TokenType::NUMBER);
    assert(tokens[1].value == "456.789");
    assert(tokens[2].type == TokenType::NUMBER);
    assert(tokens[2].value == "1.6");
    
    std::cout << "✓ Numbers test passed\n";
}

int main() {
    std::cout << "=== CHTL Lexer Tests ===\n\n";
    
    try {
        testBasicTokens();
        testKeywords();
        testStrings();
        testComments();
        testComplexCHTL();
        testNumbers();
        
        std::cout << "\n✅ All tests passed!\n";
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << "\n";
        return 1;
    }
    
    // 清理
    GlobalMap::destroyInstance();
    
    return 0;
}