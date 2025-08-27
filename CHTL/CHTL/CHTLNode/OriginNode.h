#pragma once
#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 原始嵌入节点类
 * 表示CHTL中的原始嵌入内容
 */
class OriginNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param name 原始嵌入名称
     */
    OriginNode(const std::string& name = "");

    /**
     * @brief 析构函数
     */
    virtual ~OriginNode() = default;

    /**
     * @brief 克隆节点
     * @param deep 是否深度克隆
     * @return 克隆的节点
     */
    virtual NodePtr clone(bool deep = true) const override;

    /**
     * @brief 接受访问者
     * @param visitor 访问者对象
     */
    virtual void accept(NodeVisitor& visitor) override;

    /**
     * @brief 转换为字符串表示
     * @param indent 缩进级别
     * @return 字符串表示
     */
    virtual std::string toString(int indent = 0) const override;
};

} // namespace CHTL