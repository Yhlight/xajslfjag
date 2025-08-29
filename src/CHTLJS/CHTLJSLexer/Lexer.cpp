#include "Lexer.h"
#include <regex>
#include <algorithm>
#include <cctype>

namespace CHTL {
namespace JS {

// CHTLJSLexer实现
CHTLJSLexer::CHTLJSLexer(const String& source, const CHTLJSLexerConfig& config)
    : sourceCode(source), config(config), currentPosition(0), currentLine(1), 
      currentColumn(1), currentState(CHTLJSLexerState::NORMAL) {
    
    // 初始化全局映射
    CHTLJSGlobalMap::initialize();
    
    // 清空错误列表
    errors.clear();
    
    // 清空Token缓冲区
    while (!tokenBuffer.empty()) {
        tokenBuffer.pop();
    }
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    reset();
    
    try {
        while (hasMoreTokens()) {
            CHTLJSToken token = nextToken();
            
            if (token.type == CHTLJSTokenType::INVALID) {
                if (config.strictMode) {
                    addError("遇到无效Token: " + token.value);
                    break;
                }
                continue;  // 在非严格模式下跳过无效Token
            }
            
            // 根据配置决定是否跳过空白和注释
            if (config.skipWhitespace && token.type == CHTLJSTokenType::WHITESPACE) {
                continue;
            }
            if (config.skipComments && 
                (token.type == CHTLJSTokenType::COMMENT_SINGLE || token.type == CHTLJSTokenType::COMMENT_MULTI)) {
                continue;
            }
            
            tokens.push_back(token);
            
            // 检查Token数量限制
            if (tokens.size() >= config.maxTokens) {
                addError("Token数量超过限制: " + std::to_string(config.maxTokens));
                break;
            }
        }
        
        // 添加EOF Token
        tokens.push_back(createToken(CHTLJSTokenType::EOF_TOKEN, ""));
        
        // 优化Token序列
        if (!config.debugMode) {
            optimizeTokens(tokens);
        }
        
    } catch (const std::exception& e) {
        addError("词法分析异常: " + String(e.what()));
    }
    
    return tokens;
}

CHTLJSToken CHTLJSLexer::nextToken() {
    // 检查缓冲区
    if (!tokenBuffer.empty()) {
        CHTLJSToken token = tokenBuffer.front();
        tokenBuffer.pop();
        return token;
    }
    
    // 扫描新Token
    return scanNextToken();
}

CHTLJSToken CHTLJSLexer::peekToken(size_t offset) {
    // 确保缓冲区有足够的Token
    while (tokenBuffer.size() <= offset && hasMoreTokens()) {
        tokenBuffer.push(scanNextToken());
    }
    
    if (tokenBuffer.size() > offset) {
        // 转换queue为vector来支持随机访问
        std::vector<CHTLJSToken> temp;
        std::queue<CHTLJSToken> tempQueue = tokenBuffer;
        while (!tempQueue.empty()) {
            temp.push_back(tempQueue.front());
            tempQueue.pop();
        }
        return temp[offset];
    }
    
    return createToken(CHTLJSTokenType::EOF_TOKEN, "");
}

bool CHTLJSLexer::hasMoreTokens() const {
    return currentPosition < sourceCode.length();
}

Position CHTLJSLexer::getCurrentPosition() const {
    return {currentLine, currentColumn, currentPosition};
}

CHTLJSLexerState CHTLJSLexer::getCurrentState() const {
    return currentState;
}

bool CHTLJSLexer::isAtEnd() const {
    return currentPosition >= sourceCode.length();
}

size_t CHTLJSLexer::getRemainingLength() const {
    return sourceCode.length() - currentPosition;
}

const std::vector<CHTLJSLexerError>& CHTLJSLexer::getErrors() const {
    return errors;
}

bool CHTLJSLexer::hasErrors() const {
    return !errors.empty();
}

String CHTLJSLexer::getErrorSummary() const {
    if (errors.empty()) {
        return "没有错误";
    }
    
    String summary = "词法分析错误 (" + std::to_string(errors.size()) + " 个):\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        const auto& error = errors[i];
        summary += std::to_string(i + 1) + ". [" + 
                   std::to_string(error.position.line) + ":" +
                   std::to_string(error.position.column) + "] " +
                   error.message + "\n";
    }
    return summary;
}

void CHTLJSLexer::clearErrors() {
    errors.clear();
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenizeWithContext() {
    auto tokens = tokenize();
    
    // 为Token添加上下文信息
    for (size_t i = 0; i < tokens.size(); ++i) {
        // 这里可以添加上下文分析逻辑
        // 例如：识别函数调用、变量声明等
    }
    
    return tokens;
}

StringVector CHTLJSLexer::extractSelectors() {
    return CHTLJSSelectorMap::extractSelectors(sourceCode);
}

StringVector CHTLJSLexer::extractEventBindings() {
    StringVector bindings;
    std::regex eventPattern(R"(\{\{[^}]+\}\}\s*&->\s*(\w+))");
    std::sregex_iterator iter(sourceCode.begin(), sourceCode.end(), eventPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        bindings.push_back((*iter)[1].str());
    }
    
    return bindings;
}

StringVector CHTLJSLexer::extractModules() {
    StringVector modules;
    std::regex modulePattern(R"(module\s*\{\s*([^}]+)\s*\})");
    std::sregex_iterator iter(sourceCode.begin(), sourceCode.end(), modulePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        modules.push_back((*iter)[1].str());
    }
    
    return modules;
}

void CHTLJSLexer::reset() {
    currentPosition = 0;
    currentLine = 1;
    currentColumn = 1;
    currentState = CHTLJSLexerState::NORMAL;
    errors.clear();
    
    while (!tokenBuffer.empty()) {
        tokenBuffer.pop();
    }
    
    stateStack.clear();
}

void CHTLJSLexer::setState(CHTLJSLexerState newState) {
    currentState = newState;
}

String CHTLJSLexer::getCurrentLine() const {
    size_t lineStart = currentPosition;
    
    // 找到行开始
    while (lineStart > 0 && sourceCode[lineStart - 1] != '\n') {
        lineStart--;
    }
    
    // 找到行结束
    size_t lineEnd = currentPosition;
    while (lineEnd < sourceCode.length() && sourceCode[lineEnd] != '\n') {
        lineEnd++;
    }
    
    return sourceCode.substr(lineStart, lineEnd - lineStart);
}

String CHTLJSLexer::getSourceSubstring(size_t start, size_t length) const {
    if (start >= sourceCode.length()) return "";
    if (start + length > sourceCode.length()) {
        length = sourceCode.length() - start;
    }
    return sourceCode.substr(start, length);
}

// 私有方法实现
CHTLJSToken CHTLJSLexer::scanNextToken() {
    if (isAtEnd()) {
        return createToken(CHTLJSTokenType::EOF_TOKEN, "");
    }
    
    // 跳过空白字符（如果配置要求）
    if (config.skipWhitespace) {
        skipWhitespace();
        if (isAtEnd()) {
            return createToken(CHTLJSTokenType::EOF_TOKEN, "");
        }
    }
    
    char c = currentChar();
    
    // 根据当前状态处理
    switch (currentState) {
        case CHTLJSLexerState::IN_STRING:
            return scanString();
        case CHTLJSLexerState::IN_COMMENT_SINGLE:
        case CHTLJSLexerState::IN_COMMENT_MULTI:
            return scanComment();
        case CHTLJSLexerState::IN_SELECTOR:
            return scanSelector();
        case CHTLJSLexerState::IN_LISTEN_BLOCK:
            return scanListenBlock();
        case CHTLJSLexerState::IN_DELEGATE_BLOCK:
            return scanDelegateBlock();
        case CHTLJSLexerState::IN_ANIMATE_BLOCK:
            return scanAnimateBlock();
        case CHTLJSLexerState::IN_VIR_BLOCK:
            return scanVirBlock();
        case CHTLJSLexerState::IN_MODULE_BLOCK:
            return scanModuleBlock();
        case CHTLJSLexerState::IN_CJMOD_BLOCK:
            return scanCJMODBlock();
        default:
            break;
    }
    
    // 正常状态下的Token识别
    if (isAlpha(c) || c == '_' || c == '$') {
        return scanIdentifierOrKeyword();
    }
    
    if (isDigit(c)) {
        return scanNumber();
    }
    
    if (c == '"' || c == '\'' || c == '`') {
        return scanString();
    }
    
    if (c == '/' && peekChar() == '/') {
        return scanComment();
    }
    
    if (c == '/' && peekChar() == '*') {
        return scanComment();
    }
    
    // 检查CHTL JS特殊语法
    if (c == '{' && peekChar() == '{') {
        return scanSelector();
    }
    
    if (c == '&' && peekChar() == '-' && peekChar(2) == '>') {
        return scanEventBinding();
    }
    
    // 扫描操作符
    return scanOperator();
}

CHTLJSToken CHTLJSLexer::scanIdentifierOrKeyword() {
    size_t start = currentPosition;
    
    while (hasMoreTokens() && (isAlphaNumeric(currentChar()) || currentChar() == '_' || currentChar() == '$')) {
        advance();
    }
    
    String value = sourceCode.substr(start, currentPosition - start);
    
    // 检查是否为关键字
    if (CHTLJSKeywordMap::isKeyword(value)) {
        CHTLJSTokenType type = CHTLJSKeywordMap::getKeywordType(value);
        CHTLJSToken token = createToken(type, value, currentPosition - start);
        
        // 特殊处理CHTL JS关键字状态转换
        if (type == CHTLJSTokenType::LISTEN) {
            pushState(CHTLJSLexerState::IN_LISTEN_BLOCK);
        } else if (type == CHTLJSTokenType::DELEGATE) {
            pushState(CHTLJSLexerState::IN_DELEGATE_BLOCK);
        } else if (type == CHTLJSTokenType::ANIMATE) {
            pushState(CHTLJSLexerState::IN_ANIMATE_BLOCK);
        } else if (type == CHTLJSTokenType::VIR) {
            pushState(CHTLJSLexerState::IN_VIR_BLOCK);
        } else if (type == CHTLJSTokenType::MODULE) {
            pushState(CHTLJSLexerState::IN_MODULE_BLOCK);
        }
        
        return token;
    }
    
    // 检查是否为CJMOD关键字
    if (config.enableCJMODSupport && isCJMODKeyword(value)) {
        pushState(CHTLJSLexerState::IN_CJMOD_BLOCK);
        return createToken(CHTLJSTokenType::CJMOD_KEYWORD, value, currentPosition - start);
    }
    
    return createToken(CHTLJSTokenType::IDENTIFIER, value, currentPosition - start);
}

CHTLJSToken CHTLJSLexer::scanNumber() {
    size_t start = currentPosition;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    while (hasMoreTokens()) {
        char c = currentChar();
        
        if (isDigit(c)) {
            advance();
        } else if (c == '.' && !hasDecimal && !hasExponent) {
            hasDecimal = true;
            advance();
        } else if ((c == 'e' || c == 'E') && !hasExponent) {
            hasExponent = true;
            advance();
            if (hasMoreTokens() && (currentChar() == '+' || currentChar() == '-')) {
                advance();
            }
        } else {
            break;
        }
    }
    
    String value = sourceCode.substr(start, currentPosition - start);
    return createToken(CHTLJSTokenType::NUMBER, value, currentPosition - start);
}

CHTLJSToken CHTLJSLexer::scanString() {
    char quote = currentChar();
    advance();  // 跳过开始引号
    
    size_t start = currentPosition - 1;
    String value;
    value += quote;
    
    while (hasMoreTokens() && currentChar() != quote) {
        char c = currentChar();
        
        if (c == '\\' && hasMoreTokens()) {
            value += c;
            advance();
            if (hasMoreTokens()) {
                value += currentChar();
                advance();
            }
        } else if (c == '\n') {
            addError("字符串未正确结束");
            break;
        } else {
            value += c;
            advance();
        }
    }
    
    if (hasMoreTokens() && currentChar() == quote) {
        value += quote;
        advance();  // 跳过结束引号
    } else {
        addError("字符串未正确结束");
    }
    
    return createToken(CHTLJSTokenType::STRING, value, currentPosition - start);
}

CHTLJSToken CHTLJSLexer::scanOperator() {
    char c = currentChar();
    String op;
    op += c;
    
    // 尝试匹配多字符操作符
    if (hasMoreTokens()) {
        String twoChar = op + peekChar();
        if (CHTLJSOperatorMap::isOperator(twoChar)) {
            advance(2);
            
            // 尝试三字符操作符
            if (hasMoreTokens()) {
                String threeChar = twoChar + currentChar();
                if (CHTLJSOperatorMap::isOperator(threeChar)) {
                    advance();
                    return createToken(CHTLJSOperatorMap::getOperatorType(threeChar), threeChar, 3);
                }
            }
            
            return createToken(CHTLJSOperatorMap::getOperatorType(twoChar), twoChar, 2);
        }
    }
    
    advance();
    
    if (CHTLJSOperatorMap::isOperator(op)) {
        return createToken(CHTLJSOperatorMap::getOperatorType(op), op, 1);
    }
    
    // 处理单字符分隔符
    switch (c) {
        case '(': return createToken(CHTLJSTokenType::LEFT_PAREN, op, 1);
        case ')': return createToken(CHTLJSTokenType::RIGHT_PAREN, op, 1);
        case '{': return createToken(CHTLJSTokenType::LEFT_BRACE, op, 1);
        case '}': return createToken(CHTLJSTokenType::RIGHT_BRACE, op, 1);
        case '[': return createToken(CHTLJSTokenType::LEFT_BRACKET, op, 1);
        case ']': return createToken(CHTLJSTokenType::RIGHT_BRACKET, op, 1);
        case ';': return createToken(CHTLJSTokenType::SEMICOLON, op, 1);
        case ',': return createToken(CHTLJSTokenType::COMMA, op, 1);
        case '.': return createToken(CHTLJSTokenType::DOT, op, 1);
        case ':': return createToken(CHTLJSTokenType::COLON, op, 1);
        case '?': return createToken(CHTLJSTokenType::QUESTION, op, 1);
        default:
            addError("未识别的字符: " + op);
            return createToken(CHTLJSTokenType::INVALID, op, 1);
    }
}

CHTLJSToken CHTLJSLexer::scanComment() {
    size_t start = currentPosition;
    
    if (currentChar() == '/' && peekChar() == '/') {
        // 单行注释
        advance(2);
        while (hasMoreTokens() && currentChar() != '\n') {
            advance();
        }
        String value = sourceCode.substr(start, currentPosition - start);
        return createToken(CHTLJSTokenType::COMMENT_SINGLE, value, currentPosition - start);
    } else if (currentChar() == '/' && peekChar() == '*') {
        // 多行注释
        advance(2);
        while (hasMoreTokens()) {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(2);
                break;
            }
            advance();
        }
        String value = sourceCode.substr(start, currentPosition - start);
        return createToken(CHTLJSTokenType::COMMENT_MULTI, value, currentPosition - start);
    }
    
    return createErrorToken("无效的注释语法");
}

CHTLJSToken CHTLJSLexer::scanSelector() {
    if (currentChar() == '{' && peekChar() == '{') {
        advance(2);  // 跳过 {{
        
        size_t start = currentPosition;
        while (hasMoreTokens() && !(currentChar() == '}' && peekChar() == '}')) {
            advance();
        }
        
        if (hasMoreTokens() && currentChar() == '}' && peekChar() == '}') {
            String selector = sourceCode.substr(start, currentPosition - start);
            advance(2);  // 跳过 }}
            
            // 验证选择器
            if (validateSelector(selector)) {
                return createToken(CHTLJSTokenType::SELECTOR_START, "{{" + selector + "}}", 
                                 selector.length() + 4);
            } else {
                addError("无效的CSS选择器: " + selector);
                return createErrorToken("无效选择器");
            }
        } else {
            addError("选择器未正确结束");
            return createErrorToken("选择器语法错误");
        }
    }
    
    return createErrorToken("选择器扫描错误");
}

CHTLJSToken CHTLJSLexer::scanEventBinding() {
    if (matchString("&->")) {
        advance(3);
        return createToken(CHTLJSTokenType::EVENT_BIND, "&->", 3);
    }
    
    return createErrorToken("事件绑定扫描错误");
}

// 辅助方法实现
char CHTLJSLexer::currentChar() const {
    if (currentPosition >= sourceCode.length()) return '\0';
    return sourceCode[currentPosition];
}

char CHTLJSLexer::peekChar(size_t offset) const {
    size_t pos = currentPosition + offset;
    if (pos >= sourceCode.length()) return '\0';
    return sourceCode[pos];
}

void CHTLJSLexer::advance(size_t count) {
    for (size_t i = 0; i < count && currentPosition < sourceCode.length(); ++i) {
        if (sourceCode[currentPosition] == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
        currentPosition++;
    }
}

void CHTLJSLexer::skipWhitespace() {
    while (hasMoreTokens() && isWhitespace(currentChar())) {
        advance();
    }
}

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool CHTLJSLexer::matchString(const String& str) {
    if (currentPosition + str.length() > sourceCode.length()) {
        return false;
    }
    
    return sourceCode.substr(currentPosition, str.length()) == str;
}

void CHTLJSLexer::pushState(CHTLJSLexerState newState) {
    stateStack.push_back(currentState);
    currentState = newState;
}

void CHTLJSLexer::popState() {
    if (!stateStack.empty()) {
        currentState = stateStack.back();
        stateStack.pop_back();
    } else {
        currentState = CHTLJSLexerState::NORMAL;
    }
}

void CHTLJSLexer::addError(const String& message, const String& token) {
    errors.emplace_back(message, getCurrentPosition(), token, currentState);
}

CHTLJSToken CHTLJSLexer::createErrorToken(const String& message) {
    addError(message);
    return createToken(CHTLJSTokenType::INVALID, "", 0);
}

CHTLJSToken CHTLJSLexer::createToken(CHTLJSTokenType type, const String& value, size_t length) {
    Position pos = getCurrentTokenPosition();
    return CHTLJSToken(type, value, pos, length);
}

Position CHTLJSLexer::getCurrentTokenPosition() const {
    return {currentLine, currentColumn, currentPosition};
}

bool CHTLJSLexer::validateSelector(const String& selector) {
    return CHTLJSSelectorMap::isValidSelector(selector);
}

bool CHTLJSLexer::isCJMODKeyword(const String& word) {
    return CHTLJSKeywordMap::isCJMODKeyword(word);
}

void CHTLJSLexer::optimizeTokens(std::vector<CHTLJSToken>& tokens) {
    // 移除重复的空白Token
    removeDuplicateWhitespace(tokens);
    
    // 合并字符串字面量
    mergeStringLiterals(tokens);
}

void CHTLJSLexer::removeDuplicateWhitespace(std::vector<CHTLJSToken>& tokens) {
    auto it = std::unique(tokens.begin(), tokens.end(), 
        [](const CHTLJSToken& a, const CHTLJSToken& b) {
            return a.type == CHTLJSTokenType::WHITESPACE && 
                   b.type == CHTLJSTokenType::WHITESPACE;
        });
    tokens.erase(it, tokens.end());
}

void CHTLJSLexer::mergeStringLiterals(std::vector<CHTLJSToken>& tokens) {
    // 这里可以实现字符串合并逻辑
    // 暂时保持原样
}

// 简化实现的其他扫描方法
CHTLJSToken CHTLJSLexer::scanListenBlock() {
    // 简化实现，返回到正常状态
    popState();
    return scanNextToken();
}

CHTLJSToken CHTLJSLexer::scanDelegateBlock() {
    popState();
    return scanNextToken();
}

CHTLJSToken CHTLJSLexer::scanAnimateBlock() {
    popState();
    return scanNextToken();
}

CHTLJSToken CHTLJSLexer::scanVirBlock() {
    popState();
    return scanNextToken();
}

CHTLJSToken CHTLJSLexer::scanModuleBlock() {
    popState();
    return scanNextToken();
}

CHTLJSToken CHTLJSLexer::scanCJMODBlock() {
    popState();
    return scanNextToken();
}

// CHTLJSLexerFactory实现
std::unique_ptr<CHTLJSLexer> CHTLJSLexerFactory::createLexer(const String& source, const CHTLJSLexerConfig& config) {
    return std::make_unique<CHTLJSLexer>(source, config);
}

std::unique_ptr<CHTLJSLexer> CHTLJSLexerFactory::createStrictLexer(const String& source) {
    return createLexer(source, getStrictConfig());
}

std::unique_ptr<CHTLJSLexer> CHTLJSLexerFactory::createDebugLexer(const String& source) {
    return createLexer(source, getDebugConfig());
}

std::unique_ptr<CHTLJSLexer> CHTLJSLexerFactory::createCJMODLexer(const String& source) {
    return createLexer(source, getCJMODConfig());
}

CHTLJSLexerConfig CHTLJSLexerFactory::getDefaultConfig() {
    return CHTLJSLexerConfig{};
}

CHTLJSLexerConfig CHTLJSLexerFactory::getStrictConfig() {
    CHTLJSLexerConfig config;
    config.strictMode = true;
    config.skipWhitespace = true;
    config.skipComments = true;
    return config;
}

CHTLJSLexerConfig CHTLJSLexerFactory::getDebugConfig() {
    CHTLJSLexerConfig config;
    config.debugMode = true;
    config.skipWhitespace = false;
    config.skipComments = false;
    config.trackPositions = true;
    return config;
}

CHTLJSLexerConfig CHTLJSLexerFactory::getCJMODConfig() {
    CHTLJSLexerConfig config;
    config.enableCJMODSupport = true;
    config.enableCHTLJSExtensions = true;
    return config;
}

} // namespace JS
} // namespace CHTL