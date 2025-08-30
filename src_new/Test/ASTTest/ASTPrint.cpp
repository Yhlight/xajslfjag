#include "ASTPrint.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace CHTL {
namespace Test {

void ASTPrint::printCHTLAST(const std::shared_ptr<CHTL::BaseNode>& root) {
    std::cout << "\n=== CHTL AST树结构 ===\n";
    if (root) {
        printNodeRecursive(root, 0);
    } else {
        std::cout << "AST根节点为空\n";
    }
}

void ASTPrint::printCHTLJSAST(const std::shared_ptr<CHTLJS::BaseNode>& root) {
    std::cout << "\n=== CHTL JS AST树结构 ===\n";
    if (root) {
        printNodeRecursive(root, 0);
    } else {
        std::cout << "AST根节点为空\n";
    }
}

std::string ASTPrint::generateASTText(const std::shared_ptr<CHTL::BaseNode>& root) {
    std::ostringstream oss;
    
    oss << "CHTL AST树结构\n";
    oss << "===============\n";
    
    if (root) {
        generateASTTextRecursive(root, oss, 0);
    } else {
        oss << "AST根节点为空\n";
    }
    
    return oss.str();
}

std::string ASTPrint::generateASTText(const std::shared_ptr<CHTLJS::BaseNode>& root) {
    std::ostringstream oss;
    
    oss << "CHTL JS AST树结构\n";
    oss << "==================\n";
    
    if (root) {
        generateASTTextRecursive(root, oss, 0);
    } else {
        oss << "AST根节点为空\n";
    }
    
    return oss.str();
}

bool ASTPrint::exportASTToFile(const std::shared_ptr<CHTL::BaseNode>& root, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateASTText(root);
    return true;
}

bool ASTPrint::exportASTToFile(const std::shared_ptr<CHTLJS::BaseNode>& root, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateASTText(root);
    return true;
}

void ASTPrint::printNodeRecursive(const std::shared_ptr<CHTL::BaseNode>& node, size_t depth) {
    if (!node) return;
    
    std::cout << getIndentation(depth) << "├─ " << node->toString() << "\n";
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            printNodeRecursive(child, depth + 1);
        }
    }
}

void ASTPrint::printNodeRecursive(const std::shared_ptr<CHTLJS::BaseNode>& node, size_t depth) {
    if (!node) return;
    
    std::cout << getIndentation(depth) << "├─ " << node->toString() << "\n";
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            printNodeRecursive(child, depth + 1);
        }
    }
}

void ASTPrint::generateASTTextRecursive(const std::shared_ptr<CHTL::BaseNode>& node, 
                                       std::ostringstream& oss, size_t depth) {
    if (!node) return;
    
    oss << getIndentation(depth) << "├─ " << node->toString() << "\n";
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            generateASTTextRecursive(child, oss, depth + 1);
        }
    }
}

void ASTPrint::generateASTTextRecursive(const std::shared_ptr<CHTLJS::BaseNode>& node, 
                                       std::ostringstream& oss, size_t depth) {
    if (!node) return;
    
    oss << getIndentation(depth) << "├─ " << node->toString() << "\n";
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            generateASTTextRecursive(child, oss, depth + 1);
        }
    }
}

std::string ASTPrint::getIndentation(size_t depth) {
    std::string indent = "";
    for (size_t i = 0; i < depth; ++i) {
        indent += "  ";
    }
    return indent;
}

} // namespace Test
} // namespace CHTL