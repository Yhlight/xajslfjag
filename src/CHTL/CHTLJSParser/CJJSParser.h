#ifndef CHTL_CJJS_PARSER_H
#define CHTL_CJJS_PARSER_H

#include <memory>
#include <string>
#include <vector>
#include "../CHTLParser/Parser.h"

namespace CHTL {

// CHTL JS解析器
class CJJSParser : public Parser {
public:
    CJJSParser(Lexer* lexer, Context* context);
    
    // 解析CJJS文件
    std::shared_ptr<ProgramNode> parseCJJS();
    
protected:
    // 扩展的解析方法
    std::shared_ptr<ASTNode> parseModule();
    std::shared_ptr<ASTNode> parseEnhancedSelector();
    std::shared_ptr<ASTNode> parseArrowOperator();
    std::shared_ptr<ASTNode> parseListen();
    std::shared_ptr<ASTNode> parseAnimation();
    std::shared_ptr<ASTNode> parseVirtualObject();
    
    // 解析增强选择器 {{selector}}
    std::string parseDoubleBraceExpression();
    
    // 检查是否是CJJS特有的token
    bool isCJJSToken();
};

} // namespace CHTL

#endif // CHTL_CJJS_PARSER_H