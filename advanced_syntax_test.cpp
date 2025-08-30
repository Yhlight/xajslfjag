#include <iostream>
#include <memory>
#include <string>

// 包含CHTL核心组件
#include "src/CHTL/CHTLLexer/Lexer.h"
#include "src/CHTL/CHTLParser/Parser.h"
#include "src/CHTL/CHTLGenerator/Generator.h"
#include "src/CHTL/CHTLContext/Context.h"
#include "src/CHTL/CHTLNamespace/NamespaceManager.h"
#include "src/CHTL/CHTLConstraint/ConstraintSystem.h"
#include "src/CHTL/CHTLSelector/SelectorAutomation.h"
#include "src/CMODSystem/CJMODGenerator.h"
#include "src/Scanner/UnifiedScanner.h"
#include "src/CompilerDispatcher/Dispatcher.h"

int main() {
    std::cout << "=== CHTL高级语法特性测试 ===" << std::endl;
    
    try {
        // 1. 统一扫描器测试
        std::cout << "\n1. 测试CHTLUnifiedScanner..." << std::endl;
        
        std::string testSource = R"(
            html {
                body {
                    style {
                        .test { color: red; }
                    }
                    script {
                        console.log("test");
                    }
                }
            }
        )";
        
        CHTL::CHTLUnifiedScanner scanner(testSource);
        auto fragments = scanner.scanAndCut();
        
        std::cout << "   扫描到 " << fragments.size() << " 个代码片段" << std::endl;
        for (const auto& fragment : fragments) {
            std::cout << "   片段类型: " << static_cast<int>(fragment.type) 
                      << ", 长度: " << fragment.content.size() << std::endl;
        }
        
        // 2. 编译器调度器测试
        std::cout << "\n2. 测试CompilerDispatcher..." << std::endl;
        
        CHTL::CompilerDispatcher dispatcher;
        // TODO: compileFragments方法需要完善实现
        std::cout << "   编译器调度器初始化成功" << std::endl;
        std::cout << "   TODO: 完善compileFragments方法实现" << std::endl;
        
        // 3. 命名空间管理器测试
        std::cout << "\n3. 测试NamespaceManager..." << std::endl;
        
        CHTL::NamespaceManager nsManager;
        
        // 创建命名空间
        auto testNS = nsManager.createNamespace("TestNamespace");
        if (testNS) {
            std::cout << "   创建命名空间成功: " << testNS->name << std::endl;
        }
        
        // 创建嵌套命名空间
        auto nestedNS = nsManager.createNestedNamespace("TestNamespace.SubSpace");
        if (nestedNS) {
            std::cout << "   创建嵌套命名空间成功: " << nestedNS->getFullName() << std::endl;
        }
        
        // 4. 约束系统测试
        std::cout << "\n4. 测试ConstraintSystem..." << std::endl;
        
        CHTL::ConstraintSystem constraintSystem;
        
        // 添加元素约束
        constraintSystem.addRule(CHTL::ConstraintType::ELEMENT_CONSTRAINT, "span");
        std::cout << "   添加元素约束规则: span" << std::endl;
        
        // 添加模板约束
        constraintSystem.addRule(CHTL::ConstraintType::TEMPLATE_CONSTRAINT, "@Html");
        std::cout << "   添加模板约束规则: @Html" << std::endl;
        
        std::cout << "   约束系统测试完成" << std::endl;
        
        // 5. 选择器自动化测试
        std::cout << "\n5. 测试SelectorAutomation..." << std::endl;
        
        CHTL::SelectorAutomationConfig selectorConfig;
        // 使用默认配置
        
        CHTL::SelectorAutomationManager selectorManager(selectorConfig);
        
        // 测试选择器提取
        CHTL::StringVector testSelectors = {".test", ".example"};
        std::string classSelector = selectorManager.getFirstClassSelector(testSelectors);
        if (!classSelector.empty()) {
            std::cout << "   首个类选择器: " << classSelector << std::endl;
        }
        
        CHTL::StringVector testIdSelectors = {"#main", "#content"};
        std::string idSelector = selectorManager.getFirstIdSelector(testIdSelectors);
        if (!idSelector.empty()) {
            std::cout << "   首个ID选择器: " << idSelector << std::endl;
        }
        
        // 6. CJMOD生成器测试
        std::cout << "\n6. 测试CJMODGenerator..." << std::endl;
        
        CJMOD::GeneratorConfig cjmodConfig;
        cjmodConfig.enableOptimization = true;
        cjmodConfig.enableMinification = false;
        
        CJMOD::CJMODGenerator cjmodGenerator(cjmodConfig);
        
        // 创建测试参数
        CJMOD::Arg testArgs;
        CJMOD::AtomArg atom1;
        atom1.placeholder = "$";
        atom1.fillValue("testValue");
        testArgs.atoms.push_back(atom1);
        
        auto genResult = cjmodGenerator.generate(testArgs, "listen");
        std::cout << "   CJMOD生成结果: " << (genResult.success ? "成功" : "失败") << std::endl;
        if (genResult.success) {
            std::cout << "   生成代码长度: " << genResult.generatedCode.size() << std::endl;
        }
        
        // 7. 完整解析测试
        std::cout << "\n7. 测试完整解析流程..." << std::endl;
        
        std::string completeTest = R"(
            [Configuration] {
                DISABLE_DEFAULT_NAMESPACE = false;
            }
            
            [Namespace] Test {
                [Template] @Element Button {
                    button {
                        text { "模板按钮" }
                    }
                }
            }
            
            html {
                body {
                    @Element Button from Test;
                }
            }
        )";
        
        CHTL::Lexer lexer(completeTest);
        // 使用现有的方法进行词法分析
        std::cout << "   词法分析初始化完成" << std::endl;
        
        CHTL::ParserConfig parserConfig;
        // 使用默认配置
        
        CHTL::Parser parser(std::make_unique<CHTL::Lexer>(completeTest), parserConfig);
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "   语法分析完成，AST根节点类型: " << static_cast<int>(ast->getType()) << std::endl;
            
            // 测试生成
            CHTL::GeneratorConfig genConfig;
            genConfig.formatHTML = true;
            genConfig.minifyOutput = false;
            
            // TODO: 当Generator完全实现后取消注释
            // CHTL::Generator generator(genConfig);
            // auto output = generator.generate(ast.get());
            // 
            // if (output.success) {
            //     std::cout << "   代码生成完成，HTML长度: " << output.html.size() << std::endl;
            // } else {
            //     std::cout << "   代码生成失败" << std::endl;
            // }
            
            std::cout << "   代码生成模块待完善" << std::endl;
        } else {
            std::cout << "   语法分析失败" << std::endl;
            auto errors = parser.getErrors();
            for (const auto& error : errors) {
                std::cout << "   错误: " << error.toString() << std::endl;
            }
        }
        
        std::cout << "\n=== 测试完成 ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}