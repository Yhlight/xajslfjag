#include <iostream>
#include "../CHTLJS/CJMODSystem/SimpleCJMOD.h"

using namespace CHTL::CJMOD;

/**
 * 测试简化的CJMOD API
 * 验证记忆中简单强大的设计
 */

void testCJMODAPI() {
    std::cout << "=== 测试简化的CJMOD API ===" << std::endl;
    
    // 使用案例：完全按照记忆中的简单API
    std::cout << "\n1. 语法分析:" << std::endl;
    Arg args = Syntax::analyze("$ ** $");  // 语法分析
    args.print();  // 输出-> ["$", "**", "$"]
    
    std::cout << "\n2. 绑定函数:" << std::endl;
    args.bind("$", [](const std::string& value) {
        return value;
    });
    
    args.bind("**", [](const std::string& value) {
        return value;
    });
    
    args.bind("$", [](const std::string& value) {
        return value;
    });
    
    std::cout << "\n3. 扫描结果:" << std::endl;
    Arg result = CJMODScanner::scan(args, "**");
    result.print();  // 输出-> ["3", "**", "4"]
    
    std::cout << "\n4. 填充值:" << std::endl;
    args.fillValue(result);
    const auto& tokens = args.getTokens();
    std::cout << "arg[0].value: " << tokens[0] << std::endl;  // 输出-> 3
    std::cout << "arg[1].value: " << tokens[1] << std::endl;  // 输出-> **
    std::cout << "arg[2].value: " << tokens[2] << std::endl;  // 输出-> 4
    
    std::cout << "\n5. 转换:" << std::endl;
    args.transform("pow(" + tokens[0] + ", " + tokens[2] + ")");
    
    std::cout << "\n6. 导出结果:" << std::endl;
    std::string generated_js = CJMODGenerator::exportResult(args);
    std::cout << "生成的JavaScript: " << generated_js << std::endl;
    
    std::cout << "\n7. 包装HTML:" << std::endl;
    std::string html = CJMODGenerator::wrapInScriptTag(generated_js);
    std::cout << "生成的HTML:\n" << html << std::endl;
}

void testCHTLJSFunction() {
    std::cout << "\n=== 测试CHTL JS函数API ===" << std::endl;
    
    // 测试CreateCHTLJSFunction
    std::cout << "\n1. 创建CHTL JS函数:" << std::endl;
    bool success = CHTLJSFunction::CreateCHTLJSFunction("printMyLove {url: $!_, mode: $?_}");
    std::cout << "创建printMyLove函数: " << (success ? "成功" : "失败") << std::endl;
    
    // 测试虚对象绑定
    std::cout << "\n2. 虚对象支持测试:" << std::endl;
    bool supports_vir = CHTLJSFunction::supportsVirtualObject("printMyLove");
    std::cout << "printMyLove支持虚对象: " << (supports_vir ? "是" : "否") << std::endl;
    
    // 测试手动绑定
    std::cout << "\n3. 手动绑定虚对象:" << std::endl;
    bool manual_bind = CHTLJSFunction::bindVirtualObject("customFunction");
    std::cout << "手动绑定customFunction: " << (manual_bind ? "成功" : "失败") << std::endl;
}

void testSyntaxDetection() {
    std::cout << "\n=== 测试语法检测API ===" << std::endl;
    
    // 测试各种语法检测
    std::cout << "\n1. JavaScript对象检测:" << std::endl;
    bool is_obj = Syntax::isObject("{b: 1}");
    std::cout << "{b: 1} 是对象: " << (is_obj ? "是" : "否") << std::endl;
    
    std::cout << "\n2. JavaScript函数检测:" << std::endl;
    bool is_func = Syntax::isFunction("function a(){}");
    std::cout << "function a(){} 是函数: " << (is_func ? "是" : "否") << std::endl;
    
    std::cout << "\n3. JavaScript数组检测:" << std::endl;
    bool is_array = Syntax::isArray("[1, 2, 3]");
    std::cout << "[1, 2, 3] 是数组: " << (is_array ? "是" : "否") << std::endl;
    
    std::cout << "\n4. CHTL JS函数检测:" << std::endl;
    bool is_chtljs = Syntax::isCHTLJSFunction("test {test: 1, test2: 2};");
    std::cout << "test {test: 1, test2: 2}; 是CHTL JS函数: " << (is_chtljs ? "是" : "否") << std::endl;
}

int main() {
    std::cout << "🎉 CJMOD API简化测试程序启动！" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    try {
        // 测试核心CJMOD API
        testCJMODAPI();
        
        // 测试CHTL JS函数
        testCHTLJSFunction();
        
        // 测试语法检测
        testSyntaxDetection();
        
        std::cout << "\n==========================================" << std::endl;
        std::cout << "✅ CJMOD API简化测试完成！" << std::endl;
        std::cout << "🚀 新的CJMOD API特点:" << std::endl;
        std::cout << "   - 极其简单的语法分析 Syntax::analyze()" << std::endl;
        std::cout << "   - 强大的绑定机制 args.bind()" << std::endl;
        std::cout << "   - 高效的扫描接口 CJMODScanner::scan()" << std::endl;
        std::cout << "   - 简洁的代码生成 CJMODGenerator::exportResult()" << std::endl;
        std::cout << "   - 天然虚对象支持 CHTLJSFunction::CreateCHTLJSFunction()" << std::endl;
        std::cout << "   - 内部实现复杂，使用极其简单！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}