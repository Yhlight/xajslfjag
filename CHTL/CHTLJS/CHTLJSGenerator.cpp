#include "CHTLJSGenerator.h"
#include <algorithm>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator(std::shared_ptr<GlobalMap> globalMap)
    : globalMap(globalMap), minify(false), indentSize(4), useSpaces(true),
      currentIndent(0), lineCount(0), charCount(0) {
}

CHTLJSGenerator::~CHTLJSGenerator() = default;

std::string CHTLJSGenerator::Generate(NodePtr root) {
    output.str("");
    output.clear();
    lineCount = 0;
    charCount = 0;
    currentIndent = 0;
    
    GenerateNode(root);
    
    // 生成事件委托代码
    GenerateEventDelegations();
    
    std::string result = output.str();
    charCount = result.length();
    
    return result;
}

void CHTLJSGenerator::GenerateNode(NodePtr node) {
    if (!node) return;
    
    switch (node->GetType()) {
        case NodeType::PROGRAM:
            GenerateProgram(node);
            break;
            
        case NodeType::ENHANCED_SELECTOR:
            GenerateEnhancedSelector(std::static_pointer_cast<EnhancedSelectorNode>(node));
            break;
            
        case NodeType::ARROW_ACCESS:
            GenerateArrowAccess(std::static_pointer_cast<ArrowAccessNode>(node));
            break;
            
        case NodeType::VIR_DECLARATION:
            GenerateVirDeclaration(std::static_pointer_cast<VirDeclarationNode>(node));
            break;
            
        case NodeType::CHTLJS_FUNCTION:
            GenerateCHTLJSFunction(std::static_pointer_cast<CHTLJSFunctionNode>(node));
            break;
            
        case NodeType::EVENT_BINDING:
            GenerateEventBinding(std::static_pointer_cast<EventBindingNode>(node));
            break;
            
        case NodeType::LITERAL:
            GenerateLiteral(std::static_pointer_cast<LiteralNode>(node));
            break;
            
        case NodeType::IDENTIFIER:
            GenerateIdentifier(std::static_pointer_cast<IdentifierNode>(node));
            break;
            
        case NodeType::BINARY_OP:
            GenerateBinaryOp(std::static_pointer_cast<BinaryOpNode>(node));
            break;
            
        case NodeType::FUNCTION_CALL:
            GenerateFunctionCall(std::static_pointer_cast<FunctionCallNode>(node));
            break;
            
        case NodeType::OBJECT_LITERAL:
            GenerateObjectLiteral(std::static_pointer_cast<ObjectLiteralNode>(node));
            break;
            
        default:
            // 生成子节点
            for (auto& child : node->GetChildren()) {
                GenerateNode(child);
            }
            break;
    }
}

void CHTLJSGenerator::GenerateProgram(NodePtr node) {
    // 生成"use strict"
    if (!minify) {
        WriteLine("\"use strict\";");
        WriteLine();
    } else {
        Write("\"use strict\";");
    }
    
    // 生成所有语句
    for (auto& child : node->GetChildren()) {
        GenerateNode(child);
    }
}

void CHTLJSGenerator::GenerateEnhancedSelector(std::shared_ptr<EnhancedSelectorNode> node) {
    // 检查缓存
    std::string selector = node->GetSelector();
    if (globalMap->HasCachedSelector(selector)) {
        Write(globalMap->GetCachedSelector(selector));
    } else {
        Write(node->GenerateJS());
    }
}

void CHTLJSGenerator::GenerateArrowAccess(std::shared_ptr<ArrowAccessNode> node) {
    // -> 转换为 .
    GenerateNode(node->GetObject());
    Write(".");
    Write(node->GetProperty());
}

void CHTLJSGenerator::GenerateVirDeclaration(std::shared_ptr<VirDeclarationNode> node) {
    // vir声明不生成代码，它是编译时概念
    // 但需要处理其值的副作用
    auto value = node->GetValue();
    if (value && value->GetType() == NodeType::CHTLJS_FUNCTION) {
        // 如果是CHTL JS函数，可能需要生成一些初始化代码
        GenerateNode(value);
    }
}

