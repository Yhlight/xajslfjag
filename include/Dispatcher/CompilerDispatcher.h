#ifndef CHTL_DISPATCHER_COMPILER_DISPATCHER_H
#define CHTL_DISPATCHER_COMPILER_DISPATCHER_H

#include "Core/Common.h"
#include "Scanner/UnifiedScanner.h"
#include <memory>

namespace CHTL {

// 前向声明
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JavaScriptCompiler;
class ResultMerger;
class ModuleResolver;
class ConfigurationManager;
class CompileTimer;

// 编译器调度器
class CompilerDispatcher {
private:
    // 编译器实例
    UniquePtr<UnifiedScanner> scanner_;
    UniquePtr<CHTLCompiler> chtlCompiler_;
    UniquePtr<CHTLJSCompiler> chtljsCompiler_;
    UniquePtr<CSSCompiler> cssCompiler_;
    UniquePtr<JavaScriptCompiler> jsCompiler_;
    UniquePtr<ResultMerger> resultMerger_;
    UniquePtr<ModuleResolver> moduleResolver_;
    UniquePtr<ConfigurationManager> configManager_;
    
    // 模块搜索路径
    Vector<Path> modulePaths_;
    
    // 配置文件路径
    Path configFile_;
    
    // 编译选项
    struct CompileOptions {
        bool debug = false;
        bool enableTimer = false;
        bool enableOptimization = true;
        bool enableMinification = false;
        bool enableSourceMap = false;
        String outputEncoding = "UTF-8";
    } options_;
    
    // 编译上下文
    struct CompileContext {
        Path currentFile;
        Path outputFile;
        Vector<Path> importStack;
        HashMap<Path, String> compiledFiles;
        CompileTimer* timer = nullptr;
    };
    
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 禁用拷贝
    CompilerDispatcher(const CompilerDispatcher&) = delete;
    CompilerDispatcher& operator=(const CompilerDispatcher&) = delete;
    
    // 添加模块搜索路径
    void AddModulePath(const String& path);
    
    // 设置配置文件
    void SetConfigFile(const String& configFile);
    
    // 设置编译选项
    void SetOption(const String& name, bool value);
    void SetOption(const String& name, const String& value);
    
    // 编译文件
    Result<String> CompileFile(const String& inputFile, const String& outputFile);
    
    // 编译字符串
    Result<String> CompileString(const String& content, const String& filename = "<string>");
    
    // 打包CMOD模块
    Result<void> PackCMOD(const String& directory, const String& outputFile);
    
    // 打包CJMOD模块
    Result<void> PackCJMOD(const String& directory, const String& outputFile);
    
    // 解包模块
    Result<void> UnpackModule(const String& moduleFile, const String& outputDir);
    
private:
    // 初始化编译器
    void InitializeCompilers();
    
    // 加载配置
    Result<void> LoadConfiguration(CompileContext& context);
    
    // 预处理文件
    Result<String> PreprocessFile(const Path& file, CompileContext& context);
    
    // 扫描并分割代码
    Result<CodeSlices> ScanAndSlice(const String& content, CompileContext& context);
    
    // 分发代码片段到各编译器
    Result<CompileResults> DispatchSlices(const CodeSlices& slices, CompileContext& context);
    
    // 合并编译结果
    Result<String> MergeResults(const CompileResults& results, CompileContext& context);
    
    // 后处理输出
    Result<String> PostprocessOutput(const String& output, CompileContext& context);
    
    // 写入输出文件
    Result<void> WriteOutput(const String& content, const Path& outputFile);
    
    // 处理导入
    Result<String> ProcessImport(const String& importPath, const String& importType, 
                                CompileContext& context);
    
    // 检测循环依赖
    bool CheckCircularDependency(const Path& file, const CompileContext& context);
    
    // 解析模块路径
    Optional<Path> ResolveModulePath(const String& moduleName, const String& moduleType);
    
    // 获取官方模块目录
    Path GetOfficialModuleDir() const;
    
    // 获取用户模块目录
    Vector<Path> GetUserModuleDirs(const Path& currentFile) const;
};

// 代码片段类型
enum class SliceType {
    CHTL,
    CHTLJS,
    CSS,
    JavaScript,
    HTML,
    Comment,
    Origin
};

// 代码片段
struct CodeSlice {
    SliceType type;
    String content;
    SourceLocation location;
    HashMap<String, String> metadata;
};

// 代码片段集合
using CodeSlices = Vector<CodeSlice>;

// 编译结果
struct CompileResult {
    String html;
    String css;
    String javascript;
    HashMap<String, String> additionalFiles;
    Vector<String> warnings;
    Vector<String> errors;
};

// 编译结果集合
using CompileResults = Vector<CompileResult>;

} // namespace CHTL

#endif // CHTL_DISPATCHER_COMPILER_DISPATCHER_H