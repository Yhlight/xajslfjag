#include <iostream>
#include <string>
#include <memory>
#include "CHTLScanner/UnifiedScanner.h"
#include "CHTLCompiler/CompilerDispatcher.h"
#include "CHTLManage/ModuleManager.h"

using namespace std;
using namespace CHTL;

void testUnifiedScanner() {
    cout << "\n=== 测试统一扫描器 ===" << endl;
    
    CHTLUnifiedScanner scanner;
    
    string testCode = R"(
[Template] @Style DefaultButton {
    color: #333;
    padding: 10px;
}

div {
    style {
        .btn {
            @Style DefaultButton;
            background: #007bff;
        }
        
        &:hover {
            background: #0056b3;
        }
    }
    
    script {
        {{.btn}}->addEventListener('click', () => {
            console.log('Button clicked!');
        });
        
        {{.btn}} &-> click {
            alert('CHTL JS Event!');
        }
    }
}
)";
    
    auto fragments = scanner.scan(testCode);
    
    cout << "扫描结果:" << endl;
    cout << "片段数量: " << fragments.size() << endl;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        cout << "片段 " << i + 1 << ":" << endl;
        cout << "  类型: ";
        switch (fragments[i].type) {
            case FragmentType::CHTL: cout << "CHTL"; break;
            case FragmentType::CHTL_JS: cout << "CHTL JS"; break;
            case FragmentType::CSS: cout << "CSS"; break;
            case FragmentType::JAVASCRIPT: cout << "JavaScript"; break;
            case FragmentType::HTML: cout << "HTML"; break;
            default: cout << "Unknown"; break;
        }
        cout << endl;
        cout << "  内容长度: " << fragments[i].content.length() << " 字符" << endl;
        cout << "  位置: " << fragments[i].startPosition << "-" << fragments[i].endPosition << endl;
        cout << endl;
    }
    
    // 生成扫描报告
    string report = scanner.generateScanReport(fragments);
    cout << report << endl;
    
    cout << "✓ 统一扫描器测试完成" << endl;
}

void testCompilerDispatcher() {
    cout << "\n=== 测试编译器调度器 ===" << endl;
    
    CompilerDispatcher dispatcher;
    
    string testCode = R"(
[Template] @Var ThemeColors {
    primary: #007bff;
    secondary: #6c757d;
    success: #28a745;
}

div {
    style {
        .card {
            background: ThemeColors(primary);
            border: 1px solid ThemeColors(secondary);
            padding: 16px;
            border-radius: 8px;
        }
        
        &:hover {
            background: ThemeColors(success);
        }
    }
    
    script {
        {{.card}} &-> click {
            console.log('Card clicked with color:', 'ThemeColors(primary)');
        }
    }
    
    text {
        "Welcome to CHTL!"
    }
}
)";
    
    // 编译源代码
    auto result = dispatcher.compileSource(testCode);
    
    cout << "编译结果:" << endl;
    cout << "成功: " << (result.success ? "是" : "否") << endl;
    
    if (result.success) {
        cout << "生成的HTML长度: " << result.output.length() << " 字符" << endl;
        cout << "\n生成的HTML内容:" << endl;
        cout << result.output << endl;
    } else {
        cout << "编译错误: " << result.errorMessage << endl;
    }
    
    // 获取编译错误
    auto errors = dispatcher.getCompilationErrors();
    if (!errors.empty()) {
        cout << "\n编译器错误列表:" << endl;
        for (const auto& error : errors) {
            cout << "  - " << error << endl;
        }
    }
    
    cout << "✓ 编译器调度器测试完成" << endl;
}

void testModuleManager() {
    cout << "\n=== 测试模块管理器 ===" << endl;
    
    ModuleManager moduleManager;
    
    // 设置模块搜索路径
    moduleManager.addModuleSearchPath("./modules");
    moduleManager.addModuleSearchPath("./test_modules");
    
    cout << "模块搜索路径:" << endl;
    auto searchPaths = moduleManager.getModuleSearchPaths();
    for (const auto& path : searchPaths) {
        cout << "  " << path << endl;
    }
    
    // 测试官方模块前缀
    string officialModule = "chtl::theme";
    cout << "\n测试官方模块前缀:" << endl;
    cout << "模块名: " << officialModule << endl;
    cout << "是否为官方模块: " << (moduleManager.isOfficialModulePrefix(officialModule) ? "是" : "否") << endl;
    
    if (moduleManager.isOfficialModulePrefix(officialModule)) {
        string parsedName = moduleManager.parseOfficialModuleName(officialModule);
        cout << "解析后的名称: " << parsedName << endl;
    }
    
    // 测试模块搜索
    string foundModule = moduleManager.findModule("theme", ModuleType::CMOD);
    cout << "\n搜索模块 'theme':" << endl;
    cout << "找到路径: " << (foundModule.empty() ? "未找到" : foundModule) << endl;
    
    // 生成模块报告
    string report = moduleManager.generateModuleReport();
    cout << "\n" << report << endl;
    
    cout << "✓ 模块管理器测试完成" << endl;
}

