#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include "CHTLJSNode.h"
#include "CHTLJSLexer/GlobalMap.h"

namespace CHTLJS {

// CHTL JS代码生成器
class CHTLJSGenerator {
public:
    CHTLJSGenerator(std::shared_ptr<GlobalMap> globalMap);
    ~CHTLJSGenerator();
    
    // 生成JavaScript代码
    std::string Generate(NodePtr root);
    
    // 配置选项
    void SetMinify(bool minify) { this->minify = minify; }
    void SetIndentSize(int size) { indentSize = size; }
    void SetUseSpaces(bool spaces) { useSpaces = spaces; }
    
    // 获取生成的代码统计
    int GetLineCount() const { return lineCount; }
    int GetCharCount() const { return charCount; }
    
private:
    std::shared_ptr<GlobalMap> globalMap;
    std::stringstream output;
    
    // 格式化选项
    bool minify;
    int indentSize;
    bool useSpaces;
    int currentIndent;
    int lineCount;
    int charCount;
    
    // 生成方法
    void GenerateNode(NodePtr node);
    void GenerateProgram(NodePtr node);
    void GenerateStatement(NodePtr node);
    void GenerateExpression(NodePtr node);
    
    // CHTL JS特有节点生成
    void GenerateEnhancedSelector(std::shared_ptr<EnhancedSelectorNode> node);
    void GenerateArrowAccess(std::shared_ptr<ArrowAccessNode> node);
    void GenerateVirDeclaration(std::shared_ptr<VirDeclarationNode> node);
    void GenerateCHTLJSFunction(std::shared_ptr<CHTLJSFunctionNode> node);
    void GenerateEventBinding(std::shared_ptr<EventBindingNode> node);
    
    // 标准节点生成
    void GenerateLiteral(std::shared_ptr<LiteralNode> node);
    void GenerateIdentifier(std::shared_ptr<IdentifierNode> node);
    void GenerateBinaryOp(std::shared_ptr<BinaryOpNode> node);
    void GenerateFunctionCall(std::shared_ptr<FunctionCallNode> node);
    void GenerateObjectLiteral(std::shared_ptr<ObjectLiteralNode> node);
    
    // 格式化辅助
    void WriteIndent();
    void Write(const std::string& text);
    void WriteLine(const std::string& text = "");
    void IncreaseIndent();
    void DecreaseIndent();
    std::string GetIndentString() const;
    
    // 事件委托处理
    void GenerateEventDelegations();
    
    // 动画处理
    std::string GenerateAnimationConfig(std::shared_ptr<CHTLJSFunctionNode> animNode);
};

} // namespace CHTLJS