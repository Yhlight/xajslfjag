#pragma once
#include "Lexer/CHTLJSLexer.h"
#include "Parser/CHTLJSParser.h"
#include "Generator/CHTLJSGenerator.h"
#include "AST/CHTLJSNodes.h"
#include <string>
#include <memory>
#include <vector>
#include <chrono>

namespace CHTLJS {

// CHTL JS编译器配置
struct CHTLJSCompilerConfig {
    // 词法分析配置
    bool preserveComments = true;           // 保留注释
    bool preserveWhitespace = false;        // 保留空白
    bool enableExtendedSyntax = true;       // 启用扩展语法
    
    // 语法分析配置
    bool strictMode = true;                 // 严格模式
    bool allowAsyncAwait = true;            // 允许async/await
    bool enableVirtualObjects = true;       // 启用虚拟对象
    bool enableAnimations = true;           // 启用动画
    
    // 代码生成配置
    std::string targetES = "ES2020";        // 目标ES版本
    bool generateSourceMap = false;         // 生成源映射
    bool minifyOutput = false;              // 压缩输出
    bool enableOptimization = true;         // 启用优化
    std::string indentString = "  ";        // 缩进字符串
    
    // CHTL JS特性
    bool enableSelectorExtensions = true;   // 启用选择器扩展
    bool enableListenerEnhancements = true; // 启用监听器增强
    bool enableEventDelegation = true;      // 启用事件委托
    bool enableAnimationAPI = true;         // 启用动画API
    bool enableVirtualObjectAPI = true;     // 启用虚拟对象API
    
    // 调试和优化
    bool enableDebugging = false;           // 启用调试
    bool enableCaching = true;              // 启用缓存
    bool validateOutput = true;             // 验证输出
    
    // 模块系统
    std::string moduleFormat = "ESM";       // 模块格式：ESM, CommonJS, UMD
    bool enableTreeShaking = true;          // 启用Tree Shaking
};

// CHTL JS编译结果
struct CHTLJSCompilerResult {
    // 编译产物
    std::string javascript;                 // 生成的JavaScript
    std::string sourceMap;                  // 源映射
    std::string typescript;                 // TypeScript定义（可选）
    
    // AST
    std::shared_ptr<CHTLJSDocumentNode> ast; // 抽象语法树
    
    // 编译信息
    std::vector<std::string> errors;       // 错误信息
    std::vector<std::string> warnings;     // 警告信息
    std::vector<CHTLJSToken> tokens;        // Token列表（调试用）
    
    // 元数据
    std::unordered_map<std::string, std::string> metadata; // 编译元数据
    double compilationTime = 0.0;           // 编译耗时（秒）
    size_t sourceSize = 0;                  // 源码大小
    size_t outputSize = 0;                  // 输出大小
    
    // 状态
    bool success = false;                   // 是否成功
    std::string phase;                      // 当前阶段或失败阶段
    
    // CHTL JS特有统计
    size_t selectorCount = 0;               // 选择器数量
    size_t listenerCount = 0;               // 监听器数量
    size_t animationCount = 0;              // 动画数量
    size_t virtualObjectCount = 0;          // 虚拟对象数量
    size_t functionCount = 0;               // 函数数量
    size_t classCount = 0;                  // 类数量
};

// CHTL JS编译器主类
class CHTLJSCompiler {
private:
    CHTLJSCompilerConfig config;
    std::unique_ptr<CHTLJSLexer> lexer;
    std::unique_ptr<CHTLJSParser> parser;
    std::unique_ptr<CHTLJSGenerator> generator;
    
    // 缓存系统
    std::unordered_map<std::string, CHTLJSCompilerResult> compilationCache;
    
    // 性能监控
    mutable std::chrono::high_resolution_clock::time_point startTime;
    
    // CHTL JS运行时API
    std::string runtimeAPI;
    
public:
    // 构造函数
    explicit CHTLJSCompiler(const CHTLJSCompilerConfig& cfg = CHTLJSCompilerConfig());
    
    // 析构函数
    ~CHTLJSCompiler() = default;
    
    // 禁用拷贝构造和赋值
    CHTLJSCompiler(const CHTLJSCompiler&) = delete;
    CHTLJSCompiler& operator=(const CHTLJSCompiler&) = delete;
    
    // 移动构造和赋值
    CHTLJSCompiler(CHTLJSCompiler&&) = default;
    CHTLJSCompiler& operator=(CHTLJSCompiler&&) = default;
    
    // === 主编译接口 ===
    
    // 编译CHTL JS源码
    CHTLJSCompilerResult compile(const std::string& source, const std::string& filename = "");
    
    // 编译CHTL JS文件
    CHTLJSCompilerResult compileFile(const std::string& filepath);
    
    // 批量编译
    std::vector<CHTLJSCompilerResult> compileFiles(const std::vector<std::string>& filepaths);
    
    // 增量编译（基于缓存）
    CHTLJSCompilerResult incrementalCompile(const std::string& source, const std::string& filename = "");
    
    // === 分阶段编译接口 ===
    
    // 词法分析
    std::vector<CHTLJSToken> tokenize(const std::string& source);
    
    // 语法分析
    std::shared_ptr<CHTLJSDocumentNode> parse(const std::vector<CHTLJSToken>& tokens);
    std::shared_ptr<CHTLJSDocumentNode> parse(const std::string& source);
    
    // 代码生成
    CHTLJSGenerationResult generate(std::shared_ptr<CHTLJSDocumentNode> ast);
    
