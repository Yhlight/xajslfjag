#include "Parser.h"
#include "../CHTLLexer/EnhancedLexer.h"
#include <algorithm>
#include <cctype>
#include <functional>

namespace CHTL {

// HTML元素验证器的静态成员初始化
std::unordered_set<std::string> HtmlElementValidator::validElements;
std::unordered_set<std::string> HtmlElementValidator::voidElements;

// Parser实现

Parser::Parser(std::shared_ptr<Lexer> lex, std::shared_ptr<StateManager> st, std::shared_ptr<Context> ctx)
    : lexer(lex), state(st), context(ctx), currentTokenIndex(0) {
    HtmlElementValidator::Initialize();
}

std::shared_ptr<BaseNode> Parser::Parse(const std::string& source) {
    // 清除之前的状态
    tokens.clear();
    currentTokenIndex = 0;
    errorMessages.clear();
    
    // 使用增强型词法分析器
    auto enhancedLexer = std::make_shared<EnhancedLexer>(lexer->GetGlobalMap());
    enhancedLexer->SetSource(source);
    enhancedLexer->Analyze();
    
    if (enhancedLexer->HasErrors()) {
        for (const auto& error : enhancedLexer->GetErrors()) {
            AddError("词法分析错误: " + error);
        }
        return nullptr;
    }
    
    tokens = enhancedLexer->GetTokens();
    
    if (tokens.empty()) {
        AddError("输入为空或词法分析失败");
        return nullptr;
    }
    
    // 语法分析
    return ParseDocument();
}

Token& Parser::CurrentToken() {
    if (currentTokenIndex >= tokens.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", {0, 0, 0});
        return eofToken;
    }
    return tokens[currentTokenIndex];
}

Token& Parser::NextToken() {
    if (currentTokenIndex < tokens.size()) {
        currentTokenIndex++;
    }
    return CurrentToken();
}

bool Parser::IsCurrentToken(TokenType type) const {
    if (currentTokenIndex >= tokens.size()) {
        return type == TokenType::EOF_TOKEN;
    }
    return tokens[currentTokenIndex].type == type;
}

bool Parser::ConsumeToken(TokenType expectedType) {
    if (IsCurrentToken(expectedType)) {
        NextToken();
        return true;
    }
    return false;
}

void Parser::SkipToken() {
    NextToken();
}

bool Parser::IsAtEnd() const {
    return currentTokenIndex >= tokens.size() || IsCurrentToken(TokenType::EOF_TOKEN);
}

void Parser::AddError(const std::string& message) {
    std::string fullMessage = "解析错误";
    if (!IsAtEnd()) {
        auto& token = CurrentToken();
        fullMessage += " 第" + std::to_string(token.position.line) + 
                      "行:" + std::to_string(token.position.column) + " ";
    }
    fullMessage += ": " + message;
    errorMessages.push_back(fullMessage);
}

std::shared_ptr<BaseNode> Parser::ParseDocument() {
    auto root = std::make_shared<ElementNode>("document");
    
    SkipWhitespace();
    
    while (!IsAtEnd()) {
        std::shared_ptr<BaseNode> child = nullptr;
        
        // 检查当前Token类型
        if (IsCurrentToken(TokenType::IDENTIFIER)) {
            std::string identifier = CurrentToken().value;
            
            // 检查是否为use语句
            if (identifier == "use") {
                child = ParseUseStatement();
            }
            // 检查是否为HTML元素
            else if (HtmlElementValidator::IsValidElement(identifier)) {
                child = ParseElement();
            }
            // 检查是否为text关键字
            else if (identifier == "text") {
                child = ParseTextNode();
            }
            else {
                AddError("未识别的标识符: " + identifier);
                SkipToken();
                continue;
            }
        }
        else if (IsCurrentToken(TokenType::COMMENT_LINE) || 
                 IsCurrentToken(TokenType::COMMENT_BLOCK) ||
                 IsCurrentToken(TokenType::COMMENT_GENERATOR)) {
            child = ParseComment();
        }
        else if (IsCurrentToken(TokenType::WHITESPACE) || 
                 IsCurrentToken(TokenType::NEWLINE)) {
            SkipToken();
            continue;
        }
        else {
            AddError("意外的Token类型");
            SkipToken();
            continue;
        }
        
        if (child) {
            root->AddChild(child);
        }
        
        SkipWhitespace();
    }
    
    return root;
}

std::shared_ptr<BaseNode> Parser::ParseUseStatement() {
    // 暂时跳过use语句的实现
    ConsumeToken(TokenType::IDENTIFIER); // "use"
    
    SkipWhitespace();
    
    if (IsCurrentToken(TokenType::IDENTIFIER)) {
        std::string useType = CurrentToken().value;
        SkipToken();
        
        // 创建一个注释节点来记录use语句
        auto useComment = std::make_shared<CommentNode>(
            "USE: " + useType, 
            CommentNode::CommentType::GENERATOR
        );
        
        ConsumeToken(TokenType::SEMICOLON);
        return useComment;
    }
    
    AddError("use语句后面需要跟类型");
    return nullptr;
}

std::shared_ptr<ElementNode> Parser::ParseElement() {
    if (!IsCurrentToken(TokenType::IDENTIFIER)) {
        AddError("期望元素名");
        return nullptr;
    }
    
    std::string tagName = CurrentToken().value;
    auto element = std::make_shared<ElementNode>(tagName);
    
    // 检查是否为有效HTML元素
    if (!HtmlElementValidator::IsValidElement(tagName)) {
        AddError("无效的HTML元素: " + tagName);
    }
    
    // 设置是否为自闭合元素
    element->SetSelfClosing(HtmlElementValidator::IsVoidElement(tagName));
    
    SkipToken(); // 消费元素名
    SkipWhitespace();
    
    // 解析元素内容
    if (IsCurrentToken(TokenType::LEFT_BRACE)) {
        ConsumeToken(TokenType::LEFT_BRACE);
        ParseElementContent(element);
        
        if (!ConsumeToken(TokenType::RIGHT_BRACE)) {
            AddError("期望 '}'");
        }
    }
    
    return element;
}

void Parser::ParseElementContent(std::shared_ptr<ElementNode> element) {
    SkipWhitespace();
    
    while (!IsAtEnd() && !IsCurrentToken(TokenType::RIGHT_BRACE)) {
        std::shared_ptr<BaseNode> child = nullptr;
        
        if (IsCurrentToken(TokenType::IDENTIFIER) || 
            IsCurrentToken(TokenType::STYLE) ||
            IsCurrentToken(TokenType::SCRIPT) ||
            IsCurrentToken(TokenType::TEXT)) {
            
            std::string identifier = CurrentToken().value;
            
            if (identifier == "style" || IsCurrentToken(TokenType::STYLE)) {
                child = ParseStyleBlock();
            }
            else if (identifier == "script" || IsCurrentToken(TokenType::SCRIPT)) {
                child = std::static_pointer_cast<BaseNode>(ParseScriptBlock());
            }
            else if (identifier == "text" || IsCurrentToken(TokenType::TEXT)) {
                child = ParseTextNode();
            }
            else if (HtmlElementValidator::IsValidElement(identifier)) {
                child = ParseElement();
            }
            else {
                // 尝试解析为属性
                ParseAttribute(element);
                SkipWhitespace();
                continue;
            }
        }
        else if (IsCurrentToken(TokenType::COMMENT_LINE) || 
                 IsCurrentToken(TokenType::COMMENT_BLOCK) ||
                 IsCurrentToken(TokenType::COMMENT_GENERATOR)) {
            child = ParseComment();
        }
        else if (IsCurrentToken(TokenType::WHITESPACE) || 
                 IsCurrentToken(TokenType::NEWLINE)) {
            SkipToken();
            continue;
        }
        else {
            AddError("元素内容中的意外Token");
            SkipToken();
            continue;
        }
        
        if (child) {
            element->AddChild(child);
        }
        
        SkipWhitespace();
    }
}

void Parser::ParseAttribute(std::shared_ptr<ElementNode> element) {
    if (!IsCurrentToken(TokenType::IDENTIFIER)) {
        AddError("期望属性名");
        return;
    }
    
    std::string attributeName = CurrentToken().value;
    SkipToken();
    SkipWhitespace();
    
    // 检查CE对等式（: 或 =）
    if (!IsCurrentToken(TokenType::COLON) && !IsCurrentToken(TokenType::EQUALS)) {
        AddError("期望 ':' 或 '=' 在属性名后");
        return;
    }
    
    SkipToken(); // 消费 : 或 =
    SkipWhitespace();
    
    // 解析属性值
    std::string attributeValue = ParseStringValue();
    
    element->SetAttribute(attributeName, attributeValue);
    
    // 可选的分号
    ConsumeToken(TokenType::SEMICOLON);
}

std::shared_ptr<TextNode> Parser::ParseTextNode() {
    // 消费text关键字（可能是IDENTIFIER或TEXT类型）
    if (IsCurrentToken(TokenType::TEXT)) {
        ConsumeToken(TokenType::TEXT);
    } else if (IsCurrentToken(TokenType::IDENTIFIER) && CurrentToken().value == "text") {
        ConsumeToken(TokenType::IDENTIFIER);
    } else {
        AddError("期望text关键字");
        return nullptr;
    }
    
    SkipWhitespace();
    
    if (!ConsumeToken(TokenType::LEFT_BRACE)) {
        AddError("text后面期望 '{'");
        return nullptr;
    }
    
    SkipWhitespace();
    
    // 收集text块中的所有内容
    std::string content;
    bool hasContent = false;
    
    while (!IsAtEnd() && !IsCurrentToken(TokenType::RIGHT_BRACE)) {
        if (IsCurrentToken(TokenType::WHITESPACE) || IsCurrentToken(TokenType::NEWLINE)) {
            if (hasContent) {
                content += " "; // 将换行和空白转换为空格
            }
            SkipToken();
            continue;
        }
        
        // 收集所有非结构Token作为文本内容
        if (IsCurrentToken(TokenType::STRING_LITERAL) ||
            IsCurrentToken(TokenType::IDENTIFIER) ||
            IsCurrentToken(TokenType::NUMBER) ||
            IsCurrentToken(TokenType::UNQUOTED_LITERAL)) {
            
            if (hasContent) {
                content += " "; // 在Token之间添加空格
            }
            
            std::string tokenValue = CurrentToken().value;
            
            // 处理字符串字面量，移除引号
            if (IsCurrentToken(TokenType::STRING_LITERAL) && tokenValue.size() >= 2) {
                if ((tokenValue.front() == '"' && tokenValue.back() == '"') ||
                    (tokenValue.front() == '\'' && tokenValue.back() == '\'')) {
                    tokenValue = tokenValue.substr(1, tokenValue.size() - 2);
                }
            }
            
            content += tokenValue;
            hasContent = true;
            SkipToken();
        } else {
            // 遇到不认识的Token，跳过
            SkipToken();
        }
    }
    
    auto textNode = std::make_shared<TextNode>(content);
    
    if (!ConsumeToken(TokenType::RIGHT_BRACE)) {
        AddError("text块期望 '}'");
    }
    
    return textNode;
}

std::shared_ptr<CommentNode> Parser::ParseComment() {
    CommentNode::CommentType commentType;
    
    if (IsCurrentToken(TokenType::COMMENT_LINE)) {
        commentType = CommentNode::CommentType::LINE;
    } else if (IsCurrentToken(TokenType::COMMENT_BLOCK)) {
        commentType = CommentNode::CommentType::BLOCK;
    } else {
        commentType = CommentNode::CommentType::GENERATOR;
    }
    
    std::string content = CurrentToken().value;
    auto comment = std::make_shared<CommentNode>(content, commentType);
    
    SkipToken();
    return comment;
}

std::shared_ptr<StyleBlockNode> Parser::ParseStyleBlock() {
    ConsumeToken(TokenType::IDENTIFIER); // "style"
    SkipWhitespace();
    
    if (!ConsumeToken(TokenType::LEFT_BRACE)) {
        AddError("style后面期望 '{'");
        return nullptr;
    }
    
    auto styleBlock = std::make_shared<StyleBlockNode>();
    
    SkipWhitespace();
    
    while (!IsAtEnd() && !IsCurrentToken(TokenType::RIGHT_BRACE)) {
        // 简化实现：暂时将样式内容作为原始内容保存
        std::string styleContent;
        int braceCount = 0;
        
        while (!IsAtEnd()) {
            if (IsCurrentToken(TokenType::LEFT_BRACE)) {
                braceCount++;
                styleContent += CurrentToken().value;
            } else if (IsCurrentToken(TokenType::RIGHT_BRACE)) {
                if (braceCount == 0) {
                    break; // 这是结束的右大括号
                }
                braceCount--;
                styleContent += CurrentToken().value;
            } else {
                styleContent += CurrentToken().value;
            }
            SkipToken();
        }
        
        // 创建一个内联样式属性来保存内容
        auto inlineProperty = std::make_shared<StylePropertyNode>("_raw_content", styleContent);
        styleBlock->AddInlineProperty(inlineProperty);
        
        break; // 简化处理，一次读取所有内容
    }
    
    if (!ConsumeToken(TokenType::RIGHT_BRACE)) {
        AddError("style块期望 '}'");
    }
    
    return styleBlock;
}

std::shared_ptr<ScriptBlockNode> Parser::ParseScriptBlock() {
    ConsumeToken(TokenType::IDENTIFIER); // "script"
    SkipWhitespace();
    
    if (!ConsumeToken(TokenType::LEFT_BRACE)) {
        AddError("script后面期望 '{'");
        return nullptr;
    }
    
    auto scriptBlock = std::make_shared<ScriptBlockNode>();
    
    // 简化实现：读取原始脚本内容
    std::string scriptContent;
    int braceCount = 0;
    
    while (!IsAtEnd()) {
        if (IsCurrentToken(TokenType::LEFT_BRACE)) {
            braceCount++;
            scriptContent += CurrentToken().value;
        } else if (IsCurrentToken(TokenType::RIGHT_BRACE)) {
            if (braceCount == 0) {
                break; // 这是结束的右大括号
            }
            braceCount--;
            scriptContent += CurrentToken().value;
        } else {
            scriptContent += CurrentToken().value;
        }
        SkipToken();
    }
    
    scriptBlock->SetRawContent(scriptContent);
    
    if (!ConsumeToken(TokenType::RIGHT_BRACE)) {
        AddError("script块期望 '}'");
    }
    
    return scriptBlock;
}

std::string Parser::ParseStringValue() {
    if (IsCurrentToken(TokenType::STRING_LITERAL)) {
        std::string value = CurrentToken().value;
        SkipToken();
        
        // 移除引号
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            return value.substr(1, value.size() - 2);
        }
        return value;
    }
    else if (IsCurrentToken(TokenType::IDENTIFIER) || 
             IsCurrentToken(TokenType::NUMBER) ||
             IsCurrentToken(TokenType::UNQUOTED_LITERAL)) {
        // 无修饰字面量
        std::string value = CurrentToken().value;
        SkipToken();
        return value;
    }
    
