#include "CHTLGenerator.h"
#include <algorithm>
#include <iomanip>

namespace CHTL {

CHTLGenerator::CHTLGenerator(CHTLContext* ctx) 
    : context(ctx), indent_level(0), pretty_print(true), class_counter(0), id_counter(0) {}

void CHTLGenerator::writeIndent() {
    if (pretty_print) {
        for (int i = 0; i < indent_level; i++) {
            html_output << "  ";
        }
    }
}

void CHTLGenerator::writeLine(const std::string& content) {
    writeIndent();
    html_output << content;
    if (pretty_print) {
        html_output << "\n";
    }
}

void CHTLGenerator::writeHtml(const std::string& content) {
    html_output << content;
}

void CHTLGenerator::writeCss(const std::string& content) {
    css_output << content;
}

void CHTLGenerator::writeJs(const std::string& content) {
    js_output << content;
}

std::string CHTLGenerator::generateUniqueClass() {
    return "chtl-class-" + std::to_string(++class_counter);
}

std::string CHTLGenerator::generateUniqueId() {
    return "chtl-id-" + std::to_string(++id_counter);
}

CHTLGenerator::GeneratedCode CHTLGenerator::generate(std::shared_ptr<CHTLNode> ast) {
    reset();
    
    if (!ast) {
        return {"", "", ""};
    }
    
    // Generate all components
    generateDocument(ast);
    processGlobalStyles();
    
    GeneratedCode result;
    result.html = html_output.str();
    result.css = css_output.str();
    result.js = js_output.str();
    
    return result;
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<CHTLNode> ast) {
    auto code = generate(ast);
    return code.html;
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<CHTLNode> ast) {
    auto code = generate(ast);
    return code.css;
}

std::string CHTLGenerator::generateJS(std::shared_ptr<CHTLNode> ast) {
    auto code = generate(ast);
    return code.js;
}

void CHTLGenerator::generateDocument(std::shared_ptr<CHTLNode> node) {
    if (!node || node->type != NodeType::DOCUMENT) {
        return;
    }
    
    // Check for use declaration
    bool has_html5_declaration = false;
    for (const auto& child : node->children) {
        if (child->type == NodeType::USE_DECLARATION && child->value == "html5") {
            has_html5_declaration = true;
            break;
        }
    }
    
    // Generate HTML5 doctype if needed
    if (has_html5_declaration) {
        writeLine("<!DOCTYPE html>");
    }
    
    // Process all child nodes
    for (const auto& child : node->children) {
        switch (child->type) {
            case NodeType::HTML_ELEMENT:
                generateElement(child);
                break;
            case NodeType::TEXT_NODE:
                generateTextNode(child);
                break;
            case NodeType::TEMPLATE_DEFINITION:
                generateTemplateDefinition(child);
                break;
            case NodeType::CUSTOM_DEFINITION:
                generateCustomDefinition(child);
                break;
            case NodeType::COMMENT:
            case NodeType::GENERATOR_COMMENT:
                generateComment(child);
                break;
            case NodeType::USE_DECLARATION:
                // Already processed
                break;
            default:
                // Handle other node types
                break;
        }
    }
}

void CHTLGenerator::generateElement(std::shared_ptr<CHTLNode> node) {
    if (!node || node->type != NodeType::HTML_ELEMENT) {
        return;
    }
    
    std::string tag_name = node->name;
    if (tag_name.empty()) {
        tag_name = "div"; // Default element
    }
    
    // Auto-add classes and IDs if needed
    autoAddClassesAndIds(node);
    
    // Generate opening tag
    writeIndent();
    writeHtml("<" + tag_name);
    
    // Generate attributes
    for (const auto& attr : node->attributes) {
        writeHtml(" " + attr.first + "=\"" + escapeHtml(attr.second) + "\"");
    }
    
    // Check for attributes in children
    std::vector<std::shared_ptr<CHTLNode>> content_children;
    for (const auto& child : node->children) {
        if (child->type == NodeType::ATTRIBUTE) {
            writeHtml(" " + child->name + "=\"" + escapeHtml(child->value) + "\"");
        } else {
            content_children.push_back(child);
        }
    }
    
    // Check if it's a self-closing tag
    bool is_self_closing = (tag_name == "img" || tag_name == "br" || tag_name == "hr" || 
                           tag_name == "input" || tag_name == "meta" || tag_name == "link");
    
    if (is_self_closing && content_children.empty()) {
        writeHtml(" />");
        if (pretty_print) writeHtml("\n");
        return;
    }
    
    writeHtml(">");
    if (pretty_print) writeHtml("\n");
    
    // Generate content
    indent_level++;
    
    std::shared_ptr<CHTLNode> style_block = nullptr;
    
    for (const auto& child : content_children) {
        switch (child->type) {
            case NodeType::HTML_ELEMENT:
                generateElement(child);
                break;
            case NodeType::TEXT_NODE:
                generateTextNode(child);
                break;
            case NodeType::STYLE_BLOCK:
                style_block = child;
                break;
            case NodeType::SCRIPT_BLOCK:
                generateScriptBlock(child);
                break;
            case NodeType::TEMPLATE_USAGE:
                generateTemplateUsage(child);
                break;
            case NodeType::CUSTOM_USAGE:
                generateCustomUsage(child);
                break;
            case NodeType::COMMENT:
            case NodeType::GENERATOR_COMMENT:
                generateComment(child);
                break;
            default:
                break;
        }
    }
    
    // Process local styles after element content
    if (style_block) {
        processLocalStyles(node, style_block);
    }
    
    indent_level--;
    
    // Generate closing tag
    writeLine("</" + tag_name + ">");
}

void CHTLGenerator::generateTextNode(std::shared_ptr<CHTLNode> node) {
    if (!node || node->type != NodeType::TEXT_NODE) {
        return;
    }
    
    std::string content = node->value;
    if (content.empty() && !node->children.empty()) {
        // Collect text from children
        for (const auto& child : node->children) {
            content += child->value;
        }
    }
    
    writeIndent();
    writeHtml(escapeHtml(content));
    if (pretty_print) writeHtml("\n");
}

void CHTLGenerator::generateStyleBlock(std::shared_ptr<CHTLNode> node) {
    // Style blocks are processed separately
    // This method is called for global style blocks
    if (!node || node->type != NodeType::STYLE_BLOCK) {
        return;
    }
    
    for (const auto& child : node->children) {
        if (child->type == NodeType::STYLE_RULE) {
            generateStyleRule(child);
        }
    }
}

void CHTLGenerator::generateScriptBlock(std::shared_ptr<CHTLNode> node) {
    if (!node || node->type != NodeType::SCRIPT_BLOCK) {
        return;
    }
    
    // Generate script content to JS output
    writeJs("// Local script block\n");
    writeJs("(function() {\n");
    
    std::string script_content = node->value;
    if (script_content.empty() && !node->children.empty()) {
        for (const auto& child : node->children) {
            script_content += child->value + "\n";
        }
    }
    
    writeJs(script_content);
    writeJs("\n})();\n\n");
}

void CHTLGenerator::generateTemplateDefinition(std::shared_ptr<CHTLNode> node) {
    // Template definitions are stored for later use, not directly generated
    if (!node || node->type != NodeType::TEMPLATE_DEFINITION) {
        return;
    }
    
    // Templates are processed during usage, not definition
}

void CHTLGenerator::generateCustomDefinition(std::shared_ptr<CHTLNode> node) {
    // Custom definitions are stored for later use, not directly generated
    if (!node || node->type != NodeType::CUSTOM_DEFINITION) {
        return;
    }
    
    // Customs are processed during usage, not definition
}

void CHTLGenerator::generateTemplateUsage(std::shared_ptr<CHTLNode> node) {
    if (!node || node->type != NodeType::TEMPLATE_USAGE) {
        return;
    }
    
    // Expand template content
    std::string template_content = expandTemplate(node->name, node->value);
    writeHtml(template_content);
}

void CHTLGenerator::generateCustomUsage(std::shared_ptr<CHTLNode> node) {
    if (!node || node->type != NodeType::CUSTOM_USAGE) {
        return;
    }
    
    // Expand custom content
    std::string custom_content = expandCustom(node->name, node->value);
    writeHtml(custom_content);
}

void CHTLGenerator::generateComment(std::shared_ptr<CHTLNode> node) {
    if (!node) return;
    
    if (node->type == NodeType::COMMENT) {
        // Regular HTML comment
        writeLine("<!-- " + escapeHtml(node->value) + " -->");
    } else if (node->type == NodeType::GENERATOR_COMMENT) {
        // Generator comment - output to all three streams
        writeHtml("<!-- " + escapeHtml(node->value) + " -->\n");
        writeCss("/* " + escapeCss(node->value) + " */\n");
        writeJs("// " + escapeJs(node->value) + "\n");
    }
}

void CHTLGenerator::generateStyleRule(std::shared_ptr<CHTLNode> node) {
    if (!node || node->type != NodeType::STYLE_RULE) {
        return;
    }
    
    std::string selector = node->name;
    
    writeCss(selector + " {\n");
    
    for (const auto& child : node->children) {
        if (child->type == NodeType::ATTRIBUTE) {
            writeCss("  " + child->name + ": " + child->value + ";\n");
        }
    }
    
    writeCss("}\n\n");
}

void CHTLGenerator::processLocalStyles(std::shared_ptr<CHTLNode> element, std::shared_ptr<CHTLNode> style_block) {
    if (!element || !style_block) return;
    
    // Process local styles and add them to global CSS
    for (const auto& rule : style_block->children) {
        if (rule->type == NodeType::STYLE_RULE) {
            std::string selector = rule->name;
            
            // Process selector context
            if (selector.empty()) {
                // Direct properties - apply to element
                for (const auto& prop : rule->children) {
                    if (prop->type == NodeType::ATTRIBUTE) {
                        // Add inline style
                        std::string current_style = element->getAttribute("style");
                        if (!current_style.empty()) current_style += "; ";
                        current_style += prop->name + ": " + prop->value;
                        element->setAttribute("style", current_style);
                    }
                }
            } else {
                // Selector rule - add to global styles
                std::string processed_selector = generateSelector(selector, element->name);
                
                std::stringstream rule_css;
                rule_css << processed_selector << " {\n";
                
                for (const auto& prop : rule->children) {
                    if (prop->type == NodeType::ATTRIBUTE) {
                        rule_css << "  " << prop->name << ": " << prop->value << ";\n";
                    }
                }
                
                rule_css << "}\n\n";
                global_styles.push_back(rule_css.str());
                
                // Auto-add class/id to element if needed
                if (selector.length() > 0 && selector[0] == '.') {
                    std::string class_name = selector.substr(1);
                    std::string current_class = element->getAttribute("class");
                    if (current_class.empty()) {
                        element->setAttribute("class", class_name);
                    } else if (current_class.find(class_name) == std::string::npos) {
                        element->setAttribute("class", current_class + " " + class_name);
                    }
                } else if (selector.length() > 0 && selector[0] == '#') {
                    std::string id_name = selector.substr(1);
                    if (element->getAttribute("id").empty()) {
                        element->setAttribute("id", id_name);
                    }
                }
            }
        }
    }
}

void CHTLGenerator::processGlobalStyles() {
    // Output all collected global styles
    for (const auto& style : global_styles) {
        writeCss(style);
    }
}

std::string CHTLGenerator::generateSelector(const std::string& selector, const std::string& context_element) {
    if (selector.length() > 0 && selector[0] == '&') {
        // Replace & with context element class or id
        std::string result = selector;
        // This is a simplified implementation
        return result;
    }
    return selector;
}

void CHTLGenerator::autoAddClassesAndIds(std::shared_ptr<CHTLNode> element) {
    if (!context) return;
    
    // Check configuration for auto-add settings
    if (context->getConfig("DISABLE_STYLE_AUTO_ADD_CLASS") != "true") {
        // Auto-add class logic would go here
    }
    
    if (context->getConfig("DISABLE_STYLE_AUTO_ADD_ID") != "true") {
        // Auto-add ID logic would go here
    }
}

std::string CHTLGenerator::expandTemplate(const std::string& template_name, const std::string& template_type) {
    // Template expansion logic - simplified implementation
    return "<!-- Template: " + template_name + " -->";
}

std::string CHTLGenerator::expandCustom(const std::string& custom_name, const std::string& custom_type) {
    // Custom expansion logic - simplified implementation
    return "<!-- Custom: " + custom_name + " -->";
}

void CHTLGenerator::reset() {
    html_output.str("");
    html_output.clear();
    css_output.str("");
    css_output.clear();
    js_output.str("");
    js_output.clear();
    
    indent_level = 0;
    global_styles.clear();
    local_styles.clear();
    class_counter = 0;
    id_counter = 0;
}

std::string CHTLGenerator::escapeHtml(const std::string& text) {
    std::string result = text;
    std::replace(result.begin(), result.end(), '<', '&');
    // Simplified - would need full HTML escaping
    return result;
}

std::string CHTLGenerator::escapeCss(const std::string& text) {
    // CSS escaping logic
    return text;
}

std::string CHTLGenerator::escapeJs(const std::string& text) {
    // JavaScript escaping logic
    return text;
}

} // namespace CHTL