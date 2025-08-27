#include "CHTLCompiler.h"
#include "../../Common/UnquotedLiteralManager.h"
#include "../../Common/IndexAccessManager.h"
#include "../../Common/PositionKeywordManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace CHTL {
namespace Compiler {

// ============================================================================
// CHTLParser 实现
// ============================================================================

CHTLParser::CHTLParser() 
    : current_token_index(0), strict_mode(false), debug_mode(false) {
    lexer = std::make_unique<Token::CHTLLexer>();
    state_machine = std::make_unique<Common::StateMachine>();
    context = std::make_unique<Common::Context>();
    global_map = Common::GlobalMap::getInstance();
}

CHTLParser::~CHTLParser() = default;

std::shared_ptr<AST::ASTNode> CHTLParser::parse(const std::vector<Scanner::CodeFragment>& fragments) {
    clearErrors();
    
    try {
        // 创建根节点
        auto root = std::make_shared<AST::RootNode>();
        
        // 解析每个代码片段
        for (const auto& fragment : fragments) {
            if (fragment.type == Scanner::FragmentType::CHTL) {
                auto node = parseFragment(fragment);
                if (node) {
                    root->addChild(node);
                }
            }
        }
        
        return root;
        
    } catch (const std::exception& e) {
        reportError("Parse exception: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<AST::ASTNode> CHTLParser::parseFragment(const Scanner::CodeFragment& fragment) {
    // 词法分析
    tokens = lexer->tokenize(fragment.content);
    current_token_index = 0;
    
    if (tokens.empty()) {
        return nullptr;
    }
    
    // 开始解析
    return parseTopLevelElement();
}

std::shared_ptr<AST::ASTNode> CHTLParser::parseTopLevelElement() {
    while (!isAtEnd()) {
        Token::TokenType type = currentToken().type;
        
        switch (type) {
            case Token::TokenType::HTML_ELEMENT:
                return parseHTMLElement();
                
            case Token::TokenType::TEXT:
                return parseTextNode();
                
            case Token::TokenType::TEMPLATE:
                return parseTemplateElement();
                
            case Token::TokenType::CUSTOM:
                return parseCustomElement();
                
            case Token::TokenType::ORIGIN:
                return parseOriginBlock();
                
            case Token::TokenType::CONFIGURATION:
                return parseConfiguration();
                
            case Token::TokenType::NAMESPACE:
                return parseNamespace();
                
            case Token::TokenType::IMPORT:
                return parseImport();
                
            case Token::TokenType::INHERIT:
                return parseInherit();
                
            case Token::TokenType::DELETE:
                return parseDelete();
                
            case Token::TokenType::INSERT:
                return parseInsert();
                
            default:
                reportError("Unexpected token: " + currentToken().value);
                advance();
                break;
        }
    }
    
    return nullptr;
}

std::shared_ptr<AST::HTMLElementNode> CHTLParser::parseHTMLElement() {
    auto element = std::make_shared<AST::HTMLElementNode>();
    
    // 解析元素名
    if (currentToken().type == Token::TokenType::HTML_ELEMENT) {
        element->tag_name = currentToken().value;
        advance();
    } else {
        reportError("Expected HTML element name");
        return nullptr;
    }
    
    // 进入HTML元素状态
    enterState(Common::StateType::HTML_ELEMENT, element->tag_name);
    
    // 解析可能的索引访问 div[1]
    if (match(Token::TokenType::LEFT_BRACKET)) {
        auto index_manager = std::make_unique<Common::IndexAccessManager>();
        
        std::string index_expr;
        while (!match(Token::TokenType::RIGHT_BRACKET) && !isAtEnd()) {
            index_expr += currentToken().value;
            advance();
        }
        
        auto index_result = index_manager->parseIndexAccess(index_expr);
        if (index_result.is_valid) {
            element->setContextData("index_access", index_expr);
        }
    }
    
    // 解析属性或内容块
    if (match(Token::TokenType::LEFT_BRACE)) {
        
        while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
            Token::TokenType type = currentToken().type;
            
            if (type == Token::TokenType::IDENTIFIER) {
                // 可能是属性或子元素
                std::string name = currentToken().value;
                advance();
                
                if (match(Token::TokenType::COLON) || match(Token::TokenType::EQUALS)) {
                    // 这是属性
                    auto attr = parseAttribute();
                    if (attr) {
                        attr->name = name;
                        element->addAttribute(attr);
                    }
                } else {
                    // 这是子元素，回退
                    current_token_index--;
                    auto child = parseTopLevelElement();
                    if (child) {
                        element->addChild(child);
                    }
                }
            } else if (type == Token::TokenType::STYLE) {
                auto style = parseStyleBlock();
                if (style) {
                    element->addChild(style);
                }
            } else if (type == Token::TokenType::SCRIPT) {
                auto script = parseScriptBlock();
                if (script) {
                    element->addChild(script);
                }
            } else {
                auto child = parseTopLevelElement();
                if (child) {
                    element->addChild(child);
                }
            }
        }
    }
    
    exitState();
    return element;
}

std::shared_ptr<AST::AttributeNode> CHTLParser::parseAttribute() {
    auto attr = std::make_shared<AST::AttributeNode>();
    
    // 解析属性值
    if (!isAtEnd()) {
        std::string value = currentToken().value;
        
        // 处理无修饰字面量
        auto literal_manager = std::make_unique<Common::UnquotedLiteralManager>();
        if (literal_manager->isValidUnquotedLiteral(value, Common::UnquotedLiteralContext::ELEMENT_ATTRIBUTE)) {
            // 是有效的无修饰字面量
            attr->value = literal_manager->normalizeUnquotedLiteral(value, Common::UnquotedLiteralType::ATTRIBUTE_VALUE);
        } else {
            // 普通字符串值
            attr->value = value;
        }
        
        advance();
    }
    
    if (match(Token::TokenType::SEMICOLON)) {
        // 可选的分号
    }
    
    return attr;
}

std::shared_ptr<AST::TextNode> CHTLParser::parseTextNode() {
    auto text = std::make_shared<AST::TextNode>();
    
    consume(Token::TokenType::TEXT, "Expected 'text' keyword");
    consume(Token::TokenType::LEFT_BRACE, "Expected '{' after 'text'");
    
    // 解析文本内容
    std::string content;
    while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
        content += currentToken().value;
        if (peekToken().type != Token::TokenType::RIGHT_BRACE) {
            content += " ";
        }
        advance();
    }
    
    // 处理无修饰字面量
    auto literal_manager = std::make_unique<Common::UnquotedLiteralManager>();
    if (literal_manager->isValidUnquotedLiteral(content, Common::UnquotedLiteralContext::TEXT_BLOCK)) {
        text->content = literal_manager->normalizeUnquotedLiteral(content, Common::UnquotedLiteralType::TEXT_CONTENT);
    } else {
        text->content = content;
    }
    
    return text;
}

std::shared_ptr<AST::StyleBlockNode> CHTLParser::parseStyleBlock() {
    auto style = std::make_shared<AST::StyleBlockNode>();
    
    consume(Token::TokenType::STYLE, "Expected 'style' keyword");
    consume(Token::TokenType::LEFT_BRACE, "Expected '{' after 'style'");
    
    // 进入样式块状态
    enterState(Common::StateType::LOCAL_STYLE_BLOCK, "style");
    
    while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto selector = parseStyleSelector();
        if (selector) {
            style->addChild(selector);
        }
    }
    
