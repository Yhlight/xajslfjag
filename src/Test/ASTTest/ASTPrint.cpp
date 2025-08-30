#include "ASTPrint.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <queue>

namespace CHTL::Test {

// Colors定义
const String ASTPrint::Colors::RESET = "\033[0m";
const String ASTPrint::Colors::NODE_TYPE = "\033[34m";
const String ASTPrint::Colors::ATTRIBUTE = "\033[36m";
const String ASTPrint::Colors::VALUE = "\033[32m";
const String ASTPrint::Colors::POSITION = "\033[33m";
const String ASTPrint::Colors::ERROR = "\033[31m";
const String ASTPrint::Colors::WARNING = "\033[35m";
const String ASTPrint::Colors::SUCCESS = "\033[32m";

void ASTPrint::printAST(const std::unique_ptr<CHTL::BaseNode>& root, std::ostream& os) {
    PrintOptions defaultOptions;
    printASTWithOptions(root, defaultOptions, os);
}

void ASTPrint::printASTWithOptions(const std::unique_ptr<CHTL::BaseNode>& root,
                                  const PrintOptions& options,
                                  std::ostream& os) {
    if (!root) {
        os << "AST is null" << std::endl;
        return;
    }
    
    printNodeRecursive(root.get(), "", true, options, os, 0);
}

String ASTPrint::astToString(const std::unique_ptr<CHTL::BaseNode>& root) {
    std::ostringstream oss;
    printAST(root, oss);
    return oss.str();
}

String ASTPrint::astToStringWithOptions(const std::unique_ptr<CHTL::BaseNode>& root,
                                       const PrintOptions& options) {
    std::ostringstream oss;
    printASTWithOptions(root, options, oss);
    return oss.str();
}

void ASTPrint::printASTAsTree(const std::unique_ptr<CHTL::BaseNode>& root, std::ostream& os) {
    PrintOptions options;
    options.useColors = false;
    options.compactFormat = false;
    printASTWithOptions(root, options, os);
}

void ASTPrint::printASTAsJSON(const std::unique_ptr<CHTL::BaseNode>& root, std::ostream& os) {
    if (!root) {
        os << "null" << std::endl;
        return;
    }
    
    os << "{\n";
    printJSONNode(root.get(), os, 0);
    os << "\n}" << std::endl;
}

void ASTPrint::printASTAsXML(const std::unique_ptr<CHTL::BaseNode>& root, std::ostream& os) {
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    
    if (!root) {
        os << "<ast>null</ast>" << std::endl;
        return;
    }
    
    os << "<ast>\n";
    printXMLNode(root.get(), os, 1);
    os << "</ast>" << std::endl;
}

void ASTPrint::printASTAsGraphviz(const std::unique_ptr<CHTL::BaseNode>& root, std::ostream& os) {
    os << "digraph AST {\n";
    os << "  node [shape=box];\n";
    
    if (root) {
        int nodeId = 0;
        printGraphvizNode(root.get(), os, nodeId);
    }
    
    os << "}\n";
}

void ASTPrint::printASTStatistics(const std::unique_ptr<CHTL::BaseNode>& root, std::ostream& os) {
    if (!root) {
        os << "AST is null" << std::endl;
        return;
    }
    
    auto stats = collectNodeStatistics(root.get());
    int depth = calculateTreeDepth(root.get());
    int totalNodes = countNodes(root.get());
    
    os << "AST Statistics:\n";
    os << "===============\n";
    os << "Total Nodes: " << totalNodes << "\n";
    os << "Tree Depth: " << depth << "\n";
    os << "Node Types:\n";
    
    for (const auto& [type, count] : stats) {
        os << "  " << std::setw(15) << getNodeTypeName(type) 
           << ": " << std::setw(4) << count 
           << " (" << std::fixed << std::setprecision(1) 
           << (double)count / totalNodes * 100.0 << "%)\n";
    }
}

String ASTPrint::getNodeTypeName(CHTL::NodeType type) {
    switch (type) {
        case CHTL::NodeType::ELEMENT: return "ELEMENT";
        case CHTL::NodeType::TEXT: return "TEXT";
        case CHTL::NodeType::COMMENT: return "COMMENT";
        case CHTL::NodeType::TEMPLATE: return "TEMPLATE";
        case CHTL::NodeType::CUSTOM: return "CUSTOM";
        case CHTL::NodeType::STYLE: return "STYLE";
        case CHTL::NodeType::SCRIPT: return "SCRIPT";
        case CHTL::NodeType::ORIGIN: return "ORIGIN";
        case CHTL::NodeType::IMPORT: return "IMPORT";
        case CHTL::NodeType::CONFIGURATION: return "CONFIG";
        case CHTL::NodeType::CONSTRAINT: return "CONSTRAINT";
        case CHTL::NodeType::DELETE_NODE: return "DELETE";
        case CHTL::NodeType::INSERT_NODE: return "INSERT";
        case CHTL::NodeType::INDEX_ACCESS_NODE: return "INDEX_ACCESS";
        case CHTL::NodeType::NO_VALUE_STYLE: return "NO_VALUE_STYLE";
        case CHTL::NodeType::INHERIT: return "INHERIT";
        default: return "UNKNOWN";
    }
}

String ASTPrint::getNodeDescription(const CHTL::BaseNode* node) {
    if (!node) {
        return "null";
    }
    
    String typeName = getNodeTypeName(node->getType());
    
    // 根据节点类型添加特定信息
    // 这里可以根据具体的节点类型来扩展
    
    return typeName;
}

String ASTPrint::formatNodeAttributes(const CHTL::BaseNode* node) {
    if (!node) {
        return "";
    }
    
    // 这里需要根据实际的BaseNode接口来实现
    // 暂时返回空字符串
    return "";
}

String ASTPrint::formatPosition(const CHTL::Position& pos) {
    std::ostringstream oss;
    oss << pos.line << ":" << pos.column;
    if (pos.offset >= 0) {
        oss << "@" << pos.offset;
    }
    return oss.str();
}

String ASTPrint::colorize(const String& text, const String& color, bool useColor) {
    if (!useColor) {
        return text;
    }
    return color + text + Colors::RESET;
}

void ASTPrint::printNodeRecursive(const CHTL::BaseNode* node,
                                 const String& prefix,
                                 bool isLast,
                                 const PrintOptions& options,
                                 std::ostream& os,
                                 int currentDepth) {
    if (!node) {
        return;
    }
    
    if (options.maxDepth >= 0 && currentDepth > options.maxDepth) {
        return;
    }
    
    // 打印当前节点
    String nodePrefix = isLast ? options.lastNodePrefix : options.nodePrefix;
    String nodeDescription = getNodeDescription(node);
    
    os << prefix << nodePrefix;
    
    if (options.showNodeTypes) {
        os << colorize(nodeDescription, Colors::NODE_TYPE, options.useColors);
    }
    
    if (options.showAttributes) {
        String attributes = formatNodeAttributes(node);
        if (!attributes.empty()) {
            os << " " << colorize(attributes, Colors::ATTRIBUTE, options.useColors);
        }
    }
    
    if (options.showPositions) {
        // 这里需要获取节点的位置信息
        // os << " " << colorize(formatPosition(node->getPosition()), Colors::POSITION, options.useColors);
    }
    
    if (options.showMemoryAddresses) {
        os << " " << colorize("[" + std::to_string(reinterpret_cast<uintptr_t>(node)) + "]", 
                             Colors::VALUE, options.useColors);
    }
    
    os << "\n";
    
    // 打印子节点
    // 这里需要根据实际的BaseNode接口来获取子节点
    // 暂时跳过子节点的打印
    
    String childPrefix = prefix + (isLast ? options.emptySpace : options.verticalLine);
    
    // 如果有子节点，递归打印
    // for (auto& child : node->getChildren()) {
    //     bool isLastChild = (&child == &node->getChildren().back());
    //     printNodeRecursive(child.get(), childPrefix, isLastChild, options, os, currentDepth + 1);
    // }
}

void ASTPrint::printJSONNode(const CHTL::BaseNode* node, std::ostream& os, int indentLevel) {
    if (!node) {
        os << "null";
        return;
    }
    
    String indent(indentLevel * 2, ' ');
    
    os << indent << "{\n";
    os << indent << "  \"type\": \"" << getNodeTypeName(node->getType()) << "\",\n";
    os << indent << "  \"description\": \"" << escapeJSON(getNodeDescription(node)) << "\"\n";
    
    // 如果有子节点，添加children数组
    // os << indent << "  \"children\": [\n";
    // ...子节点处理...
    // os << indent << "  ]\n";
    
    os << indent << "}";
}

void ASTPrint::printXMLNode(const CHTL::BaseNode* node, std::ostream& os, int indentLevel) {
    if (!node) {
        os << String(indentLevel * 2, ' ') << "<node>null</node>\n";
        return;
    }
    
    String indent(indentLevel * 2, ' ');
    String typeName = getNodeTypeName(node->getType());
    
    os << indent << "<node type=\"" << escapeXML(typeName) << "\">\n";
    os << indent << "  <description>" << escapeXML(getNodeDescription(node)) << "</description>\n";
    
    // 如果有子节点，递归处理
    // os << indent << "  <children>\n";
    // ...子节点处理...
    // os << indent << "  </children>\n";
    
    os << indent << "</node>\n";
}

void ASTPrint::printGraphvizNode(const CHTL::BaseNode* node, std::ostream& os, int& nodeId) {
    if (!node) {
        return;
    }
    
    int currentId = nodeId++;
    String description = escapeGraphviz(getNodeDescription(node));
    
    os << "  node" << currentId << " [label=\"" << description << "\"];\n";
    
    // 如果有子节点，创建边连接
    // for (auto& child : node->getChildren()) {
    //     int childId = nodeId;
    //     printGraphvizNode(child.get(), os, nodeId);
    //     os << "  node" << currentId << " -> node" << childId << ";\n";
    // }
}

String ASTPrint::escapeJSON(const String& text) {
    String result = text;
    size_t pos = 0;
    
    while ((pos = result.find('\\', pos)) != String::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find('"', pos)) != String::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    return result;
}

String ASTPrint::escapeXML(const String& text) {
    String result = text;
    size_t pos = 0;
    
    while ((pos = result.find('&', pos)) != String::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find('<', pos)) != String::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('>', pos)) != String::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    return result;
}

