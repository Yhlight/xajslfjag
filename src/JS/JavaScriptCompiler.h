#ifndef JAVASCRIPT_COMPILER_H
#define JAVASCRIPT_COMPILER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "../CompilerDispatcher/CompilerDispatcher.h"

// Forward declarations for ANTLR generated classes
namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
    namespace tree {
        class ParseTree;
    }
}

// Generated ANTLR classes (will be generated from JavaScript.g4)
class JavaScriptLexer;
class JavaScriptParser;
class JavaScriptBaseVisitor;

namespace CHTL {

// JavaScript AST Node types
enum class JSNodeType {
    PROGRAM,
    FUNCTION_DECLARATION,
    CLASS_DECLARATION,
    VARIABLE_DECLARATION,
    EXPRESSION_STATEMENT,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    RETURN_STATEMENT,
    BLOCK_STATEMENT,
    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    CALL_EXPRESSION,
    MEMBER_EXPRESSION,
    IDENTIFIER,
    LITERAL,
    ARRAY_EXPRESSION,
    OBJECT_EXPRESSION,
    ARROW_FUNCTION,
    IMPORT_DECLARATION,
    EXPORT_DECLARATION
};

// Base JavaScript AST Node
class JSNode {
public:
    JSNode(JSNodeType type) : type_(type) {}
    virtual ~JSNode() = default;
    
    JSNodeType getType() const { return type_; }
    const std::vector<std::shared_ptr<JSNode>>& getChildren() const { return children_; }
    void addChild(std::shared_ptr<JSNode> child) { children_.push_back(child); }
    
    virtual std::string toString() const = 0;
    virtual std::string toES5() const { return toString(); }
    virtual std::string toES6() const { return toString(); }

protected:
    JSNodeType type_;
    std::vector<std::shared_ptr<JSNode>> children_;
};

// Program Node
class ProgramNode : public JSNode {
public:
    ProgramNode() : JSNode(JSNodeType::PROGRAM) {}
    
    void setSourceType(const std::string& type) { sourceType_ = type; }
    const std::string& getSourceType() const { return sourceType_; }
    
    std::string toString() const override;

private:
    std::string sourceType_ = "script"; // "script" or "module"
};

// Function Declaration Node
class FunctionDeclarationNode : public JSNode {
public:
    FunctionDeclarationNode(const std::string& name) 
        : JSNode(JSNodeType::FUNCTION_DECLARATION), name_(name) {}
    
    void setAsync(bool async) { async_ = async; }
    void setGenerator(bool generator) { generator_ = generator; }
    void setParams(const std::vector<std::string>& params) { params_ = params; }
    void setBody(std::shared_ptr<JSNode> body) { body_ = body; }
    
    std::string toString() const override;
    std::string toES5() const override;

private:
    std::string name_;
    bool async_ = false;
    bool generator_ = false;
    std::vector<std::string> params_;
    std::shared_ptr<JSNode> body_;
};

// Variable Declaration Node
class VariableDeclarationNode : public JSNode {
public:
    VariableDeclarationNode(const std::string& kind) 
        : JSNode(JSNodeType::VARIABLE_DECLARATION), kind_(kind) {}
    
    struct Declaration {
        std::string name;
        std::shared_ptr<JSNode> init;
    };
    
    void addDeclaration(const Declaration& decl) { declarations_.push_back(decl); }
    
    std::string toString() const override;
    std::string toES5() const override;

private:
    std::string kind_; // "var", "let", "const"
    std::vector<Declaration> declarations_;
};

// Binary Expression Node
class BinaryExpressionNode : public JSNode {
public:
    BinaryExpressionNode(const std::string& op, 
                        std::shared_ptr<JSNode> left, 
                        std::shared_ptr<JSNode> right)
        : JSNode(JSNodeType::BINARY_EXPRESSION), operator_(op), left_(left), right_(right) {}
    
    std::string toString() const override;

private:
    std::string operator_;
    std::shared_ptr<JSNode> left_;
    std::shared_ptr<JSNode> right_;
};

// Literal Node
class LiteralNode : public JSNode {
public:
    LiteralNode(const std::string& value, const std::string& raw = "")
        : JSNode(JSNodeType::LITERAL), value_(value), raw_(raw.empty() ? value : raw) {}
    
