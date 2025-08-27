#include "ImportNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

std::string ImportNode::getResolvedPath() const {
    std::string resolved = fromPath;
    // 将'.'替换为'/'
    std::replace(resolved.begin(), resolved.end(), '.', '/');
    return resolved;
}

void ImportNode::accept(NodeVisitor* visitor) {
    if (auto* importVisitor = dynamic_cast<ImportNodeVisitor*>(visitor)) {
        importVisitor->visitImportNode(this);
    }
}

std::shared_ptr<BaseNode> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(category, importType, line, column);
    
    // 复制所有属性
    cloned->importName = importName;
    cloned->fromPath = fromPath;
    cloned->asName = asName;
    cloned->importAll = importAll;
    cloned->prefix = prefix;
    
    // 递归克隆子节点（虽然导入节点通常没有子节点）
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string ImportNode::toString() const {
    std::stringstream ss;
    
    ss << "[Import] ";
    
    // 添加前缀
    if (!prefix.empty()) {
        ss << prefix << " ";
    }
    
    // 添加类型
    ss << importType;
    
    // 添加名称
    if (!importName.empty()) {
        ss << " " << importName;
    }
    
    // 添加路径
    ss << " from " << fromPath;
    
    // 添加重命名
    if (!asName.empty()) {
        ss << " as " << asName;
    }
    
    ss << ";";
    
    return ss.str();
}

} // namespace CHTL