    exitState();
    return style;
}

std::shared_ptr<AST::StyleSelectorNode> CHTLParser::parseStyleSelector() {
    auto selector = std::make_shared<AST::StyleSelectorNode>();
    
    // 解析选择器
    std::string selector_text;
    while (!match(Token::TokenType::LEFT_BRACE) && !isAtEnd()) {
        selector_text += currentToken().value;
        advance();
    }
    
    selector->selector = selector_text;
    
    // 解析样式属性
    while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto property = parseStyleProperty();
        if (property) {
            selector->addProperty(property);
        }
    }
    
    return selector;
}

std::shared_ptr<AST::StylePropertyNode> CHTLParser::parseStyleProperty() {
    auto property = std::make_shared<AST::StylePropertyNode>();
    
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        property->property = currentToken().value;
        advance();
        
        if (match(Token::TokenType::COLON) || match(Token::TokenType::EQUALS)) {
            // 解析属性值
            std::string value;
            while (!match(Token::TokenType::SEMICOLON) && 
                   !match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
                value += currentToken().value;
                advance();
            }
            
            // 处理无修饰字面量
            auto literal_manager = std::make_unique<Common::UnquotedLiteralManager>();
            if (literal_manager->isValidUnquotedLiteral(value, Common::UnquotedLiteralContext::STYLE_PROPERTY)) {
                property->value = literal_manager->normalizeUnquotedLiteral(value, Common::UnquotedLiteralType::CSS_PROPERTY_VALUE);
            } else {
                property->value = value;
            }
        }
    }
    
    return property;
}

