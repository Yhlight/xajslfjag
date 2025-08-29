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
    explicit JSFileStream(const String& filePath);
    virtual ~JSFileStream();

    // CHTL JS特定的文件操作
    bool openCHTLJSFile(const String& filePath);
    bool openJSFile(const String& filePath);
    bool openCJJSFile(const String& filePath);
    
    // CHTL JS内容处理
    String readCHTLJSContent();
    StringVector extractCHTLJSBlocks();
    StringVector extractJSBlocks();
    
    // 模块相关操作
    StringVector extractModuleImports();
    String extractModuleDefinition();
    bool writeModuleOutput(const String& content);
    
    // CJMOD支持
    StringVector extractCJMODSyntax();
    bool processCJMODFile(const String& inputPath, const String& outputPath);
    
    // 增量编译支持
    bool hasFileChanged() const;
    String getFileHash() const;
    void updateFileCache();
    
    // CHTL JS语法验证
    bool validateCHTLJSSyntax(const String& content);
    StringVector findSyntaxErrors(const String& content);
    
    // 性能优化
    void enableCHTLJSOptimization(bool enable = true);
    bool isCHTLJSOptimizationEnabled() const;
    
private:
    std::unique_ptr<std::fstream> fileStream_;
    String currentFilePath_;
    String fileHash_;
    bool optimizationEnabled_;
    
    // 缓存系统
    struct FileCache {
        String lastHash;
        String lastContent;
        std::time_t lastModified;
    };
    FileCache cache_;
    
    // CHTL JS解析辅助
    String extractBetweenMarkers(const String& content, const String& startMarker, const String& endMarker);
    StringVector tokenizeCHTLJS(const String& content);
    bool validateBraceMatching(const String& content);
    String calculateMD5Hash(const String& content);
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
    
    static FileType detectFileType(const String& filePath);
    static FileType detectContentType(const String& content);
    
    // CHTL JS预处理
    static String preprocessCHTLJS(const String& content);
    static String extractCHTLJSScript(const String& content);
    static String extractPureJS(const String& content);
    
    // 模块处理
    static StringVector extractModuleDependencies(const String& content);
    static String generateModuleWrapper(const String& content, const String& moduleName);
    
    // CJMOD处理
    static StringVector findCJMODKeywords(const String& content);
    static String processCJMODSyntax(const String& content);
    
    // 优化处理
    static String minifyCHTLJS(const String& content);
    static String beautifyCHTLJS(const String& content);
    
private:
    static bool containsCHTLJSMarkers(const String& content);
    static bool containsModuleSyntax(const String& content);
    static String normalizeLineEndings(const String& content);
    static StringVector splitIntoBlocks(const String& content);
};

// CHTL JS异步文件处理器
class AsyncCHTLJSProcessor {
public:
    AsyncCHTLJSProcessor();
    ~AsyncCHTLJSProcessor();
    
    // 异步文件处理
    void processFileAsync(const String& inputPath, const String& outputPath, 
                         std::function<void(bool)> callback);
    
    // 批量处理
    void processBatchAsync(const StringVector& inputPaths, const String& outputDir,
                          std::function<void(size_t, size_t)> progressCallback);
    
    // 监控文件变化
    void watchFile(const String& filePath, std::function<void(const String&)> changeCallback);
    void stopWatching();
    
    bool isProcessing() const;
    void cancelProcessing();
    
private:
    bool isProcessing_;
    bool shouldCancel_;
    
    void processInBackground(const String& inputPath, const String& outputPath,
                           std::function<void(bool)> callback);
};

} // namespace CHTLJS::IO