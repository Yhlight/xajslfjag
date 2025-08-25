#include "CHTL/Generator/CommentGenerator.h"
#include <algorithm>

namespace CHTL {

std::string CommentGenerator::Generate(const CommentNode& node, CommentContext context) {
    switch (context) {
        case CommentContext::HTML:
            return GenerateHTMLComment(node.Content);
        case CommentContext::CSS:
            return GenerateCSSComment(node.Content);
        case CommentContext::JavaScript:
            return GenerateJSComment(node.Content);
        default:
            return GenerateHTMLComment(node.Content);
    }
}

std::string CommentGenerator::GenerateHTMLComment(const std::string& content) {
    // HTML comments: <!-- content -->
    std::string escaped = EscapeComment(content, CommentContext::HTML);
    return "<!-- " + escaped + " -->";
}

std::string CommentGenerator::GenerateCSSComment(const std::string& content) {
    // CSS comments: /* content */
    std::string escaped = EscapeComment(content, CommentContext::CSS);
    return "/* " + escaped + " */";
}

std::string CommentGenerator::GenerateJSComment(const std::string& content) {
    // JavaScript comments: // content (single line) or /* content */ (multi-line)
    std::string escaped = EscapeComment(content, CommentContext::JavaScript);
    
    // Check if content contains newlines
    if (escaped.find('\n') != std::string::npos) {
        // Multi-line comment
        return "/* " + escaped + " */";
    } else {
        // Single-line comment
        return "// " + escaped;
    }
}

std::string CommentGenerator::EscapeComment(const std::string& content, CommentContext context) {
    std::string result = content;
    
    switch (context) {
        case CommentContext::HTML:
            // Escape --> sequences in HTML comments
            size_t pos = 0;
            while ((pos = result.find("-->", pos)) != std::string::npos) {
                result.replace(pos, 3, "-- >");
                pos += 4;
            }
            break;
            
        case CommentContext::CSS:
        case CommentContext::JavaScript:
            // Escape */ sequences in CSS/JS comments
            pos = 0;
            while ((pos = result.find("*/", pos)) != std::string::npos) {
                result.replace(pos, 2, "* /");
                pos += 3;
            }
            break;
    }
    
    return result;
}

}