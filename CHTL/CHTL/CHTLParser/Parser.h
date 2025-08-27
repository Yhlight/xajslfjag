#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include <string>
#include <stack>
#include <unordered_set>
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/ConfigNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/OperatorNode.h"

namespace CHTL {

// 解析器错误类
class ParseError : public std::runtime_error {
private:
    int line;
    int column;
    
public:
    ParseError(const std::string& message, int l, int c)
        : std::runtime_error(message), line(l), column(c) {}
    
    int getLine() const { return line; }
    int getColumn() const { return column; }
};

// 解析器上下文
struct ParserContext {
    std::shared_ptr<ConfigNode> currentConfig;  // 当前配置
    std::shared_ptr<NamespaceNode> currentNamespace;  // 当前命名空间
    std::stack<std::shared_ptr<BaseNode>> nodeStack;  // 节点栈
    std::unordered_set<std::string> definedTemplates;  // 已定义的模板
    std::unordered_set<std::string> definedCustoms;    // 已定义的自定义
    bool inTextNode;  // 是否在文本节点内
    bool inStyleBlock;  // 是否在样式块内
    bool inScriptBlock;  // 是否在脚本块内
    
    ParserContext() : inTextNode(false), inStyleBlock(false), inScriptBlock(false) {}
};

// 语法分析器类
class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    ParserContext context;
    
    // Token操作
    Token& peek(int offset = 0);
    Token& previous();
    Token& advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token& consume(TokenType type, const std::string& message);
    
    // 错误处理
    ParseError error(const Token& token, const std::string& message);
    void synchronize();
    
    // 解析方法
    std::shared_ptr<BaseNode> parseProgram();
    std::shared_ptr<BaseNode> parseTopLevel();
    std::shared_ptr<BaseNode> parseStatement();
    
    // 特殊节点解析
    std::shared_ptr<OperatorNode> parseUse();
    std::shared_ptr<ConfigNode> parseConfiguration();
    std::shared_ptr<NamespaceNode> parseNamespace();
    std::shared_ptr<ImportNode> parseImport();
    std::shared_ptr<OriginNode> parseOrigin();
    std::shared_ptr<TemplateNode> parseTemplate();
    std::shared_ptr<CustomNode> parseCustom();
    
    // 元素解析
    std::shared_ptr<ElementNode> parseElement();
    std::shared_ptr<TextNode> parseText();
    std::shared_ptr<StyleNode> parseStyle(bool isGlobal = false);
    std::shared_ptr<ScriptNode> parseScript(bool isGlobal = false);
    
    // 属性解析
    void parseAttributes(ElementNode* element);
    std::pair<std::string, std::string> parseAttribute();
    
    // 样式解析
    void parseStyleContent(StyleNode* styleNode);
    void parseInlineStyle(StyleNode* styleNode);
    void parseSelectorRule(StyleNode* styleNode);
    
    // 字面量解析
    std::string parseLiteral();
    std::string parseUnquotedLiteral();
    
    // 辅助方法
    bool isHtmlElement(const std::string& name) const;
    bool isKeywordContext(const std::string& word) const;
    std::string parseIdentifierPath();  // 解析类似 space.room2 的路径
    
    // CE对等式处理
    bool matchColonOrEquals();
    
public:
    Parser(const std::vector<Token>& tokenList);
    ~Parser() = default;
    
    // 执行解析
    std::shared_ptr<BaseNode> parse();
    
    // 获取解析上下文（用于调试）
    const ParserContext& getContext() const { return context; }
};

} // namespace CHTL

#endif // CHTL_PARSER_H