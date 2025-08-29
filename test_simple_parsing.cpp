#include "src/Util/Common.h"
#include "src/CHTL/CHTLLexer/Token.h"
#include "src/CHTL/CHTLLexer/GlobalMap.h"
#include <iostream>

int main() {
    using namespace CHTL;
    
    std::cout << "Testing Token Types and Basic Functionality:\n";
    std::cout << "===========================================\n";
    
    // 测试Token类型
    Token textToken(TokenType::TEXT, "text");
    std::cout << "Token: " << textToken.toString() << "\n";
    
    Token elementToken(TokenType::HTML_ELEMENT, "div");
    std::cout << "Token: " << elementToken.toString() << "\n";
    
    Token templateToken(TokenType::TEMPLATE, "[Template]");
    std::cout << "Token: " << templateToken.toString() << "\n";
    
    // 测试关键字识别
    std::cout << "\nKeyword Recognition Test:\n";
    std::cout << "text is keyword: " << KeywordMap::isKeyword("text") << "\n";
    std::cout << "div is HTML element: " << HTMLElementMap::isHTMLElement("div") << "\n";
    std::cout << "listen is CHTL JS keyword: " << KeywordMap::isCHTLJSKeyword("listen") << "\n";
    
    // 测试Token类型转换
    std::cout << "\nToken Type to String Test:\n";
    std::cout << "TEXT: " << Token::tokenTypeToString(TokenType::TEXT) << "\n";
    std::cout << "TEMPLATE: " << Token::tokenTypeToString(TokenType::TEMPLATE) << "\n";
    std::cout << "MODULE: " << Token::tokenTypeToString(TokenType::MODULE) << "\n";
    std::cout << "LISTEN: " << Token::tokenTypeToString(TokenType::LISTEN) << "\n";
    
    // 测试关键字类型获取
    std::cout << "\nKeyword Type Recognition:\n";
    TokenType textType = KeywordMap::getKeywordType("text");
    std::cout << "text type: " << Token::tokenTypeToString(textType) << "\n";
    
    TokenType moduleType = KeywordMap::getKeywordType("module");
    std::cout << "module type: " << Token::tokenTypeToString(moduleType) << "\n";
    
    TokenType listenType = KeywordMap::getKeywordType("listen");
    std::cout << "listen type: " << Token::tokenTypeToString(listenType) << "\n";
    
    TokenType configType = KeywordMap::getKeywordType("@Config");
    std::cout << "@Config type: " << Token::tokenTypeToString(configType) << "\n";
    
    // 测试HTML元素识别
    std::cout << "\nHTML Element Recognition:\n";
    StringVector htmlElements = {"div", "span", "p", "h1", "html", "body", "unknown"};
    for (const auto& elem : htmlElements) {
        std::cout << elem << " is HTML element: " << HTMLElementMap::isHTMLElement(elem) << "\n";
    }
    
    // 测试CSS属性识别
    std::cout << "\nCSS Property Recognition:\n";
    StringVector cssProps = {"color", "background", "width", "height", "unknown-prop"};
    for (const auto& prop : cssProps) {
        std::cout << prop << " is CSS property: " << CSSPropertyMap::isCSSProperty(prop) << "\n";
    }
    
    std::cout << "\nAll tests completed successfully!\n";
    
    return 0;
}