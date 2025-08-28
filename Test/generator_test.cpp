#include <iostream>
#include <memory>
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"

int main() {
    std::cout << "CHTL项目生成器测试开始..." << std::endl;
    
    try {
        // 测试CHTL生成器
        std::cout << "\n测试CHTL生成器..." << std::endl;
        
        // 解析CHTL代码
        std::string chtlCode = R"(
[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}

div
{
    id: "box";
    class: "container";
    
    text
    {
        Hello World
    }
}
)";
        
        CHTL::CHTLParser chtlParser;
        chtlParser.setSource(chtlCode);
        auto chtlAST = chtlParser.parse();
        
        if (chtlAST && chtlParser.isSuccess()) {
            std::cout << "CHTL解析成功，AST类型: " << static_cast<int>(chtlAST->type) << std::endl;
            
            // 生成CHTL代码
            CHTL::CHTLGenerator chtlGenerator;
            std::string generatedCHTL = chtlGenerator.generate(chtlAST);
            
            if (chtlGenerator.isSuccess()) {
                std::cout << "CHTL代码生成成功:" << std::endl;
                std::cout << "=== 生成的CHTL代码 ===" << std::endl;
                std::cout << generatedCHTL;
                std::cout << "=== 代码生成完成 ===" << std::endl;
            } else {
                std::cout << "CHTL代码生成失败:" << std::endl;
                for (const auto& error : chtlGenerator.getErrors()) {
                    std::cout << "  - " << error << std::endl;
                }
            }
        } else {
            std::cout << "CHTL解析失败:" << std::endl;
            for (const auto& error : chtlParser.getErrors()) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        // 测试CHTL JS生成器
        std::cout << "\n测试CHTL JS生成器..." << std::endl;
        
        // 解析CHTL JS代码
        std::string chtlJSCode = R"(
vir test = listen {
    click: () => {
        std::cout << "Clicked!";
    },
    hover: () => {
        console.log("Hovered!");
    }
}

{{.box}}->addEventListener('click', () => {
    std::cout << "Box clicked!";
})
)";
        
        CHTLJS::CHTLJSParser chtlJSParser;
        chtlJSParser.setSource(chtlJSCode);
        auto chtlJSAST = chtlJSParser.parse();
        
        if (chtlJSAST && chtlJSParser.isSuccess()) {
            std::cout << "CHTL JS解析成功，AST类型: " << static_cast<int>(chtlJSAST->type) << std::endl;
            
            // 生成CHTL JS代码
            CHTLJS::CHTLJSGenerator chtlJSGenerator;
            std::string generatedCHTLJS = chtlJSGenerator.generate(chtlJSAST);
            
            if (chtlJSGenerator.isSuccess()) {
                std::cout << "CHTL JS代码生成成功:" << std::endl;
                std::cout << "=== 生成的CHTL JS代码 ===" << std::endl;
                std::cout << generatedCHTLJS;
                std::cout << "=== 代码生成完成 ===" << std::endl;
            } else {
                std::cout << "CHTL JS代码生成失败:" << std::endl;
                for (const auto& error : chtlJSGenerator.getErrors()) {
                    std::cout << "  - " << error << std::endl;
                }
            }
        } else {
            std::cout << "CHTL JS解析失败:" << std::endl;
            for (const auto& error : chtlJSParser.getErrors()) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        std::cout << "\n生成器测试完成!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "测试异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "未知异常" << std::endl;
        return 1;
    }
}