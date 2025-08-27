#include "ImportNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

ImportNode::ImportNode()
    : BaseNode(NodeType::IMPORT), targetType(ImportTargetType::CHTL) {
}

std::string ImportNode::Generate() const {
    // Import节点不生成任何输出
    // 导入的内容会在解析阶段被处理并合并到AST中
    return "";
}

NodePtr ImportNode::Clone() const {
    auto clone = std::make_shared<ImportNode>();
    clone->SetPosition(line, column);
    clone->targetType = targetType;
    clone->itemType = itemType;
    clone->itemName = itemName;
    clone->fromPath = fromPath;
    clone->asName = asName;
    return clone;
}

void ImportNode::Accept(NodeVisitor* visitor) {
    visitor->VisitImportNode(this);
}

bool ImportNode::IsValid() const {
    // 检查必要的字段
    if (fromPath.empty()) {
        return false;
    }
    
    // 某些导入类型需要as名称
    if (RequiresAsName() && asName.empty()) {
        return false;
    }
    
    return true;
}

bool ImportNode::RequiresAsName() const {
    // @Html, @Style, @JavaScript 类型的导入必须有as名称
    return targetType == ImportTargetType::HTML ||
           targetType == ImportTargetType::STYLE ||
           targetType == ImportTargetType::JAVASCRIPT;
}

std::string ImportNode::GetNormalizedPath() const {
    std::string path = fromPath;
    
    // 将 . 替换为 /
    std::replace(path.begin(), path.end(), '.', '/');
    
    return path;
}

std::vector<std::string> ImportNode::GetSearchPaths() const {
    std::vector<std::string> paths;
    
    // 根据导入类型确定搜索路径
    // TODO: 实现具体的搜索路径逻辑
    
    return paths;
}

bool ImportNode::IsWildcardImport() const {
    // 检查是否是通配符导入
    if (fromPath.find("*") != std::string::npos) {
        return true;
    }
    
    // 没有指定具体项名也是导入所有
    if (targetType == ImportTargetType::CUSTOM ||
        targetType == ImportTargetType::TEMPLATE ||
        targetType == ImportTargetType::ORIGIN) {
        return itemName.empty();
    }
    
    return false;
}

bool ImportNode::IsSpecificImport() const {
    return !IsWildcardImport();
}

std::vector<std::string> ImportNode::GetExpectedExtensions() const {
    std::vector<std::string> extensions;
    
    switch (targetType) {
        case ImportTargetType::HTML:
            extensions.push_back(".html");
            extensions.push_back(".htm");
            break;
            
        case ImportTargetType::STYLE:
            extensions.push_back(".css");
            break;
            
        case ImportTargetType::JAVASCRIPT:
            extensions.push_back(".js");
            extensions.push_back(".mjs");
            break;
            
        case ImportTargetType::CHTL:
            extensions.push_back(".chtl");
            extensions.push_back(".cmod");
            break;
            
        case ImportTargetType::CJMOD:
            extensions.push_back(".cjmod");
            break;
            
        default:
            extensions.push_back(".chtl");
            break;
    }
    
    return extensions;
}

} // namespace CHTL