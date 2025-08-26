#pragma once
#include "Lexer/CHTLLexer.h"
#include "Parser/CHTLParser.h"
#include "Generator/CHTLGenerator.h"
#include "AST/CHTLNodes.h"
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

// CHTL编译器配置
struct CHTLCompilerConfig {
    // 词法分析配置
    bool preserveComments = true;           // 保留注释
    bool preserveWhitespace = false;        // 保留空白
    
    // 语法分析配置
    bool strictMode = true;                 // 严格模式
    bool allowImplicitElements = false;     // 允许隐式元素
    
    // 代码生成配置
    CHTLGeneratorConfig generatorConfig;    // 生成器配置
    
    // 调试和优化
    bool enableDebugging = false;           // 启用调试
    bool enableOptimization = true;         // 启用优化
    bool enableCaching = true;              // 启用缓存
    
    // 输出控制
    bool outputHTML = true;                 // 输出HTML
    bool outputCSS = true;                  // 输出CSS
    bool outputJS = false;                  // 输出JavaScript（CHTL不生成JS）
};

// CHTL编译结果
struct CHTLCompilerResult {
    // 编译产物
    std::string html;                       // 生成的HTML
    std::string css;                        // 生成的CSS
    std::string javascript;                 // 生成的JavaScript
    
    // AST
    std::shared_ptr<CHTLDocumentNode> ast;  // 抽象语法树
    
    // 编译信息
    std::vector<std::string> errors;       // 错误信息
    std::vector<std::string> warnings;     // 警告信息
    std::vector<CHTLToken> tokens;          // Token列表（调试用）
    
    // 元数据
    std::unordered_map<std::string, std::string> metadata; // 编译元数据
    double compilationTime = 0.0;           // 编译耗时（秒）
    size_t sourceSize = 0;                  // 源码大小
    size_t outputSize = 0;                  // 输出大小
    
    // 状态
    bool success = false;                   // 是否成功
    std::string phase;                      // 当前阶段或失败阶段
    
    // 统计信息
    size_t elementCount = 0;                // 元素数量
    size_t templateCount = 0;               // 模板数量
    size_t customCount = 0;                 // 自定义数量
    size_t styleRuleCount = 0;              // 样式规则数量
};

// CHTL编译器主类
class CHTLCompiler {
private:
    CHTLCompilerConfig config;
    std::unique_ptr<CHTLLexer> lexer;
    std::unique_ptr<CHTLParser> parser;
    std::unique_ptr<CHTLGenerator> generator;
    
    // 缓存系统
    std::unordered_map<std::string, CHTLCompilerResult> compilationCache;
    
    // 性能监控
    mutable std::chrono::high_resolution_clock::time_point startTime;
    
public:
    // 构造函数
    explicit CHTLCompiler(const CHTLCompilerConfig& cfg = CHTLCompilerConfig());
    
    // 析构函数
    ~CHTLCompiler() = default;
    
    // 禁用拷贝构造和赋值
    CHTLCompiler(const CHTLCompiler&) = delete;
    CHTLCompiler& operator=(const CHTLCompiler&) = delete;
    
    // 移动构造和赋值
    CHTLCompiler(CHTLCompiler&&) = default;
    CHTLCompiler& operator=(CHTLCompiler&&) = default;
    
    // === 主编译接口 ===
    
    // 编译CHTL源码
    CHTLCompilerResult compile(const std::string& source, const std::string& filename = "");
    
    // 编译CHTL文件
    CHTLCompilerResult compileFile(const std::string& filepath);
    
    // 批量编译
    std::vector<CHTLCompilerResult> compileFiles(const std::vector<std::string>& filepaths);
    
    // 增量编译（基于缓存）
    CHTLCompilerResult incrementalCompile(const std::string& source, const std::string& filename = "");
    
    // === 分阶段编译接口 ===
    
    // 词法分析
    std::vector<CHTLToken> tokenize(const std::string& source);
    
    // 语法分析
    std::shared_ptr<CHTLDocumentNode> parse(const std::vector<CHTLToken>& tokens);
    std::shared_ptr<CHTLDocumentNode> parse(const std::string& source);
    
