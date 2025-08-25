#include "CHTL_JS/Generator/Generator.h"
#include "CHTL_JS/Core/ViewRegistry.h"

namespace CHTL_JS {

// ... (other visit methods)

void Generator::VisitVirMemberAccessNode(const VirMemberAccessNode& node) {
    auto view = ViewRegistry::GetInstance().GetView(node.ObjectName);
    if (!view) { /* Error: vir object not found */ return; }

    auto memberIt = view->Members.find(node.MemberName);
    if (memberIt == view->Members.end()) { /* Error: member not found */ return; }

    AstNodePtr memberNode = memberIt->second;

    // --- Type-based Code Generation ---
    if (auto funcNode = std::dynamic_pointer_cast<FunctionLiteralNode>(memberNode)) {
        // For a function, generate a reference to the special global function.
        // The generator needs a separate pass to define these global functions.
        m_Result << "__vir_" << node.ObjectName << "_" << node.MemberName;
        AddVirFunctionToGenerate(node.ObjectName, node.MemberName, funcNode);
    } else {
        // For any other type (Object, Array, Literal), we can generate the value directly.
        // This requires the generator to be able to visit any node and produce its string representation.
        m_Result << GenerateStringFromNode(memberNode);
    }
}

void Generator::GenerateVirFunctions() {
    // Iterate through the collected vir functions and generate their definitions.
    for (const auto& virFunc : m_VirFunctionsToGenerate) {
        m_Result << "function " << "__vir_" << virFunc.ObjectName << "_" << virFunc.MethodName << "() {\n";
        m_Result << "  // Body of the function from the AST\n";
        m_Result << "}
";
    }
}

} // namespace CHTL_JS
