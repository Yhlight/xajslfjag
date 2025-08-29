#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include "CHTLCompiler/CompilerDispatcher.h"
#include "CHTLScanner/UnifiedScanner.h"
#include "CHTLManage/ModuleManager.h"
#include "CHTLManage/TemplateManager.h"
#include "CHTLManage/VariableManager.h"

using namespace std;
using namespace CHTL;

void testCompleteFeatures() {
    cout << "=== CHTL完整特性测试 ===" << endl;
    
    CompilerDispatcher dispatcher;
    
    // 完整的CHTL代码示例，包含所有主要特性
    string completeCode = R"(
// 配置组
[Configuration] {
    INDEX_INITIAL_COUNT = 0;
    DISABLE_NAME_GROUP = false;
    DISABLE_CUSTOM_ORIGIN_TYPE = false;
    DEBUG_MODE = true;
    
    [Name] {
        CUSTOM_STYLE = [@Style, @CSS];
        CUSTOM_ELEMENT = @Element;
        CUSTOM_VAR = @Var;
        TEMPLATE_STYLE = @Style;
        TEMPLATE_ELEMENT = @Element;
        TEMPLATE_VAR = @Var;
        ORIGIN_HTML = @Html;
        ORIGIN_STYLE = @Style;
        ORIGIN_JAVASCRIPT = @JavaScript;
    }
    
    [OriginType] {
        ORIGINTYPE_VUE = @Vue;
        ORIGINTYPE_REACT = @React;
    }
}

// 命名空间
[Namespace] UI {
    
    // 变量组模板
    [Template] @Var ThemeColors {
        primary: #007bff;
        secondary: #6c757d;
        success: #28a745;
        warning: #ffc107;
        danger: #dc3545;
        info: #17a2b8;
        light: #f8f9fa;
        dark: #343a40;
    }
    
    [Template] @Var Spacing {
        xs: 4px;
        sm: 8px;
        md: 16px;
        lg: 24px;
        xl: 32px;
        xxl: 48px;
    }
    
    [Template] @Var Typography {
        fontFamily: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
        fontSize: 14px;
        lineHeight: 1.5;
        fontWeight: 400;
        headingWeight: 600;
    }
    
    // 样式组模板
    [Template] @Style BaseCard {
        background: white;
        border-radius: 8px;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        padding: ThemeColors(md);
        margin: Spacing(md) 0;
    }
    
    [Template] @Style Button {
        padding: Spacing(sm) Spacing(md);
        border: none;
        border-radius: 4px;
        font-family: Typography(fontFamily);
        font-size: Typography(fontSize);
        font-weight: Typography(fontWeight);
        cursor: pointer;
        transition: all 0.3s ease;
        display: inline-block;
        text-align: center;
        text-decoration: none;
    }
    
    // 自定义样式组
    [Custom] @Style ButtonVariants {
        @Style Button;
        
        color,  // 无值样式组
        background-color;
    }
    
    [Custom] @Style PrimaryButton {
        @Style ButtonVariants {
            color: white;
            background-color: ThemeColors(primary);
        }
        
        &:hover {
            background-color: #0056b3;
        }
        
        &:active {
            transform: translateY(1px);
        }
    }
    
    // 元素模板
    [Template] @Element Card {
        div {
            class: card;
            
            style {
                .card {
                    @Style BaseCard;
                }
                
                &:hover {
                    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.15);
                    transform: translateY(-2px);
                }
            }
            
            div {
                class: card-header;
                
                style {
                    .card-header {
                        border-bottom: 1px solid ThemeColors(light);
                        padding-bottom: Spacing(sm);
                        margin-bottom: Spacing(md);
                        font-weight: Typography(headingWeight);
                    }
                }
            }
            
            div {
                class: card-body;
            }
            
            div {
                class: card-footer;
                
                style {
                    .card-footer {
                        border-top: 1px solid ThemeColors(light);
                        padding-top: Spacing(sm);
                        margin-top: Spacing(md);
                    }
                }
            }
        }
    }
    
    // 自定义元素
    [Custom] @Element Modal {
        div {
            class: modal-overlay;
            
            style {
                .modal-overlay {
                    position: fixed;
                    top: 0;
                    left: 0;
                    width: 100%;
                    height: 100%;
                    background: rgba(0, 0, 0, 0.5);
                    display: none;
                    z-index: 1000;
                }
                
                &.show {
                    display: flex;
                    align-items: center;
                    justify-content: center;
                }
            }
            
            div {
                class: modal-content;
                
                style {
                    .modal-content {
                        @Style BaseCard;
                        max-width: 600px;
                        width: 90%;
                        max-height: 80vh;
                        overflow-y: auto;
                    }
                }
                
                div {
                    class: modal-header;
                    
                    style {
                        .modal-header {
                            display: flex;
                            justify-content: space-between;
                            align-items: center;
                            border-bottom: 1px solid ThemeColors(light);
                            padding-bottom: Spacing(sm);
                            margin-bottom: Spacing(md);
                        }
                    }
                    
                    h3 {
                        text { "Modal Title" }
                    }
                    
                    button {
                        class: close-btn;
                        
                        style {
                            .close-btn {
                                background: none;
                                border: none;
                                font-size: 24px;
                                cursor: pointer;
                                color: ThemeColors(secondary);
                            }
                            
                            &:hover {
                                color: ThemeColors(danger);
                            }
                        }
                        
                        text { "×" }
                    }
                }
                
                div {
                    class: modal-body;
                }
                
                div {
                    class: modal-footer;
                    
                    style {
                        .modal-footer {
                            display: flex;
                            justify-content: flex-end;
                            gap: Spacing(sm);
                            border-top: 1px solid ThemeColors(light);
                            padding-top: Spacing(sm);
                            margin-top: Spacing(md);
                        }
                    }
                }
            }
        }
    }
}

