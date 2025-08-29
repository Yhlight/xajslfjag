#pragma once

#include "../../Util/Common.h"
#include "../../CHTL/CHTLNode/BaseNode.h"

namespace CHTL {

// CJMOD文件类型
enum class CJMODFileType {
    CPP_SOURCE,         // .cpp源文件
    HEADER_FILE,        // .h/.hpp头文件
    INFO_FILE,          // info/模块名.chtl信息文件
    SUBMODULE,          // 子模块
    UNKNOWN_FILE
};

// CJMOD包结构
struct CJMODStructure {
    String moduleName;              // 模块名称
    String rootPath;                // 根路径
    String srcPath;                 // src路径
    String infoPath;                // info路径
    
    // 源文件
    StringVector cppFiles;          // .cpp文件列表
    StringVector headerFiles;       // .h/.hpp文件列表
    String infoFile;                // info/模块名.chtl
    
    // 子模块
    struct SubModule {
        String name;
        String srcPath;             // 子模块名/src/
        String infoPath;            // 子模块名/info/
        StringVector cppFiles;
        StringVector headerFiles;
        String infoFile;
        
        SubModule() = default;
    };
    
    std::vector<SubModule> subModules;
    bool isValid;
    
    CJMODStructure() : isValid(false) {}
    
    bool validate() const;
    String toString() const;
    StringVector getAllSourceFiles() const;
    StringVector getAllHeaderFiles() const;
};

// CJMOD信息
struct CJMODInfo {
    String name;                    // 模块名称
    String version;                 // 版本
    String description;             // 描述
    String author;                  // 作者
    String license;                 // 许可证
    StringVector dependencies;      // 依赖
    String category;                // 类别
    String minCHTLVersion;          // 最小CHTL版本
    String maxCHTLVersion;          // 最大CHTL版本
    
    // C++编译配置
    struct CompileConfig {
        String cppStandard = "c++17";       // C++标准
        StringVector includeDirectories;    // 包含目录
        StringVector libraryDirectories;    // 库目录
        StringVector libraries;             // 链接库
        StringVector compileFlags;          // 编译标志
        StringVector linkFlags;             // 链接标志
        StringVector preprocessorDefines;   // 预处理器定义
    } compileConfig;
    
    // 导出的CHTL JS功能
    struct ExportTable {
        StringVector functions;             // 导出的函数
        StringVector syntaxExtensions;      // 语法扩展
        StringVector operators;             // 操作符
        StringVector types;                 // 类型
        
        bool isEmpty() const;
        String toString() const;
    } exportTable;
    
    StringUnorderedMap metadata;    // 额外元数据
    
    CJMODInfo() = default;
    
    bool isValid() const;
    String generateInfoContent() const;
    void parseFromContent(const String& content);
};

// C++编译器集成
class CppCompiler {
public:
    enum class CompilerType {
        GCC,
        CLANG,
        MSVC,
        AUTO_DETECT
    };
    
    struct CompilerConfig {
        CompilerType type = CompilerType::AUTO_DETECT;
        String compilerPath = "";           // 编译器路径
        String cppStandard = "c++17";       // C++标准
        String optimizationLevel = "O2";    // 优化级别
        bool enableDebugInfo = false;       // 调试信息
        bool enableWarnings = true;         // 警告
        bool treatWarningsAsErrors = false; // 警告当作错误
        StringVector additionalFlags;       // 额外标志
        String outputDirectory = "build";   // 输出目录
    };
    
    explicit CppCompiler(const CompilerConfig& config = CompilerConfig{});
    
    // 编译方法
    bool compileSource(const String& sourceFile, const String& outputFile);
    bool compileMultiple(const StringVector& sourceFiles, const String& outputFile);
    bool compileLibrary(const StringVector& sourceFiles, const String& libraryName, bool isStatic = true);
    
    // 编译CJMOD
    bool compileCJMOD(const CJMODStructure& structure, const String& outputPath);
    bool compileCJMODPackage(const String& cjmodPath, const String& outputPath);
    
    // 依赖管理
    StringVector analyzeDependencies(const String& sourceFile) const;
    StringVector resolveIncludes(const String& sourceFile) const;
    bool checkDependencies(const StringVector& sourceFiles) const;
    
    // 配置管理
    void setConfig(const CompilerConfig& config) { this->config = config; }
    const CompilerConfig& getConfig() const { return config; }
    
    void addIncludeDirectory(const String& directory);
    void addLibraryDirectory(const String& directory);
    void addLibrary(const String& library);
    void addCompileFlag(const String& flag);
    
    // 编译信息
    struct CompilationResult {
        bool success = false;
        String outputFile;
        StringVector compiledFiles;
        StringVector errors;
        StringVector warnings;
        double compilationTime = 0.0;
        size_t outputSize = 0;
    };
    
