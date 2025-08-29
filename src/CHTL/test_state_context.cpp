#include "CHTLState/State.h"
#include "CHTLContext/Context.h"
#include "CHTLLexer/Lexer.h"
#include <iostream>
#include <cassert>

using namespace CHTL;

void testStateMachine() {
    std::cout << "=== Testing CHTL State Machine ===" << std::endl;
    
    CHTLStateMachine stateMachine;
    
    // 测试初始状态
    assert(stateMachine.getCurrentStateType() == CHTLStateType::GLOBAL);
    std::cout << "✓ Initial state is GLOBAL" << std::endl;
    
    // 测试进入元素状态
    {
        CHTL_STATE_GUARD(stateMachine, CHTLStateType::ELEMENT);
        assert(stateMachine.getCurrentStateType() == CHTLStateType::ELEMENT);
        std::cout << "✓ Entered ELEMENT state" << std::endl;
        
        // 测试嵌套元素
        {
            CHTL_STATE_GUARD_WITH_CONTEXT(stateMachine, CHTLStateType::ELEMENT, "nested div");
            assert(stateMachine.getCurrentStateType() == CHTLStateType::ELEMENT);
            assert(stateMachine.getDepth() == 3); // global + element + nested element
            std::cout << "✓ Nested ELEMENT state" << std::endl;
            
            // 测试样式块
            {
                CHTL_STATE_GUARD(stateMachine, CHTLStateType::STYLE_BLOCK);
                assert(stateMachine.getCurrentStateType() == CHTLStateType::STYLE_BLOCK);
                std::cout << "✓ Entered STYLE_BLOCK state" << std::endl;
            }
            
            assert(stateMachine.getCurrentStateType() == CHTLStateType::ELEMENT);
            std::cout << "✓ Exited STYLE_BLOCK state automatically" << std::endl;
        }
        
        assert(stateMachine.getCurrentStateType() == CHTLStateType::ELEMENT);
        std::cout << "✓ Exited nested ELEMENT state automatically" << std::endl;
    }
    
    assert(stateMachine.getCurrentStateType() == CHTLStateType::GLOBAL);
    std::cout << "✓ Returned to GLOBAL state automatically" << std::endl;
    
    // 测试模板状态
    {
        CHTL_STATE_GUARD(stateMachine, CHTLStateType::TEMPLATE_DEFINITION);
        assert(stateMachine.getCurrentStateType() == CHTLStateType::TEMPLATE_DEFINITION);
        std::cout << "✓ Entered TEMPLATE_DEFINITION state" << std::endl;
        
        {
            CHTL_STATE_GUARD(stateMachine, CHTLStateType::TEMPLATE_STYLE);
            assert(stateMachine.getCurrentStateType() == CHTLStateType::TEMPLATE_STYLE);
            std::cout << "✓ Entered TEMPLATE_STYLE state" << std::endl;
        }
    }
    
    std::cout << "State Machine test completed successfully!" << std::endl;
}

