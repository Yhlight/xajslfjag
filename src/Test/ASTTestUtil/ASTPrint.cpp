#include "ASTPrint.h"
#include <queue>
#include <stack>
#include <iomanip>
#include <fstream>
#include <algorithm>

namespace CHTL {
namespace Test {

// ASTPrinter implementation
void ASTPrinter::print(const BaseNode* node) {
    if (!node) {
        *os_ << "(null)\n";
        return;
    }
    
    switch (format_) {
        case Format::TREE:
            printTree(node);
            break;
        case Format::INDENTED:
            printIndented(node);
            break;
        case Format::LISP:
            printLisp(node);
            break;
        case Format::JSON:
            printJSON(node);
            break;
        case Format::XML:
            printXML(node);
            break;
        case Format::DOT:
            printDOT(node);
            break;
    }
}

void ASTPrinter::printTree(const BaseNode* node, const std::string& prefix, bool isLast) {
    if (!node || (maxDepth_ >= 0 && prefix.length() / 4 > maxDepth_)) return;
    
    *os_ << prefix;
    *os_ << (isLast ? "└── " : "├── ");
    
    // 节点信息
    std::string nodeStr = nodeTypeToString(node->getType());
    if (colorOutput_) {
        nodeStr = colorize(nodeStr, getColorForNodeType(node->getType()));
    }
    
    *os_ << nodeStr;
    
    if (showValues_ && !node->getValue().empty()) {
        *os_ << ": " << node->getValue();
    }
    
    if (showPositions_) {
        *os_ << " [" << node->getLine() << ":" << node->getColumn() << "]";
    }
    
    *os_ << "\n";
    
    // 子节点
    const auto& children = node->getChildren();
    for (size_t i = 0; i < children.size(); ++i) {
        std::string childPrefix = prefix + (isLast ? "    " : "│   ");
        bool isLastChild = (i == children.size() - 1);
        printTree(children[i].get(), childPrefix, isLastChild);
    }
}

void ASTPrinter::printIndented(const BaseNode* node, int indent) {
    if (!node || (maxDepth_ >= 0 && indent > maxDepth_)) return;
    
    // 缩进
    for (int i = 0; i < indent; ++i) {
        *os_ << "  ";
    }
    
    // 节点信息
    *os_ << nodeTypeToString(node->getType());
    
    if (showValues_ && !node->getValue().empty()) {
        *os_ << " \"" << node->getValue() << "\"";
    }
    
    *os_ << "\n";
    
    // 子节点
    for (const auto& child : node->getChildren()) {
        printIndented(child.get(), indent + 1);
    }
}

void ASTPrinter::printLisp(const BaseNode* node) {
    if (!node) {
        *os_ << "nil";
        return;
    }
    
    *os_ << "(" << nodeTypeToString(node->getType());
    
    if (!node->getValue().empty()) {
        *os_ << " \"" << escapeString(node->getValue()) << "\"";
    }
    
    for (const auto& child : node->getChildren()) {
        *os_ << " ";
        printLisp(child.get());
    }
    
    *os_ << ")";
}

void ASTPrinter::printJSON(const BaseNode* node, int indent) {
    if (!node) {
        *os_ << "null";
        return;
    }
    
    std::string ind(indent * 2, ' ');
    std::string ind2((indent + 1) * 2, ' ');
    
    *os_ << "{\n";
    *os_ << ind2 << "\"type\": \"" << nodeTypeToString(node->getType()) << "\"";
    
    if (!node->getValue().empty()) {
        *os_ << ",\n" << ind2 << "\"value\": \"" << escapeString(node->getValue()) << "\"";
    }
    
    if (showPositions_) {
        *os_ << ",\n" << ind2 << "\"position\": {"
             << "\"line\": " << node->getLine()
             << ", \"column\": " << node->getColumn() << "}";
    }
    
    const auto& children = node->getChildren();
    if (!children.empty()) {
        *os_ << ",\n" << ind2 << "\"children\": [\n";
        
        for (size_t i = 0; i < children.size(); ++i) {
            *os_ << ind2 << "  ";
            printJSON(children[i].get(), indent + 2);
            if (i < children.size() - 1) {
                *os_ << ",";
            }
            *os_ << "\n";
        }
        
        *os_ << ind2 << "]";
    }
    
    *os_ << "\n" << ind << "}";
}

void ASTPrinter::printXML(const BaseNode* node, int indent) {
    if (!node) return;
    
    std::string ind(indent * 2, ' ');
    
    *os_ << ind << "<" << nodeTypeToString(node->getType());
    
    if (!node->getValue().empty()) {
        *os_ << " value=\"" << escapeString(node->getValue()) << "\"";
    }
    
    if (showPositions_) {
        *os_ << " line=\"" << node->getLine() << "\" column=\"" << node->getColumn() << "\"";
    }
    
    const auto& children = node->getChildren();
    if (children.empty()) {
        *os_ << "/>\n";
    } else {
        *os_ << ">\n";
        
        for (const auto& child : children) {
            printXML(child.get(), indent + 1);
        }
        
        *os_ << ind << "</" << nodeTypeToString(node->getType()) << ">\n";
    }
}

void ASTPrinter::printDOT(const BaseNode* node) {
    *os_ << "digraph AST {\n";
    *os_ << "  node [shape=box];\n";
    
    int nodeId = 0;
    printDOTNode(node, nodeId);
    
    nodeId = 0;
    int rootId = nodeId++;
    printDOTEdges(node, rootId, nodeId);
    
    *os_ << "}\n";
}

void ASTPrinter::printDOTNode(const BaseNode* node, int& nodeId) {
    if (!node) return;
    
    int currentId = nodeId++;
    
    *os_ << "  n" << currentId << " [label=\"" << nodeTypeToString(node->getType());
    if (!node->getValue().empty()) {
        *os_ << "\\n" << escapeString(node->getValue());
    }
    *os_ << "\"];\n";
    
    for (const auto& child : node->getChildren()) {
        printDOTNode(child.get(), nodeId);
    }
}

void ASTPrinter::printDOTEdges(const BaseNode* node, int parentId, int& nodeId) {
    if (!node) return;
    
    const auto& children = node->getChildren();
    for (const auto& child : children) {
        int childId = nodeId++;
        *os_ << "  n" << parentId << " -> n" << childId << ";\n";
        printDOTEdges(child.get(), childId, nodeId);
    }
}

std::string ASTPrinter::toString(const BaseNode* node) {
    std::ostringstream ss;
    std::ostream* oldOs = os_;
    os_ = &ss;
    print(node);
    os_ = oldOs;
    return ss.str();
}

std::string ASTPrinter::nodeTypeToString(NodeType type) {
    static const std::map<NodeType, std::string> typeNames = {
        {NodeType::PROGRAM, "Program"},
        {NodeType::ELEMENT, "Element"},
        {NodeType::TEXT, "Text"},
        {NodeType::COMMENT, "Comment"},
        {NodeType::TEMPLATE, "Template"},
        {NodeType::CUSTOM, "Custom"},
        {NodeType::STYLE, "Style"},
        {NodeType::SCRIPT, "Script"},
        {NodeType::IMPORT, "Import"},
        {NodeType::NAMESPACE, "Namespace"},
        {NodeType::CONFIGURATION, "Configuration"},
        {NodeType::ORIGIN, "Origin"},
        {NodeType::ATTRIBUTE, "Attribute"},
        {NodeType::OPERATOR, "Operator"}
    };
    
    auto it = typeNames.find(type);
    return it != typeNames.end() ? it->second : "Unknown";
}

std::string ASTPrinter::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    return result;
}

std::string ASTPrinter::colorize(const std::string& text, const std::string& color) {
    if (!colorOutput_) return text;
    return color + text + "\033[0m";
}

std::string ASTPrinter::getColorForNodeType(NodeType type) {
    switch (type) {
        case NodeType::PROGRAM: return "\033[35m";      // Magenta
        case NodeType::ELEMENT: return "\033[34m";      // Blue
        case NodeType::TEXT: return "\033[37m";         // White
        case NodeType::COMMENT: return "\033[90m";      // Gray
        case NodeType::TEMPLATE: return "\033[36m";     // Cyan
        case NodeType::CUSTOM: return "\033[33m";       // Yellow
        case NodeType::STYLE: return "\033[32m";        // Green
        case NodeType::SCRIPT: return "\033[31m";       // Red
        default: return "";
    }
}

// ASTComparator implementation
std::vector<ASTComparator::Difference> ASTComparator::compare(const BaseNode* expected, const BaseNode* actual) {
    std::vector<Difference> diffs;
    compareNodes(expected, actual, "/", diffs);
    return diffs;
}

void ASTComparator::compareNodes(const BaseNode* expected, const BaseNode* actual,
                               const std::string& path, std::vector<Difference>& diffs) {
    if (!expected && !actual) return;
    
    if (!expected || !actual) {
        Difference diff;
        diff.path = path;
        diff.field = "existence";
        diff.expected = expected ? "exists" : "null";
        diff.actual = actual ? "exists" : "null";
        diffs.push_back(diff);
        return;
    }
    
    // 比较类型
    if (expected->getType() != actual->getType()) {
        Difference diff;
        diff.path = path;
        diff.field = "type";
        diff.expected = std::to_string(static_cast<int>(expected->getType()));
        diff.actual = std::to_string(static_cast<int>(actual->getType()));
        diffs.push_back(diff);
    }
    
    // 比较值
    if (expected->getValue() != actual->getValue()) {
        Difference diff;
        diff.path = path;
        diff.field = "value";
        diff.expected = expected->getValue();
        diff.actual = actual->getValue();
        diffs.push_back(diff);
    }
    
    // 比较子节点数量
    const auto& expectedChildren = expected->getChildren();
    const auto& actualChildren = actual->getChildren();
    
    if (expectedChildren.size() != actualChildren.size()) {
        Difference diff;
        diff.path = path;
        diff.field = "children_count";
        diff.expected = std::to_string(expectedChildren.size());
        diff.actual = std::to_string(actualChildren.size());
        diffs.push_back(diff);
    }
    
    // 比较子节点
    size_t minSize = std::min(expectedChildren.size(), actualChildren.size());
    for (size_t i = 0; i < minSize; ++i) {
        std::string childPath = path + "/" + std::to_string(i);
        compareNodes(expectedChildren[i].get(), actualChildren[i].get(), childPath, diffs);
    }
}

bool ASTComparator::equals(const BaseNode* a, const BaseNode* b) {
    auto diffs = compare(a, b);
    return diffs.empty();
}

void ASTComparator::printDifferences(const std::vector<Difference>& diffs) {
    if (diffs.empty()) {
        std::cout << "ASTs are identical.\n";
        return;
    }
    
    std::cout << "Found " << diffs.size() << " differences:\n";
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& diff : diffs) {
        std::cout << "Path: " << diff.path << "\n";
        std::cout << "Field: " << diff.field << "\n";
        std::cout << "Expected: " << diff.expected << "\n";
        std::cout << "Actual: " << diff.actual << "\n";
        std::cout << std::string(70, '-') << "\n";
    }
}

