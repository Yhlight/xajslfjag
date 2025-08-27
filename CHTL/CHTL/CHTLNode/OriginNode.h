#pragma once

#include "BaseNode.h"

namespace CHTL {

// 原始嵌入节点
class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& type, const std::string& name = "");
    
    std::string GetOriginType() const { return originType; }
    std::string GetName() const override { return originName; }
    std::string GetContent() const { return content; }
    
    void SetContent(const std::string& c) { content = c; }
    void SetName(const std::string& n) { originName = n; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 判断是否是命名原始嵌入
    bool IsNamed() const { return !originName.empty(); }
    
    // 根据类型生成适当的包装
    std::string GenerateWrapped() const;
    
private:
    std::string originType;   // @Html, @Style, @JavaScript, 或自定义类型如@Vue
    std::string originName;   // 可选的名称
    std::string content;      // 原始内容
    
    // 判断是否是标准类型
    bool IsStandardType() const;
};

} // namespace CHTL