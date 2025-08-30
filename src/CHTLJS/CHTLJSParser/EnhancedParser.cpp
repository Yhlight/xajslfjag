#include "EnhancedParser.h"
#include "../../Util/StringUtils.h"
#include <iostream>
#include <regex>
#include <algorithm>

namespace CHTLJS {

// CHTLJSNode实现
std::unique_ptr<CHTL::BaseNode> CHTLJSNode::clone() const {
    auto cloned = std::make_unique<CHTLJSNode>(chtljsType, getValue(), getPosition());
    
    cloned->jsProperties = jsProperties;
    cloned->jsEvents = jsEvents;
    cloned->jsTarget = jsTarget;
    
    // 复制子节点
    for (size_t i = 0; i < getChildCount(); ++i) {
        cloned->addChild(getChild(i)->clone());
    }
    
    return cloned;
}

String CHTLJSNode::toString() const {
    String result = "CHTLJSNode{type=" + std::to_string(static_cast<int>(chtljsType));
    result += ", value=" + getValue();
    if (!jsTarget.empty()) {
        result += ", target=" + jsTarget;
    }
    if (!jsProperties.empty()) {
        result += ", properties=" + std::to_string(jsProperties.size());
    }
    result += "}";
    return result;
}

// CHTLJSEnhancedParser实现
CHTLJSEnhancedParser::CHTLJSEnhancedParser(std::unique_ptr<CHTL::JS::CHTLJSLexer> lex, const CHTLJSParserConfig& cfg)
    : lexer(std::move(lex)), config(cfg) {
    if (lexer) {
        currentToken = lexer->nextToken();
    }
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parse() {
    auto root = std::make_unique<CHTLJSNode>(CHTLJSNodeType::CHTLJS_FUNCTION, "script", CHTL::Position());
    
    while (!isAtEnd()) {
        auto statement = parseStatement();
        if (statement) {
            root->addChild(std::move(statement));
        }
    }
    
    return root;
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parseStatement() {
    try {
        // 检测语句类型
        String currentValue = currentToken.value;
        
        if (currentValue == "module" && config.enableModuleImport) {
            return parseModuleBlock();
        } else if (currentValue.find("{{") != String::npos && config.enableEnhancedSelectors) {
            return parseEnhancedSelector();
        } else if (currentValue == "vir" && config.enableVirtualObjects) {
            return parseVirtualObject();
        } else if (config.enableAdvancedSyntax) {
            // 检查高级语法
            if (currentValue.find("listen") != String::npos) {
                return parseListenBlock();
            } else if (currentValue.find("delegate") != String::npos) {
                return parseDelegateBlock();
            } else if (currentValue.find("animate") != String::npos) {
                return parseAnimateBlock();
            } else if (currentValue.find("&->") != String::npos) {
                return parseEventBinding();
            }
        }
        
        // 解析普通JavaScript表达式
        return parseCHTLJSFunction();
        
    } catch (const std::exception& e) {
        reportError("解析语句时发生错误: " + String(e.what()));
        if (config.enableErrorRecovery) {
            synchronize();
        }
        return nullptr;
    }
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parseModuleBlock() {
    // module { load: ./module.js, load: ./utils.js }
    advance(); // 消费 'module'
    
    auto moduleNode = std::make_unique<CHTLJSNode>(CHTLJSNodeType::MODULE_IMPORT, "module", currentToken.position);
    
    if (check(CHTL::JS::CHTLJSTokenType::LEFT_BRACE)) {
        advance(); // 消费 '{'
        
        while (!check(CHTL::JS::CHTLJSTokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (currentToken.value == "load") {
                advance(); // 消费 'load'
                
                if (check(CHTL::JS::CHTLJSTokenType::COLON)) {
                    advance(); // 消费 ':'
                    
                    String modulePath = currentToken.value;
                    moduleNode->setJSProperty("load", modulePath);
                    advance();
                }
            }
            
            // 处理逗号
            if (check(CHTL::JS::CHTLJSTokenType::COMMA)) {
                advance();
            }
        }
        
        if (check(CHTL::JS::CHTLJSTokenType::RIGHT_BRACE)) {
            advance(); // 消费 '}'
        }
    }
    
    return moduleNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parseEnhancedSelector() {
    // {{.box}}, {{#header}}, {{button}}
    String selectorText = parseSelector();
    
    auto selectorNode = std::make_unique<CHTLJSNode>(CHTLJSNodeType::ENHANCED_SELECTOR, selectorText, currentToken.position);
    selectorNode->setJSTarget(selectorText);
    
    return selectorNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parseListenBlock() {
    // {{selector}}->listen { click: function() {}, hover: () => {} }
    String target = parseSelector();
    
    // 查找 '->' 或 'listen'
    while (!isAtEnd() && currentToken.value != "listen") {
        advance();
    }
    
    if (currentToken.value == "listen") {
        advance(); // 消费 'listen'
    }
    
    auto listenNode = std::make_unique<CHTLJSNode>(CHTLJSNodeType::LISTEN_BLOCK, "listen", currentToken.position);
    listenNode->setJSTarget(target);
    
    // 解析事件处理程序
    if (check(CHTL::JS::CHTLJSTokenType::LEFT_BRACE)) {
        auto eventHandlers = parseKeyValuePairs();
        for (const auto& handler : eventHandlers) {
            listenNode->setJSProperty(handler.first, handler.second);
            listenNode->addJSEvent(handler.first);
        }
    }
    
    return listenNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parseDelegateBlock() {
    // {{parent}}->delegate { target: {{.child}}, click: function() {} }
    String target = parseSelector();
    
    // 查找 'delegate'
    while (!isAtEnd() && currentToken.value != "delegate") {
        advance();
    }
    
    if (currentToken.value == "delegate") {
        advance(); // 消费 'delegate'
    }
    
    auto delegateNode = std::make_unique<CHTLJSNode>(CHTLJSNodeType::DELEGATE_BLOCK, "delegate", currentToken.position);
    delegateNode->setJSTarget(target);
    
    if (check(CHTL::JS::CHTLJSTokenType::LEFT_BRACE)) {
        auto properties = parseKeyValuePairs();
        for (const auto& prop : properties) {
            delegateNode->setJSProperty(prop.first, prop.second);
        }
    }
    
    return delegateNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parseAnimateBlock() {
    // animate { target: {{.box}}, duration: 1000, begin: {...}, end: {...} }
    advance(); // 消费 'animate'
    
    auto animateNode = std::make_unique<CHTLJSNode>(CHTLJSNodeType::ANIMATE_BLOCK, "animate", currentToken.position);
    
    if (check(CHTL::JS::CHTLJSTokenType::LEFT_BRACE)) {
        auto properties = parseKeyValuePairs();
        for (const auto& prop : properties) {
            animateNode->setJSProperty(prop.first, prop.second);
        }
    }
    
    return animateNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parseVirtualObject() {
    // vir objName = listen { click: function() {} }
    advance(); // 消费 'vir'
    
    String objectName = currentToken.value;
    advance(); // 消费对象名
    
    if (check(CHTL::JS::CHTLJSTokenType::ASSIGN)) {
        advance(); // 消费 '='
    }
    
    auto virNode = std::make_unique<CHTLJSNode>(CHTLJSNodeType::VIRTUAL_OBJECT, objectName, currentToken.position);
    
    // 解析虚对象定义
    String definition = parseJavaScriptBlock();
    virNode->setJSProperty("definition", definition);
    
    return virNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parseEventBinding() {
    // {{.box}} &-> click { console.log('clicked'); }
    String target = parseSelector();
    
    // 查找 '&->'
    while (!isAtEnd() && currentToken.value != "&->") {
        advance();
    }
    
    if (currentToken.value == "&->") {
        advance(); // 消费 '&->'
    }
    
    String eventName = currentToken.value;
    advance(); // 消费事件名
    
    auto bindingNode = std::make_unique<CHTLJSNode>(CHTLJSNodeType::EVENT_BINDING, "eventBinding", currentToken.position);
    bindingNode->setJSTarget(target);
    bindingNode->setJSProperty("event", eventName);
    
    // 解析处理程序
    String handler = parseJavaScriptBlock();
    bindingNode->setJSProperty("handler", handler);
    
    return bindingNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSEnhancedParser::parseCHTLJSFunction() {
    // 解析普通的JavaScript表达式，但可能包含CHTL JS语法
    String expression = parseJavaScriptExpression();
    
    auto functionNode = std::make_unique<CHTLJSNode>(CHTLJSNodeType::CHTLJS_FUNCTION, expression, currentToken.position);
    
    return functionNode;
}

StringUnorderedMap CHTLJSEnhancedParser::parseKeyValuePairs() {
    StringUnorderedMap pairs;
    
    if (!check(CHTL::JS::CHTLJSTokenType::LEFT_BRACE)) {
        return pairs;
    }
    
    advance(); // 消费 '{'
    
    while (!check(CHTL::JS::CHTLJSTokenType::RIGHT_BRACE) && !isAtEnd()) {
        String key = currentToken.value;
        advance();
        
        if (check(CHTL::JS::CHTLJSTokenType::COLON)) {
            advance(); // 消费 ':'
            
            String value = parseJavaScriptExpression();
            pairs[key] = value;
        }
        
        // 处理逗号
        if (check(CHTL::JS::CHTLJSTokenType::COMMA)) {
            advance();
        }
    }
    
    if (check(CHTL::JS::CHTLJSTokenType::RIGHT_BRACE)) {
        advance(); // 消费 '}'
    }
    
    return pairs;
}

String CHTLJSEnhancedParser::parseSelector() {
    String selector;
    
    // 查找 {{...}} 模式
    while (!isAtEnd()) {
        if (currentToken.value.find("{{") != String::npos) {
            // 提取选择器
            size_t start = currentToken.value.find("{{");
            size_t end = currentToken.value.find("}}", start);
            
            if (end != String::npos) {
                selector = currentToken.value.substr(start + 2, end - start - 2);
                advance();
                break;
            }
        }
        advance();
    }
    
    return CHTL::Util::StringUtils::trim(selector);
}

String CHTLJSEnhancedParser::parseJavaScriptExpression() {
    String expression;
    
    // 简单地收集tokens直到语句结束
    while (!isAtEnd() && currentToken.type != CHTL::JS::CHTLJSTokenType::SEMICOLON) {
        expression += currentToken.value + " ";
        advance();
    }
    
    if (check(CHTL::JS::CHTLJSTokenType::SEMICOLON)) {
        advance(); // 消费分号
    }
    
    return CHTL::Util::StringUtils::trim(expression);
}

String CHTLJSEnhancedParser::parseJavaScriptBlock() {
    String block;
    
    if (check(CHTL::JS::CHTLJSTokenType::LEFT_BRACE)) {
        advance(); // 消费 '{'
        
        int braceCount = 1;
        while (!isAtEnd() && braceCount > 0) {
            if (currentToken.type == CHTL::JS::CHTLJSTokenType::LEFT_BRACE) {
                braceCount++;
            } else if (currentToken.type == CHTL::JS::CHTLJSTokenType::RIGHT_BRACE) {
                braceCount--;
            }
            
            if (braceCount > 0) {
                block += currentToken.value + " ";
            }
            advance();
        }
    }
    
    return CHTL::Util::StringUtils::trim(block);
}

void CHTLJSEnhancedParser::advance() {
    if (lexer && !isAtEnd()) {
        currentToken = lexer->nextToken();
        currentPosition++;
    }
}

bool CHTLJSEnhancedParser::check(CHTL::JS::CHTLJSTokenType type) const {
    return currentToken.type == type;
}

bool CHTLJSEnhancedParser::isAtEnd() const {
    return currentToken.type == CHTL::JS::CHTLJSTokenType::EOF_TOKEN;
}

void CHTLJSEnhancedParser::reportError(const String& message, const String& code) {
    String error = message + " at position " + std::to_string(currentPosition);
    if (!code.empty()) {
        error += " (code: " + code + ")";
    }
    
    errors.push_back(error);
    hasErrors = true;
}

// CHTLJSSyntaxDetector实现
bool CHTLJSSyntaxDetector::isModuleImport(const String& code) {
    return code.find("module") != String::npos && code.find("{") != String::npos;
}

bool CHTLJSSyntaxDetector::isEnhancedSelector(const String& code) {
    return code.find("{{") != String::npos && code.find("}}") != String::npos;
}

bool CHTLJSSyntaxDetector::isListenBlock(const String& code) {
    return code.find("listen") != String::npos && code.find("{") != String::npos;
}

bool CHTLJSSyntaxDetector::isDelegateBlock(const String& code) {
    return code.find("delegate") != String::npos && code.find("{") != String::npos;
}

bool CHTLJSSyntaxDetector::isAnimateBlock(const String& code) {
    return code.find("animate") != String::npos && code.find("{") != String::npos;
}

bool CHTLJSSyntaxDetector::isVirtualObject(const String& code) {
    return CHTL::Util::StringUtils::starts_with(CHTL::Util::StringUtils::trim(code), "vir ");
}

bool CHTLJSSyntaxDetector::isEventBinding(const String& code) {
    return code.find("&->") != String::npos;
}

bool CHTLJSSyntaxDetector::hasCHTLJSSyntax(const String& code) {
    return isModuleImport(code) || isEnhancedSelector(code) || isListenBlock(code) ||
           isDelegateBlock(code) || isAnimateBlock(code) || isVirtualObject(code) ||
           isEventBinding(code);
}

// CHTLJSCodeGenerator实现
String CHTLJSCodeGenerator::generateFromNode(CHTLJSNode* node) {
    if (!node) {
        return "";
    }
    
    switch (node->getCHTLJSType()) {
        case CHTLJSNodeType::MODULE_IMPORT:
            return generateModuleImport(node);
        case CHTLJSNodeType::ENHANCED_SELECTOR:
            return generateEnhancedSelector(node);
        case CHTLJSNodeType::LISTEN_BLOCK:
            return generateListenBlock(node);
        case CHTLJSNodeType::DELEGATE_BLOCK:
            return generateDelegateBlock(node);
        case CHTLJSNodeType::ANIMATE_BLOCK:
            return generateAnimateBlock(node);
        case CHTLJSNodeType::VIRTUAL_OBJECT:
            return generateVirtualObject(node);
        case CHTLJSNodeType::EVENT_BINDING:
            return generateEventBinding(node);
        default:
            return node->getValue();
    }
}

String CHTLJSCodeGenerator::generateEnhancedSelector(CHTLJSNode* node) {
    String target = node->getJSTarget();
    
    if (target.empty()) {
        return "null";
    }
    
    // 转换选择器
    if (CHTL::Util::StringUtils::starts_with(target, ".") || 
        CHTL::Util::StringUtils::starts_with(target, "#") ||
        target.find(" ") != String::npos) {
        return "document.querySelector('" + target + "')";
    } else {
        return "document.getElementsByTagName('" + target + "')[0]";
    }
}

String CHTLJSCodeGenerator::generateListenBlock(CHTLJSNode* node) {
    String target = generateEnhancedSelector(node);
    String code;
    
    auto properties = node->getJSProperties();
    for (const auto& prop : properties) {
        code += target + ".addEventListener('" + prop.first + "', " + prop.second + ");\n";
    }
    
    return code;
}

String CHTLJSCodeGenerator::generateDelegateBlock(CHTLJSNode* node) {
    String parentTarget = generateEnhancedSelector(node);
    String code;
    
    auto properties = node->getJSProperties();
    String childTarget = properties.count("target") ? properties.at("target") : "";
    
    code += parentTarget + ".addEventListener('click', function(event) {\n";
    code += "  if (event.target.matches('" + childTarget + "')) {\n";
    
    for (const auto& prop : properties) {
        if (prop.first != "target") {
            code += "    (" + prop.second + ")(event);\n";
        }
    }
    
    code += "  }\n";
    code += "});\n";
    
    return code;
}

// CHTLJSUtils命名空间实现
namespace CHTLJSUtils {

String convertSelector(const String& chtljsSelector) {
    String selector = extractSelectorFromBraces(chtljsSelector);
    
    if (CHTL::Util::StringUtils::starts_with(selector, ".") || 
        CHTL::Util::StringUtils::starts_with(selector, "#") ||
        selector.find(" ") != String::npos) {
        return "document.querySelector('" + selector + "')";
    } else {
        return "document.getElementsByTagName('" + selector + "')[0]";
    }
}

String extractSelectorFromBraces(const String& bracedSelector) {
    size_t start = bracedSelector.find("{{");
    size_t end = bracedSelector.find("}}");
    
    if (start != String::npos && end != String::npos && end > start) {
        return bracedSelector.substr(start + 2, end - start - 2);
    }
    
    return bracedSelector;
}

StringVector parseEventHandlers(const String& eventBlock) {
    StringVector handlers;
    
    // 简单的事件解析
    std::regex eventPattern(R"((\w+):\s*([^,}]+))");
    std::sregex_iterator iter(eventBlock.begin(), eventBlock.end(), eventPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        handlers.push_back(iter->str());
    }
    
    return handlers;
}

String generateEventListener(const String& event, const String& handler) {
    return "addEventListener('" + event + "', " + handler + ")";
}

} // namespace CHTLJSUtils

} // namespace CHTLJS