void testContext() {
    std::cout << "\n=== Testing CHTL Context ===" << std::endl;
    
    CHTLContext context;
    
    // 测试初始状态
    assert(context.getScopeDepth() == 1);
    assert(context.getCurrentNamespace().empty());
    std::cout << "✓ Initial context state" << std::endl;
    
    // 测试符号添加和查找
    {
        CHTL_SCOPE_GUARD(context, ContextScopeType::GLOBAL_SCOPE);
        
        SymbolInfo templateSymbol(SymbolType::TEMPLATE_STYLE, "DefaultText", "", TypeInfo());
        assert(context.addSymbol(templateSymbol));
        std::cout << "✓ Added template symbol" << std::endl;
        
        const SymbolInfo* found = context.findSymbol("DefaultText");
        assert(found != nullptr);
        assert(found->name == "DefaultText");
        assert(found->type == SymbolType::TEMPLATE_STYLE);
        std::cout << "✓ Found template symbol" << std::endl;
    }
    
    // 测试命名空间
    {
        context.enterNamespace("TestNamespace");
        assert(context.getCurrentNamespace() == "TestNamespace");
        std::cout << "✓ Entered namespace" << std::endl;
        
        {
            CHTL_SCOPE_GUARD_WITH_NAMESPACE(context, ContextScopeType::NAMESPACE_SCOPE, "test", "TestNamespace");
            
            SymbolInfo customSymbol(SymbolType::CUSTOM_ELEMENT, "CustomBox", "TestNamespace", TypeInfo());
            assert(context.addSymbol(customSymbol));
            std::cout << "✓ Added symbol in namespace" << std::endl;
            
            const SymbolInfo* found = context.findSymbolInNamespace("CustomBox", "TestNamespace");
            assert(found != nullptr);
            assert(found->namespace_ == "TestNamespace");
            std::cout << "✓ Found symbol in namespace" << std::endl;
        }
        
        context.exitNamespace();
        assert(context.getCurrentNamespace().empty());
        std::cout << "✓ Exited namespace" << std::endl;
    }
    
    // 测试选择器上下文
    {
        SelectorContext& selCtx = context.getSelectorContext();
        
        selCtx.setClass("container", true);
        selCtx.setId("main", true);
        
        assert(selCtx.getEffectiveClass() == "container");
        assert(selCtx.getEffectiveId() == "main");
        std::cout << "✓ Selector context works" << std::endl;
        
        selCtx.addAutoClass("auto-class");
        selCtx.reset();
        
        selCtx.addAutoClass("auto-class");
        assert(selCtx.getEffectiveClass() == "auto-class");
        std::cout << "✓ Auto selector works" << std::endl;
    }
    
    // 测试约束
    {
        context.addConstraint("span");
        assert(context.isConstrained("span"));
        assert(!context.isConstrained("div"));
        std::cout << "✓ Constraints work" << std::endl;
    }
    
    // 测试继承关系
    {
        context.addInheritance("ChildTemplate", "ParentTemplate");
        assert(context.isInherited("ChildTemplate", "ParentTemplate"));
        
        std::vector<std::string> chain = context.getInheritanceChain("ChildTemplate");
        assert(chain.size() >= 1);
        assert(chain[0] == "ChildTemplate");
        std::cout << "✓ Inheritance tracking works" << std::endl;
    }
    
    std::cout << "Context test completed successfully!" << std::endl;
}

void testIntegration() {
    std::cout << "\n=== Testing State Machine + Context Integration ===" << std::endl;
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    
    // 模拟解析一个CHTL文件
    {
        // 进入模板定义
        CHTL_STATE_GUARD(stateMachine, CHTLStateType::TEMPLATE_DEFINITION);
        
        assert(stateMachine.getCurrentStateType() == CHTLStateType::TEMPLATE_DEFINITION);
        std::cout << "✓ Entered template definition context" << std::endl;
        
        {
            // 定义样式组模板 - 添加到全局作用域
            CHTL_STATE_GUARD(stateMachine, CHTLStateType::TEMPLATE_STYLE);
            
            SymbolInfo styleTemplate(SymbolType::TEMPLATE_STYLE, "ButtonStyle", "", TypeInfo());
            assert(context.addSymbol(styleTemplate));
            std::cout << "✓ Added style template in correct context" << std::endl;
        }
    }
    
    // 现在在全局作用域使用模板
    {
        CHTL_STATE_GUARD(stateMachine, CHTLStateType::ELEMENT);
        CHTL_SCOPE_GUARD(context, ContextScopeType::ELEMENT_SCOPE);
        
        // 查找之前定义的模板
        const SymbolInfo* found = context.findSymbol("ButtonStyle");
        assert(found != nullptr);
        assert(found->type == SymbolType::TEMPLATE_STYLE);
        std::cout << "✓ Found template across scopes" << std::endl;
        
        {
            // 进入样式块
            CHTL_STATE_GUARD(stateMachine, CHTLStateType::STYLE_BLOCK);
            CHTL_SCOPE_GUARD(context, ContextScopeType::STYLE_SCOPE);
            
            // 设置选择器上下文
            context.getSelectorContext().addAutoClass("button");
            assert(context.getSelectorContext().getEffectiveClass() == "button");
            std::cout << "✓ Selector context updated in style block" << std::endl;
        }
        
        // 样式块退出后，选择器上下文保持
        assert(context.getSelectorContext().getEffectiveClass() == "button");
        std::cout << "✓ Selector context persists after style block" << std::endl;
    }
    
    std::cout << "Integration test completed successfully!" << std::endl;
}

int main() {
    std::cout << "CHTL State Machine and Context Test Program" << std::endl;
    std::cout << "============================================" << std::endl;
    
    try {
        testStateMachine();
        testContext();
        testIntegration();
        
        std::cout << "\n=== All tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}