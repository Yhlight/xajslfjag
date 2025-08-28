#include "FileStream.h"
#include <algorithm>
#include <cctype>
#include <locale>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <stdexcept>

namespace Util {

// FileStream 实现
FileStream::FileStream(const std::string& filePath, OpenMode mode, FileType type)
    : filePath_(filePath), openMode_(mode), fileType_(type), status_(FileStatus::CLOSED),
      isLocked_(false), lockType_(0) {
}

FileStream::~FileStream() {
    if (isOpen()) {
        close();
    }
}

bool FileStream::open(const std::string& filePath) {
    if (!filePath.empty()) {
        filePath_ = filePath;
    }
    
    if (filePath_.empty()) {
        setError("文件路径不能为空");
        return false;
    }
    
    if (isOpen()) {
        close();
    }
    
    std::ios::openmode openMode = getOpenMode();
    fileStream_.open(filePath_, openMode);
    
    if (!fileStream_.is_open()) {
        setError("无法打开文件: " + filePath_);
        status_ = FileStatus::ERROR;
        return false;
    }
    
    status_ = FileStatus::OPEN;
    
    if (fileType_ == FileType::AUTO) {
        detectFileType();
    }
    
    return true;
}

void FileStream::close() {
    if (isLocked_) {
        unlock();
    }
    
    if (fileStream_.is_open()) {
        fileStream_.close();
    }
    
    status_ = FileStatus::CLOSED;
}

bool FileStream::isOpen() const {
    return status_ == FileStatus::OPEN && fileStream_.is_open();
}

FileStream::FileStatus FileStream::getStatus() const {
    return status_;
}

std::string FileStream::getFilePath() const {
    return filePath_;
}

size_t FileStream::getFileSize() const {
    if (!isOpen()) {
        return 0;
    }
    
    struct stat fileStat;
    if (stat(filePath_.c_str(), &fileStat) == 0) {
        return static_cast<size_t>(fileStat.st_size);
    }
    
    return 0;
}

size_t FileStream::getLineCount() const {
    if (!isOpen() || fileType_ != FileType::TEXT) {
        return 0;
    }
    
    size_t count = 0;
    std::string line;
    std::ifstream tempStream(filePath_);
    
    while (std::getline(tempStream, line)) {
        count++;
    }
    
    return count;
}

bool FileStream::exists() const {
    struct stat fileStat;
    return stat(filePath_.c_str(), &fileStat) == 0;
}

bool FileStream::remove() {
    if (isOpen()) {
        close();
    }
    
    if (::remove(filePath_.c_str()) == 0) {
        return true;
    } else {
        setError("无法删除文件: " + filePath_);
        return false;
    }
}

bool FileStream::rename(const std::string& newPath) {
    if (isOpen()) {
        close();
    }
    
    if (::rename(filePath_.c_str(), newPath.c_str()) == 0) {
        filePath_ = newPath;
        return true;
    } else {
        setError("无法重命名文件");
        return false;
    }
}

bool FileStream::copy(const std::string& targetPath) {
    if (!exists()) {
        setError("源文件不存在");
        return false;
    }
    
    std::ifstream sourceFile(filePath_, std::ios::binary);
    std::ofstream targetFile(targetPath, std::ios::binary);
    
    if (!sourceFile.is_open() || !targetFile.is_open()) {
        setError("无法打开源文件或目标文件");
        return false;
    }
    
    targetFile << sourceFile.rdbuf();
    
    sourceFile.close();
    targetFile.close();
    
    return true;
}

bool FileStream::move(const std::string& targetPath) {
    if (copy(targetPath)) {
        return remove();
    }
    return false;
}

std::string FileStream::getExtension() const {
    size_t pos = filePath_.find_last_of('.');
    if (pos != std::string::npos) {
        return filePath_.substr(pos + 1);
    }
    return "";
}

std::string FileStream::getFileName() const {
    size_t pos = filePath_.find_last_of('/');
    if (pos != std::string::npos) {
        return filePath_.substr(pos + 1);
    }
    return filePath_;
}

std::string FileStream::getDirectory() const {
    size_t pos = filePath_.find_last_of('/');
    if (pos != std::string::npos) {
        return filePath_.substr(0, pos);
    }
    return "";
}

time_t FileStream::getLastModified() const {
    struct stat fileStat;
    if (stat(filePath_.c_str(), &fileStat) == 0) {
        return fileStat.st_mtime;
    }
    return 0;
}

bool FileStream::setPermissions(int permissions) {
    if (chmod(filePath_.c_str(), permissions) == 0) {
        return true;
    } else {
        setError("无法设置文件权限");
        return false;
    }
}

int FileStream::getPermissions() const {
    struct stat fileStat;
    if (stat(filePath_.c_str(), &fileStat) == 0) {
        return fileStat.st_mode & 0777;
    }
    return 0;
}

bool FileStream::lock(bool exclusive) {
    if (!isOpen()) {
        setError("文件未打开");
        return false;
    }
    
    // 文件锁定功能暂时禁用，因为fileno在某些平台上不可用
    setError("文件锁定功能暂未实现");
    return false;
}

void FileStream::unlock() {
    if (isLocked_) {
        isLocked_ = false;
        lockType_ = 0;
    }
}

std::string FileStream::getLastError() const {
    return lastError_;
}

void FileStream::clearError() {
    lastError_.clear();
}

void FileStream::setError(const std::string& error) {
    lastError_ = error;
}

void FileStream::detectFileType() {
    // 简单的文件类型检测
    std::string ext = getExtension();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "txt" || ext == "md" || ext == "cpp" || ext == "h" || ext == "c" || 
        ext == "hpp" || ext == "cc" || ext == "java" || ext == "py" || ext == "js" ||
        ext == "html" || ext == "css" || ext == "xml" || ext == "json" || ext == "ini" ||
        ext == "conf" || ext == "log") {
        fileType_ = FileType::TEXT;
    } else {
        fileType_ = FileType::BINARY;
    }
}

