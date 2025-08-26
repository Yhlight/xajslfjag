#pragma once

#include "../../Token/CHTL/CHTLToken.h"
#include "../../AST/CHTL/CHTLNodes.h"
#include "../../Common/StateMachine.h"
#include "../../Common/Context.h"
#include "../../Common/GlobalMap.h"
#include "../../Scanner/CHTLUnifiedScanner.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace Compiler {

// 编译结果
struct CHTLCompilationResult {
    std::string generated_html;         // 生成的HTML代码
    std::string generated_css;          // 生成的CSS代码
    std::string generated_js;           // 生成的JavaScript代码
    std::vector<std::string> errors;    // 错误信息
    std::vector<std::string> warnings;  // 警告信息
    bool success;                       // 编译是否成功
    
    CHTLCompilationResult() : success(false) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// CHTL解析器
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();
    
    // 解析CHTL代码片段
    std::shared_ptr<AST::ASTNode> parse(const std::vector<Scanner::CodeFragment>& fragments);
    
    // 解析单个片段
    std::shared_ptr<AST::ASTNode> parseFragment(const Scanner::CodeFragment& fragment);
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors; }
    std::vector<std::string> getWarnings() const { return warnings; }
    void clearErrors() { errors.clear(); warnings.clear(); }
    
    // 配置解析器
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setDebugMode(bool debug) { debug_mode = debug; }

private:
    // 核心组件
    std::unique_ptr<Token::CHTLLexer> lexer;
    std::unique_ptr<Common::StateMachine> state_machine;
    std::unique_ptr<Common::Context> context;
    Common::GlobalMap* global_map;
    
    // 解析状态
    std::vector<Token::CHTLToken> tokens;
    size_t current_token_index;
    bool strict_mode;
    bool debug_mode;
    
    // 错误信息
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 解析方法
    
    // 顶级解析
    std::shared_ptr<AST::RootNode> parseRoot();
    std::shared_ptr<AST::ASTNode> parseTopLevelElement();
    
    // HTML元素解析
    std::shared_ptr<AST::HTMLElementNode> parseHTMLElement();
    std::shared_ptr<AST::AttributeNode> parseAttribute();
    std::shared_ptr<AST::TextNode> parseTextNode();
    
    // 模板和自定义解析
    std::shared_ptr<AST::TemplateStyleNode> parseTemplateStyle();
    std::shared_ptr<AST::TemplateElementNode> parseTemplateElement();
    std::shared_ptr<AST::TemplateVarNode> parseTemplateVar();
    std::shared_ptr<AST::CustomStyleNode> parseCustomStyle();
    std::shared_ptr<AST::CustomElementNode> parseCustomElement();
    std::shared_ptr<AST::CustomVarNode> parseCustomVar();
    
    // 样式块解析
    std::shared_ptr<AST::StyleBlockNode> parseStyleBlock();
    std::shared_ptr<AST::StyleSelectorNode> parseStyleSelector();
    std::shared_ptr<AST::StylePropertyNode> parseStyleProperty();
    
    // 脚本块解析
    std::shared_ptr<AST::ScriptBlockNode> parseScriptBlock();
    
    // 原始嵌入解析
    std::shared_ptr<AST::OriginBlockNode> parseOriginBlock();
    
    // 配置和命名空间解析
    std::shared_ptr<AST::ConfigurationNode> parseConfiguration();
    std::shared_ptr<AST::NamespaceNode> parseNamespace();
    std::shared_ptr<AST::ImportNode> parseImport();
    
    // 特殊操作解析
    std::shared_ptr<AST::InheritNode> parseInherit();
    std::shared_ptr<AST::DeleteNode> parseDelete();
    std::shared_ptr<AST::InsertNode> parseInsert();
    std::shared_ptr<AST::ExceptNode> parseExcept();
    
    // 工具方法
    Token::CHTLToken& currentToken();
    Token::CHTLToken& peekToken(int offset = 1);
    bool isAtEnd() const;
    void advance();
    bool match(Token::TokenType type);
    bool match(const std::vector<Token::TokenType>& types);
    void consume(Token::TokenType type, const std::string& error_message);
    
    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    void synchronize(); // 错误恢复
    
    // 状态管理
    void enterState(Common::StateType state, const std::string& name);
    void exitState();
    bool canUseFeature(const std::string& feature) const;
    
    // 验证
    bool validateElementName(const std::string& name) const;
    bool validateAttributeName(const std::string& name) const;
    bool validateSelectorSyntax(const std::string& selector) const;
};

// CHTL生成器
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator();
    
    // 生成代码
    CHTLCompilationResult generate(std::shared_ptr<AST::ASTNode> ast_root);
    
    // 配置生成器
    void setMinifyOutput(bool minify) { minify_output = minify; }
    void setIndentationSize(int size) { indentation_size = size; }
    void setPrettyPrint(bool pretty) { pretty_print = pretty; }
    
    // 获取生成统计
    struct GenerationStats {
        size_t html_lines;
        size_t css_lines;
        size_t js_lines;
        size_t total_elements;
        size_t total_selectors;
    };
    GenerationStats getStats() const { return stats; }

