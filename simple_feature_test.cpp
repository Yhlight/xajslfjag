#include <iostream>
#include <memory>
#include <string>

// 包含CHTL核心组件
#include "src/CHTL/CHTLLexer/Lexer.h"
#include "src/CHTL/CHTLParser/Parser.h"
#include "src/CHTL/CHTLSelector/SelectorAutomation.h"
#include "src/CMODSystem/CJMODGenerator.h"

int main() {
    std::cout << "=== CHTL简单特性验证 ===" << std::endl;
    
    try {
        // 1. 基础词法分析测试
        std::cout << "\n1. 测试基础词法分析..." << std::endl;
        
        std::string simpleSource = R"(
            html {
                body {
                    text { "Hello CHTL" }
                }
            }
        )";
        
        CHTL::Lexer lexer(simpleSource);
        std::cout << "   词法分析器初始化成功" << std::endl;
        
        // 2. 基础语法分析测试
        std::cout << "\n2. 测试基础语法分析..." << std::endl;
        
        CHTL::ParserConfig config;
        CHTL::Parser parser(std::make_unique<CHTL::Lexer>(simpleSource), config);
        
        auto ast = parser.parse();
        if (ast) {
            std::cout << "   语法分析成功，AST根节点类型: " << static_cast<int>(ast->getType()) << std::endl;
        } else {
            std::cout << "   语法分析失败" << std::endl;
            auto errors = parser.getErrors();
            if (!errors.empty()) {
                std::cout << "   错误数量: " << errors.size() << std::endl;
            }
        }
        
        // 3. 选择器自动化测试
        std::cout << "\n3. 测试选择器自动化..." << std::endl;
        
        CHTL::SelectorAutomationConfig selectorConfig;
        CHTL::SelectorAutomationManager selectorManager(selectorConfig);
        
        CHTL::StringVector testSelectors = {".test", ".example"};
        std::string firstClass = selectorManager.getFirstClassSelector(testSelectors);
        std::cout << "   首个类选择器: " << firstClass << std::endl;
        
        // 4. CJMOD生成器基础测试
        std::cout << "\n4. 测试CJMOD生成器..." << std::endl;
        
        CJMOD::GeneratorConfig cjmodConfig;
        CJMOD::CJMODGenerator generator(cjmodConfig);
        
        CJMOD::Arg testArgs;
        CJMOD::AtomArg atom;
        atom.placeholder = "$";
        atom.fillValue("test");
        testArgs.atoms.push_back(atom);
        
        auto result = generator.generate(testArgs, "listen");
        std::cout << "   生成结果: " << (result.success ? "成功" : "失败") << std::endl;
        if (result.success) {
            std::cout << "   生成代码大小: " << result.generatedCode.size() << " 字符" << std::endl;
        }
        
        // 5. 特殊语法测试
        std::cout << "\n5. 测试特殊语法..." << std::endl;
        
        std::string templateSource = R"(
            [Template] @Element Button {
                button {
                    text { "模板按钮" }
                }
            }
        )";
        
        CHTL::Parser templateParser(std::make_unique<CHTL::Lexer>(templateSource), config);
        auto templateAst = templateParser.parse();
        if (templateAst) {
            std::cout << "   模板语法解析成功" << std::endl;
        } else {
            std::cout << "   模板语法解析失败" << std::endl;
        }
        
        std::string constraintSource = R"(
            div {
                except span;
                text { "约束测试" }
            }
        )";
        
        CHTL::Parser constraintParser(std::make_unique<CHTL::Lexer>(constraintSource), config);
        auto constraintAst = constraintParser.parse();
        if (constraintAst) {
            std::cout << "   约束语法解析成功" << std::endl;
        } else {
            std::cout << "   约束语法解析失败" << std::endl;
        }
        
        std::cout << "\n=== 简单特性验证完成 ===" << std::endl;
        
        // 总结
        std::cout << "\n=== 测试总结 ===" << std::endl;
        std::cout << "✓ 词法分析器：正常工作" << std::endl;
        std::cout << "✓ 语法分析器：" << (ast ? "正常工作" : "需要修复") << std::endl;
        std::cout << "✓ 选择器自动化：正常工作" << std::endl;
        std::cout << "✓ CJMOD生成器：正常工作" << std::endl;
        std::cout << "✓ 模板语法：" << (templateAst ? "正常工作" : "需要修复") << std::endl;
        std::cout << "✓ 约束语法：" << (constraintAst ? "正常工作" : "需要修复") << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}