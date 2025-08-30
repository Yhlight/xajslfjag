#pragma once
#include "../CHTLJSLexer/Lexer.h"
#include "../CHTLJSNode/BaseNode.h"
#include "../CHTLJSState/State.h"
#include "../CHTLJSContext/Context.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS解析选项
 */
struct ParseOptions {
    bool enableModules = true;              // 是否解析模块
    bool enableEnhancedSelectors = true;    // 是否解析增强选择器
    bool enableEventListeners = true;       // 是否解析事件监听器
    bool enableEventDelegation = true;      // 是否解析事件委托
    bool enableAnimations = true;           // 是否解析动画
    bool enableVirtualObjects = true;       // 是否解析虚拟对象
    bool enableEventBinding = true;         // 是否解析事件绑定操作符
    bool strictMode = false;                // 严格模式
    size_t maxDepth = 1000;                // 最大嵌套深度
};

/**
 * CHTL JS解析结果
 */
struct ParseResult {
    NodePtr rootNode;                       // 根节点
    std::vector<NodePtr> modules;           // 模块节点
    std::vector<NodePtr> enhancedSelectors; // 增强选择器节点
    std::vector<NodePtr> eventListeners;   // 事件监听器节点
    std::vector<NodePtr> animations;        // 动画节点
    std::vector<NodePtr> virtualObjects;   // 虚拟对象节点
    bool success;                          // 是否成功
    size_t tokensProcessed;                // 处理的Token数量
    size_t nodesCreated;                   // 创建的节点数量
    
    ParseResult() : success(false), tokensProcessed(0), nodesCreated(0) {}
};

/**
 * CHTL JS语法解析器
 * 专门解析CHTL JS语法，完全独立于JS
 * 严格按照CHTL语法文档中的CHTL JS部分实现
 */
class CHTLJSParser {
public:
    /**
     * 构造函数
     */
    explicit CHTLJSParser(const ParseOptions& options = ParseOptions());
    
    /**
     * 析构函数
     */
    ~CHTLJSParser() = default;
    
    // 禁用拷贝和移动
    CHTLJSParser(const CHTLJSParser&) = delete;
    CHTLJSParser& operator=(const CHTLJSParser&) = delete;
    
    // ========== 解析入口 ==========
    
    /**
     * 解析Token序列
     */
    ParseResult parse(const TokenSequence& tokens);
    
    /**
     * 解析字符串
     */
    ParseResult parseString(const std::string& source);
    
    // ========== CHTL JS特有语法解析 ==========
    
    /**
     * 解析模块导入 module{}
     */
    NodePtr parseModule();
    
    /**
     * 解析增强选择器 {{CSS选择器}}
     */
    NodePtr parseEnhancedSelector();
    
    /**
     * 解析事件监听器 listen
     */
    NodePtr parseEventListener();
    
    /**
     * 解析事件委托 delegate
     */
    NodePtr parseEventDelegate();
    
    /**
     * 解析动画 animate
     */
    NodePtr parseAnimation();
    
    /**
     * 解析虚拟对象 vir
     */
    NodePtr parseVirtualObject();
    
    /**
     * 解析事件绑定操作符 &->
     */
    NodePtr parseEventBinding();
    
    /**
     * 解析CHTL JS函数语法
     */
    NodePtr parseCHTLJSFunction();

private:
    ParseOptions m_options;                 // 解析选项
    TokenSequence m_tokens;                 // Token序列
    size_t m_currentToken;                  // 当前Token位置
    CHTLJSStateMachine m_stateMachine;      // 状态机
    CHTLJSContext m_context;                // 上下文
    
    // ========== 解析辅助方法 ==========
    
    Token getCurrentToken() const;
    Token peekToken(size_t offset = 1) const;
    void advanceToken();
    bool matchToken(TokenType type);
    bool expectToken(TokenType type);
    
    // ========== 错误处理 ==========
    
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
};

} // namespace CHTLJS