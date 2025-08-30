#include "CHTLJSParser.h"
#include "../CHTLJSLexer/Lexer.h"
#include "../../Error/ErrorReporter.h"
#include <iostream>
#include <regex>

namespace CHTL {

CHTLJSParser::CHTLJSParser() : m_currentTokenIndex(0), m_debugMode(false) {
    m_context = std::make_shared<CHTLJSContext>();
}

CHTLJSParser::~CHTLJSParser() = default;

std::shared_ptr<BaseNode> CHTLJSParser::parse(const std::string& code) {
    try {
        if (m_debugMode) {
            std::cout << "🔍 开始CHTL JS解析，代码长度: " << code.length() << std::endl;
        }
        
        // 词法分析
        CHTLJSLexer lexer;
        m_tokens = lexer.tokenize(code);
        m_currentTokenIndex = 0;
        
        if (m_debugMode) {
            std::cout << "  ✓ CHTL JS词法分析完成，Token数量: " << m_tokens.size() << std::endl;
        }
        
        // 语法分析
        auto rootNode = parseDocument();
        
        if (m_debugMode) {
            std::cout << "  ✓ CHTL JS语法分析完成" << std::endl;
        }
        
        return rootNode;
        
    } catch (const std::exception& e) {
        reportError("CHTL JS解析过程中发生异常: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLJSParser::parseDocument() {
    auto documentNode = std::make_shared<BaseNode>(NodeType::CHTL_JS_DOCUMENT);
    documentNode->setValue("chtl-js-document");
    
    while (hasMoreTokens()) {
        Token token = getCurrentToken();
        
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
        
        std::shared_ptr<BaseNode> childNode = nullptr;
        
        // 根据CHTL JS语法解析
        if (token.value == "module") {
            childNode = parseModuleImport();
        } else if (token.value == "animate") {
            childNode = parseAnimateBlock();
        } else if (token.value == "listen") {
            childNode = parseListenBlock();
        } else if (token.value == "vir") {
            childNode = parseVirtualObject();
        } else if (token.value == "delegate") {
            childNode = parseDelegateBlock();
        } else if (isEnhancedSelector(token.value)) {
            childNode = parseEnhancedSelector();
        } else if (isEventBinding()) {
            childNode = parseEventBinding();
        } else {
            // 普通JavaScript代码
            childNode = parseJavaScriptStatement();
        }
        
        if (childNode) {
            documentNode->addChild(childNode);
        } else {
            consumeToken();  // 跳过无法识别的Token
        }
    }
    
    return documentNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseModuleImport() {
    auto moduleNode = std::make_shared<BaseNode>(NodeType::MODULE_IMPORT);
    
    consumeToken();  // 消费 'module'
    
    if (!expectToken(TokenType::BRACE_OPEN)) {
        return nullptr;
    }
    
    // 解析模块加载列表
    std::vector<std::string> modules;
    
    while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
        if (matchToken(TokenType::IDENTIFIER) && getCurrentToken().value == "load") {
            consumeToken();  // 消费 'load'
            expectToken(TokenType::COLON);
            
            Token modulePathToken = getCurrentToken();
            modules.push_back(modulePathToken.value);
            consumeToken();
            
            // 可选的逗号
            if (matchToken(TokenType::COMMA)) {
                consumeToken();
            }
        } else {
            consumeToken();
        }
    }
    
    expectToken(TokenType::BRACE_CLOSE);
    
    // 设置模块列表
    for (size_t i = 0; i < modules.size(); ++i) {
        moduleNode->setAttribute("module" + std::to_string(i), modules[i]);
    }
    
    moduleNode->setAttribute("count", std::to_string(modules.size()));
    
    if (m_debugMode) {
        std::cout << "  ✓ 解析模块导入: " << modules.size() << " 个模块" << std::endl;
    }
    
    return moduleNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseAnimateBlock() {
    auto animateNode = std::make_shared<BaseNode>(NodeType::ANIMATE);
    
    consumeToken();  // 消费 'animate'
    
    if (!expectToken(TokenType::BRACE_OPEN)) {
        return nullptr;
    }
    
    // 解析动画参数
    while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
        Token paramToken = getCurrentToken();
        if (paramToken.type == TokenType::IDENTIFIER) {
            std::string paramName = paramToken.value;
            consumeToken();
            
            if (expectToken(TokenType::COLON)) {
                Token valueToken = getCurrentToken();
                std::string paramValue = valueToken.value;
                consumeToken();
                
                animateNode->setAttribute(paramName, paramValue);
                
                // 可选的逗号
                if (matchToken(TokenType::COMMA)) {
                    consumeToken();
                }
            }
        } else {
            consumeToken();
        }
    }
    
    expectToken(TokenType::BRACE_CLOSE);
    
    if (m_debugMode) {
        std::cout << "  ✓ 解析动画块" << std::endl;
    }
    
    return animateNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseListenBlock() {
    auto listenNode = std::make_shared<BaseNode>(NodeType::LISTEN);
    
    consumeToken();  // 消费 'listen'
    
    if (!expectToken(TokenType::BRACE_OPEN)) {
        return nullptr;
    }
    
    // 解析事件监听器
    while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
        Token eventToken = getCurrentToken();
        if (eventToken.type == TokenType::IDENTIFIER) {
            std::string eventName = eventToken.value;
            consumeToken();
            
            if (expectToken(TokenType::COLON)) {
                // 收集函数内容
                std::string functionContent = collectFunctionContent();
                
                auto eventNode = std::make_shared<BaseNode>(NodeType::EVENT_LISTENER);
                eventNode->setAttribute("event", eventName);
                eventNode->setValue(functionContent);
                
                listenNode->addChild(eventNode);
                
                // 可选的逗号
                if (matchToken(TokenType::COMMA)) {
                    consumeToken();
                }
            }
        } else {
            consumeToken();
        }
    }
    
    expectToken(TokenType::BRACE_CLOSE);
    
    return listenNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseVirtualObject() {
    auto virNode = std::make_shared<BaseNode>(NodeType::VIRTUAL_OBJECT);
    
    consumeToken();  // 消费 'vir'
    
    // 解析虚拟对象名称
    Token nameToken = getCurrentToken();
    if (nameToken.type != TokenType::IDENTIFIER) {
        reportError("期望虚拟对象名称");
        return nullptr;
    }
    
    std::string virName = nameToken.value;
    virNode->setValue(virName);
    virNode->setAttribute("name", virName);
    consumeToken();
    
    if (!expectToken(TokenType::ASSIGN)) {
        return nullptr;
    }
    
    // 解析虚拟对象内容（通常是listen块）
    if (matchToken(TokenType::IDENTIFIER) && getCurrentToken().value == "listen") {
        auto listenNode = parseListenBlock();
        if (listenNode) {
            virNode->addChild(listenNode);
        }
    }
    
    return virNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseEnhancedSelector() {
    auto selectorNode = std::make_shared<BaseNode>(NodeType::ENHANCED_SELECTOR);
    
    Token selectorToken = getCurrentToken();
    std::string selectorText = selectorToken.value;
    
    // 解析增强选择器 {{.box}}, {{#id}}, {{button[0]}}
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::smatch match;
    
    if (std::regex_search(selectorText, match, selectorRegex)) {
        std::string selector = match[1].str();
        selectorNode->setValue(selector);
        selectorNode->setAttribute("selector", selector);
        
        // 分析选择器类型
        if (selector.starts_with('.')) {
            selectorNode->setAttribute("type", "class");
        } else if (selector.starts_with('#')) {
            selectorNode->setAttribute("type", "id");
        } else if (selector.find('[') != std::string::npos) {
            selectorNode->setAttribute("type", "indexed");
        } else {
            selectorNode->setAttribute("type", "tag");
        }
    }
    
    consumeToken();
    
    return selectorNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseEventBinding() {
    auto bindingNode = std::make_shared<BaseNode>(NodeType::EVENT_BINDING);
    
    // 解析选择器部分
    auto selectorNode = parseEnhancedSelector();
    if (selectorNode) {
        bindingNode->addChild(selectorNode);
    }
    
    // 期望 &->
    if (matchToken(TokenType::EVENT_BINDING)) {
        consumeToken();
        
        // 解析事件名称
        Token eventToken = getCurrentToken();
        std::string eventName = eventToken.value;
        bindingNode->setAttribute("event", eventName);
        consumeToken();
        
        // 解析事件处理函数
        if (expectToken(TokenType::BRACE_OPEN)) {
            std::string handlerContent = collectFunctionContent();
            bindingNode->setValue(handlerContent);
        }
    }
    
    return bindingNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseDelegateBlock() {
    auto delegateNode = std::make_shared<BaseNode>(NodeType::DELEGATE);
    
    consumeToken();  // 消费 'delegate'
    
    if (!expectToken(TokenType::BRACE_OPEN)) {
        return nullptr;
    }
    
    // 解析委托参数
    while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
        Token paramToken = getCurrentToken();
        if (paramToken.type == TokenType::IDENTIFIER) {
            std::string paramName = paramToken.value;
            consumeToken();
            
            if (expectToken(TokenType::COLON)) {
                if (paramName == "target") {
                    // 解析目标选择器
                    std::string targetValue = collectParameterValue();
                    delegateNode->setAttribute("target", targetValue);
                } else {
                    // 事件处理器
                    std::string handlerValue = collectParameterValue();
                    delegateNode->setAttribute(paramName, handlerValue);
                }
                
                // 可选的逗号
                if (matchToken(TokenType::COMMA)) {
                    consumeToken();
                }
            }
        } else {
            consumeToken();
        }
    }
    
    expectToken(TokenType::BRACE_CLOSE);
    
    return delegateNode;
}

std::string CHTLJSParser::collectFunctionContent() {
    std::string content;
    int braceDepth = 1;
    
    while (hasMoreTokens() && braceDepth > 0) {
        Token token = getCurrentToken();
        
        if (token.type == TokenType::BRACE_OPEN) {
            braceDepth++;
        } else if (token.type == TokenType::BRACE_CLOSE) {
            braceDepth--;
        }
        
        if (braceDepth > 0) {
            content += token.value + " ";
        }
        
        consumeToken();
    }
    
    return content;
}

std::string CHTLJSParser::collectParameterValue() {
    std::string value;
    
    Token valueToken = getCurrentToken();
    
    if (valueToken.type == TokenType::BRACE_OPEN) {
        // 对象或函数
        value = collectFunctionContent();
    } else if (valueToken.type == TokenType::BRACKET_OPEN) {
        // 数组
        value = collectArrayContent();
    } else {
        // 简单值
        value = valueToken.value;
        consumeToken();
    }
    
    return value;
}

std::string CHTLJSParser::collectArrayContent() {
    std::string content = "[";
    consumeToken();  // 消费 '['
    
    while (hasMoreTokens() && !matchToken(TokenType::BRACKET_CLOSE)) {
        Token token = getCurrentToken();
        content += token.value;
        consumeToken();
        
        if (matchToken(TokenType::COMMA)) {
            content += ", ";
            consumeToken();
        }
    }
    
    if (expectToken(TokenType::BRACKET_CLOSE)) {
        content += "]";
    }
    
    return content;
}

bool CHTLJSParser::isEnhancedSelector(const std::string& value) {
    return value.find("{{") != std::string::npos && value.find("}}") != std::string::npos;
}

bool CHTLJSParser::isEventBinding() {
    Token current = getCurrentToken();
    Token next = peekToken();
    
    return isEnhancedSelector(current.value) && next.value == "&->";
}

// Token操作方法
Token CHTLJSParser::getCurrentToken() const {
    if (m_currentTokenIndex < m_tokens.size()) {
        return m_tokens[m_currentTokenIndex];
    }
    
    Token eofToken;
    eofToken.type = TokenType::EOF_TOKEN;
    return eofToken;
}

Token CHTLJSParser::peekToken(size_t offset) const {
    size_t index = m_currentTokenIndex + offset;
    if (index < m_tokens.size()) {
        return m_tokens[index];
    }
    
    Token eofToken;
    eofToken.type = TokenType::EOF_TOKEN;
    return eofToken;
}

bool CHTLJSParser::hasMoreTokens() const {
    return m_currentTokenIndex < m_tokens.size();
}

void CHTLJSParser::consumeToken() {
    if (m_currentTokenIndex < m_tokens.size()) {
        m_currentTokenIndex++;
    }
}

bool CHTLJSParser::expectToken(TokenType type) {
    Token token = getCurrentToken();
    if (token.type == type) {
        consumeToken();
        return true;
    }
    
    reportError("期望Token类型: " + std::to_string(static_cast<int>(type)) + 
                ", 实际: " + std::to_string(static_cast<int>(token.type)));
    return false;
}

bool CHTLJSParser::matchToken(TokenType type) const {
    Token token = getCurrentToken();
    return token.type == type;
}

void CHTLJSParser::setContext(std::shared_ptr<CHTLJSContext> context) {
    m_context = context;
}

std::shared_ptr<CHTLJSContext> CHTLJSParser::getContext() const {
    return m_context;
}

void CHTLJSParser::clearErrors() {
    m_errors.clear();
    m_warnings.clear();
}

void CHTLJSParser::reportError(const std::string& message) {
    m_errors.push_back(message);
    
    if (m_debugMode) {
        std::cout << "  ❌ CHTL JS解析错误: " << message << std::endl;
    }
    
    ErrorReporter reporter;
    reporter.error("CHTLJSParser", message);
}

void CHTLJSParser::reportWarning(const std::string& message) {
    m_warnings.push_back(message);
    
    if (m_debugMode) {
        std::cout << "  ⚠️ CHTL JS解析警告: " << message << std::endl;
    }
    
    ErrorReporter reporter;
    reporter.warning("CHTLJSParser", message);
}

}