// ASTTraverser implementation
void ASTTraverser::preOrder(const BaseNode* root, NodeVisitor visitor) {
    preOrderImpl(root, visitor, 0);
}

void ASTTraverser::preOrderImpl(const BaseNode* node, NodeVisitor visitor, int depth) {
    if (!node) return;
    
    if (!visitor(node, depth)) return;
    
    for (const auto& child : node->getChildren()) {
        preOrderImpl(child.get(), visitor, depth + 1);
    }
}

void ASTTraverser::postOrder(const BaseNode* root, NodeVisitor visitor) {
    postOrderImpl(root, visitor, 0);
}

void ASTTraverser::postOrderImpl(const BaseNode* node, NodeVisitor visitor, int depth) {
    if (!node) return;
    
    for (const auto& child : node->getChildren()) {
        postOrderImpl(child.get(), visitor, depth + 1);
    }
    
    visitor(node, depth);
}

void ASTTraverser::levelOrder(const BaseNode* root, NodeVisitor visitor) {
    if (!root) return;
    
    std::queue<std::pair<const BaseNode*, int>> queue;
    queue.push({root, 0});
    
    while (!queue.empty()) {
        auto [node, depth] = queue.front();
        queue.pop();
        
        if (!visitor(node, depth)) return;
        
        for (const auto& child : node->getChildren()) {
            queue.push({child.get(), depth + 1});
        }
    }
}

