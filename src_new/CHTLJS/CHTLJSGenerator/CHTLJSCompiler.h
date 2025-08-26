#pragma once

#include "../../Token/CHTLJS/CHTLJSToken.h"
#include "../../AST/CHTLJS/CHTLJSNodes.h"
#include "../../Common/StateMachine.h"
#include "../../Common/Context.h"
#include "../../Common/GlobalMap.h"
#include "../../Common/CHTLJSKeyValueManager.h"
#include "../../Scanner/CHTLUnifiedScanner.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace Compiler {

// CHTL JS编译结果
struct CHTLJSCompilationResult {
    std::string generated_js;          // 生成的JavaScript代码
    std::string generated_css;         // 相关的CSS代码（如动画）
    std::vector<std::string> errors;   // 错误信息
    std::vector<std::string> warnings; // 警告信息
    bool success;                      // 编译是否成功
    
    // VIR对象注册表
    std::unordered_map<std::string, std::string> vir_registry;
    
    // 全局函数注册表
    std::unordered_map<std::string, std::string> global_functions;
    
    // 事件委托注册表
    std::unordered_map<std::string, std::vector<std::string>> event_delegates;
    
    CHTLJSCompilationResult() : success(false) {}
    
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

// CHTL JS词法分析器
class CHTLJSLexer {
public:
    CHTLJSLexer();
    ~CHTLJSLexer();
    
    // 词法分析
    std::vector<Token::CHTLJSToken> tokenize(const std::string& source);
    
    // 配置
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setDebugMode(bool debug) { debug_mode = debug; }
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }

private:
    bool strict_mode;
    bool debug_mode;
    std::vector<std::string> errors;
    
    // 内部方法
    Token::CHTLJSToken scanToken(const std::string& source, size_t& position);
    Token::CHTLJSTokenType recognizeKeyword(const std::string& identifier);
    bool isEventBindOperator(const std::string& source, size_t position);
    std::string scanEnhancedSelector(const std::string& source, size_t& position);
    void skipWhitespace(const std::string& source, size_t& position);
    void reportError(const std::string& message, size_t position);
};

// CHTL JS解析器
class CHTLJSParser {
public:
    CHTLJSParser();
    ~CHTLJSParser();
    
    // 解析CHTL JS代码片段
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
    std::unique_ptr<CHTLJSLexer> lexer;
    std::unique_ptr<Common::StateMachine> state_machine;
    std::unique_ptr<Common::Context> context;
    std::unique_ptr<Common::CHTLJSKeyValueManager> kv_manager;
    Common::GlobalMap* global_map;
    
    // 解析状态
    std::vector<Token::CHTLJSToken> tokens;
    size_t current_token_index;
    bool strict_mode;
    bool debug_mode;
    
    // 错误信息
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 解析方法
    
    // 顶级解析
    std::shared_ptr<AST::ASTNode> parseStatement();
    std::shared_ptr<AST::ASTNode> parseExpression();
    
    // 增强选择器解析
    std::shared_ptr<AST::EnhancedSelectorNode> parseEnhancedSelector();
    
    // CHTL JS函数解析
    std::shared_ptr<AST::ListenCallNode> parseListenCall();
    std::shared_ptr<AST::DelegateCallNode> parseDelegateCall();
    std::shared_ptr<AST::AnimateCallNode> parseAnimateCall();
    
    // 扩展功能解析
    std::shared_ptr<AST::EventBindExpressionNode> parseEventBindExpression();
    std::shared_ptr<AST::VirDeclarationNode> parseVirDeclaration();
    std::shared_ptr<AST::AnimationKeyframeNode> parseAnimationKeyframe();
    
    // 虚对象解析
    std::shared_ptr<AST::VirObjectNode> parseVirObject();
    std::shared_ptr<AST::ASTNode> parseVirAccess();
    
    // 箭头操作符解析
    std::shared_ptr<AST::ArrowOperatorNode> parseArrowOperator();
    
    // 动画配置解析
    std::shared_ptr<AST::AnimationConfigNode> parseAnimationConfig();
    std::vector<std::shared_ptr<AST::AnimationKeyframeNode>> parseWhenArray();
    
    // 事件对象解析
    std::shared_ptr<AST::EventObjectNode> parseEventObject();
    std::shared_ptr<AST::KeyValuePairNode> parseKeyValuePair();
    
    // 特殊函数解析
    std::shared_ptr<AST::INeverAwayCallNode> parseINeverAwayCall();
    std::shared_ptr<AST::PrintMyLoveCallNode> parsePrintMyLoveCall();
    
