#ifndef CSS_COMPILER_H
#define CSS_COMPILER_H

#include <string>
#include <memory>
#include <vector>
#include "../CompilerDispatcher/CompilerDispatcher.h"

// Forward declarations for ANTLR generated classes
namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
    namespace tree {
        class ParseTree;
    }
}

// Generated ANTLR classes (will be generated from CSS.g4)
class CSSLexer;
class CSSParser;
class CSSBaseVisitor;

namespace CHTL {

// CSS AST Node types
enum class CSSNodeType {
    STYLESHEET,
    RULE_SET,
    AT_RULE,
    SELECTOR,
    DECLARATION,
    PROPERTY,
    VALUE,
    FUNCTION,
    COMMENT
};

// Base CSS AST Node
class CSSNode {
public:
    CSSNode(CSSNodeType type) : type_(type) {}
    virtual ~CSSNode() = default;
    
    CSSNodeType getType() const { return type_; }
    const std::vector<std::shared_ptr<CSSNode>>& getChildren() const { return children_; }
    void addChild(std::shared_ptr<CSSNode> child) { children_.push_back(child); }
    
    virtual std::string toString() const = 0;

protected:
    CSSNodeType type_;
    std::vector<std::shared_ptr<CSSNode>> children_;
};

// Stylesheet Node
class StylesheetNode : public CSSNode {
public:
    StylesheetNode() : CSSNode(CSSNodeType::STYLESHEET) {}
    std::string toString() const override;
};

// Rule Set Node
class RuleSetNode : public CSSNode {
public:
    RuleSetNode() : CSSNode(CSSNodeType::RULE_SET) {}
    
    void setSelectors(const std::vector<std::string>& selectors) { selectors_ = selectors; }
    const std::vector<std::string>& getSelectors() const { return selectors_; }
    
    std::string toString() const override;

private:
    std::vector<std::string> selectors_;
};

// At-Rule Node
class AtRuleNode : public CSSNode {
public:
    AtRuleNode(const std::string& keyword) 
        : CSSNode(CSSNodeType::AT_RULE), keyword_(keyword) {}
    
    const std::string& getKeyword() const { return keyword_; }
    void setContent(const std::string& content) { content_ = content; }
    
    std::string toString() const override;

private:
    std::string keyword_;
    std::string content_;
};

// Declaration Node
class DeclarationNode : public CSSNode {
public:
    DeclarationNode(const std::string& property, const std::string& value, bool important = false)
        : CSSNode(CSSNodeType::DECLARATION), property_(property), value_(value), important_(important) {}
    
    const std::string& getProperty() const { return property_; }
    const std::string& getValue() const { return value_; }
    bool isImportant() const { return important_; }
    
    std::string toString() const override;

private:
    std::string property_;
    std::string value_;
    bool important_;
};

// CSS Visitor for building AST
class CSSASTBuilder : public CSSBaseVisitor {
public:
    CSSASTBuilder();
    virtual ~CSSASTBuilder() = default;
    
    // Visit methods for CSS grammar rules
    virtual antlrcpp::Any visitStylesheet(CSSParser::StylesheetContext* ctx) override;
    virtual antlrcpp::Any visitRuleSet(CSSParser::RuleSetContext* ctx) override;
    virtual antlrcpp::Any visitAtRule(CSSParser::AtRuleContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration(CSSParser::DeclarationContext* ctx) override;
    virtual antlrcpp::Any visitSelector(CSSParser::SelectorContext* ctx) override;
    
    std::shared_ptr<StylesheetNode> getAST() const { return ast_; }

private:
    std::shared_ptr<StylesheetNode> ast_;
    
    std::string extractText(antlr4::tree::ParseTree* tree);
};

// CSS Compiler Implementation
class CSSCompilerANTLR : public CSSCompiler {
public:
    CSSCompilerANTLR();
    virtual ~CSSCompilerANTLR();
    
    // ICompiler interface
    CompileResult compile(const std::string& code, const CompileOptions& options) override;
    bool validate(const std::string& code) override;
    std::string getName() const override { return "ANTLR CSS Compiler"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    // CSSCompiler interface
    void setPreprocessor(const std::string& preprocessor) override;
    void setAutoprefixer(bool enable) override;

private:
    std::string preprocessor_;
    bool autoprefixer_ = false;
    
    // Compilation steps
    std::shared_ptr<StylesheetNode> parse(const std::string& code);
    std::string generate(std::shared_ptr<StylesheetNode> ast, const CompileOptions& options);
    std::string optimize(const std::string& css, const CompileOptions& options);
    std::string addPrefixes(const std::string& css);
    std::string minify(const std::string& css);
    std::string prettify(const std::string& css);
};

// CSS Error Listener
class CSSErrorListener : public antlr4::BaseErrorListener {
public:
    void syntaxError(antlr4::Recognizer* recognizer,
                    antlr4::Token* offendingSymbol,
                    size_t line,
                    size_t charPositionInLine,
                    const std::string& msg,
                    std::exception_ptr e) override;
    
    bool hasErrors() const { return !errors_.empty(); }
    const std::vector<std::string>& getErrors() const { return errors_; }

private:
    std::vector<std::string> errors_;
};

// CSS Utilities
class CSSUtil {
public:
    // Color conversion
    static std::string hexToRgb(const std::string& hex);
    static std::string rgbToHex(int r, int g, int b);
    static std::string hslToRgb(int h, int s, int l);
    
    // Unit conversion
    static double convertUnit(double value, const std::string& from, const std::string& to);
    
    // Selector specificity
    static int calculateSpecificity(const std::string& selector);
    
    // Vendor prefixes
    static std::vector<std::string> getVendorPrefixes(const std::string& property);
    
    // Property validation
    static bool isValidProperty(const std::string& property);
    static bool isValidValue(const std::string& property, const std::string& value);
};

} // namespace CHTL

#endif // CSS_COMPILER_H