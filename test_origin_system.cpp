#include "src/CHTL/CHTLNode/OriginNode.h"
#include "src/CHTL/CHTLNode/BaseNode.h"
#include <iostream>

void testHtmlOriginNode() {
    using namespace CHTL;
    
    std::cout << "Testing HTML Origin Node:\n";
    std::cout << "========================\n";
    
    // 创建HTML原始嵌入
    String htmlContent = R"(
        <div class="container">
            <h1>Hello World</h1>
            <p>This is a test paragraph.</p>
            <script>console.log('embedded script');</script>
            <style>body { margin: 0; }</style>
        </div>
    )";
    
    auto htmlOrigin = std::make_unique<HtmlOriginNode>(htmlContent, "testHtml");
    
    // 测试基础功能
    std::cout << "HTML Origin: " << htmlOrigin->toString() << "\n";
    std::cout << "Content length: " << htmlOrigin->getContent().length() << "\n";
    std::cout << "Origin name: " << htmlOrigin->getOriginName() << "\n";
    std::cout << "Origin type: " << htmlOrigin->getOriginType() << "\n";
    std::cout << "Is named: " << htmlOrigin->isNamed << "\n";
    std::cout << "Is HTML origin: " << htmlOrigin->isHtmlOrigin() << "\n";
    
    // 测试HTML特定功能
    std::cout << "Is well formed: " << htmlOrigin->isWellFormed() << "\n";
    std::cout << "Has script tags: " << htmlOrigin->hasScriptTags() << "\n";
    std::cout << "Has style tags: " << htmlOrigin->hasStyleTags() << "\n";
    
    // 提取标签
    auto tags = htmlOrigin->extractTags();
    std::cout << "Extracted tags: ";
    for (const auto& tag : tags) {
        std::cout << tag << " ";
    }
    std::cout << "\n";
    
    // 验证内容
    std::cout << "Content is valid: " << htmlOrigin->validateContent() << "\n";
    
    auto errors = htmlOrigin->getContentValidationErrors();
    if (!errors.empty()) {
        std::cout << "Validation errors:\n";
        for (const auto& error : errors) {
            std::cout << "  - " << error << "\n";
        }
    }
    
    // 生成输出
    std::cout << "Output type: " << htmlOrigin->getOutputType() << "\n";
    std::cout << "Generated output preview: " << htmlOrigin->generateOutput().substr(0, 50) << "...\n";
    
    // 测试元数据
    htmlOrigin->setMetadata("source", "test.html");
    htmlOrigin->setMetadata("charset", "utf-8");
    std::cout << "Metadata source: " << htmlOrigin->getMetadata("source") << "\n";
    std::cout << "Metadata charset: " << htmlOrigin->getMetadata("charset") << "\n";
}

void testStyleOriginNode() {
    using namespace CHTL;
    
    std::cout << "\nTesting Style Origin Node:\n";
    std::cout << "==========================\n";
    
    // 创建CSS原始嵌入
    String cssContent = R"(
        body {
            margin: 0;
            padding: 0;
            font-family: Arial, sans-serif;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }
        
        @media (max-width: 768px) {
            .container {
                padding: 10px;
            }
        }
        
        @keyframes fadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }
        
        .fade-in {
            animation: fadeIn 0.5s ease-in-out;
        }
    )";
    
    auto styleOrigin = std::make_unique<StyleOriginNode>(cssContent, "mainStyles");
    
    // 测试基础功能
    std::cout << "Style Origin: " << styleOrigin->toString() << "\n";
    std::cout << "Is valid CSS: " << styleOrigin->isValidCSS() << "\n";
    std::cout << "Has media queries: " << styleOrigin->hasMediaQueries() << "\n";
    std::cout << "Has keyframes: " << styleOrigin->hasKeyframes() << "\n";
    
    // 提取选择器
    auto selectors = styleOrigin->extractSelectors();
    std::cout << "Extracted selectors:\n";
    for (const auto& selector : selectors) {
        std::cout << "  - " << selector << "\n";
    }
    
    // 提取属性
    auto properties = styleOrigin->extractProperties();
    std::cout << "Extracted properties (first 5):\n";
    for (size_t i = 0; i < std::min(properties.size(), size_t(5)); ++i) {
        std::cout << "  - " << properties[i] << "\n";
    }
    
    // 测试CSS优化
    std::cout << "\nMinified CSS preview:\n";
    String minified = styleOrigin->minifyCSS();
    std::cout << minified.substr(0, 100) << "...\n";
    
    std::cout << "\nWith vendor prefixes preview:\n";
    String prefixed = styleOrigin->addVendorPrefixes();
    std::cout << prefixed.substr(0, 100) << "...\n";
    
    // 测试元数据控制的输出
    styleOrigin->setMetadata("minify", "true");
    styleOrigin->setMetadata("autoprefixer", "true");
    std::cout << "\nGenerated output (with optimizations) preview:\n";
    std::cout << styleOrigin->generateOutput().substr(0, 100) << "...\n";
    
    // 验证内容
    std::cout << "Content is valid: " << styleOrigin->validateContent() << "\n";
}

