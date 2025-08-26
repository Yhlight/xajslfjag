#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * OperatorNode - 操作节点
 * 处理所有CHTL操作：delete、insert、inherit、use等
 */
class OperatorNode : public BaseNode {
public:
    enum class OperatorType {
        // 基本操作
        DELETE,         // delete
        INSERT,         // insert
        INHERIT,        // inherit
        
        // 位置操作
        AFTER,          // after
        BEFORE,         // before
        REPLACE,        // replace
        AT_TOP,         // at top
        AT_BOTTOM,      // at bottom
        
        // 使用操作（合并use节点）
        USE_HTML5,      // use html5;
        USE_CONFIG,     // use @Config Basic;
        USE_CONSTRAINT, // use [Configuration] @Config Basic;
        
        // 约束操作
        EXCEPT          // except
    };

private:
    OperatorType operator_type_;
    std::string target_;                        // 操作目标
    std::string source_;                        // 操作源（inherit时使用）
    std::vector<std::string> parameters_;       // 操作参数
    std::string position_context_;              // 位置上下文
    
    // Use操作特有属性
    std::string config_name_;                   // 配置名称（use时）
    bool is_configuration_prefix_;              // 是否有[Configuration]前缀
    
    // 约束操作特有属性
    std::vector<std::string> except_items_;     // except列表
    
public:
    /**
     * 构造函数
     * @param type 操作类型
     * @param target 操作目标
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    OperatorNode(OperatorType type,
                 const std::string& target,
                 const std::string& location = "", 
                 int line = 0, 
                 int column = 0);
    
    /**
     * 获取操作类型
     * @return 操作类型
     */
    OperatorType getOperatorType() const;
    
    /**
     * 设置操作类型
     * @param type 操作类型
     */
    void setOperatorType(OperatorType type);
    
    /**
     * 获取操作目标
     * @return 操作目标
     */
    const std::string& getTarget() const;
    
    /**
     * 设置操作目标
     * @param target 操作目标
     */
    void setTarget(const std::string& target);
    
    /**
     * 获取操作源
     * @return 操作源
     */
    const std::string& getSource() const;
    
    /**
     * 设置操作源
     * @param source 操作源
     */
    void setSource(const std::string& source);
    
    /**
     * 添加参数
     * @param parameter 参数
     */
    void addParameter(const std::string& parameter);
    
    /**
     * 获取参数列表
     * @return 参数列表
     */
    const std::vector<std::string>& getParameters() const;
    
    /**
     * 获取位置上下文
     * @return 位置上下文
     */
    const std::string& getPositionContext() const;
    
    /**
     * 设置位置上下文
     * @param context 位置上下文
     */
    void setPositionContext(const std::string& context);
    
    // Use操作相关方法
    /**
     * 获取配置名称
     * @return 配置名称
     */
    const std::string& getConfigName() const;
    
    /**
     * 设置配置名称
     * @param config_name 配置名称
     */
    void setConfigName(const std::string& config_name);
    
    /**
     * 是否有Configuration前缀
     * @return 是否有前缀
     */
    bool hasConfigurationPrefix() const;
    
    /**
     * 设置是否有Configuration前缀
     * @param has_prefix 是否有前缀
     */
    void setConfigurationPrefix(bool has_prefix);
    
    // Except操作相关方法
    /**
     * 添加except项目
     * @param item except项目
     */
    void addExceptItem(const std::string& item);
    
    /**
     * 获取except项目列表
     * @return except项目列表
     */
    const std::vector<std::string>& getExceptItems() const;
    
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
     * 执行操作
     * @return 执行结果
     */
    std::string executeOperation() const;
    
    /**
     * 生成JavaScript操作代码
     * @return JavaScript代码
     */
    std::string generateJSOperation() const;
    
    /**
     * 生成Use操作的HTML5声明
     * @return HTML5声明
     */
    std::string generateHTML5Declaration() const;
    
    /**
     * 应用配置
     * @param config_content 配置内容
     * @return 应用结果
     */
    std::string applyConfiguration(const std::string& config_content) const;
    
    /**
     * 验证约束条件
     * @param context 验证上下文
     * @return 是否满足约束
     */
    bool validateConstraints(const std::string& context) const;
    
    /**
     * 操作类型转字符串
     * @param type 操作类型
     * @return 字符串
     */
    static std::string operatorTypeToString(OperatorType type);
    
    /**
     * 字符串转操作类型
     * @param type_str 类型字符串
     * @return 操作类型
     */
    static OperatorType stringToOperatorType(const std::string& type_str);
    
    /**
     * 判断是否为位置操作
     * @param type 操作类型
     * @return 是否为位置操作
     */
    static bool isPositionOperation(OperatorType type);
    
    /**
     * 判断是否为Use操作
     * @param type 操作类型
     * @return 是否为Use操作
     */
    static bool isUseOperation(OperatorType type);
};

} // namespace Node
} // namespace CHTL