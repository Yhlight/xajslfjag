#include "CHTL_JS/Generator/Generator.h"
#include "CHTL_JS/Core/ViewRegistry.h"
#include "CHTL_JS/Generator/SelectorGenerator.h"
#include "CHTL_JS/BuiltinFunctions/AnimateHandler.h"
#include "CHTL_JS/BuiltinFunctions/ListenHandler.h"
#include "CHTL_JS/BuiltinFunctions/DelegateHandler.h"
#include <sstream>

namespace CHTL_JS {

// Constructor
Generator::Generator() {
}

// Generate code for different node types
void Generator::VisitSelectorNode(const SelectorNode& node) {
    m_Result << SelectorGenerator::Generate(node);
}

void Generator::VisitListenNode(const ListenNode& node) {
    m_Result << ListenHandler::Generate(node);
}

void Generator::VisitDelegateNode(const DelegateNode& node) {
    DelegateHandler::Process(node);
    // Delegate doesn't generate inline code, it registers for later generation
}

void Generator::VisitAnimateNode(const AnimateNode& node) {
    m_Result << AnimateHandler::Generate(node);
}

void Generator::VisitVirObjectDefNode(std::shared_ptr<VirObjectDefNode> node) {
    // Parse the function call to extract View information
    View view;
    view.Name = node->Name;
    
    // TODO: Parse node->ArgumentsNode to extract member functions
    // For now, we'll add a placeholder
    
    ViewRegistry::GetInstance().RegisterView(view.Name, std::make_shared<View>(view));
}

void Generator::VisitVirMemberAccessNode(const VirMemberAccessNode& node) {
    auto view = ViewRegistry::GetInstance().GetView(node.ObjectName);
    if (!view) {
        m_Result << "/* Error: vir object '" << node.ObjectName << "' not found */";
        return;
    }

    auto memberIt = view->Members.find(node.MemberName);
    if (memberIt == view->Members.end()) {
        m_Result << "/* Error: member '" << node.MemberName << "' not found in vir object '" << node.ObjectName << "' */";
        return;
    }

    const MemberInfo& memberInfo = memberIt->second;

    // Type-based Code Generation
    if (memberInfo.Type == MemberType::FUNCTION) {
        // For a function, generate a reference to the special global function
        std::string funcName = "__vir_" + node.ObjectName + "_" + node.MemberName;
        m_Result << funcName;
        
        if (auto funcNode = std::dynamic_pointer_cast<FunctionLiteralNode>(memberInfo.ValueNode)) {
            m_VirFunctionsToGenerate.push_back({node.ObjectName, node.MemberName, funcNode});
        }
    } else {
        // For any other type (Object, Array, Literal), generate the value directly
        m_Result << GenerateStringFromNode(memberInfo.ValueNode);
    }
}

void Generator::GenerateVirFunctions() {
    // Iterate through the collected vir functions and generate their definitions
    for (const auto& virFunc : m_VirFunctionsToGenerate) {
        m_Result << "function __vir_" << virFunc.ObjectName << "_" << virFunc.MemberName << "(";
        
        // Generate parameters
        if (virFunc.FunctionNode) {
            bool first = true;
            for (const auto& param : virFunc.FunctionNode->Parameters) {
                if (!first) m_Result << ", ";
                m_Result << param;
                first = false;
            }
        }
        
        m_Result << ") {\n";
        
        // Generate function body
        if (virFunc.FunctionNode) {
            m_Result << virFunc.FunctionNode->Body;
        }
        
        m_Result << "\n}\n\n";
    }
}

std::string Generator::GenerateStringFromNode(std::shared_ptr<AstNode> node) {
    // TODO: Implement proper node-to-string generation
    // For now, return a placeholder
    return "/* node value */";
}

void Generator::AddVirFunctionToGenerate(const std::string& objectName, 
                                        const std::string& memberName, 
                                        std::shared_ptr<FunctionLiteralNode> funcNode) {
    m_VirFunctionsToGenerate.push_back({objectName, memberName, funcNode});
}

std::string Generator::Generate(std::shared_ptr<AstNode> rootNode) {
    m_Result.str("");
    m_VirFunctionsToGenerate.clear();
    
    // TODO: Visit the root node and all its children
    // For now, return empty result
    
    // Generate vir functions at the end
    GenerateVirFunctions();
    
    return m_Result.str();
}

} // namespace CHTL_JS