void testJavaScriptOriginNode() {
    using namespace CHTL;
    
    std::cout << "\nTesting JavaScript Origin Node:\n";
    std::cout << "===============================\n";
    
    // 创建JavaScript原始嵌入
    String jsContent = R"(
        // ES6 modules
        import { utils } from './utils.js';
        export const API_URL = 'https://api.example.com';
        
        // Variables and functions
        const config = {
            debug: true,
            version: '1.0.0'
        };
        
        let userCount = 0;
        var globalData = null;
        
        function initializeApp() {
            console.log('App initialized');
            userCount++;
        }
        
        const fetchData = async () => {
            const response = await fetch(API_URL);
            return response.json();
        };
        
        // Arrow function
        const processData = (data) => {
            return data.map(item => item.id);
        };
        
        // CommonJS (mixed with ES6 for testing)
        module.exports = { initializeApp, fetchData };
        
        // Event handling
        document.addEventListener('DOMContentLoaded', initializeApp);
    )";
    
    auto jsOrigin = std::make_unique<JavaScriptOriginNode>(jsContent, "appScript");
    
    // 测试基础功能
    std::cout << "JavaScript Origin: " << jsOrigin->toString() << "\n";
    std::cout << "Is valid JavaScript: " << jsOrigin->isValidJavaScript() << "\n";
    std::cout << "Has ES module syntax: " << jsOrigin->hasESModuleSyntax() << "\n";
    std::cout << "Has CommonJS syntax: " << jsOrigin->hasCommonJSSyntax() << "\n";
    
    // 提取函数
    auto functions = jsOrigin->extractFunctions();
    std::cout << "Extracted functions:\n";
    for (const auto& func : functions) {
        std::cout << "  - " << func << "\n";
    }
    
    // 提取变量
    auto variables = jsOrigin->extractVariables();
    std::cout << "Extracted variables:\n";
    for (const auto& var : variables) {
        std::cout << "  - " << var << "\n";
    }
    
    // 测试JavaScript优化
    std::cout << "\nMinified JavaScript preview:\n";
    String minified = jsOrigin->minifyJavaScript();
    std::cout << minified.substr(0, 100) << "...\n";
    
    std::cout << "\nWith strict mode:\n";
    String strictMode = jsOrigin->addStrictMode();
    std::cout << strictMode.substr(0, 100) << "...\n";
    
    // 验证内容
    std::cout << "Content is valid: " << jsOrigin->validateContent() << "\n";
    std::cout << "Output type: " << jsOrigin->getOutputType() << "\n";
}