std::shared_ptr<AST::ScriptBlockNode> CHTLParser::parseScriptBlock() {
    auto script = std::make_shared<AST::ScriptBlockNode>();
    
    consume(Token::TokenType::SCRIPT, "Expected 'script' keyword");
    consume(Token::TokenType::LEFT_BRACE, "Expected '{' after 'script'");
    
    // 进入脚本块状态
    enterState(Common::StateType::LOCAL_SCRIPT_BLOCK, "script");
    
    // 解析脚本内容（可能包含CHTL JS语法）
    std::string content;
    int brace_count = 0;
    
    while (!isAtEnd()) {
        if (currentToken().type == Token::TokenType::LEFT_BRACE) {
            brace_count++;
        } else if (currentToken().type == Token::TokenType::RIGHT_BRACE) {
            if (brace_count == 0) {
                advance(); // 消费结束的大括号
                break;
            }
            brace_count--;
        }
        
        content += currentToken().value + " ";
        advance();
    }
    
    script->content = content;
    script->setContextData("language", "javascript");
    
    // 检测是否包含CHTL JS语法
    Scanner::CHTLUnifiedScanner scanner;
    if (scanner.containsCHTLJSSyntax(content)) {
        script->setContextData("contains_chtljs", "true");
    }
    
    exitState();
    return script;
}

std::shared_ptr<AST::TemplateElementNode> CHTLParser::parseTemplateElement() {
    auto element = std::make_shared<AST::TemplateElementNode>();
    
    consume(Token::TokenType::TEMPLATE, "Expected '[Template]'");
    
    // 解析模板类型
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        std::string type = currentToken().value;
        advance();
        
        if (type == "@Style") {
            return parseTemplateStyle();
        } else if (type == "@Element") {
            element->element_type = "Element";
        } else if (type == "@Var") {
            element->element_type = "Var";
        }
    }
    
    // 解析模板名称
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        element->template_name = currentToken().value;
        advance();
    }
    
    // 解析模板内容
    if (match(Token::TokenType::LEFT_BRACE)) {
        while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto child = parseTopLevelElement();
            if (child) {
                element->addChild(child);
            }
        }
    }
    
    return element;
}

std::shared_ptr<AST::TemplateStyleNode> CHTLParser::parseTemplateStyle() {
    auto style = std::make_shared<AST::TemplateStyleNode>();
    
    // 解析样式名称
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        style->style_name = currentToken().value;
        advance();
    }
    
    // 解析样式内容
    if (match(Token::TokenType::LEFT_BRACE)) {
        while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto selector = parseStyleSelector();
            if (selector) {
                style->addChild(selector);
            }
        }
    }
    
    return style;
}

std::shared_ptr<AST::CustomElementNode> CHTLParser::parseCustomElement() {
    auto element = std::make_shared<AST::CustomElementNode>();
    
    consume(Token::TokenType::CUSTOM, "Expected '[Custom]'");
    
    // 解析自定义类型
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        std::string type = currentToken().value;
        advance();
        
        if (type == "@Style") {
            return parseCustomStyle();
        } else if (type == "@Element") {
            element->element_type = "Element";
        } else if (type == "@Var") {
            element->element_type = "Var";
        }
    }
    
    // 解析自定义名称
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        element->custom_name = currentToken().value;
        advance();
    }
    
    // 解析自定义内容
    if (match(Token::TokenType::LEFT_BRACE)) {
        while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto child = parseTopLevelElement();
            if (child) {
                element->addChild(child);
            }
        }
    }
    
    return element;
}

std::shared_ptr<AST::CustomStyleNode> CHTLParser::parseCustomStyle() {
    auto style = std::make_shared<AST::CustomStyleNode>();
    
    // 解析样式名称
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        style->style_name = currentToken().value;
        advance();
    }
    
    // 解析样式内容
    if (match(Token::TokenType::LEFT_BRACE)) {
        while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto selector = parseStyleSelector();
            if (selector) {
                style->addChild(selector);
            }
        }
    }
    
    return style;
}

std::shared_ptr<AST::InheritNode> CHTLParser::parseInherit() {
    auto inherit = std::make_shared<AST::InheritNode>();
    
    consume(Token::TokenType::INHERIT, "Expected 'inherit' keyword");
    
    // 解析继承源
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        inherit->source = currentToken().value;
        advance();
    }
    
    return inherit;
}

std::shared_ptr<AST::DeleteNode> CHTLParser::parseDelete() {
    auto delete_node = std::make_shared<AST::DeleteNode>();
    
    consume(Token::TokenType::DELETE, "Expected 'delete' keyword");
    
    // 解析删除目标
    std::string target;
    while (!match(Token::TokenType::SEMICOLON) && !isAtEnd()) {
        target += currentToken().value;
        advance();
    }
    
    delete_node->target = target;
    
    return delete_node;
}

