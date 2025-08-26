#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * TemplateNode - 模板节点
 * 处理[Template] @Style, @Element, @Var定义
 */
class TemplateNode : public BaseNode {
public:
    enum class TemplateType {
        STYLE,      // @Style
        ELEMENT,    // @Element
        VAR         // @Var
    };

private:
    TemplateType template_type_;
    std::string template_name_;
    std::string template_definition_;
    std::unordered_map<std::string, std::string> parameters_;
    bool is_exported_;
    
public:
    /**
     * 构造函数
     * @param type 模板类型
     * @param name 模板名称
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    TemplateNode(TemplateType type,
                 const std::string& name,
                 const std::string& location = "", 
                 int line = 0, 
                 int column = 0);
    
    /**
     * 获取模板类型
     * @return 模板类型
     */
    TemplateType getTemplateType() const;
    
    /**
     * 获取模板名称
     * @return 模板名称
     */
    const std::string& getTemplateName() const;
    
    /**
     * 设置模板名称
     * @param name 模板名称
     */
    void setTemplateName(const std::string& name);
    
    /**
     * 获取模板定义
     * @return 模板定义
     */
    const std::string& getTemplateDefinition() const;
    
    /**
     * 设置模板定义
     * @param definition 模板定义
     */
    void setTemplateDefinition(const std::string& definition);
    
    /**
     * 添加参数
     * @param key 参数键
     * @param value 参数值
     */
    void addParameter(const std::string& key, const std::string& value);
    
    /**
     * 获取参数
     * @param key 参数键
     * @return 参数值
     */
    std::string getParameter(const std::string& key) const;
    
    /**
     * 获取所有参数
     * @return 参数映射
     */
    const std::unordered_map<std::string, std::string>& getParameters() const;
    
    /**
     * 是否被导出
     * @return 是否导出
     */
    bool isExported() const;
    
    /**
     * 设置导出状态
     * @param exported 是否导出
     */
    void setExported(bool exported);
    
    /**
     * 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;
    
    /**
     * 接受访问者模式
     * @param visitor 访问者
     */
    void accept(NodeVisitor& visitor) override;
    
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 实例化模板
     * @param instance_name 实例名称
     * @param parameters 实例参数
     * @return 实例化结果
     */
    std::string instantiate(const std::string& instance_name,
                           const std::unordered_map<std::string, std::string>& parameters) const;
    
    /**
     * 模板类型转字符串
     * @param type 模板类型
     * @return 字符串
     */
    static std::string templateTypeToString(TemplateType type);
};

} // namespace Node
} // namespace CHTL