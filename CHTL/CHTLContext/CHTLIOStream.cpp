#include "CHTLIOStream.h"
#include <algorithm>
#include <sstream>
#include <cstring>

namespace CHTL {

CHTLIOStream::CHTLIOStream()
    : totalBytesRead_(0), totalBytesWritten_(0), totalOperations_(0), totalErrors_(0) {
    info_.state = StreamState::CLOSED;
}

CHTLIOStream::~CHTLIOStream() {
    if (fileStream_.is_open()) {
        closeFile();
    }
}

bool CHTLIOStream::openFile(const std::string& path, const std::string& mode, const StreamConfig& config) {
    if (fileStream_.is_open()) {
        closeFile();
    }
    
    config_ = config;
    info_.path = path;
    info_.name = getFileName(path);
    info_.type = config.type;
    info_.state = StreamState::CLOSED;
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
        info_.state = StreamState::GOOD;
        
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

bool CHTLIOStream::closeFile() {
    if (!fileStream_.is_open()) {
        return false;
    }
    
    flush();
    fileStream_.close();
    info_.state = StreamState::CLOSED;
    info_.position = 0;
    
    updateStatistics("operation");
    return true;
}

size_t CHTLIOStream::readText(char* buffer, size_t size) {
    if (!fileStream_.is_open() || info_.state != StreamState::GOOD) {
        return 0;
    }
    
    fileStream_.read(buffer, size);
    size_t bytesRead = fileStream_.gcount();
    
    if (bytesRead > 0) {
        info_.position += bytesRead;
        totalBytesRead_ += bytesRead;
        updateStatistics("read", bytesRead);
        
        checkStreamState();
    }
    
    return bytesRead;
}

std::string CHTLIOStream::readLine() {
    if (!fileStream_.is_open() || info_.state != StreamState::GOOD) {
        return "";
    }
    
    std::string line;
    if (std::getline(fileStream_, line)) {
        info_.position = fileStream_.tellg();
        totalBytesRead_ += line.length() + 1; // +1 for newline
        updateStatistics("read", line.length() + 1);
        
        checkStreamState();
        return line;
    }
    
    return "";
}

std::string CHTLIOStream::readAll() {
    if (!fileStream_.is_open() || info_.state != StreamState::GOOD) {
        return "";
    }
    
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

size_t CHTLIOStream::writeText(const char* data, size_t size) {
    if (!fileStream_.is_open() || info_.state != StreamState::GOOD) {
        return 0;
    }
    
    fileStream_.write(data, size);
    
    if (fileStream_.good()) {
        info_.position += size;
        totalBytesWritten_ += size;
        updateStatistics("write", size);
        
        if (config_.autoFlush) {
            flush();
        }
        
        return size;
    } else {
        setError("Write operation failed");
        return 0;
    }
}

bool CHTLIOStream::writeString(const std::string& str) {
    size_t written = writeText(str.c_str(), str.length());
    return written == str.length();
}

bool CHTLIOStream::writeLine(const std::string& line) {
    std::string lineWithNewline = line + "\n";
    return writeString(lineWithNewline);
}

bool CHTLIOStream::flush() {
    if (!fileStream_.is_open()) {
        return false;
    }
    
    fileStream_.flush();
    return fileStream_.good();
}

bool CHTLIOStream::seek(long offset, int whence) {
    if (!fileStream_.is_open()) {
        return false;
    }
    
    std::ios_base::seekdir seekDir;
    switch (whence) {
        case 0: seekDir = std::ios_base::beg; break;
        case 1: seekDir = std::ios_base::cur; break;
        case 2: seekDir = std::ios_base::end; break;
        default: seekDir = std::ios_base::beg; break;
    }
    
    fileStream_.seekg(offset, seekDir);
    
    if (fileStream_.good()) {
        info_.position = fileStream_.tellg();
        updateStatistics("operation");
        return true;
    } else {
        setError("Seek operation failed");
        return false;
    }
}

size_t CHTLIOStream::tell() const {
    return info_.position;
}

bool CHTLIOStream::isEOF() const {
    return info_.state == StreamState::EOF;
}

StreamState CHTLIOStream::getState() const {
    return info_.state;
}

StreamInfo CHTLIOStream::getInfo() const {
    return info_;
}

bool CHTLIOStream::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

size_t CHTLIOStream::getFileSize(const std::string& path) {
    if (std::filesystem::exists(path)) {
        return std::filesystem::file_size(path);
    }
    return 0;
}

bool CHTLIOStream::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

bool CHTLIOStream::deleteFile(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (...) {
        return false;
    }
}

bool CHTLIOStream::copyFile(const std::string& source, const std::string& destination) {
    try {
        std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

bool CHTLIOStream::moveFile(const std::string& source, const std::string& destination) {
    try {
        std::filesystem::rename(source, destination);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> CHTLIOStream::listDirectory(const std::string& path) {
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

std::string CHTLIOStream::getFileExtension(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.extension().string();
}

std::string CHTLIOStream::getFileName(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.filename().string();
}

std::string CHTLIOStream::getDirectoryPath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.parent_path().string();
}

std::string CHTLIOStream::getAbsolutePath(const std::string& path) {
    try {
        return std::filesystem::absolute(path).string();
    } catch (...) {
        return path;
    }
}

std::string CHTLIOStream::normalizePath(const std::string& path) {
    try {
        return std::filesystem::canonical(path).string();
    } catch (...) {
        return path;
    }
}

bool CHTLIOStream::isAbsolutePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.is_absolute();
}

bool CHTLIOStream::isDirectory(const std::string& path) {
    try {
        return std::filesystem::is_directory(path);
    } catch (...) {
        return false;
    }
}

bool CHTLIOStream::isFile(const std::string& path) {
    try {
        return std::filesystem::is_regular_file(path);
    } catch (...) {
        return false;
    }
}

std::string CHTLIOStream::getStatistics() const {
    std::stringstream ss;
    ss << "CHTL I/O流统计信息:\n";
    ss << "总读取字节数: " << totalBytesRead_ << "\n";
    ss << "总写入字节数: " << totalBytesWritten_ << "\n";
    ss << "总操作次数: " << totalOperations_ << "\n";
    ss << "总错误次数: " << totalErrors_ << "\n";
    ss << "\n当前流信息:\n";
    ss << "  名称: " << info_.name << "\n";
    ss << "  路径: " << info_.path << "\n";
    ss << "  类型: " << (int)info_.type << "\n";
    ss << "  状态: " << (int)info_.state << "\n";
    ss << "  位置: " << info_.position << "\n";
    ss << "  大小: " << info_.size << "\n";
    
    return ss.str();
}

void CHTLIOStream::clearStatistics() {
    totalBytesRead_ = 0;
    totalBytesWritten_ = 0;
    totalOperations_ = 0;
    totalErrors_ = 0;
}

void CHTLIOStream::updateStatistics(const std::string& type, size_t value) {
    if (type == "read") {
        totalBytesRead_ += value;
    } else if (type == "write") {
        totalBytesWritten_ += value;
    } else if (type == "operation") {
        totalOperations_++;
    } else if (type == "error") {
        totalErrors_++;
    }
}

void CHTLIOStream::checkStreamState() {
    if (fileStream_.eof()) {
        info_.state = StreamState::EOF;
    } else if (fileStream_.fail() || fileStream_.bad()) {
        info_.state = StreamState::ERROR;
        totalErrors_++;
    } else {
        info_.state = StreamState::GOOD;
    }
}

void CHTLIOStream::setError(const std::string& error) {
    info_.state = StreamState::ERROR;
    totalErrors_++;
    // 可以在这里添加错误日志记录
}

} // namespace CHTL