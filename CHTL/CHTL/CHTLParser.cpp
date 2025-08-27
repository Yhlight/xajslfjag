#include "CHTLParser.h"
#include "CHTLContext.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/TemplateNode.h"
#include "CHTLNode/CustomNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/OriginNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/ConfigNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/OperatorNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser(std::shared_ptr<GlobalMap> globalMap)
    : globalMap(globalMap ? globalMap : std::make_shared<GlobalMap>()),
      debugMode(false), inLocalStyle(false), inLocalScript(false) {
    context = std::make_shared<CHTLContext>();
}

CHTLParser::~CHTLParser() = default;

NodePtr CHTLParser::Parse(const std::string& source, const std::string& filename) {
    ClearErrors();
    
    // 创建词法分析器
    lexer = std::make_shared<Lexer>(source, globalMap);
    lexer->SetDebugMode(debugMode);
    
    // 读取第一个Token
    Advance();
    
    try {
        return ParseDocument();
    } catch (const ParseError& e) {
        ReportError(e.what());
        return nullptr;
    }
}

void CHTLParser::Advance() {
    previousToken = currentToken;
    currentToken = lexer->NextToken();
    
    // 跳过空白和注释（除了生成器注释）
    while (currentToken.type == TokenType::WHITESPACE ||
           currentToken.type == TokenType::SINGLE_COMMENT ||
           currentToken.type == TokenType::MULTI_COMMENT) {
        currentToken = lexer->NextToken();
    }
    
    if (debugMode) {
        std::cout << "Token: " << currentToken.ToString() << "\n";
    }
}

Token CHTLParser::Peek(int offset) {
    return lexer->PeekToken(offset);
}

