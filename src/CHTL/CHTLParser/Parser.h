#pragma once

#include "../../Util/Common.h"
#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/Context.h"
#include "../CHTLState/State.h"
#include "../CHTLSelector/SelectorAutomation.h"

namespace CHTL {

// 解析器配置
struct ParserConfig {
    bool enableUnquotedLiterals = true;
    bool enableAutoSemicolon = true;
    bool enableStyleAutomation = true;
    bool enableScriptAutomation = false;
    bool strictMode = false;
    bool allowMissingBraces = false;
    bool enableRecovery = true;
    size_t maxRecoveryAttempts = 3;
    size_t maxParseDepth = 100;
};

// 解析错误
struct ParseError {
    String message;
    Position position;
    String errorCode;
    String suggestion;
    
    ParseError(const String& msg, const Position& pos, const String& code = "", const String& suggest = "")
        : message(msg), position(pos), errorCode(code), suggestion(suggest) {}
    
    String toString() const;
};

// CHTL解析器
class Parser {
public:
    explicit Parser(std::unique_ptr<Lexer> lexer, const ParserConfig& config = ParserConfig{});
    ~Parser() = default;
    
    // 主解析方法
    std::unique_ptr<BaseNode> parse();
    std::unique_ptr<BaseNode> parseDocument();
    std::unique_ptr<BaseNode> parseFragment();
    
    // 特定节点解析
    std::unique_ptr<BaseNode> parseElement();
    std::unique_ptr<BaseNode> parseText();
    std::unique_ptr<BaseNode> parseStyle();
    std::unique_ptr<BaseNode> parseScript();
    std::unique_ptr<BaseNode> parseTemplate();
    std::unique_ptr<BaseNode> parseCustom();
    std::unique_ptr<BaseNode> parseOrigin();
    std::unique_ptr<BaseNode> parseImport();
    std::unique_ptr<BaseNode> parseConfiguration();
    std::unique_ptr<BaseNode> parseNameConfiguration();
    std::unique_ptr<BaseNode> parseOriginTypeConfiguration();
    std::unique_ptr<BaseNode> parseNamespace();
    std::unique_ptr<BaseNode> parseComment();
    
    // 操作符解析
    std::unique_ptr<BaseNode> parseInherit();
    std::unique_ptr<BaseNode> parseDelete();
    std::unique_ptr<BaseNode> parseInsert();
    std::unique_ptr<BaseNode> parseUse();
    
    // 约束解析
    std::unique_ptr<class ConstraintNode> parseConstraint();
    std::unique_ptr<class ConstraintNode> parseExcept();
    StringVector parseConstraintTargets();
    String getCurrentScope() const;
    bool isInNamespaceScope() const;
    
    // 选择器自动化处理
    void processSelectorAutomation(BaseNode* elementNode);
    void processStyleBlockSelectors(BaseNode* elementNode, BaseNode* styleNode);
    void processScriptBlockSelectors(BaseNode* elementNode, BaseNode* scriptNode);
    void processReferenceRules(BaseNode* documentNode);
    void processDocumentSelectorAutomation(BaseNode* documentNode);
    
    // 特例化解析
    std::unique_ptr<class NoValueStyleNode> parseNoValueStyle();
    bool isIndexAccessSyntax(const String& input) const;
    bool isNoValueStyleSyntax(const String& input) const;
    
    // 属性和值解析
    StringUnorderedMap parseAttributes();
    void parseElementAttributes(ElementNode* element);
    String parseAttributeValue();
    String parseLiteral();
    String parseIdentifier();
    String parseSelector();
    
    // 表达式解析
    std::unique_ptr<BaseNode> parseExpression();
    std::unique_ptr<BaseNode> parseVariableReference();
    std::unique_ptr<BaseNode> parseFunctionCall();
    
    // 状态和上下文
    void setContext(std::shared_ptr<Context> ctx) { context = ctx; }
    std::shared_ptr<Context> getContext() const { return context; }
    
    void setStateManager(std::shared_ptr<StateManager> sm) { stateManager = sm; }
    std::shared_ptr<StateManager> getStateManager() const { return stateManager; }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    std::vector<ParseError> getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // 配置
    void setConfig(const ParserConfig& config) { this->config = config; }
    const ParserConfig& getConfig() const { return config; }
    
