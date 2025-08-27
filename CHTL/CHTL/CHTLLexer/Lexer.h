#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Token.h"
#include "GlobalMap.h"

namespace CHTL {

/**
 * @brief CHTL词法分析器
 * 负责将CHTL源代码分解为Token序列
 */
class Lexer {
private:
    std::string source_;           // 源代码
    size_t current_;              // 当前位置
    size_t line_;                 // 当前行号
    size_t column_;               // 当前列号
    GlobalMapPtr global_map_;     // 全局映射表
    TokenList tokens_;            // 生成的Token列表
    bool ignore_whitespace_;      // 是否忽略空白字符
    bool ignore_comments_;        // 是否忽略注释

public:
    /**
     * @brief 构造函数
     * @param source 源代码字符串
     * @param global_map 全局映射表
     */
    explicit Lexer(const std::string& source, GlobalMapPtr global_map = nullptr);

    /**
     * @brief 析构函数
     */
    ~Lexer() = default;

    /**
     * @brief 执行词法分析
     * @return 生成的Token列表
     */
    TokenList tokenize();

    /**
     * @brief 获取生成的Token列表
     * @return Token列表的常量引用
     */
    const TokenList& getTokens() const { return tokens_; }

    /**
     * @brief 设置是否忽略空白字符
     * @param ignore 是否忽略
     */
    void setIgnoreWhitespace(bool ignore) { ignore_whitespace_ = ignore; }

    /**
     * @brief 设置是否忽略注释
     * @param ignore 是否忽略
     */
    void setIgnoreComments(bool ignore) { ignore_comments_ = ignore; }

    /**
     * @brief 重置词法分析器状态
     * @param source 新的源代码
     */
    void reset(const std::string& source);

    /**
     * @brief 获取当前位置信息
     * @return 位置信息
     */
    Position getCurrentPosition() const;

    /**
     * @brief 打印Token列表（调试用）
     */
    void printTokens() const;

private:
    /**
     * @brief 检查是否到达源代码末尾
     * @return 如果到达末尾返回true
     */
    bool isAtEnd() const;

    /**
     * @brief 获取当前字符
     * @return 当前字符
     */
    char peek() const;

    /**
     * @brief 获取下一个字符
     * @return 下一个字符
     */
    char peekNext() const;

    /**
     * @brief 消费当前字符并前进
     * @return 被消费的字符
     */
    char advance();

    /**
     * @brief 检查当前字符是否匹配期望字符
     * @param expected 期望的字符
     * @return 匹配返回true并前进，否则返回false
     */
    bool match(char expected);

    /**
     * @brief 添加Token到列表
     * @param type Token类型
     * @param value Token值
     * @param start_pos 起始位置
     */
    void addToken(TokenType type, const std::string& value, const Position& start_pos);

    /**
     * @brief 跳过空白字符
     */
    void skipWhitespace();

    /**
     * @brief 扫描字符串字面量
     * @param quote 引号字符(' 或 ")
     * @return 字符串Token
     */
    TokenPtr scanString(char quote);

    /**
     * @brief 扫描数字字面量
     * @return 数字Token
     */
    TokenPtr scanNumber();

    /**
     * @brief 扫描标识符或关键字
     * @return 标识符或关键字Token
     */
    TokenPtr scanIdentifier();

    /**
     * @brief 扫描无修饰字面量
     * @return 字面量Token
     */
    TokenPtr scanLiteral();

    /**
     * @brief 扫描单行注释
     * @return 注释Token
     */
    TokenPtr scanSingleLineComment();

    /**
     * @brief 扫描多行注释
     * @return 注释Token
     */
    TokenPtr scanMultiLineComment();

    /**
     * @brief 扫描生成器注释
     * @return 生成器注释Token
     */
    TokenPtr scanGeneratorComment();

    /**
     * @brief 扫描块类型标识符（如[Template]）
     * @return 块类型Token
     */
    TokenPtr scanBlockType();

    /**
     * @brief 扫描类型标识符（如@Style）
     * @return 类型标识符Token
     */
    TokenPtr scanTypeIdentifier();

    /**
     * @brief 判断字符是否为字母
     * @param c 字符
     * @return 是字母返回true
     */
    bool isAlpha(char c) const;

    /**
     * @brief 判断字符是否为数字
     * @param c 字符
     * @return 是数字返回true
     */
    bool isDigit(char c) const;

    /**
     * @brief 判断字符是否为字母或数字
     * @param c 字符
     * @return 是字母或数字返回true
     */
    bool isAlphaNumeric(char c) const;

    /**
     * @brief 判断字符是否为空白字符
     * @param c 字符
     * @return 是空白字符返回true
     */
    bool isWhitespace(char c) const;

    /**
     * @brief 判断字符是否可以作为标识符的开始
     * @param c 字符
     * @return 可以返回true
     */
    bool isIdentifierStart(char c) const;

    /**
     * @brief 判断字符是否可以作为标识符的一部分
     * @param c 字符
     * @return 可以返回true
     */
    bool isIdentifierPart(char c) const;

    /**
     * @brief 处理特殊的组合关键字（如"at top", "at bottom"）
     * @param current_token 当前Token
     * @return 处理后的Token
     */
    TokenPtr handleCombinedKeywords(TokenPtr current_token);

    /**
     * @brief 错误处理
     * @param message 错误消息
     */
    void error(const std::string& message) const;
};

/**
 * @brief Lexer智能指针类型定义
 */
using LexerPtr = std::shared_ptr<Lexer>;

} // namespace CHTL