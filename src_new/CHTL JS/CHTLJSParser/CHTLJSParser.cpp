#include "CHTLJSParser.h"
#include "../CHTLJSLexer/Lexer.h"

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(const ParseOptions& options)
    : m_options(options), m_currentToken(0) {
}

ParseResult CHTLJSParser::parse(const TokenSequence& tokens) {
    m_tokens = tokens;
    m_currentToken = 0;
    
    ParseResult result;
    result.rootNode = std::make_shared<BaseNode>(CHTLJSNodeType::ROOT_NODE, "root");
    
    while (m_currentToken < m_tokens.size() && getCurrentToken().type != TokenType::EOF_TOKEN) {
        NodePtr node = nullptr;
        
        Token current = getCurrentToken();
        
        switch (current.type) {
            case TokenType::MODULE:
                if (m_options.enableModules) {
                    node = parseModule();
                    if (node) result.modules.push_back(node);
                }
                break;
                
            case TokenType::ENHANCED_SELECTOR_START:
                if (m_options.enableEnhancedSelectors) {
                    node = parseEnhancedSelector();
                    if (node) result.enhancedSelectors.push_back(node);
                }
                break;
                
            case TokenType::LISTEN:
                if (m_options.enableEventListeners) {
                    node = parseEventListener();
                    if (node) result.eventListeners.push_back(node);
                }
                break;
                
            case TokenType::DELEGATE:
                if (m_options.enableEventDelegation) {
                    node = parseEventDelegate();
                }
                break;
                
            case TokenType::ANIMATE:
                if (m_options.enableAnimations) {
                    node = parseAnimation();
                    if (node) result.animations.push_back(node);
                }
                break;
                
            case TokenType::VIR:
                if (m_options.enableVirtualObjects) {
                    node = parseVirtualObject();
                    if (node) result.virtualObjects.push_back(node);
                }
                break;
                
            case TokenType::EVENT_BINDING:
                if (m_options.enableEventBinding) {
                    node = parseEventBinding();
                }
                break;
                
            case TokenType::SCRIPT:
                {
                    StateGuard guard(m_stateMachine, CHTLJSStateType::SCRIPT_BLOCK, "script");
                    advanceToken(); // 跳过script
                    if (expectToken(TokenType::LEFT_BRACE)) {
                        // 解析script块内容
                        while (getCurrentToken().type != TokenType::RIGHT_BRACE && 
                               getCurrentToken().type != TokenType::EOF_TOKEN) {
                            NodePtr childNode = nullptr;
                            
                            Token childToken = getCurrentToken();
                            if (childToken.type == TokenType::ENHANCED_SELECTOR_START) {
                                childNode = parseEnhancedSelector();
                            } else if (childToken.type == TokenType::LISTEN) {
                                childNode = parseEventListener();
                            } else if (childToken.type == TokenType::VIR) {
                                childNode = parseVirtualObject();
                            } else {
                                advanceToken(); // 跳过未知token
                            }
                            
                            if (childNode) {
                                result.rootNode->addChild(childNode);
                                result.nodesCreated++;
                            }
                        }
                        expectToken(TokenType::RIGHT_BRACE);
                    }
                }
                break;
                
            default:
                advanceToken(); // 跳过未知token
                break;
        }
        
        if (node) {
            result.rootNode->addChild(node);
            result.nodesCreated++;
        }
        
        result.tokensProcessed++;
    }
    
    result.success = true;
    return result;
}

ParseResult CHTLJSParser::parseString(const std::string& source) {
    Lexer lexer(source, &m_stateMachine, &m_context);
    TokenSequence tokens = lexer.tokenize();
    return parse(tokens);
}

