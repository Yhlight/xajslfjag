#include "ConstraintSystem.h"
#include "../CHTLNode/BaseNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// 辅助函数：将NodeType转换为字符串
std::string nodeTypeToString(NodeType type) {
    switch(type) {
        case NodeType::PROGRAM: return "PROGRAM";
        case NodeType::ELEMENT: return "ELEMENT";
        case NodeType::TEXT: return "TEXT";
        case NodeType::COMMENT: return "COMMENT";
        case NodeType::TEMPLATE: return "TEMPLATE";
        case NodeType::CUSTOM: return "CUSTOM";
        case NodeType::STYLE_BLOCK: return "STYLE_BLOCK";
        case NodeType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case NodeType::ORIGIN: return "ORIGIN";
        case NodeType::IMPORT: return "IMPORT";
        case NodeType::CONFIGURATION: return "CONFIGURATION";
        case NodeType::NAMESPACE: return "NAMESPACE";
        case NodeType::DELETE_OP: return "DELETE_OP";
        case NodeType::INSERT_OP: return "INSERT_OP";
        case NodeType::INHERIT_OP: return "INHERIT_OP";
        case NodeType::EXCEPT_OP: return "EXCEPT_OP";
        case NodeType::USE_OP: return "USE_OP";
        case NodeType::ATTRIBUTE: return "ATTRIBUTE";
        case NodeType::PROPERTY: return "PROPERTY";
        case NodeType::SELECTOR: return "SELECTOR";
        default: return "UNKNOWN";
    }
}

// SyntaxBoundaryConstraint Implementation

ConstraintResult SyntaxBoundaryConstraint::check(const void* context) const {
    if (!enabled_) {
        return {true, level_, "", 0, 0, ""};
    }
    
    // Cast context to BaseNode
    const ASTNode* node = static_cast<const ASTNode*>(context);
    if (!node) {
        return {false, level_, "Invalid context for syntax boundary check", 0, 0, ""};
    }
    
    ConstraintResult result;
    result.level = level_;
    
    // Check parent constraints
    // TODO: 实现getParent()方法后启用
    /*
    if (!allowedParents_.empty()) {
        const ASTNode* parent = node->getParent();
        if (parent) {
            std::string parentType = nodeTypeToString(parent->getType());
            if (allowedParents_.find(parentType) == allowedParents_.end()) {
                result.satisfied = false;
                result.message = "Node type '" + nodeTypeToString(node->getType()) + 
                               "' is not allowed under parent type '" + parentType + "'";
                return result;
            }
        }
    }
    */
    
    // Check children constraints
    if (!allowedChildren_.empty()) {
        for (const auto& child : node->getChildren()) {
            std::string childType = nodeTypeToString(child->getType());
            if (allowedChildren_.find(childType) == allowedChildren_.end()) {
                result.satisfied = false;
                result.message = "Child type '" + childType + 
                               "' is not allowed under node type '" + 
                               nodeTypeToString(node->getType()) + "'";
                return result;
            }
        }
    }
    
    result.satisfied = true;
    return result;
}

// ExceptConstraint Implementation

ConstraintResult ExceptConstraint::check(const void* context) const {
    if (!enabled_) {
        return {true, level_, "", 0, 0, ""};
    }
    
    const ASTNode* node = static_cast<const ASTNode*>(context);
    if (!node) {
        return {false, level_, "Invalid context for except constraint check", 0, 0, ""};
    }
    
    ConstraintResult result;
    result.level = level_;
    
    // Check excluded types
    std::string nodeType = nodeTypeToString(node->getType());
    if (excludedTypes_.find(nodeType) != excludedTypes_.end()) {
        result.satisfied = false;
        result.message = "Node type '" + nodeType + "' is excluded by except constraint";
        return result;
    }
    
    // Check excluded attributes
    // TODO: 实现hasAttribute()方法后启用
    /*
    for (const auto& attr : excludedAttributes_) {
        if (node->hasAttribute(attr)) {
            result.satisfied = false;
            result.message = "Attribute '" + attr + "' is excluded by except constraint";
            return result;
        }
    }
    */
    
    // Check excluded values
    // TODO: 实现getValue()方法后启用
    /*
    std::string nodeValue = node->getValue();
    if (excludedValues_.find(nodeValue) != excludedValues_.end()) {
        result.satisfied = false;
        result.message = "Value '" + nodeValue + "' is excluded by except constraint";
        return result;
    }
    */
    
    result.satisfied = true;
    return result;
}

