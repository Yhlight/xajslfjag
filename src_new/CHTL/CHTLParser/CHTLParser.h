#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace Parser {

/**
 * CHTLParser - CHTL语法解析器
 * 将Token序列解析为AST
 */
class CHTLParser {
public:
    /**
     * 解析结果
     */
    struct ParseResult {
        bool success;
        std::shared_ptr<Node::BaseNode> root_node;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ParseResult() : success(false) {}
    };

private:
    std::vector<Lexer::Token> tokens_;
    size_t current_token_index_;
    std::shared_ptr<Context::CHTLContext> context_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
public:
    /**
     * 构造函数
     * @param tokens Token列表
     * @param context CHTL上下文
     */
    CHTLParser(const std::vector<Lexer::Token>& tokens,
               std::shared_ptr<Context::CHTLContext> context);
    
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
    const Lexer::Token& getCurrentToken() const;
    
    /**
     * 前进到下一个Token
     */
    void nextToken();
    
    /**
     * 检查是否为指定Token类型
     * @param type Token类型
     * @return 是否匹配
     */
    bool isCurrentToken(Lexer::TokenType type) const;
    
    /**
     * 消费指定Token类型
     * @param type Token类型
     * @return 是否成功
     */
    bool consumeToken(Lexer::TokenType type);
    
    /**
     * 解析CHTL文档
     * @return 根节点
     */
    std::shared_ptr<Node::BaseNode> parseDocument();
    
    /**
     * 解析命名空间声明
     * @return 命名空间节点
     */
    std::shared_ptr<Node::BaseNode> parseNamespace();
    
    /**
     * 解析导入声明
     * @return 导入节点
     */
    std::shared_ptr<Node::BaseNode> parseImport();
    
    /**
     * 解析配置声明
     * @return 配置节点
     */
    std::shared_ptr<Node::BaseNode> parseConfiguration();
    
    /**
     * 解析模板声明
     * @return 模板节点
     */
    std::shared_ptr<Node::BaseNode> parseTemplate();
    
    /**
     * 解析自定义声明
     * @return 自定义节点
     */
    std::shared_ptr<Node::BaseNode> parseCustom();
    
    /**
     * 解析HTML元素
     * @return 元素节点
     */
    std::shared_ptr<Node::BaseNode> parseElement();
    
    /**
     * 解析文本内容
     * @return 文本节点
     */
    std::shared_ptr<Node::BaseNode> parseText();
    
    /**
     * 解析样式块
     * @return 样式节点
     */
    std::shared_ptr<Node::BaseNode> parseStyle();
    
    /**
     * 解析脚本块
     * @return 脚本节点
     */
    std::shared_ptr<Node::BaseNode> parseScript();
    
    /**
     * 解析原始嵌入
     * @return 原始嵌入节点
     */
    std::shared_ptr<Node::BaseNode> parseOrigin();
    
    /**
     * 解析操作符
     * @return 操作符节点
     */
    std::shared_ptr<Node::BaseNode> parseOperator();
    
    /**
     * 解析注释
     * @return 注释节点
     */
    std::shared_ptr<Node::BaseNode> parseComment();
    
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
} // namespace CHTL