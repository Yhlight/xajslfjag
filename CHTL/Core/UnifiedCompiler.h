#pragma once
#include "../CHTLCompiler/CHTLCompiler.h"
#include "../CHTLJSCompiler/CHTLJSCompiler.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace CHTL::Core {

// 统一编译器配置
struct UnifiedCompilerConfig {
    // CHTL配置
    CHTL::CHTLCompilerConfig chtlConfig;
    
    // CHTL JS配置  
    CHTLJS::CHTLJSCompilerConfig chtljsConfig;
    
    // 集成配置
    bool enableCHTL = true;                    // 启用CHTL编译
    bool enableCHTLJS = true;                  // 启用CHTL JS编译
    bool enableIntegration = true;             // 启用集成功能
    
    // 输出配置
    bool generateHTML = true;                  // 生成HTML
    bool generateCSS = true;                   // 生成CSS
    bool generateJS = false;                   // 生成JavaScript（仅CHTL JS）
    bool generateBundle = false;               // 生成打包文件
    
    // 模块化配置
    bool enableModularOutput = true;           // 启用模块化输出
    bool enableCrossReference = true;          // 启用交叉引用
    bool enableSharedResources = true;         // 启用共享资源
    
    // 优化配置
    bool enableOptimization = true;            // 启用优化
    bool enableMinification = false;           // 启用压缩
    bool enableTreeShaking = true;             // 启用Tree Shaking
    bool enableCodeSplitting = false;          // 启用代码分割
    
    // 调试配置
    bool enableDebugging = false;              // 启用调试
    bool generateSourceMaps = false;           // 生成源映射
    bool preserveComments = true;              // 保留注释
};

// 统一编译结果
struct UnifiedCompilerResult {
    // CHTL编译结果
    CHTL::CHTLCompilerResult chtlResult;
    
    // CHTL JS编译结果
    CHTLJS::CHTLJSCompilerResult chtljsResult;
    
    // 集成产物
    std::string html;                          // 最终HTML
    std::string css;                           // 最终CSS
    std::string javascript;                    // 最终JavaScript
    std::string bundle;                        // 打包文件
    
    // 模块化产物
    std::unordered_map<std::string, std::string> modules; // 模块文件
    std::vector<std::string> dependencies;    // 依赖列表
    std::string manifest;                      // 清单文件
    
    // 元数据
    std::unordered_map<std::string, std::string> metadata;
    double totalCompilationTime = 0.0;
    size_t totalSourceSize = 0;
    size_t totalOutputSize = 0;
    
    // 状态
    bool success = false;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string phase;
    
    // 统计信息
    size_t chtlFileCount = 0;
    size_t chtljsFileCount = 0;
    size_t totalElementCount = 0;
    size_t totalTemplateCount = 0;
    size_t totalFunctionCount = 0;
};

// 文件类型枚举
enum class FileType {
    CHTL,           // .chtl文件
    CHTLJS,         // .chtljs文件
    UNKNOWN         // 未知类型
};

// 项目结构
struct ProjectStructure {
    std::string rootPath;                                    // 项目根路径
    std::vector<std::string> chtlFiles;                     // CHTL文件列表
    std::vector<std::string> chtljsFiles;                   // CHTL JS文件列表
    std::unordered_map<std::string, std::string> dependencies; // 依赖关系
    std::string configFile;                                 // 配置文件路径
    std::string outputDir;                                  // 输出目录
};

// 统一编译器主类
class UnifiedCompiler {
private:
    UnifiedCompilerConfig config;
    std::unique_ptr<CHTL::CHTLCompiler> chtlCompiler;
    std::unique_ptr<CHTLJS::CHTLJSCompiler> chtljsCompiler;
    
    // 项目管理
    ProjectStructure project;
    
    // 缓存系统
    std::unordered_map<std::string, UnifiedCompilerResult> resultCache;
    
    // 依赖管理
    std::unordered_map<std::string, std::vector<std::string>> dependencyGraph;
    
public:
    // 构造函数
    explicit UnifiedCompiler(const UnifiedCompilerConfig& cfg = UnifiedCompilerConfig());
    
    // 析构函数
    ~UnifiedCompiler() = default;
    
    // 禁用拷贝
    UnifiedCompiler(const UnifiedCompiler&) = delete;
    UnifiedCompiler& operator=(const UnifiedCompiler&) = delete;
    
    // 移动语义
    UnifiedCompiler(UnifiedCompiler&&) = default;
    UnifiedCompiler& operator=(UnifiedCompiler&&) = default;
    
    // === 主编译接口 ===
    
    // 编译单个文件
    UnifiedCompilerResult compileFile(const std::string& filepath);
    
    // 编译项目
    UnifiedCompilerResult compileProject(const std::string& projectPath);
    
    // 批量编译
    UnifiedCompilerResult compileFiles(const std::vector<std::string>& filepaths);
    
    // 增量编译
    UnifiedCompilerResult incrementalCompile(const std::string& projectPath);
    
    // 监听模式编译
    void startWatchMode(const std::string& projectPath, 
                       std::function<void(const UnifiedCompilerResult&)> callback);
    void stopWatchMode();
    
    // === 项目管理 ===
    
    // 初始化项目
    bool initializeProject(const std::string& projectPath);
    
    // 加载项目配置
    bool loadProjectConfig(const std::string& configPath);
    