    // 调试支持
    void enableDebugMode(bool enable) { debugMode = enable; }
    bool isDebugMode() const { return debugMode; }
    void dumpParseState() const;
    
private:
    std::unique_ptr<Lexer> lexer;
    ParserConfig config;
    std::shared_ptr<Context> context;
    std::shared_ptr<StateManager> stateManager;
    std::shared_ptr<SelectorAutomationManager> selectorAutomation;
    std::shared_ptr<ReferenceRuleManager> referenceRuleManager;
    std::vector<ParseError> errors;
    bool debugMode;
    size_t maxParseDepth = 100;    // 最大解析深度限制 (降低到100)
    size_t currentDepth = 0;      // 当前解析深度
    size_t parseNodeCount = 0;    // 解析节点计数
    size_t maxParseNodes = 1000;  // 最大节点数限制 (降低到1000)
    
    // Token操作
    Token currentToken;
    Token nextToken();
    Token peekToken(size_t lookahead = 1) const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const String& errorMessage = "");
    void advance();
    
    // 错误处理和恢复
    void reportError(const String& message, const String& errorCode = "", const String& suggestion = "");
    void reportUnexpectedToken(TokenType expected);
    bool recover();
    void synchronize();
    void skipToNextStatement();
    void skipToEndOfBlock();
    
    // 解析工具方法
    bool isAtEnd() const;
    bool isBlockStart() const;
    bool isBlockEnd() const;
    bool isStatementEnd() const;
    bool isKeyword(const String& keyword) const;
    bool isHTMLElement() const;
    bool isSelector() const;
    bool isCHTLJSSyntax() const;
    
    // 安全解析保护
    bool checkParseDepth();
    bool checkParseNodeCount();
    void incrementDepth();
    void decrementDepth();
    void incrementNodeCount();
    bool isAttribute() const;
    
    // 特定语法解析
    std::unique_ptr<BaseNode> parseBlock();
    std::unique_ptr<BaseNode> parseStatement();
    StringVector parseParameterList();
    StringUnorderedMap parseKeyValuePairs();
    String parseStringLiteral();
    String parseUnquotedLiteral();
    
    // 特例化操作解析
    std::unique_ptr<BaseNode> parseDeleteOperation();
    std::unique_ptr<BaseNode> parseInsertOperation();
    std::unique_ptr<IndexAccessNode> parseIndexAccess();
    bool isIndexAccess();
    
    // 无值样式组解析
    std::unique_ptr<BaseNode> parseNoValueStyleGroup();
    
    // 变量组特例化解析
    std::unique_ptr<BaseNode> parseVariableSpecialization();
    bool isVariableSpecialization();
    
    // 模板系统解析
    std::unique_ptr<BaseNode> parseTemplateStyle();
    std::unique_ptr<BaseNode> parseTemplateElement();
    std::unique_ptr<BaseNode> parseTemplateVar();
    std::unique_ptr<BaseNode> parseCustomStyle();
    std::unique_ptr<BaseNode> parseCustomElement();
    std::unique_ptr<BaseNode> parseCustomVar();
    
    // 原始嵌入解析
    std::unique_ptr<BaseNode> parseOriginHtml();
    std::unique_ptr<BaseNode> parseOriginStyle();
    std::unique_ptr<BaseNode> parseOriginJavaScript();
    std::unique_ptr<BaseNode> parseOriginCustom();
    
    // 导入系统解析
    std::unique_ptr<BaseNode> parseImportHtml();
    std::unique_ptr<BaseNode> parseImportStyle();
    std::unique_ptr<BaseNode> parseImportJavaScript();
    std::unique_ptr<BaseNode> parseImportChtl();
    std::unique_ptr<BaseNode> parseImportCjmod();
    std::unique_ptr<BaseNode> parseImportConfig();
    std::unique_ptr<BaseNode> parseImportOrigin();
    
    // 配置解析
    std::unique_ptr<BaseNode> parseConfigName();
    std::unique_ptr<BaseNode> parseConfigOriginType();
    StringUnorderedMap parseConfigValues();
    