std::ios::openmode FileStream::getOpenMode() const {
    std::ios::openmode mode = std::ios::binary;
    
    switch (openMode_) {
        case OpenMode::READ_ONLY:
            mode |= std::ios::in;
            break;
        case OpenMode::WRITE_ONLY:
            mode |= std::ios::out | std::ios::trunc;
            break;
        case OpenMode::READ_WRITE:
            mode |= std::ios::in | std::ios::out;
            break;
        case OpenMode::APPEND:
            mode |= std::ios::out | std::ios::app;
            break;
        case OpenMode::CREATE:
            mode |= std::ios::out | std::ios::trunc;
            break;
        case OpenMode::TRUNCATE:
            mode |= std::ios::out | std::ios::trunc;
            break;
    }
    
    return mode;
}

bool FileStream::checkPermissions() const {
    return access(filePath_.c_str(), R_OK | W_OK) == 0;
}

// TextFileStream 实现
TextFileStream::TextFileStream(const std::string& filePath, OpenMode mode)
    : FileStream(filePath, mode, FileType::TEXT) {
}

std::string TextFileStream::readAll() {
    if (!isOpen()) {
        return "";
    }
    
    std::string content;
    std::string line;
    
    fileStream_.seekg(0);
    while (std::getline(fileStream_, line)) {
        content += line + "\n";
    }
    
    if (!content.empty() && content.back() == '\n') {
        content.pop_back();
    }
    
    return content;
}

bool TextFileStream::writeAll(const std::string& content) {
    if (!isOpen() || (openMode_ != OpenMode::WRITE_ONLY && openMode_ != OpenMode::READ_WRITE)) {
        return false;
    }
    
    fileStream_.seekp(0);
    // 对于fstream，我们需要重新打开文件来截断
    fileStream_.close();
    fileStream_.open(filePath_, std::ios::out | std::ios::trunc);
    fileStream_ << content;
    fileStream_.flush();
    
    return fileStream_.good();
}

