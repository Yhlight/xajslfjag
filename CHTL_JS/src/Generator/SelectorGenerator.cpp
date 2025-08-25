#include "CHTL_JS/Generator/SelectorGenerator.h"
#include "CHTL_JS/Core/Runtime.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL_JS {

// Common HTML tag names for detection
static const std::vector<std::string> HTML_TAGS = {
    "div", "span", "p", "a", "button", "input", "form", "h1", "h2", "h3", "h4", "h5", "h6",
    "ul", "ol", "li", "table", "tr", "td", "th", "img", "video", "audio", "canvas",
    "header", "footer", "nav", "main", "section", "article", "aside", "figure", "figcaption"
};

std::string SelectorGenerator::Generate(const SelectorNode& node) {
    return GenerateByType(node.Selector, node.Index);
}

std::string SelectorGenerator::GenerateByType(const std::string& selector, std::optional<int> index) {
    std::stringstream chtljs;
    
    if (selector.empty()) {
        return "null";
    }
    
    // Generate CHTL JS selector syntax, NOT JavaScript!
    // CHTL JS has its own selector resolution mechanism
    
    if (selector[0] == '.') {
        // Class selector in CHTL JS
        std::string className = selector.substr(1);
        if (index.has_value()) {
            chtljs << "select_class(\"" << className << "\")[" << index.value() << "]";
        } else {
            chtljs << "select_class(\"" << className << "\")";
        }
    } 
    else if (selector[0] == '#') {
        // ID selector in CHTL JS
        std::string id = selector.substr(1);
        chtljs << "select_id(\"" << id << "\")";
    }
    else {
        // Could be tag or implicit class/id
        if (IsTagSelector(selector)) {
            // Tag selector in CHTL JS
            if (index.has_value()) {
                chtljs << "select_element(\"" << selector << "\")[" << index.value() << "]";
            } else {
                chtljs << "select_element(\"" << selector << "\")";
            }
        } else {
            // CHTL JS smart selector - tries ID first, then class
            chtljs << "smart_select(\"" << selector << "\")";
            if (index.has_value()) {
                chtljs << "[" << index.value() << "]";
            }
        }
    }
    
    return chtljs.str();
}

bool SelectorGenerator::IsTagSelector(const std::string& selector) {
    // Convert to lowercase for comparison
    std::string lower = selector;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    return std::find(HTML_TAGS.begin(), HTML_TAGS.end(), lower) != HTML_TAGS.end();
}

}