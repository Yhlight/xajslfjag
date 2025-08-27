#ifndef CHTL_ORIGIN_NODE_H
#define CHTL_ORIGIN_NODE_H

#include "BaseNode.h"

namespace CHTL {

// 原始嵌入节点类
class OriginNode : public BaseNode {
private:
    std::string originType;  // @Html, @Style, @JavaScript, 或自定义类型
    std::string name;        // 可选的名称
    std::string content;     // 原始内容
    bool isCustomType;       // 是否为自定义类型
    
public:
    OriginNode(const std::string& type, const std::string& n = "", int line = 0, int col = 0)
        : BaseNode(NodeType::ORIGIN, line, col), originType(type), name(n) {
        // 检查是否为标准类型
        isCustomType = !(type == "@Html" || type == "@Style" || type == "@JavaScript");
    }
    
    // 类型访问
    const std::string& getOriginType() const { return originType; }
    void setOriginType(const std::string& type) { 
        originType = type;
        isCustomType = !(type == "@Html" || type == "@Style" || type == "@JavaScript");
    }
    
    // 名称访问
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    bool hasName() const { return !name.empty(); }
    
    // 内容访问
    const std::string& getContent() const { return content; }
    void setContent(const std::string& c) { content = c; }
    void appendContent(const std::string& c) { content += c; }
    
    // 类型判断
    bool getIsCustomType() const { return isCustomType; }
    bool isHtmlType() const { return originType == "@Html"; }
    bool isStyleType() const { return originType == "@Style"; }
    bool isJavaScriptType() const { return originType == "@JavaScript"; }
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class OriginNodeVisitor : public NodeVisitor {
public:
    virtual void visitOriginNode(OriginNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_ORIGIN_NODE_H