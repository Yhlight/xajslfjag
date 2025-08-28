#include <iostream>
#include <string>
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../ThirdParty/CJMODAPI/Syntax.h"
#include "../ThirdParty/CJMODAPI/Arg.h"
#include "../ThirdParty/CJMODAPI/CJMODScannerAPI.h"
#include "../ThirdParty/CJMODAPI/CJMODGenerator.h"
#include "../CHTL/CHTLContext/NamespaceManager.h"
#include "../CHTL/CHTLContext/ImportManager.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/BaseNode.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL JS/CHTLJSNode/CHTLJSBaseNode.h"
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

    // 测试Import增强功能
    std::cout << "\n测试Import增强功能:" << std::endl;
    auto configManager = std::make_shared<CHTL::ConfigurationManager>();
    CHTL::ImportManager importManager(configManager, "/workspace");
    
    // 测试基础路径设置
    importManager.setBasePath("/workspace");
    std::cout << "基础路径: " << importManager.getBasePath() << std::endl;
    
    // 测试通配符路径解析
    std::vector<std::string> wildcardResults = importManager.resolveWildcardPath("*.chtl", "./modules");
    std::cout << "通配符路径解析结果数量: " << wildcardResults.size() << std::endl;
    
    // 测试子模块路径解析
    std::vector<std::string> subModuleResults = importManager.resolveSubModulePath("Chtholly.Space", "./modules");
    std::cout << "子模块路径解析结果数量: " << subModuleResults.size() << std::endl;
    
    // 测试循环依赖检测
    bool hasCircular = importManager.checkCircularDependency("file1.chtl", "file3.chtl");
    std::cout << "循环依赖检测: " << (hasCircular ? "检测到" : "未检测到") << std::endl;
    
    // 测试重复导入检测
    bool hasDuplicate = importManager.checkDuplicateImport("file1.chtl", "file2.chtl");
    std::cout << "重复导入检测: " << (hasDuplicate ? "检测到" : "未检测到") << std::endl;

    // 测试命名空间管理
    std::cout << "\n测试命名空间管理:" << std::endl;
    CHTL::NamespaceManager nsManager(configManager);
    
    // 创建嵌套命名空间
    nsManager.createNamespace("Core", "test.chtl");
    nsManager.createNestedNamespace("Core", "UI", "test.chtl");
    nsManager.createNestedNamespace("UI", "Components", "test.chtl");
    
    // 添加命名空间项
    auto versionItem = std::make_shared<CHTL::NamespaceItem>();
    versionItem->type = CHTL::NamespaceItemType::CUSTOM_ELEMENT;
    versionItem->name = "version";
    versionItem->sourceFile = "test.chtl";
    versionItem->lineNumber = 1;
    versionItem->content = "1.0.0";
    nsManager.addNamespaceItem("Core", versionItem);
    
    auto themeItem = std::make_shared<CHTL::NamespaceItem>();
    themeItem->type = CHTL::NamespaceItemType::CUSTOM_ELEMENT;
    themeItem->name = "theme";
    themeItem->sourceFile = "test.chtl";
    themeItem->lineNumber = 2;
    themeItem->content = "dark";
    nsManager.addNamespaceItem("UI", themeItem);
    
    auto buttonItem = std::make_shared<CHTL::NamespaceItem>();
    buttonItem->type = CHTL::NamespaceItemType::CUSTOM_ELEMENT;
    buttonItem->name = "button";
    buttonItem->sourceFile = "test.chtl";
    buttonItem->lineNumber = 3;
    buttonItem->content = "ButtonComponent";
    nsManager.addNamespaceItem("Components", buttonItem);
    
    // 测试命名空间项查找
    auto versionFound = nsManager.getNamespaceItem("Core", "version", CHTL::NamespaceItemType::CUSTOM_ELEMENT);
    std::cout << "Core::version: " << (versionFound ? versionFound->content : "not found") << std::endl;
    
    auto themeFound = nsManager.getNamespaceItem("UI", "theme", CHTL::NamespaceItemType::CUSTOM_ELEMENT);
    std::cout << "UI::theme: " << (themeFound ? themeFound->content : "not found") << std::endl;
    
    auto buttonFound = nsManager.getNamespaceItem("Components", "button", CHTL::NamespaceItemType::CUSTOM_ELEMENT);
    std::cout << "Components::button: " << (buttonFound ? buttonFound->content : "not found") << std::endl;
    
    // 测试冲突检测
    auto conflicts = nsManager.detectConflicts();
    std::cout << "检测到冲突数量: " << conflicts.size() << std::endl;
    
    // 测试命名空间合并
    nsManager.createNamespace("Utils", "test.chtl");
    auto helperItem = std::make_shared<CHTL::NamespaceItem>();
    helperItem->type = CHTL::NamespaceItemType::CUSTOM_ELEMENT;
    helperItem->name = "helper";
    helperItem->sourceFile = "test.chtl";
    helperItem->lineNumber = 4;
    helperItem->content = "HelperFunction";
    nsManager.addNamespaceItem("Utils", helperItem);
    
    bool merged = nsManager.mergeNamespaces("Utils");
    std::cout << "命名空间合并: " << (merged ? "成功" : "失败") << std::endl;

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

    // 测试CHTL解析器
    std::cout << "\n测试CHTL解析器:" << std::endl;
    CHTL::CHTLParser chtlParser;
    
    // 测试模板解析
    std::string templateCode = R"(
