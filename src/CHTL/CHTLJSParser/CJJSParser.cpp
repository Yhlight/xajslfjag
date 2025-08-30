#include "CJJSParser.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLContext/Context.h"

namespace CHTL {

CJJSParser::CJJSParser(Lexer* lexer, Context* context)
    : Parser(lexer, context) {}

std::shared_ptr<ProgramNode> CJJSParser::parseCJJS() {
    // 解析CJJS文件（.cjjs扩展名）
    auto program = std::make_shared<ProgramNode>();
    
    while (!isAtEnd()) {
        auto node = parseTopLevel();
        if (node) {
            program->addChild(node);
        }
    }
    
    return program;
}

std::shared_ptr<ASTNode> CJJSParser::parseModule() {
    // 解析module块
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_MODULE, "Expected 'module'");
    consume(TokenType::LEFT_BRACE, "Expected '{' after module");
    
    std::vector<std::string> moduleList;
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::KEYWORD_LOAD)) {
            consume(TokenType::COLON, "Expected ':' after 'load'");
            
            // 解析模块路径列表
            do {
                std::string path = parseString();
                moduleList.push_back(path);
            } while (match(TokenType::COMMA));
        } else {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after module content");
    
    // TODO: 创建ModuleNode
    return nullptr;
}

std::shared_ptr<ASTNode> CJJSParser::parseEnhancedSelector() {
    // 解析增强选择器 {{selector}}
    std::string selector = parseDoubleBraceExpression();
    
    // TODO: 创建EnhancedSelectorNode
    return nullptr;
}

std::string CJJSParser::parseDoubleBraceExpression() {
    // 解析 {{ }} 表达式
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    consume(TokenType::LEFT_BRACE, "Expected second '{'");
    
    std::string expression;
    int braceDepth = 0;
    
    while (!isAtEnd()) {
        if (check(TokenType::LEFT_BRACE)) {
            braceDepth++;
            expression += current_->getLexeme();
            advance();
        } else if (check(TokenType::RIGHT_BRACE)) {
            if (braceDepth == 0) {
                // 检查是否是结束的 }}
                advance();
                if (check(TokenType::RIGHT_BRACE)) {
                    advance();
                    break;
                } else {
                    error("Expected '}}' to close enhanced selector");
                }
            } else {
                braceDepth--;
                expression += current_->getLexeme();
                advance();
            }
        } else {
            expression += current_->getLexeme();
            advance();
        }
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CJJSParser::parseArrowOperator() {
    // 解析 -> 操作符
    // 在CJJS中，-> 等价于 .
    consume(TokenType::ARROW, "Expected '->'");
    
    // TODO: 处理箭头操作符
    return nullptr;
}

std::shared_ptr<ASTNode> CJJSParser::parseListen() {
    // 解析 listen 监听器
    consume(TokenType::KEYWORD_LISTEN, "Expected 'listen'");
    
    // TODO: 创建ListenNode
    return nullptr;
}

std::shared_ptr<ASTNode> CJJSParser::parseAnimation() {
    // 解析动画相关语法
    // TODO: 实现动画解析
    return nullptr;
}

std::shared_ptr<ASTNode> CJJSParser::parseVirtualObject() {
    // 解析虚拟对象
    // TODO: 实现虚拟对象解析
    return nullptr;
}

bool CJJSParser::isCJJSToken() {
    // 检查是否是CJJS特有的token
    return check(TokenType::ARROW) || 
           check(TokenType::KEYWORD_MODULE) ||
           check(TokenType::KEYWORD_LISTEN) ||
           (check(TokenType::LEFT_BRACE) && peekNext() && peekNext()->getType() == TokenType::LEFT_BRACE);
}

} // namespace CHTL