std::vector<const BaseNode*> ASTTraverser::findByType(const BaseNode* root, NodeType type) {
    std::vector<const BaseNode*> result;
    
    preOrder(root, [&](const BaseNode* node, int) {
        if (node->getType() == type) {
            result.push_back(node);
        }
        return true;
    });
    
    return result;
}

std::vector<const BaseNode*> ASTTraverser::findByValue(const BaseNode* root, const std::string& value) {
    std::vector<const BaseNode*> result;
    
    preOrder(root, [&](const BaseNode* node, int) {
        if (node->getValue() == value) {
            result.push_back(node);
        }
        return true;
    });
    
    return result;
}

// ASTStatistics implementation
ASTStatistics::Stats ASTStatistics::calculate(const BaseNode* root) {
    Stats stats;
    calculateImpl(root, stats, 0);
    
    if (stats.totalNodes > 0) {
        size_t totalChildren = 0;
        ASTTraverser::preOrder(root, [&](const BaseNode* node, int) {
            totalChildren += node->getChildren().size();
            return true;
        });
        stats.averageFanout = static_cast<double>(totalChildren) / stats.totalNodes;
    }
    
    return stats;
}

void ASTStatistics::calculateImpl(const BaseNode* node, Stats& stats, int depth) {
    if (!node) return;
    
    stats.totalNodes++;
    stats.maxDepth = std::max(stats.maxDepth, static_cast<size_t>(depth));
    stats.nodeTypeCounts[node->getType()]++;
    
    if (!node->getValue().empty()) {
        stats.nodeValueCounts[node->getValue()]++;
    }
    
    const auto& children = node->getChildren();
    stats.maxWidth = std::max(stats.maxWidth, children.size());
    
    for (const auto& child : children) {
        calculateImpl(child.get(), stats, depth + 1);
    }
}

