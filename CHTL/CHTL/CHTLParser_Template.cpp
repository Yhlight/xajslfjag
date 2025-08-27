#include "CHTLParser.h"
#include "CHTLNode/TemplateNode.h"
#include "CHTLNode/CustomNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/OriginNode.h"
#include "CHTLNode/OperatorNode.h"

namespace CHTL {

// 模板解析实现
NodePtr CHTLParser::ParseTemplate() {
    Consume(TokenType::TEMPLATE, "期望 '[Template]'");
    
    if (Match(TokenType::TYPE_STYLE)) {
        return ParseTemplateStyle();
    } else if (Match(TokenType::TYPE_ELEMENT)) {
        return ParseTemplateElement();
    } else if (Match(TokenType::TYPE_VAR)) {
        return ParseTemplateVar();
    } else {
        throw ParseError("期望模板类型 (@Style, @Element, @Var)", 
                        currentToken.line, currentToken.column);
    }
}

NodePtr CHTLParser::ParseTemplateStyle() {
    // [Template] @Style Name
    std::string templateName;
    if (Check(TokenType::IDENTIFIER)) {
        templateName = currentToken.value;
        Advance();
    } else {
        throw ParseError("期望模板名称", currentToken.line, currentToken.column);
    }
    
    auto templateNode = std::make_shared<TemplateNode>(TemplateType::STYLE, templateName);
    templateNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析样式属性
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::IDENTIFIER)) {
            std::string propName = currentToken.value;
            Advance();
            
            Consume(TokenType::COLON, "期望 ':'");
            
            std::string propValue = ParseExpression();
            templateNode->AddStyleProperty(propName, propValue);
            
            Consume(TokenType::SEMICOLON, "期望 ';'");
        } else if (Check(TokenType::AT) && Peek().type == TokenType::TYPE_STYLE) {
            // 继承其他样式组模板
            Advance(); // 跳过 @
            Advance(); // 跳过 Style
            
            if (Check(TokenType::IDENTIFIER)) {
                templateNode->AddInheritedTemplate(currentToken.value);
                Advance();
            }
            
            Consume(TokenType::SEMICOLON, "期望 ';'");
        } else {
            throw ParseError("样式模板中的意外Token", currentToken.line, currentToken.column);
        }
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    // 添加到全局映射
    globalMap->AddTemplate("@Style", templateName, templateNode);
    
    return templateNode;
}

NodePtr CHTLParser::ParseTemplateElement() {
    // [Template] @Element Name
    std::string templateName;
    if (Check(TokenType::IDENTIFIER)) {
        templateName = currentToken.value;
        Advance();
    } else {
        throw ParseError("期望模板名称", currentToken.line, currentToken.column);
    }
    
    auto templateNode = std::make_shared<TemplateNode>(TemplateType::ELEMENT, templateName);
    templateNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析元素内容
    auto contentNode = std::make_shared<BaseNode>(NodeType::ELEMENT);
    
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::HTML_TAG) || Check(TokenType::IDENTIFIER)) {
            auto element = ParseElement();
            if (element) {
                contentNode->AddChild(element);
            }
        } else if (Check(TokenType::TEXT)) {
            auto text = ParseText();
            if (text) {
                contentNode->AddChild(text);
            }
        } else if (Check(TokenType::AT) && Peek().type == TokenType::TYPE_ELEMENT) {
            // 使用其他元素模板
            Advance();
            Advance();
            if (Check(TokenType::IDENTIFIER)) {
                // TODO: 解析元素模板使用
                Advance();
            }
            Consume(TokenType::SEMICOLON, "期望 ';'");
        } else {
            throw ParseError("元素模板中的意外Token", currentToken.line, currentToken.column);
        }
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    templateNode->SetContent(contentNode);
    
    // 添加到全局映射
    globalMap->AddTemplate("@Element", templateName, templateNode);
    
    return templateNode;
}

NodePtr CHTLParser::ParseTemplateVar() {
    // [Template] @Var Name
    std::string templateName;
    if (Check(TokenType::IDENTIFIER)) {
        templateName = currentToken.value;
        Advance();
    } else {
        throw ParseError("期望模板名称", currentToken.line, currentToken.column);
    }
    
    auto templateNode = std::make_shared<TemplateNode>(TemplateType::VAR, templateName);
    templateNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析变量定义
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::IDENTIFIER)) {
            std::string varName = currentToken.value;
            Advance();
            
            Consume(TokenType::COLON, "期望 ':'");
            
            std::string varValue = ParseExpression();
            templateNode->AddVariable(varName, varValue);
            
            Consume(TokenType::SEMICOLON, "期望 ';'");
        } else {
            throw ParseError("变量组模板中的意外Token", currentToken.line, currentToken.column);
        }
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    // 添加到全局映射
    globalMap->AddTemplate("@Var", templateName, templateNode);
    
    return templateNode;
}

// 自定义解析实现
NodePtr CHTLParser::ParseCustom() {
    Consume(TokenType::CUSTOM, "期望 '[Custom]'");
    
    if (Match(TokenType::TYPE_STYLE)) {
        return ParseCustomStyle();
    } else if (Match(TokenType::TYPE_ELEMENT)) {
        return ParseCustomElement();
    } else if (Match(TokenType::TYPE_VAR)) {
        return ParseCustomVar();
    } else {
        throw ParseError("期望自定义类型 (@Style, @Element, @Var)", 
                        currentToken.line, currentToken.column);
    }
}