    CompilationResult getLastResult() const { return lastResult; }
    
    // 工具方法
    static bool isCompilerAvailable(CompilerType type);
    static String detectCompilerPath(CompilerType type);
    static String getCompilerVersion(CompilerType type, const String& compilerPath = "");
    static CompilerType detectCompilerType();
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    CompilerConfig config;
    CompilationResult lastResult;
    StringVector errors;
    
    // 内部编译逻辑
    String buildCompileCommand(const StringVector& sourceFiles, const String& outputFile) const;
    String buildLibraryCommand(const StringVector& sourceFiles, const String& libraryName, bool isStatic) const;
    bool executeCommand(const String& command, String& output, String& errors) const;
    
    // 路径处理
    String resolveCompilerPath() const;
    StringVector resolveIncludePaths() const;
    StringVector resolveLibraryPaths() const;
    
    // 文件操作
    bool createOutputDirectory(const String& path) const;
    String getObjectFileName(const String& sourceFile) const;
    
    // 错误报告
    void reportError(const String& message);
    void parseCompilerOutput(const String& output, StringVector& errors, StringVector& warnings);
};

// CJMOD API接口
class CJMODAPIInterface {
public:
    // API类型定义
    struct APIFunction {
        String name;
        String signature;
        String description;
        StringVector parameters;
        String returnType;
        bool isVirtual;
        
        APIFunction() : isVirtual(false) {}
    };
    
    struct APIClass {
        String name;
        String description;
        std::vector<APIFunction> methods;
        StringVector baseClasses;
        bool isAbstract;
        
        APIClass() : isAbstract(false) {}
    };
    
    // 核心API类
    static APIClass getSyntaxClass();      // Syntax类API
    static APIClass getArgClass();         // Arg类API
    static APIClass getAtomArgClass();     // AtomArg类API
    static APIClass getScannerClass();     // CJMODScanner类API
    static APIClass getGeneratorClass();   // CJMODGenerator类API
    static APIClass getCHTLJSFunctionClass(); // CHTLJSFunction类API
    
    // API文档生成
    static String generateAPIDocumentation();
    static String generateClassDocumentation(const APIClass& apiClass);
    static String generateFunctionDocumentation(const APIFunction& function);
    
    // 代码生成
    static String generateHeaderTemplate(const String& moduleName);
    static String generateSourceTemplate(const String& moduleName);
    static String generateExampleCode(const String& functionName);
    
    // 验证
    static bool validateAPIUsage(const String& sourceCode);
    static StringVector getAPIValidationErrors(const String& sourceCode);
};

// CJMOD包装器
class CJMODPackage {
public:
    explicit CJMODPackage(const String& packagePath);
    ~CJMODPackage() = default;
    
    // 基础信息
    const String& getPackagePath() const { return packagePath; }
    const String& getModuleName() const { return structure.moduleName; }
    const CJMODStructure& getStructure() const { return structure; }
    const CJMODInfo& getInfo() const { return info; }
    
    // 加载和解析
    bool load();
    bool compile();
    bool isLoaded() const { return loaded; }
    bool isCompiled() const { return compiled; }
    
    // 内容访问
    String getSourceContent(const String& fileName) const;
    String getHeaderContent(const String& fileName) const;
    String getInfoContent() const;
    StringVector getSourceFiles() const;
    StringVector getHeaderFiles() const;
    
    // 子模块访问
    StringVector getSubModuleNames() const;
    String getSubModuleSource(const String& subModuleName, const String& fileName) const;
    String getSubModuleHeader(const String& subModuleName, const String& fileName) const;
    
    // 编译产物
    String getCompiledLibraryPath() const;
    bool isLibraryAvailable() const;
    
    // 导出功能
    StringVector getExportedFunctions() const;
    StringVector getExportedSyntaxExtensions() const;
    StringVector getExportedOperators() const;
    StringVector getExportedTypes() const;
    
    // 依赖管理
    StringVector getDependencies() const { return info.dependencies; }
    bool hasDependency(const String& dependency) const;
    
    // API验证
    bool validateAPI() const;
    StringVector getAPIValidationErrors() const;
    
    // 编译配置
    void setCompilerConfig(const CppCompiler::CompilerConfig& config);
    const CppCompiler::CompilerConfig& getCompilerConfig() const;
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    String packagePath;
    CJMODStructure structure;
    CJMODInfo info;
    StringUnorderedMap sourceContents;   // 文件名 -> 内容
    StringUnorderedMap headerContents;   // 文件名 -> 内容
    std::unique_ptr<CppCompiler> compiler;
    String compiledLibraryPath;
    StringVector errors;
    bool loaded;
    bool compiled;
    