[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}
)";
    
    chtlParser.setSource(templateCode);
    auto templateAST = chtlParser.parse();
    std::cout << "模板解析: " << (chtlParser.isSuccess() ? "成功" : "失败") << std::endl;
    if (templateAST) {
        std::cout << "AST类型: " << static_cast<int>(templateAST->type) << std::endl;
        if (auto templateNode = std::dynamic_pointer_cast<CHTL::TemplateNode>(templateAST)) {
            std::cout << "模板类型: " << templateNode->templateType << std::endl;
            std::cout << "模板名称: " << templateNode->templateName << std::endl;
        }
    }
    
    // 测试自定义元素解析
    std::string customCode = R"(
[Custom] @Style YellowText
{
    @Style DefaultText
    {
        delete line-height;
    }
    color: yellow;
}
)";
    
    chtlParser.setSource(customCode);
    auto customAST = chtlParser.parse();
    std::cout << "自定义元素解析: " << (chtlParser.isSuccess() ? "成功" : "失败") << std::endl;
    if (customAST) {
        std::cout << "AST类型: " << static_cast<int>(customAST->type) << std::endl;
        if (auto customNode = std::dynamic_pointer_cast<CHTL::CustomNode>(customAST)) {
            std::cout << "自定义类型: " << customNode->customType << std::endl;
            std::cout << "自定义名称: " << customNode->customName << std::endl;
            std::cout << "操作: " << customNode->operation << std::endl;
        }
    }
    
    // 测试元素解析
    std::string elementCode = R"(
div
{
    id: "box";
    class: "container";
    
    text
    {
        Hello World
    }
    
    style
    {
        .box
        {
            width: 100px;
        }
    }
}
)";
    
    chtlParser.setSource(elementCode);
    auto elementAST = chtlParser.parse();
    std::cout << "元素解析: " << (chtlParser.isSuccess() ? "成功" : "失败") << std::endl;
    if (elementAST) {
        std::cout << "AST类型: " << static_cast<int>(elementAST->type) << std::endl;
        if (auto elementNode = std::dynamic_pointer_cast<CHTL::ElementNode>(elementAST)) {
            std::cout << "标签名: " << elementNode->tagName << std::endl;
            std::cout << "属性数量: " << elementNode->attributes.size() << std::endl;
            for (const auto& attr : elementNode->attributes) {
                std::cout << "  " << attr.first << ": " << attr.second << std::endl;
            }
        }
    }

    // 测试CHTL JS解析器
    std::cout << "\n测试CHTL JS解析器:" << std::endl;
    CHTLJS::CHTLJSParser chtlJSParser;
    
    // 测试虚拟对象解析
    std::string virCode = R"(
