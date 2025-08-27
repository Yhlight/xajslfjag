#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include "../CHTLLexer/Token.h"
#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/CHTLContext.h"

namespace CHTL {

/**
 * @brief 解析错误类型枚举
 */
enum class ParseErrorType {
    UNEXPECTED_TOKEN,   // 意外的Token
    MISSING_TOKEN,      // 缺少Token
    INVALID_SYNTAX,     // 无效语法
    SEMANTIC_ERROR,     // 语义错误
    UNKNOWN_ERROR       // 未知错误
};

/**
 * @brief 解析错误结构
 */
struct ParseError {
    ParseErrorType type;
    std::string message;
    Position position;
    TokenPtr token;
    
    ParseError(ParseErrorType t, const std::string& msg, const Position& pos, TokenPtr tok = nullptr)
        : type(t), message(msg), position(pos), token(tok) {}
};

/**
 * @brief CHTL语法解析器
 * 将Token序列解析为抽象语法树(AST)
 */
class CHTLParser {
private:
    TokenList tokens_;                          // Token列表
    size_t current_;                            // 当前Token索引
    CHTLContextPtr context_;                    // 编译上下文
    std::vector<ParseError> errors_;            // 解析错误列表
    NodePtr root_;                             // 根节点
    bool panic_mode_;                          // 恐慌模式（错误恢复）

public:
    /**
     * @brief 构造函数
     * @param tokens Token列表
     * @param context 编译上下文
     */
    CHTLParser(const TokenList& tokens, CHTLContextPtr context);

    /**
     * @brief 析构函数
     */
    ~CHTLParser() = default;

    /**
     * @brief 执行解析
     * @return 解析得到的根节点
     */
    NodePtr parse();

    /**
     * @brief 获取解析错误
     * @return 错误列表
     */
    const std::vector<ParseError>& getErrors() const { return errors_; }

    /**
     * @brief 检查是否有解析错误
     * @return 是否有错误
     */
    bool hasErrors() const { return !errors_.empty(); }

    /**
     * @brief 重置解析器状态
     * @param tokens 新的Token列表
     */
    void reset(const TokenList& tokens);

    /**
     * @brief 获取根节点
     * @return 根节点
     */
    NodePtr getRoot() const { return root_; }

private:
    /**
     * @brief 检查是否到达Token序列末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;

    /**
     * @brief 获取当前Token
     * @return 当前Token
     */
    TokenPtr peek() const;

    /**
     * @brief 获取前一个Token
     * @return 前一个Token
     */
    TokenPtr previous() const;

    /**
     * @brief 获取下一个Token
     * @return 下一个Token
     */
    TokenPtr peekNext() const;

    /**
     * @brief 前进到下一个Token
     * @return 当前Token
     */
    TokenPtr advance();

    /**
     * @brief 检查当前Token是否匹配指定类型
     * @param type Token类型
     * @return 是否匹配
     */
    bool check(TokenType type) const;

    /**
     * @brief 匹配并消费Token
     * @param types Token类型列表
     * @return 是否匹配任一类型
     */
    bool match(std::initializer_list<TokenType> types);

    /**
     * @brief 消费指定类型的Token
     * @param type 期望的Token类型
     * @param error_message 错误消息
     * @return 消费的Token
     */
    TokenPtr consume(TokenType type, const std::string& error_message);

    /**
     * @brief 添加解析错误
     * @param type 错误类型
     * @param message 错误消息
     * @param token 相关Token
     */
    void addError(ParseErrorType type, const std::string& message, TokenPtr token = nullptr);

    /**
     * @brief 错误恢复：跳到下一个同步点
     */
    void synchronize();

    /**
     * @brief 检查Token是否为同步点
     * @param token Token
     * @return 是否为同步点
     */
    bool isSynchronizationPoint(TokenPtr token) const;

    // 解析方法
    /**
     * @brief 解析程序（顶层）
     * @return 程序节点
     */
    NodePtr parseProgram();

    /**
     * @brief 解析声明
     * @return 声明节点
     */
    NodePtr parseDeclaration();

    /**
     * @brief 解析元素声明
     * @return 元素节点
     */
    NodePtr parseElementDeclaration();

    /**
     * @brief 解析文本块
     * @return 文本节点
     */
    NodePtr parseTextBlock();

    /**
     * @brief 解析样式块
     * @return 样式节点
     */
    NodePtr parseStyleBlock();

    /**
     * @brief 解析脚本块
     * @return 脚本节点
     */
    NodePtr parseScriptBlock();

    /**
     * @brief 解析模板声明
     * @return 模板节点
     */
    NodePtr parseTemplateDeclaration();

    /**
     * @brief 解析自定义声明
     * @return 自定义节点
     */
    NodePtr parseCustomDeclaration();

    /**
     * @brief 解析原始嵌入声明
     * @return 原始嵌入节点
     */
    NodePtr parseOriginDeclaration();

    /**
     * @brief 解析导入声明
     * @return 导入节点
     */
    NodePtr parseImportDeclaration();

    /**
     * @brief 解析配置声明
     * @return 配置节点
     */
    NodePtr parseConfigurationDeclaration();

    /**
     * @brief 解析命名空间声明
     * @return 命名空间节点
     */
    NodePtr parseNamespaceDeclaration();

    /**
     * @brief 解析use语句
     * @return use节点
     */
    NodePtr parseUseStatement();

    /**
     * @brief 解析注释
     * @return 注释节点
     */
    NodePtr parseComment();

    /**
     * @brief 解析属性列表
     * @param target_node 目标节点
     * @return 是否成功解析
     */
    bool parseAttributes(NodePtr target_node);

    /**
     * @brief 解析属性
     * @param target_node 目标节点
     * @return 是否成功解析
     */
    bool parseAttribute(NodePtr target_node);

    /**
     * @brief 解析属性值
     * @return 属性值字符串
     */
    std::string parseAttributeValue();

    /**
     * @brief 解析块内容
     * @param parent_node 父节点
     * @return 是否成功解析
     */
    bool parseBlockContent(NodePtr parent_node);

    /**
     * @brief 解析标识符
     * @return 标识符字符串
     */
    std::string parseIdentifier();

    /**
     * @brief 解析字符串字面量
     * @return 字符串值
     */
    std::string parseStringLiteral();

    /**
     * @brief 解析数字字面量
     * @return 数字值
     */
    std::string parseNumberLiteral();

    /**
     * @brief 解析字面量
     * @return 字面量值
     */
    std::string parseLiteral();

    /**
     * @brief 跳过空白和注释Token
     */
    void skipWhitespaceAndComments();

    /**
     * @brief 验证语法规则
     * @param node 节点
     * @return 是否有效
     */
    bool validateSyntax(NodePtr node);

    /**
     * @brief 创建错误恢复节点
     * @param error_message 错误消息
     * @return 错误节点
     */
    NodePtr createErrorNode(const std::string& error_message);
};

/**
 * @brief CHTLParser智能指针类型定义
 */
using CHTLParserPtr = std::shared_ptr<CHTLParser>;

} // namespace CHTL