void testCustomOriginNode() {
    using namespace CHTL;
    
    std::cout << "\nTesting Custom Origin Node:\n";
    std::cout << "===========================\n";
    
    // 注册自定义类型
    OriginManager::registerCustomType("Vue", "application/x-vue");
    OriginManager::registerCustomType("Markdown", "text/markdown");
    
    // 创建Vue组件原始嵌入
    String vueContent = R"(
        <template>
            <div class="hello-world">
                <h1>{{ title }}</h1>
                <p>{{ message }}</p>
                <button @click="handleClick">Click me</button>
            </div>
        </template>
        
        <script>
        export default {
            name: 'HelloWorld',
            data() {
                return {
                    title: 'Hello Vue!',
                    message: 'This is a Vue component'
                };
            },
            methods: {
                handleClick() {
                    this.message = 'Button clicked!';
                }
            }
        };
        </script>
        
        <style scoped>
        .hello-world {
            text-align: center;
            padding: 20px;
        }
        
        button {
            background: #42b883;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 4px;
            cursor: pointer;
        }
        </style>
    )";
    
    auto vueOrigin = std::make_unique<CustomOriginNode>("Vue", vueContent, "HelloWorld");
    
    // 设置自定义属性
    vueOrigin->setCustomProperty("framework", "Vue.js");
    vueOrigin->setCustomProperty("version", "3.0");
    vueOrigin->setCustomProperty("scoped", "true");
    
    // 设置内容处理器
    vueOrigin->setContentProcessor([](const String& content) {
        return "<!-- Processed Vue Component -->\n" + content;
    });
    
    vueOrigin->setOutputMimeType("application/x-vue");
    
    // 测试功能
    std::cout << "Custom Origin: " << vueOrigin->toString() << "\n";
    std::cout << "Is custom origin: " << vueOrigin->isCustomOrigin() << "\n";
    std::cout << "Framework: " << vueOrigin->getCustomProperty("framework") << "\n";
    std::cout << "Version: " << vueOrigin->getCustomProperty("version") << "\n";
    std::cout << "Output type: " << vueOrigin->getOutputType() << "\n";
    
    // 生成处理后的输出
    std::cout << "\nGenerated output preview:\n";
    std::cout << vueOrigin->generateOutput().substr(0, 100) << "...\n";
    
    // 创建Markdown原始嵌入
    String markdownContent = R"(
        # Hello Markdown
        
        This is a **markdown** document with *emphasis*.
        
        ## Features
        
        - Lists
        - Links: [Example](https://example.com)
        - Code: `inline code`
        
        ```javascript
        console.log('Hello World');
        ```
        
        > This is a blockquote
    )";
    
    auto markdownOrigin = std::make_unique<CustomOriginNode>("Markdown", markdownContent, "readme");
    markdownOrigin->setOutputMimeType("text/markdown");
    
    std::cout << "\nMarkdown Origin: " << markdownOrigin->toString() << "\n";
    std::cout << "Content preview: " << markdownOrigin->getContent().substr(0, 50) << "...\n";
    
    // 验证注册的自定义类型
    std::cout << "\nRegistered custom types:\n";
    auto customTypes = OriginManager::getRegisteredCustomTypes();
    for (const auto& type : customTypes) {
        std::cout << "  - " << type << "\n";
    }
    
    std::cout << "Is Vue registered: " << OriginManager::isRegisteredCustomType("Vue") << "\n";
    std::cout << "Is Markdown registered: " << OriginManager::isRegisteredCustomType("Markdown") << "\n";
}