    // 工具方法
    Token::CHTLJSToken& currentToken();
    Token::CHTLJSToken& peekToken(int offset = 1);
    bool isAtEnd() const;
    void advance();
    bool match(Token::CHTLJSTokenType type);
    bool match(const std::vector<Token::CHTLJSTokenType>& types);
    void consume(Token::CHTLJSTokenType type, const std::string& error_message);
    
    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    void synchronize(); // 错误恢复
    
    // 状态管理
    void enterState(Common::StateType state, const std::string& name);
    void exitState();
    bool canUseFeature(const std::string& feature) const;
    
    // 验证
    bool validateEnhancedSelector(const std::string& selector) const;
    bool validateEventType(const std::string& event) const;
    bool validateEasingFunction(const std::string& easing) const;
    bool validateAnimationProperty(const std::string& property) const;
};

// CHTL JS生成器
class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    ~CHTLJSGenerator();
    
    // 生成代码
    CHTLJSCompilationResult generate(std::shared_ptr<AST::ASTNode> ast_root);
    
    // 配置生成器
    void setMinifyOutput(bool minify) { minify_output = minify; }
    void setES6Compatible(bool es6) { es6_compatible = es6; }
    void setIndentationSize(int size) { indentation_size = size; }
    
    // 获取生成统计
    struct GenerationStats {
        size_t js_lines;
        size_t css_lines;
        size_t total_selectors;
        size_t total_listeners;
        size_t total_animations;
        size_t vir_objects;
    };
    GenerationStats getStats() const { return stats; }

private:
    // 核心组件
    Common::GlobalMap* global_map;
    std::unique_ptr<Common::Context> context;
    
    // 生成配置
    bool minify_output;
    bool es6_compatible;
    int indentation_size;
    
    // 生成状态
    std::string js_output;
    std::string css_output;
    int current_indent_level;
    GenerationStats stats;
    
    // 注册表
    std::unordered_map<std::string, std::string> vir_registry;
    std::unordered_map<std::string, std::string> global_functions;
    std::unordered_map<std::string, std::vector<std::string>> event_delegates;
    std::vector<std::string> animation_keyframes;
    
    // 错误信息
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // JavaScript生成
    void generateJS(std::shared_ptr<AST::ASTNode> node);
    void generateEnhancedSelector(std::shared_ptr<AST::EnhancedSelectorNode> selector);
    void generateListenCall(std::shared_ptr<AST::ListenCallNode> listen);
    void generateDelegateCall(std::shared_ptr<AST::DelegateCallNode> delegate);
    void generateAnimateCall(std::shared_ptr<AST::AnimateCallNode> animate);
    void generateVirDeclaration(std::shared_ptr<AST::VirDeclarationNode> vir);
    void generateEventBindExpression(std::shared_ptr<AST::EventBindExpressionNode> event_bind);
    void generateArrowOperator(std::shared_ptr<AST::ArrowOperatorNode> arrow);
    
    // CSS生成（动画相关）
    void generateAnimationCSS(std::shared_ptr<AST::AnimateCallNode> animate);
    void generateKeyframeCSS(const std::vector<std::shared_ptr<AST::AnimationKeyframeNode>>& keyframes, 
                           const std::string& animation_name);
    
    // 增强选择器处理
    std::string generateSelectorCode(std::shared_ptr<AST::EnhancedSelectorNode> selector);
    std::string optimizeSelector(const std::string& selector);
    
    // 事件处理
    std::string generateEventListener(const std::string& selector, const std::string& event, 
                                    const std::string& handler);
    std::string generateEventDelegate(const std::string& parent_selector, 
                                    const std::string& child_selector,
                                    const std::string& event, const std::string& handler);
    
    // 动画处理
    std::string generateRequestAnimationFrame(const std::string& animation_code);
    std::string generateEasingFunction(const std::string& easing);
    std::string generateCSSKeyframes(const std::string& name, 
                                   const std::vector<std::shared_ptr<AST::AnimationKeyframeNode>>& keyframes);
    
    // vir对象处理
    void processVirObject(std::shared_ptr<AST::VirDeclarationNode> vir);
    std::string generateVirAccessor(const std::string& vir_name, const std::string& key);
    void registerVirObject(const std::string& name, const std::unordered_map<std::string, std::string>& keys);
    
    // 特殊函数处理
    void generateINeverAwayCall(std::shared_ptr<AST::INeverAwayCallNode> call);
    void generatePrintMyLoveCall(std::shared_ptr<AST::PrintMyLoveCallNode> call);
    
    // 代码优化
    std::string optimizeJavaScript(const std::string& js);
    std::string minifyJavaScript(const std::string& js);
    
    // 工具方法
    void appendJS(const std::string& content);
    void appendCSS(const std::string& content);
    std::string getIndentation() const;
    void incrementIndent() { current_indent_level++; }
    void decrementIndent() { if (current_indent_level > 0) current_indent_level--; }
    
    // 唯一名称生成
    std::string generateUniqueAnimationName();
    std::string generateUniqueFunctionName();
    std::string generateUniqueVirName();
    
    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    
    // 验证
    bool validateGeneratedJS(const std::string& js);
    bool validateGeneratedCSS(const std::string& css);
};