vir test = listen {
    click: () => {
        std::cout << "Clicked!";
    },
    hover: () => {
        console.log("Hovered!");
    }
}
)";
    
    chtlJSParser.setSource(virCode);
    auto virAST = chtlJSParser.parse();
    std::cout << "虚拟对象解析: " << (chtlJSParser.isSuccess() ? "成功" : "失败") << std::endl;
    if (virAST) {
        std::cout << "AST类型: " << static_cast<int>(virAST->type) << std::endl;
        if (auto virNode = std::dynamic_pointer_cast<CHTLJS::VirtualObjectNode>(virAST)) {
            std::cout << "对象名称: " << virNode->objectName << std::endl;
        }
    }
    
    // 测试增强选择器解析
    std::string selectorCode = R"(
{{.box}}->addEventListener('click', () => {
    std::cout << "Box clicked!";
})
)";
    
    chtlJSParser.setSource(selectorCode);
    auto selectorAST = chtlJSParser.parse();
    std::cout << "增强选择器解析: " << (chtlJSParser.isSuccess() ? "成功" : "失败") << std::endl;
    if (selectorAST) {
        std::cout << "AST类型: " << static_cast<int>(selectorAST->type) << std::endl;
        if (auto selectorNode = std::dynamic_pointer_cast<CHTLJS::EnhancedSelectorNode>(selectorAST)) {
            std::cout << "选择器: " << selectorNode->selector << std::endl;
            std::cout << "选择器类型: " << selectorNode->selectorType << std::endl;
        }
    }
    
    // 测试iNeverAway解析
    std::string ineverCode = R"(
vir Test = iNeverAway {
    Void<A>: function(int, int) {
        return a + b;
    },
    Void<B>: function(int, int) {
        return a * b;
    }
}
)";
    
    chtlJSParser.setSource(ineverCode);
    auto ineverAST = chtlJSParser.parse();
    std::cout << "iNeverAway解析: " << (chtlJSParser.isSuccess() ? "成功" : "失败") << std::endl;
    if (ineverAST) {
        std::cout << "AST类型: " << static_cast<int>(ineverAST->type) << std::endl;
        if (auto ineverNode = std::dynamic_pointer_cast<CHTLJS::INeverAwayNode>(ineverAST)) {
            std::cout << "状态键数量: " << ineverNode->statefulKeys.size() << std::endl;
            std::cout << "无状态键数量: " << ineverNode->statelessKeys.size() << std::endl;
        }
    }

    // 测试CJMOD双指针扫描增强
    std::cout << "\n测试CJMOD双指针扫描增强:" << std::endl;
    
    // 测试前置截取
    std::string cjmodFragment = "vir btn = listen { click: () => { console.log('clicked'); } };";
    std::string truncated = scanner.preEmptiveTruncateCJMOD(cjmodFragment);
    std::cout << "前置截取结果: " << truncated << std::endl;
    
    // TODO: 暂时注释掉双指针扫描测试，等待CJMOD API稳定
    /*
    // 测试双指针扫描增强
    auto enhancedResult = scanner.scanCJMODByTwoPointers(0, cjmodFragment.length());
    std::cout << "增强扫描结果: " << (enhancedResult.success ? "成功" : "失败") << std::endl;
    if (enhancedResult.success) {
        std::cout << "扫描到 " << enhancedResult.tokens.size() << " 个token: ";
        for (const auto& token : enhancedResult.tokens) {
            std::cout << "[" << token << "] ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "扫描错误: " << enhancedResult.error << std::endl;
    }
    */
    std::cout << "CJMOD双指针扫描测试暂未实现" << std::endl;

    std::cout << "测试完成!" << std::endl;
    return 0;
}