    std::string toString() const override { return raw_; }

private:
    std::string value_;
    std::string raw_;
};

// JavaScript Visitor for building AST
class JSASTBuilder : public JavaScriptBaseVisitor {
public:
    JSASTBuilder();
    virtual ~JSASTBuilder() = default;
    
    // Visit methods for JavaScript grammar rules
    virtual antlrcpp::Any visitProgram(JavaScriptParser::ProgramContext* ctx) override;
    virtual antlrcpp::Any visitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext* ctx) override;
    virtual antlrcpp::Any visitVariableStatement(JavaScriptParser::VariableStatementContext* ctx) override;
    virtual antlrcpp::Any visitExpressionStatement(JavaScriptParser::ExpressionStatementContext* ctx) override;
    virtual antlrcpp::Any visitSingleExpression(JavaScriptParser::SingleExpressionContext* ctx) override;
    
    std::shared_ptr<ProgramNode> getAST() const { return ast_; }

private:
    std::shared_ptr<ProgramNode> ast_;
    
    std::string extractText(antlr4::tree::ParseTree* tree);
};

// JavaScript Compiler Implementation
class JavaScriptCompilerANTLR : public JavaScriptCompiler {
public:
    JavaScriptCompilerANTLR();
    virtual ~JavaScriptCompilerANTLR();
    
    // ICompiler interface
    CompileResult compile(const std::string& code, const CompileOptions& options) override;
    bool validate(const std::string& code) override;
    std::string getName() const override { return "ANTLR JavaScript Compiler"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    // JavaScriptCompiler interface
    void setTargetVersion(const std::string& version) override;
    void setModuleSystem(const std::string& system) override;

private:
    std::string targetVersion_ = "ES6";
    std::string moduleSystem_ = "ESM";
    
    // Compilation steps
    std::shared_ptr<ProgramNode> parse(const std::string& code);
    std::string generate(std::shared_ptr<ProgramNode> ast, const CompileOptions& options);
    std::string transpile(const std::string& js, const std::string& targetVersion);
    std::string transformModules(const std::string& js, const std::string& moduleSystem);
    std::string optimize(const std::string& js, const CompileOptions& options);
    std::string minify(const std::string& js);
    std::string prettify(const std::string& js);
    std::string generateSourceMap(const std::string& original, const std::string& generated);
};

// JavaScript Error Listener
class JSErrorListener : public antlr4::BaseErrorListener {
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

// JavaScript Transformer (for ES6 to ES5 conversion)
class JSTransformer {
public:
    JSTransformer(const std::string& targetVersion) : targetVersion_(targetVersion) {}
    
    std::string transform(std::shared_ptr<JSNode> ast);
    
    // Transformation methods
    std::string transformArrowFunctions(const std::string& code);
    std::string transformClasses(const std::string& code);
    std::string transformLetConst(const std::string& code);
    std::string transformTemplateLiterals(const std::string& code);
    std::string transformDestructuring(const std::string& code);
    std::string transformSpread(const std::string& code);
    std::string transformAsyncAwait(const std::string& code);
    std::string transformModules(const std::string& code, const std::string& system);

private:
    std::string targetVersion_;
    
    bool needsTransformation(const std::string& feature);
};

// JavaScript Utilities
class JSUtil {
public:
    // Scope analysis
    static std::vector<std::string> findVariables(std::shared_ptr<JSNode> ast);
    static std::vector<std::string> findFunctions(std::shared_ptr<JSNode> ast);
    static std::vector<std::string> findClasses(std::shared_ptr<JSNode> ast);
    
    // Dead code elimination
    static std::shared_ptr<JSNode> removeUnusedCode(std::shared_ptr<JSNode> ast);
    
    // Constant folding
    static std::shared_ptr<JSNode> foldConstants(std::shared_ptr<JSNode> ast);
    
    // Variable renaming (for minification)
    static std::unordered_map<std::string, std::string> generateVariableMap(
        const std::vector<std::string>& variables);
    
    // Module analysis
    static std::vector<std::string> findImports(std::shared_ptr<JSNode> ast);
    static std::vector<std::string> findExports(std::shared_ptr<JSNode> ast);
    
    // Validation
    static bool isValidIdentifier(const std::string& name);
    static bool isReservedWord(const std::string& word);
};

} // namespace CHTL

#endif // JAVASCRIPT_COMPILER_H