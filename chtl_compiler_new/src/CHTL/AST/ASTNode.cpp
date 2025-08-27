#include "ASTNode.h"
#include <sstream>

namespace CHTL {
namespace AST {

const char* nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::ROOT: return "ROOT";
        case NodeType::ELEMENT: return "ELEMENT";
        case NodeType::TEXT: return "TEXT";
        case NodeType::COMMENT: return "COMMENT";
        case NodeType::ATTRIBUTE: return "ATTRIBUTE";
        case NodeType::ATTRIBUTE_LIST: return "ATTRIBUTE_LIST";
        case NodeType::STYLE_BLOCK: return "STYLE_BLOCK";
        case NodeType::STYLE_RULE: return "STYLE_RULE";
        case NodeType::STYLE_PROPERTY: return "STYLE_PROPERTY";
        case NodeType::CSS_SELECTOR: return "CSS_SELECTOR";
        case NodeType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case NodeType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case NodeType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case NodeType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case NodeType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
        case NodeType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case NodeType::CUSTOM_VAR: return "CUSTOM_VAR";
        case NodeType::IMPORT_HTML: return "IMPORT_HTML";
        case NodeType::IMPORT_STYLE: return "IMPORT_STYLE";
        case NodeType::IMPORT_JAVASCRIPT: return "IMPORT_JAVASCRIPT";
        case NodeType::IMPORT_CHTL: return "IMPORT_CHTL";
        case NodeType::IMPORT_CJMOD: return "IMPORT_CJMOD";
        case NodeType::IMPORT_SPECIFIC: return "IMPORT_SPECIFIC";
        case NodeType::NAMESPACE: return "NAMESPACE";
        case NodeType::CONFIGURATION: return "CONFIGURATION";
        case NodeType::CONFIG_OPTION: return "CONFIG_OPTION";
        case NodeType::ORIGIN_BLOCK: return "ORIGIN_BLOCK";
        case NodeType::VAR_GROUP: return "VAR_GROUP";
        case NodeType::VAR_DECLARATION: return "VAR_DECLARATION";
        case NodeType::VAR_REFERENCE: return "VAR_REFERENCE";
        case NodeType::SPECIALIZATION: return "SPECIALIZATION";
        case NodeType::INHERITANCE: return "INHERITANCE";
        case NodeType::LITERAL: return "LITERAL";
        case NodeType::NODE_LIST: return "NODE_LIST";
        default: return "UNKNOWN";
    }
}

std::string ASTNode::toString() const {
    std::ostringstream oss;
    oss << nodeTypeToString(type_);
    
    // 添加主要属性信息
    if (hasAttribute("name")) {
        oss << "(" << getAttribute("name") << ")";
    } else if (hasAttribute("value")) {
        oss << "(\"" << getAttribute("value") << "\")";
    }
    
    return oss.str();
}

std::string ASTNode::toTreeString(int indent) const {
    std::string result;
    
    // 添加缩进
    for (int i = 0; i < indent; ++i) {
        result += "  ";
    }
    
    // 添加节点信息
    result += toString();
    
    // 添加位置信息
    if (range_.start.line > 0) {
        result += " [" + std::to_string(range_.start.line) + ":" + 
                 std::to_string(range_.start.column) + "]";
    }
    
    result += "\n";
    
    // 递归添加子节点
    for (const auto& child : children_) {
        result += child->toTreeString(indent + 1);
    }
    
    return result;
}

void ASTVisitor::visit(ASTNode* node) {
    if (!node) return;
    
    switch (node->getType()) {
        case NodeType::ROOT: visitRoot(node); break;
        case NodeType::ELEMENT: visitElement(node); break;
        case NodeType::TEXT: visitText(node); break;
        case NodeType::COMMENT: visitComment(node); break;
        case NodeType::ATTRIBUTE: visitAttribute(node); break;
        case NodeType::ATTRIBUTE_LIST: visitAttributeList(node); break;
        case NodeType::STYLE_BLOCK: visitStyleBlock(node); break;
        case NodeType::STYLE_RULE: visitStyleRule(node); break;
        case NodeType::STYLE_PROPERTY: visitStyleProperty(node); break;
        case NodeType::CSS_SELECTOR: visitCSSSelector(node); break;
        case NodeType::SCRIPT_BLOCK: visitScriptBlock(node); break;
        case NodeType::TEMPLATE_ELEMENT: visitTemplateElement(node); break;
        case NodeType::TEMPLATE_STYLE: visitTemplateStyle(node); break;
        case NodeType::TEMPLATE_VAR: visitTemplateVar(node); break;
        case NodeType::CUSTOM_ELEMENT: visitCustomElement(node); break;
        case NodeType::CUSTOM_STYLE: visitCustomStyle(node); break;
        case NodeType::CUSTOM_VAR: visitCustomVar(node); break;
        case NodeType::IMPORT_HTML: visitImportHTML(node); break;
        case NodeType::IMPORT_STYLE: visitImportStyle(node); break;
        case NodeType::IMPORT_JAVASCRIPT: visitImportJavaScript(node); break;
        case NodeType::IMPORT_CHTL: visitImportCHTL(node); break;
        case NodeType::IMPORT_CJMOD: visitImportCJMOD(node); break;
        case NodeType::IMPORT_SPECIFIC: visitImportSpecific(node); break;
        case NodeType::NAMESPACE: visitNamespace(node); break;
        case NodeType::CONFIGURATION: visitConfiguration(node); break;
        case NodeType::ORIGIN_BLOCK: visitOriginBlock(node); break;
        case NodeType::VAR_GROUP: visitVarGroup(node); break;
        case NodeType::LITERAL: visitLiteral(node); break;
        case NodeType::NODE_LIST: visitNodeList(node); break;
        default: break;
    }
    
    // 访问子节点
    for (auto& child : node->getChildren()) {
        child->accept(*this);
    }
}

} // namespace AST
} // namespace CHTL