private:
    // 核心组件
    Common::GlobalMap* global_map;
    std::unique_ptr<Common::Context> context;
    
    // 生成配置
    bool minify_output;
    int indentation_size;
    bool pretty_print;
    
    // 生成状态
    std::string html_output;
    std::string css_output;
    std::string js_output;
    int current_indent_level;
    GenerationStats stats;
    
    // 错误信息
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // HTML生成
    void generateHTML(std::shared_ptr<AST::ASTNode> node);
    void generateHTMLElement(std::shared_ptr<AST::HTMLElementNode> element);
    void generateTextNode(std::shared_ptr<AST::TextNode> text);
    void generateAttributes(const std::vector<std::shared_ptr<AST::AttributeNode>>& attributes);
    
    // CSS生成
    void generateCSS(std::shared_ptr<AST::ASTNode> node);
    void generateStyleBlock(std::shared_ptr<AST::StyleBlockNode> style_block);
    void generateStyleSelector(std::shared_ptr<AST::StyleSelectorNode> selector);
    void generateStyleProperties(const std::vector<std::shared_ptr<AST::StylePropertyNode>>& properties);
    
    // JavaScript生成
    void generateJS(std::shared_ptr<AST::ASTNode> node);
    void generateScriptBlock(std::shared_ptr<AST::ScriptBlockNode> script_block);
    
    // 模板和自定义处理
    void processTemplateStyle(std::shared_ptr<AST::TemplateStyleNode> template_style);
    void processCustomStyle(std::shared_ptr<AST::CustomStyleNode> custom_style);
    void processTemplateElement(std::shared_ptr<AST::TemplateElementNode> template_element);
    void processCustomElement(std::shared_ptr<AST::CustomElementNode> custom_element);
    
    // 特殊功能处理
    void processInheritance(std::shared_ptr<AST::InheritNode> inherit);
    void processDelete(std::shared_ptr<AST::DeleteNode> delete_node);
    void processInsert(std::shared_ptr<AST::InsertNode> insert);
    
    // 原始嵌入处理
    void processOriginBlock(std::shared_ptr<AST::OriginBlockNode> origin);
    
    // 选择器自动化
    void applyAutomaticSelectors();
    std::string generateAutomaticClassName(const std::string& element_name);
    std::string generateAutomaticIdName(const std::string& element_name);
    
    // 工具方法
    void appendHTML(const std::string& content);
    void appendCSS(const std::string& content);
    void appendJS(const std::string& content);
    std::string getIndentation() const;
    void incrementIndent() { current_indent_level++; }
    void decrementIndent() { if (current_indent_level > 0) current_indent_level--; }
    
    // 代码优化
    void optimizeHTML();
    void optimizeCSS();
    void optimizeJS();
    std::string minifyCSS(const std::string& css);
    std::string minifyJS(const std::string& js);
    
    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    
    // 验证
    bool validateGeneratedHTML(const std::string& html);
    bool validateGeneratedCSS(const std::string& css);
    bool validateGeneratedJS(const std::string& js);
};

// CHTL编译器主类
class CHTLCompiler {
public:
    CHTLCompiler();
    ~CHTLCompiler();
    
    // 编译CHTL代码
    CHTLCompilationResult compile(const std::vector<Scanner::CodeFragment>& fragments);
    
    // 编译单个文件
    CHTLCompilationResult compileFile(const std::string& file_path);
    
    // 编译字符串
    CHTLCompilationResult compileString(const std::string& chtl_code, const std::string& file_path = "");
    
    // 配置编译器
    void setStrictMode(bool strict);
    void setDebugMode(bool debug);
    void setOptimizationLevel(int level);
    void setTargetFormat(const std::string& format); // HTML5, XHTML等
    
    // 获取编译器信息
    std::string getVersion() const { return "1.0.0"; }
    std::vector<std::string> getSupportedFeatures() const;
    
    // 性能统计
    struct CompilerStats {
        size_t compilation_time_ms;
        size_t input_size_bytes;
        size_t output_size_bytes;
        size_t tokens_processed;
        size_t ast_nodes_created;
    };
    CompilerStats getCompilerStats() const { return compiler_stats; }

private:
    // 核心组件
    std::unique_ptr<CHTLParser> parser;
    std::unique_ptr<CHTLGenerator> generator;
    std::unique_ptr<Scanner::CHTLUnifiedScanner> scanner;
    Common::GlobalMap* global_map;
    
    // 编译配置
    bool strict_mode;
    bool debug_mode;
    int optimization_level;
    std::string target_format;
    
    // 性能统计
    CompilerStats compiler_stats;
    
    // 预处理
    std::vector<Scanner::CodeFragment> preprocessFragments(const std::vector<Scanner::CodeFragment>& fragments);
    
    // 后处理
    CHTLCompilationResult postprocessResult(const CHTLCompilationResult& result);
    
    // 验证
    bool validateInput(const std::vector<Scanner::CodeFragment>& fragments);
    bool validateConfiguration();
    
    // 错误处理
    void handleCompilationError(const std::string& error, const std::string& stage);
    
    // 性能监控
    void startTimer();
    void recordTiming(const std::string& stage);
    void updateStats(const std::vector<Scanner::CodeFragment>& fragments, 
                    const CHTLCompilationResult& result);
};

// CHTL编译器工具类
class CHTLCompilerUtils {
public:
    // 创建预配置的编译器
    static std::unique_ptr<CHTLCompiler> createStandardCompiler();
    static std::unique_ptr<CHTLCompiler> createMinimalCompiler();
    static std::unique_ptr<CHTLCompiler> createOptimizedCompiler();
    
    // 代码分析
    static bool isValidCHTLCode(const std::string& code);
    static std::vector<std::string> analyzeCHTLSyntax(const std::string& code);
    
    // 代码转换
    static std::string convertHTMLToCHTL(const std::string& html);
    static std::string formatCHTLCode(const std::string& code);
    
    // 编译器测试
    static bool runCompilerTests();
    static std::vector<std::string> validateCompilerOutput(const CHTLCompilationResult& result);
};

} // namespace Compiler
} // namespace CHTL