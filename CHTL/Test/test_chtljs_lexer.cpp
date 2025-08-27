#include <iostream>
#include <cassert>
#include "../CHTLJS/CHTLJSLexer/Lexer.h"
#include "../CHTLJS/CHTLJSLexer/GlobalMap.h"

using namespace CHTLJS;

void testBasicTokens() {
    std::cout << "Testing basic CHTL JS tokens...\n";
    
    Lexer lexer("const x = 42;");
    auto tokens = lexer.getFilteredTokens();
    
    assert(tokens.size() >= 5);
    assert(tokens[0].type == TokenType::KEYWORD_CONST);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[1].value == "x");
    assert(tokens[2].type == TokenType::ASSIGN);
    assert(tokens[3].type == TokenType::NUMBER_LITERAL);
    assert(tokens[3].value == "42");
    assert(tokens[4].type == TokenType::SEMICOLON);
    
    std::cout << "✓ Basic tokens test passed\n";
}

void testCHTLSelectors() {
    std::cout << "Testing CHTL selectors...\n";
    
    Lexer lexer("{{box}} {{.box}} {{#box}} {{button[0]}}");
    auto tokens = lexer.getFilteredTokens();
    
    assert(tokens[0].type == TokenType::DOUBLE_LEFT_BRACE);
    assert(tokens[0].value == "box");
    assert(tokens[1].type == TokenType::DOUBLE_LEFT_BRACE);
    assert(tokens[1].value == ".box");
    assert(tokens[2].type == TokenType::DOUBLE_LEFT_BRACE);
    assert(tokens[2].value == "#box");
    assert(tokens[3].type == TokenType::DOUBLE_LEFT_BRACE);
    assert(tokens[3].value == "button[0]");
    
    std::cout << "✓ CHTL selectors test passed\n";
}

void testArrowOperators() {
    std::cout << "Testing arrow operators...\n";
    
    Lexer lexer("obj->method() {{box}} &-> click func => result");
    auto tokens = lexer.getFilteredTokens();
    
    bool hasArrow = false, hasBindArrow = false, hasDoubleArrow = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::ARROW) hasArrow = true;
        if (token.type == TokenType::BIND_ARROW) hasBindArrow = true;
        if (token.type == TokenType::DOUBLE_ARROW) hasDoubleArrow = true;
    }
    
    assert(hasArrow);
    assert(hasBindArrow);
    assert(hasDoubleArrow);
    
    std::cout << "✓ Arrow operators test passed\n";
}

void testListenExpression() {
    std::cout << "Testing listen expression...\n";
    
    std::string code = R"(
{{box}}->listen {
    click: () => {
        console.log('clicked');
    },
    mouseenter: handleMouseEnter
};
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    bool hasListen = false, hasArrow = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::KEYWORD_LISTEN) hasListen = true;
        if (token.type == TokenType::ARROW) hasArrow = true;
    }
    
    assert(hasListen);
    assert(hasArrow);
    
    std::cout << "✓ Listen expression test passed\n";
}

void testAnimateExpression() {
    std::cout << "Testing animate expression...\n";
    
    std::string code = R"(
const anim = animate {
    target: {{.box}},
    duration: 1000,
    easing: ease-in-out,
    begin: {
        opacity: 0
    },
    end: {
        opacity: 1
    }
};
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    bool hasAnimate = false, hasTarget = false, hasDuration = false, hasEasing = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::KEYWORD_ANIMATE) hasAnimate = true;
        if (token.type == TokenType::KEYWORD_TARGET) hasTarget = true;
        if (token.type == TokenType::KEYWORD_DURATION) hasDuration = true;
        if (token.type == TokenType::KEYWORD_EASING) hasEasing = true;
    }
    
    assert(hasAnimate);
    assert(hasTarget);
    assert(hasDuration);
    assert(hasEasing);
    
    std::cout << "✓ Animate expression test passed\n";
}

void testVirDeclaration() {
    std::cout << "Testing vir declaration...\n";
    
    std::string code = R"(
vir Test = listen {
    click: () => {
        console.log('Virtual object');
    }
};

Test->click();
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    assert(tokens[0].type == TokenType::KEYWORD_VIR);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[1].value == "Test");
    
    std::cout << "✓ Vir declaration test passed\n";
}

void testAllOperators() {
    std::cout << "Testing all operators...\n";
    
    std::string code = "+ - * / % += -= *= /= %= == != === !== < > <= >= && || ! & | ^ ~ << >> >>> ++ -- ?";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    // 验证所有操作符都被正确识别
    assert(tokens.size() >= 29);
    for (const auto& token : tokens) {
        assert(token.isOperator());
    }
    
    std::cout << "✓ All operators test passed\n";
}

int main() {
    std::cout << "=== CHTL JS Lexer Tests ===\n\n";
    
    try {
        testBasicTokens();
        testCHTLSelectors();
        testArrowOperators();
        testListenExpression();
        testAnimateExpression();
        testVirDeclaration();
        testAllOperators();
        
        std::cout << "\n✅ All tests passed!\n";
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << "\n";
        return 1;
    }
    
    // 清理
    GlobalMap::destroyInstance();
    
    return 0;
}