void ASTStatistics::printStats(const Stats& stats) {
    std::cout << "\nAST Statistics:\n";
    std::cout << std::string(50, '-') << "\n";
    std::cout << "Total nodes: " << stats.totalNodes << "\n";
    std::cout << "Max depth: " << stats.maxDepth << "\n";
    std::cout << "Max width: " << stats.maxWidth << "\n";
    std::cout << "Average fanout: " << std::fixed << std::setprecision(2) 
              << stats.averageFanout << "\n";
    
    std::cout << "\nNode type distribution:\n";
    for (const auto& [type, count] : stats.nodeTypeCounts) {
        double percentage = (count * 100.0) / stats.totalNodes;
        std::cout << "  " << std::setw(20) << std::left << ASTPrinter().nodeTypeToString(type)
                  << ": " << std::setw(5) << count
                  << " (" << std::fixed << std::setprecision(1) << percentage << "%)\n";
    }
}

double ASTStatistics::getComplexity(const BaseNode* root) {
    Stats stats = calculate(root);
    // 简单的复杂度计算：节点数 * 深度 * 平均扇出
    return stats.totalNodes * stats.maxDepth * stats.averageFanout;
}

// ASTValidator implementation
std::vector<ASTValidator::ValidationError> ASTValidator::validate(const BaseNode* root) {
    std::vector<ValidationError> errors;
    validateNode(root, "/", errors);
    return errors;
}

void ASTValidator::validateNode(const BaseNode* node, const std::string& path,
                               std::vector<ValidationError>& errors) {
    if (!node) {
        errors.push_back({path, "Null node", "Remove null reference"});
        return;
    }
    
    // 验证节点类型特定规则
    switch (node->getType()) {
        case NodeType::ELEMENT:
            if (node->getValue().empty()) {
                errors.push_back({path, "Element without tag name", "Add tag name"});
            }
            break;
        case NodeType::ATTRIBUTE:
            if (node->getValue().empty()) {
                errors.push_back({path, "Attribute without name", "Add attribute name"});
            }
            break;
        default:
            break;
    }
    
    // 递归验证子节点
    const auto& children = node->getChildren();
    for (size_t i = 0; i < children.size(); ++i) {
        std::string childPath = path + "/" + std::to_string(i);
        validateNode(children[i].get(), childPath, errors);
    }
}

void ASTValidator::printErrors(const std::vector<ValidationError>& errors) {
    if (errors.empty()) {
        std::cout << "AST validation passed.\n";
        return;
    }
    
    std::cout << "Found " << errors.size() << " validation errors:\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& error : errors) {
        std::cout << "Path: " << error.path << "\n";
        std::cout << "Error: " << error.message << "\n";
        if (!error.suggestion.empty()) {
            std::cout << "Suggestion: " << error.suggestion << "\n";
        }
        std::cout << std::string(60, '-') << "\n";
    }
}

// ASTVisualizer implementation
void ASTVisualizer::generateDOT(const BaseNode* root, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }
    
    ASTPrinter printer(ASTPrinter::Format::DOT);
    printer.setOutputStream(file);
    printer.print(root);
    
    file.close();
    std::cout << "DOT file generated: " << filename << "\n";
}

void ASTVisualizer::generateHTML(const BaseNode* root, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }
    
    file << generateHTMLContent(root);
    file.close();
    std::cout << "HTML file generated: " << filename << "\n";
}

std::string ASTVisualizer::generateHTMLContent(const BaseNode* root) {
    std::stringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html>\n<head>\n";
    html << "<title>AST Visualization</title>\n";
    html << "<style>\n";
    html << "  .node { margin-left: 20px; }\n";
    html << "  .type { font-weight: bold; color: #0066cc; }\n";
    html << "  .value { color: #009900; }\n";
    html << "  .position { color: #999; font-size: 0.8em; }\n";
    html << "</style>\n";
    html << "</head>\n<body>\n";
    html << "<h1>AST Visualization</h1>\n";
    
    // Generate tree view
    std::function<void(const BaseNode*, int)> generateNode = [&](const BaseNode* node, int depth) {
        if (!node) return;
        
        html << "<div class='node' style='margin-left: " << (depth * 20) << "px;'>\n";
        html << "<span class='type'>" << ASTPrinter().nodeTypeToString(node->getType()) << "</span>";
        
        if (!node->getValue().empty()) {
            html << ": <span class='value'>\"" << node->getValue() << "\"</span>";
        }
        
        html << " <span class='position'>[" << node->getLine() << ":" << node->getColumn() << "]</span>";
        html << "</div>\n";
        
        for (const auto& child : node->getChildren()) {
            generateNode(child.get(), depth + 1);
        }
    };
    
    generateNode(root, 0);
    
    html << "</body>\n</html>\n";
    
    return html.str();
}

} // namespace Test
} // namespace CHTL