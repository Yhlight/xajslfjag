/**
 * CHTL词法分析器实现
 * 
 * 实现CHTLLexer类的所有方法
 * 严格按照CHTL语法文档进行词法分析
 * 
 * @author CHTL Team
 * @version 1.0.0
 * @license MIT
 */

#include "CHTLLexer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL {

// 构造函数
CHTLLexer::CHTLLexer()
    : m_inputStream(nullptr)
    , m_useStringSource(false)
    , m_currentPosition(0)
    , m_currentLine(1)
    , m_currentColumn(1)
    , m_startPosition(0)
    , m_startLine(1)
    , m_startColumn(1)
    , m_currentTokenIndex(0)
    , m_globalMap(GlobalMap::getInstance()) {
    initialize();
}

CHTLLexer::CHTLLexer(const std::string& source)
    : m_source(source)
    , m_inputStream(nullptr)
    , m_useStringSource(true)
    , m_currentPosition(0)
    , m_currentLine(1)
    , m_currentColumn(1)
    , m_startPosition(0)
    , m_startLine(1)
    , m_startColumn(1)
    , m_currentTokenIndex(0)
    , m_globalMap(GlobalMap::getInstance()) {
    initialize();
}

CHTLLexer::CHTLLexer(std::istream& input)
    : m_inputStream(&input)
    , m_useStringSource(false)
    , m_currentPosition(0)
    , m_currentLine(1)
    , m_currentColumn(1)
    , m_startPosition(0)
    , m_startLine(1)
    , m_startColumn(1)
    , m_currentTokenIndex(0)
    , m_globalMap(GlobalMap::getInstance()) {
    initialize();
}

// 初始化
void CHTLLexer::initialize() {
    m_globalMap.initialize();
    reset();
}

// 设置输入源
void CHTLLexer::setSource(const std::string& source) {
    m_source = source;
    m_useStringSource = true;
    m_inputStream = nullptr;
    reset();
}

void CHTLLexer::setInput(std::istream& input) {
    m_inputStream = &input;
    m_useStringSource = false;
    reset();
}

// 词法分析主函数
std::vector<std::shared_ptr<Token>> CHTLLexer::tokenize() {
    reset();
    
    while (!isAtEnd()) {
        auto token = recognizeToken();
        if (token) {
            m_tokens.push_back(token);
        }
    }
    
    // 添加文件结束Token
    auto eofToken = TokenFactory::createToken(TokenType::END_OF_FILE, "", m_currentLine, m_currentColumn);
    m_tokens.push_back(eofToken);
    
    return m_tokens;
}

// 获取下一个Token
std::shared_ptr<Token> CHTLLexer::getNextToken() {
    if (m_currentTokenIndex < m_tokens.size()) {
        return m_tokens[m_currentTokenIndex++];
    }
    return nullptr;
}

// 查看下一个Token（不消耗）
std::shared_ptr<Token> CHTLLexer::peekNextToken() {
    if (m_currentTokenIndex < m_tokens.size()) {
        return m_tokens[m_currentTokenIndex];
    }
    return nullptr;
}

// 重置词法分析器状态
void CHTLLexer::reset() {
    m_currentPosition = 0;
    m_currentLine = 1;
    m_currentColumn = 1;
    m_startPosition = 0;
    m_startLine = 1;
    m_startColumn = 1;
    m_currentTokenIndex = 0;
    m_tokens.clear();
    m_errors.clear();
}

// 字符读取方法
char CHTLLexer::getCurrentChar() const {
    if (m_useStringSource) {
        if (m_currentPosition < m_source.length()) {
            return m_source[m_currentPosition];
        }
    } else if (m_inputStream) {
        // 对于流输入，需要特殊处理
        return '\0'; // 简化实现
    }
    return '\0';
}

char CHTLLexer::getNextChar() {
    char ch = getCurrentChar();
    advance();
    return ch;
}

char CHTLLexer::peekNextChar() const {
    if (m_useStringSource) {
        if (m_currentPosition + 1 < m_source.length()) {
            return m_source[m_currentPosition + 1];
        }
    }
    return '\0';
}

char CHTLLexer::peekChar(size_t offset) const {
    if (m_useStringSource) {
        if (m_currentPosition + offset < m_source.length()) {
            return m_source[m_currentPosition + offset];
        }
    }
    return '\0';
}

bool CHTLLexer::isAtEnd() const {
    if (m_useStringSource) {
        return m_currentPosition >= m_source.length();
    }
    return false; // 简化实现
}

void CHTLLexer::advance() {
    if (m_useStringSource) {
        if (m_currentPosition < m_source.length()) {
            if (m_source[m_currentPosition] == '\n') {
                m_currentLine++;
                m_currentColumn = 1;
            } else {
                m_currentColumn++;
            }
            m_currentPosition++;
        }
    }
}

