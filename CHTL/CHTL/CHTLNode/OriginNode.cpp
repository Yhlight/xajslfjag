#include "OriginNode.h"
#include <algorithm>

namespace CHTL {

OriginNode::OriginNode(const std::string& type, const std::string& name)
    : BaseNode(NodeType::ORIGIN), originType(type), originName(name) {
}

std::string OriginNode::Generate() const {
    // 原始嵌入的内容直接输出，不进行任何处理
    return content;
}

NodePtr OriginNode::Clone() const {
    auto clone = std::make_shared<OriginNode>(originType, originName);
    clone->SetPosition(line, column);
    clone->content = content;
    return clone;
}

void OriginNode::Accept(NodeVisitor* visitor) {
    visitor->VisitOriginNode(this);
}

std::string OriginNode::GenerateWrapped() const {
    // 某些情况下可能需要包装原始内容
    if (originType == "@Html") {
        return content;  // HTML直接输出
    } else if (originType == "@Style") {
        return "<style>\n" + content + "\n</style>";
    } else if (originType == "@JavaScript") {
        return "<script>\n" + content + "\n</script>";
    } else {
        // 自定义类型，直接输出
        return content;
    }
}

bool OriginNode::IsStandardType() const {
    static const std::vector<std::string> standardTypes = {
        "@Html", "@Style", "@JavaScript"
    };
    
    return std::find(standardTypes.begin(), standardTypes.end(), originType) 
           != standardTypes.end();
}

} // namespace CHTL