std::shared_ptr<AST::InsertNode> CHTLParser::parseInsert() {
    auto insert = std::make_shared<AST::InsertNode>();
    
    consume(Token::TokenType::INSERT, "Expected 'insert' keyword");
    
    // 解析位置关键字
    auto position_manager = std::make_unique<Common::PositionKeywordManager>();
    
    std::string statement;
    while (!match(Token::TokenType::LEFT_BRACE) && !isAtEnd()) {
        statement += currentToken().value + " ";
        advance();
    }
    
    auto position_result = position_manager->parseInsertStatement(statement);
    if (position_result.is_valid) {
        insert->position_type = static_cast<int>(position_result.position_type);
        insert->target = position_result.target_element;
    }
    
    // 解析插入内容
    if (match(Token::TokenType::LEFT_BRACE)) {
        while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto child = parseTopLevelElement();
            if (child) {
                insert->addChild(child);
            }
        }
    }
    
    return insert;
}

std::shared_ptr<AST::OriginBlockNode> CHTLParser::parseOriginBlock() {
    auto origin = std::make_shared<AST::OriginBlockNode>();
    
    consume(Token::TokenType::ORIGIN, "Expected '[Origin]'");
    
    // 解析原始类型
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        origin->origin_type = currentToken().value;
        advance();
    }
    
    // 解析原始内容
    if (match(Token::TokenType::LEFT_BRACE)) {
        std::string content;
        int brace_count = 0;
        
        while (!isAtEnd()) {
            if (currentToken().type == Token::TokenType::LEFT_BRACE) {
                brace_count++;
            } else if (currentToken().type == Token::TokenType::RIGHT_BRACE) {
                if (brace_count == 0) {
                    advance();
                    break;
                }
                brace_count--;
            }
            
            content += currentToken().value;
            advance();
        }
        
        origin->content = content;
    }
    
    return origin;
}

std::shared_ptr<AST::ConfigurationNode> CHTLParser::parseConfiguration() {
    auto config = std::make_shared<AST::ConfigurationNode>();
    
    consume(Token::TokenType::CONFIGURATION, "Expected '[Configuration]'");
    
    // 解析配置名称
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        config->config_name = currentToken().value;
        advance();
    }
    
    // 解析配置内容
    if (match(Token::TokenType::LEFT_BRACE)) {
        while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
            // 解析配置项
            if (currentToken().type == Token::TokenType::IDENTIFIER) {
                std::string key = currentToken().value;
                advance();
                
                if (match(Token::TokenType::EQUALS)) {
                    std::string value;
                    while (!match(Token::TokenType::SEMICOLON) && 
                           !match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
                        value += currentToken().value;
                        advance();
                    }
                    config->setConfigValue(key, value);
                }
            } else {
                advance(); // 跳过未识别的token
            }
        }
    }
    
    return config;
}

