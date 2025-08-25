#include "ASTGraph.h"
#include <fstream>
#include <sstream>

namespace CHTL {
namespace Test {

std::string ASTGraph::GetNodeId(const AstNode* node) {
    std::stringstream ss;
    ss << "node" << (void*)node;
    return ss.str();
}

void ASTGraph::Generate(const ProgramNode& root, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return;
    m_OutputStream = &outFile;

    *m_OutputStream << "digraph AST {\n";
    *m_OutputStream << "  node [shape=box];\n";
    root.Accept(this);
    *m_OutputStream << "}\n";
}

void ASTGraph::Visit(const ProgramNode& node) {
    std::string id = GetNodeId(&node);
    *m_OutputStream << "  " << id << " [label=\"ProgramNode\"];\n";
    for (const auto& child : node.Children) {
        *m_OutputStream << "  " << id << " -> " << GetNodeId(child.get()) << ";\n";
        child->Accept(this);
    }
}

void ASTGraph::Visit(const ElementNode& node) {
    std::string id = GetNodeId(&node);
    *m_OutputStream << "  " << id << " [label=\"ElementNode: <" << node.TagName << ">\"];\n";
    for (const auto& child : node.Children) {
        *m_OutputStream << "  " << id << " -> " << GetNodeId(child.get()) << ";\n";
        child->Accept(this);
    }
}

void ASTGraph::Visit(const TextNode& node) {
    std::string id = GetNodeId(&node);
    *m_OutputStream << "  " << id << " [label=\"TextNode: \\\"" << node.Content << "\\\""];\n";
}

// ... implementations for all other Visit methods

} // namespace Test
} // namespace CHTL