    // 内部方法
    bool analyzeStructure();
    bool loadSourceContents();
    bool parseModuleInfo();
    CJMODInfo parseInfoFromContent(const String& content) const;
    bool compileSources();
    void generateExportTable();
    
    // 文件操作
    String readFile(const String& filePath) const;
    bool fileExists(const String& filePath) const;
    bool directoryExists(const String& dirPath) const;
    StringVector listSourceFiles(const String& directory) const;
    StringVector listHeaderFiles(const String& directory) const;
    
    // 错误报告
    void reportError(const String& message);
};

// CJMOD打包器
class CJMODPackager {
public:
    struct PackagingOptions {
        bool includeSourceCode = true;      // 包含源代码
        bool includeCompiledLibrary = false; // 包含编译库
        bool includeSubModules = true;      // 包含子模块
        bool validateAPI = true;            // 验证API
        bool compressPackage = true;        // 压缩包
        String outputPath = "";             // 输出路径
        String compressionLevel = "normal"; // 压缩级别
        CppCompiler::CompilerConfig compilerConfig; // 编译配置
    };
    
    explicit CJMODPackager(const PackagingOptions& options = PackagingOptions{});
    
    // 打包方法
    bool packageDirectory(const String& sourceDir, const String& outputPath);
    bool packageFiles(const StringUnorderedMap& files, const String& outputPath, const String& moduleName);
    
    // 从现有结构打包
    bool packageFromStructure(const CJMODStructure& structure, const String& outputPath);
    
    // 预编译打包
    bool packageWithCompilation(const String& sourceDir, const String& outputPath);
    
    // 验证
    bool validateSourceDirectory(const String& sourceDir) const;
    CJMODStructure analyzeSourceDirectory(const String& sourceDir) const;
    
    // 打包统计
    struct PackagingStats {
        size_t totalFiles = 0;
        size_t sourceFiles = 0;
        size_t headerFiles = 0;
        size_t totalSize = 0;
        size_t compressedSize = 0;
        double compressionRatio = 0.0;
        double packagingTime = 0.0;
        double compilationTime = 0.0;
        bool compilationSuccessful = false;
    };
    
    PackagingStats getLastPackagingStats() const { return lastStats; }
    
    // 配置
    void setOptions(const PackagingOptions& options) { this->options = options; }
    const PackagingOptions& getOptions() const { return options; }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    PackagingOptions options;
    PackagingStats lastStats;
    StringVector errors;
    
    // 内部打包逻辑
    bool createPackageStructure(const String& tempDir, const CJMODStructure& structure);
    bool copySourceFiles(const String& sourceDir, const String& tempDir, const CJMODStructure& structure);
    bool generateInfoFile(const String& infoPath, const CJMODInfo& info);
    bool compileAndInclude(const String& tempDir, const CJMODStructure& structure);
    bool compressDirectory(const String& sourceDir, const String& outputPath);
    
    // 验证
    bool validateAPICompliance(const CJMODStructure& structure) const;
    StringVector getAPIComplianceErrors(const CJMODStructure& structure) const;
    
    // 错误报告
    void reportError(const String& message);
};

// CJMOD解包器
class CJMODUnpacker {
public:
    struct UnpackingOptions {
        bool extractSources = true;         // 解压源代码
        bool extractLibraries = true;       // 解压库文件
        bool compileAfterExtraction = false; // 解压后编译
        bool validateAfterExtraction = true; // 解压后验证
        bool overwriteExisting = false;     // 覆盖已存在文件
        String extractionPath = "";         // 解压路径
        CppCompiler::CompilerConfig compilerConfig; // 编译配置
    };
    
    explicit CJMODUnpacker(const UnpackingOptions& options = UnpackingOptions{});
    
    // 解包方法
    bool unpackage(const String& cjmodPath, const String& outputDir);
    bool unpackageToMemory(const String& cjmodPath, StringUnorderedMap& extractedFiles);
    
    // 编译解包
    bool unpackageAndCompile(const String& cjmodPath, const String& outputDir);
    
    // 验证
    bool validateCJMODFile(const String& cjmodPath) const;
    CJMODStructure analyzeCJMODFile(const String& cjmodPath) const;
    
    // 配置
    void setOptions(const UnpackingOptions& options) { this->options = options; }
    const UnpackingOptions& getOptions() const { return options; }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    UnpackingOptions options;
    StringVector errors;
    
    // 内部解包逻辑
    bool extractArchive(const String& archivePath, const String& outputDir);
    bool compileExtractedSources(const String& extractedDir);
    bool validateExtractedStructure(const String& extractedDir) const;
    
    // 错误报告
    void reportError(const String& message);
};

// CJMOD管理器
class CJMODManager {
public:
    explicit CJMODManager(const String& installDirectory = "cjmods");
    ~CJMODManager() = default;
    