// ConstraintManager Implementation

void ConstraintManager::registerConstraint(std::shared_ptr<Constraint> constraint) {
    if (constraint) {
        constraints_[constraint->getName()] = constraint;
    }
}

std::shared_ptr<Constraint> ConstraintManager::getConstraint(const std::string& name) const {
    auto it = constraints_.find(name);
    if (it != constraints_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<ConstraintResult> ConstraintManager::checkAll(const void* context) const {
    std::vector<ConstraintResult> results;
    
    for (const auto& [name, constraint] : constraints_) {
        if (constraint->isEnabled()) {
            results.push_back(constraint->check(context));
        }
    }
    
    return results;
}

std::vector<ConstraintResult> ConstraintManager::checkByType(ConstraintType type, const void* context) const {
    std::vector<ConstraintResult> results;
    
    for (const auto& [name, constraint] : constraints_) {
        if (constraint->getType() == type && constraint->isEnabled()) {
            results.push_back(constraint->check(context));
        }
    }
    
    return results;
}

void ConstraintManager::enableConstraint(const std::string& name, bool enable) {
    auto constraint = getConstraint(name);
    if (constraint) {
        constraint->setEnabled(enable);
    }
}

void ConstraintManager::clearAll() {
    constraints_.clear();
}

void ConstraintManager::loadPredefinedConstraints() {
    createCHTLConstraints();
    createCHTLJSConstraints();
    createImportConstraints();
    createOriginConstraints();
}

void ConstraintManager::createCHTLConstraints() {
    // Template constraints
    auto templateConstraint = std::make_shared<SyntaxBoundaryConstraint>("template_boundary");
    templateConstraint->setAllowedParents({"program", "namespace"});
    templateConstraint->setAllowedChildren({"style", "element", "var"});
    registerConstraint(templateConstraint);
    
    // Custom element constraints
    auto customConstraint = std::make_shared<SyntaxBoundaryConstraint>("custom_boundary");
    customConstraint->setAllowedParents({"program", "namespace"});
    customConstraint->setAllowedChildren({"style", "element", "var"});
    registerConstraint(customConstraint);
    
    // Local style block constraints
    auto styleConstraint = std::make_shared<SyntaxBoundaryConstraint>("local_style_boundary");
    styleConstraint->setAllowedParents({"element"});
    registerConstraint(styleConstraint);
    
    // Local script block constraints
    auto scriptConstraint = std::make_shared<SyntaxBoundaryConstraint>("local_script_boundary");
    scriptConstraint->setAllowedParents({"element"});
    registerConstraint(scriptConstraint);
    
    // Configuration constraints
    auto configConstraint = std::make_shared<SyntaxBoundaryConstraint>("config_boundary");
    configConstraint->setAllowedParents({"program"});
    registerConstraint(configConstraint);
    
    // Namespace constraints
    auto namespaceConstraint = std::make_shared<SyntaxBoundaryConstraint>("namespace_boundary");
    namespaceConstraint->setAllowedParents({"program", "namespace"});
    registerConstraint(namespaceConstraint);
}

void ConstraintManager::createCHTLJSConstraints() {
    // Module import constraints
    auto moduleConstraint = std::make_shared<SyntaxBoundaryConstraint>("module_import_boundary");
    moduleConstraint->setAllowedParents({"program", "script"});
    registerConstraint(moduleConstraint);
    
    // Enhanced selector constraints
    auto selectorConstraint = std::make_shared<SyntaxBoundaryConstraint>("enhanced_selector_boundary");
    selectorConstraint->setAllowedParents({"script", "function"});
    registerConstraint(selectorConstraint);
    
    // Virtual object constraints
    auto virConstraint = std::make_shared<SyntaxBoundaryConstraint>("virtual_object_boundary");
    virConstraint->setAllowedParents({"function"});
    registerConstraint(virConstraint);
    
    // Event delegation constraints
    auto delegateConstraint = std::make_shared<SyntaxBoundaryConstraint>("event_delegation_boundary");
    delegateConstraint->setAllowedParents({"script", "function"});
    registerConstraint(delegateConstraint);
}

void ConstraintManager::createImportConstraints() {
    // Import must be at top level or in namespace
    auto importConstraint = std::make_shared<SyntaxBoundaryConstraint>("import_boundary");
    importConstraint->setAllowedParents({"program", "namespace"});
    registerConstraint(importConstraint);
    
    // Import order constraint
    auto importOrderConstraint = std::make_shared<SyntaxBoundaryConstraint>("import_order");
    importOrderConstraint->setAllowedPredecessors({"import", "use", "configuration"});
    registerConstraint(importOrderConstraint);
}

void ConstraintManager::createOriginConstraints() {
    // Origin can be used anywhere (as per documentation)
    auto originConstraint = std::make_shared<SyntaxBoundaryConstraint>("origin_boundary", ConstraintLevel::INFO);
    // No restrictions on parents or children
    registerConstraint(originConstraint);
}

// CHTL Constraints namespace implementation

namespace CHTLConstraints {

std::shared_ptr<Constraint> createTemplateConstraint() {
    return ConstraintBuilder("template_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("program")
        .allowedParent("namespace")
        .allowedChild("style")
        .allowedChild("element")
        .allowedChild("var")
        .build();
}

std::shared_ptr<Constraint> createCustomElementConstraint() {
    return ConstraintBuilder("custom_element_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("program")
        .allowedParent("namespace")
        .allowedChild("style")
        .allowedChild("element")
        .allowedChild("var")
        .except("template")
        .build();
}

std::shared_ptr<Constraint> createLocalStyleConstraint() {
    return ConstraintBuilder("local_style_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("element")
        .except("template")
        .except("custom")
        .build();
}

std::shared_ptr<Constraint> createLocalScriptConstraint() {
    return ConstraintBuilder("local_script_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("element")
        .except("template")
        .except("custom")
        .build();
}

std::shared_ptr<Constraint> createConfigurationConstraint() {
    return ConstraintBuilder("configuration_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("program")
        .allowedBefore("import")
        .allowedBefore("namespace")
        .build();
}

std::shared_ptr<Constraint> createNamespaceConstraint() {
    return ConstraintBuilder("namespace_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("program")
        .allowedParent("namespace")
        .allowedAfter("import")
        .allowedAfter("configuration")
        .build();
}

std::shared_ptr<Constraint> createImportConstraint() {
    return ConstraintBuilder("import_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("program")
        .allowedParent("namespace")
        .allowedAfter("configuration")
        .allowedBefore("template")
        .allowedBefore("custom")
        .allowedBefore("element")
        .build();
}

std::shared_ptr<Constraint> createOriginConstraint() {
    // Origin可以在任何地方使用
    return ConstraintBuilder("origin_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .withLevel(ConstraintLevel::INFO)
        .build();
}

} // namespace CHTLConstraints

// CHTL JS Constraints namespace implementation

namespace CHTLJSConstraints {

std::shared_ptr<Constraint> createModuleImportConstraint() {
    return ConstraintBuilder("module_import_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("program")
        .allowedParent("script")
        .allowedBefore("function")
        .allowedBefore("class")
        .build();
}

std::shared_ptr<Constraint> createEnhancedSelectorConstraint() {
    return ConstraintBuilder("enhanced_selector_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("script")
        .allowedParent("function")
        .allowedParent("listen")
        .allowedParent("delegate")
        .build();
}

std::shared_ptr<Constraint> createVirtualObjectConstraint() {
    return ConstraintBuilder("virtual_object_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("function")
        .allowedParent("method")
        .except("arrow_function")
        .build();
}

std::shared_ptr<Constraint> createEventDelegationConstraint() {
    return ConstraintBuilder("event_delegation_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("script")
        .allowedParent("function")
        .allowedChild("selector")
        .allowedChild("event_handler")
        .build();
}

std::shared_ptr<Constraint> createAnimationConstraint() {
    return ConstraintBuilder("animation_syntax")
        .ofType(ConstraintType::SYNTAX_BOUNDARY)
        .allowedParent("script")
        .allowedParent("function")
        .allowedChild("selector")
        .allowedChild("properties")
        .allowedChild("options")
        .build();
}

} // namespace CHTLJSConstraints

// ConstraintChecker Implementation

ConstraintChecker::ConstraintChecker() {}

std::vector<ConstraintResult> ConstraintChecker::checkNode(const void* node) {
    std::vector<ConstraintResult> results;
    
    // Get constraints from manager
    auto& manager = ConstraintManager::getInstance();
    auto managerResults = manager.checkAll(node);
    results.insert(results.end(), managerResults.begin(), managerResults.end());
    
    // Apply custom checker if set
    if (customChecker_) {
        auto customResults = customChecker_(node);
        results.insert(results.end(), customResults.begin(), customResults.end());
    }
    
    // Update counters
    updateCounters(results);
    
    return results;
}

std::vector<ConstraintResult> ConstraintChecker::checkAST(const void* ast) {
    std::vector<ConstraintResult> allResults;
    
    // Cast to BaseNode
    const ASTNode* rootNode = static_cast<const ASTNode*>(ast);
    if (!rootNode) {
        return allResults;
    }
    
    // Check root node
    auto rootResults = checkNode(rootNode);
    allResults.insert(allResults.end(), rootResults.begin(), rootResults.end());
    
    // Recursively check all children
    std::function<void(const ASTNode*)> checkChildren = [&](const ASTNode* currentNode) {
        for (const auto& child : currentNode->getChildren()) {
            auto childResults = checkNode(child.get());
            allResults.insert(allResults.end(), childResults.begin(), childResults.end());
            checkChildren(child.get());
        }
    };
    
    checkChildren(rootNode);
    
    return allResults;
}

void ConstraintChecker::updateCounters(const std::vector<ConstraintResult>& results) {
    for (const auto& result : results) {
        if (!result.satisfied) {
            switch (result.level) {
                case ConstraintLevel::ERROR:
                    errorCount_++;
                    break;
                case ConstraintLevel::WARNING:
                    warningCount_++;
                    break;
                default:
                    break;
            }
        }
    }
}

// ConstraintBuilder Implementation

std::shared_ptr<Constraint> ConstraintBuilder::build() {
    std::shared_ptr<Constraint> constraint;
    
    switch (type_) {
        case ConstraintType::SYNTAX_BOUNDARY: {
            auto boundaryConstraint = std::make_shared<SyntaxBoundaryConstraint>(name_, level_);
            boundaryConstraint->setAllowedPredecessors(allowedBefore_);
            boundaryConstraint->setAllowedSuccessors(allowedAfter_);
            boundaryConstraint->setAllowedParents(allowedParents_);
            boundaryConstraint->setAllowedChildren(allowedChildren_);
            constraint = boundaryConstraint;
            break;
        }
        case ConstraintType::NODE_EXCLUSION: {
            auto exceptConstraint = std::make_shared<ExceptConstraint>(name_);
            for (const auto& type : exceptTypes_) {
                exceptConstraint->addExcludedType(type);
            }
            for (const auto& attr : exceptAttributes_) {
                exceptConstraint->addExcludedAttribute(attr);
            }
            constraint = exceptConstraint;
            break;
        }
        default:
            // Create a default syntax boundary constraint
            constraint = std::make_shared<SyntaxBoundaryConstraint>(name_, level_);
            break;
    }
    
    return constraint;
}

} // namespace CHTL