#include <iostream>
#include <cassert>
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"

using namespace CHTL;

void testTemplateDeclaration() {
    std::cout << "Testing template declarations...\n";
    
    std::string code = R"(
[Template] @Style DefaultText {
    color: black;
    line-height: 1.6;
}

[Template] @Element Card {
    div {
        style {
            padding: 20px;
        }
    }
}

[Template] @Var ThemeColors {
    primary: #007bff;
    secondary: #6c757d;
}
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    // 验证关键字正确识别
    int templateCount = 0;
    int atStyleCount = 0;
    int atElementCount = 0;
    int atVarCount = 0;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type == TokenType::KEYWORD_TEMPLATE) {
            templateCount++;
        }
        if (tokens[i].type == TokenType::IDENTIFIER && tokens[i].value == "@Style") {
            atStyleCount++;
        }
        if (tokens[i].type == TokenType::IDENTIFIER && tokens[i].value == "@Element") {
            atElementCount++;
        }
        if (tokens[i].type == TokenType::IDENTIFIER && tokens[i].value == "@Var") {
            atVarCount++;
        }
    }
    
    assert(templateCount == 3);
    assert(atStyleCount >= 1);
    assert(atElementCount >= 1);
    assert(atVarCount >= 1);
    
    std::cout << "✓ Template declaration test passed\n";
}

void testCustomDeclaration() {
    std::cout << "Testing custom declarations...\n";
    
    std::string code = R"(
[Custom] @Style TextSet {
    color,
    font-size;
}

[Custom] @Element Box {
    div {
        delete @Style WhiteText;
    }
}
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    int customCount = 0;
    int deleteCount = 0;
    
    for (const auto& token : tokens) {
        if (token.type == TokenType::KEYWORD_CUSTOM) {
            customCount++;
        }
        if (token.type == TokenType::KEYWORD_DELETE) {
            deleteCount++;
        }
    }
    
    assert(customCount == 2);
    assert(deleteCount == 1);
    
    std::cout << "✓ Custom declaration test passed\n";
}

void testLocalStyleBlock() {
    std::cout << "Testing local style blocks...\n";
    
    std::string code = R"(
div {
    style {
        .box {
            width: 100px;
            height: 100px;
        }
        
        &:hover {
            background-color: red;
        }
        
        #main-box {
            position: relative;
        }
    }
}
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    // 查找关键标记
    bool hasStyleKeyword = false;
    bool hasClassSelector = false;
    bool hasIdSelector = false;
    bool hasAmpersand = false;
    
    for (const auto& token : tokens) {
        if (token.type == TokenType::KEYWORD_STYLE) {
            hasStyleKeyword = true;
        }
        if (token.type == TokenType::DOT && !tokens.empty()) {
            hasClassSelector = true;
        }
        if (token.type == TokenType::HASH) {
            hasIdSelector = true;
        }
        if (token.type == TokenType::AMPERSAND) {
            hasAmpersand = true;
        }
    }
    
    assert(hasStyleKeyword);
    assert(hasClassSelector);
    assert(hasIdSelector);
    assert(hasAmpersand);
    
    std::cout << "✓ Local style block test passed\n";
}

void testImportStatement() {
    std::cout << "Testing import statements...\n";
    
    std::string code = R"(
[Import] @Html from "header.html" as Header;
[Import] @Style from styles/main.css as MainStyles;
[Import] [Custom] @Element Card from components.chtl;
[Import] @Chtl from chtholly;
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    int importCount = 0;
    int fromCount = 0;
    int asCount = 0;
    
    for (const auto& token : tokens) {
        if (token.type == TokenType::KEYWORD_IMPORT) {
            importCount++;
        }
        if (token.type == TokenType::KEYWORD_FROM) {
            fromCount++;
        }
        if (token.type == TokenType::KEYWORD_AS) {
            asCount++;
        }
    }
    
    assert(importCount == 4);
    assert(fromCount == 4);
    assert(asCount == 2);  // 只有前两个有as
    
    std::cout << "✓ Import statement test passed\n";
}

void testOperators() {
    std::cout << "Testing operators...\n";
    
    std::string code = R"(
div {
    except span, [Custom] @Element Box;
    
    insert after div[0] {
        span {
            text {
                "Inserted content"
            }
        }
    }
    
    delete @Style WhiteText;
}
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    int exceptCount = 0;
    int insertCount = 0;
    int deleteCount = 0;
    int afterCount = 0;
    
    for (const auto& token : tokens) {
        if (token.type == TokenType::KEYWORD_EXCEPT) exceptCount++;
        if (token.type == TokenType::KEYWORD_INSERT) insertCount++;
        if (token.type == TokenType::KEYWORD_DELETE) deleteCount++;
        if (token.type == TokenType::KEYWORD_AFTER) afterCount++;
    }
    
    assert(exceptCount == 1);
    assert(insertCount == 1);
    assert(deleteCount == 1);
    assert(afterCount == 1);
    
    std::cout << "✓ Operators test passed\n";
}

void testVariableUsage() {
    std::cout << "Testing variable usage...\n";
    
    std::string code = R"(
[Template] @Var ThemeColor {
    tableColor: rgb(255, 192, 203);
}

div {
    style {
        background-color: ThemeColor(tableColor);
        color: ThemeColor(tableColor = rgb(145, 155, 200));
    }
}
)";
    
    Lexer lexer(code);
    auto tokens = lexer.getFilteredTokens();
    
    // 验证变量使用的语法
    bool hasVarKeyword = false;
    bool hasParentheses = false;
    
    for (const auto& token : tokens) {
        if (token.type == TokenType::IDENTIFIER && token.value == "@Var") {
            hasVarKeyword = true;
        }
        if (token.type == TokenType::LEFT_PAREN) {
            hasParentheses = true;
        }
    }
    
    assert(hasVarKeyword);
    assert(hasParentheses);
    
    std::cout << "✓ Variable usage test passed\n";
}

int main() {
    std::cout << "=== CHTL Features Tests ===\n\n";
    
    try {
        testTemplateDeclaration();
        testCustomDeclaration();
        testLocalStyleBlock();
        testImportStatement();
        testOperators();
        testVariableUsage();
        
        std::cout << "\n✅ All CHTL feature tests passed!\n";
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << "\n";
        return 1;
    }
    
    // 清理
    GlobalMap::destroyInstance();
    
    return 0;
}