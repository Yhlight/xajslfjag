#include "CSSCompiler.h"
#include "generated/src/CSS/CSSSimpleLexer.h"
#include "generated/src/CSS/CSSSimpleParser.h"
#include "generated/src/CSS/CSSSimpleBaseVisitor.h"
#include "../Error/ErrorReport.h"
#include <antlr4-runtime.h>
#include <sstream>

using namespace antlr4;
using namespace CHTL;

// CSS访问者实现
class CSSVisitorImpl : public CSSSimpleBaseVisitor {
private:
    std::stringstream output_;
    int indentLevel_ = 0;
    
    std::string indent() {
        return std::string(indentLevel_ * 2, ' ');
    }

public:
    std::string getOutput() { return output_.str(); }
    
    antlrcpp::Any visitStylesheet(CSSSimpleParser::StylesheetContext *ctx) override {
        for (auto stmt : ctx->statement()) {
            visit(stmt);
        }
        return nullptr;
    }
    
    antlrcpp::Any visitRuleset(CSSSimpleParser::RulesetContext *ctx) override {
        // 输出选择器
        bool first = true;
        for (auto selector : ctx->selector()) {
            if (!first) output_ << ", ";
            first = false;
            visit(selector);
        }
        
        output_ << " {\n";
        indentLevel_++;
        
        // 输出声明
        for (auto decl : ctx->declaration()) {
            visit(decl);
        }
        
        indentLevel_--;
        output_ << "}\n\n";
        
        return nullptr;
    }
    
    antlrcpp::Any visitSelector(CSSSimpleParser::SelectorContext *ctx) override {
        for (auto simple : ctx->simpleSelector()) {
            visit(simple);
        }
        return nullptr;
    }
    
    antlrcpp::Any visitSimpleSelector(CSSSimpleParser::SimpleSelectorContext *ctx) override {
        output_ << ctx->getText();
        return nullptr;
    }
    
    antlrcpp::Any visitDeclaration(CSSSimpleParser::DeclarationContext *ctx) override {
        output_ << indent();
        visit(ctx->property());
        output_ << ": ";
        visit(ctx->value());
        output_ << ";\n";
        return nullptr;
    }
    
    antlrcpp::Any visitProperty(CSSSimpleParser::PropertyContext *ctx) override {
        output_ << ctx->getText();
        return nullptr;
    }
    
    antlrcpp::Any visitValue(CSSSimpleParser::ValueContext *ctx) override {
        output_ << ctx->getText();
        return nullptr;
    }
    
    antlrcpp::Any visitAtRule(CSSSimpleParser::AtRuleContext *ctx) override {
        output_ << "@" << ctx->Ident()->getText();
        
        for (auto any : ctx->any()) {
            output_ << " " << any->getText();
        }
        
        if (ctx->getText().find(';') != std::string::npos) {
            output_ << ";\n";
        } else {
            output_ << " {\n";
            indentLevel_++;
            for (auto stmt : ctx->statement()) {
                visit(stmt);
            }
            indentLevel_--;
            output_ << "}\n";
        }
        
        return nullptr;
    }
};

// 错误监听器
class CSSErrorListener : public BaseErrorListener {
private:
    std::vector<std::string>& errors_;
    
public:
    CSSErrorListener(std::vector<std::string>& errors) : errors_(errors) {}
    
    void syntaxError(Recognizer *recognizer, Token *offendingSymbol,
                    size_t line, size_t charPositionInLine,
                    const std::string &msg, std::exception_ptr e) override {
        std::ostringstream ss;
        ss << "CSS Syntax error at line " << line << ":" << charPositionInLine 
           << " - " << msg;
        errors_.push_back(ss.str());
        
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
            .withMessage("CSS Syntax Error")
            .withDetail(msg)
            .withLocation(line, charPositionInLine)
            .report();
    }
};

// CSS编译器实现
CSSCompilerImpl::CSSCompilerImpl() {
    ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
        .withMessage("CSS Compiler initialized")
        .withDetail("Using ANTLR4 parser")
        .report();
}

CompileResult CSSCompilerImpl::compile(const std::string& code, const CompileOptions& options) {
    CompileResult result;
    
    ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
        .withMessage("CSS Compiler received complete CSS code") 
        .withDetail("Code length: " + std::to_string(code.length()) + " characters")
        .report();
    
    try {
        // 创建ANTLR输入流
        ANTLRInputStream input(code);
        
        // 创建词法分析器
        CSSSimpleLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        
        // 创建语法分析器
        CSSSimpleParser parser(&tokens);
        
        // 添加错误监听器
        parser.removeErrorListeners();
        CSSErrorListener errorListener(result.errors);
        parser.addErrorListener(&errorListener);
        
        // 解析
        auto tree = parser.stylesheet();
        
        // 使用访问者生成代码
        CSSVisitorImpl visitor;
        visitor.visit(tree);
        
        result.cssOutput = visitor.getOutput();
        result.success = result.errors.empty();
        
        if (result.success) {
            ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
                .withMessage("CSS compilation successful")
                .withDetail("Output size: " + std::to_string(result.cssOutput.length()) + " characters")
                .report();
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(e.what());
        
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::INTERNAL_ERROR)
            .withMessage("CSS compilation failed")
            .withDetail(e.what())
            .report();
    }
    
    return result;
}

bool CSSCompilerImpl::validate(const std::string& code) {
    try {
        ANTLRInputStream input(code);
        CSSSimpleLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        CSSSimpleParser parser(&tokens);
        
        // 静默模式
        parser.removeErrorListeners();
        parser.stylesheet();
        
        return true;
    } catch (...) {
        return false;
    }
}

std::string CSSCompilerImpl::getName() const { 
    return "CSS Compiler (ANTLR)"; 
}

std::string CSSCompilerImpl::getVersion() const { 
    return "1.0.0"; 
}

void CSSCompilerImpl::setSelectorOptimization(bool enable) {
    selectorOptimization_ = enable;
}

void CSSCompilerImpl::setMinification(bool enable) {
    minification_ = enable;
}