NodePtr CHTLJSParser::parseModule() {
    StateGuard guard(m_stateMachine, CHTLJSStateType::MODULE_BLOCK, "module");
    
    Token moduleToken = getCurrentToken();
    advanceToken(); // 跳过module
    
    if (!expectToken(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    
    auto moduleNode = std::make_shared<BaseNode>(CHTLJSNodeType::MODULE_NODE, "module", moduleToken.position);
    
    // 解析模块内容
    while (getCurrentToken().type != TokenType::RIGHT_BRACE && 
           getCurrentToken().type != TokenType::EOF_TOKEN) {
        
        if (getCurrentToken().type == TokenType::LOAD) {
            advanceToken(); // 跳过load
            if (expectToken(TokenType::COLON)) {
                Token pathToken = getCurrentToken();
                if (pathToken.isLiteral()) {
                    auto loadNode = std::make_shared<BaseNode>(CHTLJSNodeType::MODULE_NODE, pathToken.value, pathToken.position);
                    moduleNode->addChild(loadNode);
                    advanceToken();
                }
            }
        } else {
            advanceToken();
        }
    }
    
    expectToken(TokenType::RIGHT_BRACE);
    
    m_context.addSymbol(SymbolInfo(SymbolType::MODULE_IMPORT, "module", moduleNode->content, moduleToken.position));
    
    return moduleNode;
}

NodePtr CHTLJSParser::parseEnhancedSelector() {
    Token startToken = getCurrentToken();
    advanceToken(); // 跳过{{
    
    std::string selectorContent = "";
    
    // 收集选择器内容
    while (getCurrentToken().type != TokenType::ENHANCED_SELECTOR_END && 
           getCurrentToken().type != TokenType::EOF_TOKEN) {
        selectorContent += getCurrentToken().value;
        advanceToken();
    }
    
    if (getCurrentToken().type == TokenType::ENHANCED_SELECTOR_END) {
        advanceToken(); // 跳过}}
    }
    
    auto selectorNode = std::make_shared<BaseNode>(CHTLJSNodeType::ENHANCED_SELECTOR_NODE, selectorContent, startToken.position);
    
    m_context.registerEnhancedSelector(selectorContent);
    m_context.addSymbol(SymbolInfo(SymbolType::ENHANCED_SELECTOR, selectorContent, selectorContent, startToken.position));
    
    return selectorNode;
}

NodePtr CHTLJSParser::parseEventListener() {
    StateGuard guard(m_stateMachine, CHTLJSStateType::EVENT_LISTENER, "listen");
    
    Token listenToken = getCurrentToken();
    advanceToken(); // 跳过listen
    
    if (!expectToken(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    
    auto listenerNode = std::make_shared<BaseNode>(CHTLJSNodeType::EVENT_LISTENER_NODE, "listen", listenToken.position);
    
    // 解析事件监听器内容
    while (getCurrentToken().type != TokenType::RIGHT_BRACE && 
           getCurrentToken().type != TokenType::EOF_TOKEN) {
        
        Token eventToken = getCurrentToken();
        if (eventToken.isEvent()) {
            advanceToken();
            if (expectToken(TokenType::COLON)) {
                // 解析事件处理函数
                std::string handlerContent = "";
                int braceCount = 0;
                
                while (getCurrentToken().type != TokenType::EOF_TOKEN) {
                    Token token = getCurrentToken();
                    handlerContent += token.value;
                    
                    if (token.type == TokenType::LEFT_BRACE) braceCount++;
                    else if (token.type == TokenType::RIGHT_BRACE) braceCount--;
                    
                    advanceToken();
                    
                    if (braceCount == 0 && (token.type == TokenType::COMMA || token.type == TokenType::RIGHT_BRACE)) {
                        break;
                    }
                }
                
                auto eventNode = std::make_shared<BaseNode>(CHTLJSNodeType::EVENT_LISTENER_NODE, 
                                                           eventToken.value + ":" + handlerContent, 
                                                           eventToken.position);
                listenerNode->addChild(eventNode);
                
                m_context.registerEventListener(eventToken.value, "");
            }
        } else {
            advanceToken();
        }
    }
    
    expectToken(TokenType::RIGHT_BRACE);
    
    return listenerNode;
}

NodePtr CHTLJSParser::parseEventDelegate() {
    StateGuard guard(m_stateMachine, CHTLJSStateType::EVENT_DELEGATE, "delegate");
    
    Token delegateToken = getCurrentToken();
    advanceToken(); // 跳过delegate
    
    if (!expectToken(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    
    auto delegateNode = std::make_shared<BaseNode>(CHTLJSNodeType::EVENT_DELEGATE_NODE, "delegate", delegateToken.position);
    
    // 解析委托内容（简化实现）
    while (getCurrentToken().type != TokenType::RIGHT_BRACE && 
           getCurrentToken().type != TokenType::EOF_TOKEN) {
        advanceToken();
    }
    
    expectToken(TokenType::RIGHT_BRACE);
    
    return delegateNode;
}

NodePtr CHTLJSParser::parseAnimation() {
    StateGuard guard(m_stateMachine, CHTLJSStateType::ANIMATION_BLOCK, "animate");
    
    Token animateToken = getCurrentToken();
    advanceToken(); // 跳过animate
    
    if (!expectToken(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    
    auto animationNode = std::make_shared<BaseNode>(CHTLJSNodeType::ANIMATION_NODE, "animate", animateToken.position);
    
    // 解析动画内容（简化实现）
    while (getCurrentToken().type != TokenType::RIGHT_BRACE && 
           getCurrentToken().type != TokenType::EOF_TOKEN) {
        advanceToken();
    }
    
    expectToken(TokenType::RIGHT_BRACE);
    
    return animationNode;
}

NodePtr CHTLJSParser::parseVirtualObject() {
    StateGuard guard(m_stateMachine, CHTLJSStateType::VIRTUAL_OBJECT, "vir");
    
    Token virToken = getCurrentToken();
    advanceToken(); // 跳过vir
    
    // 获取虚拟对象名称
    Token nameToken = getCurrentToken();
    if (nameToken.type != TokenType::IDENTIFIER) {
        reportError("Expected identifier after 'vir'");
        return nullptr;
    }
    advanceToken();
    
    if (!expectToken(TokenType::EQUAL)) {
        return nullptr;
    }
    
    auto virtualNode = std::make_shared<BaseNode>(CHTLJSNodeType::VIRTUAL_OBJECT_NODE, nameToken.value, virToken.position);
    
    // 解析虚拟对象定义
    if (getCurrentToken().type == TokenType::LISTEN) {
        NodePtr listenNode = parseEventListener();
        if (listenNode) {
            virtualNode->addChild(listenNode);
        }
    }
    
    m_context.registerVirtualObject(nameToken.value, virtualNode->content);
    m_context.addSymbol(SymbolInfo(SymbolType::VIRTUAL_OBJECT, nameToken.value, nameToken.value, virToken.position));
    
    return virtualNode;
}

NodePtr CHTLJSParser::parseEventBinding() {
    Token bindingToken = getCurrentToken();
    advanceToken(); // 跳过&->
    
    // 解析事件类型
    Token eventToken = getCurrentToken();
    if (!eventToken.isEvent()) {
        reportError("Expected event type after '&->'");
        return nullptr;
    }
    advanceToken();
    
    if (!expectToken(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    
    auto bindingNode = std::make_shared<BaseNode>(CHTLJSNodeType::EVENT_BINDING_NODE, 
                                                 eventToken.value, 
                                                 bindingToken.position);
    
    // 解析事件处理代码
    std::string handlerCode = "";
    int braceCount = 1;
    
    while (getCurrentToken().type != TokenType::EOF_TOKEN && braceCount > 0) {
        Token token = getCurrentToken();
        handlerCode += token.value;
        
        if (token.type == TokenType::LEFT_BRACE) braceCount++;
        else if (token.type == TokenType::RIGHT_BRACE) braceCount--;
        
        advanceToken();
    }
    
    bindingNode->content = eventToken.value + ":" + handlerCode;
    
    return bindingNode;
}

NodePtr CHTLJSParser::parseCHTLJSFunction() {
    Token nameToken = getCurrentToken();
    if (nameToken.type != TokenType::IDENTIFIER) {
        return nullptr;
    }
    
    // 检查是否为CHTL JS函数语法
    std::string syntax = nameToken.value;
    size_t savedPos = m_currentToken;
    
    advanceToken();
    if (getCurrentToken().type == TokenType::LEFT_BRACE) {
        // 收集完整的函数语法
        std::string functionContent = "";
        int braceCount = 1;
        advanceToken(); // 跳过{
        
        while (getCurrentToken().type != TokenType::EOF_TOKEN && braceCount > 0) {
            Token token = getCurrentToken();
            functionContent += token.value;
            
            if (token.type == TokenType::LEFT_BRACE) braceCount++;
            else if (token.type == TokenType::RIGHT_BRACE) braceCount--;
            
            advanceToken();
        }
        
        syntax += " {" + functionContent;
        
        if (CHTLJS_GLOBAL_MAP.isCHTLJSFunction(syntax)) {
            auto functionNode = std::make_shared<BaseNode>(CHTLJSNodeType::CHTLJS_FUNCTION_NODE, syntax, nameToken.position);
            
            m_context.addSymbol(SymbolInfo(SymbolType::CHTLJS_FUNCTION, nameToken.value, syntax, nameToken.position));
            
            return functionNode;
        }
    }
    
    // 回退
    m_currentToken = savedPos;
    return nullptr;
}

Token CHTLJSParser::getCurrentToken() const {
    if (m_currentToken >= m_tokens.size()) {
        return Token(TokenType::EOF_TOKEN);
    }
    return m_tokens[m_currentToken];
}

Token CHTLJSParser::peekToken(size_t offset) const {
    size_t pos = m_currentToken + offset;
    if (pos >= m_tokens.size()) {
        return Token(TokenType::EOF_TOKEN);
    }
    return m_tokens[pos];
}

void CHTLJSParser::advanceToken() {
    if (m_currentToken < m_tokens.size()) {
        m_currentToken++;
    }
}

bool CHTLJSParser::matchToken(TokenType type) {
    if (getCurrentToken().type == type) {
        advanceToken();
        return true;
    }
    return false;
}

bool CHTLJSParser::expectToken(TokenType type) {
    if (getCurrentToken().type == type) {
        advanceToken();
        return true;
    }
    
    reportError("Expected token type: " + std::to_string(static_cast<int>(type)));
    return false;
}

void CHTLJSParser::reportError(const std::string& message) {
    Token current = getCurrentToken();
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, message, "", current.position.line, current.position.column);
}

void CHTLJSParser::reportWarning(const std::string& message) {
    Token current = getCurrentToken();
    ErrorReporter reporter;
    reporter.warning(message, "", current.position.line, current.position.column);
}

} // namespace CHTLJS