// Token识别主方法
std::shared_ptr<Token> CHTLLexer::recognizeToken() {
    if (isAtEnd()) {
        return nullptr;
    }
    
    saveTokenStart();
    
    char currentChar = getCurrentChar();
    
    // 跳过空白字符
    if (isWhitespace(currentChar)) {
        return recognizeWhitespace();
    }
    
    // 处理换行符
    if (isNewline(currentChar)) {
        return recognizeNewline();
    }
    
    // 处理注释
    if (currentChar == '/' && peekNextChar() == '/') {
        return recognizeComment();
    }
    if (currentChar == '/' && peekNextChar() == '*') {
        return recognizeComment();
    }
    if (currentChar == '-' && peekNextChar() == '-') {
        return recognizeComment();
    }
    
    // 处理字符串字面量
    if (currentChar == '"') {
        return recognizeStringLiteral();
    }
    if (currentChar == '\'') {
        return recognizeStringLiteral();
    }
    
    // 处理数字
    if (isDigit(currentChar)) {
        return recognizeNumberLiteral();
    }
    
    // 处理标识符和关键字
    if (isIdentifierStart(currentChar)) {
        return recognizeIdentifier();
    }
    
    // 处理操作符和分隔符
    if (m_globalMap.isOperator(std::string(1, currentChar))) {
        return recognizeOperator();
    }
    if (m_globalMap.isDelimiter(std::string(1, currentChar))) {
        return recognizeDelimiter();
    }
    
    // 处理特殊符号
    if (m_globalMap.isSpecialSymbol(std::string(1, currentChar))) {
        return recognizeSpecialSymbol();
    }
    
    // 未知字符
    addError("未知字符: " + std::string(1, currentChar));
    advance();
    return nullptr;
}

// 识别注释
std::shared_ptr<Token> CHTLLexer::recognizeComment() {
    char currentChar = getCurrentChar();
    
    if (currentChar == '/' && peekNextChar() == '/') {
        return processSingleLineComment();
    } else if (currentChar == '/' && peekNextChar() == '*') {
        return processMultiLineComment();
    } else if (currentChar == '-' && peekNextChar() == '-') {
        return processGeneratorComment();
    }
    
    return nullptr;
}

// 识别字符串字面量
std::shared_ptr<Token> CHTLLexer::recognizeStringLiteral() {
    char currentChar = getCurrentChar();
    
    if (currentChar == '"') {
        return processDoubleQuotedString();
    } else if (currentChar == '\'') {
        return processSingleQuotedString();
    }
    
    return nullptr;
}

// 识别无修饰字面量
std::shared_ptr<Token> CHTLLexer::recognizeUnquotedLiteral() {
    // 无修饰字面量的识别比较复杂，需要上下文分析
    // 这里简化实现
    return nullptr;
}

// 识别数字字面量
std::shared_ptr<Token> CHTLLexer::recognizeNumberLiteral() {
    char currentChar = getCurrentChar();
    
    if (currentChar == '0' && (peekNextChar() == 'x' || peekNextChar() == 'X')) {
        return processHexNumber();
    }
    
    // 尝试处理浮点数
    if (isDigit(currentChar)) {
        size_t startPos = m_currentPosition;
        advance(); // 跳过第一个数字
        
        // 继续读取数字
        while (!isAtEnd() && isDigit(getCurrentChar())) {
            advance();
        }
        
        // 检查是否有小数点
        if (!isAtEnd() && getCurrentChar() == '.') {
            advance(); // 跳过小数点
            // 继续读取小数部分
            while (!isAtEnd() && isDigit(getCurrentChar())) {
                advance();
            }
            return processFloat();
        }
        
        // 恢复位置并处理整数
        m_currentPosition = startPos;
        m_currentLine = m_startLine;
        m_currentColumn = m_startColumn;
        return processInteger();
    }
    
    return nullptr;
}

// 识别标识符
std::shared_ptr<Token> CHTLLexer::recognizeIdentifier() {
    return processIdentifier();
}

// 识别操作符
std::shared_ptr<Token> CHTLLexer::recognizeOperator() {
    return processIdentifier(); // 操作符作为标识符处理
}

// 识别分隔符
std::shared_ptr<Token> CHTLLexer::recognizeDelimiter() {
    char currentChar = getCurrentChar();
    std::string delimiter(1, currentChar);
    
    auto token = TokenFactory::createDelimiter(delimiter, m_startLine, m_startColumn);
    advance();
    return token;
}

