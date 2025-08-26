#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/CHTLNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <sstream>
#include <memory>

namespace CHTL {

class CHTLGenerator {
private:
    std::stringstream html_output;
    std::stringstream css_output;
    std::stringstream js_output;
    
    CHTLContext* context;
    
    // Internal state
    int indent_level;
    bool pretty_print;
    
    // CSS generation state
    std::vector<std::string> global_styles;
    std::vector<std::string> local_styles;
    int class_counter;
    int id_counter;
    
    // Helper methods
    void writeIndent();
    void writeLine(const std::string& content);
    void writeHtml(const std::string& content);
    void writeCss(const std::string& content);
    void writeJs(const std::string& content);
    
    std::string generateUniqueClass();
    std::string generateUniqueId();
    
    // Node generation methods
    void generateDocument(std::shared_ptr<CHTLNode> node);
    void generateElement(std::shared_ptr<CHTLNode> node);
    void generateTextNode(std::shared_ptr<CHTLNode> node);
    void generateStyleBlock(std::shared_ptr<CHTLNode> node);
    void generateScriptBlock(std::shared_ptr<CHTLNode> node);
    void generateTemplateDefinition(std::shared_ptr<CHTLNode> node);
    void generateCustomDefinition(std::shared_ptr<CHTLNode> node);
    void generateTemplateUsage(std::shared_ptr<CHTLNode> node);
    void generateCustomUsage(std::shared_ptr<CHTLNode> node);
    void generateComment(std::shared_ptr<CHTLNode> node);
    void generateAttribute(std::shared_ptr<CHTLNode> node);
    void generateStyleRule(std::shared_ptr<CHTLNode> node);
    
    // Template and custom expansion
    std::string expandTemplate(const std::string& template_name, const std::string& template_type);
    std::string expandCustom(const std::string& custom_name, const std::string& custom_type);
    
    // Style processing
    void processLocalStyles(std::shared_ptr<CHTLNode> element, std::shared_ptr<CHTLNode> style_block);
    void processGlobalStyles();
    std::string generateSelector(const std::string& selector, const std::string& context_element);
    
    // Auto-generated classes and IDs
    void autoAddClassesAndIds(std::shared_ptr<CHTLNode> element);
    
public:
    CHTLGenerator(CHTLContext* ctx = nullptr);
    
    // Main generation methods
    std::string generateHTML(std::shared_ptr<CHTLNode> ast);
    std::string generateCSS(std::shared_ptr<CHTLNode> ast);
    std::string generateJS(std::shared_ptr<CHTLNode> ast);
    
    // Combined generation
    struct GeneratedCode {
        std::string html;
        std::string css;
        std::string js;
    };
    
    GeneratedCode generate(std::shared_ptr<CHTLNode> ast);
    
    // Configuration
    void setPrettyPrint(bool enable) { pretty_print = enable; }
    void setContext(CHTLContext* ctx) { context = ctx; }
    
    // Utility
    void reset();
    std::string escapeHtml(const std::string& text);
    std::string escapeCss(const std::string& text);
    std::string escapeJs(const std::string& text);
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H