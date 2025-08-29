#include "CHTLTestSuite.h"

using namespace CHTL::Test;

// CHTL基础语法测试套件
CHTL_TEST_SUITE(CHTLBasicSyntax) {
    CHTL_ADD_TEST(CHTLBasicSyntax, Comments);
    CHTL_ADD_TEST(CHTLBasicSyntax, TextNodes);
    CHTL_ADD_TEST(CHTLBasicSyntax, Literals);
    CHTL_ADD_TEST(CHTLBasicSyntax, HTMLElements);
    CHTL_ADD_TEST(CHTLBasicSyntax, Attributes);
    CHTL_ADD_TEST(CHTLBasicSyntax, CEEquivalence);
}

// 注释测试
CHTL_TEST(CHTLBasicSyntax, Comments) {
    // 测试单行注释
    std::string code1 = "// This is a comment\ndiv { Hello }";
    assertTrue(TestUtil::validateSyntax(code1), "Single line comment should be valid");
    
    // 测试多行注释
    std::string code2 = "/* This is a\nmulti-line comment */\ndiv { Hello }";
    assertTrue(TestUtil::validateSyntax(code2), "Multi-line comment should be valid");
    
    // 测试HTML注释
    std::string code3 = "-- This is an HTML comment\ndiv { Hello }";
    assertTrue(TestUtil::validateSyntax(code3), "HTML comment should be valid");
}

// 文本节点测试
CHTL_TEST(CHTLBasicSyntax, TextNodes) {
    // 纯文本
    std::string code1 = "Hello World";
    assertTrue(TestUtil::validateSyntax(code1), "Plain text should be valid");
    
    // 带有特殊字符的文本
    std::string code2 = "Hello & World < > \"quotes\"";
    assertTrue(TestUtil::validateSyntax(code2), "Text with special characters should be valid");
}

// 字面量测试
CHTL_TEST(CHTLBasicSyntax, Literals) {
    // 无修饰字面量
    std::string code1 = "div class=myclass { Content }";
    assertTrue(TestUtil::validateSyntax(code1), "Unquoted literal should be valid");
    
    // 单引号字面量
    std::string code2 = "div class='my class' { Content }";
    assertTrue(TestUtil::validateSyntax(code2), "Single quoted literal should be valid");
    
    // 双引号字面量
    std::string code3 = "div class=\"my class\" { Content }";
    assertTrue(TestUtil::validateSyntax(code3), "Double quoted literal should be valid");
}

// HTML元素测试
CHTL_TEST(CHTLBasicSyntax, HTMLElements) {
    // 基本元素
    std::string code1 = "div { Content }";
    assertTrue(TestUtil::validateSyntax(code1), "Basic div element should be valid");
    
    // 嵌套元素
    std::string code2 = "div { p { Inner text } }";
    assertTrue(TestUtil::validateSyntax(code2), "Nested elements should be valid");
    
    // 多个兄弟元素
    std::string code3 = "div { Text } p { More text }";
    assertTrue(TestUtil::validateSyntax(code3), "Sibling elements should be valid");
}

// 属性测试
CHTL_TEST(CHTLBasicSyntax, Attributes) {
    // 单个属性
    std::string code1 = "div id=myid { Content }";
    assertTrue(TestUtil::validateSyntax(code1), "Single attribute should be valid");
    
    // 多个属性
    std::string code2 = "div id=myid class=myclass data-value=123 { Content }";
    assertTrue(TestUtil::validateSyntax(code2), "Multiple attributes should be valid");
    
    // 布尔属性
    std::string code3 = "input disabled { }";
    assertTrue(TestUtil::validateSyntax(code3), "Boolean attribute should be valid");
}

// CE等价性测试
CHTL_TEST(CHTLBasicSyntax, CEEquivalence) {
    // 使用冒号
    std::string code1 = "div class:myclass { Content }";
    assertTrue(TestUtil::validateSyntax(code1), "Colon syntax should be valid");
    
    // 使用等号
    std::string code2 = "div class=myclass { Content }";
    assertTrue(TestUtil::validateSyntax(code2), "Equals syntax should be valid");
    
    // 混合使用
    std::string code3 = "div id:myid class=myclass { Content }";
    assertTrue(TestUtil::validateSyntax(code3), "Mixed syntax should be valid");
}

