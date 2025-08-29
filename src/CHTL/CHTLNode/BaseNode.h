#pragma once
#include "../CHTLLexer/Token.h"
#include "../CHTLLexer/Type.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * CHTL节点类型枚举
 * 严格按照CHTL语法文档定义
 */
enum class CHTLNodeType {
    // 基础节点
    ROOT_NODE,              // 根节点
    ELEMENT_NODE,           // HTML元素节点
    TEXT_NODE,              // 文本节点
    COMMENT_NODE,           // 注释节点
    ATTRIBUTE_NODE,         // 属性节点
    
    // 样式节点
    STYLE_NODE,             // 局部样式节点
    INLINE_STYLE_NODE,      // 内联样式节点
    SELECTOR_STYLE_NODE,    // 选择器样式节点
    
    // 脚本节点
    SCRIPT_NODE,            // 局部脚本节点
    
    // 模板节点
    TEMPLATE_STYLE_NODE,    // 样式组模板节点
    TEMPLATE_ELEMENT_NODE,  // 元素模板节点
    TEMPLATE_VAR_NODE,      // 变量组模板节点
    
    // 高级模板节点（支持继承和特例化）
    ADVANCED_TEMPLATE_STYLE_NODE,   // 高级样式组模板节点
    ADVANCED_TEMPLATE_ELEMENT_NODE, // 高级元素模板节点
    ADVANCED_TEMPLATE_VAR_NODE,     // 高级变量组模板节点
    
    // 自定义节点
    CUSTOM_STYLE_NODE,      // 自定义样式组节点
    CUSTOM_ELEMENT_NODE,    // 自定义元素节点
    CUSTOM_VAR_NODE,        // 自定义变量组节点
    
    // 模板使用节点
    TEMPLATE_USAGE_NODE,    // 模板使用节点
    VARIABLE_REFERENCE_NODE, // 变量引用节点
    
    // 原始嵌入节点
    ORIGIN_HTML_NODE,       // 原始HTML嵌入节点
    ORIGIN_STYLE_NODE,      // 原始样式嵌入节点
    ORIGIN_JAVASCRIPT_NODE, // 原始JS嵌入节点
    ORIGIN_CUSTOM_NODE,     // 自定义原始嵌入节点
    
    // 导入节点
    IMPORT_HTML_NODE,       // 导入HTML节点
    IMPORT_STYLE_NODE,      // 导入样式节点
    IMPORT_JAVASCRIPT_NODE, // 导入JS节点
    IMPORT_CHTL_NODE,       // 导入CHTL节点
    IMPORT_CJMOD_NODE,      // 导入CJMOD节点
    IMPORT_CONFIG_NODE,     // 导入配置节点
    IMPORT_TEMPLATE_NODE,   // 导入模板节点
    IMPORT_CUSTOM_NODE,     // 导入自定义节点
    IMPORT_ORIGIN_NODE,     // 导入原始嵌入节点
    
    // 配置节点
    CONFIG_NODE,            // 配置节点
    CONFIG_NAME_NODE,       // Name配置节点
    CONFIG_ORIGINTYPE_NODE, // OriginType配置节点
    
    // 命名空间节点
    NAMESPACE_NODE,         // 命名空间节点
    
    // 操作节点
    USE_NODE,               // use操作节点
    INHERIT_NODE,           // inherit操作节点
    DELETE_NODE,            // delete操作节点
    INSERT_NODE,            // insert操作节点
    EXCEPT_NODE,            // except约束节点
    
    // 特殊节点
    VARIABLE_NODE,          // 变量节点
    SELECTOR_NODE,          // 选择器节点
    INDEX_ACCESS_NODE,      // 索引访问节点
    
    // 未知节点
    UNKNOWN_NODE            // 未知节点
};

/**
 * 节点访问权限
 */
enum class NodeAccess {
    PUBLIC,     // 公共访问
    PRIVATE,    // 私有访问
    PROTECTED,  // 保护访问
    INTERNAL    // 内部访问
};

/**
 * CHTL AST基础节点类
 * 所有CHTL节点的基类
 */
class BaseNode {
public:
    /**
     * 构造函数
     */
    BaseNode(CHTLNodeType type, const std::string& name = "");
    
    /**
     * 虚析构函数
     */
    virtual ~BaseNode() = default;
    
    // ========== 基础属性 ==========
    
    /**
     * 获取节点类型
     */
    CHTLNodeType getNodeType() const;
    
    /**
     * 获取节点名称
     */
    const std::string& getName() const;
    
    /**
     * 设置节点名称
     */
    void setName(const std::string& name);
    
    /**
     * 获取节点值
     */
    const std::string& getValue() const;
    
    /**
     * 设置节点值
     */
    void setValue(const std::string& value);
    
    /**
     * 获取节点位置
     */
    const Position& getPosition() const;
    
    /**
     * 设置节点位置
     */
    void setPosition(const Position& position);
    
