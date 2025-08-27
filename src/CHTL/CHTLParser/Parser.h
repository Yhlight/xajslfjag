#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLState/State.h"
#include "../CHTLContext/Context.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_set>

namespace CHTL {



/// CHTL解析器类
/// 负责将Token流解析为AST
class Parser {
private:
    std::shared_ptr<Lexer> lexer;           // 词法分析器
    std::shared_ptr<StateManager> state;           // 状态管理器
    std::shared_ptr<Context> context;       // 上下文管理器
    
    std::vector<Token> tokens;              // Token流
    size_t currentTokenIndex;               // 当前Token索引
    
    std::vector<std::string> errorMessages; // 错误消息列表

public:
    /// 构造函数
    Parser(std::shared_ptr<Lexer> lex, std::shared_ptr<StateManager> st, std::shared_ptr<Context> ctx);
    
    /// 解析CHTL源代码
    std::shared_ptr<BaseNode> Parse(const std::string& source);
    
    /// 获取错误消息
    const std::vector<std::string>& GetErrors() const { return errorMessages; }
    
    /// 检查是否有错误
    bool HasErrors() const { return !errorMessages.empty(); }

private:
    // === 基础解析方法 ===
    
    /// 获取当前Token
    Token& CurrentToken();
    
    /// 获取下一个Token
    Token& NextToken();
    
    /// 检查当前Token类型
    bool IsCurrentToken(TokenType type) const;
    
    /// 消费期望的Token类型
    bool ConsumeToken(TokenType expectedType);
    
    /// 跳过当前Token
    void SkipToken();
    
    /// 检查是否到达结尾
    bool IsAtEnd() const;
    
    /// 添加错误消息
    void AddError(const std::string& message);
    
    // === 语法解析方法 ===
    
    /// 解析文档根节点
    std::shared_ptr<BaseNode> ParseDocument();
    
    /// 解析use语句
    std::shared_ptr<BaseNode> ParseUseStatement();
    
    /// 解析元素节点
    std::shared_ptr<ElementNode> ParseElement();
    
    /// 解析元素属性
    void ParseAttributes(std::shared_ptr<ElementNode> element);
    
    /// 解析单个属性
    void ParseAttribute(std::shared_ptr<ElementNode> element);
    
    /// 解析元素内容
    void ParseElementContent(std::shared_ptr<ElementNode> element);
    
    /// 解析文本节点
    std::shared_ptr<TextNode> ParseTextNode();
    
    /// 解析注释节点
    std::shared_ptr<CommentNode> ParseComment();
    
    /// 解析样式块
    std::shared_ptr<StyleBlockNode> ParseStyleBlock();
    
    /// 解析样式规则
    std::shared_ptr<StyleRuleNode> ParseStyleRule();
    
    /// 解析样式属性
    std::shared_ptr<StylePropertyNode> ParseStyleProperty();
    
    /// 解析选择器
    std::shared_ptr<SelectorNode> ParseSelector();
    
    /// 解析脚本块
    std::shared_ptr<ScriptBlockNode> ParseScriptBlock();
    
    // === 辅助方法 ===
    
    /// 解析字符串值（处理引号和无修饰字面量）
    std::string ParseStringValue();
    
    /// 解析标识符
    std::string ParseIdentifier();
    
    /// 跳过空白Token
    void SkipWhitespace();
    
    /// 检查是否为HTML元素名
    bool IsValidHtmlElement(const std::string& tagName) const;
    
    /// 处理CE对等式（冒号和等号等价）
    bool IsAttributeAssignment() const;
    
    /// 解析块内容（通用）
    void ParseBlockContent(std::shared_ptr<BaseNode> parent, 
                          std::function<bool()> shouldContinue,
                          std::function<std::shared_ptr<BaseNode>()> parseChild);
};

/// HTML元素验证器
class HtmlElementValidator {
private:
    static std::unordered_set<std::string> validElements;
    static std::unordered_set<std::string> voidElements;

public:
    /// 检查是否为有效的HTML元素
    static bool IsValidElement(const std::string& tagName);
    
    /// 检查是否为void元素（自闭合）
    static bool IsVoidElement(const std::string& tagName);
    
    /// 初始化元素集合
    static void Initialize();
};

/// 解析结果
struct ParseResult {
    std::shared_ptr<BaseNode> rootNode;    // 根节点
    std::vector<std::string> errors;       // 错误列表
    std::vector<std::string> warnings;     // 警告列表
    bool success;                          // 是否成功
    
    ParseResult() : success(false) {}
};

/// 解析器工厂
class ParserFactory {
public:
    /// 创建标准CHTL解析器
    static std::unique_ptr<Parser> CreateCHTLParser();
};

} // namespace CHTL