bool CHTLParser::Match(TokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool CHTLParser::Check(TokenType type) {
    return currentToken.type == type;
}

void CHTLParser::Consume(TokenType type, const std::string& message) {
    if (!Check(type)) {
        throw ParseError(message, currentToken.line, currentToken.column);
    }
    Advance();
}

void CHTLParser::ReportError(const std::string& message) {
    std::stringstream ss;
    ss << "解析错误 [" << currentToken.line << ":" << currentToken.column << "]: " << message;
    errors.push_back(ss.str());
}

void CHTLParser::Synchronize() {
    // 错误恢复：跳到下一个语句开始
    while (!Check(TokenType::EOF_TOKEN)) {
        if (previousToken.type == TokenType::SEMICOLON ||
            previousToken.type == TokenType::RIGHT_BRACE) {
            return;
        }
        
        if (IsTopLevelKeyword()) {
            return;
        }
        
        Advance();
    }
}

NodePtr CHTLParser::ParseDocument() {
    auto root = std::make_shared<BaseNode>(NodeType::ELEMENT);
    
    // 处理可能的use语句
    if (Check(TokenType::USE)) {
        auto useNode = ParseUse();
        if (useNode) {
            root->AddChild(useNode);
        }
    }
    
    // 解析顶层元素
    while (!Check(TokenType::EOF_TOKEN)) {
        try {
            auto node = ParseTopLevel();
            if (node) {
                root->AddChild(node);
            }
        } catch (const ParseError& e) {
            ReportError(e.what());
            Synchronize();
        }
    }
    
    return root;
}

NodePtr CHTLParser::ParseTopLevel() {
    // 处理生成器注释
    if (Check(TokenType::GEN_COMMENT)) {
        auto comment = std::make_shared<CommentNode>(CommentType::GENERATOR, currentToken.value);
        comment->SetPosition(currentToken.line, currentToken.column);
        Advance();
        return comment;
    }
    
    // 处理前缀关键字
    if (Check(TokenType::CONFIGURATION)) {
        return ParseConfiguration();
    }
    
    if (Check(TokenType::NAMESPACE)) {
        return ParseNamespace();
    }
    
    if (Check(TokenType::IMPORT)) {
        return ParseImport();
    }
    
    if (Check(TokenType::TEMPLATE)) {
        return ParseTemplate();
    }
    
    if (Check(TokenType::CUSTOM)) {
        return ParseCustom();
    }
    
    if (Check(TokenType::ORIGIN)) {
        return ParseOrigin();
    }
    
    // 处理HTML元素
    if (Check(TokenType::HTML_TAG) || Check(TokenType::IDENTIFIER)) {
        return ParseElement();
    }
    
    // 处理文本节点
    if (Check(TokenType::TEXT)) {
        return ParseText();
    }
    
    throw ParseError("意外的Token: " + currentToken.ToString(), 
                     currentToken.line, currentToken.column);
}

NodePtr CHTLParser::ParseUse() {
    Consume(TokenType::USE, "期望 'use'");
    
    auto useNode = std::make_shared<OperatorNode>(OperatorType::USE);
    useNode->SetPosition(previousToken.line, previousToken.column);
    
    if (Match(TokenType::IDENTIFIER) && previousToken.value == "html5") {
        useNode->SetUseTarget("html5");
    } else if (Match(TokenType::TYPE_CONFIG)) {
        // use @Config Name
        std::string configName = "";
        if (Check(TokenType::IDENTIFIER)) {
            configName = currentToken.value;
            Advance();
        }
        useNode->SetUseTarget("@Config:" + configName);
        
        // 应用配置
        auto config = globalMap->GetConfig(configName);
        if (config) {
            ApplyConfiguration(config);
        }
    } else if (Match(TokenType::CONFIGURATION)) {
        // use [Configuration] @Config Name
        Consume(TokenType::TYPE_CONFIG, "期望 '@Config'");
        std::string configName = "";
        if (Check(TokenType::IDENTIFIER)) {
            configName = currentToken.value;
            Advance();
        }
        useNode->SetUseTarget("[Configuration]@Config:" + configName);
        
        // 应用配置
        auto config = globalMap->GetConfig(configName);
        if (config) {
            ApplyConfiguration(config);
        }
    } else {
        throw ParseError("use语句语法错误", currentToken.line, currentToken.column);
    }
    
    Consume(TokenType::SEMICOLON, "期望 ';'");
    
    return useNode;
}

NodePtr CHTLParser::ParseConfiguration() {
    Consume(TokenType::CONFIGURATION, "期望 '[Configuration]'");
    
    std::string configName = "";
    
    // 检查是否有 @Config Name
    if (Match(TokenType::TYPE_CONFIG)) {
        if (Check(TokenType::IDENTIFIER)) {
            configName = currentToken.value;
            Advance();
        }
    }
    
    auto configNode = std::make_shared<ConfigNode>(configName);
    configNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析配置内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::NAME)) {
            // [Name] 配置块
            auto nameBlock = ParseNameBlock();
            // TODO: 将Name块的内容添加到configNode
        } else if (Check(TokenType::ORIGIN_TYPE)) {
            // [OriginType] 配置块
            auto originTypeBlock = ParseOriginTypeBlock();
            // TODO: 将OriginType块的内容添加到configNode
        } else if (Check(TokenType::IDENTIFIER)) {
            // 配置项
            std::string key = currentToken.value;
            Advance();
            
            Consume(TokenType::EQUALS, "期望 '='");
            
            if (Check(TokenType::STRING_LITERAL) || Check(TokenType::UNQUOTED_LITERAL) ||
                Check(TokenType::IDENTIFIER) || Check(TokenType::NUMBER)) {
                std::string value = currentToken.value;
                Advance();
                configNode->SetConfigItem(key, value);
            } else {
                throw ParseError("期望配置值", currentToken.line, currentToken.column);
            }
            
            Consume(TokenType::SEMICOLON, "期望 ';'");
        } else {
            throw ParseError("配置块中的意外Token", currentToken.line, currentToken.column);
        }
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    // 将配置添加到全局映射
    globalMap->AddConfig(configName, configNode);
    
    // 如果是默认配置，立即应用
    if (configName.empty()) {
        ApplyConfiguration(configNode);
    }
    
    return configNode;
}

NodePtr CHTLParser::ParseElement() {
    std::string tagName = currentToken.value;
    auto elementNode = std::make_shared<ElementNode>(tagName);
    elementNode->SetPosition(currentToken.line, currentToken.column);
    
    Advance();
    
    // 可能有索引访问 div[1]
    if (Match(TokenType::LEFT_BRACKET)) {
        if (Check(TokenType::NUMBER)) {
            elementNode->SetAttribute("index", currentToken.value);
            Advance();
        }
        Consume(TokenType::RIGHT_BRACKET, "期望 ']'");
    }
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 保存当前元素信息
    std::string previousElement = currentElementName;
    currentElementName = tagName;
    elementStack.push(tagName);
    
    // 解析元素内容
    ParseElementBody(elementNode);
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    // 恢复元素信息
    currentElementName = previousElement;
    elementStack.pop();
    
    return elementNode;
}

