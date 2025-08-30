#include "CHTLTestSuite.h"

using namespace CHTL::Test;

// CHTL JS基础语法测试套件
CHTL_TEST_SUITE(CHTLJSBasicSyntax) {
    CHTL_ADD_TEST(CHTLJSBasicSyntax, FileExtension);
    CHTL_ADD_TEST(CHTLJSBasicSyntax, ModuleImport);
    CHTL_ADD_TEST(CHTLJSBasicSyntax, LocalScriptBlock);
    CHTL_ADD_TEST(CHTLJSBasicSyntax, ArrowOperator);
}

// 文件扩展名测试
CHTL_TEST(CHTLJSBasicSyntax, FileExtension) {
    // .cjjs文件应该被识别为CHTL JS
    std::string filename = "test.cjjs";
    assertTrue(filename.substr(filename.find_last_of(".") + 1) == "cjjs", 
               "CHTL JS files should use .cjjs extension");
}

// 模块导入测试
CHTL_TEST(CHTLJSBasicSyntax, ModuleImport) {
    std::string code = R"(
        module {
            load: ["jquery", "lodash", "./utils"]
        }
        
        console.log("Modules loaded");
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "AMD-style module import should be valid");
}

// 局部脚本块测试
CHTL_TEST(CHTLJSBasicSyntax, LocalScriptBlock) {
    std::string code = R"(
        script {
            let x = 10;
            console.log(x);
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Local script block should be valid");
}

// 箭头操作符测试
CHTL_TEST(CHTLJSBasicSyntax, ArrowOperator) {
    std::string code = R"(
        let result = getValue() -> processValue() -> formatOutput();
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Arrow operator chaining should be valid");
}

// 增强选择器测试套件
CHTL_TEST_SUITE(CHTLJSEnhancedSelectors) {
    CHTL_ADD_TEST(CHTLJSEnhancedSelectors, ClassSelector);
    CHTL_ADD_TEST(CHTLJSEnhancedSelectors, IdSelector);
    CHTL_ADD_TEST(CHTLJSEnhancedSelectors, TagSelector);
    CHTL_ADD_TEST(CHTLJSEnhancedSelectors, IndexedSelector);
    CHTL_ADD_TEST(CHTLJSEnhancedSelectors, ChainedSelectors);
}

// 类选择器测试
CHTL_TEST(CHTLJSEnhancedSelectors, ClassSelector) {
    std::string code = R"(
        let element = {{.myclass}};
        element -> addClass("active");
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Class selector should be valid");
    assertContains(code, "{{.myclass}}", "Should contain class selector");
}

// ID选择器测试
CHTL_TEST(CHTLJSEnhancedSelectors, IdSelector) {
    std::string code = R"(
        let header = {{#header}};
        header -> style.color = "red";
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "ID selector should be valid");
    assertContains(code, "{{#header}}", "Should contain ID selector");
}

// 标签选择器测试
CHTL_TEST(CHTLJSEnhancedSelectors, TagSelector) {
    std::string code = R"(
        let divs = {{div}};
        divs -> forEach(d => d.classList.add("styled"));
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Tag selector should be valid");
    assertContains(code, "{{div}}", "Should contain tag selector");
}

// 索引选择器测试
CHTL_TEST(CHTLJSEnhancedSelectors, IndexedSelector) {
    std::string code = R"(
        let firstP = {{p[0]}};
        let thirdLi = {{li[2]}};
        
        firstP -> style.fontWeight = "bold";
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Indexed selector should be valid");
    assertContains(code, "{{p[0]}}", "Should contain indexed selector");
}

// 链式选择器测试
CHTL_TEST(CHTLJSEnhancedSelectors, ChainedSelectors) {
    std::string code = R"(
        {{.container}} -> {{.item}} -> addClass("selected");
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Chained selectors should be valid");
}

// 增强监听器测试套件
CHTL_TEST_SUITE(CHTLJSListeners) {
    CHTL_ADD_TEST(CHTLJSListeners, BasicListen);
    CHTL_ADD_TEST(CHTLJSListeners, MultipleEvents);
    CHTL_ADD_TEST(CHTLJSListeners, ListenWithOptions);
    CHTL_ADD_TEST(CHTLJSListeners, NestedListen);
}

// 基本监听器测试
CHTL_TEST(CHTLJSListeners, BasicListen) {
    std::string code = R"(
        listen {
            {{button}} -> click: function() {
                console.log("Button clicked");
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Basic listen block should be valid");
}

// 多事件监听测试
CHTL_TEST(CHTLJSListeners, MultipleEvents) {
    std::string code = R"(
        listen {
            {{.input}} -> {
                focus: () => console.log("focused"),
                blur: () => console.log("blurred"),
                input: (e) => console.log(e.target.value)
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Multiple event listeners should be valid");
}

// 带选项的监听器测试
CHTL_TEST(CHTLJSListeners, ListenWithOptions) {
    std::string code = R"(
        listen {
            {{document}} -> click: {
                handler: (e) => console.log(e),
                capture: true,
                once: true
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Listen with options should be valid");
}

// 嵌套监听器测试
CHTL_TEST(CHTLJSListeners, NestedListen) {
    std::string code = R"(
        listen {
            {{.parent}} -> click: function() {
                listen {
                    {{.child}} -> click: (e) => e.stopPropagation()
                }
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Nested listen blocks should be valid");
}

// 事件委托测试套件
CHTL_TEST_SUITE(CHTLJSEventDelegation) {
    CHTL_ADD_TEST(CHTLJSEventDelegation, BasicDelegate);
    CHTL_ADD_TEST(CHTLJSEventDelegation, DelegateWithSelector);
    CHTL_ADD_TEST(CHTLJSEventDelegation, MultipleDelegates);
}

// 基本事件委托测试
CHTL_TEST(CHTLJSEventDelegation, BasicDelegate) {
    std::string code = R"(
        delegate {
            {{.container}} -> {
                ".button": {
                    click: (e) => console.log("Delegated click")
                }
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Basic delegate block should be valid");
}

// 带选择器的委托测试
CHTL_TEST(CHTLJSEventDelegation, DelegateWithSelector) {
    std::string code = R"(
        delegate {
            {{#list}} -> {
                "li.item": {
                    click: handleItemClick,
                    mouseover: highlightItem
                }
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Delegate with selector should be valid");
}

// 多重委托测试
CHTL_TEST(CHTLJSEventDelegation, MultipleDelegates) {
    std::string code = R"(
        delegate {
            {{body}} -> {
                ".btn-primary": { click: primaryAction },
                ".btn-secondary": { click: secondaryAction },
                "input[type='text']": { change: validateInput }
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Multiple delegates should be valid");
}

// 动画测试套件
CHTL_TEST_SUITE(CHTLJSAnimations) {
    CHTL_ADD_TEST(CHTLJSAnimations, BasicAnimate);
    CHTL_ADD_TEST(CHTLJSAnimations, AnimateWithOptions);
    CHTL_ADD_TEST(CHTLJSAnimations, ChainedAnimations);
    CHTL_ADD_TEST(CHTLJSAnimations, AnimateKeyframes);
}

// 基本动画测试
CHTL_TEST(CHTLJSAnimations, BasicAnimate) {
    std::string code = R"(
        animate {
            {{.box}} -> {
                opacity: 0,
                transform: "translateX(100px)"
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Basic animate block should be valid");
}

// 带选项的动画测试
CHTL_TEST(CHTLJSAnimations, AnimateWithOptions) {
    std::string code = R"(
        animate {
            {{#modal}} -> {
                properties: {
                    opacity: 1,
                    transform: "scale(1)"
                },
                duration: 300,
                easing: "ease-in-out",
                delay: 100
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Animate with options should be valid");
}

// 链式动画测试
CHTL_TEST(CHTLJSAnimations, ChainedAnimations) {
    std::string code = R"(
        {{.element}}
            -> animate({ opacity: 0 }, 200)
            -> animate({ height: 0 }, 300)
            -> remove();
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Chained animations should be valid");
}

// 关键帧动画测试
CHTL_TEST(CHTLJSAnimations, AnimateKeyframes) {
    std::string code = R"(
        animate {
            {{.spinner}} -> {
                keyframes: [
                    { transform: "rotate(0deg)" },
                    { transform: "rotate(180deg)" },
                    { transform: "rotate(360deg)" }
                ],
                duration: 1000,
                iterations: "infinite"
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Keyframe animation should be valid");
}

// 虚拟对象测试套件
CHTL_TEST_SUITE(CHTLJSVirtualObjects) {
    CHTL_ADD_TEST(CHTLJSVirtualObjects, VirDeclaration);
    CHTL_ADD_TEST(CHTLJSVirtualObjects, VirAccess);
    CHTL_ADD_TEST(CHTLJSVirtualObjects, VirInFunction);
    CHTL_ADD_TEST(CHTLJSVirtualObjects, VirBinding);
}

// vir声明测试
CHTL_TEST(CHTLJSVirtualObjects, VirDeclaration) {
    std::string code = R"(
        function myFunction() {
            vir.state = {
                count: 0,
                items: []
            };
            
            vir.increment = function() {
                vir.state.count++;
            };
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "vir declaration should be valid");
}

// vir访问测试
CHTL_TEST(CHTLJSVirtualObjects, VirAccess) {
    std::string code = R"(
        function component() {
            console.log(vir.props.title);
            vir.state.active = true;
            return vir.render();
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "vir access should be valid");
}

// 函数中的vir测试
CHTL_TEST(CHTLJSVirtualObjects, VirInFunction) {
    std::string code = R"(
        myComponent {
            init: function() {
                vir.data = { loaded: false };
            },
            load: async function() {
                vir.data.loaded = true;
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "vir in CHTL JS function should be valid");
}

// vir绑定测试
CHTL_TEST(CHTLJSVirtualObjects, VirBinding) {
    std::string code = R"(
        printMyLove {url: $!_, mode: $?_}
        
        // Manual binding
        CHTLJSFunction::bindVirtualObject("printMyLove");
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "vir binding should be valid");
}

// 事件绑定操作符测试套件
CHTL_TEST_SUITE(CHTLJSEventBinding) {
    CHTL_ADD_TEST(CHTLJSEventBinding, BasicBinding);
    CHTL_ADD_TEST(CHTLJSEventBinding, ChainedBinding);
    CHTL_ADD_TEST(CHTLJSEventBinding, ComplexBinding);
}

// 基本事件绑定测试
CHTL_TEST(CHTLJSEventBinding, BasicBinding) {
    std::string code = R"(
        {{button}} &-> click: handleClick;
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Basic event binding should be valid");
}

// 链式事件绑定测试
CHTL_TEST(CHTLJSEventBinding, ChainedBinding) {
    std::string code = R"(
        {{.form}}
            &-> submit: validateForm
            &-> reset: clearForm;
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Chained event binding should be valid");
}

// 复杂事件绑定测试
CHTL_TEST(CHTLJSEventBinding, ComplexBinding) {
    std::string code = R"(
        {{.interactive}}
            &-> {
                mouseenter: showTooltip,
                mouseleave: hideTooltip,
                click: (e) => {
                    e.preventDefault();
                    processClick(e);
                }
            };
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Complex event binding should be valid");
}

// CHTL JS函数语法测试套件
CHTL_TEST_SUITE(CHTLJSFunctions) {
    CHTL_ADD_TEST(CHTLJSFunctions, BasicFunction);
    CHTL_ADD_TEST(CHTLJSFunctions, FunctionWithParams);
    CHTL_ADD_TEST(CHTLJSFunctions, KeyValuePairs);
    CHTL_ADD_TEST(CHTLJSFunctions, OptionalParams);
}

// 基本CHTL JS函数测试
CHTL_TEST(CHTLJSFunctions, BasicFunction) {
    std::string code = R"(
        showMessage {
            text: "Hello",
            type: "info"
        };
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Basic CHTL JS function should be valid");
}

// 带参数的函数测试
CHTL_TEST(CHTLJSFunctions, FunctionWithParams) {
    std::string code = R"(
        createUser {
            name: $_,
            email: $!_,
            role: $?_
        };
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Function with special params should be valid");
}

// 键值对无序测试
CHTL_TEST(CHTLJSFunctions, KeyValuePairs) {
    std::string code = R"(
        // 这两个调用应该等价
        config { width: 100, height: 200 };
        config { height: 200, width: 100 };
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Unordered key-value pairs should be valid");
}

// 可选参数测试
CHTL_TEST(CHTLJSFunctions, OptionalParams) {
    std::string code = R"(
        request {
            url: "/api/data",
            method: "GET"
            // headers is optional
        };
        
        request {
            url: "/api/data",
            method: "POST",
            headers: { "Content-Type": "application/json" }
        };
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Optional parameters should be valid");
}

// 集成测试套件
CHTL_TEST_SUITE(CHTLJSIntegration) {
    CHTL_ADD_TEST(CHTLJSIntegration, CompleteComponent);
    CHTL_ADD_TEST(CHTLJSIntegration, WithCHTL);
    CHTL_ADD_TEST(CHTLJSIntegration, ModularCode);
}

// 完整组件测试
CHTL_TEST(CHTLJSIntegration, CompleteComponent) {
    std::string code = R"(
        module {
            load: ["jquery", "./utils"]
        }
        
        script {
            // Component definition
            TodoList {
                init: function() {
                    vir.items = [];
                    vir.bindEvents();
                },
                
                bindEvents: function() {
                    listen {
                        {{#add-btn}} -> click: vir.addItem
                    }
                    
                    delegate {
                        {{#todo-list}} -> {
                            ".delete-btn": { click: vir.deleteItem }
                        }
                    }
                },
                
                addItem: function() {
                    let input = {{#todo-input}};
                    vir.items.push(input.value);
                    vir.render();
                },
                
                render: function() {
                    {{#todo-list}} -> innerHTML = vir.items
                        .map(item => `<li>${item} <button class="delete-btn">X</button></li>`)
                        .join('');
                }
            };
            
            // Initialize
            TodoList.init();
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Complete component should be valid");
}

// 与CHTL集成测试
CHTL_TEST(CHTLJSIntegration, WithCHTL) {
    std::string code = R"(
        div id="app" {
            h1 { Todo App }
            
            input id="todo-input" type="text" placeholder="Add item" { }
            button id="add-btn" { Add }
            
            ul id="todo-list" { }
            
            script {
                // CHTL JS code
                listen {
                    {{#add-btn}} -> click: function() {
                        let input = {{#todo-input}};
                        if (input.value) {
                            {{#todo-list}} -> innerHTML += `<li>${input.value}</li>`;
                            input.value = "";
                        }
                    }
                }
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtl"), "CHTL with embedded CHTL JS should be valid");
}

// 模块化代码测试
CHTL_TEST(CHTLJSIntegration, ModularCode) {
    std::string code = R"(
        module {
            load: ["./components/button", "./utils/validator"]
        }
        
        script {
            // Using imported modules
            Button.create {
                text: "Submit",
                type: "primary",
                onClick: function() {
                    if (Validator.checkForm({{#form}})) {
                        submitForm();
                    }
                }
            };
            
            animate {
                {{.fade-in}} -> {
                    opacity: 1,
                    duration: 500
                }
            }
        }
    )";
    
    assertTrue(TestUtil::validateSyntax(code, "chtljs"), "Modular code should be valid");
}