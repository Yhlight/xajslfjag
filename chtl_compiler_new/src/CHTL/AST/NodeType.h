#pragma once

namespace CHTL {
namespace AST {

/**
 * CHTL AST节点类型
 * 严格按照CHTL语法文档定义
 */
enum class NodeType {
    // 基础节点
    ROOT,                 // 根节点
    ELEMENT,              // 元素节点 (div, span, html等)
    TEXT,                 // 文本节点 (text { })
    COMMENT,              // 注释节点
    
    // 属性
    ATTRIBUTE,            // 属性 (id: box;)
    ATTRIBUTE_LIST,       // 属性列表
    
    // 样式相关
    STYLE_BLOCK,          // 局部样式块 (style { })
    STYLE_RULE,           // 样式规则 (.box { width: 100px; })
    STYLE_PROPERTY,       // 样式属性 (width: 100px;)
    CSS_SELECTOR,         // CSS选择器 (.box, #id, &:hover)
    
    // 脚本相关
    SCRIPT_BLOCK,         // 局部脚本块 (script { })
    
    // 模板系统
    TEMPLATE_ELEMENT,     // 元素模板 ([Template] @Element Box)
    TEMPLATE_STYLE,       // 样式组模板 ([Template] @Style DefaultText)
    TEMPLATE_VAR,         // 变量组模板 ([Template] @Var Colors)
    
    // 自定义系统
    CUSTOM_ELEMENT,       // 自定义元素 ([Custom] @Element Button)
    CUSTOM_STYLE,         // 自定义样式组 ([Custom] @Style Theme)
    CUSTOM_VAR,           // 自定义变量组 ([Custom] @Var Sizes)
    
    // 导入系统
    IMPORT_HTML,          // HTML导入 ([Import] @Html from "file.html" as name)
    IMPORT_STYLE,         // CSS导入 ([Import] @Style from "file.css" as name)
    IMPORT_JAVASCRIPT,    // JS导入 ([Import] @JavaScript from "file.js" as name)
    IMPORT_CHTL,          // CHTL导入 ([Import] @Chtl from "file.chtl")
    IMPORT_CJMOD,         // CJMOD导入 ([Import] @CJmod from "module")
    IMPORT_SPECIFIC,      // 特定导入 ([Import] [Custom] @Element Button from "file.chtl")
    
    // 命名空间
    NAMESPACE,            // 命名空间 ([Namespace] UI.Components)
    
    // 配置
    CONFIGURATION,        // 配置 ([Configuration] { ... })
    CONFIG_OPTION,        // 配置选项
    
    // 原始嵌入
    ORIGIN_BLOCK,         // 原始嵌入块 ([Origin] @Html { ... })
    
    // 变量组
    VAR_GROUP,            // 变量组 (@Var Colors { ... })
    VAR_DECLARATION,      // 变量声明 (primary: #007bff;)
    VAR_REFERENCE,        // 变量引用 ($primary)
    
    // 特殊化
    SPECIALIZATION,       // 特例化 (Box<string> { ... })
    
    // 继承
    INHERITANCE,          // 继承 (from ParentStyle)
    
    // 字面量
    LITERAL,              // 字面量 (支持无修饰字符串)
    
    // 列表
    NODE_LIST             // 节点列表
};

// 节点类型转换为字符串
const char* nodeTypeToString(NodeType type);

} // namespace AST
} // namespace CHTL