void testOriginValidation() {
    using namespace CHTL;
    
    std::cout << "\nTesting Origin Validation:\n";
    std::cout << "==========================\n";
    
    // 测试安全性验证
    String maliciousHtml = R"(
        <div>
            <script>alert('XSS');</script>
            <img src="x" onerror="alert('XSS')">
            <a href="javascript:alert('XSS')">Click me</a>
        </div>
    )";
    
    auto maliciousOrigin = std::make_unique<HtmlOriginNode>(maliciousHtml);
    std::cout << "Malicious HTML is valid: " << maliciousOrigin->validateContent() << "\n";
    
    auto htmlErrors = maliciousOrigin->getContentValidationErrors();
    if (!htmlErrors.empty()) {
        std::cout << "HTML validation errors:\n";
        for (const auto& error : htmlErrors) {
            std::cout << "  - " << error << "\n";
        }
    }
    
    // 测试CSS注入检测
    String maliciousCSS = R"(
        body {
            background: url('javascript:alert("XSS")');
            behavior: url('evil.htc');
            -moz-binding: url('evil.xml#evil');
        }
        
        @import url('evil.css');
    )";
    
    auto maliciousCSSOrigin = std::make_unique<StyleOriginNode>(maliciousCSS);
    std::cout << "Malicious CSS is valid: " << maliciousCSSOrigin->validateContent() << "\n";
    
    auto cssErrors = maliciousCSSOrigin->getContentValidationErrors();
    if (!cssErrors.empty()) {
        std::cout << "CSS validation errors:\n";
        for (const auto& error : cssErrors) {
            std::cout << "  - " << error << "\n";
        }
    }
    
    // 测试JavaScript注入检测
    String maliciousJS = R"(
        eval('alert("XSS")');
        setTimeout('alert("XSS")', 1000);
        new Function('alert("XSS")')();
        document.write('<script>alert("XSS")</script>');
    )";
    
    auto maliciousJSOrigin = std::make_unique<JavaScriptOriginNode>(maliciousJS);
    std::cout << "Malicious JavaScript is valid: " << maliciousJSOrigin->validateContent() << "\n";
    
    auto jsErrors = maliciousJSOrigin->getContentValidationErrors();
    if (!jsErrors.empty()) {
        std::cout << "JavaScript validation errors:\n";
        for (const auto& error : jsErrors) {
            std::cout << "  - " << error << "\n";
        }
    }
    
    // 测试性能警告
    auto perfWarnings = OriginValidator::getPerformanceWarnings(maliciousJS, "JavaScript");
    if (!perfWarnings.empty()) {
        std::cout << "Performance warnings:\n";
        for (const auto& warning : perfWarnings) {
            std::cout << "  - " << warning << "\n";
        }
    }
}

void testOriginCloning() {
    using namespace CHTL;
    
    std::cout << "\nTesting Origin Cloning:\n";
    std::cout << "=======================\n";
    
    // 创建原始HTML origin
    String htmlContent = "<div>Original content</div>";
    auto original = std::make_unique<HtmlOriginNode>(htmlContent, "originalHtml");
    original->setMetadata("source", "test.html");
    original->setMetadata("minify", "false");
    
    // 克隆
    auto cloned = original->clone();
    HtmlOriginNode* clonedHtml = static_cast<HtmlOriginNode*>(cloned.get());
    
    std::cout << "Original: " << original->toString() << "\n";
    std::cout << "Cloned: " << clonedHtml->toString() << "\n";
    
    // 验证克隆结果
    std::cout << "Cloned content: " << clonedHtml->getContent() << "\n";
    std::cout << "Cloned name: " << clonedHtml->getOriginName() << "\n";
    std::cout << "Cloned metadata source: " << clonedHtml->getMetadata("source") << "\n";
    std::cout << "Cloned metadata minify: " << clonedHtml->getMetadata("minify") << "\n";
    
    // 修改克隆的内容
    clonedHtml->setContent("<div>Modified content</div>");
    clonedHtml->setOriginName("modifiedHtml");
    clonedHtml->setMetadata("minify", "true");
    
    std::cout << "\nAfter modifying cloned origin:\n";
    std::cout << "Original content: " << original->getContent() << "\n";
    std::cout << "Cloned content: " << clonedHtml->getContent() << "\n";
    std::cout << "Original name: " << original->getOriginName() << "\n";
    std::cout << "Cloned name: " << clonedHtml->getOriginName() << "\n";
    std::cout << "Original minify: " << original->getMetadata("minify") << "\n";
    std::cout << "Cloned minify: " << clonedHtml->getMetadata("minify") << "\n";
}

int main() {
    std::cout << "CHTL Origin System Test Suite\n";
    std::cout << "==============================\n\n";
    
    try {
        testHtmlOriginNode();
        testStyleOriginNode();
        testJavaScriptOriginNode();
        testCustomOriginNode();
        testOriginValidation();
        testOriginCloning();
        
        std::cout << "\nAll origin system tests completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Test exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}