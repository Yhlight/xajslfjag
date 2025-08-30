#pragma once

#include "../../Util/FileSystem/FileSystem.h"
#include "../../Util/Common.h"
#include <memory>
#include <fstream>
#include <vector>

namespace CHTL::IO {

// CHTL IO流系统 - 继承自Util::FileSystem
class FileStream : public Util::FileSystem {
public:
    FileStream();
    explicit FileStream(const CHTL::String& filePath);
    virtual ~FileStream();

    // 高性能文件读取
    bool openForRead(const CHTL::String& filePath);
    bool openForWrite(const CHTL::String& filePath);
    bool openForAppend(const CHTL::String& filePath);
    
    // 缓冲区读写
    CHTL::String readAll();
    CHTL::StringVector readLines();
    bool writeContent(const CHTL::String& content);
    bool appendContent(const CHTL::String& content);
    
    // 流式读写（用于大文件）
    bool readChunk(String& chunk, size_t chunkSize = 8192);
    bool writeChunk(const CHTL::String& chunk);
    
    // 内存映射文件支持
    bool mapFile();
    const char* getMappedData() const;
    size_t getMappedSize() const;
    
    // 异步IO支持
    void enableAsyncMode(bool enable = true);
    bool isAsyncModeEnabled() const;
    
    // 性能监控
    size_t getBytesRead() const;
    size_t getBytesWritten() const;
    double getLastIOTime() const;
    
    // 文件状态
    bool isOpen() const;
    bool isEOF() const;
    void close();
    
private:
    std::unique_ptr<std::fstream> fileStream_;
    CHTL::String currentFilePath_;
    bool isAsyncMode_;
    size_t bytesRead_;
    size_t bytesWritten_;
    double lastIOTime_;
    
    // 内存映射相关
    void* mappedData_;
    size_t mappedSize_;
    
    // 缓冲区
    std::vector<char> readBuffer_;
    std::vector<char> writeBuffer_;
    
    // 性能优化方法
    void optimizeBufferSize();
    void flushWriteBuffer();
    void updateIOStats(size_t bytes, bool isWrite);
};

// CHTL专用的文件处理工具
class CHTLFileProcessor {
public:
    static bool processCHTLFile(const CHTL::String& inputPath, const CHTL::String& outputPath);
    static StringVector extractImports(const CHTL::String& chtlContent);
    static String preprocessCHTLContent(const CHTL::String& content);
    static bool validateCHTLSyntax(const CHTL::String& content);
    
private:
    static String removeComments(const CHTL::String& content);
    static String normalizeWhitespace(const CHTL::String& content);
    static StringVector tokenizeForImports(const CHTL::String& content);
};

} // namespace CHTL::IO