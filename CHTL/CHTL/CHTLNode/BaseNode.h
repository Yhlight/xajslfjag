#ifndef CHTL_BASENODE_H
#define CHTL_BASENODE_H

namespace CHTL {

// 基础节点类
class BaseNode {
public:
    BaseNode();
    virtual ~BaseNode();
};

// 元素节点类
class ElementNode : public BaseNode {
public:
    ElementNode();
    ~ElementNode();
};

// 文本节点类
class TextNode : public BaseNode {
public:
    TextNode();
    ~TextNode();
};

} // namespace CHTL

#endif // CHTL_BASENODE_H