NodePtr CHTLParser::ParseCustomStyle() {
    // [Custom] @Style Name
    std::string customName;
    if (Check(TokenType::IDENTIFIER)) {
        customName = currentToken.value;
        Advance();
    } else {
        throw ParseError("期望自定义名称", currentToken.line, currentToken.column);
    }
    
    auto customNode = std::make_shared<CustomNode>(CustomType::STYLE, customName);
    customNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析自定义样式内容
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::IDENTIFIER)) {
            std::string propName = currentToken.value;
            Advance();
            
            if (Match(TokenType::COMMA)) {
                // 无值属性
                customNode->AddStyleProperty(propName, "");
            } else if (Match(TokenType::COLON)) {
                // 有值属性
                std::string propValue = ParseExpression();
                customNode->AddStyleProperty(propName, propValue);
            }
            
            if (Check(TokenType::SEMICOLON)) {
                Advance();
            }
        } else if (Check(TokenType::AT) && Peek().type == TokenType::TYPE_STYLE) {
            // 继承其他样式组
            Advance();
            Advance();
            
            if (Check(TokenType::IDENTIFIER)) {
                std::string inheritName = currentToken.value;
                Advance();
                
                // 可能有特例化
                if (Match(TokenType::LEFT_BRACE)) {
                    // TODO: 解析特例化内容
                    // 跳过到右大括号
                    int braceCount = 1;
                    while (braceCount > 0 && !Check(TokenType::EOF_TOKEN)) {
                        if (Check(TokenType::LEFT_BRACE)) braceCount++;
                        else if (Check(TokenType::RIGHT_BRACE)) braceCount--;
                        Advance();
                    }
                }
                
                customNode->AddInheritedCustom("@Style", inheritName);
            }
            
            if (Check(TokenType::SEMICOLON)) {
                Advance();
            }
        } else if (Check(TokenType::DELETE)) {
            // delete操作
            Advance();
            
            // TODO: 解析删除目标
            while (!Check(TokenType::SEMICOLON) && !Check(TokenType::EOF_TOKEN)) {
                if (Check(TokenType::IDENTIFIER)) {
                    customNode->AddDeletedProperty(currentToken.value);
                    Advance();
                    if (Match(TokenType::COMMA)) {
                        // 继续解析下一个
                    }
                }
            }
            
            Consume(TokenType::SEMICOLON, "期望 ';'");
        } else {
            throw ParseError("自定义样式中的意外Token", currentToken.line, currentToken.column);
        }
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    // 添加到全局映射
    globalMap->AddCustom("@Style", customName, customNode);
    
    return customNode;
}

NodePtr CHTLParser::ParseCustomElement() {
    // [Custom] @Element Name
    std::string customName;
    if (Check(TokenType::IDENTIFIER)) {
        customName = currentToken.value;
        Advance();
    } else {
        throw ParseError("期望自定义名称", currentToken.line, currentToken.column);
    }
    
    auto customNode = std::make_shared<CustomNode>(CustomType::ELEMENT, customName);
    customNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析元素内容
    auto contentNode = std::make_shared<BaseNode>(NodeType::ELEMENT);
    
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::HTML_TAG) || Check(TokenType::IDENTIFIER)) {
            auto element = ParseElement();
            if (element) {
                contentNode->AddChild(element);
            }
        } else if (Check(TokenType::TEXT)) {
            auto text = ParseText();
            if (text) {
                contentNode->AddChild(text);
            }
        } else if (Check(TokenType::AT) && Peek().type == TokenType::TYPE_ELEMENT) {
            // 使用其他元素
            Advance();
            Advance();
            if (Check(TokenType::IDENTIFIER)) {
                // TODO: 解析元素使用
                Advance();
            }
            Consume(TokenType::SEMICOLON, "期望 ';'");
        } else {
            throw ParseError("自定义元素中的意外Token", currentToken.line, currentToken.column);
        }
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    customNode->SetContent(contentNode);
    
    // 添加到全局映射
    globalMap->AddCustom("@Element", customName, customNode);
    
    return customNode;
}

NodePtr CHTLParser::ParseCustomVar() {
    // [Custom] @Var Name
    std::string customName;
    if (Check(TokenType::IDENTIFIER)) {
        customName = currentToken.value;
        Advance();
    } else {
        throw ParseError("期望自定义名称", currentToken.line, currentToken.column);
    }
    
    auto customNode = std::make_shared<CustomNode>(CustomType::VAR, customName);
    customNode->SetPosition(previousToken.line, previousToken.column);
    
    Consume(TokenType::LEFT_BRACE, "期望 '{'");
    
    // 解析变量定义
    while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::EOF_TOKEN)) {
        if (Check(TokenType::IDENTIFIER)) {
            std::string varName = currentToken.value;
            Advance();
            
            Consume(TokenType::COLON, "期望 ':'");
            
            std::string varValue = ParseExpression();
            customNode->AddVariable(varName, varValue);
            
            Consume(TokenType::SEMICOLON, "期望 ';'");
        } else {
            throw ParseError("自定义变量组中的意外Token", currentToken.line, currentToken.column);
        }
    }
    
    Consume(TokenType::RIGHT_BRACE, "期望 '}'");
    
    // 添加到全局映射
    globalMap->AddCustom("@Var", customName, customNode);
    
    return customNode;
}

// 其他解析方法实现...

} // namespace CHTL