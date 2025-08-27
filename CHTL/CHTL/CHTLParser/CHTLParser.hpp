#pragma once
#include "../CHTLLexer/Lexer.hpp"
#include "../CHTLLexer/Token.hpp"
#include "../CHTLNode/BaseNode.hpp"
#include "../CHTLState/CHTLState.hpp"
#include <memory>
#include <functional>

namespace CHTL {

/**
 * 解析错误结构
 */
struct ParseError {
    std::string message;
    size_t line;
    size_t column;
    size_t position;
    Token token;
    
    ParseError(const std::string& msg, const Token& tok)
        : message(msg), line(tok.line), column(tok.column), 
          position(tok.position), token(tok) {}
    
    std::string toString() const {
        return "解析错误 [" + std::to_string(line) + ":" + std::to_string(column) + 
               "]: " + message + " (token: " + token.value + ")";
    }
};

/**
 * 解析结果结构
 */
struct ParseResult {
    std::shared_ptr<BaseNode> ast;
    bool success;
    std::vector<ParseError> errors;
    std::vector<std::string> warnings;
    size_t tokensProcessed;
    
    ParseResult() : success(false), tokensProcessed(0) {}
    
    void addError(const ParseError& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

/**
 * CHTL解析器
 * 严格按照CHTL语法文档实现语法解析
 * 支持所有CHTL语法特性
 */
class CHTLParser {
private:
    std::unique_ptr<Lexer> lexer;
    std::vector<Token> tokens;
    size_t currentTokenIndex;
    CHTLState state;
    ParseResult result;
    
    // 解析配置
    bool strictMode;
    bool allowPartialParsing;
    bool enableWarnings;
    
    // 当前token访问
    const Token& currentToken() const;
    const Token& peekToken(size_t offset = 1) const;
    bool hasMoreTokens() const;
    void advance();
    bool consume(TokenType expectedType);
    bool match(TokenType type) const;
    bool matchAny(const std::vector<TokenType>& types) const;
    
    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& warning);
    void synchronize();
    
    // 主要解析方法
    std::shared_ptr<BaseNode> parseDocument();
    std::shared_ptr<BaseNode> parseStatement();
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseTextBlock();
    std::shared_ptr<BaseNode> parseStyleBlock();
    std::shared_ptr<BaseNode> parseScriptBlock();
    
    // 模板和自定义解析
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseOrigin();
    std::shared_ptr<BaseNode> parseImport();
    std::shared_ptr<BaseNode> parseConfiguration();
    std::shared_ptr<BaseNode> parseNamespace();
    
    // 模板子类型解析
    std::shared_ptr<BaseNode> parseStyleTemplate();
    std::shared_ptr<BaseNode> parseElementTemplate();
    std::shared_ptr<BaseNode> parseVarTemplate();
    
    // 自定义子类型解析
    std::shared_ptr<BaseNode> parseStyleCustom();
    std::shared_ptr<BaseNode> parseElementCustom();
    std::shared_ptr<BaseNode> parseVarCustom();
    
    // 操作符解析
    std::shared_ptr<BaseNode> parseDeleteOperation();
    std::shared_ptr<BaseNode> parseInsertOperation();
    std::shared_ptr<BaseNode> parseUseOperation();
    std::shared_ptr<BaseNode> parseInheritOperation();
    
    // 属性和值解析
    void parseAttributes(std::shared_ptr<BaseNode> node);
    std::string parseAttributeValue();
    std::string parseLiteral();
    std::string parseStringLiteral();
    std::string parseUnquotedLiteral();
    
    // CSS相关解析
    void parseStyleProperties(std::shared_ptr<BaseNode> styleNode);
    void parseSelector(std::shared_ptr<BaseNode> styleNode);
    void parseProperty(std::shared_ptr<BaseNode> styleNode);
    
    // 特殊语法解析
    std::shared_ptr<BaseNode> parseReference();      // & 引用
    std::shared_ptr<BaseNode> parseVariableRef();    // 变量引用
    std::shared_ptr<BaseNode> parseIndexAccess();    // [index] 索引访问
    
    // 注释解析
    std::shared_ptr<BaseNode> parseComment();
    
    // 表达式解析
    std::shared_ptr<BaseNode> parseExpression();
    std::shared_ptr<BaseNode> parsePrimaryExpression();
    
    // 验证方法
    bool validateElementName(const std::string& name) const;
    bool validateAttributeName(const std::string& name) const;
    bool validateSelector(const std::string& selector) const;
    
    // 上下文感知解析
    void enterContext(ContextType context);
    void exitContext();
    bool isValidInCurrentContext(TokenType tokenType) const;
    
    // 特殊处理
    void handleAutomation(std::shared_ptr<BaseNode> element);
    void processInheritance(std::shared_ptr<BaseNode> node);
    void processSpecialization(std::shared_ptr<BaseNode> node);

public:
    CHTLParser();
    explicit CHTLParser(std::unique_ptr<Lexer> lexer);
    ~CHTLParser();
    
    // 主要接口
    ParseResult parse(const std::string& source);
    ParseResult parse(const std::vector<Token>& tokens);
    
    // 配置
    void setStrictMode(bool strict);
    void setAllowPartialParsing(bool allow);
    void setEnableWarnings(bool enable);
    
    bool getStrictMode() const;
    bool getAllowPartialParsing() const;
    bool getEnableWarnings() const;
    
    // 词法分析器管理
    void setLexer(std::unique_ptr<Lexer> lexer);
    Lexer* getLexer() const;
    
    // 状态管理
    CHTLState& getState();
    const CHTLState& getState() const;
    
    // 结果获取
    const ParseResult& getLastResult() const;
    
    // 工具方法
    void reset();
    size_t getCurrentPosition() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 调试
    void dumpTokens() const;
    void dumpAST() const;
    void enableDebugMode(bool enable);
};

/**
 * 递归下降解析器基类
 * 提供通用的递归下降解析功能
 */
class RecursiveDescentParser {
protected:
    std::vector<Token> tokens;
    size_t currentIndex;
    
