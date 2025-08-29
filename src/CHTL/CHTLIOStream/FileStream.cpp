#include "FileStream.h"
#include <iostream>
#include <algorithm>
#include <chrono>

namespace CHTL::IO {

FileStream::FileStream() 
    : fileStream_(nullptr)
    , isAsyncMode_(false)
    , bytesRead_(0)
    , bytesWritten_(0)
    , lastIOTime_(0.0)
    , mappedData_(nullptr)
    , mappedSize_(0) {
    readBuffer_.reserve(8192);
    writeBuffer_.reserve(8192);
}

FileStream::FileStream(const String& filePath) : FileStream() {
    currentFilePath_ = filePath;
}

FileStream::~FileStream() {
    close();
}

bool FileStream::openForRead(const String& filePath) {
    close();
    currentFilePath_ = filePath;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    fileStream_ = std::make_unique<std::fstream>(filePath, std::ios::in | std::ios::binary);
    
    auto end = std::chrono::high_resolution_clock::now();
    lastIOTime_ = std::chrono::duration<double, std::milli>(end - start).count();
    
    return fileStream_ && fileStream_->is_open();
}

bool FileStream::openForWrite(const String& filePath) {
    close();
    currentFilePath_ = filePath;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    fileStream_ = std::make_unique<std::fstream>(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
    
    auto end = std::chrono::high_resolution_clock::now();
    lastIOTime_ = std::chrono::duration<double, std::milli>(end - start).count();
    
    return fileStream_ && fileStream_->is_open();
}

bool FileStream::openForAppend(const String& filePath) {
    close();
    currentFilePath_ = filePath;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    fileStream_ = std::make_unique<std::fstream>(filePath, std::ios::out | std::ios::app | std::ios::binary);
    
    auto end = std::chrono::high_resolution_clock::now();
    lastIOTime_ = std::chrono::duration<double, std::milli>(end - start).count();
    
    return fileStream_ && fileStream_->is_open();
}

String FileStream::readAll() {
    if (!fileStream_ || !fileStream_->is_open()) {
        return "";
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    fileStream_->seekg(0, std::ios::end);
    size_t fileSize = fileStream_->tellg();
    fileStream_->seekg(0, std::ios::beg);
    
    String content;
    content.resize(fileSize);
    
    fileStream_->read(&content[0], fileSize);
    size_t actualRead = fileStream_->gcount();
    content.resize(actualRead);
    
    auto end = std::chrono::high_resolution_clock::now();
    lastIOTime_ = std::chrono::duration<double, std::milli>(end - start).count();
    
    updateIOStats(actualRead, false);
    
    return content;
}

StringVector FileStream::readLines() {
    StringVector lines;
    if (!fileStream_ || !fileStream_->is_open()) {
        return lines;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    String line;
    size_t totalBytes = 0;
    
    while (std::getline(*fileStream_, line)) {
        lines.push_back(line);
        totalBytes += line.length() + 1; // +1 for newline
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    lastIOTime_ = std::chrono::duration<double, std::milli>(end - start).count();
    
    updateIOStats(totalBytes, false);
    
    return lines;
}

bool FileStream::writeContent(const String& content) {
    if (!fileStream_ || !fileStream_->is_open()) {
        return false;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    fileStream_->write(content.c_str(), content.length());
    fileStream_->flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    lastIOTime_ = std::chrono::duration<double, std::milli>(end - start).count();
    
    updateIOStats(content.length(), true);
    
    return fileStream_->good();
}

bool FileStream::appendContent(const String& content) {
    if (!fileStream_ || !fileStream_->is_open()) {
        return false;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    fileStream_->seekp(0, std::ios::end);
    fileStream_->write(content.c_str(), content.length());
    fileStream_->flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    lastIOTime_ = std::chrono::duration<double, std::milli>(end - start).count();
    
    updateIOStats(content.length(), true);
    
    return fileStream_->good();
}

bool FileStream::readChunk(String& chunk, size_t chunkSize) {
    if (!fileStream_ || !fileStream_->is_open()) {
        return false;
    }
    
    readBuffer_.resize(chunkSize);
    fileStream_->read(readBuffer_.data(), chunkSize);
    
    size_t actualRead = fileStream_->gcount();
    chunk.assign(readBuffer_.data(), actualRead);
    
    updateIOStats(actualRead, false);
    
    return actualRead > 0;
}

bool FileStream::writeChunk(const String& chunk) {
    if (!fileStream_ || !fileStream_->is_open()) {
        return false;
    }
    
    fileStream_->write(chunk.c_str(), chunk.length());
    updateIOStats(chunk.length(), true);
    
    return fileStream_->good();
}

void FileStream::enableAsyncMode(bool enable) {
    isAsyncMode_ = enable;
}

bool FileStream::isAsyncModeEnabled() const {
    return isAsyncMode_;
}

size_t FileStream::getBytesRead() const {
    return bytesRead_;
}

size_t FileStream::getBytesWritten() const {
    return bytesWritten_;
}

double FileStream::getLastIOTime() const {
    return lastIOTime_;
}

bool FileStream::isOpen() const {
    return fileStream_ && fileStream_->is_open();
}

bool FileStream::isEOF() const {
    return fileStream_ ? fileStream_->eof() : true;
}

void FileStream::close() {
    if (fileStream_ && fileStream_->is_open()) {
        flushWriteBuffer();
        fileStream_->close();
    }
    fileStream_.reset();
    
    // 清理内存映射
    if (mappedData_) {
        // 这里应该调用系统特定的内存映射清理函数
        mappedData_ = nullptr;
        mappedSize_ = 0;
    }
}

void FileStream::updateIOStats(size_t bytes, bool isWrite) {
    if (isWrite) {
        bytesWritten_ += bytes;
    } else {
        bytesRead_ += bytes;
    }
}

void FileStream::optimizeBufferSize() {
    // 根据文件大小动态调整缓冲区大小
    if (fileStream_ && fileStream_->is_open()) {
        fileStream_->seekg(0, std::ios::end);
        size_t fileSize = fileStream_->tellg();
        fileStream_->seekg(0, std::ios::beg);
        
        size_t optimalSize = std::min(fileSize, static_cast<size_t>(64 * 1024)); // 最大64KB
        readBuffer_.reserve(optimalSize);
        writeBuffer_.reserve(optimalSize);
    }
}

void FileStream::flushWriteBuffer() {
    if (fileStream_ && !writeBuffer_.empty()) {
        fileStream_->write(writeBuffer_.data(), writeBuffer_.size());
        fileStream_->flush();
        writeBuffer_.clear();
    }
}

// CHTLFileProcessor实现
bool CHTLFileProcessor::processCHTLFile(const String& inputPath, const String& outputPath) {
    FileStream inputStream(inputPath);
    if (!inputStream.openForRead(inputPath)) {
        return false;
    }
    
    String content = inputStream.readAll();
    inputStream.close();
    
    // 预处理CHTL内容
    String processedContent = preprocessCHTLContent(content);
    
    FileStream outputStream(outputPath);
    if (!outputStream.openForWrite(outputPath)) {
        return false;
    }
    
    bool success = outputStream.writeContent(processedContent);
    outputStream.close();
    
    return success;
}

StringVector CHTLFileProcessor::extractImports(const String& chtlContent) {
    StringVector imports;
    
    // 简单的import提取逻辑
    size_t pos = 0;
    while ((pos = chtlContent.find("@import", pos)) != String::npos) {
        size_t lineEnd = chtlContent.find('\n', pos);
        if (lineEnd != String::npos) {
            String importLine = chtlContent.substr(pos, lineEnd - pos);
            imports.push_back(importLine);
        }
        pos = lineEnd != String::npos ? lineEnd + 1 : chtlContent.length();
    }
    
    return imports;
}

String CHTLFileProcessor::preprocessCHTLContent(const String& content) {
    String processed = content;
    
    // 移除注释
    processed = removeComments(processed);
    
    // 规范化空白
    processed = normalizeWhitespace(processed);
    
    return processed;
}

bool CHTLFileProcessor::validateCHTLSyntax(const String& content) {
    // 基本的语法验证
    // 检查括号匹配
    int braceCount = 0;
    int parenCount = 0;
    
    for (char c : content) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '(': parenCount++; break;
            case ')': parenCount--; break;
        }
        
        if (braceCount < 0 || parenCount < 0) {
            return false;
        }
    }
    
    return braceCount == 0 && parenCount == 0;
}

String CHTLFileProcessor::removeComments(const String& content) {
    String result;
    bool inLineComment = false;
    bool inBlockComment = false;
    
    for (size_t i = 0; i < content.length(); ++i) {
        if (!inLineComment && !inBlockComment) {
            if (i + 1 < content.length()) {
                if (content[i] == '/' && content[i + 1] == '/') {
                    inLineComment = true;
                    i++; // 跳过下一个字符
                    continue;
                } else if (content[i] == '/' && content[i + 1] == '*') {
                    inBlockComment = true;
                    i++; // 跳过下一个字符
                    continue;
                }
            }
            result += content[i];
        } else if (inLineComment) {
            if (content[i] == '\n') {
                inLineComment = false;
                result += content[i];
            }
        } else if (inBlockComment) {
            if (i + 1 < content.length() && content[i] == '*' && content[i + 1] == '/') {
                inBlockComment = false;
                i++; // 跳过下一个字符
            }
        }
    }
    
    return result;
}

String CHTLFileProcessor::normalizeWhitespace(const String& content) {
    String result;
    bool previousWasSpace = false;
    
    for (char c : content) {
        if (std::isspace(c)) {
            if (!previousWasSpace) {
                result += ' ';
                previousWasSpace = true;
            }
        } else {
            result += c;
            previousWasSpace = false;
        }
    }
    
    return result;
}

} // namespace CHTL::IO