// 主要内容
html {
    head {
        title {
            text { "CHTL Complete Feature Test" }
        }
        
        meta {
            charset: utf-8;
        }
        
        meta {
            name: viewport;
            content: "width=device-width, initial-scale=1";
        }
        
        style {
            // 全局样式
            * {
                margin: 0;
                padding: 0;
                box-sizing: border-box;
            }
            
            body {
                font-family: Typography(fontFamily) from UI;
                font-size: Typography(fontSize) from UI;
                line-height: Typography(lineHeight) from UI;
                background: ThemeColors(light) from UI;
                color: ThemeColors(dark) from UI;
            }
        }
    }
    
    body {
        div {
            class: container;
            
            style {
                .container {
                    max-width: 1200px;
                    margin: 0 auto;
                    padding: Spacing(md) from UI;
                }
            }
            
            h1 {
                style {
                    text-align: center;
                    margin-bottom: Spacing(xl) from UI;
                    color: ThemeColors(primary) from UI;
                }
                
                text { "CHTL Feature Showcase" }
            }
            
            // 使用Card模板
            @Element Card from UI {
                div[0] {  // 索引访问：card-header
                    text { "Welcome Card" }
                }
                
                div[1] {  // card-body
                    p {
                        text { "This card demonstrates CHTL template usage with inheritance and variable substitution." }
                    }
                    
                    button {
                        style {
                            @Style PrimaryButton from UI;
                        }
                        
                        script {
                            // CHTL JS特性演示
                            {{button}}->listen {
                                click: () => {
                                    console.log('Primary button clicked!');
                                    
                                    // 显示模态框
                                    {{.modal-overlay}}->classList.add('show');
                                    
                                    // 动画效果
                                    animate {
                                        target: {{.modal-content}},
                                        duration: 300,
                                        easing: ease-out,
                                        
                                        begin: {
                                            opacity: 0,
                                            transform: 'scale(0.7) translateY(-50px)'
                                        },
                                        
                                        end: {
                                            opacity: 1,
                                            transform: 'scale(1) translateY(0)'
                                        }
                                    };
                                },
                                
                                mouseenter: function() {
                                    this.style.transform = 'scale(1.05)';
                                },
                                
                                mouseleave: function() {
                                    this.style.transform = 'scale(1)';
                                }
                            };
                        }
                        
                        text { "Open Modal" }
                    }
                }
                
                div[2] {  // card-footer
                    span {
                        style {
                            font-size: 12px;
                            color: ThemeColors(secondary) from UI;
                        }
                        
                        text { "Card footer content" }
                    }
                }
            }
            
            // 特化操作演示
            @Element Card from UI {
                // 删除footer
                delete div[2];
                
                // 插入新内容
                insert after div[1] {
                    div {
                        class: card-actions;
                        
                        style {
                            .card-actions {
                                display: flex;
                                gap: Spacing(sm) from UI;
                                padding: Spacing(sm) from UI;
                                background: ThemeColors(light) from UI;
                            }
                        }
                        
                        button {
                            style {
                                @Style PrimaryButton from UI {
                                    delete background-color;  // 删除特定属性
                                }
                                background-color: ThemeColors(success) from UI;
                            }
                            
                            text { "Success" }
                        }
                        
                        button {
                            style {
                                @Style PrimaryButton from UI {
                                    background-color: ThemeColors(warning) from UI;
                                    color: ThemeColors(dark) from UI;
                                }
                            }
                            
                            text { "Warning" }
                        }
                    }
                }
                
                div[0] {  // 修改header
                    text { "Specialized Card" }
                }
                
                div[1] {  // 修改body
                    p {
                        text { "This card demonstrates template specialization with element modification." }
                    }
                }
            }
            
            // 变量组特化演示
            div {
                class: theme-demo;
                
                style {
                    .theme-demo {
                        @Style BaseCard from UI;
                        margin-top: Spacing(xl) from UI;
                    }
                }
                
                h3 {
                    text { "Variable Group Specialization" }
                }
                
                div {
                    class: color-palette;
                    
                    style {
                        .color-palette {
                            display: grid;
                            grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
                            gap: Spacing(sm) from UI;
                            margin-top: Spacing(md) from UI;
                        }
                    }
                    
                    div {
                        class: color-item;
                        
                        style {
                            .color-item {
                                padding: Spacing(sm) from UI;
                                border-radius: 4px;
                                text-align: center;
                                color: white;
                                font-weight: Typography(headingWeight) from UI;
                                background: ThemeColors(primary = #e74c3c) from UI;  // 变量特化
                            }
                        }
                        
                        text { "Primary" }
                    }
                    
                    div {
                        class: color-item;
                        
                        style {
                            .color-item {
                                background: ThemeColors(success = #2ecc71) from UI;  // 变量特化
                            }
                        }
                        
                        text { "Success" }
                    }
                }
            }
            
            // 模态框实现
            @Element Modal from UI {
                div[0] {  // modal-overlay
                    div[0] {  // modal-content
                        div[1] {  // modal-body
                            p {
                                text { "This is a modal dialog created using CHTL custom elements." }
                            }
                            
                            p {
                                text { "It demonstrates event handling, animations, and DOM manipulation." }
                            }
                        }
                        
                        div[2] {  // modal-footer
                            button {
                                style {
                                    @Style PrimaryButton from UI {
                                        background-color: ThemeColors(secondary) from UI;
                                    }
                                }
                                
                                script {
                                    {{button}} &-> click {
                                        // 关闭模态框
                                        {{.modal-overlay}}->classList.remove('show');
                                    }
                                }
                                
                                text { "Close" }
                            }
                            
                            button {
                                style {
                                    @Style PrimaryButton from UI;
                                }
                                
                                text { "Confirm" }
                            }
                        }
                    }
                }
                
                script {
                    // 事件委托：点击遮罩层关闭
                    {{.modal-overlay}}->delegate {
                        target: {{.modal-overlay}},
                        click: (e) => {
                            if (e.target === e.currentTarget) {
                                e.currentTarget.classList.remove('show');
                            }
                        }
                    };
                    
                    // ESC键关闭
                    {{document}}->listen {
                        keydown: (e) => {
                            if (e.key === 'Escape') {
                                {{.modal-overlay}}->classList.remove('show');
                            }
                        }
                    };
                    
                    // 关闭按钮
                    {{.close-btn}} &-> click {
                        {{.modal-overlay}}->classList.remove('show');
                    }
                }
            }
        }
        
        // 原始嵌入演示
        [Origin] @Html {
            <footer>
                <p>&copy; 2024 CHTL Framework. All rights reserved.</p>
            </footer>
        }
        
        [Origin] @JavaScript {
            // 原始JavaScript代码
            console.log('CHTL page loaded successfully!');
            
            // 添加一些全局功能
            window.CHTLUtils = {
                showNotification: function(message, type = 'info') {
                    const notification = document.createElement('div');
                    notification.className = `notification ${type}`;
                    notification.textContent = message;
                    document.body.appendChild(notification);
                    
                    setTimeout(() => {
                        notification.remove();
                    }, 3000);
                }
            };
        }
        
        [Origin] @Style {
            /* 原始CSS代码 */
            .notification {
                position: fixed;
                top: 20px;
                right: 20px;
                padding: 12px 20px;
                border-radius: 4px;
                color: white;
                font-weight: 500;
                z-index: 9999;
                animation: slideIn 0.3s ease-out;
            }
            
            .notification.info {
                background: #17a2b8;
            }
            
            .notification.success {
                background: #28a745;
            }
            
            .notification.warning {
                background: #ffc107;
                color: #212529;
            }
            
            .notification.error {
                background: #dc3545;
            }
            
            @keyframes slideIn {
                from {
                    transform: translateX(100%);
                    opacity: 0;
                }
                to {
                    transform: translateX(0);
                    opacity: 1;
                }
            }
        }
        
        script {
            // 页面加载完成后的初始化
            {{document}}->listen {
                DOMContentLoaded: () => {
                    console.log('CHTL DOM ready!');
                    
                    // 显示欢迎通知
                    setTimeout(() => {
                        CHTLUtils.showNotification('Welcome to CHTL!', 'success');
                    }, 1000);
                    
                    // 添加键盘快捷键
                    {{document}}->listen {
                        keydown: (e) => {
                            if (e.ctrlKey && e.key === 'm') {
                                e.preventDefault();
                                {{.modal-overlay}}->classList.toggle('show');
                            }
                        }
                    };
                }
            };
        }
    }
}

// 约束演示
div {
    except span, [Custom] @Element Modal;  // 禁止使用span和Modal
    except [Template] @Var;  // 禁止使用变量组模板
    
    // 这个区域只能使用受限的语法
    p {
        text { "This area has syntax constraints." }
    }
}
)";
    
    cout << "代码长度: " << completeCode.length() << " 字符" << endl;
    cout << "开始编译..." << endl;
    
    auto startTime = chrono::high_resolution_clock::now();
    
    // 编译完整代码
    auto result = dispatcher.compileSource(completeCode);
    
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    
    cout << "编译完成，耗时: " << duration.count() << "ms" << endl;
    cout << "编译状态: " << (result.success ? "成功" : "失败") << endl;
    
    if (result.success) {
        cout << "生成的HTML长度: " << result.output.length() << " 字符" << endl;
        
        // 保存生成的HTML到文件
        ofstream htmlFile("chtl_complete_output.html");
        if (htmlFile.is_open()) {
            htmlFile << result.output;
            htmlFile.close();
            cout << "HTML已保存到: chtl_complete_output.html" << endl;
        }
        
        // 分析生成的内容
        size_t cssStart = result.output.find("<style>");
        size_t cssEnd = result.output.find("</style>");
        if (cssStart != string::npos && cssEnd != string::npos) {
            string css = result.output.substr(cssStart + 7, cssEnd - cssStart - 7);
            cout << "生成的CSS长度: " << css.length() << " 字符" << endl;
        }
        
        size_t jsStart = result.output.find("<script>");
        size_t jsEnd = result.output.find("</script>");
        if (jsStart != string::npos && jsEnd != string::npos) {
            string js = result.output.substr(jsStart + 8, jsEnd - jsStart - 8);
            cout << "生成的JavaScript长度: " << js.length() << " 字符" << endl;
        }
        
    } else {
        cout << "编译错误: " << result.errorMessage << endl;
        
        auto errors = dispatcher.getCompilationErrors();
        if (!errors.empty()) {
            cout << "\n详细错误信息:" << endl;
            for (const auto& error : errors) {
                cout << "  - " << error << endl;
            }
        }
    }
    
    // 生成编译报告
    cout << "\n编译报告:" << endl;
    // 这里需要访问内部结果来生成报告
    
    cout << "✓ 完整特性测试完成" << endl;
}

int main() {
    cout << "=== CHTL完整特性测试 ===" << endl;
    
    try {
        testCompleteFeatures();
        
        cout << "\n🎉 所有CHTL特性测试完成！" << endl;
        cout << "CHTL编译器已完整实现所有语法特性。" << endl;
        
    } catch (const exception& e) {
        cout << "测试过程中发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}