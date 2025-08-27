#include <iostream>
#include <cassert>
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"

using namespace CHTL;

void testBasicUnquotedLiterals() {
    std::cout << "Testing basic unquoted literals...\n";
    
    // 测试text中的无引号字面量
    Lexer lexer1("text { 这是一段中文文本 }");
    auto tokens1 = lexer1.getFilteredTokens();
    
    bool foundUnquoted = false;
    for (const auto& token : tokens1) {
        if (token.type == TokenType::UNQUOTED_LITERAL) {
            foundUnquoted = true;
            assert(token.value == "这是一段中文文本");
        }
    }
    assert(foundUnquoted);
    
    // 测试属性值的无引号字面量
    Lexer lexer2("div { id: myBox; class: welcome-text; }");
    auto tokens2 = lexer2.getFilteredTokens();
    
    int unquotedCount = 0;
    for (const auto& token : tokens2) {
        if (token.type == TokenType::UNQUOTED_LITERAL) {
            unquotedCount++;
        }
    }
    assert(unquotedCount == 2);  // myBox 和 welcome-text
    
    std::cout << "✓ Basic unquoted literals test passed\n";
}

void testUnquotedInStyle() {
    std::cout << "Testing unquoted literals in style...\n";
    
    Lexer lexer("style { color: red; background-color: #ff0000; font-size: 16px; }");
    auto tokens = lexer.getFilteredTokens();
    
    // 验证CSS值被正确识别为无引号字面量
    int unquotedCount = 0;
    for (const auto& token : tokens) {
        if (token.type == TokenType::UNQUOTED_LITERAL) {
            unquotedCount++;
        }
    }
    assert(unquotedCount >= 3);  // red, #ff0000, 16px
    
    std::cout << "✓ Unquoted in style test passed\n";
}

void testMixedLiterals() {
    std::cout << "Testing mixed literals...\n";
    
    std::string code = R"(
div {
    id: box;
    title: "这是一个标题";
    class: 'my-class';
    data-value: 123;
    
    text {
        Hello World
        "引号字符串"
        中文内容测试
    }
}
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    int stringCount = 0, singleStringCount = 0, unquotedCount = 0;
    for (const auto& token : tokens) {
        if (token.type == TokenType::STRING_LITERAL) stringCount++;
        if (token.type == TokenType::SINGLE_STRING_LITERAL) singleStringCount++;
        if (token.type == TokenType::UNQUOTED_LITERAL) unquotedCount++;
    }
    
    assert(stringCount >= 2);      // "这是一个标题" 和 "引号字符串"
    assert(singleStringCount >= 1); // 'my-class'
    assert(unquotedCount >= 3);    // box, 123, Hello World, 中文内容测试
    
    std::cout << "✓ Mixed literals test passed\n";
}

void testUnquotedEdgeCases() {
    std::cout << "Testing unquoted edge cases...\n";
    
    // 测试各种边界情况
    Lexer lexer1("attr: value-with-dash;");
    auto tokens1 = lexer1.getFilteredTokens();
    bool found1 = false;
    for (const auto& token : tokens1) {
        if (token.type == TokenType::UNQUOTED_LITERAL && token.value == "value-with-dash") {
            found1 = true;
        }
    }
    assert(found1);
    
    // 测试数字开头的无引号字面量
    Lexer lexer2("font-weight: 700;");
    auto tokens2 = lexer2.getFilteredTokens();
    bool foundNumber = false;
    for (const auto& token : tokens2) {
        if (token.type == TokenType::NUMBER && token.value == "700") {
            foundNumber = true;
        }
    }
    assert(foundNumber);
    
    std::cout << "✓ Edge cases test passed\n";
}

void testCEEquivalence() {
    std::cout << "Testing CE (Colon = Equal) equivalence...\n";
    
    // 测试冒号
    Lexer lexer1("div { id: box; }");
    auto tokens1 = lexer1.getFilteredTokens();
    
    // 测试等号（CE对等式）
    Lexer lexer2("div { id = box; }");
    auto tokens2 = lexer2.getFilteredTokens();
    
    // 两种写法应该产生相似的token序列
    bool hasColon = false, hasEquals = false;
    for (const auto& token : tokens1) {
        if (token.type == TokenType::COLON) hasColon = true;
    }
    for (const auto& token : tokens2) {
        if (token.type == TokenType::EQUALS) hasEquals = true;
    }
    
    assert(hasColon);
    assert(hasEquals);
    
    std::cout << "✓ CE equivalence test passed\n";
}

int main() {
    std::cout << "=== CHTL Unquoted Literals Tests ===\n\n";
    
    try {
        testBasicUnquotedLiterals();
        testUnquotedInStyle();
        testMixedLiterals();
        testUnquotedEdgeCases();
        testCEEquivalence();
        
        std::cout << "\n✅ All unquoted literal tests passed!\n";
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << "\n";
        return 1;
    }
    
    // 清理
    GlobalMap::destroyInstance();
    
    return 0;
}