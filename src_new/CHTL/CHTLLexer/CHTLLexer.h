#pragma once

#include "Token.h"
#include "GlobalMap.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace Lexer {

/**
 * CHTLLexer - CHTL词法分析器
 * 新目录结构下的核心词法分析器
 */
class CHTLLexer {
private:
    std::string source_code_;
    std::string file_path_;
    size_t current_position_;
    size_t line_number_;
    size_t column_number_;
    std::vector<Token> tokens_;
    std::shared_ptr<GlobalMap> global_map_;
    
public:
    /**
     * 构造函数
     * @param source_code 源代码
     * @param file_path 文件路径
     * @param global_map 全局映射
     */
    CHTLLexer(const std::string& source_code, 
              const std::string& file_path = "",
              std::shared_ptr<GlobalMap> global_map = nullptr);
    
    /**
     * 执行词法分析
     * @return 是否成功
     */
    bool tokenize();
    
    /**
     * 获取Token列表
     * @return Token列表
     */
    const std::vector<Token>& getTokens() const;
    
    /**
     * 获取错误信息
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * 获取警告信息
     * @return 警告信息列表
     */
    const std::vector<std::string>& getWarnings() const;

private:
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    /**
     * 读取下一个字符
     * @return 字符
     */
    char nextChar();
    
    /**
     * 预览字符
     * @param offset 偏移量
     * @return 字符
     */
    char peekChar(size_t offset = 0) const;
    
    /**
     * 是否到达文件末尾
     * @return 是否EOF
     */
    bool isEOF() const;
    
    /**
     * 跳过空白字符
     */
    void skipWhitespace();
    
    /**
     * 读取标识符
     * @return Token
     */
    Token readIdentifier();
    
    /**
     * 读取字符串字面量
     * @param quote 引号字符
     * @return Token
     */
    Token readStringLiteral(char quote);
    
    /**
     * 读取数字
     * @return Token
     */
    Token readNumber();
    
    /**
     * 读取注释
     * @return Token
     */
    Token readComment();
    
    /**
     * 读取操作符
     * @return Token
     */
    Token readOperator();
    
    /**
     * 识别关键字
     * @param identifier 标识符
     * @return Token类型
     */
    TokenType identifyKeyword(const std::string& identifier) const;
    
    /**
     * 创建Token
     * @param type Token类型
     * @param value Token值
     * @return Token
     */
    Token createToken(TokenType type, const std::string& value = "");
    
    /**
     * 添加错误
     * @param message 错误信息
     */
    void addError(const std::string& message);
    
    /**
     * 添加警告
     * @param message 警告信息
     */
    void addWarning(const std::string& message);
};

} // namespace Lexer
} // namespace CHTL