    AddError("期望字符串值");
    return "";
}

std::string Parser::ParseIdentifier() {
    if (!IsCurrentToken(TokenType::IDENTIFIER)) {
        AddError("期望标识符");
        return "";
    }
    
    std::string identifier = CurrentToken().value;
    SkipToken();
    return identifier;
}

void Parser::SkipWhitespace() {
    while (IsCurrentToken(TokenType::WHITESPACE) || IsCurrentToken(TokenType::NEWLINE)) {
        SkipToken();
    }
}

bool Parser::IsAttributeAssignment() const {
    return IsCurrentToken(TokenType::COLON) || IsCurrentToken(TokenType::EQUALS);
}

// HtmlElementValidator实现

bool HtmlElementValidator::IsValidElement(const std::string& tagName) {
    if (validElements.empty()) {
        Initialize();
    }
    return validElements.count(tagName) > 0;
}

bool HtmlElementValidator::IsVoidElement(const std::string& tagName) {
    if (voidElements.empty()) {
        Initialize();
    }
    return voidElements.count(tagName) > 0;
}

void HtmlElementValidator::Initialize() {
    // HTML5有效元素列表
    validElements = {
        // 基础元素
        "html", "head", "body", "title", "meta", "link", "style", "script",
        
        // 区块元素
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "header", "footer", "nav", "section", "article", "aside", "main",
        
        // 列表元素
        "ul", "ol", "li", "dl", "dt", "dd",
        
        // 表格元素
        "table", "thead", "tbody", "tfoot", "tr", "td", "th", "caption",
        
        // 表单元素
        "form", "input", "textarea", "button", "select", "option", "label",
        "fieldset", "legend",
        
        // 媒体元素
        "img", "video", "audio", "source", "canvas", "svg",
        
        // 格式化元素
        "b", "i", "u", "s", "strong", "em", "mark", "small", "del", "ins",
        "sub", "sup", "code", "pre", "blockquote", "cite", "q",
        
        // 其他元素
        "a", "br", "hr", "figure", "figcaption", "address", "time",
        "details", "summary", "dialog"
    };
    
    // Void元素（自闭合）
    voidElements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
}

// ParserFactory实现

std::unique_ptr<Parser> ParserFactory::CreateCHTLParser() {
    auto globalMap = std::make_shared<GlobalMap>();
    auto lexer = std::make_shared<Lexer>(globalMap);
    auto stateManager = std::make_shared<StateManager>();
    auto context = std::make_shared<Context>();
    
    return std::make_unique<Parser>(lexer, stateManager, context);
}

} // namespace CHTL