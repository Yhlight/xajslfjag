#include "ASTGraph.h"
#include <sstream>
#include <fstream>

namespace CHTL {
namespace Test {

std::string ASTGraph::generateDOTGraph(const std::shared_ptr<CHTL::BaseNode>& root) {
    std::ostringstream dot;
    
    dot << "digraph CHTL_AST {\n";
    dot << "  rankdir=TB;\n";
    dot << "  node [shape=box, style=filled, fillcolor=lightblue];\n";
    
    if (root) {
        size_t nodeId = 0;
        dot << generateNodeDOT(root, nodeId);
    }
    
    dot << "}\n";
    
    return dot.str();
}

std::string ASTGraph::generateDOTGraph(const std::shared_ptr<CHTLJS::BaseNode>& root) {
    std::ostringstream dot;
    
    dot << "digraph CHTLJS_AST {\n";
    dot << "  rankdir=TB;\n";
    dot << "  node [shape=box, style=filled, fillcolor=lightgreen];\n";
    
    if (root) {
        size_t nodeId = 0;
        dot << generateNodeDOT(root, nodeId);
    }
    
    dot << "}\n";
    
    return dot.str();
}

std::string ASTGraph::generateSVGGraph(const std::shared_ptr<CHTL::BaseNode>& root) {
    // 简化的SVG生成
    std::ostringstream svg;
    
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">\n";
    svg << "  <rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    svg << "  <text x=\"400\" y=\"30\" text-anchor=\"middle\" font-size=\"16\" font-weight=\"bold\">CHTL AST</text>\n";
    
    if (root) {
        svg << "  <rect x=\"350\" y=\"50\" width=\"100\" height=\"30\" fill=\"lightblue\" stroke=\"black\"/>\n";
        svg << "  <text x=\"400\" y=\"70\" text-anchor=\"middle\" font-size=\"12\">" << escapeLabel(root->toString()) << "</text>\n";
    }
    
    svg << "</svg>\n";
    
    return svg.str();
}

std::string ASTGraph::generateSVGGraph(const std::shared_ptr<CHTLJS::BaseNode>& root) {
    // 简化的SVG生成
    std::ostringstream svg;
    
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">\n";
    svg << "  <rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    svg << "  <text x=\"400\" y=\"30\" text-anchor=\"middle\" font-size=\"16\" font-weight=\"bold\">CHTL JS AST</text>\n";
    
    if (root) {
        svg << "  <rect x=\"350\" y=\"50\" width=\"100\" height=\"30\" fill=\"lightgreen\" stroke=\"black\"/>\n";
        svg << "  <text x=\"400\" y=\"70\" text-anchor=\"middle\" font-size=\"12\">" << escapeLabel(root->toString()) << "</text>\n";
    }
    
    svg << "</svg>\n";
    
    return svg.str();
}

bool ASTGraph::exportGraphToFile(const std::shared_ptr<CHTL::BaseNode>& root, 
                                const std::string& filename, 
                                const std::string& format) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "dot") {
        file << generateDOTGraph(root);
    } else if (format == "svg") {
        file << generateSVGGraph(root);
    } else {
        return false;
    }
    
    return true;
}

bool ASTGraph::exportGraphToFile(const std::shared_ptr<CHTLJS::BaseNode>& root, 
                                const std::string& filename, 
                                const std::string& format) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "dot") {
        file << generateDOTGraph(root);
    } else if (format == "svg") {
        file << generateSVGGraph(root);
    } else {
        return false;
    }
    
    return true;
}

std::string ASTGraph::generateNodeDOT(const std::shared_ptr<CHTL::BaseNode>& node, size_t& nodeId) {
    std::ostringstream dot;
    
    size_t currentId = nodeId++;
    dot << "  node" << currentId << " [label=\"" << escapeLabel(node->toString()) << "\"];\n";
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            size_t childId = nodeId;
            dot << generateNodeDOT(child, nodeId);
            dot << "  node" << currentId << " -> node" << childId << ";\n";
        }
    }
    
    return dot.str();
}

std::string ASTGraph::generateNodeDOT(const std::shared_ptr<CHTLJS::BaseNode>& node, size_t& nodeId) {
    std::ostringstream dot;
    
    size_t currentId = nodeId++;
    dot << "  node" << currentId << " [label=\"" << escapeLabel(node->toString()) << "\"];\n";
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            size_t childId = nodeId;
            dot << generateNodeDOT(child, nodeId);
            dot << "  node" << currentId << " -> node" << childId << ";\n";
        }
    }
    
    return dot.str();
}

std::string ASTGraph::escapeLabel(const std::string& label) {
    std::string escaped = label;
    
    // 转义DOT特殊字符
    size_t pos = 0;
    while ((pos = escaped.find("\"", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = escaped.find("\n", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\n");
        pos += 2;
    }
    
    return escaped;
}

} // namespace Test
} // namespace CHTL