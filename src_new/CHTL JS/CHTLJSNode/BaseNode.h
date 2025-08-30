#pragma once
#include "../CHTLJSLexer/Token.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS节点类型枚举
 * 严格按照CHTL语法文档中的CHTL JS语法定义
 */
enum class CHTLJSNodeType {
    // 基础节点
    ROOT_NODE,                  // 根节点
    SCRIPT_NODE,                // script块节点
    MODULE_NODE,                // module块节点
    
    // CHTL JS特有节点
    ENHANCED_SELECTOR_NODE,     // 增强选择器节点 {{...}}
    EVENT_LISTENER_NODE,        // 事件监听器节点 listen
    EVENT_DELEGATE_NODE,        // 事件委托节点 delegate
    ANIMATION_NODE,             // 动画节点 animate
    VIRTUAL_OBJECT_NODE,        // 虚拟对象节点 vir
    EVENT_BINDING_NODE,         // 事件绑定节点 &->
    
    // CHTL JS函数节点
    CHTLJS_FUNCTION_NODE,       // CHTL JS函数节点
    FUNCTION_PARAM_NODE,        // 函数参数节点
    
    // JavaScript兼容节点
    JS_EXPRESSION_NODE,         // JS表达式节点
    JS_STATEMENT_NODE,          // JS语句节点
    
    // 未知节点
    UNKNOWN_NODE
};

/**
 * CHTL JS基础节点类
 * 所有CHTL JS AST节点的基类
 */
class BaseNode {
public:
    CHTLJSNodeType nodeType;        // 节点类型
    std::string content;            // 节点内容
    Position position;              // 位置信息
    std::vector<std::shared_ptr<BaseNode>> children; // 子节点
    
    BaseNode(CHTLJSNodeType type = CHTLJSNodeType::UNKNOWN_NODE,
             const std::string& cont = "",
             const Position& pos = Position())
        : nodeType(type), content(cont), position(pos) {}
    
    virtual ~BaseNode() = default;
    
    // ========== 节点操作 ==========
    
    /**
     * 添加子节点
     */
    void addChild(std::shared_ptr<BaseNode> child);
    
    /**
     * 获取子节点
     */
    std::shared_ptr<BaseNode> getChild(size_t index) const;
    
    /**
     * 获取子节点数量
     */
    size_t getChildCount() const;
    
    // ========== 节点信息 ==========
    
    /**
     * 获取节点类型名称
     */
    std::string getNodeTypeName() const;
    
    /**
     * 检查是否为CHTL JS特有节点
     */
    bool isCHTLJSSpecificNode() const;
    
    /**
     * 检查是否为事件相关节点
     */
    bool isEventNode() const;
    
    // ========== 虚函数接口 ==========
    
    /**
     * 生成JavaScript代码
     */
    virtual std::string generateJS() const;
    
    /**
     * 验证节点
     */
    virtual bool validate() const;
    
    /**
     * 获取节点描述
     */
    virtual std::string toString() const;
    
    /**
     * 克隆节点
     */
    virtual std::shared_ptr<BaseNode> clone() const;

protected:
    /**
     * 报告节点错误
     */
    void reportError(const std::string& message) const;
};

/**
 * 节点智能指针类型定义
 */
using NodePtr = std::shared_ptr<BaseNode>;

/**
 * 节点工厂类
 */
class NodeFactory {
public:
    /**
     * 创建指定类型的节点
     */
    static NodePtr createNode(CHTLJSNodeType type, const std::string& content = "");
    
    /**
     * 从Token创建节点
     */
    static NodePtr createNodeFromToken(const Token& token);
};

} // namespace CHTLJS