#include "ASTPrint.h"

namespace CHTL {
namespace Test {

void ASTPrint::Indent() {
    for (int i = 0; i < m_IndentLevel; ++i) {
        std::cout << "  ";
    }
}

void ASTPrint::Print(const ProgramNode& root) {
    root.Accept(this);
}

void ASTPrint::Visit(const ProgramNode& node) {
    Indent();
    std::cout << "ProgramNode" << std::endl;
    m_IndentLevel++;
    for (const auto& child : node.Children) {
        child->Accept(this);
    }
    m_IndentLevel--;
}

void ASTPrint::Visit(const ElementNode& node) {
    Indent();
    std::cout << "ElementNode: <" << node.TagName << ">" << std::endl;
    m_IndentLevel++;
    for (const auto& child : node.Children) {
        child->Accept(this);
    }
    m_IndentLevel--;
}

void ASTPrint::Visit(const TextNode& node) {
    Indent();
    std::cout << "TextNode: \"" << node.Content << "\"" << std::endl;
}

// ... implementations for all other Visit methods

} // namespace Test
} // namespace CHTL
