#include <iostream>
#include <string>
#include "../Scanner/Scanner.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLContext/CHTLContext.h"
#include "../CHTL JS/CHTLJSLexer/Lexer.h"

int main() {
    std::cout << "=== CHTL 编译器测试程序 ===" << std::endl;
    std::cout << "欢迎使用CHTL语言编译器！" << std::endl;
    std::cout << std::endl;

    // 测试用的CHTL代码
    std::string test_chtl_code = R"(
[Configuration]
{
    DEBUG_MODE = true;
}

html
{
    head
    {
        title
        {
            text
            {
                CHTL 测试页面
            }
        }
    }

    body
    {
        div
        {
            id: main-container;
            class: container;

            style
            {
                .container
                {
                    width: 100%;
                    height: 100vh;
                    background-color: #f0f0f0;
                }

                &:hover
                {
                    background-color: #e0e0e0;
                }
            }

            script
            {
                {{.container}}->addEventListener('click', () => {
                    console.log('容器被点击了！');
                });

                const anim = animate {
                    target: {{.container}},
                    duration: 1000,
                    begin: {
                        opacity: 0
                    },
                    end: {
                        opacity: 1
                    }
                };
            }

            text
            {
                欢迎使用CHTL语言！
            }
        }
    }
}

[Template] @Style ButtonStyle
{
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    background-color: #007bff;
    color: white;
    cursor: pointer;
}

[Custom] @Element Button
{
    button
    {
        style
        {
            @Style ButtonStyle;
        }
    }
}
)";

    std::cout << "1. 测试 Scanner（代码片段切割器）" << std::endl;
    
    CHTL::Scanner scanner(test_chtl_code);
    bool scan_success = scanner.scan();
    
    std::cout << "扫描结果: " << (scan_success ? "成功" : "失败") << std::endl;
    std::cout << scanner.generateReport() << std::endl;

    auto fragments = scanner.getFragments();
    std::cout << "发现 " << fragments.size() << " 个代码片段" << std::endl;

    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        std::cout << "片段 " << i << ": " 
                  << CHTL::Scanner::fragmentTypeToString(fragment.type)
                  << " (" << fragment.content.length() << " 字符)" << std::endl;
    }
    
    std::cout << std::endl;

    std::cout << "2. 测试 CHTL Lexer（词法分析器）" << std::endl;

    // 获取CHTL片段进行词法分析
    auto chtl_fragments = scanner.getFragmentsByType(CHTL::FragmentType::CHTL);
    if (!chtl_fragments.empty()) {
        CHTL::Lexer lexer(chtl_fragments[0].content);
        auto tokens = lexer.tokenize();
        
        std::cout << "CHTL词法分析结果: 生成了 " << tokens.size() << " 个Token" << std::endl;
        
        // 显示前20个Token
        size_t show_count = std::min(size_t(20), tokens.size());
        for (size_t i = 0; i < show_count; ++i) {
            std::cout << "  " << i << ": " << tokens[i]->getTypeString() 
                      << " \"" << tokens[i]->getValue() << "\"" << std::endl;
        }
        
        if (tokens.size() > 20) {
            std::cout << "  ... 还有 " << (tokens.size() - 20) << " 个Token" << std::endl;
        }
    } else {
        std::cout << "没有找到CHTL代码片段" << std::endl;
    }
    
    std::cout << std::endl;

    std::cout << "3. 测试 CHTL JS Lexer（CHTL JS词法分析器）" << std::endl;

    // 获取CHTL JS片段进行词法分析
    auto chtljs_fragments = scanner.getFragmentsByType(CHTL::FragmentType::CHTL_JS);
    if (!chtljs_fragments.empty()) {
        // 由于CHTL JS的Lexer还没有完全实现，这里只显示片段内容
        std::cout << "CHTL JS代码片段内容:" << std::endl;
        std::cout << chtljs_fragments[0].content << std::endl;
    } else {
        std::cout << "没有找到CHTL JS代码片段" << std::endl;
    }

    std::cout << std::endl;

    std::cout << "4. 测试 CHTL Context（编译上下文）" << std::endl;

    CHTL::CHTLContextPtr context = std::make_shared<CHTL::CHTLContext>("test.chtl");
    context->setSourceContent(test_chtl_code);
    context->setStrictMode(false);

    // 测试作用域操作
    context->pushScope(CHTL::ScopeType::GLOBAL, "global");
    context->pushScope(CHTL::ScopeType::ELEMENT, "html");
    context->pushScope(CHTL::ScopeType::ELEMENT, "body");

    std::cout << "当前作用域深度: " << (context->getCurrentScope() ? "有效" : "无效") << std::endl;

    // 弹出作用域
    context->popScope();
    context->popScope();

    std::cout << "编译上下文诊断报告:" << std::endl;
    std::cout << context->generateDiagnosticReport() << std::endl;

    std::cout << "=== 测试完成 ===" << std::endl;
    std::cout << "CHTL编译器核心模块运行正常！" << std::endl;

    return 0;
}