// 识别特殊符号
std::shared_ptr<Token> CHTLLexer::recognizeSpecialSymbol() {
    char currentChar = getCurrentChar();
    std::string symbol(1, currentChar);
    
    auto token = TokenFactory::createSpecialToken(
        m_globalMap.getSpecialSymbolType(symbol), 
        symbol, 
        m_startLine, 
        m_startColumn
    );
    advance();
    return token;
}

// 识别关键字
std::shared_ptr<Token> CHTLLexer::recognizeKeyword() {
    return processKeyword();
}

// 识别空白字符
std::shared_ptr<Token> CHTLLexer::recognizeWhitespace() {
    skipWhitespace();
    return TokenFactory::createToken(TokenType::WHITESPACE, " ", m_startLine, m_startColumn);
}

// 识别换行符
std::shared_ptr<Token> CHTLLexer::recognizeNewline() {
    char currentChar = getCurrentChar();
    if (currentChar == '\n') {
        advance();
        return TokenFactory::createToken(TokenType::NEWLINE, "\n", m_startLine, m_startColumn);
    } else if (currentChar == '\r') {
        advance();
        if (!isAtEnd() && getCurrentChar() == '\n') {
            advance();
            return TokenFactory::createToken(TokenType::NEWLINE, "\r\n", m_startLine, m_startColumn);
        }
        return TokenFactory::createToken(TokenType::NEWLINE, "\r", m_startLine, m_startColumn);
    }
    return nullptr;
}

// 辅助方法实现
bool CHTLLexer::isWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\r';
}

bool CHTLLexer::isNewline(char ch) const {
    return ch == '\n' || ch == '\r';
}

bool CHTLLexer::isDigit(char ch) const {
    return std::isdigit(static_cast<unsigned char>(ch));
}

bool CHTLLexer::isLetter(char ch) const {
    return std::isalpha(static_cast<unsigned char>(ch));
}

bool CHTLLexer::isIdentifierStart(char ch) const {
    return isLetter(ch) || ch == '_';
}

bool CHTLLexer::isIdentifierPart(char ch) const {
    return isLetter(ch) || isDigit(ch) || ch == '_';
}