    // 包管理
    bool installPackage(const String& cjmodPath, bool compile = true);
    bool uninstallPackage(const String& packageName);
    bool updatePackage(const String& packageName);
    bool recompilePackage(const String& packageName);
    
    // 包查询
    bool isPackageInstalled(const String& packageName) const;
    bool isPackageCompiled(const String& packageName) const;
    StringVector getInstalledPackages() const;
    CJMODPackage getInstalledPackage(const String& packageName) const;
    
    // 编译管理
    bool compileAll();
    bool compilePackage(const String& packageName);
    StringVector getCompilationErrors(const String& packageName) const;
    
    // API集成
    bool loadCJMODAPI(const String& packageName);
    bool unloadCJMODAPI(const String& packageName);
    StringVector getLoadedAPIs() const;
    
    // 依赖管理
    StringVector getPackageDependencies(const String& packageName) const;
    StringVector resolveDependencies(const String& packageName) const;
    bool hasCircularDependencies(const String& packageName) const;
    
    // 编译器配置
    void setDefaultCompilerConfig(const CppCompiler::CompilerConfig& config);
    const CppCompiler::CompilerConfig& getDefaultCompilerConfig() const { return defaultCompilerConfig; }
    
    // 统计信息
    struct ManagerStats {
        size_t installedPackages = 0;
        size_t compiledPackages = 0;
        size_t loadedAPIs = 0;
        size_t compilationErrors = 0;
        double totalCompilationTime = 0.0;
        StringVector recentInstalls;
        StringVector recentCompilations;
    };
    
    ManagerStats getStats() const;
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    String installDirectory;
    CppCompiler::CompilerConfig defaultCompilerConfig;
    StringUnorderedMap installedPackages;    // name -> install_path
    StringUnorderedMap compiledLibraries;    // name -> library_path
    StringUnorderedMap loadedAPIs;           // name -> api_handle
    StringVector errors;
    
    // 内部方法
    String getPackageInstallPath(const String& packageName) const;
    String getPackageLibraryPath(const String& packageName) const;
    bool createPackageDirectory(const String& packageName);
    bool removePackageDirectory(const String& packageName);
    
    // API加载（需要平台特定实现）
    bool loadDynamicLibrary(const String& libraryPath, const String& packageName);
    bool unloadDynamicLibrary(const String& packageName);
    
    // 注册表操作
    void registerPackage(const String& packageName, const String& installPath);
    void unregisterPackage(const String& packageName);
    void saveRegistry();
    void loadRegistry();
    
    // 错误报告
    void reportError(const String& message);
};

// CJMOD工厂
class CJMODFactory {
public:
    static std::unique_ptr<CJMODPackage> loadPackage(const String& packagePath);
    static std::unique_ptr<CJMODPackager> createPackager(const CJMODPackager::PackagingOptions& options = {});
    static std::unique_ptr<CJMODUnpacker> createUnpacker(const CJMODUnpacker::UnpackingOptions& options = {});
    static std::unique_ptr<CJMODManager> createManager(const String& installDirectory = "cjmods");
    static std::unique_ptr<CppCompiler> createCompiler(const CppCompiler::CompilerConfig& config = {});
    
    // 便捷方法
    static bool quickPackage(const String& sourceDir, const String& outputPath, bool includeCompiled = false);
    static bool quickUnpackage(const String& cjmodPath, const String& outputDir, bool compile = false);
    static CJMODStructure quickAnalyze(const String& path);
    static bool quickCompile(const StringVector& sourceFiles, const String& outputPath);
};

// CJMOD工具
class CJMODTools {
public:
    // 代码生成
    static String generateCJMODTemplate(const String& moduleName, const StringVector& functions);
    static String generateAPIWrapper(const String& moduleName, const StringVector& functions);
    static String generateMakefile(const CJMODStructure& structure, const CJMODInfo& info);
    static String generateCMakeFile(const CJMODStructure& structure, const CJMODInfo& info);
    
    // 代码分析
    static StringVector extractFunctionSignatures(const String& sourceCode);
    static StringVector extractIncludeDependencies(const String& sourceCode);
    static bool validateCJMODCompliance(const String& sourceCode);
    static StringVector getCJMODComplianceErrors(const String& sourceCode);
    
    // 文档生成
    static String generateAPIDocumentation(const CJMODPackage& package);
    static String generateUsageExamples(const CJMODPackage& package);
    static String generateInstallationGuide(const CJMODPackage& package);
    
    // 测试生成
    static String generateUnitTests(const CJMODPackage& package);
    static String generateIntegrationTests(const CJMODPackage& package);
    
    // 版本管理
    static String generateVersionHeader(const String& version);
    static bool updateVersionInFiles(const StringVector& files, const String& newVersion);
};

} // namespace CHTL