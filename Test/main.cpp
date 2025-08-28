#include <iostream>
#include <string>
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../ThirdParty/CJMODAPI/Syntax.h"
#include "../ThirdParty/CJMODAPI/Arg.h"
#include "../ThirdParty/CJMODAPI/CJMODScannerAPI.h"
#include "../ThirdParty/CJMODAPI/CJMODGenerator.h"
#include "../CHTL/CHTLContext/NamespaceManager.h"
#include "../CompilerDispatcher/CompilerDispatcher.h"

using namespace Scanner;
using namespace CHTL;
using namespace CHTLCompiler;

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

    // 测试占位符类型
    std::cout << "\n测试占位符类型($! $? $_ $):" << std::endl;
    auto placeholderTest = CJMODAPI::Syntax::analyze("$! ** $? $_");
    placeholderTest.print();
    std::cout << "占位符类型: ";
    for (size_t i = 0; i < placeholderTest.size(); ++i) {
        auto& atom = placeholderTest[i];
        if (atom.isRequired()) std::cout << "Required ";
        else if (atom.isOptional()) std::cout << "Optional ";
        else if (atom.isUnordered()) std::cout << "Unordered ";
        else std::cout << "Regular ";
    }
    std::cout << std::endl;

    // 测试选择器自动化配置
    std::cout << "\n测试选择器自动化配置:" << std::endl;
    std::cout << "局部style: .box { color: red; }" << std::endl;
    std::cout << "局部script: {{.box}}->click()" << std::endl;
    std::cout << "引用选择器: &:hover { background: blue; }" << std::endl;

    // 测试Import路径解析
    std::cout << "\n测试Import路径解析:" << std::endl;
    std::cout << "[Import] @Chtl from Chtholly.*" << std::endl;
    std::cout << "[Import] @CJmod from Box" << std::endl;
    std::cout << "[Import] @Html from index.html as mainPage" << std::endl;

    // 测试命名空间管理
    std::cout << "\n测试命名空间管理:" << std::endl;
    NamespaceManager nsManager;
    
    // 创建嵌套命名空间
    nsManager.createNamespace("Core");
    nsManager.createNamespace("UI", "Core");
    nsManager.createNamespace("Components", "UI");
    
    // 添加符号
    nsManager.addSymbol("Core", "version", "1.0.0");
    nsManager.addSymbol("UI", "theme", "dark");
    nsManager.addSymbol("Components", "button", "ButtonComponent");
    
    // 测试符号查找
    std::cout << "Core::version: " << nsManager.findSymbol("Core", "version") << std::endl;
    std::cout << "UI::theme: " << nsManager.findSymbol("UI", "theme") << std::endl;
    std::cout << "Components::button: " << nsManager.findSymbol("Components", "button") << std::endl;
    
    // 测试继承查找
    std::cout << "Components继承Core::version: " << nsManager.findSymbol("Components", "version") << std::endl;
    
    // 测试冲突检测
    nsManager.addSymbol("Core", "conflict", "CoreValue");
    nsManager.addSymbol("UI", "conflict", "UIValue");
    auto conflicts = nsManager.detectConflicts("Core", "UI");
    std::cout << "检测到冲突: ";
    for (const auto& conflict : conflicts) {
        std::cout << conflict << " ";
    }
    std::cout << std::endl;
    
    // 测试命名空间合并
    nsManager.createNamespace("Utils");
    nsManager.addSymbol("Utils", "helper", "HelperFunction");
    bool merged = nsManager.mergeNamespace("Core", "Utils");
    std::cout << "合并Utils到Core: " << (merged ? "成功" : "失败") << std::endl;
    std::cout << "Core::helper: " << nsManager.findSymbol("Core", "helper") << std::endl;

    // 测试编译器调度器
    std::cout << "\n测试编译器调度器:" << std::endl;
    CHTLCompiler::CompilerDispatcher dispatcher;
    
    // 测试混合代码编译
    std::string mixedCode = R"(
[Template] @Style Button
{
    background: "blue";
    color: "white";
}

button
{
    style
    {
        @Style Button;
    }
    
    script
    {
        vir btn = listen {
            click: () => {
                std::cout << "Button clicked!";
            }
        };
    }
}

<style>
.button {
    border-radius: 5px;
}
</style>

<script>
function init() {
    console.log("Initialized");
}
</script>
)";
    
    dispatcher.setSource(mixedCode);
    auto compilationResult = dispatcher.compile();
    
    std::cout << "编译结果: " << (compilationResult.success ? "成功" : "失败") << std::endl;
    if (!compilationResult.errors.empty()) {
        std::cout << "编译错误:" << std::endl;
        for (const auto& error : compilationResult.errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    std::cout << "输出长度: " << compilationResult.output.length() << " 字符" << std::endl;

    std::cout << "测试完成!" << std::endl;
    return 0;
}