// 局部样式块测试套件
CHTL_TEST_SUITE(CHTLLocalStyles) {
    CHTL_ADD_TEST(CHTLLocalStyles, InlineStyles);
    CHTL_ADD_TEST(CHTLLocalStyles, AutomatedClass);
    CHTL_ADD_TEST(CHTLLocalStyles, AutomatedId);
    CHTL_ADD_TEST(CHTLLocalStyles, ContextInference);
}

// 内联样式测试
CHTL_TEST(CHTLLocalStyles, InlineStyles) {
    std::string code = R"(
        div {
            style {
                color: red;
                font-size: 16px;
            }
            Content
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Inline style block should be valid");
    
    std::string expectedHTML = "<div style=\"color: red; font-size: 16px;\">Content</div>";
    assertTrue(TestUtil::compileAndCompare(code, expectedHTML), "Inline styles should compile correctly");
}

// 自动化类测试
CHTL_TEST(CHTLLocalStyles, AutomatedClass) {
    std::string code = R"(
        div {
            style {
                .myclass {
                    color: blue;
                }
            }
            Content
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Automated class style should be valid");
    assertContains(TestUtil::normalizeCode(code), ".myclass", "Should contain class selector");
}

// 自动化ID测试
CHTL_TEST(CHTLLocalStyles, AutomatedId) {
    std::string code = R"(
        div {
            style {
                #myid {
                    background: yellow;
                }
            }
            Content
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Automated ID style should be valid");
    assertContains(TestUtil::normalizeCode(code), "#myid", "Should contain ID selector");
}

// 上下文推断测试
CHTL_TEST(CHTLLocalStyles, ContextInference) {
    std::string code = R"(
        div class=container {
            style {
                & {
                    padding: 10px;
                }
                & > p {
                    margin: 5px;
                }
            }
            p { Text }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Context inference with & should be valid");
}

// 模板测试套件
CHTL_TEST_SUITE(CHTLTemplates) {
    CHTL_ADD_TEST(CHTLTemplates, StyleTemplate);
    CHTL_ADD_TEST(CHTLTemplates, ElementTemplate);
    CHTL_ADD_TEST(CHTLTemplates, VarTemplate);
    CHTL_ADD_TEST(CHTLTemplates, TemplateInheritance);
}

// 样式模板测试
CHTL_TEST(CHTLTemplates, StyleTemplate) {
    std::string code = R"(
        [Template] @Style Button {
            background: blue;
            color: white;
            padding: 10px;
        }
        
        button [Style.Button] { Click me }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Style template should be valid");
}

// 元素模板测试
CHTL_TEST(CHTLTemplates, ElementTemplate) {
    std::string code = R"(
        [Template] @Element Card {
            div class=card {
                h3 { $title }
                p { $content }
            }
        }
        
        [Element.Card title="Hello" content="World"]
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Element template should be valid");
}

// 变量模板测试
CHTL_TEST(CHTLTemplates, VarTemplate) {
    std::string code = R"(
        [Template] @Var Colors {
            primary: #007bff;
            secondary: #6c757d;
            success: #28a745;
        }
        
        div {
            style {
                color: [Var.Colors.primary];
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Var template should be valid");
}

// 模板继承测试
CHTL_TEST(CHTLTemplates, TemplateInheritance) {
    std::string code = R"(
        [Template] @Style BaseButton {
            padding: 10px;
            border: none;
            cursor: pointer;
        }
        
        [Template] @Style PrimaryButton : BaseButton {
            background: blue;
            color: white;
        }
        
        button [Style.PrimaryButton] { Click }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Template inheritance should be valid");
}

// 自定义元素测试套件
CHTL_TEST_SUITE(CHTLCustomElements) {
    CHTL_ADD_TEST(CHTLCustomElements, BasicCustom);
    CHTL_ADD_TEST(CHTLCustomElements, Specialization);
    CHTL_ADD_TEST(CHTLCustomElements, IndexAccess);
    CHTL_ADD_TEST(CHTLCustomElements, InsertDelete);
}

// 基本自定义元素测试
CHTL_TEST(CHTLCustomElements, BasicCustom) {
    std::string code = R"(
        [Custom] @Element MyButton {
            button class=custom-btn {
                $text
            }
        }
        
        [MyButton text="Click me"]
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Basic custom element should be valid");
}

// 特化测试
CHTL_TEST(CHTLCustomElements, Specialization) {
    std::string code = R"(
        [Custom] @Style ColorScheme {
            [0] { background: red; }
            [1] { background: blue; }
            [2] { background: green; }
        }
        
        div [Style.ColorScheme[1]] { Blue background }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Custom element specialization should be valid");
}

// 索引访问测试
CHTL_TEST(CHTLCustomElements, IndexAccess) {
    std::string code = R"(
        [Custom] @Var Config {
            [0]: "First";
            [1]: "Second";
            [2]: "Third";
        }
        
        p { [Var.Config[0]] }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Index access should be valid");
}

// 插入删除操作测试
CHTL_TEST(CHTLCustomElements, InsertDelete) {
    std::string code = R"(
        [Custom] @Element List {
            ul {
                [0] { li { Item 1 } }
                [1] { li { Item 2 } }
            }
        }
        
        [List insert[2] { li { Item 3 } }]
        [List delete[0]]
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Insert/delete operations should be valid");
}

// 导入测试套件
CHTL_TEST_SUITE(CHTLImports) {
    CHTL_ADD_TEST(CHTLImports, BasicImport);
    CHTL_ADD_TEST(CHTLImports, WildcardImport);
    CHTL_ADD_TEST(CHTLImports, AsImport);
    CHTL_ADD_TEST(CHTLImports, TypedImport);
}

// 基本导入测试
CHTL_TEST(CHTLImports, BasicImport) {
    std::string code = R"(
        [Import] @Chtl from "components/Button"
        
        [Button text="Click"]
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Basic import should be valid");
}

// 通配符导入测试
CHTL_TEST(CHTLImports, WildcardImport) {
    std::string code = R"(
        [Import] @Chtl from "components/*"
        
        div { Using imported components }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Wildcard import should be valid");
}

// As导入测试
CHTL_TEST(CHTLImports, AsImport) {
    std::string code = R"(
        [Import] @Html from "template.html" as MyTemplate
        
        [Origin] @Html : MyTemplate
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Import with 'as' should be valid");
}

// 类型化导入测试
CHTL_TEST(CHTLImports, TypedImport) {
    std::string code = R"(
        [Import] @Style from "styles.css"
        [Import] @JavaScript from "script.js"
        [Import] @CJmod from "extension"
        
        div { Content }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Typed imports should be valid");
}

// 命名空间测试套件
CHTL_TEST_SUITE(CHTLNamespaces) {
    CHTL_ADD_TEST(CHTLNamespaces, BasicNamespace);
    CHTL_ADD_TEST(CHTLNamespaces, NestedNamespace);
    CHTL_ADD_TEST(CHTLNamespaces, NamespaceMerging);
    CHTL_ADD_TEST(CHTLNamespaces, DefaultNamespace);
}

// 基本命名空间测试
CHTL_TEST(CHTLNamespaces, BasicNamespace) {
    std::string code = R"(
        [Namespace] Components {
            [Custom] @Element Button {
                button { $text }
            }
        }
        
        [Components.Button text="Click"]
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Basic namespace should be valid");
}

// 嵌套命名空间测试
CHTL_TEST(CHTLNamespaces, NestedNamespace) {
    std::string code = R"(
        [Namespace] UI {
            [Namespace] Forms {
                [Custom] @Element Input {
                    input type=$type { }
                }
            }
        }
        
        [UI.Forms.Input type="text"]
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Nested namespace should be valid");
}

// 命名空间合并测试
CHTL_TEST(CHTLNamespaces, NamespaceMerging) {
    std::string code = R"(
        [Namespace] MyLib {
            [Custom] @Element A { div { A } }
        }
        
        [Namespace] MyLib {
            [Custom] @Element B { div { B } }
        }
        
        [MyLib.A]
        [MyLib.B]
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Namespace merging should be valid");
}

// 默认命名空间测试
CHTL_TEST(CHTLNamespaces, DefaultNamespace) {
    std::string code = R"(
        [Namespace] {
            [Custom] @Element Card {
                div class=card { $content }
            }
        }
        
        [Card content="Hello"]
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Default namespace should be valid");
}

// 配置测试套件
CHTL_TEST_SUITE(CHTLConfiguration) {
    CHTL_ADD_TEST(CHTLConfiguration, BasicConfig);
    CHTL_ADD_TEST(CHTLConfiguration, DisableAutomation);
    CHTL_ADD_TEST(CHTLConfiguration, KeywordRedefinition);
}

// 基本配置测试
CHTL_TEST(CHTLConfiguration, BasicConfig) {
    std::string code = R"(
        [Configuration] {
            DISABLE_STYLE_AUTO_ADD_CLASS = false;
            DISABLE_STYLE_AUTO_ADD_ID = false;
        }
        
        div { Content }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Basic configuration should be valid");
}

// 禁用自动化测试
CHTL_TEST(CHTLConfiguration, DisableAutomation) {
    std::string code = R"(
        [Configuration] {
            DISABLE_STYLE_AUTO_ADD_CLASS = true;
            DISABLE_SCRIPT_AUTO_ADD_CLASS = false;
        }
        
        div {
            style {
                .myclass { color: red; }
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Disable automation config should be valid");
}

// 关键字重定义测试
CHTL_TEST(CHTLConfiguration, KeywordRedefinition) {
    std::string code = R"(
        [Configuration] MyConfig {
            Template = "模板";
            Custom = "自定义";
        }
        
        use MyConfig;
        
        [模板] @Style Button {
            padding: 10px;
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Keyword redefinition should be valid");
}

// 原始嵌入测试套件
CHTL_TEST_SUITE(CHTLOrigin) {
    CHTL_ADD_TEST(CHTLOrigin, HtmlOrigin);
    CHTL_ADD_TEST(CHTLOrigin, StyleOrigin);
    CHTL_ADD_TEST(CHTLOrigin, JavaScriptOrigin);
    CHTL_ADD_TEST(CHTLOrigin, CustomOrigin);
}

// HTML原始嵌入测试
CHTL_TEST(CHTLOrigin, HtmlOrigin) {
    std::string code = R"(
        [Origin] @Html {
            <div class="raw-html">
                <p>This is raw HTML</p>
            </div>
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "HTML origin embedding should be valid");
}

// CSS原始嵌入测试
CHTL_TEST(CHTLOrigin, StyleOrigin) {
    std::string code = R"(
        [Origin] @Style {
            .raw-css {
                color: red;
                font-size: 16px;
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Style origin embedding should be valid");
}

// JavaScript原始嵌入测试
CHTL_TEST(CHTLOrigin, JavaScriptOrigin) {
    std::string code = R"(
        [Origin] @JavaScript {
            console.log('Raw JavaScript');
            function test() {
                return 42;
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "JavaScript origin embedding should be valid");
}

// 自定义类型原始嵌入测试
CHTL_TEST(CHTLOrigin, CustomOrigin) {
    std::string code = R"(
        [Configuration] {
            [Origin] @Vue = "vue";
            [Origin] @React = "react";
        }
        
        [Origin] @Vue {
            <template>
                <div>{{ message }}</div>
            </template>
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Custom origin type should be valid");
}

// except约束测试套件
CHTL_TEST_SUITE(CHTLExcept) {
    CHTL_ADD_TEST(CHTLExcept, BasicExcept);
    CHTL_ADD_TEST(CHTLExcept, MultipleExcept);
    CHTL_ADD_TEST(CHTLExcept, ExceptInContext);
}

// 基本except测试
CHTL_TEST(CHTLExcept, BasicExcept) {
    std::string code = R"(
        div except class {
            Only ID allowed
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Basic except constraint should be valid");
}

// 多个except测试
CHTL_TEST(CHTLExcept, MultipleExcept) {
    std::string code = R"(
        [Custom] @Element Card except style except script {
            div { $content }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Multiple except constraints should be valid");
}

// 上下文中的except测试
CHTL_TEST(CHTLExcept, ExceptInContext) {
    std::string code = R"(
        [Template] @Style Button except :hover {
            background: blue;
            color: white;
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code), "Except in style context should be valid");
}