#pragma once
#include "BaseNode.h"
#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * 原始嵌入类型枚举
 */
enum class OriginType {
    HTML,           // @Html
    STYLE,          // @Style
    JAVASCRIPT,     // @JavaScript
    CUSTOM          // 自定义类型 (如 @Vue, @Markdown 等)
};

/**
 * 原始嵌入节点
 * 表示CHTL中的[Origin]原始代码嵌入
 */
class OriginNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    OriginNode(OriginType type, const std::string& typeName = "", const std::string& name = "");
    
    /**
     * 析构函数
     */
    virtual ~OriginNode() = default;
    
    /**
     * 获取原始嵌入类型
     */
    OriginType getOriginType() const;
    
    /**
     * 获取类型名称
     */
    const std::string& getTypeName() const;
    
    /**
     * 设置类型名称
     */
    void setTypeName(const std::string& typeName);
    
    /**
     * 获取嵌入名称（如果有）
     */
    const std::string& getOriginName() const;
    
    /**
     * 设置嵌入名称
     */
    void setOriginName(const std::string& name);
    
    /**
     * 获取原始内容
     */
    const std::string& getContent() const;
    
    /**
     * 设置原始内容
     */
    void setContent(const std::string& content);
    
    /**
     * 添加内容行
     */
    void addContentLine(const std::string& line);
    
    /**
     * 检查是否为命名嵌入
     */
    bool isNamed() const;
    
    /**
     * 检查是否为自定义类型
     */
    bool isCustomType() const;
    
    /**
     * 获取生成器输出类型
     */
    std::string getGeneratorOutputType() const;
    
    // BaseNode 接口实现
    CHTLNodeType getNodeType() const { return CHTLNodeType::ORIGIN_HTML_NODE; }
    std::string toString() const override;
    NodePtr clone() const override;
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

private:
    OriginType m_originType;            // 原始嵌入类型
    std::string m_typeName;             // 类型名称 (@Html, @Style, @JavaScript, @Vue等)
    std::string m_originName;           // 嵌入名称（可选）
    std::string m_content;              // 原始内容
    
    /**
     * 根据类型确定节点类型
     */
    CHTLNodeType determineNodeType() const;
};

/**
 * HTML原始嵌入节点
 */
class OriginHtmlNode : public OriginNode {
public:
    OriginHtmlNode(const std::string& name = "") : OriginNode(OriginType::HTML, "@Html", name) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::ORIGIN_HTML_NODE; }
};

/**
 * 样式原始嵌入节点  
 */
class OriginStyleNode : public OriginNode {
public:
    OriginStyleNode(const std::string& name = "") : OriginNode(OriginType::STYLE, "@Style", name) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::ORIGIN_STYLE_NODE; }
};

/**
 * JavaScript原始嵌入节点
 */
class OriginJavaScriptNode : public OriginNode {
public:
    OriginJavaScriptNode(const std::string& name = "") : OriginNode(OriginType::JAVASCRIPT, "@JavaScript", name) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::ORIGIN_JAVASCRIPT_NODE; }
};

/**
 * 自定义原始嵌入节点
 */
class OriginCustomNode : public OriginNode {
public:
    OriginCustomNode(const std::string& typeName, const std::string& name = "") 
        : OriginNode(OriginType::CUSTOM, typeName, name) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::ORIGIN_CUSTOM_NODE; }
};

/**
 * 原始嵌入使用节点
 * 表示对已定义原始嵌入的引用使用
 */
class OriginUsageNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    OriginUsageNode(const std::string& typeName, const std::string& name);
    
    /**
     * 析构函数
     */
    virtual ~OriginUsageNode() = default;
    
    /**
     * 获取类型名称
     */
    const std::string& getTypeName() const;
    
    /**
     * 获取引用名称
     */
    const std::string& getReferenceName() const;
    
    /**
     * 设置全缀名标记
     */
    void setFullyQualified(bool fullyQualified);
    
    /**
     * 检查是否为全缀名访问
     */
    bool isFullyQualified() const;
    
    // BaseNode 接口实现
    CHTLNodeType getNodeType() const { return CHTLNodeType::ORIGIN_HTML_NODE; } // 运行时确定
    std::string toString() const override;
    NodePtr clone() const override;
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

private:
    std::string m_typeName;             // 类型名称
    std::string m_referenceName;        // 引用名称
    bool m_fullyQualified;              // 是否为全缀名访问
};

} // namespace CHTL