    // 基础方法
    virtual const Token& current() const;
    virtual const Token& peek(size_t offset = 1) const;
    virtual void advance();
    virtual bool match(TokenType type) const;
    virtual bool consume(TokenType type);
    
    // 错误恢复
    virtual void synchronize() = 0;
    virtual void reportError(const std::string& message) = 0;

public:
    RecursiveDescentParser() : currentIndex(0) {}
    virtual ~RecursiveDescentParser() = default;
    
    virtual ParseResult parse(const std::vector<Token>& tokens) = 0;
};

/**
 * 解析器工厂
 * 创建不同配置的解析器实例
 */
class ParserFactory {
public:
    static std::unique_ptr<CHTLParser> createDefaultParser();
    static std::unique_ptr<CHTLParser> createStrictParser();
    static std::unique_ptr<CHTLParser> createPermissiveParser();
    static std::unique_ptr<CHTLParser> createDebugParser();
    
    // 配置方法
    static void configureForProduction(CHTLParser& parser);
    static void configureForDevelopment(CHTLParser& parser);
    static void configureForDebugging(CHTLParser& parser);
};

/**
 * 解析器扩展接口
 * 允许扩展解析器功能
 */
class IParserExtension {
public:
    virtual ~IParserExtension() = default;
    virtual bool canHandle(const Token& token) const = 0;
    virtual std::shared_ptr<BaseNode> parse(CHTLParser& parser, const Token& token) = 0;
    virtual std::string getName() const = 0;
};

/**
 * 可扩展解析器
 * 支持插件式扩展
 */
class ExtensibleParser : public CHTLParser {
private:
    std::vector<std::unique_ptr<IParserExtension>> extensions;

public:
    ExtensibleParser();
    
    // 扩展管理
    void addExtension(std::unique_ptr<IParserExtension> extension);
    void removeExtension(const std::string& name);
    bool hasExtension(const std::string& name) const;
    
    // 重写解析方法以支持扩展
    std::shared_ptr<BaseNode> parseStatement() override;

private:
    IParserExtension* findExtension(const Token& token) const;
};

}