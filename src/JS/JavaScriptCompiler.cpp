#include "JavaScriptCompiler.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <unordered_set>

namespace CHTL {

// Mock ANTLR classes (replace with generated code)
namespace {
    class MockJSLexer {
    public:
        MockJSLexer(const std::string& input) : input_(input) {}
        // Mock implementation
    private:
        std::string input_;
    };
    
    class MockJSParser {
    public:
        MockJSParser(MockJSLexer& lexer) : lexer_(lexer) {}
        // Mock implementation
        void parse() {
            // Simple JS parsing logic
        }
    private:
        MockJSLexer& lexer_;
    };
}

// JavaScript Node implementations

std::string ProgramNode::toString() const {
    std::stringstream ss;
    
    if (sourceType_ == "module") {
        ss << "'use strict';\n\n";
    }
    
    for (const auto& child : children_) {
        ss << child->toString();
        if (child->getType() != JSNodeType::BLOCK_STATEMENT) {
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::string FunctionDeclarationNode::toString() const {
    std::stringstream ss;
    
    if (async_) ss << "async ";
    ss << "function";
    if (generator_) ss << "*";
    ss << " " << name_ << "(";
    
    for (size_t i = 0; i < params_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << params_[i];
    }
    
    ss << ") ";
    if (body_) {
        ss << body_->toString();
    } else {
        ss << "{}";
    }
    
    return ss.str();
}

std::string FunctionDeclarationNode::toES5() const {
    if (!async_ && !generator_) {
        return toString();
    }
    
    // Transform async/generator to ES5
    std::stringstream ss;
    ss << "function " << name_ << "(";
    
    for (size_t i = 0; i < params_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << params_[i];
    }
    
    ss << ") ";
    
    if (async_) {
        // Wrap in Promise
        ss << "{ return new Promise(function(resolve, reject) { try { ";
        if (body_) {
            ss << "resolve((function() " << body_->toString() << ")())";
        }
        ss << " } catch(e) { reject(e); } }); }";
    } else {
        if (body_) {
            ss << body_->toString();
        } else {
            ss << "{}";
        }
    }
    
    return ss.str();
}

std::string VariableDeclarationNode::toString() const {
    std::stringstream ss;
    ss << kind_ << " ";
    
    for (size_t i = 0; i < declarations_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << declarations_[i].name;
        if (declarations_[i].init) {
            ss << " = " << declarations_[i].init->toString();
        }
    }
    
    ss << ";";
    return ss.str();
}

std::string VariableDeclarationNode::toES5() const {
    if (kind_ == "var") {
        return toString();
    }
    
    // Convert let/const to var
    std::stringstream ss;
    ss << "var ";
    
    for (size_t i = 0; i < declarations_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << declarations_[i].name;
        if (declarations_[i].init) {
            ss << " = " << declarations_[i].init->toString();
        }
    }
    
    ss << ";";
    return ss.str();
}

std::string BinaryExpressionNode::toString() const {
    return "(" + left_->toString() + " " + operator_ + " " + right_->toString() + ")";
}

// JavaScriptCompilerANTLR Implementation

JavaScriptCompilerANTLR::JavaScriptCompilerANTLR() {}

JavaScriptCompilerANTLR::~JavaScriptCompilerANTLR() = default;

CompileResult JavaScriptCompilerANTLR::compile(const std::string& code, const CompileOptions& options) {
    CompileResult result;
    
    try {
        // Parse JavaScript
        auto ast = parse(code);
        if (!ast) {
            result.success = false;
            result.errors.push_back("Failed to parse JavaScript");
            return result;
        }
        
        // Generate JavaScript
        std::string js = generate(ast, options);
        
        // Transpile if needed
        if (targetVersion_ != "ES6" && targetVersion_ != "ES2015") {
            js = transpile(js, targetVersion_);
        }
        
        // Transform modules if needed
        if (ast->getSourceType() == "module" && moduleSystem_ != "ESM") {
            js = transformModules(js, moduleSystem_);
        }
        
        // Optimize
        js = optimize(js, options);
        
        // Minify or prettify
        if (options.minify) {
            js = minify(js);
        } else if (options.prettify) {
            js = prettify(js);
        }
        
        // Generate source map if requested
        if (options.generateSourceMap) {
            result.sourceMap = generateSourceMap(code, js);
        }
        
        result.jsOutput = js;
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("JavaScript compilation error: ") + e.what());
    }
    
    return result;
}

bool JavaScriptCompilerANTLR::validate(const std::string& code) {
    try {
        auto ast = parse(code);
        return ast != nullptr;
    } catch (...) {
        return false;
    }
}

void JavaScriptCompilerANTLR::setTargetVersion(const std::string& version) {
    targetVersion_ = version;
}

void JavaScriptCompilerANTLR::setModuleSystem(const std::string& system) {
    moduleSystem_ = system;
}

std::shared_ptr<ProgramNode> JavaScriptCompilerANTLR::parse(const std::string& code) {
    // This would use ANTLR-generated parser
    // For now, create a simple mock implementation
    
    auto program = std::make_shared<ProgramNode>();
    
    // Check if it's a module
    if (code.find("import ") != std::string::npos || 
        code.find("export ") != std::string::npos) {
        program->setSourceType("module");
    }
    
    // Simple function detection
    std::regex funcRegex(R"((?:async\s+)?function\s*\*?\s+(\w+)\s*\([^)]*\)\s*\{)");
    std::smatch match;
    std::string::const_iterator searchStart(code.cbegin());
    
    while (std::regex_search(searchStart, code.cend(), match, funcRegex)) {
        std::string funcName = match[1];
        auto funcNode = std::make_shared<FunctionDeclarationNode>(funcName);
        
        if (match[0].find("async") != std::string::npos) {
            funcNode->setAsync(true);
        }
        if (match[0].find("*") != std::string::npos) {
            funcNode->setGenerator(true);
        }
        
        program->addChild(funcNode);
        searchStart = match.suffix().first;
    }
    
    // Simple variable detection
    std::regex varRegex(R"((var|let|const)\s+(\w+)(?:\s*=\s*([^;]+))?\s*;)");
    searchStart = code.cbegin();
    
    while (std::regex_search(searchStart, code.cend(), match, varRegex)) {
        std::string kind = match[1];
        std::string varName = match[2];
        
        auto varNode = std::make_shared<VariableDeclarationNode>(kind);
        VariableDeclarationNode::Declaration decl;
        decl.name = varName;
        
        if (match[3].matched) {
            decl.init = std::make_shared<LiteralNode>(match[3]);
        }
        
        varNode->addDeclaration(decl);
        program->addChild(varNode);
        
        searchStart = match.suffix().first;
    }
    
    return program;
}

std::string JavaScriptCompilerANTLR::generate(std::shared_ptr<ProgramNode> ast, const CompileOptions& options) {
    if (targetVersion_ == "ES5") {
        // Generate ES5 compatible code
        std::stringstream ss;
        for (const auto& child : ast->getChildren()) {
            ss << child->toES5() << "\n";
        }
        return ss.str();
    }
    
    return ast->toString();
}

std::string JavaScriptCompilerANTLR::transpile(const std::string& js, const std::string& targetVersion) {
    JSTransformer transformer(targetVersion);
    
    std::string result = js;
    
    // Transform features based on target version
    if (targetVersion == "ES5") {
        result = transformer.transformArrowFunctions(result);
        result = transformer.transformClasses(result);
        result = transformer.transformLetConst(result);
        result = transformer.transformTemplateLiterals(result);
        result = transformer.transformDestructuring(result);
        result = transformer.transformSpread(result);
        result = transformer.transformAsyncAwait(result);
    }
    
    return result;
}

std::string JavaScriptCompilerANTLR::transformModules(const std::string& js, const std::string& moduleSystem) {
    std::string result = js;
    
    if (moduleSystem == "CommonJS") {
        // Transform ES6 imports to require()
        std::regex importRegex(R"(import\s+(\w+)\s+from\s+['"]([^'"]+)['"];?)");
        result = std::regex_replace(result, importRegex, "const $1 = require('$2');");
        
        // Transform ES6 exports to module.exports
        std::regex exportRegex(R"(export\s+default\s+(\w+);?)");
        result = std::regex_replace(result, exportRegex, "module.exports = $1;");
        
        std::regex namedExportRegex(R"(export\s+\{([^}]+)\};?)");
        result = std::regex_replace(result, namedExportRegex, "module.exports = {$1};");
    }
    else if (moduleSystem == "AMD") {
        // Wrap in define()
        std::stringstream ss;
        ss << "define(function(require, exports, module) {\n";
        ss << result;
        ss << "\n});";
        result = ss.str();
    }
    
    return result;
}

std::string JavaScriptCompilerANTLR::optimize(const std::string& js, const CompileOptions& options) {
    std::string optimized = js;
    
    // Simple optimizations
    // Remove console.log statements in production
    if (!options.enableDebugInfo) {
        std::regex consoleRegex(R"(console\.(log|debug|info|warn|error)\([^)]*\);?\s*)");
        optimized = std::regex_replace(optimized, consoleRegex, "");
    }
    
    // Remove debugger statements
    std::regex debuggerRegex(R"(debugger;?\s*)");
    optimized = std::regex_replace(optimized, debuggerRegex, "");
    
    return optimized;
}

std::string JavaScriptCompilerANTLR::minify(const std::string& js) {
    std::string minified;
    minified.reserve(js.size());
    
    bool inString = false;
    bool inRegex = false;
    char stringChar = 0;
    bool lastWasAlnum = false;
    bool inLineComment = false;
    bool inBlockComment = false;
    
    for (size_t i = 0; i < js.length(); ++i) {
        char ch = js[i];
        char nextCh = (i + 1 < js.length()) ? js[i + 1] : '\0';
        
        // Handle comments
        if (!inString && !inRegex) {
            if (ch == '/' && nextCh == '/') {
                inLineComment = true;
                i++; // Skip next char
                continue;
            }
            if (ch == '/' && nextCh == '*') {
                inBlockComment = true;
                i++; // Skip next char
                continue;
            }
        }
        
        if (inLineComment && ch == '\n') {
            inLineComment = false;
            // Keep newline to prevent syntax errors
            minified += '\n';
            continue;
        }
        
        if (inBlockComment && ch == '*' && nextCh == '/') {
            inBlockComment = false;
            i++; // Skip next char
            continue;
        }
        
        if (inLineComment || inBlockComment) {
            continue;
        }
        
        // Handle strings
        if ((ch == '"' || ch == '\'' || ch == '`') && (i == 0 || js[i-1] != '\\')) {
            if (!inString && !inRegex) {
                inString = true;
                stringChar = ch;
            } else if (inString && ch == stringChar) {
                inString = false;
            }
        }
        
        // Handle regex (simplified)
        if (ch == '/' && !inString && i > 0 && 
            (js[i-1] == '=' || js[i-1] == '(' || js[i-1] == ',')) {
            inRegex = true;
        } else if (inRegex && ch == '/' && js[i-1] != '\\') {
            inRegex = false;
        }
        
        if (inString || inRegex) {
            minified += ch;
            continue;
        }
        
        // Skip whitespace
        if (std::isspace(ch)) {
            // Keep space if needed between identifiers
            if (lastWasAlnum && i + 1 < js.length() && std::isalnum(js[i + 1])) {
                minified += ' ';
                lastWasAlnum = false;
            }
            continue;
        }
        
        minified += ch;
        lastWasAlnum = std::isalnum(ch);
    }
    
    return minified;
}

std::string JavaScriptCompilerANTLR::prettify(const std::string& js) {
    std::string pretty;
    int indentLevel = 0;
    const std::string indent = "  ";
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < js.length(); ++i) {
        char ch = js[i];
        
        // Handle strings
        if ((ch == '"' || ch == '\'' || ch == '`') && (i == 0 || js[i-1] != '\\')) {
            if (!inString) {
                inString = true;
                stringChar = ch;
            } else if (ch == stringChar) {
                inString = false;
            }
        }
        
        if (inString) {
            pretty += ch;
            continue;
        }
        
        if (ch == '{') {
            pretty += " {\n";
            ++indentLevel;
            // Add indent for next line
            for (int j = 0; j < indentLevel; ++j) {
                pretty += indent;
            }
        } else if (ch == '}') {
            // Remove last indent
            if (!pretty.empty() && pretty.substr(pretty.length() - indent.length()) == indent) {
                pretty.erase(pretty.length() - indent.length());
            }
            --indentLevel;
            pretty += "}\n";
            // Add indent for next line if not EOF
            if (i + 1 < js.length()) {
                for (int j = 0; j < indentLevel; ++j) {
                    pretty += indent;
                }
            }
        } else if (ch == ';') {
            pretty += ";\n";
            // Add indent for next line
            for (int j = 0; j < indentLevel; ++j) {
                pretty += indent;
            }
        } else if (ch == '\n') {
            pretty += '\n';
            // Add indent for next line
            for (int j = 0; j < indentLevel; ++j) {
                pretty += indent;
            }
        } else if (!std::isspace(ch)) {
            pretty += ch;
        } else if (!pretty.empty() && pretty.back() != ' ' && pretty.back() != '\n') {
            pretty += ' ';
        }
    }
    
    return pretty;
}

std::string JavaScriptCompilerANTLR::generateSourceMap(const std::string& original, const std::string& generated) {
    // Simplified source map generation
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"version\": 3,\n";
    ss << "  \"file\": \"output.js\",\n";
    ss << "  \"sourceRoot\": \"\",\n";
    ss << "  \"sources\": [\"input.js\"],\n";
    ss << "  \"names\": [],\n";
    ss << "  \"mappings\": \"AAAA\"\n"; // Simplified mapping
    ss << "}";
    return ss.str();
}

// JSTransformer Implementation

std::string JSTransformer::transform(std::shared_ptr<JSNode> ast) {
    // Transform based on target version
    return ast->toString();
}

std::string JSTransformer::transformArrowFunctions(const std::string& code) {
    // Transform arrow functions to regular functions
    std::regex arrowRegex(R"(\(([^)]*)\)\s*=>\s*\{)");
    std::string result = std::regex_replace(code, arrowRegex, "function($1) {");
    
    // Handle single expression arrows
    std::regex singleExprArrow(R"(\(([^)]*)\)\s*=>\s*([^{][^;]+))");
    result = std::regex_replace(result, singleExprArrow, "function($1) { return $2; }");
    
    return result;
}

std::string JSTransformer::transformClasses(const std::string& code) {
    // Simple class to function transformation
    std::regex classRegex(R"(class\s+(\w+)\s*\{)");
    std::string result = std::regex_replace(code, classRegex, "function $1() {");
    
    // Transform constructor
    std::regex constructorRegex(R"(constructor\s*\([^)]*\)\s*\{)");
    result = std::regex_replace(result, constructorRegex, "function $1(");
    
    return result;
}

std::string JSTransformer::transformLetConst(const std::string& code) {
    // Replace let and const with var
    std::regex letConstRegex(R"(\b(let|const)\b)");
    return std::regex_replace(code, letConstRegex, "var");
}

std::string JSTransformer::transformTemplateLiterals(const std::string& code) {
    // Transform template literals to string concatenation
    std::regex templateRegex(R"(`([^`]*)\$\{([^}]+)\}([^`]*)`)");
    std::string result = code;
    
    while (std::regex_search(result, templateRegex)) {
        result = std::regex_replace(result, templateRegex, "\"$1\" + ($2) + \"$3\"", 
                                  std::regex_constants::format_first_only);
    }
    
    // Handle simple template literals without expressions
    std::regex simpleTemplateRegex(R"(`([^`]*)`)");
    result = std::regex_replace(result, simpleTemplateRegex, "\"$1\"");
    
    return result;
}

std::string JSTransformer::transformDestructuring(const std::string& code) {
    // Simple destructuring transformation
    // This is a very simplified version
    return code;
}

std::string JSTransformer::transformSpread(const std::string& code) {
    // Transform spread operator
    // This is a very simplified version
    return code;
}

std::string JSTransformer::transformAsyncAwait(const std::string& code) {
    // Transform async/await to promises
    std::regex asyncFuncRegex(R"(async\s+function\s+(\w+))");
    std::string result = std::regex_replace(code, asyncFuncRegex, "function $1");
    
    // Transform await to .then()
    std::regex awaitRegex(R"(await\s+([^;]+))");
    result = std::regex_replace(result, awaitRegex, "$1.then(function(result) { return result; })");
    
    return result;
}

std::string JSTransformer::transformModules(const std::string& code, const std::string& system) {
    // Module transformation handled in main compiler
    return code;
}

bool JSTransformer::needsTransformation(const std::string& feature) {
    if (targetVersion_ == "ES5") {
        static const std::unordered_set<std::string> es6Features = {
            "arrow", "class", "let", "const", "template", "destructuring",
            "spread", "async", "await", "import", "export"
        };
        return es6Features.find(feature) != es6Features.end();
    }
    return false;
}

// JSUtil Implementation

std::vector<std::string> JSUtil::findVariables(std::shared_ptr<JSNode> ast) {
    std::vector<std::string> variables;
    // Traverse AST and collect variable names
    return variables;
}

std::vector<std::string> JSUtil::findFunctions(std::shared_ptr<JSNode> ast) {
    std::vector<std::string> functions;
    // Traverse AST and collect function names
    return functions;
}

std::vector<std::string> JSUtil::findClasses(std::shared_ptr<JSNode> ast) {
    std::vector<std::string> classes;
    // Traverse AST and collect class names
    return classes;
}

bool JSUtil::isValidIdentifier(const std::string& name) {
    if (name.empty()) return false;
    
    // Must start with letter, underscore, or dollar sign
    char first = name[0];
    if (!std::isalpha(first) && first != '_' && first != '$') {
        return false;
    }
    
    // Rest can be letters, digits, underscores, or dollar signs
    for (size_t i = 1; i < name.length(); ++i) {
        char ch = name[i];
        if (!std::isalnum(ch) && ch != '_' && ch != '$') {
            return false;
        }
    }
    
    // Check if it's a reserved word
    return !isReservedWord(name);
}

bool JSUtil::isReservedWord(const std::string& word) {
    static const std::unordered_set<std::string> reserved = {
        "break", "case", "catch", "class", "const", "continue",
        "debugger", "default", "delete", "do", "else", "export",
        "extends", "finally", "for", "function", "if", "import",
        "in", "instanceof", "new", "return", "super", "switch",
        "this", "throw", "try", "typeof", "var", "void", "while",
        "with", "yield", "let", "static", "async", "await",
        // Future reserved words
        "enum", "implements", "interface", "package", "private",
        "protected", "public",
        // Literals
        "null", "true", "false"
    };
    
    return reserved.find(word) != reserved.end();
}

} // namespace CHTL