String ASTPrint::escapeGraphviz(const String& text) {
    String result = text;
    size_t pos = 0;
    
    while ((pos = result.find('"', pos)) != String::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    return result;
}

std::map<CHTL::NodeType, int> ASTPrint::collectNodeStatistics(const CHTL::BaseNode* node) {
    std::map<CHTL::NodeType, int> stats;
    
    if (!node) {
        return stats;
    }
    
    std::queue<const CHTL::BaseNode*> queue;
    queue.push(node);
    
    while (!queue.empty()) {
        const CHTL::BaseNode* current = queue.front();
        queue.pop();
        
        stats[current->getType()]++;
        
        // 如果有子节点，添加到队列
        // for (auto& child : current->getChildren()) {
        //     queue.push(child.get());
        // }
    }
    
    return stats;
}

int ASTPrint::calculateTreeDepth(const CHTL::BaseNode* node) {
    if (!node) {
        return 0;
    }
    
    int maxDepth = 0;
    
    // 如果有子节点，递归计算深度
    // for (auto& child : node->getChildren()) {
    //     int childDepth = calculateTreeDepth(child.get());
    //     maxDepth = std::max(maxDepth, childDepth);
    // }
    
    return maxDepth + 1;
}

int ASTPrint::countNodes(const CHTL::BaseNode* node) {
    if (!node) {
        return 0;
    }
    
    int count = 1; // 计算当前节点
    
    // 如果有子节点，递归计算
    // for (auto& child : node->getChildren()) {
    //     count += countNodes(child.get());
    // }
    
    return count;
}

// ASTComparator基础实现
std::vector<ASTComparator::Difference> ASTComparator::compareASTs(const std::unique_ptr<CHTL::BaseNode>& expected,
                                                                  const std::unique_ptr<CHTL::BaseNode>& actual) {
    std::vector<Difference> differences;
    
    if (!expected && !actual) {
        return differences; // 两者都为空，相等
    }
    
    if (!expected || !actual) {
        Difference diff(Difference::STRUCTURE_MISMATCH, "root", 
                       expected ? "Expected non-null, got null" : "Expected null, got non-null");
        differences.push_back(diff);
        return differences;
    }
    
    compareNodesRecursive(expected.get(), actual.get(), "root", differences);
    
    return differences;
}

bool ASTComparator::areASTsEqual(const std::unique_ptr<CHTL::BaseNode>& ast1,
                                const std::unique_ptr<CHTL::BaseNode>& ast2,
                                bool ignorePositions) {
    auto differences = compareASTs(ast1, ast2);
    return differences.empty();
}

void ASTComparator::compareNodesRecursive(const CHTL::BaseNode* expected,
                                         const CHTL::BaseNode* actual,
                                         const String& path,
                                         std::vector<Difference>& differences) {
    if (!expected || !actual) {
        return;
    }
    
    // 比较节点类型
    if (expected->getType() != actual->getType()) {
        Difference diff(Difference::NODE_TYPE_MISMATCH, path, "Node type mismatch");
        diff.expectedValue = ASTPrint::getNodeTypeName(expected->getType());
        diff.actualValue = ASTPrint::getNodeTypeName(actual->getType());
        differences.push_back(diff);
    }
    
    // 比较节点属性
    if (!compareNodeAttributes(expected, actual)) {
        Difference diff(Difference::ATTRIBUTE_MISMATCH, path, "Node attributes mismatch");
        differences.push_back(diff);
    }
    
    // 递归比较子节点
    // 这里需要根据实际的BaseNode接口来实现
    // compareChildNodes(expected, actual, path, differences);
}

bool ASTComparator::compareNodeAttributes(const CHTL::BaseNode* node1,
                                         const CHTL::BaseNode* node2) {
    // 这里需要根据实际的BaseNode接口来比较属性
    // 暂时返回true
    return true;
}

void ASTComparator::printDifferences(const std::vector<Difference>& differences, std::ostream& os) {
    os << "AST Differences Found: " << differences.size() << "\n";
    os << "==============================\n";
    
    for (const auto& diff : differences) {
        os << "[" << diff.path << "] " << diff.description << "\n";
        if (!diff.expectedValue.empty() || !diff.actualValue.empty()) {
            os << "  Expected: " << diff.expectedValue << "\n";
            os << "  Actual:   " << diff.actualValue << "\n";
        }
    }
}

String ASTComparator::differencesToString(const std::vector<Difference>& differences) {
    std::ostringstream oss;
    printDifferences(differences, oss);
    return oss.str();
}

} // namespace CHTL::Test