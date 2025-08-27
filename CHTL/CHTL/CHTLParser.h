#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stack>
#include "CHTLLexer/Lexer.h"
#include "CHTLLexer/Token.h"
#include "CHTLLexer/GlobalMap.h"
#include "CHTLNode/BaseNode.h"

namespace CHTL {

// 前向声明
class CHTLContext;
class ConfigNode;

// 解析器错误
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, int line, int column)
        : std::runtime_error(message), line_(line), column_(column) {}
    
    int GetLine() const { return line_; }
    int GetColumn() const { return column_; }
    
private:
    int line_;
    int column_;
};

// CHTL解析器
class CHTLParser {
public:
    CHTLParser(std::shared_ptr<GlobalMap> globalMap = nullptr);
    ~CHTLParser();
    
    // 解析CHTL源代码
    NodePtr Parse(const std::string& source, const std::string& filename = "");
    
    // 错误处理
    bool HasErrors() const { return !errors.empty(); }
    std::vector<std::string> GetErrors() const { return errors; }
    void ClearErrors() { errors.clear(); }
    
    // 配置
    void SetDebugMode(bool debug) { debugMode = debug; }
    
private:
    std::shared_ptr<Lexer> lexer;
    std::shared_ptr<GlobalMap> globalMap;
    std::shared_ptr<CHTLContext> context;
    
    Token currentToken;
    Token previousToken;
    std::vector<std::string> errors;
    bool debugMode;
    
    // 解析状态
    bool inLocalStyle;
    bool inLocalScript;
    std::string currentElementName;
    std::stack<std::string> elementStack;
    
    // Token操作
    void Advance();
    Token Peek(int offset = 0);
    bool Match(TokenType type);
    bool Check(TokenType type);
    void Consume(TokenType type, const std::string& message);
    
    // 错误处理
    void ReportError(const std::string& message);
    void Synchronize();
    
    // 顶层解析
    NodePtr ParseDocument();
    NodePtr ParseTopLevel();
    
    // use语句解析
    NodePtr ParseUse();
    
    // 配置解析
    NodePtr ParseConfiguration();
    NodePtr ParseConfigBody();
    NodePtr ParseNameBlock();
    NodePtr ParseOriginTypeBlock();
    
    // 命名空间解析
    NodePtr ParseNamespace();
    
    // 导入解析
    NodePtr ParseImport();
    
    // 模板解析
    NodePtr ParseTemplate();
    NodePtr ParseTemplateStyle();
    NodePtr ParseTemplateElement();
    NodePtr ParseTemplateVar();
    
    // 自定义解析
    NodePtr ParseCustom();
    NodePtr ParseCustomStyle();
    NodePtr ParseCustomElement();
    NodePtr ParseCustomVar();
    
    // 原始嵌入解析
    NodePtr ParseOrigin();
    
    // 元素解析
    NodePtr ParseElement();
    NodePtr ParseElementBody();
    NodePtr ParseAttribute();
    
    // 文本解析
    NodePtr ParseText();
    
    // 样式解析
    NodePtr ParseStyle();
    NodePtr ParseStyleBody();
    NodePtr ParseStyleRule();
    NodePtr ParseSelector();
    
    // 脚本解析
    NodePtr ParseScript();
    
    // 操作符解析
    NodePtr ParseDelete();
    NodePtr ParseInsert();
    NodePtr ParseExcept();
    NodePtr ParseInherit();
    
    // 表达式解析
    std::string ParseExpression();
    std::string ParseStringLiteral();
    std::string ParseUnquotedLiteral();
    
    // 辅助方法
    bool IsTopLevelKeyword();
    bool IsElementKeyword();
    bool IsStyleProperty();
    std::string ParseIdentifierPath();  // 解析 namespace.name 形式
    std::vector<std::string> ParseIdentifierList();  // 解析逗号分隔的标识符列表
    
    // 自动化选择器处理
    void ProcessAutoSelectors(NodePtr styleNode, NodePtr elementNode);
    
    // 应用配置
    void ApplyConfiguration(std::shared_ptr<ConfigNode> config);
};

} // namespace CHTL