void CHTLParser::ParseElementBody(NodePtr elementNode) {
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        // 处理属性
        if (Check(TokenType::IDENTIFIER) && Peek().type == TokenType::COLON) {
            std::string attrName = currentToken.value;
            Advance();
            Consume(TokenType::COLON, "期望 ':'");
            
            std::string attrValue = ParseExpression();
            
            auto element = std::dynamic_pointer_cast<ElementNode>(elementNode);
            if (element) {
                element->SetAttribute(attrName, attrValue);
            }
            
            Consume(TokenType::SEMICOLON, "期望 ';'");
            continue;
        }
        
        // 处理局部样式
        if (Check(TokenType::STYLE)) {
            auto styleNode = ParseStyle();
            auto element = std::dynamic_pointer_cast<ElementNode>(elementNode);
            if (element && styleNode) {
                element->SetStyleNode(styleNode);
                ProcessAutoSelectors(styleNode, element);
            }
            continue;
        }
        
        // 处理局部脚本
        if (Check(TokenType::SCRIPT)) {
            auto scriptNode = ParseScript();
            auto element = std::dynamic_pointer_cast<ElementNode>(elementNode);
            if (element && scriptNode) {
                element->SetScriptNode(scriptNode);
                // TODO: 处理脚本的自动化选择器
            }
            continue;
        }
        
        // 处理文本节点
        if (Check(TokenType::TEXT)) {
            auto textNode = ParseText();
            if (textNode) {
                elementNode->AddChild(textNode);
            }
            continue;
        }
        
        // 处理嵌套元素
        if (Check(TokenType::HTML_TAG) || Check(TokenType::IDENTIFIER)) {
            auto childElement = ParseElement();
            if (childElement) {
                elementNode->AddChild(childElement);
            }
            continue;
        }
        
        // 处理操作符
        if (Check(TokenType::DELETE)) {
            auto deleteNode = ParseDelete();
            if (deleteNode) {
                elementNode->AddChild(deleteNode);
            }
            continue;
        }
        
        if (Check(TokenType::INSERT)) {
            auto insertNode = ParseInsert();
            if (insertNode) {
                elementNode->AddChild(insertNode);
            }
            continue;
        }
        
        if (Check(TokenType::EXCEPT)) {
            auto exceptNode = ParseExcept();
            if (exceptNode) {
                elementNode->AddChild(exceptNode);
            }
            continue;
        }
        
        // 处理模板/自定义使用
        if (Check(TokenType::AT)) {
            Advance();
            // TODO: 解析模板/自定义使用
            continue;
        }
        
        throw ParseError("元素内容中的意外Token: " + currentToken.ToString(),
                        currentToken.line, currentToken.column);
    }
}

NodePtr CHTLParser::ParseText() {
    Consume(TokenType::TEXT, "期望 'text'");
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    std::string content = "";
    
    if (Check(TokenType::STRING_LITERAL) || Check(TokenType::UNQUOTED_LITERAL)) {
        content = currentToken.value;
        Advance();
    } else {
        throw ParseError("文本节点需要内容", currentToken.line, currentToken.column);
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    auto textNode = std::make_shared<TextNode>(content);
    textNode->SetPosition(previousToken.line, previousToken.column);
    
    return textNode;
}

NodePtr CHTLParser::ParseStyle() {
    Consume(TokenType::STYLE, "期望 'style'");
    
    bool isLocal = !elementStack.empty();
    auto styleNode = std::make_shared<StyleNode>(isLocal);
    styleNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    inLocalStyle = isLocal;
    
    // 解析样式内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        // 检查是否是选择器
        if ((Check(TokenType::DOT) || Check(TokenType::HASH) || 
             Check(TokenType::AMPERSAND) || Check(TokenType::IDENTIFIER)) &&
            Peek().type == TokenType::LEFT_BRACE) {
            
            // 解析选择器规则
            StyleRule rule;
            rule.isInlineStyle = false;
            rule.selector = ParseSelector();
            
            Consume(TokenType::LEFT_BRACE, "期望 '{'");
            
            // 解析属性
            while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
                if (Check(TokenType::IDENTIFIER)) {
                    std::string propName = currentToken.value;
                    Advance();
                    
                    Consume(TokenType::COLON, "期望 ':'");
                    
                    std::string propValue = ParseExpression();
                    rule.properties[propName] = propValue;
                    
                    Consume(TokenType::SEMICOLON, "期望 ';'");
                }
            }
            
            Consume(TokenType::RIGHT_BRACE, "期望 '}'");
            
            styleNode->AddRule(rule);
        } else if (Check(TokenType::IDENTIFIER)) {
            // 内联样式属性
            std::string propName = currentToken.value;
            Advance();
            
            Consume(TokenType::COLON, "期望 ':'");
            
            std::string propValue = ParseExpression();
            styleNode->AddInlineProperty(propName, propValue);
            
            Consume(TokenType::SEMICOLON, "期望 ';'");
        } else if (Check(TokenType::AT)) {
            // 模板样式组使用
            Advance();
            // TODO: 解析模板样式组使用
        } else {
            throw ParseError("样式块中的意外Token", currentToken.line, currentToken.column);
        }
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    inLocalStyle = false;
    
    return styleNode;
}

