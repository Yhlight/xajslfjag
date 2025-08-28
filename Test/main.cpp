#include <iostream>
#include <string>
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../ThirdParty/CJMODAPI/Syntax.h"
#include "../ThirdParty/CJMODAPI/Arg.h"
#include "../ThirdParty/CJMODAPI/CJMODScannerAPI.h"
#include "../ThirdParty/CJMODAPI/CJMODGenerator.h"

using namespace Scanner;

int main() {
    std::cout << "CHTL项目测试开始..." << std::endl;
    
    // 测试统一扫描器
    CHTLUnifiedScanner scanner;
    
    // 测试CHTL代码
    std::string chtlCode = R"(
[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}

div
{
    style
    {
        @Style DefaultText;
    }
}
)";
    
    std::cout << "测试CHTL代码扫描..." << std::endl;
    scanner.setSource(chtlCode);
    auto fragments = scanner.scan();
    
    std::cout << "扫描到 " << fragments.size() << " 个代码片段:" << std::endl;
    for (const auto& fragment : fragments) {
        std::string typeStr;
        switch (fragment.type) {
            case CodeFragmentType::CHTL: typeStr = "CHTL"; break;
            case CodeFragmentType::CHTL_JS: typeStr = "CHTL_JS"; break;
            case CodeFragmentType::CSS: typeStr = "CSS"; break;
            case CodeFragmentType::JAVASCRIPT: typeStr = "JAVASCRIPT"; break;
            default: typeStr = "UNKNOWN"; break;
        }
        
        std::cout << "类型: " << typeStr 
                  << ", 位置: [" << fragment.startPos << ", " << fragment.endPos << "]"
                  << ", 行: " << fragment.line << ", 列: " << fragment.column << std::endl;
        std::cout << "内容: " << fragment.content.substr(0, 100) 
                  << (fragment.content.length() > 100 ? "..." : "") << std::endl;
        std::cout << "---" << std::endl;
    }
    
    // 测试CHTL JS代码
    std::string chtlJSCode = R"(
script
{
    vir test = listen {
        click: () => {
            std::cout << "Clicked!";
        }
    };
    
    {{.box}}->addEventListener('click', () => {
        std::cout << "Box clicked!";
    });
}
)";
    
    std::cout << "\n测试CHTL JS代码扫描..." << std::endl;
    scanner.setSource(chtlJSCode);
    fragments = scanner.scan();
    
    std::cout << "扫描到 " << fragments.size() << " 个代码片段:" << std::endl;
    for (const auto& fragment : fragments) {
        std::string typeStr;
        switch (fragment.type) {
            case CodeFragmentType::CHTL: typeStr = "CHTL"; break;
            case CodeFragmentType::CHTL_JS: typeStr = "CHTL_JS"; break;
            case CodeFragmentType::CSS: typeStr = "CSS"; break;
            case CodeFragmentType::JAVASCRIPT: typeStr = "JAVASCRIPT"; break;
            default: typeStr = "UNKNOWN"; break;
        }
        
        std::cout << "类型: " << typeStr 
                  << ", 位置: [" << fragment.startPos << ", " << fragment.endPos << "]"
                  << ", 行: " << fragment.line << ", 列: " << fragment.column << std::endl;
        std::cout << "内容: " << fragment.content.substr(0, 100) 
                  << (fragment.content.length() > 100 ? "..." : "") << std::endl;
        std::cout << "---" << std::endl;
    }

    // 测试CJMOD基础双指针扫描
    std::string cjmodSnippet = "3 ** 4";
    scanner.setSource(cjmodSnippet);
    auto r = scanner.scanCJMODByTwoPointers(0, cjmodSnippet.size());
    std::cout << "\n测试CJMOD双指针扫描('3 ** 4')，切分结果(" << r.tokens.size() << "):" << std::endl;
    for (auto& t : r.tokens) std::cout << "[" << t << "]";
    std::cout << std::endl;

    // 使用原始API头-only骨架进行端到端示例（直接transform）
    std::cout << "\n测试CJMOD原始API端到端('3 ** 4' -> 'pow(3, 4)'):" << std::endl;
    auto pattern = CJMODAPI::Syntax::analyze("$ ** $");
    pattern.print();
    auto scanned = CJMODAPI::CJMODScannerAPI::scan(pattern, "**", cjmodSnippet);
    scanned.print();
    scanned.transform(std::string("pow(") + scanned[0].value + ", " + scanned[2].value + ")");
    CJMODAPI::CJMODGenerator::exportResult(scanned);

    // 按文档流程：bind + fillValue + transform
    std::cout << "\n测试CJMOD原始API流程(analyze→bind→scan→fillValue→transform):" << std::endl;
    auto args = CJMODAPI::Syntax::analyze("$ ** $");
    args.bind("$", [](const std::string& v){ return v; });
    args.bind("**", [](const std::string& v){ return v; });
    auto result = CJMODAPI::CJMODScannerAPI::scan(args, "**", cjmodSnippet);
    args.fillValue(result);
    std::cout << "args填充后: "; args.print();
    args.transform(std::string("pow(") + args[0].value + ", " + args[2].value + ")");
    std::cout << "导出: "; CJMODAPI::CJMODGenerator::exportResult(args);

    std::cout << "测试完成!" << std::endl;
    return 0;
}