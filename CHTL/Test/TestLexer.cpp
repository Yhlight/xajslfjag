#include <iostream>
#include <string>
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"

using namespace CHTL;

void testBasicTokens() {
    std::cout << "\n=== Testing Basic Tokens ===" << std::endl;
    
    std::string code = R"(
div
{
    id: box;
    class: "welcome";
    
    text
    {
        "Hello World"
    }
    
    style
    {
        width: 100px;
        height: 200px;
        background-color: red;
    }
}
)";
    
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.scanTokens();
    
    for (const auto& token : tokens) {
        if (token.getType() != TokenType::WHITESPACE && 
            token.getType() != TokenType::NEWLINE) {
            std::cout << token << std::endl;
        }
    }
}

void testComments() {
    std::cout << "\n=== Testing Comments ===" << std::endl;
    
    std::string code = R"(
// 单行注释
div
{
    /* 多行
       注释 */
    -- 生成器注释
    text { "内容" }
}
)";
    
    Lexer lexer(code);
    lexer.scanTokens();
    lexer.printTokens();
}

void testBracketKeywords() {
    std::cout << "\n=== Testing Bracket Keywords ===" << std::endl;
    
    std::string code = R"(
[Template] @Style DefaultText
{
    color: "black";
}

[Custom] @Element Box
{
    div { }
}

[Import] @Chtl from "module.chtl"
)";
    
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.scanTokens();
    
    for (const auto& token : tokens) {
        if (token.isBracketKeyword() || token.isAtType()) {
            std::cout << token << std::endl;
        }
    }
}

void testStringLiterals() {
    std::cout << "\n=== Testing String Literals ===" << std::endl;
    
    std::string code = R"(
text
{
    "双引号字符串"
    '单引号字符串'
    这是无修饰字面量
}
)";
    
    Lexer lexer(code);
    lexer.scanTokens();
    lexer.printTokens();
}

void testOperators() {
    std::cout << "\n=== Testing Operators ===" << std::endl;
    
    std::string code = R"(
div
{
    style
    {
        .box
        {
            &:hover
            {
                color: red;
            }
        }
    }
    
    script
    {
        {{box}}->addEventListener();
    }
}
)";
    
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.scanTokens();
    
    for (const auto& token : tokens) {
        if (token.isOperator()) {
            std::cout << token << std::endl;
        }
    }
}

void testGlobalMap() {
    std::cout << "\n=== Testing GlobalMap ===" << std::endl;
    
    GlobalMap& globalMap = GlobalMap::getInstance();
    globalMap.printAllMappings();
}

int main() {
    std::cout << "=== CHTL Lexer Test ===" << std::endl;
    
    // 测试GlobalMap
    testGlobalMap();
    
    // 测试基本Token
    testBasicTokens();
    
    // 测试注释
    testComments();
    
    // 测试方括号关键字
    testBracketKeywords();
    
    // 测试字符串字面量
    testStringLiterals();
    
    // 测试操作符
    testOperators();
    
    return 0;
}