std::string CHTLParser::ParseSelector() {
    std::string selector = "";
    
    if (Match(TokenType::DOT)) {
        selector = ".";
        if (Check(TokenType::IDENTIFIER)) {
            selector += currentToken.value;
            Advance();
        }
    } else if (Match(TokenType::HASH)) {
        selector = "#";
        if (Check(TokenType::IDENTIFIER)) {
            selector += currentToken.value;
            Advance();
        }
    } else if (Match(TokenType::AMPERSAND)) {
        selector = "&";
        // 可能有伪类
        if (Match(TokenType::COLON)) {
            selector += ":";
            if (Check(TokenType::IDENTIFIER)) {
                selector += currentToken.value;
                Advance();
            }
        }
    } else if (Check(TokenType::IDENTIFIER)) {
        selector = currentToken.value;
        Advance();
    }
    
    return selector;
}

NodePtr CHTLParser::ParseScript() {
    Consume(TokenType::SCRIPT, "期望 'script'");
    
    bool isLocal = !elementStack.empty();
    auto scriptNode = std::make_shared<ScriptNode>(isLocal);
    scriptNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    inLocalScript = isLocal;
    
    // 收集脚本内容直到遇到匹配的右大括号
    std::string content = "";
    int braceCount = 1;
    
    while (braceCount > 0 && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::LEFT_BRACE)) {
            braceCount++;
        } else if (Check(TokenType::RIGHT_BRACE)) {
            braceCount--;
            if (braceCount == 0) break;
        }
        
        content += currentToken.value;
        if (currentToken.type == TokenType::NEWLINE) {
            content += "\n";
        } else {
            content += " ";
        }
        
        Advance();
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    scriptNode->SetContent(content);
    inLocalScript = false;
    
    return scriptNode;
}

std::string CHTLParser::ParseExpression() {
    if (Check(TokenType::STRING_LITERAL)) {
        std::string value = currentToken.value;
        Advance();
        return value;
    } else if (Check(TokenType::UNQUOTED_LITERAL)) {
        std::string value = currentToken.value;
        Advance();
        return value;
    } else if (Check(TokenType::NUMBER)) {
        std::string value = currentToken.value;
        Advance();
        return value;
    } else if (Check(TokenType::IDENTIFIER)) {
        std::string value = currentToken.value;
        Advance();
        
        // 可能是变量组使用 ThemeColor(tableColor)
        if (Match(TokenType::LEFT_PAREN)) {
            value += "(";
            // TODO: 解析变量组参数
            value += ")";
        }
        
        return value;
    }
    
    throw ParseError("期望表达式", currentToken.line, currentToken.column);
}

void CHTLParser::ProcessAutoSelectors(NodePtr styleNode, NodePtr elementNode) {
    auto style = std::dynamic_pointer_cast<StyleNode>(styleNode);
    auto element = std::dynamic_pointer_cast<ElementNode>(elementNode);
    
    if (!style || !element) return;
    
    // 获取配置
    auto config = globalMap->GetDefaultConfig();
    bool disableAutoClass = false;
    bool disableAutoId = false;
    
    if (config) {
        disableAutoClass = config->GetConfigValue("DISABLE_STYLE_AUTO_ADD_CLASS") == "true";
        disableAutoId = config->GetConfigValue("DISABLE_STYLE_AUTO_ADD_ID") == "true";
    }
    
    // 自动添加类名
    if (!disableAutoClass && !element->HasAttribute("class")) {
        std::string className = style->GetFirstClassSelector();
        if (!className.empty()) {
            element->SetAttribute("class", className);
        }
    }
    
    // 自动添加ID
    if (!disableAutoId && !element->HasAttribute("id")) {
        std::string idName = style->GetFirstIdSelector();
        if (!idName.empty()) {
            element->SetAttribute("id", idName);
        }
    }
}

void CHTLParser::ApplyConfiguration(std::shared_ptr<ConfigNode> config) {
    if (!config || !lexer) return;
    
    // 将配置应用到词法分析器
    lexer->ApplyConfiguration(config);
}

bool CHTLParser::IsTopLevelKeyword() {
    return Check(TokenType::CONFIGURATION) ||
           Check(TokenType::NAMESPACE) ||
           Check(TokenType::IMPORT) ||
           Check(TokenType::TEMPLATE) ||
           Check(TokenType::CUSTOM) ||
           Check(TokenType::ORIGIN) ||
           Check(TokenType::HTML_TAG);
}

// TODO: 实现其他解析方法...

} // namespace CHTL