bool CHTLLexer::isHexDigit(char ch) const {
    return isDigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

// 注释处理方法
std::shared_ptr<Token> CHTLLexer::processSingleLineComment() {
    std::string comment = "//";
    advance(); // 跳过第一个/
    advance(); // 跳过第二个/
    
    while (!isAtEnd() && !isNewline(getCurrentChar())) {
        comment += getCurrentChar();
        advance();
    }
    
    return TokenFactory::createComment(comment, TokenType::SINGLE_LINE_COMMENT, m_startLine, m_startColumn);
}

std::shared_ptr<Token> CHTLLexer::processMultiLineComment() {
    std::string comment = "/*";
    advance(); // 跳过/
    advance(); // 跳过*
    
    while (!isAtEnd()) {
        if (getCurrentChar() == '*' && peekNextChar() == '/') {
            comment += "*/";
            advance(); // 跳过*
            advance(); // 跳过/
            break;
        }
        comment += getCurrentChar();
        advance();
    }
    
    return TokenFactory::createComment(comment, TokenType::MULTI_LINE_COMMENT, m_startLine, m_startColumn);
}

std::shared_ptr<Token> CHTLLexer::processGeneratorComment() {
    std::string comment = "--";
    advance(); // 跳过第一个-
    advance(); // 跳过第二个-
    
    while (!isAtEnd() && !isNewline(getCurrentChar())) {
        comment += getCurrentChar();
        advance();
    }
    
    return TokenFactory::createComment(comment, TokenType::GENERATOR_COMMENT, m_startLine, m_startColumn);
}

// 字符串处理方法
std::shared_ptr<Token> CHTLLexer::processDoubleQuotedString() {
    std::string value = "\"";
    advance(); // 跳过开始的双引号
    
    while (!isAtEnd() && getCurrentChar() != '"') {
        if (getCurrentChar() == '\\') {
            value += getCurrentChar();
            advance();
            if (!isAtEnd()) {
                value += getCurrentChar();
                advance();
            }
        } else {
            value += getCurrentChar();
            advance();
        }
    }
    
    if (!isAtEnd()) {
        value += getCurrentChar(); // 添加结束的双引号
        advance();
    }
    
    return TokenFactory::createStringLiteral(value, m_startLine, m_startColumn);
}

std::shared_ptr<Token> CHTLLexer::processSingleQuotedString() {
    std::string value = "'";
    advance(); // 跳过开始的单引号
    
    while (!isAtEnd() && getCurrentChar() != '\'') {
        if (getCurrentChar() == '\\') {
            value += getCurrentChar();
            advance();
            if (!isAtEnd()) {
                value += getCurrentChar();
                advance();
            }
        } else {
            value += getCurrentChar();
            advance();
        }
    }
    
    if (!isAtEnd()) {
        value += getCurrentChar(); // 添加结束的单引号
        advance();
    }
    
    return TokenFactory::createStringLiteral(value, m_startLine, m_startColumn);
}

// 数字处理方法
std::shared_ptr<Token> CHTLLexer::processInteger() {
    std::string value;
    
    while (!isAtEnd() && isDigit(getCurrentChar())) {
        value += getCurrentChar();
        advance();
    }
    
    return TokenFactory::createNumberLiteral(value, m_startLine, m_startColumn);
}

std::shared_ptr<Token> CHTLLexer::processFloat() {
    std::string value;
    
    while (!isAtEnd() && (isDigit(getCurrentChar()) || getCurrentChar() == '.')) {
        value += getCurrentChar();
        advance();
    }
    
    return TokenFactory::createNumberLiteral(value, m_startLine, m_startColumn);
}

std::shared_ptr<Token> CHTLLexer::processHexNumber() {
    std::string value = "0x";
    advance(); // 跳过0
    advance(); // 跳过x
    
    while (!isAtEnd() && isHexDigit(getCurrentChar())) {
        value += getCurrentChar();
        advance();
    }
    
    return TokenFactory::createNumberLiteral(value, m_startLine, m_startColumn);
}

// 标识符处理方法
std::shared_ptr<Token> CHTLLexer::processIdentifier() {
    std::string identifier;
    
    while (!isAtEnd() && isIdentifierPart(getCurrentChar())) {
        identifier += getCurrentChar();
        advance();
    }
    
    // 检查是否为关键字
    if (m_globalMap.isKeyword(identifier)) {
        return TokenFactory::createKeyword(identifier, m_startLine, m_startColumn);
    }
    
    return TokenFactory::createIdentifier(identifier, m_startLine, m_startColumn);
}

// 关键字识别方法
std::shared_ptr<Token> CHTLLexer::processKeyword() {
    return processIdentifier(); // 关键字作为标识符处理
}

// 错误处理方法
void CHTLLexer::addError(const std::string& error) {
    std::ostringstream oss;
    oss << "第" << m_currentLine << "行，第" << m_currentColumn << "列: " << error;
    m_errors.push_back(oss.str());
}

void CHTLLexer::addError(const std::string& error, size_t line, size_t column) {
    std::ostringstream oss;
    oss << "第" << line << "行，第" << column << "列: " << error;
    m_errors.push_back(oss.str());
}

// 位置管理方法
void CHTLLexer::updatePosition() {
    // 位置更新在advance()中处理
}

void CHTLLexer::saveTokenStart() {
    m_startPosition = m_currentPosition;
    m_startLine = m_currentLine;
    m_startColumn = m_currentColumn;
}

void CHTLLexer::restoreTokenStart() {
    m_currentPosition = m_startPosition;
    m_currentLine = m_startLine;
    m_currentColumn = m_startColumn;
}

// 工具方法
std::string CHTLLexer::extractString(size_t start, size_t end) const {
    if (m_useStringSource && start < m_source.length() && end <= m_source.length() && start < end) {
        return m_source.substr(start, end - start);
    }
    return "";
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(getCurrentChar())) {
        advance();
    }
}

bool CHTLLexer::matchString(const std::string& str) {
    if (m_useStringSource) {
        if (m_currentPosition + str.length() <= m_source.length()) {
            return m_source.substr(m_currentPosition, str.length()) == str;
        }
    }
    return false;
}

bool CHTLLexer::matchKeyword(const std::string& keyword) {
    return matchString(keyword);
}

// 调试信息方法
void CHTLLexer::printTokens() const {
    std::cout << "=== CHTL词法分析结果 ===" << std::endl;
    for (size_t i = 0; i < m_tokens.size(); ++i) {
        std::cout << "[" << i << "] " << m_tokens[i]->toString() << std::endl;
    }
    
    if (hasErrors()) {
        std::cout << "\n=== 错误信息 ===" << std::endl;
        for (const auto& error : m_errors) {
            std::cout << error << std::endl;
        }
    }
}

std::string CHTLLexer::getTokenInfo() const {
    std::ostringstream oss;
    oss << "CHTL词法分析结果:\n";
    oss << "Token总数: " << m_tokens.size() << "\n";
    oss << "当前行: " << m_currentLine << "\n";
    oss << "当前列: " << m_currentColumn << "\n";
    
    if (hasErrors()) {
        oss << "错误数量: " << m_errors.size() << "\n";
        for (const auto& error : m_errors) {
            oss << error << "\n";
        }
    }
    
    return oss.str();
}

} // namespace CHTL