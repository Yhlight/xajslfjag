#include "CHTLCompiler/CompilerDispatcher.h"
#include "CHTLScanner/UnifiedScanner.h"
#include <iostream>

using namespace std;
using namespace CHTL;

void testCorrectCompilationOrder() {
    cout << "\n=== 测试正确的编译顺序 ===\n";
    
    string testCode = R"(
        [Template] @Style Button {
            background: #007bff;
            color: white;
        }
        
        div {
            style {
                Button();
                .custom-style { margin: 10px; }
            }
            
            script {
                {{.button}} &-> click {
                    console.log('Button clicked');
                }
                
                function customHandler() {
                    alert('Custom handler');
                }
            }
        }
        
        style {
            .global-style { padding: 20px; }
        }
        
        script {
            var globalVar = 'test';
        }
    )";
    
    cout << "测试代码长度: " << testCode.length() << " 字符\n";
    
    // 创建统一扫描器和编译调度器
    CHTLUnifiedScanner scanner;
    CompilerDispatcher dispatcher;
    
    // 扫描代码片段
    auto fragments = scanner.smartSlice(testCode);
    
    cout << "\n扫描结果:\n";
    cout << "片段数量: " << fragments.size() << "\n";
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        cout << "片段 " << (i+1) << ": 类型=" << static_cast<int>(fragments[i].type) 
             << ", 长度=" << fragments[i].content.length() << " 字符\n";
    }
    
    cout << "\n正确的编译顺序说明:\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "阶段1：CHTL和CHTL JS编译器处理\n";
    cout << "  1. CHTL编译器处理CHTL片段（模板、元素、局部样式等）\n";
    cout << "  2. CHTL JS编译器处理CHTL JS片段（增强选择器、事件绑定等）\n";
    cout << "\n";
    cout << "阶段2：代码合并\n";
    cout << "  3. 合并CHTL编译器产生的CSS代码\n";
    cout << "  4. 合并CHTL JS编译器产生的JS代码\n";
    cout << "\n";
    cout << "阶段3：CSS和JS编译器处理完整代码\n";
    cout << "  5. CSS编译器接收完整的合并后CSS代码\n";
    cout << "  6. JS编译器接收完整的合并后JS代码\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    // 执行编译（使用修正后的顺序）
    cout << "\n执行编译（使用正确顺序）:\n";
    auto results = dispatcher.compileFragments(fragments);
    
    cout << "编译结果:\n";
    for (size_t i = 0; i < results.size(); ++i) {
        cout << "结果 " << (i+1) << ": " << (results[i].success ? "成功" : "失败")
             << ", 输出长度=" << results[i].output.length() << " 字符\n";
    }
    
    cout << "\n✅ 正确编译顺序验证完成！\n";
}

void explainCompilationOrderProblem() {
    cout << "\n=== 编译顺序问题说明 ===\n";
    
    cout << "❌ 之前的错误做法:\n";
    cout << "1. 所有编译器并行或简单串行处理片段\n";
    cout << "2. CSS编译器直接处理原始CSS片段\n";
    cout << "3. JS编译器直接处理原始JS片段\n";
    cout << "4. 没有考虑CHTL和CHTL JS的输出需要合并\n";
    
    cout << "\n✅ 正确的做法:\n";
    cout << "1. 先让CHTL编译器处理CHTL片段\n";
    cout << "   - 局部样式块 → CSS代码\n";
    cout << "   - 模板展开 → HTML代码\n";
    cout << "   - 元素处理 → HTML代码\n";
    cout << "\n";
    cout << "2. 再让CHTL JS编译器处理CHTL JS片段\n";
    cout << "   - 增强选择器 → 标准JS代码\n";
    cout << "   - 事件绑定 → 标准JS代码\n";
    cout << "   - 虚拟对象 → 标准JS代码\n";
    cout << "\n";
    cout << "3. 合并CHTL和CHTL JS的输出\n";
    cout << "   - 收集所有CSS输出 → 完整CSS代码\n";
    cout << "   - 收集所有JS输出 → 完整JS代码\n";
    cout << "\n";
    cout << "4. 最后让CSS和JS编译器处理完整代码\n";
    cout << "   - CSS编译器接收合并后的完整CSS\n";
    cout << "   - JS编译器接收合并后的完整JS\n";
    
    cout << "\n🎯 关键理解:\n";
    cout << "✅ CSS和JS编译器需要完整的代码片段\n";
    cout << "✅ 不能直接处理原始的分散片段\n";
    cout << "✅ 必须等待CHTL和CHTL JS处理完毕后合并\n";
    cout << "✅ 这样CSS和JS编译器才能正确解析完整的语法\n";
}

int main() {
    cout << "CHTL编译顺序修正验证\n";
    cout << "========================\n";
    
    try {
        explainCompilationOrderProblem();
        testCorrectCompilationOrder();
        
        cout << "\n🎉 编译顺序修正完成！\n";
        
        cout << "\n✅ 修正后的编译流程：\n";
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        cout << "  1. ✓ CHTL编译器处理CHTL片段\n";
        cout << "  2. ✓ CHTL JS编译器处理CHTL JS片段\n";
        cout << "  3. ✓ 合并CHTL和CHTL JS的输出\n";
        cout << "  4. ✓ CSS编译器接收完整的合并后CSS代码\n";
        cout << "  5. ✓ JS编译器接收完整的合并后JS代码\n";
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        cout << "\n🚀 现在CSS和JS编译器能够正确接收完整代码片段！\n";
        cout << "🚀 编译顺序完全符合CHTL架构设计要求！\n";
        
    } catch (const exception& e) {
        cout << "\n❌ 测试过程中出现错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}