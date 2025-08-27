#pragma once
#include "BaseNode.hpp"
#include "TemplateNode.hpp"
#include <unordered_set>

namespace CHTL {

/**
 * 自定义类型枚举
 */
enum class CustomType {
    STYLE,                // 自定义样式组
    ELEMENT,              // 自定义元素
    VAR                   // 自定义变量组
};

/**
 * 特例化操作类型
 */
enum class SpecializationType {
    DELETE_PROPERTY,      // 删除属性
    DELETE_INHERITANCE,   // 删除继承
    INSERT_ELEMENT,       // 插入元素
    REPLACE_ELEMENT,      // 替换元素
    MODIFY_PROPERTY       // 修改属性
};

/**
 * 特例化操作结构
 */
struct SpecializationOp {
    SpecializationType type;
    std::string target;              // 目标（属性名、元素名等）
    std::string value;               // 值（用于修改操作）
    std::string position;            // 位置（after, before, replace等）
    std::vector<std::string> params; // 额外参数
    
    SpecializationOp(SpecializationType t, const std::string& tgt = "", 
                    const std::string& val = "", const std::string& pos = "")
        : type(t), target(tgt), value(val), position(pos) {}
};

/**
 * CHTL自定义节点类
 * 实现自定义系统，支持样式组、元素和变量组的自定义以及特例化
 */
class CustomNode : public BaseNode {
private:
    CustomType customType;
    std::string customName;
    std::string namespacePath;
    std::vector<std::string> inheritanceChain;       // 继承链
    std::vector<SpecializationOp> specializations;   // 特例化操作
    std::unordered_map<std::string, std::string> parameters; // 参数（用于无值样式组等）
    bool hasSpecializations;                         // 是否有特例化操作
    bool allowsSpecialization;                       // 是否允许特例化

public:
    CustomNode(CustomType type, const std::string& name = "");
    
    // 自定义特定方法
    CustomType getCustomType() const;
    void setCustomType(CustomType type);
    
    const std::string& getCustomName() const;
    void setCustomName(const std::string& name);
    
    const std::string& getNamespacePath() const;
    void setNamespacePath(const std::string& path);
    
    // 继承管理
    void addInheritance(const std::string& parentCustom);
    void removeInheritance(const std::string& parentCustom);
    const std::vector<std::string>& getInheritanceChain() const;
    bool hasInheritance() const;
    bool inheritsFrom(const std::string& customName) const;
    
    // 特例化操作管理
    void addSpecialization(const SpecializationOp& op);
    void removeSpecialization(size_t index);
    const std::vector<SpecializationOp>& getSpecializations() const;
    bool hasSpecializationOps() const;
    
    // 便捷的特例化方法
    void deleteProperty(const std::string& propertyName);
    void deleteInheritance(const std::string& inheritanceName);
    void insertElement(const std::string& position, const std::string& selector, 
                      std::shared_ptr<BaseNode> element);
    void replaceElement(const std::string& selector, std::shared_ptr<BaseNode> element);
    void modifyProperty(const std::string& propertyName, const std::string& newValue);
    
    // 参数管理（用于无值样式组）
    void setParameter(const std::string& name, const std::string& value = "");
    std::string getParameter(const std::string& name) const;
    bool hasParameter(const std::string& name) const;
    void removeParameter(const std::string& name);
    const std::unordered_map<std::string, std::string>& getParameters() const;
    bool isParameterValueless(const std::string& name) const;
    
    // 特例化控制
    bool getAllowsSpecialization() const;
    void setAllowsSpecialization(bool allows);
    
    // 索引访问（用于元素特例化）
    std::shared_ptr<BaseNode> getElementByIndex(size_t index, const std::string& elementType = "") const;
    std::vector<std::shared_ptr<BaseNode>> getElementsByType(const std::string& elementType) const;
    
    // 自定义实例化
    std::shared_ptr<BaseNode> instantiate(const std::unordered_map<std::string, std::string>& params = {},
                                         const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& specializations = {}) const;
    
    bool canInstantiate(const std::unordered_map<std::string, std::string>& params) const;
    
    // 特例化应用
    void applySpecializations(std::shared_ptr<BaseNode> target, 
                             const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& specializations = {}) const;
    
    // 重写基类方法
    std::string toString() const override;
    std::string toHTML() const override;
    std::string toCSS() const override;
    std::string toJavaScript() const override;
    void validate(CHTLState& state) override;
    std::shared_ptr<BaseNode> clone() const override;
    
    // 自定义特殊方法
    void mergeWith(const CustomNode& other);
    bool isCompatibleWith(const CustomNode& other) const;
    
private:
    std::string getTypeString() const;
    void applyDeleteOperations(std::shared_ptr<BaseNode> target) const;
    void applyInsertOperations(std::shared_ptr<BaseNode> target, 
                              const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& specializations) const;
    void applyReplaceOperations(std::shared_ptr<BaseNode> target, 
                               const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& specializations) const;
    void applyModifyOperations(std::shared_ptr<BaseNode> target) const;
};

/**
 * 自定义实例化器
 * 负责处理自定义的实例化过程
 */
class CustomInstantiator {
public:
    static std::shared_ptr<BaseNode> instantiateStyleCustom(
        const CustomNode& customNode,
        const std::unordered_map<std::string, std::string>& params = {},
        const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& specializations = {}
    );
    
    static std::shared_ptr<BaseNode> instantiateElementCustom(
        const CustomNode& customNode,
        const std::unordered_map<std::string, std::string>& params = {},
        const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& specializations = {}
    );
    
    static std::shared_ptr<BaseNode> instantiateVarCustom(
        const CustomNode& customNode,
        const std::unordered_map<std::string, std::string>& params = {}
    );
    
private:
    static std::shared_ptr<BaseNode> cloneAndSpecialize(
        const std::shared_ptr<BaseNode>& node,
        const std::unordered_map<std::string, std::string>& params,
        const std::vector<SpecializationOp>& specializations
    );
    
    static void processIndexedAccess(
        std::shared_ptr<BaseNode> target,
        const std::string& selector,
        const std::function<void(std::shared_ptr<BaseNode>)>& operation
    );
};

}