    // CHTL JS集成
    std::unique_ptr<BaseNode> parseCHTLJSSelector();
    std::unique_ptr<BaseNode> parseCHTLJSFunction();
    std::unique_ptr<BaseNode> parseCHTLJSEventBinding();
    std::unique_ptr<BaseNode> parseCHTLJSVirtualObject();
    
    // 样式块特殊处理
    std::unique_ptr<BaseNode> parseStyleBlock();
    std::unique_ptr<BaseNode> parseStyleSelector();
    std::unique_ptr<BaseNode> parseStyleProperty();
    std::unique_ptr<BaseNode> parseStyleInheritance();
    std::unique_ptr<BaseNode> parseStyleDeletion();
    
    // 脚本块特殊处理
    std::unique_ptr<BaseNode> parseScriptBlock();
    String parseScriptContent();
    
    // 验证和自动化
    void validateElementStructure(BaseNode* element);
    void applyStyleAutomation(BaseNode* element);
    void applyScriptAutomation(BaseNode* element);
    void validateTemplateUsage(BaseNode* templateNode);
    void validateCustomUsage(BaseNode* customNode);
    
    // 自动化功能
    void autoAddClassSelector(BaseNode* element, const String& className);
    void autoAddIdSelector(BaseNode* element, const String& idName);
    String generateAutoClassName();
    String generateAutoIdName();
    
    // 深度检查
    bool checkMaxDepth() const;
    
    // 调试输出
    void debugLog(const String& message) const;
    void debugLogToken(const Token& token) const;
    void debugLogState() const;
};

// 解析器工厂
class ParserFactory {
public:
    static std::unique_ptr<Parser> createCHTLParser(const String& source);
    static std::unique_ptr<Parser> createCHTLParser(std::unique_ptr<Lexer> lexer);
    static std::unique_ptr<Parser> createCHTLParserWithConfig(const String& source, const ParserConfig& config);
    static std::unique_ptr<Parser> createFragmentParser(const String& fragment);
    
    // 特殊解析器
    static std::unique_ptr<Parser> createTemplateParser(const String& source);
    static std::unique_ptr<Parser> createStyleParser(const String& source);
    static std::unique_ptr<Parser> createScriptParser(const String& source);
    
    // 配置预设
    static ParserConfig getStrictConfig();
    static ParserConfig getPermissiveConfig();
    static ParserConfig getProductionConfig();
    static ParserConfig getDebugConfig();
};

// 解析结果
struct ParseResult {
    std::unique_ptr<BaseNode> ast;
    std::vector<ParseError> errors;
    std::vector<String> warnings;
    double parseTime;
    size_t nodeCount;
    bool success;
    
    ParseResult() : parseTime(0.0), nodeCount(0), success(false) {}
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    String getSummary() const;
};

// 解析器管理器
class ParserManager {
public:
    explicit ParserManager(const ParserConfig& config = ParserConfig{});
    
    // 解析方法
    ParseResult parseFile(const String& filePath);
    ParseResult parseString(const String& source);
    ParseResult parseStream(std::istream& stream);
    
    // 批量解析
    std::vector<ParseResult> parseFiles(const StringVector& filePaths);
    std::vector<ParseResult> parseDirectory(const String& dirPath, const String& extension = ".chtl");
    
    // 并行解析
    std::vector<ParseResult> parseFilesParallel(const StringVector& filePaths, size_t threadCount = 0);
    
    // 配置管理
    void setConfig(const ParserConfig& config) { this->config = config; }
    const ParserConfig& getConfig() const { return config; }
    
    // 统计信息
    struct ParseStats {
        size_t totalFiles = 0;
        size_t successfulParses = 0;
        size_t failedParses = 0;
        size_t totalNodes = 0;
        size_t totalErrors = 0;
        size_t totalWarnings = 0;
        double totalTime = 0.0;
        double averageTime = 0.0;
    };
    
    ParseStats getStats() const { return stats; }
    void resetStats() { stats = ParseStats{}; }
    
private:
    ParserConfig config;
    ParseStats stats;
    
    void updateStats(const ParseResult& result);
    ParseResult parseInternal(const String& source, const String& sourceName = "");
};

} // namespace CHTL