    // 代码生成
    CHTLGenerationResult generate(std::shared_ptr<CHTLDocumentNode> ast);
    
    // === 配置管理 ===
    
    void setConfig(const CHTLCompilerConfig& cfg);
    const CHTLCompilerConfig& getConfig() const { return config; }
    
    void setGeneratorConfig(const CHTLGeneratorConfig& cfg);
    CHTLGeneratorConfig getGeneratorConfig() const;
    
    // === 缓存管理 ===
    
    void enableCache(bool enable = true);
    void clearCache();
    size_t getCacheSize() const { return compilationCache.size(); }
    
    // === 工具方法 ===
    
    // 验证CHTL语法
    bool validateSyntax(const std::string& source);
    
    // 格式化CHTL代码
    std::string formatCode(const std::string& source);
    
    // 获取语法高亮信息
    std::vector<std::pair<std::string, std::string>> getSyntaxHighlighting(const std::string& source);
    
    // AST分析
    std::string dumpAST(std::shared_ptr<CHTLDocumentNode> ast, bool detailed = false);
    std::vector<std::string> getASTStatistics(std::shared_ptr<CHTLDocumentNode> ast);
    
    // === 错误处理和调试 ===
    
    // 检查编译器状态
    bool isReady() const;
    std::string getVersion() const;
    std::vector<std::string> getSupportedFeatures() const;
    
    // 获取详细错误信息
    std::string getDetailedError(const CHTLCompilerResult& result) const;
    
    // 性能分析
    std::unordered_map<std::string, double> getPerformanceMetrics(const CHTLCompilerResult& result) const;
    
private:
    // === 内部方法 ===
    
    // 初始化组件
    void initializeComponents();
    
    // 预处理源码
    std::string preprocessSource(const std::string& source, const std::string& filename);
    
    // 后处理结果
    void postprocessResult(CHTLCompilerResult& result);
    
    // 缓存管理
    std::string generateCacheKey(const std::string& source, const std::string& filename);
    bool hasCachedResult(const std::string& key) const;
    CHTLCompilerResult getCachedResult(const std::string& key) const;
    void setCachedResult(const std::string& key, const CHTLCompilerResult& result);
    
    // 性能监控
    void startTiming() const;
    double stopTiming() const;
    
    // 统计收集
    void collectStatistics(CHTLCompilerResult& result, std::shared_ptr<CHTLDocumentNode> ast);
    
    // 错误处理
    CHTLCompilerResult createErrorResult(const std::string& phase, const std::vector<std::string>& errors);
    void propagateErrors(CHTLCompilerResult& result, const std::vector<std::string>& errors);
    
    // 文件I/O
    std::string readFile(const std::string& filepath);
    bool writeFile(const std::string& filepath, const std::string& content);
    
    // 路径处理
    std::string resolvePath(const std::string& path, const std::string& basePath = "");
    std::string getFileExtension(const std::string& filepath);
    std::string getBaseName(const std::string& filepath);
    std::string getDirectory(const std::string& filepath);
    
    // 验证方法
    bool validateConfig() const;
    bool validateSource(const std::string& source) const;
    
    // 清理方法
    void cleanup();
    
    // 常量
    static const std::string COMPILER_VERSION;
    static const std::vector<std::string> SUPPORTED_FEATURES;
    static const size_t MAX_CACHE_SIZE;
};

// === 便利函数 ===

// 快速编译CHTL源码
CHTLCompilerResult compileCHTL(const std::string& source, const CHTLCompilerConfig& config = CHTLCompilerConfig());

// 快速编译CHTL文件
CHTLCompilerResult compileCHTLFile(const std::string& filepath, const CHTLCompilerConfig& config = CHTLCompilerConfig());

// 验证CHTL语法
bool validateCHTL(const std::string& source);

// 格式化CHTL代码
std::string formatCHTL(const std::string& source);

// 创建默认编译器
std::unique_ptr<CHTLCompiler> createCHTLCompiler(const CHTLCompilerConfig& config = CHTLCompilerConfig());

} // namespace CHTL