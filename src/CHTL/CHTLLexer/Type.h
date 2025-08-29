#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace CHTL {

/**
 * CHTL节点类型枚举
 * 严格按照CHTL语法文档定义
 */
enum class NodeType {
    // 基础节点类型
    TEXT_NODE,              // text节点
    ELEMENT_NODE,           // HTML元素节点
    ATTRIBUTE_NODE,         // 属性节点
    
    // 样式相关节点
    STYLE_BLOCK,            // style样式块
    INLINE_STYLE,           // 内联样式
    CLASS_SELECTOR_STYLE,   // 类选择器样式
    ID_SELECTOR_STYLE,      // ID选择器样式
    PSEUDO_SELECTOR_STYLE,  // 伪类/伪元素选择器样式
    
    // 模板节点
    TEMPLATE_STYLE,         // 样式组模板
    TEMPLATE_ELEMENT,       // 元素模板
    TEMPLATE_VAR,           // 变量组模板
    
    // 自定义节点
    CUSTOM_STYLE,           // 自定义样式组
    CUSTOM_ELEMENT,         // 自定义元素
    CUSTOM_VAR,             // 自定义变量组
    
    // 特殊节点
    ORIGIN_HTML,            // 原始HTML嵌入
    ORIGIN_STYLE,           // 原始样式嵌入
    ORIGIN_JAVASCRIPT,      // 原始JS嵌入
    ORIGIN_CUSTOM,          // 自定义类型原始嵌入
    
    // 导入节点
    IMPORT_HTML,            // 导入HTML
    IMPORT_STYLE,           // 导入样式
    IMPORT_JAVASCRIPT,      // 导入JS
    IMPORT_CHTL,            // 导入CHTL
    IMPORT_CJMOD,           // 导入CJMOD
    IMPORT_CONFIG,          // 导入配置
    
    // 结构节点
    NAMESPACE,              // 命名空间
    CONFIGURATION,          // 配置组
    INFO,                   // 信息节点
    EXPORT,                 // 导出节点
    
    // 操作节点
    INHERIT_OP,             // 继承操作
    DELETE_OP,              // 删除操作
    INSERT_OP,              // 插入操作
    SPECIALIZE_OP,          // 特例化操作
    
    // 约束节点
    EXCEPT_CONSTRAINT,      // except约束
    
    // 注释节点
    COMMENT_LINE,           // 行注释
    COMMENT_BLOCK,          // 块注释
    COMMENT_GENERATOR,      // 生成器注释
    
    // 根节点
    ROOT,                   // 根节点
    
    // 错误节点
    UNKNOWN_NODE,           // 未知节点
    INVALID_NODE            // 无效节点
};

/**
 * CHTL数据类型枚举
 */
enum class DataType {
    STRING,                 // 字符串类型
    IDENTIFIER,             // 标识符类型
    SELECTOR,               // 选择器类型
    HTML_TAG,               // HTML标签类型
    CSS_PROPERTY,           // CSS属性类型
    CSS_VALUE,              // CSS值类型
    PATH,                   // 路径类型
    INDEX,                  // 索引类型
    BOOLEAN,                // 布尔类型
    UNKNOWN_TYPE            // 未知类型
};

/**
 * CHTL作用域类型
 */
enum class ScopeType {
    GLOBAL,                 // 全局作用域
    NAMESPACE_SCOPE,        // 命名空间作用域
    ELEMENT_SCOPE,          // 元素作用域
    STYLE_SCOPE,            // 样式作用域
    TEMPLATE_SCOPE,         // 模板作用域
    CUSTOM_SCOPE,           // 自定义作用域
    CONFIGURATION_SCOPE,    // 配置作用域
    LOCAL_SCOPE             // 局部作用域
};

/**
 * Type信息类
 * 存储CHTL语法元素的类型信息
 */
class TypeInfo {
public:
    NodeType nodeType;          // 节点类型
    DataType dataType;          // 数据类型
    ScopeType scopeType;        // 作用域类型
    std::string name;           // 类型名称
    std::string description;    // 类型描述
    
    TypeInfo(NodeType nt = NodeType::UNKNOWN_NODE, 
             DataType dt = DataType::UNKNOWN_TYPE,
             ScopeType st = ScopeType::LOCAL_SCOPE,
             const std::string& n = "",
             const std::string& desc = "")
        : nodeType(nt), dataType(dt), scopeType(st), name(n), description(desc) {}
    
    // 判断是否为模板类型
    bool isTemplateType() const;
    
    // 判断是否为自定义类型
    bool isCustomType() const;
    
    // 判断是否为原始嵌入类型
    bool isOriginType() const;
    
    // 判断是否为导入类型
    bool isImportType() const;
    
    // 判断是否为样式相关类型
    bool isStyleType() const;
    
    // 判断是否为结构类型
    bool isStructuralType() const;
    
    // 获取类型的字符串表示
    std::string toString() const;
};

/**
 * HTML元素类型管理
 */
class HtmlElementTypes {
public:
    // 获取所有有效的HTML元素标签
    static const std::unordered_set<std::string>& getValidElements();
    
    // 检查是否为有效的HTML元素
    static bool isValidElement(const std::string& elementName);
    
    // 检查是否为单标签元素
    static bool isSelfClosingElement(const std::string& elementName);
    
    // 检查是否为块级元素
    static bool isBlockElement(const std::string& elementName);
    
    // 检查是否为行内元素
    static bool isInlineElement(const std::string& elementName);
};

/**
 * CSS属性类型管理
 */
class CssPropertyTypes {
public:
    // 获取所有有效的CSS属性
    static const std::unordered_set<std::string>& getValidProperties();
    
    // 检查是否为有效的CSS属性
    static bool isValidProperty(const std::string& propertyName);
    
    // 获取属性的值类型
    static DataType getPropertyValueType(const std::string& propertyName);
};

/**
 * CHTL关键字类型管理
 */
class ChtlKeywordTypes {
public:
    // 获取所有CHTL关键字
    static const std::unordered_set<std::string>& getKeywords();
    
    // 检查是否为CHTL关键字
    static bool isKeyword(const std::string& word);
    
    // 获取关键字对应的节点类型
    static NodeType getKeywordNodeType(const std::string& keyword);
};

/**
 * 类型工厂类
 */
class TypeFactory {
public:
    // 创建类型信息
    static TypeInfo createTypeInfo(const std::string& typeName);
    
    // 根据Token创建类型信息
    static TypeInfo createFromToken(const std::string& tokenValue, const std::string& context = "");
    
    // 检查类型兼容性
    static bool isCompatible(const TypeInfo& source, const TypeInfo& target);
    
    // 获取默认类型
    static TypeInfo getDefaultType();
};

} // namespace CHTL