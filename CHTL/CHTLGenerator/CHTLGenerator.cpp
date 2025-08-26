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
            case NodeType::ORIGIN_DEFINITION:
                generateOriginDefinition(child);
                break;
            case NodeType::CONFIGURATION_DEFINITION:
                generateConfigurationDefinition(child);
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

void CHTLGenerator::generateOriginDefinition(std::shared_ptr<CHTLNode> node) {
    if (!node || node->type != NodeType::ORIGIN_DEFINITION) {
        return;
    }
    
    std::string origin_type = node->getAttribute("type");
    std::string raw_content = node->value;
    bool is_usage = node->getAttribute("usage") == "true";
    
    if (is_usage) {
        // This is a usage reference: [Origin] @Html name;
        // Look up the named origin definition and output its content
        writeHtml("<!-- Origin reference: " + node->name + " -->\n");
        return;
    }
    
    // Direct origin definition with content
    if (origin_type == "Html") {
        // Output raw HTML directly
        writeHtml(raw_content);
        writeHtml("\n");
        
    } else if (origin_type == "Style") {
        // Output raw CSS to CSS stream
        writeCss("/* Origin CSS */\n");
        writeCss(raw_content);
        writeCss("\n");
        
    } else if (origin_type == "JavaScript") {
        // Output raw JavaScript to JS stream
        writeJs("// Origin JavaScript\n");
        writeJs(raw_content);
        writeJs("\n");
        
    } else {
        // Custom origin type - output as HTML comment with type info
        writeHtml("<!-- Origin " + origin_type + " -->\n");
        writeHtml(raw_content);
        writeHtml("\n<!-- End Origin " + origin_type + " -->\n");
    }
}

void CHTLGenerator::generateConfigurationDefinition(std::shared_ptr<CHTLNode> node) {
    if (!node || node->type != NodeType::CONFIGURATION_DEFINITION) {
        return;
    }
    
    // Configuration definitions affect compilation behavior but don't generate output
    // The configuration values are already applied to the context during parsing
    
    // Optionally generate a comment indicating the configuration
    if (context && context->getConfig("DEBUG_MODE") == "true") {
        writeHtml("<!-- Configuration applied -->\n");
    }
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
        // Replace & with context element selector
        std::string result = selector;
        std::string replacement = "." + context_element; // Use class selector by default
        
        // Replace & with the context element's class
        size_t pos = result.find('&');
        if (pos != std::string::npos) {
            result.replace(pos, 1, replacement);
        }
        
        return result;
    }
    
    // Handle automatic scoping for local style blocks
    if (!context_element.empty()) {
        // Scope the selector to the context element
        return "." + context_element + " " + selector;
    }
    
    return selector;
}

void CHTLGenerator::autoAddClassesAndIds(std::shared_ptr<CHTLNode> element) {
    if (!context || !element) return;
    
    // Check if selector automation is disabled
    if (context->getConfig("DISABLE_STYLE_AUTO_ADD_CLASS") == "true" && 
        context->getConfig("DISABLE_STYLE_AUTO_ADD_ID") == "true") {
        return;
    }
    
    // Auto-generate class name if element doesn't have one
    if (context->getConfig("DISABLE_STYLE_AUTO_ADD_CLASS") != "true") {
        if (element->getAttribute("class").empty()) {
            std::string auto_class = "chtl-" + element->name + "-" + std::to_string(class_counter++);
            element->setAttribute("class", auto_class);
        }
    }
    
    // Auto-generate ID if element doesn't have one
    if (context->getConfig("DISABLE_STYLE_AUTO_ADD_ID") != "true") {
        if (element->getAttribute("id").empty()) {
            std::string auto_id = "chtl-id-" + std::to_string(id_counter++);
            element->setAttribute("id", auto_id);
        }
    }
}

std::string CHTLGenerator::expandTemplate(const std::string& template_name, const std::string& template_type) {
    if (!context) return "";
    
    // Try to find template in global scope
    // This would integrate with GlobalMap to find template definitions
    
    std::stringstream result;
    
    if (template_type == "Style") {
        // Expand style template by including all defined properties
        result << "/* Template Style: " << template_name << " */\n";
        // In a full implementation, we would lookup the actual template definition
        // and expand all its properties here
        
    } else if (template_type == "Element") {
        // Expand element template by generating the defined HTML structure
        result << "<!-- Template Element: " << template_name << " -->\n";
        // In a full implementation, we would lookup the template definition
        // and generate the actual HTML elements here
        
    } else if (template_type == "Var") {
        // Variable templates are handled differently - they provide values
        // This would lookup variable definitions and return the requested value
        result << "/* Variable: " << template_name << " */";
    }
    
    return result.str();
}

std::string CHTLGenerator::expandCustom(const std::string& custom_name, const std::string& custom_type) {
    if (!context) return "";
    
    std::stringstream result;
    
    if (custom_type == "Style") {
        // Expand custom style with specialization support
        result << "/* Custom Style: " << custom_name << " */\n";
        // Handle custom style properties and specializations
        
    } else if (custom_type == "Element") {
        // Expand custom element with modification support
        result << "<!-- Custom Element: " << custom_name << " -->\n";
        // Handle element insertions, deletions, and modifications
        
    } else if (custom_type == "Var") {
        // Variable custom with parameter support
        result << "/* Custom Variable: " << custom_name << " */";
    }
    
    return result.str();
}

void CHTLGenerator::processInheritanceOperations(std::shared_ptr<CHTLNode> node) {
    if (!node) return;
    
    // Process inheritance operations in template/custom usage
    for (const auto& child : node->children) {
        if (child->name == "delete") {
            processDeleteOperation(child);
        } else if (child->name == "insert") {
            processInsertOperation(child);
        } else if (child->getAttribute("operation") == "inherit") {
            processInheritOperation(child);
        }
    }
}

void CHTLGenerator::processDeleteOperation(std::shared_ptr<CHTLNode> node) {
    // Handle delete operations for properties or elements
    std::string delete_items = node->value;
    
    if (context && context->getConfig("DEBUG_MODE") == "true") {
        writeHtml("<!-- Delete operation: " + delete_items + " -->\n");
    }
    
    // In a full implementation, this would:
    // 1. Parse the delete_items string
    // 2. Remove the specified properties/elements from the template
    // 3. Apply the deletion during template expansion
}

void CHTLGenerator::processInsertOperation(std::shared_ptr<CHTLNode> node) {
    // Handle insert operations
    std::string position = node->getAttribute("position");
    std::string target = node->getAttribute("target");
    
    if (context && context->getConfig("DEBUG_MODE") == "true") {
        writeHtml("<!-- Insert " + position + " " + target + " -->\n");
    }
    
    // Generate the inserted content
    for (const auto& child : node->children) {
        switch (child->type) {
            case NodeType::HTML_ELEMENT:
                generateElement(child);
                break;
            case NodeType::TEMPLATE_USAGE:
            case NodeType::CUSTOM_USAGE:
                generateTemplateUsage(child);
                break;
            default:
                break;
        }
    }
}

void CHTLGenerator::processInheritOperation(std::shared_ptr<CHTLNode> node) {
    // Handle explicit inheritance
    if (context && context->getConfig("DEBUG_MODE") == "true") {
        writeHtml("<!-- Inherit: " + node->name + " -->\n");
    }
    
    // In a full implementation, this would:
    // 1. Look up the inherited template/custom
    // 2. Merge its properties/elements into the current context
    // 3. Handle any conflicts or overrides
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