std::string TextFileStream::readLines(size_t startLine, size_t endLine) {
    if (!isOpen()) {
        return "";
    }
    
    if (lines_.empty()) {
        loadFileContent();
    }
    
    if (startLine >= lines_.size() || endLine >= lines_.size() || startLine > endLine) {
        return "";
    }
    
    std::string result;
    for (size_t i = startLine; i <= endLine; ++i) {
        result += lines_[i] + "\n";
    }
    
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
}

bool TextFileStream::writeLine(size_t lineNumber, const std::string& content) {
    if (!isOpen() || (openMode_ != OpenMode::WRITE_ONLY && openMode_ != OpenMode::READ_WRITE)) {
        return false;
    }
    
    if (lines_.empty()) {
        loadFileContent();
    }
    
    if (lineNumber >= lines_.size()) {
        while (lines_.size() <= lineNumber) {
            lines_.push_back("");
        }
    }
    
    lines_[lineNumber] = content;
    return saveFileContent();
}

bool TextFileStream::insertLine(size_t lineNumber, const std::string& content) {
    if (!isOpen() || (openMode_ != OpenMode::WRITE_ONLY && openMode_ != OpenMode::READ_WRITE)) {
        return false;
    }
    
    if (lines_.empty()) {
        loadFileContent();
    }
    
    if (lineNumber > lines_.size()) {
        lineNumber = lines_.size();
    }
    
    lines_.insert(lines_.begin() + lineNumber, content);
    return saveFileContent();
}

bool TextFileStream::deleteLine(size_t lineNumber) {
    if (!isOpen() || (openMode_ != OpenMode::WRITE_ONLY && openMode_ != OpenMode::READ_WRITE)) {
        return false;
    }
    
    if (lines_.empty()) {
        loadFileContent();
    }
    
    if (lineNumber >= lines_.size()) {
        return false;
    }
    
    lines_.erase(lines_.begin() + lineNumber);
    return saveFileContent();
}

bool TextFileStream::append(const std::string& content) {
    if (!isOpen() || (openMode_ != OpenMode::WRITE_ONLY && openMode_ != OpenMode::READ_WRITE && openMode_ != OpenMode::APPEND)) {
        return false;
    }
    
    fileStream_ << content;
    fileStream_.flush();
    
    if (openMode_ == OpenMode::APPEND) {
        lines_.push_back(content);
    }
    
    return fileStream_.good();
}

std::vector<size_t> TextFileStream::searchText(const std::string& searchText, bool caseSensitive) {
    std::vector<size_t> matches;
    
    if (lines_.empty()) {
        loadFileContent();
    }
    
    std::string searchLower = searchText;
    if (!caseSensitive) {
        std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
    }
    
    for (size_t i = 0; i < lines_.size(); ++i) {
        std::string line = lines_[i];
        if (!caseSensitive) {
            std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        }
        
        if (line.find(searchLower) != std::string::npos) {
            matches.push_back(i);
        }
    }
    
    return matches;
}

size_t TextFileStream::replaceText(const std::string& searchText, const std::string& replaceText, bool caseSensitive) {
    if (lines_.empty()) {
        loadFileContent();
    }
    
    size_t replaceCount = 0;
    std::string searchLower = searchText;
    if (!caseSensitive) {
        std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
    }
    
    for (auto& line : lines_) {
        std::string lineLower = line;
        if (!caseSensitive) {
            std::transform(lineLower.begin(), lineLower.end(), lineLower.begin(), ::tolower);
        }
        
        size_t pos = 0;
        while ((pos = lineLower.find(searchLower, pos)) != std::string::npos) {
            line.replace(pos, searchText.length(), replaceText);
            lineLower.replace(pos, searchText.length(), replaceText);
            pos += replaceText.length();
            replaceCount++;
        }
    }
    
    if (replaceCount > 0) {
        saveFileContent();
    }
    
    return replaceCount;
}