    // === CHTL JS特有功能 ===
    
    // 编译选择器表达式
    std::string compileSelector(const std::string& selector);
    
    // 编译监听器定义
    std::string compileListener(const std::string& listener);
    
    // 编译动画定义
    std::string compileAnimation(const std::string& animation);
    
    // 编译虚拟对象
    std::string compileVirtualObject(const std::string& virtualObject);
    
    // 生成CHTL JS运行时
    std::string generateRuntime();
    
    // === 配置管理 ===
    
    void setConfig(const CHTLJSCompilerConfig& cfg);
    const CHTLJSCompilerConfig& getConfig() const { return config; }
    
    void setTargetES(const std::string& version);
    void setModuleFormat(const std::string& format);
    
    // === 缓存管理 ===
    
    void enableCache(bool enable = true);
    void clearCache();
    size_t getCacheSize() const { return compilationCache.size(); }
    
    // === 工具方法 ===
    
    // 验证CHTL JS语法
    bool validateSyntax(const std::string& source);
    
    // 格式化CHTL JS代码
    std::string formatCode(const std::string& source);
    
    // 获取语法高亮信息
    std::vector<std::pair<std::string, std::string>> getSyntaxHighlighting(const std::string& source);
    
    // AST分析
    std::string dumpAST(std::shared_ptr<CHTLJSDocumentNode> ast, bool detailed = false);
    std::vector<std::string> getASTStatistics(std::shared_ptr<CHTLJSDocumentNode> ast);
    
    // CHTL JS特性分析
    std::vector<std::string> getUsedFeatures(const std::string& source);
    std::vector<std::string> getUsedAPIs(const std::string& source);
    
    // 转换和优化
    std::string transpileToVanillaJS(const std::string& source);
    std::string optimizeCode(const std::string& source);
    std::string minifyCode(const std::string& source);
    
    // === 错误处理和调试 ===
    
    // 检查编译器状态
    bool isReady() const;
    std::string getVersion() const;
    std::vector<std::string> getSupportedFeatures() const;
    
    // 获取详细错误信息
    std::string getDetailedError(const CHTLJSCompilerResult& result) const;
    
    // 性能分析
    std::unordered_map<std::string, double> getPerformanceMetrics(const CHTLJSCompilerResult& result) const;
    
    // === 运行时支持 ===
    
    // 生成运行时库
    std::string generateRuntimeLibrary();
    
    // 检查运行时依赖
    std::vector<std::string> getRuntimeDependencies(const std::string& source);
    
    // 注入运行时代码
    std::string injectRuntime(const std::string& compiledCode);
    
private:
    // === 内部方法 ===
    
    // 初始化组件
    void initializeComponents();
    
    // 初始化运行时API
    void initializeRuntimeAPI();
    
    // 预处理源码
    std::string preprocessSource(const std::string& source, const std::string& filename);
    
    // 后处理结果
    void postprocessResult(CHTLJSCompilerResult& result);
    
    // 缓存管理
    std::string generateCacheKey(const std::string& source, const std::string& filename);
    bool hasCachedResult(const std::string& key) const;
    CHTLJSCompilerResult getCachedResult(const std::string& key) const;
    void setCachedResult(const std::string& key, const CHTLJSCompilerResult& result);
    
    // 性能监控
    void startTiming() const;
    double stopTiming() const;
    
    // 统计收集
    void collectStatistics(CHTLJSCompilerResult& result, std::shared_ptr<CHTLJSDocumentNode> ast);
    
    // 错误处理
    CHTLJSCompilerResult createErrorResult(const std::string& phase, const std::vector<std::string>& errors);
    void propagateErrors(CHTLJSCompilerResult& result, const std::vector<std::string>& errors);
    
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
    bool validateTargetES(const std::string& version) const;
    
    // 清理方法
    void cleanup();
    
    // 常量
    static const std::string COMPILER_VERSION;
    static const std::vector<std::string> SUPPORTED_FEATURES;
    static const std::vector<std::string> SUPPORTED_ES_VERSIONS;
    static const std::vector<std::string> SUPPORTED_MODULE_FORMATS;
    static const size_t MAX_CACHE_SIZE;
};

// === 便利函数 ===

// 快速编译CHTL JS源码
CHTLJSCompilerResult compileCHTLJS(const std::string& source, const CHTLJSCompilerConfig& config = CHTLJSCompilerConfig());

// 快速编译CHTL JS文件
CHTLJSCompilerResult compileCHTLJSFile(const std::string& filepath, const CHTLJSCompilerConfig& config = CHTLJSCompilerConfig());

// 验证CHTL JS语法
bool validateCHTLJS(const std::string& source);

// 格式化CHTL JS代码
std::string formatCHTLJS(const std::string& source);

// 转换为原生JavaScript
std::string transpileCHTLJS(const std::string& source, const std::string& targetES = "ES2020");

// 创建默认编译器
std::unique_ptr<CHTLJSCompiler> createCHTLJSCompiler(const CHTLJSCompilerConfig& config = CHTLJSCompilerConfig());

// === CHTL JS特有API ===

// 创建选择器编译器
std::string compileCHTLJSSelector(const std::string& selector);

// 创建监听器编译器
std::string compileCHTLJSListener(const std::string& listener);

// 创建动画编译器
std::string compileCHTLJSAnimation(const std::string& animation);

// 创建虚拟对象编译器
std::string compileCHTLJSVirtualObject(const std::string& virtualObject);

} // namespace CHTLJS