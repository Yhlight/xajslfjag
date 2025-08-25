#include "CHTL_JS/Generator/SelectorGenerator.h"
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
    std::stringstream js;
    
    if (selector.empty()) {
        return "null";
    }
    
    // Check selector type
    if (selector[0] == '.') {
        // Class selector
        std::string className = selector.substr(1);
        if (index.has_value()) {
            js << "document.getElementsByClassName('" << className << "')[" << index.value() << "]";
        } else {
            js << "Array.from(document.getElementsByClassName('" << className << "'))";
        }
    } 
    else if (selector[0] == '#') {
        // ID selector
        std::string id = selector.substr(1);
        js << "document.getElementById('" << id << "')";
    }
    else {
        // Could be tag or implicit class/id
        if (IsTagSelector(selector)) {
            // Tag selector
            if (index.has_value()) {
                js << "document.getElementsByTagName('" << selector << "')[" << index.value() << "]";
            } else {
                js << "Array.from(document.getElementsByTagName('" << selector << "'))";
            }
        } else {
            // Try ID first, then class
            js << "(function() {\n";
            js << "    let elem = document.getElementById('" << selector << "');\n";
            js << "    if (elem) return elem;\n";
            js << "    let elems = document.getElementsByClassName('" << selector << "');\n";
            if (index.has_value()) {
                js << "    return elems[" << index.value() << "];\n";
            } else {
                js << "    return elems.length > 0 ? Array.from(elems) : null;\n";
            }
            js << "})()";
        }
    }
    
    return js.str();
}

bool SelectorGenerator::IsTagSelector(const std::string& selector) {
    // Convert to lowercase for comparison
    std::string lower = selector;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    return std::find(HTML_TAGS.begin(), HTML_TAGS.end(), lower) != HTML_TAGS.end();
}

}