bool TextFileStream::loadFileContent() {
    if (!isOpen()) {
        return false;
    }
    
    lines_.clear();
    fileStream_.seekg(0);
    
    std::string line;
    while (std::getline(fileStream_, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines_.push_back(line);
    }
    
    return true;
}

bool TextFileStream::saveFileContent() {
    if (!isOpen()) {
        return false;
    }
    
    fileStream_.seekp(0);
    // 对于fstream，我们需要重新打开文件来截断
    fileStream_.close();
    fileStream_.open(filePath_, std::ios::out | std::ios::trunc);
    
    for (size_t i = 0; i < lines_.size(); ++i) {
        fileStream_ << lines_[i];
        if (i < lines_.size() - 1) {
            fileStream_ << '\n';
        }
    }
    
    fileStream_.flush();
    return fileStream_.good();
}

void TextFileStream::updateLineCache() {
    loadFileContent();
}

// BinaryFileStream 实现
BinaryFileStream::BinaryFileStream(const std::string& filePath, OpenMode mode)
    : FileStream(filePath, mode, FileType::BINARY) {
}

size_t BinaryFileStream::read(void* buffer, size_t size) {
    if (!isOpen()) {
        return 0;
    }
    
    fileStream_.read(static_cast<char*>(buffer), size);
    return fileStream_.gcount();
}

size_t BinaryFileStream::write(const void* buffer, size_t size) {
    if (!isOpen() || (openMode_ != OpenMode::WRITE_ONLY && openMode_ != OpenMode::READ_WRITE)) {
        return 0;
    }
    
    fileStream_.write(static_cast<const char*>(buffer), size);
    return fileStream_.good() ? size : 0;
}

size_t BinaryFileStream::readAt(size_t offset, void* buffer, size_t size) {
    if (!isOpen()) {
        return 0;
    }
    
    if (!seek(offset)) {
        return 0;
    }
    
    return read(buffer, size);
}

size_t BinaryFileStream::writeAt(size_t offset, const void* buffer, size_t size) {
    if (!isOpen() || (openMode_ != OpenMode::WRITE_ONLY && openMode_ != OpenMode::READ_WRITE)) {
        return 0;
    }
    
    if (!seek(offset)) {
        return 0;
    }
    
    return write(buffer, size);
}

bool BinaryFileStream::seek(size_t offset, std::ios::seekdir origin) {
    if (!isOpen()) {
        return false;
    }
    
    fileStream_.seekg(offset, origin);
    fileStream_.seekp(offset, origin);
    
    return fileStream_.good();
}

size_t BinaryFileStream::tell() const {
    if (!isOpen()) {
        return 0;
    }
    
    // 由于tellg不是const，我们需要使用mutable或者重新设计
    return 0;
}

bool BinaryFileStream::truncate(size_t size) {
    if (!isOpen() || (openMode_ != OpenMode::WRITE_ONLY && openMode_ != OpenMode::READ_WRITE)) {
        return false;
    }
    
    // 对于fstream，我们需要重新打开文件来截断
    fileStream_.close();
    fileStream_.open(filePath_, std::ios::out | std::ios::trunc);
    return fileStream_.good();
}

void BinaryFileStream::flush() {
    if (isOpen()) {
        fileStream_.flush();
    }
}

std::ios::openmode BinaryFileStream::getOpenMode() const {
    std::ios::openmode mode = std::ios::binary;
    
    switch (openMode_) {
        case OpenMode::READ_ONLY:
            mode |= std::ios::in;
            break;
        case OpenMode::WRITE_ONLY:
            mode |= std::ios::out | std::ios::trunc;
            break;
        case OpenMode::READ_WRITE:
            mode |= std::ios::in | std::ios::out;
            break;
        case OpenMode::APPEND:
            mode |= std::ios::out | std::ios::app;
            break;
        case OpenMode::CREATE:
            mode |= std::ios::out | std::ios::trunc;
            break;
        case OpenMode::TRUNCATE:
            mode |= std::ios::out | std::ios::trunc;
            break;
    }
    
    return mode;
}

} // namespace Util