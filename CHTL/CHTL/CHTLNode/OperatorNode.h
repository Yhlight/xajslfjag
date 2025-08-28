#ifndef CHTL_OPERATORNODE_H
#define CHTL_OPERATORNODE_H

#include "BaseNode.h"

namespace CHTL {

// 操作符节点基类
class OperatorNode : public BaseNode {
public:
    OperatorNode();
    virtual ~OperatorNode();
};

// delete操作符节点
class DeleteOperatorNode : public OperatorNode {
public:
    DeleteOperatorNode();
    ~DeleteOperatorNode();
};

// insert操作符节点
class InsertOperatorNode : public OperatorNode {
public:
    InsertOperatorNode();
    ~InsertOperatorNode();
};

// use操作符节点
class UseOperatorNode : public OperatorNode {
public:
    UseOperatorNode();
    ~UseOperatorNode();
};

} // namespace CHTL

#endif // CHTL_OPERATORNODE_H