std::shared_ptr<AST::NamespaceNode> CHTLParser::parseNamespace() {
    auto ns = std::make_shared<AST::NamespaceNode>();
    
    consume(Token::TokenType::NAMESPACE, "Expected '[Namespace]'");
    
    // 解析命名空间名称
    if (currentToken().type == Token::TokenType::IDENTIFIER) {
        ns->namespace_name = currentToken().value;
        advance();
    }
    
    // 解析命名空间内容
    if (match(Token::TokenType::LEFT_BRACE)) {
        while (!match(Token::TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto child = parseTopLevelElement();
            if (child) {
                ns->addChild(child);
            }
        }
    }
    
    return ns;
}

std::shared_ptr<AST::ImportNode> CHTLParser::parseImport() {
    auto import = std::make_shared<AST::ImportNode>();
    
    consume(Token::TokenType::IMPORT, "Expected '[Import]'");
    
    // 解析导入语句
    std::string statement;
    while (!match(Token::TokenType::SEMICOLON) && !isAtEnd()) {
        statement += currentToken().value + " ";
        advance();
    }
    
    // 使用ImportManager解析
    global_map->getImportManager()->parseImportStatement(statement);
    import->import_statement = statement;
    
    return import;
}

// 工具方法实现
Token::CHTLToken& CHTLParser::currentToken() {
    if (current_token_index >= tokens.size()) {
        static Token::CHTLToken eof_token;
        eof_token.type = Token::TokenType::EOF_TOKEN;
        return eof_token;
    }
    return tokens[current_token_index];
}

Token::CHTLToken& CHTLParser::peekToken(int offset) {
    size_t peek_index = current_token_index + offset;
    if (peek_index >= tokens.size()) {
        static Token::CHTLToken eof_token;
        eof_token.type = Token::TokenType::EOF_TOKEN;
        return eof_token;
    }
    return tokens[peek_index];
}

bool CHTLParser::isAtEnd() const {
    return current_token_index >= tokens.size() || 
           currentToken().type == Token::TokenType::EOF_TOKEN;
}

void CHTLParser::advance() {
    if (!isAtEnd()) {
        current_token_index++;
    }
}

bool CHTLParser::match(Token::TokenType type) {
    if (currentToken().type == type) {
        advance();
        return true;
    }
    return false;
}

void CHTLParser::consume(Token::TokenType type, const std::string& error_message) {
    if (currentToken().type == type) {
        advance();
    } else {
        reportError(error_message);
    }
}

void CHTLParser::reportError(const std::string& message) {
    errors.push_back("Parse error: " + message);
}

void CHTLParser::reportWarning(const std::string& message) {
    warnings.push_back("Parse warning: " + message);
}

void CHTLParser::enterState(Common::StateType state, const std::string& name) {
    state_machine->transitionTo(state, name);
}

void CHTLParser::exitState() {
    state_machine->popState();
}

// ============================================================================
// CHTLGenerator 实现
// ============================================================================

CHTLGenerator::CHTLGenerator() {
    global_map = Common::GlobalMap::getInstance();
    selector_automation = std::make_unique<Common::SelectorAutomationManager>();
}

CHTLGenerator::~CHTLGenerator() = default;

CHTLCompilationResult CHTLGenerator::generate(std::shared_ptr<AST::ASTNode> ast) {
    CHTLCompilationResult result;
    
    if (!ast) {
        result.errors.push_back("Invalid AST provided for generation");
        result.success = false;
        return result;
    }
    
    try {
        // 重置生成状态
        html_output.clear();
        css_output.clear();
        js_output.clear();
        generated_classes.clear();
        generated_ids.clear();
        
        // 生成代码
        generateNode(ast);
        
        // 合并结果
        result.html = html_output.str();
        result.css = css_output.str();
        result.javascript = js_output.str();
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errors.push_back("Generation exception: " + std::string(e.what()));
        result.success = false;
    }
    
    return result;
}

void CHTLGenerator::generateNode(std::shared_ptr<AST::ASTNode> node) {
    if (!node) return;
    
    switch (node->getType()) {
        case AST::NodeType::ROOT:
            generateRootNode(std::static_pointer_cast<AST::RootNode>(node));
            break;
            
        case AST::NodeType::HTML_ELEMENT:
            generateHTMLElement(std::static_pointer_cast<AST::HTMLElementNode>(node));
            break;
            
        case AST::NodeType::TEXT:
            generateTextNode(std::static_pointer_cast<AST::TextNode>(node));
            break;
            
        case AST::NodeType::STYLE_BLOCK:
            generateStyleBlock(std::static_pointer_cast<AST::StyleBlockNode>(node));
            break;
            
        case AST::NodeType::SCRIPT_BLOCK:
            generateScriptBlock(std::static_pointer_cast<AST::ScriptBlockNode>(node));
            break;
            
        case AST::NodeType::TEMPLATE_ELEMENT:
            generateTemplateElement(std::static_pointer_cast<AST::TemplateElementNode>(node));
            break;
            
        case AST::NodeType::TEMPLATE_STYLE:
            generateTemplateStyle(std::static_pointer_cast<AST::TemplateStyleNode>(node));
            break;
            
        case AST::NodeType::CUSTOM_ELEMENT:
            generateCustomElement(std::static_pointer_cast<AST::CustomElementNode>(node));
            break;
            
        case AST::NodeType::CUSTOM_STYLE:
            generateCustomStyle(std::static_pointer_cast<AST::CustomStyleNode>(node));
            break;
            
        case AST::NodeType::INHERIT:
            generateInherit(std::static_pointer_cast<AST::InheritNode>(node));
            break;
            
        case AST::NodeType::DELETE:
            generateDelete(std::static_pointer_cast<AST::DeleteNode>(node));
            break;
            
        case AST::NodeType::INSERT:
            generateInsert(std::static_pointer_cast<AST::InsertNode>(node));
            break;
            
        case AST::NodeType::ORIGIN_BLOCK:
            generateOriginBlock(std::static_pointer_cast<AST::OriginBlockNode>(node));
            break;
            
        case AST::NodeType::CONFIGURATION:
            generateConfiguration(std::static_pointer_cast<AST::ConfigurationNode>(node));
            break;
            
        case AST::NodeType::NAMESPACE:
            generateNamespace(std::static_pointer_cast<AST::NamespaceNode>(node));
            break;
            
        case AST::NodeType::IMPORT:
            generateImport(std::static_pointer_cast<AST::ImportNode>(node));
            break;
            
        default:
            // 生成子节点
            for (auto child : node->getChildren()) {
                generateNode(child);
            }
            break;
    }
}

void CHTLGenerator::generateRootNode(std::shared_ptr<AST::RootNode> root) {
    // 生成所有子节点
    for (auto child : root->getChildren()) {
        generateNode(child);
    }
}

void CHTLGenerator::generateHTMLElement(std::shared_ptr<AST::HTMLElementNode> element) {
    // 开始标签
    html_output << "<" << element->tag_name;
    
    // 处理索引访问
    std::string index_access = element->getContextData("index_access", "");
    if (!index_access.empty()) {
        // 为索引访问元素生成特殊ID
        std::string generated_id = "chtl-index-" + element->tag_name + "-" + index_access;
        html_output << " id=\"" << generated_id << "\"";
        generated_ids.insert(generated_id);
    }
    
    // 生成属性
    for (auto attr : element->getAttributes()) {
        html_output << " " << attr->name << "=\"" << attr->value << "\"";
    }
    
    // 自动生成类和ID（选择器自动化）
    auto auto_class = selector_automation->generateAutoClass(element->tag_name);
    auto auto_id = selector_automation->generateAutoId(element->tag_name);
    
    if (!auto_class.empty()) {
        html_output << " class=\"" << auto_class << "\"";
        generated_classes.insert(auto_class);
    }
    
    if (!auto_id.empty() && index_access.empty()) {
        html_output << " id=\"" << auto_id << "\"";
        generated_ids.insert(auto_id);
    }
    
    html_output << ">";
    
    // 生成子元素和内容
    for (auto child : element->getChildren()) {
        generateNode(child);
    }
    
    // 结束标签
    html_output << "</" << element->tag_name << ">";
}

void CHTLGenerator::generateTextNode(std::shared_ptr<AST::TextNode> text) {
    html_output << text->content;
}

void CHTLGenerator::generateStyleBlock(std::shared_ptr<AST::StyleBlockNode> style) {
    // 本地样式块生成CSS
    for (auto child : style->getChildren()) {
        if (auto selector = std::static_pointer_cast<AST::StyleSelectorNode>(child)) {
            generateStyleSelector(selector);
        }
    }
}

void CHTLGenerator::generateStyleSelector(std::shared_ptr<AST::StyleSelectorNode> selector) {
    // 处理选择器自动化
    std::string processed_selector = selector_automation->processSelector(selector->selector);
    
    css_output << processed_selector << " {\n";
    
    for (auto property : selector->getProperties()) {
        if (auto style_prop = std::static_pointer_cast<AST::StylePropertyNode>(property)) {
            css_output << "  " << style_prop->property << ": " << style_prop->value << ";\n";
        }
    }
    
    css_output << "}\n\n";
}

void CHTLGenerator::generateScriptBlock(std::shared_ptr<AST::ScriptBlockNode> script) {
    std::string content = script->content;
    
    // 检查是否包含CHTL JS语法
    bool contains_chtljs = script->getContextData("contains_chtljs", "false") == "true";
    
    if (contains_chtljs) {
        // 需要通过CHTL JS编译器处理
        js_output << "/* CHTL JS Block - needs processing */\n";
        js_output << "// Original content: " << content << "\n";
        
        // 这里会调用CHTL JS编译器进行处理
        // 暂时输出注释说明需要进一步处理
        js_output << "// TODO: Process through CHTL JS compiler\n\n";
    } else {
        // 普通JavaScript，直接输出
        js_output << content << "\n\n";
    }
}

void CHTLGenerator::generateTemplateElement(std::shared_ptr<AST::TemplateElementNode> element) {
    // 注册模板到全局映射
    global_map->addTemplate(element->template_name, element->element_type);
    
    // 生成模板HTML结构（用于后续实例化）
    std::stringstream template_html;
    std::stringstream old_html_output;
    old_html_output << html_output.rdbuf();
    html_output.str("");
    html_output.clear();
    
    for (auto child : element->getChildren()) {
        generateNode(child);
    }
    
    template_html << html_output.str();
    html_output.str("");
    html_output.clear();
    html_output << old_html_output.str();
    
    // 存储模板内容
    global_map->setTemplateContent(element->template_name, template_html.str());
    
    // 生成模板CSS类
    css_output << "/* Template: " << element->template_name << " */\n";
    css_output << ".chtl-template-" << element->template_name << " {\n";
    css_output << "  /* Template-specific styles */\n";
    css_output << "}\n\n";
}

void CHTLGenerator::generateTemplateStyle(std::shared_ptr<AST::TemplateStyleNode> style) {
    // 注册模板样式
    global_map->addTemplate(style->style_name, "Style");
    
    css_output << "/* Template Style: " << style->style_name << " */\n";
    
    for (auto child : style->getChildren()) {
        if (auto selector = std::static_pointer_cast<AST::StyleSelectorNode>(child)) {
            generateStyleSelector(selector);
        }
    }
}

void CHTLGenerator::generateCustomElement(std::shared_ptr<AST::CustomElementNode> element) {
    // 注册自定义元素
    global_map->addCustomElement(element->custom_name, element->element_type);
    
    // 生成自定义元素HTML
    html_output << "<!-- Custom Element: " << element->custom_name << " -->\n";
    html_output << "<div class=\"chtl-custom-" << element->custom_name << "\">\n";
    
    for (auto child : element->getChildren()) {
        generateNode(child);
    }
    
    html_output << "</div>\n";
    html_output << "<!-- End Custom Element: " << element->custom_name << " -->\n";
    
    // 生成自定义元素CSS
    css_output << "/* Custom Element: " << element->custom_name << " */\n";
    css_output << ".chtl-custom-" << element->custom_name << " {\n";
    css_output << "  /* Custom element styles */\n";
    css_output << "}\n\n";
}

void CHTLGenerator::generateCustomStyle(std::shared_ptr<AST::CustomStyleNode> style) {
    // 注册自定义样式
    global_map->addCustomElement(style->style_name, "Style");
    
    css_output << "/* Custom Style: " << style->style_name << " */\n";
    
    for (auto child : style->getChildren()) {
        if (auto selector = std::static_pointer_cast<AST::StyleSelectorNode>(child)) {
            generateStyleSelector(selector);
        }
    }
}

void CHTLGenerator::generateInherit(std::shared_ptr<AST::InheritNode> inherit) {
    // 处理继承逻辑
    std::string source = inherit->source;
    
    // 查找继承源
    if (global_map->hasTemplate(source)) {
        std::string template_content = global_map->getTemplateContent(source);
        html_output << "<!-- Inherited from template: " << source << " -->\n";
        html_output << template_content;
    } else if (global_map->hasCustomElement(source)) {
        // 处理自定义元素继承
        html_output << "<!-- Inherited from custom element: " << source << " -->\n";
        // 生成继承的自定义元素
    } else {
        html_output << "<!-- Warning: Inheritance source not found: " << source << " -->\n";
    }
}

void CHTLGenerator::generateDelete(std::shared_ptr<AST::DeleteNode> delete_node) {
    // 生成删除操作的JavaScript
    std::string target = delete_node->target;
    
    js_output << "// Delete operation for: " << target << "\n";
    js_output << "document.addEventListener('DOMContentLoaded', function() {\n";
    
    // 解析目标（可能包含索引）
    if (target.find('[') != std::string::npos) {
        // 包含索引的删除
        size_t bracket_pos = target.find('[');
        std::string element_name = target.substr(0, bracket_pos);
        std::string index = target.substr(bracket_pos + 1, target.find(']') - bracket_pos - 1);
        
        js_output << "  var elements = document.getElementsByTagName('" << element_name << "');\n";
        js_output << "  if (elements[" << index << "]) {\n";
        js_output << "    elements[" << index << "].remove();\n";
        js_output << "  }\n";
    } else {
        // 简单删除
        js_output << "  var elements = document.getElementsByTagName('" << target << "');\n";
        js_output << "  for (var i = elements.length - 1; i >= 0; i--) {\n";
        js_output << "    elements[i].remove();\n";
        js_output << "  }\n";
    }
    
    js_output << "});\n\n";
}

void CHTLGenerator::generateInsert(std::shared_ptr<AST::InsertNode> insert) {
    // 生成插入操作的JavaScript
    std::string target = insert->target;
    int position_type = insert->position_type;
    
    js_output << "// Insert operation\n";
    js_output << "document.addEventListener('DOMContentLoaded', function() {\n";
    
    // 生成插入内容的HTML
    std::stringstream insert_html;
    std::stringstream old_html_output;
    old_html_output << html_output.rdbuf();
    html_output.str("");
    html_output.clear();
    
    for (auto child : insert->getChildren()) {
        generateNode(child);
    }
    
    insert_html << html_output.str();
    html_output.str("");
    html_output.clear();
    html_output << old_html_output.str();
    
    // 生成插入JavaScript
    js_output << "  var insertHTML = `" << insert_html.str() << "`;\n";
    js_output << "  var target = document.querySelector('" << target << "');\n";
    js_output << "  if (target) {\n";
    
    switch (position_type) {
        case 0: // after
            js_output << "    target.insertAdjacentHTML('afterend', insertHTML);\n";
            break;
        case 1: // before
            js_output << "    target.insertAdjacentHTML('beforebegin', insertHTML);\n";
            break;
        case 2: // replace
            js_output << "    target.outerHTML = insertHTML;\n";
            break;
        case 3: // at top
            js_output << "    target.insertAdjacentHTML('afterbegin', insertHTML);\n";
            break;
        case 4: // at bottom
            js_output << "    target.insertAdjacentHTML('beforeend', insertHTML);\n";
            break;
    }
    
    js_output << "  }\n";
    js_output << "});\n\n";
}

void CHTLGenerator::generateOriginBlock(std::shared_ptr<AST::OriginBlockNode> origin) {
    std::string type = origin->origin_type;
    std::string content = origin->content;
    
    if (type == "@Html") {
        html_output << "<!-- Origin HTML Block -->\n";
        html_output << content << "\n";
        html_output << "<!-- End Origin HTML Block -->\n";
    } else if (type == "@Style") {
        css_output << "/* Origin CSS Block */\n";
        css_output << content << "\n";
        css_output << "/* End Origin CSS Block */\n\n";
    } else if (type == "@JavaScript") {
        js_output << "/* Origin JavaScript Block */\n";
        js_output << content << "\n";
        js_output << "/* End Origin JavaScript Block */\n\n";
    }
}

void CHTLGenerator::generateConfiguration(std::shared_ptr<AST::ConfigurationNode> config) {
    // 处理配置
    std::string config_name = config->config_name;
    
    js_output << "// Configuration: " << config_name << "\n";
    js_output << "window.CHTLConfig = window.CHTLConfig || {};\n";
    js_output << "window.CHTLConfig." << config_name << " = {\n";
    
    for (const auto& pair : config->getConfigValues()) {
        js_output << "  '" << pair.first << "': '" << pair.second << "',\n";
    }
    
    js_output << "};\n\n";
}

void CHTLGenerator::generateNamespace(std::shared_ptr<AST::NamespaceNode> ns) {
    std::string namespace_name = ns->namespace_name;
    
    // 注册命名空间
    global_map->addNamespace(namespace_name);
    
    html_output << "<!-- Namespace: " << namespace_name << " -->\n";
    
    for (auto child : ns->getChildren()) {
        generateNode(child);
    }
    
    html_output << "<!-- End Namespace: " << namespace_name << " -->\n";
}

void CHTLGenerator::generateImport(std::shared_ptr<AST::ImportNode> import) {
    // 导入处理在编译时完成，这里主要是生成注释
    js_output << "// Import: " << import->import_statement << "\n";
}

// ============================================================================
// CHTLCompiler 实现
// ============================================================================

CHTLCompiler::CHTLCompiler() {
    parser = std::make_unique<CHTLParser>();
    generator = std::make_unique<CHTLGenerator>();
}

CHTLCompiler::~CHTLCompiler() = default;

CHTLCompilationResult CHTLCompiler::compile(const std::vector<Scanner::CodeFragment>& fragments) {
    CHTLCompilationResult result;
    
    try {
        // 解析阶段
        auto ast = parser->parse(fragments);
        if (!ast) {
            result.errors = parser->getErrors();
            result.warnings = parser->getWarnings();
            result.success = false;
            return result;
        }
        
        // 生成阶段
        result = generator->generate(ast);
        
        // 合并解析器的警告
        auto parser_warnings = parser->getWarnings();
        result.warnings.insert(result.warnings.end(), parser_warnings.begin(), parser_warnings.end());
        
    } catch (const std::exception& e) {
        result.errors.push_back("Compilation exception: " + std::string(e.what()));
        result.success = false;
    }
    
    return result;
}

std::vector<std::string> CHTLCompiler::getErrors() const {
    return parser->getErrors();
}

std::vector<std::string> CHTLCompiler::getWarnings() const {
    return parser->getWarnings();
}

} // namespace Compiler
} // namespace CHTL