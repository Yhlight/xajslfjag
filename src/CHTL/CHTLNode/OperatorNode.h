#pragma once
#include "BaseNode.h"
#include <vector>

namespace CHTL {

/**
 * 操作类型枚举
 */
enum class OperatorType {
    USE_OP,         // use操作
    INHERIT_OP,     // inherit操作  
    DELETE_OP,      // delete操作
    INSERT_OP,      // insert操作
    EXCEPT_OP       // except约束操作
};

/**
 * 插入位置枚举
 */
enum class InsertPosition {
    BEFORE,     // 在...之前
    AFTER,      // 在...之后
    REPLACE,    // 替换
    AT_TOP,     // 在顶部
    AT_BOTTOM   // 在底部
};

/**
 * 操作节点基类
 * 表示CHTL中的各种操作（use, inherit, delete, insert, except）
 */
class OperatorNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    OperatorNode(OperatorType operatorType, const std::string& target = "");
    
    /**
     * 析构函数
     */
    virtual ~OperatorNode() = default;
    
    // ========== 操作基础属性 ==========
    
    /**
     * 获取操作类型
     */
    OperatorType getOperatorType() const;
    
    /**
     * 获取目标
     */
    const std::string& getTarget() const;
    
    /**
     * 设置目标
     */
    void setTarget(const std::string& target);
    
    /**
     * 获取操作类型字符串
     */
    std::string getOperatorTypeString() const;
    
    // ========== 重写基类方法 ==========
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 转换为字符串
     */
    std::string toString() const override;
    
    /**
     * 验证节点
     */
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

protected:
    /**
     * 内部验证方法
     */
    bool internalValidate(ErrorReporter* errorReporter) const override;

private:
    OperatorType m_operatorType;    // 操作类型
    std::string m_target;           // 操作目标
};

/**
 * use操作节点
 * 用于指定使用的配置组或HTML5模式
 */
class UseNode : public OperatorNode {
public:
    /**
     * 构造函数
     */
    UseNode(const std::string& target = "");
    
    /**
     * 检查是否为HTML5模式
     */
    bool isHtml5Mode() const;
    
    /**
     * 设置HTML5模式
     */
    void setHtml5Mode(bool html5);
    
    /**
     * 获取配置组名称
     */
    std::string getConfigName() const;
    
    /**
     * 设置配置组名称
     */
    void setConfigName(const std::string& configName);
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;

private:
    bool m_isHtml5;         // 是否为HTML5模式
    std::string m_configName; // 配置组名称
};

/**
 * inherit操作节点
 * 用于显式继承模板或自定义
 */
class InheritNode : public OperatorNode {
public:
    /**
     * 构造函数
     */
    InheritNode(const std::string& target = "");
    
    /**
     * 获取继承类型（@Style, @Element, @Var等）
     */
    std::string getInheritType() const;
    
    /**
     * 设置继承类型
     */
    void setInheritType(const std::string& type);
    
    /**
     * 获取源名称
     */
    std::string getSourceName() const;
    
    /**
     * 设置源名称
     */
    void setSourceName(const std::string& sourceName);
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;

private:
    std::string m_inheritType;  // 继承类型
    std::string m_sourceName;   // 源名称
};

/**
 * delete操作节点
 * 用于删除属性、元素或继承
 */
class DeleteNode : public OperatorNode {
public:
    /**
     * 构造函数
     */
    DeleteNode(const std::string& target = "");
    
    /**
     * 添加删除目标
     */
    void addDeleteTarget(const std::string& target);
    
    /**
     * 获取所有删除目标
     */
    const std::vector<std::string>& getDeleteTargets() const;
    
    /**
     * 检查是否删除指定目标
     */
    bool isDeleting(const std::string& target) const;
    
    /**
     * 清空删除目标
     */
    void clearDeleteTargets();
    
    /**
     * 检查是否删除继承
     */
    bool isDeletingInheritance() const;
    
    /**
     * 设置删除继承
     */
    void setDeletingInheritance(bool deleting);
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;

private:
    std::vector<std::string> m_deleteTargets; // 删除目标列表
    bool m_deletingInheritance;               // 是否删除继承
};

/**
 * insert操作节点
 * 用于插入元素
 */
class InsertNode : public OperatorNode {
public:
    /**
     * 构造函数
     */
    InsertNode(const std::string& target = "");
    
    /**
     * 获取插入位置
     */
    InsertPosition getInsertPosition() const;
    
    /**
     * 设置插入位置
     */
    void setInsertPosition(InsertPosition position);
    
    /**
     * 获取插入位置字符串
     */
    std::string getInsertPositionString() const;
    
    /**
     * 获取目标选择器
     */
    std::string getTargetSelector() const;
    
    /**
     * 设置目标选择器
     */
    void setTargetSelector(const std::string& selector);
    
    /**
     * 获取索引（用于数组访问）
     */
    int getIndex() const;
    
    /**
     * 设置索引
     */
    void setIndex(int index);
    
    /**
     * 检查是否有索引
     */
    bool hasIndex() const;
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;

private:
    InsertPosition m_insertPosition; // 插入位置
    std::string m_targetSelector;    // 目标选择器
    int m_index;                     // 索引
    bool m_hasIndex;                 // 是否有索引
};

/**
 * except约束节点
 * 用于定义域约束
 */
class ExceptNode : public OperatorNode {
public:
    /**
     * 构造函数
     */
    ExceptNode(const std::string& target = "");
    
    /**
     * 添加约束目标
     */
    void addConstraintTarget(const std::string& target);
    
    /**
     * 获取所有约束目标
     */
    const std::vector<std::string>& getConstraintTargets() const;
    
    /**
     * 检查是否约束指定目标
     */
    bool isConstraining(const std::string& target) const;
    
    /**
     * 清空约束目标
     */
    void clearConstraintTargets();
    
    /**
     * 检查是否为全局约束
     */
    bool isGlobalConstraint() const;
    
    /**
     * 设置全局约束
     */
    void setGlobalConstraint(bool global);
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;

private:
    std::vector<std::string> m_constraintTargets; // 约束目标列表
    bool m_isGlobal;                              // 是否为全局约束
};

} // namespace CHTL