#ifndef CHTLJS_GENERATOR_H
#define CHTLJS_GENERATOR_H

#include <string>
#include <memory>
#include <sstream>
#include <stack>
#include <unordered_map>
#include "../CHTLJSNode/BaseNode.h"
#include "../CHTLJSNode/ProgramNode.h"
#include "../CHTLJSNode/ModuleNode.h"
#include "../CHTLJSNode/SelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include "../CHTLJSNode/OperatorNode.h"
#include "../CHTLJSNode/JavaScriptNode.h"
#include "../CHTLJSContext/Context.h"

namespace CHTLJS {

// 生成器配置
struct GeneratorConfig {
    bool prettyPrint = true;            // 美化输出
    int indentSize = 2;                 // 缩进大小
    bool generateSourceMap = false;     // 生成源映射
    bool minify = false;                // 压缩输出
    std::string lineEnding = "\n";      // 行结束符
    bool wrapInIIFE = true;            // 包装在立即执行函数中
};

// JavaScript生成器
class Generator : public CompleteVisitor {
public:
    Generator(std::shared_ptr<CompileContext> context,
              const GeneratorConfig& config = GeneratorConfig());
    ~Generator() = default;
    
    // 生成JavaScript
    std::string generate(std::shared_ptr<ProgramNode> program);
    
    // 访问者方法实现
    void visitProgramNode(ProgramNode* node) override;
    void visitStatementNode(StatementNode* node) override;
    void visitIdentifierNode(IdentifierNode* node) override;
    void visitLiteralNode(LiteralNode* node) override;
    void visitModuleNode(ModuleNode* node) override;
    void visitEnhancedSelectorNode(EnhancedSelectorNode* node) override;
    void visitListenNode(ListenNode* node) override;
    void visitDelegateNode(DelegateNode* node) override;
    void visitAnimateNode(AnimateNode* node) override;
    void visitAnimateStateNode(AnimateStateNode* node) override;
    void visitVirtualObjectNode(VirtualObjectNode* node) override;
    void visitINeverAwayNode(INeverAwayNode* node) override;
    void visitArrowAccessNode(ArrowAccessNode* node) override;
    void visitEventBindingNode(EventBindingNode* node) override;
    void visitBinaryExpressionNode(BinaryExpressionNode* node) override;
    void visitUnaryExpressionNode(UnaryExpressionNode* node) override;
    void visitFunctionDeclarationNode(FunctionDeclarationNode* node) override;
    void visitVariableDeclarationNode(VariableDeclarationNode* node) override;
    void visitObjectLiteralNode(ObjectLiteralNode* node) override;
    void visitArrayLiteralNode(ArrayLiteralNode* node) override;
    void visitCallExpressionNode(CallExpressionNode* node) override;

private:
    std::shared_ptr<CompileContext> context_;
    GeneratorConfig config_;
    std::stringstream output_;
    std::stringstream moduleLoader_;
    int indentLevel_ = 0;
    
    // 生成状态
    struct GeneratorState {
        bool inModule = false;
        bool inSelector = false;
        bool inEventDelegation = false;
        std::string currentVirtualObject;
        std::unordered_map<std::string, std::string> selectorCache;
        std::unordered_map<std::string, std::string> virtualObjectCache;
    };
    
    std::stack<GeneratorState> stateStack_;
    GeneratorState currentState_;
    
    // 输出辅助方法
    void write(const std::string& text);
    void writeLine(const std::string& text = "");
    void indent();
    void dedent();
    std::string getIndent() const;
    
    // JavaScript生成方法
    void generateModuleLoader();
    void generateSelectorHelpers();
    void generateEventDelegationSystem();
    void generateAnimationHelpers();
    void generateVirtualObjectSystem();
    
    // 增强选择器生成
    std::string generateSelectorCode(EnhancedSelectorNode* node);
    std::string getSelectorFunction(EnhancedSelectorNode::SelectorType type);
    
    // listen块生成
    void generateListenCode(ListenNode* node, const std::string& target);
    
    // delegate块生成
    void generateDelegateCode(DelegateNode* node, const std::string& parent);
    
    // animate块生成
    void generateAnimateCode(AnimateNode* node);
    std::string generateAnimationOptions(AnimateNode* node);
    
    // 虚对象生成
    void generateVirtualObjectCode(VirtualObjectNode* node);
    void cacheVirtualObject(const std::string& name, INeverAwayNode* node);
    
    // 表达式生成
    void generateBinaryOperator(BinaryExpressionNode::Operator op);
    void generateUnaryOperator(UnaryExpressionNode::Operator op);
    
    // 辅助方法
    std::string escapeString(const std::string& str);
    std::string generateUniqueId();
    bool isReservedWord(const std::string& word);
    
    // 状态管理
    void pushState();
    void popState();
};

// 生成器异常
class GeneratorException : public std::exception {
public:
    explicit GeneratorException(const std::string& message) : msg_(message) {}
    const char* what() const noexcept override { return msg_.c_str(); }
    
private:
    std::string msg_;
};

} // namespace CHTLJS

#endif // CHTLJS_GENERATOR_H