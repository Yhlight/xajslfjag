#pragma once

#include "../../Util/FileSystem/FileSystem.h"
#include "../../Util/Common.h"
#include <memory>
#include <fstream>
#include <vector>

namespace CHTLJS::IO {

// CHTL JS IO流系统 - 继承自Util::FileSystem
class JSFileStream : public Util::FileSystem {
public:
    JSFileStream();
    explicit JSFileStream(const CHTL::String& filePath);
    virtual ~JSFileStream();

    // CHTL JS特定的文件操作
    bool openCHTLJSFile(const CHTL::String& filePath);
    bool openJSFile(const CHTL::String& filePath);
    bool openCJJSFile(const CHTL::String& filePath);
    
    // CHTL JS内容处理
    CHTL::String readCHTLJSContent();
    CHTL::StringVector extractCHTLJSBlocks();
    CHTL::StringVector extractJSBlocks();
    
    // 模块相关操作
    CHTL::StringVector extractModuleImports();
    CHTL::String extractModuleDefinition();
    bool writeModuleOutput(const CHTL::String& content);
    
    // CJMOD支持
    CHTL::StringVector extractCJMODSyntax();
    bool processCJMODFile(const CHTL::String& inputPath, const CHTL::String& outputPath);
    
    // 增量编译支持
    bool hasFileChanged() const;
    CHTL::String getFileHash() const;
    void updateFileCache();
    
    // CHTL JS语法验证
    bool validateCHTLJSSyntax(const CHTL::String& content);
    CHTL::StringVector findSyntaxErrors(const CHTL::String& content);
    
    // 性能优化
    void enableCHTLJSOptimization(bool enable = true);
    bool isCHTLJSOptimizationEnabled() const;
    
private:
    std::unique_ptr<std::fstream> fileStream_;
    CHTL::String currentFilePath_;
    CHTL::String fileHash_;
    bool optimizationEnabled_;
    
    // 缓存系统
    struct FileCache {
        CHTL::String lastHash;
        CHTL::String lastContent;
        std::time_t lastModified;
    };
    FileCache cache_;
    
    // CHTL JS解析辅助
    CHTL::String extractBetweenMarkers(const CHTL::String& content, const CHTL::String& startMarker, const CHTL::String& endMarker);
    CHTL::StringVector tokenizeCHTLJS(const CHTL::String& content);
    bool validateBraceMatching(const CHTL::String& content);
    CHTL::String calculateMD5Hash(const CHTL::String& content) const;
};

// CHTL JS文件处理工具
class CHTLJSFileProcessor {
public:
    // 文件类型检测
    enum class FileType {
        CHTL_JS,
        PURE_JS,
        CJJS,
        MODULE,
        UNKNOWN
    };
    
    static FileType detectFileType(const CHTL::String& filePath);
    static FileType detectContentType(const CHTL::String& content);
    
    // CHTL JS预处理
    static CHTL::String preprocessCHTLJS(const CHTL::String& content);
    static CHTL::String extractCHTLJSScript(const CHTL::String& content);
    static CHTL::String extractPureJS(const CHTL::String& content);
    
    // 模块处理
    static CHTL::StringVector extractModuleDependencies(const CHTL::String& content);
    static CHTL::String generateModuleWrapper(const CHTL::String& content, const CHTL::String& moduleName);
    
    // CJMOD处理
    static CHTL::StringVector findCJMODKeywords(const CHTL::String& content);
    static CHTL::String processCJMODSyntax(const CHTL::String& content);
    
    // 优化处理
    static CHTL::String minifyCHTLJS(const CHTL::String& content);
    static CHTL::String beautifyCHTLJS(const CHTL::String& content);
    static CHTL::StringVector splitIntoBlocks(const CHTL::String& content);
    
private:
    static bool containsCHTLJSMarkers(const CHTL::String& content);
    static bool containsModuleSyntax(const CHTL::String& content);
    static CHTL::String normalizeLineEndings(const CHTL::String& content);
};

// CHTL JS异步文件处理器
class AsyncCHTLJSProcessor {
public:
    AsyncCHTLJSProcessor();
    ~AsyncCHTLJSProcessor();
    
    // 异步文件处理
    void processFileAsync(const CHTL::String& inputPath, const CHTL::String& outputPath, 
                         std::function<void(bool)> callback);
    
    // 批量处理
    void processBatchAsync(const CHTL::StringVector& inputPaths, const CHTL::String& outputDir,
                          std::function<void(size_t, size_t)> progressCallback);
    
    // 监控文件变化
    void watchFile(const CHTL::String& filePath, std::function<void(const CHTL::String&)> changeCallback);
    void stopWatching();
    
    bool isProcessing() const;
    void cancelProcessing();
    
private:
    bool isProcessing_;
    bool shouldCancel_;
    
    void processInBackground(const CHTL::String& inputPath, const CHTL::String& outputPath,
                           std::function<void(bool)> callback);
};

} // namespace CHTLJS::IO