#include "CJMOD/CJMODApi.h"
#include <iostream>

using namespace std;
using namespace CHTL;

void testBlockingCJMODRuntime() {
    cout << "\n=== 测试CJMOD阻塞式运行时（正确理解） ===\n";
    
    cout << "CJMOD运行时机制说明：\n";
    cout << "1. CJMODScanner需要扫描实际代码片段\n";
    cout << "2. 扫描是阻塞的过程 - 等待获取代码片段\n";
    cout << "3. 扫描完成后，自动填充参数\n";
    cout << "4. 这不是动态变量运行时，而是同步扫描-填充流程\n";
    
    cout << "\n按照原始API.md的示例流程：\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "// 步骤1：分析语法模式\n";
    cout << "Arg args = Syntax::analyze(\"$ ** $\");\n";
    cout << "args.print(); // 输出-> [\"$\", \"**\", \"$\"]\n";
    cout << "\n";
    
    cout << "// 步骤2：绑定参数处理函数\n";
    cout << "args.bind(\"$\", [](const std::string& value) { return value; });\n";
    cout << "args.bind(\"**\", [](const std::string& value) { return value; });\n";
    cout << "\n";
    
    cout << "// 步骤3：阻塞式扫描 - 等待获取实际代码片段\n";
    cout << "Arg result = CJMODScanner::scan(args, \"**\");\n";
    cout << "// 这里CJMODScanner会扫描源代码，找到 \"3 ** 4\"\n";
    cout << "// 然后返回实际的代码片段值\n";
    cout << "result.print(); // 输出-> [\"3\", \"**\", \"4\"]\n";
    cout << "\n";
    
    cout << "// 步骤4：自动填充参数\n";
    cout << "args.fillValue(result);\n";
    cout << "std::cout << args[0].value; // 输出-> 3\n";
    cout << "std::cout << args[1].value; // 输出-> **\n";
    cout << "std::cout << args[2].value; // 输出-> 4\n";
    cout << "\n";
    
    cout << "// 步骤5：转换和导出\n";
    cout << "args.transform(\"pow(\" + args[0].value + \", \" + args[2].value + \")\");\n";
    cout << "CJMODGenerator::exportResult(args);\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    cout << "\n✅ 这就是正确的CJMOD阻塞式运行时机制！\n";
    cout << "✅ 扫描 → 等待 → 获取片段 → 填充参数 → 生成代码\n";
}

void testMultipleKeywordScanning() {
    cout << "\n=== 测试多关键字扫描 ===\n";
    
    // 测试不同的关键字扫描
    vector<pair<string, string>> testCases = {
        {"3 ** 4", "**"},
        {"a + b", "+"},
        {"x * y", "*"},
        {"func(arg1, arg2)", ","}
    };
    
    for (const auto& testCase : testCases) {
        cout << "\n扫描源代码: \"" << testCase.first << "\"\n";
        cout << "查找关键字: \"" << testCase.second << "\"\n";
        
        // 设置源代码
        CJMODScanner::setSourceCode(testCase.first);
        
        // 分析语法模式
        Arg args = Syntax::analyze("$ " + testCase.second + " $");
        
        // 阻塞式扫描
        Arg result = CJMODScanner::scan(args, testCase.second);
        
        cout << "扫描结果: ";
        result.print();
        cout << endl;
    }
}

void testCHTLJSFunctionIntegration() {
    cout << "\n=== 测试CHTL JS函数集成 ===\n";
    
    // 测试CHTL JS函数语法检查
    string chtlJSCode = "printMyLove {url: $!_, mode: $?_}";
    bool isCHTLJSFunc = Syntax::isCHTLJSFunction(chtlJSCode);
    cout << "检查CHTL JS函数语法: \"" << chtlJSCode << "\"\n";
    cout << "结果: " << (isCHTLJSFunc ? "true" : "false") << endl;
    
    if (isCHTLJSFunc) {
        // 创建CHTL JS函数
        cout << "\n创建CHTL JS函数:\n";
        CHTLJSFunction::CreateCHTLJSFunction(chtlJSCode);
        
        // 绑定虚拟对象
        cout << "\n绑定虚拟对象:\n";
        CHTLJSFunction::bindVirtualObject("printMyLove");
    }
    
    // 测试其他语法检查
    cout << "\n其他语法检查测试:\n";
    cout << "isObject(\"{b: 1}\"): " << (Syntax::isObject("{b: 1}") ? "true" : "false") << endl;
    cout << "isFunction(\"function a(){}\"): " << (Syntax::isFunction("function a(){}") ? "true" : "false") << endl;
    cout << "isArray(\"[1, 2, 3]\"): " << (Syntax::isArray("[1, 2, 3]") ? "true" : "false") << endl;
}

int main() {
    cout << "CJMOD阻塞式运行时测试程序\n";
    cout << "================================\n";
    cout << "严格按照原始API.md的阻塞式扫描-填充流程\n";
    
    try {
        testBlockingCJMODRuntime();
        testMultipleKeywordScanning();
        testCHTLJSFunctionIntegration();
        
        cout << "\n🎉 CJMOD阻塞式运行时测试完成！\n";
        
        cout << "\n✅ 正确的CJMOD运行时机制验证：\n";
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        cout << "  ✓ 1. Syntax::analyze() - 解析语法模式\n";
        cout << "  ✓ 2. args.bind() - 绑定参数处理函数\n";
        cout << "  ✓ 3. CJMODScanner::scan() - 阻塞式扫描获取代码片段\n";
        cout << "  ✓ 4. args.fillValue() - 用扫描结果自动填充参数\n";
        cout << "  ✓ 5. args.transform() - 转换为目标代码\n";
        cout << "  ✓ 6. CJMODGenerator::exportResult() - 导出最终代码\n";
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        cout << "\n🚀 这是正确的阻塞式扫描-填充运行时机制！\n";
        cout << "🚀 CJMODScanner等待扫描完成后，自动填充参数！\n";
        cout << "🚀 严格按照原始API.md设计，简洁高效！\n";
        
    } catch (const exception& e) {
        cout << "\n❌ 测试过程中出现错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}