    // 保存项目配置
    bool saveProjectConfig(const std::string& configPath);
    
    // 分析项目结构
    ProjectStructure analyzeProject(const std::string& projectPath);
    
    // 构建依赖图
    void buildDependencyGraph();
    
    // === 模块化支持 ===
    
    // 解析模块依赖
    std::vector<std::string> resolveModuleDependencies(const std::string& filepath);
    
    // 生成模块清单
    std::string generateModuleManifest();
    
    // 打包模块
    std::string bundleModules(const std::vector<std::string>& moduleFiles);
    
    // 代码分割
    std::unordered_map<std::string, std::string> splitCode(const std::string& bundledCode);
    
    // === 配置管理 ===
    
    void setConfig(const UnifiedCompilerConfig& cfg);
    const UnifiedCompilerConfig& getConfig() const { return config; }
    
    void setCHTLConfig(const CHTL::CHTLCompilerConfig& cfg);
    void setCHTLJSConfig(const CHTLJS::CHTLJSCompilerConfig& cfg);
    
    // === 工具方法 ===
    
    // 文件类型检测
    FileType detectFileType(const std::string& filepath);
    
    // 验证项目
    bool validateProject(const std::string& projectPath);
    
    // 清理输出
    bool cleanOutput(const std::string& outputDir);
    
    // 生成项目模板
    bool generateProjectTemplate(const std::string& projectPath, const std::string& templateName);
    
    // === 优化和分析 ===
    
    // 分析项目性能
    std::unordered_map<std::string, double> analyzePerformance(const UnifiedCompilerResult& result);
    
    // 优化建议
    std::vector<std::string> getOptimizationSuggestions(const UnifiedCompilerResult& result);
    
    // 代码统计
    std::unordered_map<std::string, size_t> getCodeStatistics(const std::string& projectPath);
    
    // 依赖分析
    std::vector<std::string> analyzeDependencies(const std::string& projectPath);
    
    // === 导出和发布 ===
    
    // 导出项目
    bool exportProject(const std::string& projectPath, const std::string& exportPath, const std::string& format);
    
    // 生成部署包
    bool generateDeploymentPackage(const UnifiedCompilerResult& result, const std::string& outputPath);
    
    // 生成文档
    std::string generateDocumentation(const std::string& projectPath);
    
    // === 调试和诊断 ===
    
    // 检查编译器状态
    bool isReady() const;
    
    // 获取版本信息
    std::string getVersion() const;
    
    // 获取支持的特性
    std::vector<std::string> getSupportedFeatures() const;
    
    // 诊断项目
    std::vector<std::string> diagnoseProject(const std::string& projectPath);
    
    // 修复项目问题
    std::vector<std::string> fixProjectIssues(const std::string& projectPath);
    
private:
    // === 内部方法 ===
    
    // 初始化编译器
    void initializeCompilers();
    
    // 集成编译结果
    UnifiedCompilerResult integrateResults(const CHTL::CHTLCompilerResult& chtlResult,
                                         const CHTLJS::CHTLJSCompilerResult& chtljsResult);
    
    // 合并输出
    std::string mergeHTML(const std::string& chtlHTML, const std::string& chtljsHTML);
    std::string mergeCSS(const std::string& chtlCSS, const std::string& chtljsCSS);
    std::string mergeJS(const std::string& chtlJS, const std::string& chtljsJS);
    
    // 优化输出
    std::string optimizeHTML(const std::string& html);
    std::string optimizeCSS(const std::string& css);
    std::string optimizeJS(const std::string& js);
    
    // 文件操作
    std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern);
    std::string readFile(const std::string& filepath);
    bool writeFile(const std::string& filepath, const std::string& content);
    bool createDirectory(const std::string& path);
    
    // 路径处理
    std::string resolvePath(const std::string& path, const std::string& basePath = "");
    std::string getRelativePath(const std::string& path, const std::string& basePath);
    std::string joinPath(const std::string& path1, const std::string& path2);
    
    // 缓存管理
    std::string generateCacheKey(const std::vector<std::string>& files);
    bool hasCachedResult(const std::string& key);
    UnifiedCompilerResult getCachedResult(const std::string& key);
    void setCachedResult(const std::string& key, const UnifiedCompilerResult& result);
    
    // 错误处理
    UnifiedCompilerResult createErrorResult(const std::string& phase, const std::vector<std::string>& errors);
    void collectErrors(UnifiedCompilerResult& result);
    
    // 监听文件变化
    bool isWatching = false;
    std::function<void(const UnifiedCompilerResult&)> watchCallback;
    void watchFileChanges(const std::string& projectPath);
    
    // 常量
    static const std::string COMPILER_VERSION;
    static const std::vector<std::string> SUPPORTED_FILE_EXTENSIONS;
    static const std::vector<std::string> PROJECT_TEMPLATES;
};

// === 便利函数 ===

// 快速编译项目
UnifiedCompilerResult compileProject(const std::string& projectPath, 
                                   const UnifiedCompilerConfig& config = UnifiedCompilerConfig());

// 初始化新项目
bool initProject(const std::string& projectPath, const std::string& templateName = "basic");

// 验证项目结构
bool validateProjectStructure(const std::string& projectPath);

// 创建统一编译器
std::unique_ptr<UnifiedCompiler> createUnifiedCompiler(const UnifiedCompilerConfig& config = UnifiedCompilerConfig());

} // namespace CHTL::Core