    /**
     * 获取访问权限
     */
    NodeAccess getAccess() const;
    
    /**
     * 设置访问权限
     */
    void setAccess(NodeAccess access);
    
    // ========== 父子关系 ==========
    
    /**
     * 获取父节点
     */
    BaseNode* getParent() const;
    
    /**
     * 设置父节点
     */
    void setParent(BaseNode* parent);
    
    /**
     * 获取子节点数量
     */
    size_t getChildCount() const;
    
    /**
     * 获取子节点
     */
    BaseNode* getChild(size_t index) const;
    
    /**
     * 获取所有子节点
     */
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const;
    
    /**
     * 添加子节点
     */
    void addChild(std::shared_ptr<BaseNode> child);
    
    /**
     * 插入子节点
     */
    void insertChild(size_t index, std::shared_ptr<BaseNode> child);
    
    /**
     * 移除子节点
     */
    bool removeChild(BaseNode* child);
    
    /**
     * 移除指定索引的子节点
     */
    bool removeChild(size_t index);
    
    /**
     * 清空所有子节点
     */
    void clearChildren();
    
    // ========== 属性管理 ==========
    
    /**
     * 设置属性
     */
    void setAttribute(const std::string& key, const std::string& value);
    
    /**
     * 获取属性
     */
    std::string getAttribute(const std::string& key) const;
    
    /**
     * 检查是否有属性
     */
    bool hasAttribute(const std::string& key) const;
    
    /**
     * 移除属性
     */
    bool removeAttribute(const std::string& key);
    
    /**
     * 获取所有属性
     */
    const std::unordered_map<std::string, std::string>& getAttributes() const;
    
    // ========== 节点查找 ==========
    
    /**
     * 根据名称查找子节点
     */
    BaseNode* findChildByName(const std::string& name) const;
    
    /**
     * 根据类型查找子节点
     */
    std::vector<BaseNode*> findChildrenByType(CHTLNodeType type) const;
    
    /**
     * 根据名称和类型查找子节点
     */
    BaseNode* findChildByNameAndType(const std::string& name, CHTLNodeType type) const;
    
    /**
     * 深度优先搜索节点
     */
    BaseNode* findNodeDFS(const std::string& name) const;
    
    /**
     * 广度优先搜索节点
     */
    BaseNode* findNodeBFS(const std::string& name) const;
    
    // ========== 节点操作 ==========
    
    /**
     * 克隆节点
     */
    virtual std::shared_ptr<BaseNode> clone() const;
    
    /**
     * 深度克隆节点（包括子节点）
     */
    virtual std::shared_ptr<BaseNode> deepClone() const;
    
    /**
     * 接受访问者
     */
    virtual void accept(class NodeVisitor& visitor);
    
    // ========== 输出和调试 ==========
    
    /**
     * 转换为字符串
     */
    virtual std::string toString() const;
    
    /**
     * 转换为XML格式
     */
    virtual std::string toXml() const;
    
    /**
     * 转换为JSON格式
     */
    virtual std::string toJson() const;
    
    /**
     * 获取调试信息
     */
    virtual std::string getDebugInfo() const;
    
    // ========== 验证 ==========
    
    /**
     * 验证节点
     */
    virtual bool validate(ErrorReporter* errorReporter = nullptr) const;
    
    /**
     * 检查节点完整性
     */
    virtual bool checkIntegrity() const;

protected:
    CHTLNodeType m_nodeType;        // 节点类型
    std::string m_name;             // 节点名称
    std::string m_value;            // 节点值
    Position m_position;            // 节点位置
    NodeAccess m_access;            // 访问权限
    
    BaseNode* m_parent;             // 父节点（弱引用）
    std::vector<std::shared_ptr<BaseNode>> m_children;  // 子节点
    std::unordered_map<std::string, std::string> m_attributes; // 属性
    
    /**
     * 内部验证方法
     */
    virtual bool internalValidate(ErrorReporter* errorReporter) const;
    
    /**
     * 获取节点类型字符串
     */
    std::string getNodeTypeString() const;
};

/**
 * 节点智能指针类型定义
 */
using NodePtr = std::shared_ptr<BaseNode>;
using NodeWeakPtr = std::weak_ptr<BaseNode>;
using NodeVector = std::vector<NodePtr>;

/**
 * 节点工厂类
 */
class NodeFactory {
public:
    /**
     * 创建节点
     */
    static NodePtr createNode(CHTLNodeType type, const std::string& name = "");
    
    /**
     * 根据Token创建节点
     */
    static NodePtr createFromToken(const Token& token);
    
    /**
     * 创建文本节点
     */
    static NodePtr createTextNode(const std::string& text);
    
    /**
     * 创建元素节点
     */
    static NodePtr createElementNode(const std::string& tagName);
    
    /**
     * 创建属性节点
     */
    static NodePtr createAttributeNode(const std::string& name, const std::string& value);
};

} // namespace CHTL