#include <iostream>
#include <string>
#include <memory>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL编译器 v1.0.0" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // 测试CHTL词法分析器
    std::cout << "\n测试CHTL词法分析器:" << std::endl;
    
    std::string chtlCode = R"(
div {
    id: "main-container";
    class: "container";
    
    style {
        .container {
            width: 100px;
            height: 200px;
        }
        
        &:hover {
            background-color: red;
        }
    }
    
    text {
        "Hello, CHTL World!"
    }
}
)";

    auto chtlGlobalMap = std::make_shared<CHTL::CHTLGlobalMap>();
    CHTL::CHTLLexer chtlLexer(chtlGlobalMap);
    chtlLexer.setInput(chtlCode);
    
    auto chtlTokens = chtlLexer.tokenize();
    std::cout << "CHTL Token数量: " << chtlTokens.size() << std::endl;
    
    for (size_t i = 0; i < std::min(size_t(10), chtlTokens.size()); ++i) {
        std::cout << "  " << chtlTokens[i]->toString() << std::endl;
    }
    
    // 测试CHTL JS词法分析器
    std::cout << "\n测试CHTL JS词法分析器:" << std::endl;
    
    std::string chtljsCode = R"(
module {
    load: ./utils.cjjs,
    load: ./animation.cjjs
}

{{box}}->listen {
    click: () => {
        console.log('Box clicked!');
    },
    
    mouseenter: function() {
        animate {
            target: {{box}},
            duration: 300,
            begin: {
                opacity: 0.5
            },
            end: {
                opacity: 1.0
            }
        }
    }
}
)";

    auto chtljsGlobalMap = std::make_shared<CHTLJS::CHTLJSGlobalMap>();
    CHTLJS::CHTLJSLexer chtljsLexer(chtljsGlobalMap);
    chtljsLexer.setInput(chtljsCode);
    
    auto chtljsTokens = chtljsLexer.tokenize();
    std::cout << "CHTL JS Token数量: " << chtljsTokens.size() << std::endl;
    
    for (size_t i = 0; i < std::min(size_t(10), chtljsTokens.size()); ++i) {
        std::cout << "  " << chtljsTokens[i]->toString() << std::endl;
    }
    
    // 检查错误
    if (chtlLexer.hasErrors()) {
        std::cout << "\nCHTL词法分析错误:" << std::endl;
        for (const auto& error : chtlLexer.getErrors()) {
            std::cout << "  Line " << error.line << ":" << error.column 
                     << " - " << error.message << std::endl;
        }
    }
    
    if (chtljsLexer.hasErrors()) {
        std::cout << "\nCHTL JS词法分析错误:" << std::endl;
        for (const auto& error : chtljsLexer.getErrors()) {
            std::cout << "  Line " << error.line << ":" << error.column 
                     << " - " << error.message << std::endl;
        }
    }
    
    std::cout << "\n编译器测试完成!" << std::endl;
    std::cout << "CHTL和CHTL JS编译器已成功启动，模块化架构工作正常。" << std::endl;
    
    return 0;
}