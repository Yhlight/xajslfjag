#pragma once

#include "CHTLJSToken.h"
#include "CHTLJSGlobalMap.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace Lexer {

/**
 * CHTLJSLexer - CHTL JS词法分析器
 * 专门处理CHTL JS语法的词法分析
 */
class CHTLJSLexer {
private:
    std::string source_code_;
    std::string file_path_;
    size_t current_position_;
    size_t line_number_;
    size_t column_number_;
    std::vector<CHTLJSToken> tokens_;
    std::shared_ptr<CHTLJSGlobalMap> global_map_;
    
public:
    /**
     * 构造函数
     * @param source_code 源代码
     * @param file_path 文件路径
     * @param global_map 全局映射
     */
    CHTLJSLexer(const std::string& source_code, 
                const std::string& file_path = "",
                std::shared_ptr<CHTLJSGlobalMap> global_map = nullptr);
    
    /**
     * 执行词法分析
     * @return 是否成功
     */
    bool tokenize();
    
    /**
     * 获取Token列表
     * @return Token列表
     */
    const std::vector<CHTLJSToken>& getTokens() const;
    
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
     * 读取增强选择器 {{selector}}
     * @return Token
     */
    CHTLJSToken readEnhancedSelector();
    
    /**
     * 读取CHTL JS函数调用
     * @return Token
     */
    CHTLJSToken readCHTLJSFunction();
    
    /**
     * 读取vir对象声明
     * @return Token
     */
    CHTLJSToken readVirObject();
    
    /**
     * 读取标识符
     * @return Token
     */
    CHTLJSToken readIdentifier();
    
    /**
     * 读取字符串字面量
     * @param quote 引号字符
     * @return Token
     */
    CHTLJSToken readStringLiteral(char quote);
    
    /**
     * 读取数字
     * @return Token
     */
    CHTLJSToken readNumber();
    
    /**
     * 读取操作符
     * @return Token
     */
    CHTLJSToken readOperator();
    
    /**
     * 识别CHTL JS关键字
     * @param identifier 标识符
     * @return Token类型
     */
    CHTLJSTokenType identifyKeyword(const std::string& identifier) const;
    
    /**
     * 创建Token
     * @param type Token类型
     * @param value Token值
     * @return Token
     */
    CHTLJSToken createToken(CHTLJSTokenType type, const std::string& value = "");
    
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
} // namespace CHTLJS
} // namespace CHTL