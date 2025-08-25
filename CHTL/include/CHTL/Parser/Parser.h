#pragma once

#include "CHTL/Core/Token.h"
#include "CHTL/Core/Ast.h"
#include "CHTL/Core/GlobalMap.h"
#include <vector>
#include <memory>

namespace CHTL {

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens, Config& config);

        std::shared_ptr<ProgramNode> Parse();

        const GlobalMap& GetGlobals() const { return m_Globals; }

    private:
        // Parsing methods for different CHTL constructs
        AstNodePtr ParseDeclaration();
        AstNodePtr ParseElement();
        AstNodePtr ParseTextNode();
        AstNodePtr ParseStyleNode();
        AstNodePtr ParseScriptNode();
        std::vector<AstNodePtr> ParseBlock();
        std::vector<AstNodePtr> ParseElementContent(const std::string& tagName);

        // Style parsing
        std::shared_ptr<StyleRuleNode> ParseStyleRule();
        std::shared_ptr<InlineStyleRuleNode> ParseInlineStyleRule();
        std::shared_ptr<SelectorBlockNode> ParseSelectorBlock();

        // Attribute parsing
        std::shared_ptr<AttributeNode> ParseAttribute();

        // Template parsing
        AstNodePtr ParseTemplateDefinition();
        AstNodePtr ParseTemplateUsage();
        std::shared_ptr<VarUsageNode> ParseVarUsage();
        std::shared_ptr<InheritNode> ParseInheritNode();
        AstNodePtr ParseCustomDefinition();
        std::shared_ptr<DeleteNode> ParseDeleteNode();
        AstNodePtr ParseElementDelete();
        AstNodePtr ParseInsertNode();
        AstNodePtr ParseOriginNode();
        AstNodePtr ParseImportNode();
        AstNodePtr ParseNamespaceNode();
        void ParseConfigNode(Config& config);

        // Utility methods
        bool IsAtEnd();
        Token Advance();
        Token Peek();
        Token Previous();
        bool Check(TokenType type);
        bool Match(const std::vector<TokenType>& types);
        Token Consume(TokenType type, const std::string& message);

        // Error handling
        void Error(const Token& token, const std::string& message);
        void Synchronize();

    private:
        std::string GetCurrentNamespacePrefix() const;

    private:
        const std::vector<Token>& m_Tokens;
        size_t m_Current = 0;
        bool m_HasError = false;
        Config& m_Config;
        GlobalMap m_Globals;
        std::vector<std::string> m_NamespaceStack;
    };

}
