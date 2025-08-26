#pragma once

#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSContext/CHTLJSContext.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace Parser {

/**
 * CHTLJSParser - CHTL JS语法解析器
 * 将CHTL JS Token序列解析为AST
 */
class CHTLJSParser {
public:
    /**
     * 解析结果
     */
    struct ParseResult {
        bool success;
        std::shared_ptr<Node::CHTLJSNode> root_node;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ParseResult() : success(false) {}
    };

private:
    std::vector<Lexer::CHTLJSToken> tokens_;
    size_t current_token_index_;
    std::shared_ptr<Context::CHTLJSContext> context_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
public:
    /**
     * 构造函数
     * @param tokens Token列表
     * @param context CHTL JS上下文
     */
    CHTLJSParser(const std::vector<Lexer::CHTLJSToken>& tokens,
                 std::shared_ptr<Context::CHTLJSContext> context);
    
    /**
     * 解析Token序列
     * @return 解析结果
     */
    ParseResult parse();
    
    /**
     * 获取错误列表
     * @return 错误列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * 获取警告列表
     * @return 警告列表
     */
    const std::vector<std::string>& getWarnings() const;

private:
    /**
     * 获取当前Token
     * @return 当前Token
     */
    const Lexer::CHTLJSToken& getCurrentToken() const;
    
    /**
     * 前进到下一个Token
     */
    void nextToken();
    
    /**
     * 检查是否为指定Token类型
     * @param type Token类型
     * @return 是否匹配
     */
    bool isCurrentToken(Lexer::CHTLJSTokenType type) const;
    
    /**
     * 消费指定Token类型
     * @param type Token类型
     * @return 是否成功
     */
    bool consumeToken(Lexer::CHTLJSTokenType type);
    
    /**
     * 解析CHTL JS脚本
     * @return 根节点
     */
    std::shared_ptr<Node::CHTLJSNode> parseScript();
    
    /**
     * 解析增强选择器
     * @return 增强选择器节点
     */
    std::shared_ptr<Node::CHTLJSNode> parseEnhancedSelector();
    
    /**
     * 解析vir对象声明
     * @return vir对象节点
     */
    std::shared_ptr<Node::CHTLJSNode> parseVirObject();
    
    /**
     * 解析listen函数（新语法）
     * @return listen节点
     */
    std::shared_ptr<Node::CHTLJSNode> parseListenFunction();
    
    /**
     * 解析delegate函数（新语法）
     * @return delegate节点
     */
    std::shared_ptr<Node::CHTLJSNode> parseDelegateFunction();
    
    /**
     * 解析animate函数（新语法）
     * @return animate节点
     */
    std::shared_ptr<Node::CHTLJSNode> parseAnimateFunction();
    
    /**
     * 解析函数块（新语法 - 去掉括号）
     * @return 函数块内容
     */
    std::unordered_map<std::string, std::string> parseFunctionBlock();
    
    /**
     * 解析动画when数组
     * @return when数组内容
     */
    std::vector<std::unordered_map<std::string, std::string>> parseWhenArray();
    
    /**
     * 解析CSS块（begin, end等）
     * @return CSS块内容
     */
    std::string parseCSSBlock();
    
    /**
     * 解析表达式
     * @return 表达式节点
     */
    std::shared_ptr<Node::CHTLJSNode> parseExpression();
    
    /**
     * 解析纯JavaScript代码
     * @return JavaScript节点
     */
    std::shared_ptr<Node::CHTLJSNode> parseJavaScript();
    
    /**
     * 添加错误
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * 添加警告
     * @param warning 警告信息
     */
    void addWarning(const std::string& warning);
};

} // namespace Parser
} // namespace CHTLJS
} // namespace CHTL