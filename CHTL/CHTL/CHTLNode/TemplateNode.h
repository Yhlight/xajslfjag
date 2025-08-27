#pragma once
#include "BaseNode.h"
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * @brief 模板类型枚举
 */
enum class TemplateType {
    STYLE,      // 样式组模板
    ELEMENT,    // 元素模板
    VAR         // 变量组模板
};

/**
 * @brief 变量定义结构
 */
struct VariableDefinition {
    std::string name;
    std::string value;
    std::string type;  // 变量类型（可选）
    
    VariableDefinition() = default;
    VariableDefinition(const std::string& n, const std::string& v, const std::string& t = "")
        : name(n), value(v), type(t) {}
};

/**
 * @brief 模板节点类
 * 表示CHTL中的模板定义
 */
class TemplateNode : public BaseNode {
private:
    TemplateType template_type_;                              // 模板类型
    std::string template_name_;                               // 模板名称
    std::vector<std::string> inheritance_chain_;              // 继承链
    std::unordered_map<std::string, VariableDefinition> variables_; // 变量定义（用于变量组模板）
    bool is_abstract_;                                        // 是否为抽象模板

public:
    /**
     * @brief 构造函数
     * @param type 模板类型
     * @param name 模板名称
     */
    TemplateNode(TemplateType type, const std::string& name);

    /**
     * @brief 析构函数
     */
    virtual ~TemplateNode() = default;

    // Getter方法
    TemplateType getTemplateType() const { return template_type_; }
    const std::string& getTemplateName() const { return template_name_; }
    const std::vector<std::string>& getInheritanceChain() const { return inheritance_chain_; }
    const std::unordered_map<std::string, VariableDefinition>& getVariables() const { return variables_; }
    bool isAbstract() const { return is_abstract_; }

    // Setter方法
    void setTemplateName(const std::string& name) { template_name_ = name; }
    void setAbstract(bool abstract) { is_abstract_ = abstract; }

    /**
     * @brief 添加继承
     * @param parent_template 父模板名称
     */
    void addInheritance(const std::string& parent_template);

    /**
     * @brief 移除继承
     * @param parent_template 父模板名称
     * @return 是否成功移除
     */
    bool removeInheritance(const std::string& parent_template);

    /**
     * @brief 清空继承链
     */
    void clearInheritance();

    /**
     * @brief 检查是否继承自指定模板
     * @param template_name 模板名称
     * @return 是否继承
     */
    bool inheritsFrom(const std::string& template_name) const;

    /**
     * @brief 添加变量定义（用于变量组模板）
     * @param variable 变量定义
     */
    void addVariable(const VariableDefinition& variable);

    /**
     * @brief 获取变量定义
     * @param name 变量名
     * @return 变量定义，如果不存在返回nullptr
     */
    const VariableDefinition* getVariable(const std::string& name) const;

    /**
     * @brief 移除变量定义
     * @param name 变量名
     * @return 是否成功移除
     */
    bool removeVariable(const std::string& name);

    /**
     * @brief 检查是否有指定变量
     * @param name 变量名
     * @return 是否存在
     */
    bool hasVariable(const std::string& name) const;

    /**
     * @brief 获取所有变量名
     * @return 变量名列表
     */
    std::vector<std::string> getVariableNames() const;

    /**
     * @brief 展开模板（应用到具体元素）
     * @param target_node 目标节点
     * @param parameters 模板参数（可选）
     * @return 是否成功展开
     */
    bool expandTo(NodePtr target_node, 
                  const std::unordered_map<std::string, std::string>& parameters = {}) const;

    /**
     * @brief 合并另一个模板（用于继承）
     * @param other 另一个模板节点
     * @return 是否成功合并
     */
    bool merge(const TemplateNode& other);

    /**
     * @brief 验证模板的完整性
     * @return 验证结果消息
     */
    std::string validateTemplate() const;

    /**
     * @brief 检查是否存在循环继承
     * @param visited 已访问的模板集合（用于递归检查）
     * @return 是否存在循环继承
     */
    bool hasCircularInheritance(std::unordered_set<std::string>& visited) const;

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

    /**
     * @brief 生成模板代码
     * @param indent 缩进级别
     * @return 模板代码字符串
     */
    std::string generateTemplateCode(int indent = 0) const;

    /**
     * @brief 验证模板节点的有效性
     * @return 验证结果消息
     */
    virtual std::string validate() const override;

    /**
     * @brief 获取模板类型的字符串表示
     * @return 类型字符串
     */
    std::string getTemplateTypeString() const;

private:
    /**
     * @brief 解析变量引用
     * @param content 包含变量引用的内容
     * @param parameters 参数映射
     * @return 解析后的内容
     */
    std::string resolveVariableReferences(const std::string& content,
                                         const std::unordered_map<std::string, std::string>& parameters) const;

    /**
     * @brief 验证变量名的有效性
     * @param name 变量名
     * @return 是否有效
     */
    bool isValidVariableName(const std::string& name) const;
};

} // namespace CHTL