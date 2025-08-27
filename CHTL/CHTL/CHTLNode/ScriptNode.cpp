#include "ScriptNode.h"
#include <sstream>

namespace CHTL {

void ScriptNode::addSelector(const ScriptSelector& sel) {
    selectors.push_back(sel);
    
    // 根据选择器类型添加到自动添加集合
    if (sel.type == ScriptSelectorType::CLASS) {
        std::string className = sel.selector;
        if (!className.empty() && className[0] == '.') {
            className = className.substr(1);
        }
        autoAddClasses.insert(className);
    } else if (sel.type == ScriptSelectorType::ID) {
        std::string idName = sel.selector;
        if (!idName.empty() && idName[0] == '#') {
            idName = idName.substr(1);
        }
        autoAddIds.insert(idName);
    }
}

void ScriptNode::addAutoAddClass(const std::string& className) {
    autoAddClasses.insert(className);
}

void ScriptNode::addAutoAddId(const std::string& idName) {
    autoAddIds.insert(idName);
}

std::string ScriptNode::getFirstAutoAddClass() const {
    if (!autoAddClasses.empty()) {
        return *autoAddClasses.begin();
    }
    return "";
}

std::string ScriptNode::getFirstAutoAddId() const {
    if (!autoAddIds.empty()) {
        return *autoAddIds.begin();
    }
    return "";
}

void ScriptNode::accept(NodeVisitor* visitor) {
    if (auto* scriptVisitor = dynamic_cast<ScriptNodeVisitor*>(visitor)) {
        scriptVisitor->visitScriptNode(this);
    }
}

std::shared_ptr<BaseNode> ScriptNode::clone() const {
    auto cloned = std::make_shared<ScriptNode>(isGlobal, line, column);
    
    // 复制所有属性
    cloned->content = content;
    cloned->selectors = selectors;
    cloned->autoAddClasses = autoAddClasses;
    cloned->autoAddIds = autoAddIds;
    
    // 递归克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string ScriptNode::toString() const {
    std::stringstream ss;
    
    if (isGlobal) {
        ss << "<script>\n";
        ss << content;
        ss << "\n</script>";
    } else {
        ss << "script {\n";
        ss << content;
        ss << "\n}";
    }
    
    return ss.str();
}

} // namespace CHTL