// CHTL JS编译器主类
class CHTLJSCompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler();
    
    // 编译CHTL JS代码
    CHTLJSCompilationResult compile(const std::vector<Scanner::CodeFragment>& fragments);
    
    // 编译单个文件
    CHTLJSCompilationResult compileFile(const std::string& file_path);
    
    // 编译字符串
    CHTLJSCompilationResult compileString(const std::string& chtljs_code, const std::string& file_path = "");
    
    // 配置编译器
    void setStrictMode(bool strict);
    void setDebugMode(bool debug);
    void setES6Compatible(bool es6);
    void setOptimizationLevel(int level);
    
    // 获取编译器信息
    std::string getVersion() const { return "1.0.0"; }
    std::vector<std::string> getSupportedFeatures() const;
    
    // VIR对象管理
    void registerVirObject(const std::string& name, const std::unordered_map<std::string, std::string>& keys);
    std::unordered_map<std::string, std::string> getVirRegistry() const;
    
    // 事件委托管理
    void registerEventDelegate(const std::string& parent, const std::vector<std::string>& children);
    std::unordered_map<std::string, std::vector<std::string>> getEventDelegates() const;
    
    // 性能统计
    struct CompilerStats {
        size_t compilation_time_ms;
        size_t input_size_bytes;
        size_t output_size_bytes;
        size_t tokens_processed;
        size_t ast_nodes_created;
        size_t enhanced_selectors;
        size_t vir_objects;
        size_t animations_created;
    };
    CompilerStats getCompilerStats() const { return compiler_stats; }

private:
    // 核心组件
    std::unique_ptr<CHTLJSParser> parser;
    std::unique_ptr<CHTLJSGenerator> generator;
    std::unique_ptr<Scanner::CHTLUnifiedScanner> scanner;
    Common::GlobalMap* global_map;
    
    // 编译配置
    bool strict_mode;
    bool debug_mode;
    bool es6_compatible;
    int optimization_level;
    
    // 性能统计
    CompilerStats compiler_stats;
    
    // 预处理
    std::vector<Scanner::CodeFragment> preprocessFragments(const std::vector<Scanner::CodeFragment>& fragments);
    
    // 后处理
    CHTLJSCompilationResult postprocessResult(const CHTLJSCompilationResult& result);
    
    // 验证
    bool validateInput(const std::vector<Scanner::CodeFragment>& fragments);
    bool validateCHTLJSSyntax(const std::string& code);
    
    // 错误处理
    void handleCompilationError(const std::string& error, const std::string& stage);
    
    // 性能监控
    void startTimer();
    void recordTiming(const std::string& stage);
    void updateStats(const std::vector<Scanner::CodeFragment>& fragments, 
                    const CHTLJSCompilationResult& result);
};

// CHTL JS编译器工具类
class CHTLJSCompilerUtils {
public:
    // 创建预配置的编译器
    static std::unique_ptr<CHTLJSCompiler> createStandardCompiler();
    static std::unique_ptr<CHTLJSCompiler> createES6Compiler();
    static std::unique_ptr<CHTLJSCompiler> createOptimizedCompiler();
    
    // 代码分析
    static bool isValidCHTLJSCode(const std::string& code);
    static std::vector<std::string> analyzeCHTLJSSyntax(const std::string& code);
    static std::vector<std::string> extractEnhancedSelectors(const std::string& code);
    
    // 代码转换
    static std::string convertJSToCHTLJS(const std::string& js);
    static std::string formatCHTLJSCode(const std::string& code);
    
    // 语法检查
    static std::vector<std::string> validateEnhancedSelectors(const std::string& code);
    static std::vector<std::string> validateEventBindings(const std::string& code);
    static std::vector<std::string> validateAnimations(const std::string& code);
    
    // 编译器测试
    static bool runCompilerTests();
    static std::vector<std::string> validateCompilerOutput(const CHTLJSCompilationResult& result);
};

} // namespace Compiler
} // namespace CHTL