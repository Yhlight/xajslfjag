#include <iostream>
#include <string>
#include "../Scanner/Scanner.h"

using namespace CHTL;

void TestBasicScanning() {
    std::cout << "=== 测试基础扫描功能 ===\n";
    
    std::string source = R"(
div {
    style {
        .box {
            width: 100px;
        }
    }
    
    script {
        {{.box}}.addEventListener('click', () => {
            console.log('clicked');
        });
    }
}
)";
    
    Scanner scanner;
    scanner.SetSource(source);
    scanner.SetDebugMode(true);
    
    while (!scanner.IsEOF()) {
        auto fragment = scanner.NextFragment();
        if (fragment.type != FragmentType::UNKNOWN) {
            std::cout << "片段类型: " << static_cast<int>(fragment.type) 
                      << ", 内容长度: " << fragment.content.length() 
                      << ", 上下文: " << fragment.context << "\n";
        }
    }
}

void TestCHTLJSSplitting() {
    std::cout << "\n=== 测试CHTL JS精准切割 ===\n";
    
    std::string source = R"(
script {
    {{box}}->textContent = "Hello";
    const result = 3 ** 4;
    {{.nav}}->listen {
        click: () => { }
    };
}
)";
    
    Scanner scanner;
    scanner.SetSource(source);
    
    while (!scanner.IsEOF()) {
        auto fragment = scanner.NextFragment();
        if (fragment.type == FragmentType::CHTLJS) {
            std::cout << "CHTL JS片段: \"" << fragment.content << "\"\n";
        }
    }
}

void TestLocalStyle() {
    std::cout << "\n=== 测试局部样式块 ===\n";
    
    std::string source = R"(
div {
    style {
        width: 100px;  // 内联样式
        
        .box {  // 自动添加类名
            color: red;
        }
        
        #main {  // 自动添加ID
            background: blue;
        }
        
        &:hover {  // 引用选择器
            opacity: 0.8;
        }
    }
}
)";
    
    Scanner scanner;
    scanner.SetSource(source);
    
    while (!scanner.IsEOF()) {
        auto fragment = scanner.NextFragment();
        if (fragment.type == FragmentType::CSS && fragment.context.find("local:") == 0) {
            std::cout << "局部样式片段: \n" << fragment.content << "\n";
        }
    }
}

int main() {
    TestBasicScanning();
    TestCHTLJSSplitting();
    TestLocalStyle();
    
    return 0;
}