void CHTLJSGenerator::GenerateCHTLJSFunction(std::shared_ptr<CHTLJSFunctionNode> node) {
    std::string funcName = node->GetFunctionName();
    
    if (funcName == "listen") {
        // 生成addEventListener链式调用
        bool first = true;
        for (const auto& child : node->GetChildren()) {
            // TODO: 需要获取属性的方式
        }
    } else if (funcName == "delegate") {
        // 事件委托会被收集并统一生成
        // TODO: 实现委托收集
    } else if (funcName == "animate") {
        // 生成动画配置
        std::string animId = globalMap->GenerateAnimationId();
        std::string animConfig = GenerateAnimationConfig(node);
        globalMap->RegisterAnimation(animId, animConfig);
        
        Write("CHTLJS.animate(" + animConfig + ")");
    }
}

void CHTLJSGenerator::GenerateEventBinding(std::shared_ptr<EventBindingNode> node) {
    GenerateNode(node->GetTarget());
    Write(".addEventListener('");
    Write(node->GetEvent());
    Write("', ");
    GenerateNode(node->GetHandler());
    Write(")");
}

void CHTLJSGenerator::GenerateLiteral(std::shared_ptr<LiteralNode> node) {
    Write(node->GenerateJS());
}

void CHTLJSGenerator::GenerateIdentifier(std::shared_ptr<IdentifierNode> node) {
    Write(node->GetName());
}

void CHTLJSGenerator::GenerateBinaryOp(std::shared_ptr<BinaryOpNode> node) {
    std::string op = node->GetOperator();
    
    // 处理 ** 操作符
    if (op == "**") {
        Write("Math.pow(");
        GenerateNode(node->GetLeft());
        Write(", ");
        GenerateNode(node->GetRight());
        Write(")");
    } else {
        GenerateNode(node->GetLeft());
        Write(" " + op + " ");
        GenerateNode(node->GetRight());
    }
}

void CHTLJSGenerator::GenerateFunctionCall(std::shared_ptr<FunctionCallNode> node) {
    GenerateNode(node->GetFunction());
    Write("(");
    
    auto args = node->GetArguments();
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) Write(", ");
        GenerateNode(args[i]);
    }
    
    Write(")");
}

void CHTLJSGenerator::GenerateObjectLiteral(std::shared_ptr<ObjectLiteralNode> node) {
    if (minify) {
        Write("{");
    } else {
        WriteLine("{");
        IncreaseIndent();
    }
    
    auto props = node->GetProperties();
    bool first = true;
    
    for (const auto& [key, value] : props) {
        if (!first) {
            if (minify) {
                Write(",");
            } else {
                WriteLine(",");
            }
        }
        first = false;
        
        if (!minify) WriteIndent();
        
        // 检查key是否需要引号
        bool needQuotes = key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$") != std::string::npos;
        
        if (needQuotes) {
            Write("\"" + key + "\"");
        } else {
            Write(key);
        }
        
        Write(": ");
        GenerateNode(value);
    }
    
    if (!minify) {
        WriteLine();
        DecreaseIndent();
        WriteIndent();
    }
    Write("}");
}

void CHTLJSGenerator::GenerateEventDelegations() {
    auto parents = globalMap->GetAllDelegationParents();
    if (parents.empty()) return;
    
    if (!minify) {
        WriteLine();
        WriteLine("// Event Delegations");
    }
    
    for (const auto& parent : parents) {
        auto delegation = globalMap->GetEventDelegation(parent);
        if (delegation) {
            Write(delegation->GenerateJS());
            if (!minify) WriteLine();
        }
    }
}

std::string CHTLJSGenerator::GenerateAnimationConfig(std::shared_ptr<CHTLJSFunctionNode> animNode) {
    std::stringstream ss;
    ss << "{";
    
    // TODO: 实现动画配置生成
    
    ss << "}";
    return ss.str();
}

void CHTLJSGenerator::WriteIndent() {
    if (!minify) {
        Write(GetIndentString());
    }
}

void CHTLJSGenerator::Write(const std::string& text) {
    output << text;
}

void CHTLJSGenerator::WriteLine(const std::string& text) {
    if (!minify) {
        output << text << "\n";
        lineCount++;
    } else {
        output << text;
    }
}

void CHTLJSGenerator::IncreaseIndent() {
    currentIndent++;
}

void CHTLJSGenerator::DecreaseIndent() {
    if (currentIndent > 0) {
        currentIndent--;
    }
}

std::string CHTLJSGenerator::GetIndentString() const {
    if (useSpaces) {
        return std::string(currentIndent * indentSize, ' ');
    } else {
        return std::string(currentIndent, '\t');
    }
}

} // namespace CHTLJS