#include "Lexer.h"
#include "Token.h"
#include "Type.h"
#include "GlobalMap.h"
#include <iostream>
#include <string>

using namespace CHTL;

void testBasicTokenization() {
    std::cout << "=== Testing Basic Tokenization ===" << std::endl;
    
    std::string source = R"(
div
{
    id: box;
    class: "container";
    
    text
    {
        "Hello World"
    }
    
    style
    {
        width: 100px;
        height: 200px;
        .active
        {
            background-color: red;
        }
    }
}
)";
    
    Lexer lexer(source);
    TokenSequence tokens = lexer.tokenize();
    
    std::cout << "Generated " << tokens.size() << " tokens:" << std::endl;
    
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
    
    if (lexer.hasErrors()) {
        std::cout << "\nErrors found:" << std::endl;
        for (const auto& error : lexer.getErrors()) {
            std::cout << "Error at " << error.position.line << ":" << error.position.column 
                      << " - " << error.message << std::endl;
        }
    }
}

void testTemplateAndCustom() {
    std::cout << "\n=== Testing Template and Custom Syntax ===" << std::endl;
    
    std::string source = R"(
[Template] @Style DefaultText
{
    color: black;
    font-size: 16px;
}

[Custom] @Element Box
{
    div
    {
        @Style DefaultText;
        
        style
        {
            .container
            {
                width: 300px;
            }
        }
    }
}
)";
    
    Lexer lexer(source);
    TokenSequence tokens = lexer.tokenize();
    
    std::cout << "Generated " << tokens.size() << " tokens:" << std::endl;
    
    for (const auto& token : tokens) {
        if (token.type != TokenType::WHITESPACE && token.type != TokenType::NEWLINE) {
            std::cout << token.toString() << std::endl;
        }
    }
    
    if (lexer.hasErrors()) {
        std::cout << "\nErrors found:" << std::endl;
        for (const auto& error : lexer.getErrors()) {
            std::cout << "Error at " << error.position.line << ":" << error.position.column 
                      << " - " << error.message << std::endl;
        }
    }
}

void testComments() {
    std::cout << "\n=== Testing Comments ===" << std::endl;
    
    std::string source = R"(
// 这是行注释
div
{
    /* 这是块注释 */
    id: test;
    
    -- 这是生成器注释
    style
    {
        color: red;
    }
}
)";
    
    Lexer lexer(source);
    TokenSequence tokens = lexer.tokenize();
    
    std::cout << "Generated " << tokens.size() << " tokens:" << std::endl;
    
    for (const auto& token : tokens) {
        if (token.isComment() || token.type == TokenType::HTML_ELEMENT || 
            token.type == TokenType::IDENTIFIER) {
            std::cout << token.toString() << std::endl;
        }
    }
}

void testGlobalMap() {
    std::cout << "\n=== Testing GlobalMap ===" << std::endl;
    
    GlobalMap& globalMap = GlobalMap::getInstance();
    
    // 测试关键字检查
    std::cout << "Testing keywords:" << std::endl;
    std::cout << "  'text' is keyword: " << globalMap.isKeyword("text") << std::endl;
    std::cout << "  'style' is keyword: " << globalMap.isKeyword("style") << std::endl;
    std::cout << "  'random' is keyword: " << globalMap.isKeyword("random") << std::endl;
    
    // 测试HTML元素检查
    std::cout << "\nTesting HTML elements:" << std::endl;
    std::cout << "  'div' is HTML element: " << globalMap.isHtmlElement("div") << std::endl;
    std::cout << "  'span' is HTML element: " << globalMap.isHtmlElement("span") << std::endl;
    std::cout << "  'customtag' is HTML element: " << globalMap.isHtmlElement("customtag") << std::endl;
    
    // 测试CSS属性检查
    std::cout << "\nTesting CSS properties:" << std::endl;
    std::cout << "  'color' is CSS property: " << globalMap.isCssProperty("color") << std::endl;
    std::cout << "  'width' is CSS property: " << globalMap.isCssProperty("width") << std::endl;
    std::cout << "  'invalid-prop' is CSS property: " << globalMap.isCssProperty("invalid-prop") << std::endl;
    
    // 测试配置
    std::cout << "\nTesting configuration:" << std::endl;
    std::cout << "  DEBUG_MODE: " << globalMap.getConfigBool("DEBUG_MODE") << std::endl;
    std::cout << "  INDEX_INITIAL_COUNT: " << globalMap.getConfigInt("INDEX_INITIAL_COUNT") << std::endl;
}

int main() {
    std::cout << "CHTL Lexer Test Program" << std::endl;
    std::cout << "========================" << std::endl;
    
    try {
        testGlobalMap();
        testBasicTokenization();
        testTemplateAndCustom();
        testComments();
        
        std::cout << "\n=== All tests completed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}