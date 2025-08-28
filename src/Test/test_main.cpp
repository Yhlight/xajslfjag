#include <iostream>
#include <cassert>
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLNode/CHTLElementNode.h"
#include "../CHTL/CHTLNode/CHTLTextNode.h"
#include "../CHTLJS/CHTLJSLexer/CHTLJSLexer.h"

void testCHTLLexer() {
    std::cout << "测试CHTL词法分析器..." << std::endl;
    
    auto globalMap = std::make_shared<CHTL::CHTLGlobalMap>();
    CHTL::CHTLLexer lexer(globalMap);
    
    // 测试基本标识符
    lexer.setInput("div");
    auto tokens = lexer.tokenize();
    assert(tokens.size() == 2); // div + EOF
    assert(tokens[0]->type == CHTL::CHTLTokenType::IDENTIFIER);
    assert(tokens[0]->text == "div");
    
    // 测试关键字
    lexer.setInput("text style script");
    tokens = lexer.tokenize();
    assert(tokens.size() == 4); // text + style + script + EOF
    assert(tokens[0]->type == CHTL::CHTLTokenType::TEXT);
    assert(tokens[1]->type == CHTL::CHTLTokenType::STYLE);
    assert(tokens[2]->type == CHTL::CHTLTokenType::SCRIPT);
    
    // 测试字符串字面量
    lexer.setInput("\"Hello World\"");
    tokens = lexer.tokenize();
    assert(tokens.size() == 2); // string + EOF
    assert(tokens[0]->type == CHTL::CHTLTokenType::STRING_LITERAL);
    assert(tokens[0]->text == "Hello World");
    
    // 测试选择器引用
    lexer.setInput("{{box}} {{.container}} {{#main}}");
    tokens = lexer.tokenize();
    assert(tokens.size() == 4); // 3 selectors + EOF
    assert(tokens[0]->type == CHTL::CHTLTokenType::SELECTOR_REF);
    assert(tokens[1]->type == CHTL::CHTLTokenType::CLASS_REF);
    assert(tokens[2]->type == CHTL::CHTLTokenType::ID_REF);
    
    std::cout << "CHTL词法分析器测试通过!" << std::endl;
}

void testCHTLJSLexer() {
    std::cout << "测试CHTL JS词法分析器..." << std::endl;
    
    auto globalMap = std::make_shared<CHTLJS::CHTLJSGlobalMap>();
    CHTLJS::CHTLJSLexer lexer(globalMap);
    
    // 测试CHTL JS关键字
    lexer.setInput("module listen animate vir");
    auto tokens = lexer.tokenize();
    assert(tokens.size() == 5); // 4 keywords + EOF
    assert(tokens[0]->type == CHTLJS::CHTLJSTokenType::MODULE);
    assert(tokens[1]->type == CHTLJS::CHTLJSTokenType::LISTEN);
    assert(tokens[2]->type == CHTLJS::CHTLJSTokenType::ANIMATE);
    assert(tokens[3]->type == CHTLJS::CHTLJSTokenType::VIR);
    
    // 测试JavaScript关键字
    lexer.setInput("function const let var");
    tokens = lexer.tokenize();
    assert(tokens.size() == 5); // 4 keywords + EOF
    assert(tokens[0]->type == CHTLJS::CHTLJSTokenType::FUNCTION);
    assert(tokens[1]->type == CHTLJS::CHTLJSTokenType::CONST);
    assert(tokens[2]->type == CHTLJS::CHTLJSTokenType::LET);
    assert(tokens[3]->type == CHTLJS::CHTLJSTokenType::VAR);
    
    // 测试箭头操作符
    lexer.setInput("-> &->");
    tokens = lexer.tokenize();
    assert(tokens.size() == 3); // 2 operators + EOF
    assert(tokens[0]->type == CHTLJS::CHTLJSTokenType::ARROW);
    assert(tokens[1]->type == CHTLJS::CHTLJSTokenType::AMPERSAND_ARROW);
    
    std::cout << "CHTL JS词法分析器测试通过!" << std::endl;
}

void testCHTLNodes() {
    std::cout << "测试CHTL AST节点..." << std::endl;
    
    // 测试元素节点
    auto divNode = std::make_shared<CHTL::CHTLElementNode>("div");
    divNode->setId("main-container");
    divNode->addClass("container");
    divNode->addClass("active");
    
    assert(divNode->getTagName() == "div");
    assert(divNode->getId() == "main-container");
    assert(divNode->hasClass("container"));
    assert(divNode->hasClass("active"));
    assert(!divNode->hasClass("hidden"));
    
    // 测试文本节点
    auto textNode = std::make_shared<CHTL::CHTLTextNode>("Hello, CHTL!");
    assert(textNode->getText() == "Hello, CHTL!");
    assert(!textNode->isEmpty());
    assert(textNode->getLength() == textNode->getText().length());
    
    // 测试父子关系
    divNode->addChild(textNode);
    assert(divNode->getChildCount() == 1);
    assert(divNode->getChild(0) == textNode);
    assert(textNode->getParent() == divNode);
    
    // 测试HTML输出
    std::string html = divNode->toHTML();
    assert(html.find("<div") != std::string::npos);
    assert(html.find("id=\"main-container\"") != std::string::npos);
    assert(html.find("class=\"container active\"") != std::string::npos);
    assert(html.find("Hello, CHTL!") != std::string::npos);
    assert(html.find("</div>") != std::string::npos);
    
    std::cout << "CHTL AST节点测试通过!" << std::endl;
}

void testModularArchitecture() {
    std::cout << "测试模块化架构..." << std::endl;
    
    // 验证CHTL和CHTL JS的完全分离
    auto chtlGlobalMap = std::make_shared<CHTL::CHTLGlobalMap>();
    auto chtljsGlobalMap = std::make_shared<CHTLJS::CHTLJSGlobalMap>();
    
    // 两个系统应该有不同的关键字映射
    assert(chtlGlobalMap->getKeywordMap().isKeyword("text"));
    assert(chtlGlobalMap->getKeywordMap().isKeyword("style"));
    assert(!chtlGlobalMap->getKeywordMap().isKeyword("module"));
    assert(!chtlGlobalMap->getKeywordMap().isKeyword("listen"));
    
    assert(chtljsGlobalMap->getKeywordMap().isKeyword("module"));
    assert(chtljsGlobalMap->getKeywordMap().isKeyword("listen"));
    assert(!chtljsGlobalMap->getKeywordMap().isKeyword("text"));
    assert(!chtljsGlobalMap->getKeywordMap().isKeyword("style"));
    
    // 验证CHTL JS特有的事件映射
    assert(chtljsGlobalMap->getEventMap().isValidDOMEvent("click"));
    assert(chtljsGlobalMap->getEventMap().isMouseEvent("click"));
    assert(chtljsGlobalMap->getEventMap().isKeyboardEvent("keydown"));
    
    std::cout << "模块化架构测试通过!" << std::endl;
}

int main() {
    std::cout << "CHTL项目单元测试" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    try {
        testCHTLLexer();
        testCHTLJSLexer();
        testCHTLNodes();
        testModularArchitecture();
        
        std::cout << "\n=======================================" << std::endl;
        std::cout << "所有测试通过! CHTL项目架构正确!" << std::endl;
        std::cout << "✓ CHTL编译器独立运行正常" << std::endl;
        std::cout << "✓ CHTL JS编译器独立运行正常" << std::endl;
        std::cout << "✓ 模块化架构完全分离，互不干涉" << std::endl;
        std::cout << "✓ AST节点系统工作正常" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}