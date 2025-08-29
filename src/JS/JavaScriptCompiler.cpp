#include "JavaScriptCompiler.h"
#include "generated/Grammars/JavaScript/JavaScriptLexer.h"
#include "generated/Grammars/JavaScript/JavaScriptParser.h"
#include "generated/Grammars/JavaScript/JavaScriptParserBaseVisitor.h"
#include "../Error/ErrorReport.h"
#include <antlr4-runtime.h>
#include <sstream>

using namespace antlr4;
using namespace CHTL;

// JavaScript访问者实现
class JavaScriptVisitorImpl : public JavaScriptParserBaseVisitor {
private:
    std::stringstream output_;
    int indentLevel_ = 0;
    std::string targetVersion_;
    
    std::string indent() {
        return std::string(indentLevel_ * 2, ' ');
    }

public:
    JavaScriptVisitorImpl(const std::string& targetVersion) 
        : targetVersion_(targetVersion) {}
    
    std::string getOutput() { return output_.str(); }
    
    antlrcpp::Any visitProgram(JavaScriptParserParser::ProgramContext *ctx) override {
        if (ctx->sourceElements()) {
            visit(ctx->sourceElements());
        }
        return nullptr;
    }
    
    antlrcpp::Any visitSourceElements(JavaScriptParserParser::SourceElementsContext *ctx) override {
        for (auto element : ctx->sourceElement()) {
            visit(element);
            output_ << "\n";
        }
        return nullptr;
    }
    
    antlrcpp::Any visitStatement(JavaScriptParserParser::StatementContext *ctx) override {
        output_ << indent();
        visitChildren(ctx);
        return nullptr;
    }
    
    antlrcpp::Any visitBlock(JavaScriptParserParser::BlockContext *ctx) override {
        output_ << "{\n";
        indentLevel_++;
        
        if (ctx->statementList()) {
            visit(ctx->statementList());
        }
        
        indentLevel_--;
        output_ << indent() << "}";
        return nullptr;
    }
    
    antlrcpp::Any visitVariableStatement(JavaScriptParserParser::VariableStatementContext *ctx) override {
        output_ << indent();
        visit(ctx->variableDeclarationList());
        output_ << ";";
        return nullptr;
    }
    
    antlrcpp::Any visitVariableDeclarationList(JavaScriptParserParser::VariableDeclarationListContext *ctx) override {
        output_ << ctx->varModifier()->getText() << " ";
        
        bool first = true;
        for (auto decl : ctx->variableDeclaration()) {
            if (!first) output_ << ", ";
            first = false;
            visit(decl);
        }
        
        return nullptr;
    }
    
    antlrcpp::Any visitFunctionDeclaration(JavaScriptParserParser::FunctionDeclarationContext *ctx) override {
        output_ << indent();
        if (ctx->Async()) output_ << "async ";
        output_ << "function ";
        if (ctx->identifier()) output_ << ctx->identifier()->getText();
        output_ << "(";
        
        if (ctx->formalParameterList()) {
            visit(ctx->formalParameterList());
        }
        
        output_ << ") ";
        visit(ctx->functionBody());
        
        return nullptr;
    }
    
    // 默认访问方法 - 直接输出文本
    antlrcpp::Any visitTerminal(tree::TerminalNode *node) override {
        output_ << node->getText() << " ";
        return nullptr;
    }
};

// 错误监听器
class JavaScriptErrorListener : public BaseErrorListener {
private:
    std::vector<std::string>& errors_;
    
public:
    JavaScriptErrorListener(std::vector<std::string>& errors) : errors_(errors) {}
    
    void syntaxError(Recognizer *recognizer, Token *offendingSymbol,
                    size_t line, size_t charPositionInLine,
                    const std::string &msg, std::exception_ptr e) override {
        std::ostringstream ss;
        ss << "JavaScript Syntax error at line " << line << ":" << charPositionInLine 
           << " - " << msg;
        errors_.push_back(ss.str());
        
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
            .withMessage("JavaScript Syntax Error")
            .withDetail(msg)
            .withLocation(line, charPositionInLine)
            .report();
    }
};

// JavaScript编译器实现
JavaScriptCompilerImpl::JavaScriptCompilerImpl() {
    ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
        .withMessage("JavaScript Compiler initialized")
        .withDetail("Using ANTLR4 parser")
        .report();
}

CompileResult JavaScriptCompilerImpl::compile(const std::string& code, const CompileOptions& options) {
    CompileResult result;
    
    ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
        .withMessage("JavaScript Compiler received complete JS code")
        .withDetail("Code length: " + std::to_string(code.length()) + " characters")
        .report();
    
    try {
        // 创建ANTLR输入流
        ANTLRInputStream input(code);
        
        // 创建词法分析器
        JavaScriptLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        
        // 创建语法分析器
        JavaScriptParserParser parser(&tokens);
        
        // 添加错误监听器
        parser.removeErrorListeners();
        JavaScriptErrorListener errorListener(result.errors);
        parser.addErrorListener(&errorListener);
        
        // 使用SLL预测模式以提高性能
        parser.getInterpreter<atn::ParserATNSimulator>()->setPredictionMode(atn::PredictionMode::SLL);
        
        tree::ParseTree* tree = nullptr;
        try {
            // 尝试使用SLL模式解析
            tree = parser.program();
        } catch (std::exception &e) {
            // 如果SLL失败，回退到LL模式
            tokens.reset();
            parser.reset();
            parser.getInterpreter<atn::ParserATNSimulator>()->setPredictionMode(atn::PredictionMode::LL);
            tree = parser.program();
        }
        
        // 使用访问者生成代码
        JavaScriptVisitorImpl visitor(targetVersion_);
        visitor.visit(tree);
        
        result.jsOutput = visitor.getOutput();
        result.success = result.errors.empty();
        
        if (result.success) {
            ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
                .withMessage("JavaScript compilation successful")
                .withDetail("Output size: " + std::to_string(result.jsOutput.length()) + " characters")
                .report();
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(e.what());
        
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::INTERNAL_ERROR)
            .withMessage("JavaScript compilation failed")
            .withDetail(e.what())
            .report();
    }
    
    return result;
}

bool JavaScriptCompilerImpl::validate(const std::string& code) {
    try {
        ANTLRInputStream input(code);
        JavaScriptLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        JavaScriptParserParser parser(&tokens);
        
        // 静默模式
        parser.removeErrorListeners();
        parser.program();
        
        return true;
    } catch (...) {
        return false;
    }
}

std::string JavaScriptCompilerImpl::getName() const { 
    return "JavaScript Compiler (ANTLR)"; 
}

std::string JavaScriptCompilerImpl::getVersion() const { 
    return "1.0.0"; 
}

void JavaScriptCompilerImpl::setTargetVersion(const std::string& version) {
    targetVersion_ = version;
}

void JavaScriptCompilerImpl::setModuleSystem(const std::string& system) {
    moduleSystem_ = system;
}