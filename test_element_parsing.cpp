#include "src/CHTL/CHTLParser/Parser.h"
#include "src/CHTL/CHTLLexer/Lexer.h"
#include <iostream>

void testElementParsing() {
    using namespace CHTL;
    
    String testCode = R"(
        // 测试基础元素节点和属性
        div
        {
            id: main-container;
            class: "container flex-row";
            data-test: test-value;
            
            // 测试嵌套元素
            span
            {
                class: highlight;
                style: "color: red; font-weight: bold;";
                
                text
                {
                    这是嵌套文本
                }
            }
            
            // 测试局部样式块
            style
            {
                // 内联样式
                width: 100%;
                height: auto;
                display: flex;
                
                // 类选择器
                .highlight
                {
                    background-color: yellow;
                    padding: 5px;
                }
                
                // ID选择器
                #main-container
                {
                    border: 1px solid #ddd;
                    border-radius: 4px;
                }
                
                // 伪类选择器
                &:hover
                {
                    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
                }
                
                // 伪元素选择器
                &::before
                {
                    content: "Prefix: ";
                    font-weight: bold;
                }
                
                // 子选择器
                span
                {
                    margin: 0 5px;
                }
                
                span:first-child
                {
                    margin-left: 0;
                }
            }
            
            // 测试text节点
            text
            {
                "这是带引号的文本内容"
            }
            
            text
            {
                这是无引号的字面量文本
            }
        }
        
        // 测试单标签元素
        img
        {
            src: "./test.jpg";
            alt: 测试图片;
            width: 200;
            height: 150;
        }
        
        // 测试自闭合元素
        input
        {
            type: text;
            placeholder: "请输入内容";
            required: true;
        }
        
        // 测试复杂嵌套结构
        form
        {
            id: test-form;
            method: post;
            action: "/submit";
            
            fieldset
            {
                legend
                {
                    text { 用户信息 }
                }
                
                label
                {
                    for: username;
                    text { 用户名: }
                }
                
                input
                {
                    id: username;
                    name: username;
                    type: text;
                    required: true;
                }
                
                label
                {
                    for: email;
                    text { 邮箱: }
                }
                
                input
                {
                    id: email;
                    name: email;
                    type: email;
                    required: true;
                }
            }
            
            // 表单样式
            style
            {
                display: flex;
                flex-direction: column;
                gap: 10px;
                
                fieldset
                {
                    border: 1px solid #ccc;
                    padding: 15px;
                    border-radius: 5px;
                }
                
                label
                {
                    font-weight: bold;
                    margin-bottom: 5px;
                }
                
                input[type="text"], input[type="email"]
                {
                    padding: 8px;
                    border: 1px solid #ddd;
                    border-radius: 3px;
                }
                
                input:focus
                {
                    outline: none;
                    border-color: #007bff;
                    box-shadow: 0 0 0 2px rgba(0, 123, 255, 0.25);
                }
            }
        }
    )";
    
    try {
        auto lexer = std::make_unique<Lexer>(testCode);
        Parser parser(std::move(lexer));
        
        std::cout << "Element Parsing Test:\n";
        std::cout << "====================\n";
        
        auto rootNode = parser.parseDocument();
        
        if (parser.hasErrors()) {
            std::cout << "Parsing errors found:\n";
            for (const auto& error : parser.getErrors()) {
                std::cout << "Error: " << error.toString() << "\n";
            }
        } else {
            std::cout << "Parsing successful!\n\n";
            
            // 验证解析结果
            std::cout << "Parsed AST Structure:\n";
            if (rootNode) {
                rootNode->dumpTree();
                
                // 查找特定元素进行验证
                auto divElements = rootNode->findDescendants(NodeType::ELEMENT);
                std::cout << "\nFound " << divElements.size() << " element nodes\n";
                
                for (auto* element : divElements) {
                    ElementNode* elemNode = static_cast<ElementNode*>(element);
                    std::cout << "Element: " << elemNode->getTagName() 
                              << ", Attributes: " << elemNode->getAttributeNames().size() << "\n";
                    
                    // 检查属性
                    for (const auto& attrName : elemNode->getAttributeNames()) {
                        std::cout << "  " << attrName << ": " 
                                  << elemNode->getAttribute(attrName) << "\n";
                    }
                    
                    // 检查样式块
                    if (elemNode->getStyleBlock()) {
                        std::cout << "  Has style block\n";
                    }
                }
                
                // 查找文本节点
                auto textNodes = rootNode->findDescendants(NodeType::TEXT);
                std::cout << "\nFound " << textNodes.size() << " text nodes\n";
                
                for (auto* textNode : textNodes) {
                    TextNode* txtNode = static_cast<TextNode*>(textNode);
                    std::cout << "Text: \"" << txtNode->getText() << "\"\n";
                }
                
                // 查找样式节点
                auto styleNodes = rootNode->findDescendants(NodeType::STYLE);
                std::cout << "\nFound " << styleNodes.size() << " style nodes\n";
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    testElementParsing();
    return 0;
}