#pragma once

#include "../Core/Token.h"
#include "../Core/Ast.h"
#include <vector>
#include <memory>

namespace CHTL_JS {

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens);

        std::shared_ptr<ProgramNode> Parse();

    private:
        AstNodePtr ParseStatement();
        AstNodePtr ParseExpression();
        AstNodePtr ParsePrimaryExpression();
        AstNodePtr ParseListenStatement();
        AstNodePtr ParseDelegateStatement();
        AstNodePtr ParseAnimateStatement();
        AstNodePtr ParseVirObjectStatement();
        // ... other parsing methods

    private:
        const std::vector<Token>& m_Tokens;
        size_t m_Current = 0;
    };

}
