#include "ScriptNode.h"
#include "NodeVisitor.h"

namespace CHTL {

// ScriptBlockNode 的简化实现
ScriptBlockNode::ScriptBlockNode(ScriptType type, const NodePosition& pos)
    : BaseNode(NodeType::SCRIPT_BLOCK, pos), scriptType(type) {
}

std::string ScriptBlockNode::GenerateJavaScript() const {
    return rawContent;
}

std::shared_ptr<BaseNode> ScriptBlockNode::Clone() const {
    auto cloned = std::make_shared<ScriptBlockNode>(scriptType, position);
    cloned->rawContent = rawContent;
    cloned->autoAddedClasses = autoAddedClasses;
    cloned->autoAddedIds = autoAddedIds;
    cloned->metadata = metadata;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->AddChild(child->Clone());
    }
    
    return cloned;
}

bool ScriptBlockNode::Validate() const {
    return BaseNode::Validate();
}

std::string ScriptBlockNode::GetDescription() const {
    return "ScriptBlock(" + ScriptTypeToString(scriptType) + 
           ")[" + std::to_string(rawContent.length()) + " chars] at (" +
           std::to_string(position.line) + ":" + std::to_string(position.column) + ")";
}

void ScriptBlockNode::Accept(NodeVisitor& visitor) {
    visitor.VisitScriptBlock(*this);
}

void ScriptBlockNode::AddAutoAddedClass(const std::string& className) {
    autoAddedClasses.insert(className);
}

void ScriptBlockNode::AddAutoAddedId(const std::string& id) {
    autoAddedIds.insert(id);
}

// 辅助函数
std::string ScriptTypeToString(ScriptType type) {
    switch (type) {
        case ScriptType::JAVASCRIPT: return "JAVASCRIPT";
        case ScriptType::CHTL_JS: return "CHTL_JS";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL