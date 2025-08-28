#include "CHTLJSIOStream.h"
#include <algorithm>
#include <sstream>
#include <cstring>
#include <functional>
#include <thread>
#include <chrono>

namespace CHTLJS {

CHTLJSIOStream::CHTLJSIOStream()
    : totalBytesRead_(0), totalBytesWritten_(0), totalOperations_(0), totalErrors_(0), totalAsyncOperations_(0) {
    info_.state = CHTLJSStreamState::CLOSED;
}

CHTLJSIOStream::~CHTLJSIOStream() {
    if (info_.state != CHTLJSStreamState::CLOSED) {
        closeStream();
    }
}

bool CHTLJSIOStream::openFile(const std::string& path, const std::string& mode, const CHTLJSStreamConfig& config) {
    if (info_.state != CHTLJSStreamState::CLOSED) {
        closeStream();
    }
    
    config_ = config;
    info_.path = path;
    info_.name = getFileName(path);
    info_.type = CHTLJSStreamType::TEXT;
    info_.state = CHTLJSStreamState::CLOSED;
    info_.position = 0;
    info_.size = 0;
    
    // 设置文件打开模式
    std::ios_base::openmode openMode = std::ios_base::binary;
    
    if (mode.find('r') != std::string::npos) {
        openMode |= std::ios_base::in;
    }
    if (mode.find('w') != std::string::npos) {
        openMode |= std::ios_base::out;
        openMode |= std::ios_base::trunc;
    }
    if (mode.find('a') != std::string::npos) {
        openMode |= std::ios_base::out;
        openMode |= std::ios_base::app;
    }
    if (mode.find('+') != std::string::npos) {
        openMode |= std::ios_base::in;
        openMode |= std::ios_base::out;
    }
    
    // 打开文件
    fileStream_.open(path, openMode);
    
    if (fileStream_.is_open()) {
        info_.state = CHTLJSStreamState::GOOD;
        
        // 获取文件大小
        fileStream_.seekg(0, std::ios::end);
        info_.size = fileStream_.tellg();
        fileStream_.seekg(0, std::ios::beg);
        
        updateStatistics("operation");
        return true;
    } else {
        setError("Failed to open file: " + path);
        return false;
    }
}

bool CHTLJSIOStream::openNetwork(const std::string& url, const CHTLJSStreamConfig& config) {
    if (info_.state != CHTLJSStreamState::CLOSED) {
        closeStream();
    }
    
    config_ = config;
    info_.url = url;
    info_.name = "network_stream";
    info_.type = CHTLJSStreamType::NETWORK;
    info_.state = CHTLJSStreamState::CONNECTING;
    info_.position = 0;
    info_.size = 0;
    
    // 模拟网络连接
    // 在实际实现中，这里应该使用网络库进行连接
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    if (config_.timeout > 0) {
        info_.state = CHTLJSStreamState::CONNECTED;
        updateStatistics("operation");
        return true;
    } else {
        setError("Network connection timeout");
        return false;
    }
}

bool CHTLJSIOStream::openMemory(const std::string& initialData, const CHTLJSStreamConfig& config) {
    if (info_.state != CHTLJSStreamState::CLOSED) {
        closeStream();
    }
    
    config_ = config;
    info_.name = "memory_stream";
    info_.type = CHTLJSStreamType::MEMORY;
    info_.state = CHTLJSStreamState::GOOD;
    info_.position = 0;
    info_.size = initialData.length();
    
    // 设置内存流
    memoryStream_.str(initialData);
    memoryStream_.seekg(0, std::ios::beg);
    
    updateStatistics("operation");
    return true;
}

bool CHTLJSIOStream::closeStream() {
    if (info_.state == CHTLJSStreamState::CLOSED) {
        return false;
    }
    
    if (isMemoryStream()) {
        memoryStream_.str("");
    } else if (fileStream_.is_open()) {
        flush();
        fileStream_.close();
    }
    
    info_.state = CHTLJSStreamState::CLOSED;
    info_.position = 0;
    
    updateStatistics("operation");
    return true;
}

size_t CHTLJSIOStream::readText(char* buffer, size_t size) {
    if (info_.state != CHTLJSStreamState::GOOD && info_.state != CHTLJSStreamState::CONNECTED) {
        return 0;
    }
    
    size_t bytesRead = 0;
    
    if (isMemoryStream()) {
        memoryStream_.read(buffer, size);
        bytesRead = memoryStream_.gcount();
    } else if (fileStream_.is_open()) {
        fileStream_.read(buffer, size);
        bytesRead = fileStream_.gcount();
    }
    
    if (bytesRead > 0) {
        info_.position += bytesRead;
        totalBytesRead_ += bytesRead;
        updateStatistics("read", bytesRead);
        
        checkStreamState();
    }
    
    return bytesRead;
}

std::string CHTLJSIOStream::readLine() {
    if (info_.state != CHTLJSStreamState::GOOD && info_.state != CHTLJSStreamState::CONNECTED) {
        return "";
    }
    
    std::string line;
    
    if (isMemoryStream()) {
        if (std::getline(memoryStream_, line)) {
            info_.position = memoryStream_.tellg();
            totalBytesRead_ += line.length() + 1; // +1 for newline
            updateStatistics("read", line.length() + 1);
            
            checkStreamState();
            return line;
        }
    } else if (fileStream_.is_open()) {
        if (std::getline(fileStream_, line)) {
            info_.position = fileStream_.tellg();
            totalBytesRead_ += line.length() + 1; // +1 for newline
            updateStatistics("read", line.length() + 1);
            
            checkStreamState();
            return line;
        }
    }
    
    return "";
}

std::string CHTLJSIOStream::readAll() {
    if (info_.state != CHTLJSStreamState::GOOD && info_.state != CHTLJSStreamState::CONNECTED) {
        return "";
    }
    
    if (isMemoryStream()) {
        // 保存当前位置
        size_t currentPos = memoryStream_.tellg();
        
        // 移动到流开始
        memoryStream_.seekg(0, std::ios::beg);
        
        // 读取所有内容
        std::string content = memoryStream_.str();
        
        // 恢复位置
        memoryStream_.seekg(currentPos, std::ios::beg);
        
        totalBytesRead_ += content.length();
        updateStatistics("read", content.length());
        
        return content;
    } else if (fileStream_.is_open()) {
        // 保存当前位置
        size_t currentPos = fileStream_.tellg();
        
        // 移动到文件开始
        fileStream_.seekg(0, std::ios::beg);
        
        // 读取所有内容
        std::stringstream buffer;
        buffer << fileStream_.rdbuf();
        
        // 恢复位置
        fileStream_.seekg(currentPos, std::ios::beg);
        
        std::string content = buffer.str();
        totalBytesRead_ += content.length();
        updateStatistics("read", content.length());
        
        return content;
    }
    
    return "";
}

bool CHTLJSIOStream::readTextAsync(char* buffer, size_t size, std::function<void(size_t)> callback) {
    if (!config_.async) {
        return false;
    }
    
    // 启动异步读取线程
    std::thread([this, buffer, size, callback]() {
        size_t bytesRead = readText(buffer, size);
        callback(bytesRead);
    }).detach();
    
    totalAsyncOperations_++;
    updateStatistics("async");
    return true;
}

size_t CHTLJSIOStream::writeText(const char* data, size_t size) {
    if (info_.state != CHTLJSStreamState::GOOD && info_.state != CHTLJSStreamState::CONNECTED) {
        return 0;
    }
    
    size_t bytesWritten = 0;
    
    if (isMemoryStream()) {
        memoryStream_.write(data, size);
        if (memoryStream_.good()) {
            bytesWritten = size;
        }
    } else if (fileStream_.is_open()) {
        fileStream_.write(data, size);
        if (fileStream_.good()) {
            bytesWritten = size;
        }
    }
    
    if (bytesWritten > 0) {
        info_.position += bytesWritten;
        totalBytesWritten_ += bytesWritten;
        updateStatistics("write", bytesWritten);
        
        if (config_.autoFlush) {
            flush();
        }
        
        return bytesWritten;
    } else {
        setError("Write operation failed");
        return 0;
    }
}

bool CHTLJSIOStream::writeString(const std::string& str) {
    size_t written = writeText(str.c_str(), str.length());
    return written == str.length();
}

bool CHTLJSIOStream::writeLine(const std::string& line) {
    std::string lineWithNewline = line + "\n";
    return writeString(lineWithNewline);
}

bool CHTLJSIOStream::writeTextAsync(const char* data, size_t size, std::function<void(size_t)> callback) {
    if (!config_.async) {
        return false;
    }
    
    // 启动异步写入线程
    std::thread([this, data, size, callback]() {
        size_t bytesWritten = writeText(data, size);
        callback(bytesWritten);
    }).detach();
    
    totalAsyncOperations_++;
    updateStatistics("async");
    return true;
}

bool CHTLJSIOStream::flush() {
    if (info_.state == CHTLJSStreamState::CLOSED) {
        return false;
    }
    
    if (isMemoryStream()) {
        // 内存流不需要刷新
        return true;
    } else if (fileStream_.is_open()) {
        fileStream_.flush();
        return fileStream_.good();
    }
    
    return false;
}

bool CHTLJSIOStream::seek(long offset, int whence) {
    if (info_.state == CHTLJSStreamState::CLOSED) {
        return false;
    }
    
    std::ios_base::seekdir seekDir;
    switch (whence) {
        case 0: seekDir = std::ios_base::beg; break;
        case 1: seekDir = std::ios_base::cur; break;
        case 2: seekDir = std::ios_base::end; break;
        default: seekDir = std::ios_base::beg; break;
    }
    
    if (isMemoryStream()) {
        memoryStream_.seekg(offset, seekDir);
        if (memoryStream_.good()) {
            info_.position = memoryStream_.tellg();
            updateStatistics("operation");
            return true;
        }
    } else if (fileStream_.is_open()) {
        fileStream_.seekg(offset, seekDir);
        if (fileStream_.good()) {
            info_.position = fileStream_.tellg();
            updateStatistics("operation");
            return true;
        }
    }
    
    setError("Seek operation failed");
    return false;
}

size_t CHTLJSIOStream::tell() const {
    return info_.position;
}

bool CHTLJSIOStream::isEOF() const {
    return info_.state == CHTLJSStreamState::EOF;
}

CHTLJSStreamState CHTLJSIOStream::getState() const {
    return info_.state;
}

CHTLJSStreamInfo CHTLJSIOStream::getInfo() const {
    return info_;
}

bool CHTLJSIOStream::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

size_t CHTLJSIOStream::getFileSize(const std::string& path) {
    if (std::filesystem::exists(path)) {
        return std::filesystem::file_size(path);
    }
    return 0;
}

bool CHTLJSIOStream::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

bool CHTLJSIOStream::deleteFile(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (...) {
        return false;
    }
}

bool CHTLJSIOStream::copyFile(const std::string& source, const std::string& destination) {
    try {
        std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

bool CHTLJSIOStream::moveFile(const std::string& source, const std::string& destination) {
    try {
        std::filesystem::rename(source, destination);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> CHTLJSIOStream::listDirectory(const std::string& path) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            files.push_back(entry.path().string());
        }
    } catch (...) {
        // 忽略错误
    }
    
    return files;
}

std::string CHTLJSIOStream::getFileExtension(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.extension().string();
}

std::string CHTLJSIOStream::getFileName(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.filename().string();
}

std::string CHTLJSIOStream::getDirectoryPath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.parent_path().string();
}

std::string CHTLJSIOStream::getAbsolutePath(const std::string& path) {
    try {
        return std::filesystem::absolute(path).string();
    } catch (...) {
        return path;
    }
}

std::string CHTLJSIOStream::normalizePath(const std::string& path) {
    try {
        return std::filesystem::canonical(path).string();
    } catch (...) {
        return path;
    }
}

bool CHTLJSIOStream::isAbsolutePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.is_absolute();
}

bool CHTLJSIOStream::isDirectory(const std::string& path) {
    try {
        return std::filesystem::is_directory(path);
    } catch (...) {
        return false;
    }
}

bool CHTLJSIOStream::isFile(const std::string& path) {
    try {
        return std::filesystem::is_regular_file(path);
    } catch (...) {
        return false;
    }
}

std::string CHTLJSIOStream::getStatistics() const {
    std::stringstream ss;
    ss << "CHTL JS I/O流统计信息:\n";
    ss << "总读取字节数: " << totalBytesRead_ << "\n";
    ss << "总写入字节数: " << totalBytesWritten_ << "\n";
    ss << "总操作次数: " << totalOperations_ << "\n";
    ss << "总错误次数: " << totalErrors_ << "\n";
    ss << "总异步操作次数: " << totalAsyncOperations_ << "\n";
    ss << "\n当前流信息:\n";
    ss << "  名称: " << info_.name << "\n";
    ss << "  路径: " << info_.path << "\n";
    ss << "  URL: " << info_.url << "\n";
    ss << "  类型: " << (int)info_.type << "\n";
    ss << "  状态: " << (int)info_.state << "\n";
    ss << "  位置: " << info_.position << "\n";
    ss << "  大小: " << info_.size << "\n";
    
    return ss.str();
}

void CHTLJSIOStream::clearStatistics() {
    totalBytesRead_ = 0;
    totalBytesWritten_ = 0;
    totalOperations_ = 0;
    totalErrors_ = 0;
    totalAsyncOperations_ = 0;
}

void CHTLJSIOStream::updateStatistics(const std::string& type, size_t value) {
    if (type == "read") {
        totalBytesRead_ += value;
    } else if (type == "write") {
        totalBytesWritten_ += value;
    } else if (type == "operation") {
        totalOperations_++;
    } else if (type == "error") {
        totalErrors_++;
    } else if (type == "async") {
        totalAsyncOperations_++;
    }
}

void CHTLJSIOStream::checkStreamState() {
    if (isMemoryStream()) {
        if (memoryStream_.eof()) {
            info_.state = CHTLJSStreamState::EOF;
        } else if (memoryStream_.fail() || memoryStream_.bad()) {
            info_.state = CHTLJSStreamState::ERROR;
            totalErrors_++;
        } else {
            info_.state = CHTLJSStreamState::GOOD;
        }
    } else if (fileStream_.is_open()) {
        if (fileStream_.eof()) {
            info_.state = CHTLJSStreamState::EOF;
        } else if (fileStream_.fail() || fileStream_.bad()) {
            info_.state = CHTLJSStreamState::ERROR;
            totalErrors_++;
        } else {
            info_.state = CHTLJSStreamState::GOOD;
        }
    }
}

void CHTLJSIOStream::setError(const std::string& error) {
    info_.state = CHTLJSStreamState::ERROR;
    totalErrors_++;
    // 可以在这里添加错误日志记录
}

bool CHTLJSIOStream::isMemoryStream() const {
    return info_.type == CHTLJSStreamType::MEMORY;
}

bool CHTLJSIOStream::isNetworkStream() const {
    return info_.type == CHTLJSStreamType::NETWORK;
}

} // namespace CHTLJS