void testCHTLJSFeatures() {
    cout << "\n=== 测试CHTL JS特性 ===" << endl;
    
    CHTLUnifiedScanner scanner;
    CompilerDispatcher dispatcher;
    
    string chtlJSCode = R"(
script {
    // 增强选择器测试
    {{.button}}->textContent = "Click Me!";
    {{#submitBtn}}->style.display = "block";
    
    // 事件监听器测试
    {{.button}}->listen {
        click: () => {
            console.log('Button clicked!');
        },
        
        mouseenter: function() {
            this.style.background = '#0056b3';
        },
        
        mouseleave: () => {
            this.style.background = '#007bff';
        }
    };
    
    // 事件委托测试
    {{document}}->delegate {
        target: {{.dynamic-button}},
        click: (e) => {
            alert('Dynamic button clicked!');
        },
        mouseenter: (e) => {
            e.target.style.opacity = '0.8';
        }
    };
    
    // 动画测试
    const fadeIn = animate {
        target: {{.content}},
        duration: 500,
        easing: ease-in-out,
        
        begin: {
            opacity: 0,
            transform: 'translateY(20px)'
        },
        
        end: {
            opacity: 1,
            transform: 'translateY(0)'
        }
    };
    
    // 虚对象测试
    vir buttonActions = listen {
        click: () => {
            console.log('Virtual object click');
        },
        
        getData: () => {
            return { id: 1, name: 'test' };
        }
    };
    
    // 使用虚对象
    buttonActions->click();
    const data = buttonActions->getData();
    
    // 事件绑定操作符测试
    {{.submit}} &-> click {
        const form = {{#myForm}};
        form.submit();
    }
    
    // 模块导入测试
    module {
        load: ./utils.cjjs,
        load: ./components.cjjs
    }
}
)";
    
    auto fragments = scanner.scan(chtlJSCode);
    
    cout << "CHTL JS片段分析:" << endl;
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL_JS) {
            cout << "  发现CHTL JS片段，长度: " << fragment.content.length() << endl;
            
            // 分解为最小单元
            auto minimalUnits = scanner.splitCHTLJSToMinimalUnits(fragment);
            cout << "    最小单元数量: " << minimalUnits.size() << endl;
        }
    }
    
    // 编译CHTL JS代码
    auto results = dispatcher.compileFragments(fragments);
    
    cout << "\nCHTL JS编译结果:" << endl;
    for (const auto& result : results) {
        if (result.sourceType == FragmentType::CHTL_JS) {
            cout << "  编译状态: " << (result.success ? "成功" : "失败") << endl;
            if (result.success) {
                cout << "  输出长度: " << result.output.length() << " 字符" << endl;
            } else {
                cout << "  错误: " << result.errorMessage << endl;
            }
        }
    }
    
    cout << "✓ CHTL JS特性测试完成" << endl;
}

void testSmartSlicing() {
    cout << "\n=== 测试智能切片 ===" << endl;
    
    CHTLUnifiedScanner scanner;
    
    string largeCode = R"(
[Template] @Style ButtonTheme {
    background: linear-gradient(45deg, #007bff, #0056b3);
    border: none;
    border-radius: 8px;
    padding: 12px 24px;
    color: white;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.3s ease;
}

[Template] @Element Card {
    div {
        class: card;
        
        style {
            .card {
                background: white;
                border-radius: 12px;
                box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
                padding: 24px;
                margin: 16px 0;
            }
            
            &:hover {
                box-shadow: 0 8px 15px rgba(0, 0, 0, 0.15);
                transform: translateY(-2px);
            }
        }
    }
}

body {
    @Element Card {
        h2 {
            text { "Welcome to CHTL" }
        }
        
        button {
            style {
                @Style ButtonTheme;
                
                &:hover {
                    transform: scale(1.05);
                }
                
                &:active {
                    transform: scale(0.95);
                }
            }
            
            script {
                {{button}}->listen {
                    click: () => {
                        {{h2}}->textContent = "Button Clicked!";
                        
                        animate {
                            target: {{h2}},
                            duration: 300,
                            begin: { opacity: 0.5 },
                            end: { opacity: 1 }
                        };
                    }
                };
            }
            
            text { "Click Me!" }
        }
    }
}
)";
    
    // 测试智能切片
    auto fragments = scanner.smartSlice(largeCode, 512); // 512字符切片
    
    cout << "智能切片结果:" << endl;
    cout << "原始代码长度: " << largeCode.length() << " 字符" << endl;
    cout << "切片数量: " << fragments.size() << endl;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        cout << "切片 " << i + 1 << ":" << endl;
        cout << "  长度: " << fragments[i].content.length() << " 字符" << endl;
        cout << "  类型: ";
        switch (fragments[i].type) {
            case FragmentType::CHTL: cout << "CHTL"; break;
            case FragmentType::CHTL_JS: cout << "CHTL JS"; break;
            case FragmentType::CSS: cout << "CSS"; break;
            case FragmentType::JAVASCRIPT: cout << "JavaScript"; break;
            case FragmentType::HTML: cout << "HTML"; break;
            default: cout << "Unknown"; break;
        }
        cout << endl;
    }
    
    // 验证切片完整性
    bool integrity = scanner.validateFragmentIntegrity(fragments, largeCode);
    cout << "\n切片完整性验证: " << (integrity ? "通过" : "失败") << endl;
    
    cout << "✓ 智能切片测试完成" << endl;
}

int main() {
    cout << "=== CHTL统一系统测试 ===" << endl;
    
    try {
        testUnifiedScanner();
        testCompilerDispatcher();
        testModuleManager();
        testCHTLJSFeatures();
        testSmartSlicing();
        
        cout << "\n=== 所有统一系统测试完成 ===" << endl;
        cout << "🎉 CHTL编译器架构测试成功！" << endl;
        
    } catch (const exception& e) {
        cout << "测试过程中发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}