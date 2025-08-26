#pragma once

#include <string>
#include "CHTL/Core/Ast.h"

namespace CHTL {

    enum class CommentContext {
        HTML,
        CSS,
        JavaScript
    };

    class CommentGenerator {
    public:
        // Generate appropriate comment based on context
        static std::string Generate(const CommentNode& node, CommentContext context);
        
    private:
        static std::string GenerateHTMLComment(const std::string& content);
        static std::string GenerateCSSComment(const std::string& content);
        static std::string GenerateJSComment(const std::string& content);
        
        // Helper to escape special characters if needed
        static std::string EscapeComment(const std::string& content, CommentContext context);
    };

}