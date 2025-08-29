#include "CHTLFileSystem.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>
#include <cstring>
#include <thread>
#include <regex>

namespace CHTL {

namespace fs = std::filesystem;

// PathUtil 实现

std::string PathUtil::normalize(const std::string& path) {
    return fs::path(path).lexically_normal().string();
}

std::string PathUtil::absolute(const std::string& path) {
    return fs::absolute(path).string();
}

std::string PathUtil::relative(const std::string& path, const std::string& base) {
    return fs::relative(path, base).string();
}

std::string PathUtil::join(const std::string& path1, const std::string& path2) {
    return (fs::path(path1) / path2).string();
}

std::string PathUtil::parent(const std::string& path) {
    return fs::path(path).parent_path().string();
}

std::string PathUtil::filename(const std::string& path) {
    return fs::path(path).filename().string();
}

std::string PathUtil::basename(const std::string& path) {
    return fs::path(path).stem().string();
}

std::string PathUtil::extension(const std::string& path) {
    return fs::path(path).extension().string();
}

std::string PathUtil::replaceExtension(const std::string& path, const std::string& newExt) {
    fs::path p(path);
    p.replace_extension(newExt);
    return p.string();
}

bool PathUtil::isAbsolute(const std::string& path) {
    return fs::path(path).is_absolute();
}

bool PathUtil::isRelative(const std::string& path) {
    return fs::path(path).is_relative();
}

// FileSystem 实现

bool FileSystem::exists(const std::string& path) {
    std::error_code ec;
    return fs::exists(path, ec);
}

bool FileSystem::isFile(const std::string& path) {
    std::error_code ec;
    return fs::is_regular_file(path, ec);
}

bool FileSystem::isDirectory(const std::string& path) {
    std::error_code ec;
    return fs::is_directory(path, ec);
}

bool FileSystem::isSymlink(const std::string& path) {
    std::error_code ec;
    return fs::is_symlink(path, ec);
}

std::optional<FileInfo> FileSystem::getFileInfo(const std::string& path) {
    std::error_code ec;
    if (!fs::exists(path, ec)) {
        return std::nullopt;
    }
    
    FileInfo info;
    info.path = path;
    info.name = PathUtil::filename(path);
    
    auto status = fs::status(path, ec);
    if (ec) {
        return std::nullopt;
    }
    
    if (fs::is_regular_file(status)) {
        info.type = FileType::Regular;
        info.size = fs::file_size(path, ec);
    } else if (fs::is_directory(status)) {
        info.type = FileType::Directory;
        info.size = 0;
    } else if (fs::is_symlink(status)) {
        info.type = FileType::Symlink;
        info.size = 0;
    } else {
        info.type = FileType::Other;
        info.size = 0;
    }
    
    auto ftime = fs::last_write_time(path, ec);
    if (!ec) {
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        info.modificationTime = std::chrono::system_clock::to_time_t(sctp);
    }
    
    auto perms = status.permissions();
    info.isReadable = (perms & fs::perms::owner_read) != fs::perms::none;
    info.isWritable = (perms & fs::perms::owner_write) != fs::perms::none;
    info.isExecutable = (perms & fs::perms::owner_exec) != fs::perms::none;
    
    return info;
}

size_t FileSystem::getFileSize(const std::string& path) {
    std::error_code ec;
    auto size = fs::file_size(path, ec);
    return ec ? 0 : size;
}

std::time_t FileSystem::getModificationTime(const std::string& path) {
    std::error_code ec;
    auto ftime = fs::last_write_time(path, ec);
    if (ec) {
        return 0;
    }
    
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    return std::chrono::system_clock::to_time_t(sctp);
}

bool FileSystem::createDirectory(const std::string& path) {
    std::error_code ec;
    return fs::create_directory(path, ec);
}

bool FileSystem::createDirectories(const std::string& path) {
    std::error_code ec;
    return fs::create_directories(path, ec);
}

bool FileSystem::removeFile(const std::string& path) {
    std::error_code ec;
    return fs::remove(path, ec);
}

bool FileSystem::removeDirectory(const std::string& path) {
    std::error_code ec;
    return fs::remove(path, ec);
}

bool FileSystem::removeAll(const std::string& path) {
    std::error_code ec;
    fs::remove_all(path, ec);
    return !ec;
}

bool FileSystem::copyFile(const std::string& from, const std::string& to, bool overwrite) {
    std::error_code ec;
    auto options = overwrite ? fs::copy_options::overwrite_existing : fs::copy_options::none;
    fs::copy_file(from, to, options, ec);
    return !ec;
}

bool FileSystem::moveFile(const std::string& from, const std::string& to) {
    std::error_code ec;
    fs::rename(from, to, ec);
    return !ec;
}

std::vector<FileInfo> FileSystem::listDirectory(const std::string& path) {
    std::vector<FileInfo> results;
    std::error_code ec;
    
    for (const auto& entry : fs::directory_iterator(path, ec)) {
        if (auto info = getFileInfo(entry.path().string())) {
            results.push_back(*info);
        }
    }
    
    return results;
}

std::vector<FileInfo> FileSystem::listDirectoryRecursive(const std::string& path) {
    std::vector<FileInfo> results;
    std::error_code ec;
    
    for (const auto& entry : fs::recursive_directory_iterator(path, ec)) {
        if (auto info = getFileInfo(entry.path().string())) {
            results.push_back(*info);
        }
    }
    
    return results;
}

std::vector<std::string> FileSystem::glob(const std::string& pattern) {
    std::vector<std::string> results;
    
    // 简单的通配符实现
    fs::path patternPath(pattern);
    fs::path parent = patternPath.parent_path();
    std::string filename = patternPath.filename().string();
    
    // 将通配符转换为正则表达式
    std::string regex = filename;
    size_t pos = 0;
    while ((pos = regex.find('*', pos)) != std::string::npos) {
        regex.replace(pos, 1, ".*");
        pos += 2;
    }
    pos = 0;
    while ((pos = regex.find('?', pos)) != std::string::npos) {
        regex.replace(pos, 1, ".");
        pos += 1;
    }
    
    std::regex fileRegex(regex);
    
    std::error_code ec;
    for (const auto& entry : fs::directory_iterator(parent.empty() ? "." : parent, ec)) {
        if (std::regex_match(entry.path().filename().string(), fileRegex)) {
            results.push_back(entry.path().string());
        }
    }
    
    return results;
}

std::string FileSystem::getCurrentDirectory() {
    return fs::current_path().string();
}

bool FileSystem::setCurrentDirectory(const std::string& path) {
    std::error_code ec;
    fs::current_path(path, ec);
    return !ec;
}

std::string FileSystem::getTempDirectory() {
    return fs::temp_directory_path().string();
}

std::string FileSystem::createTempFile(const std::string& prefix) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 999999);
    
    std::string tempDir = getTempDirectory();
    std::string filename;
    
    do {
        filename = PathUtil::join(tempDir, prefix + "_" + std::to_string(dis(gen)));
    } while (exists(filename));
    
    // 创建空文件
    std::ofstream(filename).close();
    
    return filename;
}

std::string FileSystem::createTempDirectory(const std::string& prefix) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 999999);
    
    std::string tempDir = getTempDirectory();
    std::string dirname;
    
    do {
        dirname = PathUtil::join(tempDir, prefix + "_" + std::to_string(dis(gen)));
    } while (exists(dirname));
    
    createDirectory(dirname);
    
    return dirname;
}

// File 实现

std::optional<std::string> File::readToString(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::optional<std::vector<uint8_t>> File::readToBytes(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return std::nullopt;
    }
    
    size_t size = file.tellg();
    file.seekg(0);
    
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    return data;
}

std::optional<std::vector<std::string>> File::readLines(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return std::nullopt;
    }
    
    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

bool File::writeString(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool File::writeBytes(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

bool File::writeLines(const std::string& path, const std::vector<std::string>& lines) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& line : lines) {
        file << line << '\n';
    }
    
    return file.good();
}

bool File::appendString(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool File::appendLines(const std::string& path, const std::vector<std::string>& lines) {
    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& line : lines) {
        file << line << '\n';
    }
    
    return file.good();
}

// FileStream 实现

FileStream::FileStream(const std::string& path, std::ios::openmode mode) {
    open(path, mode);
}

FileStream::~FileStream() {
    close();
}

bool FileStream::open(const std::string& path, std::ios::openmode mode) {
    close();
    
    stream_ = std::make_unique<std::fstream>();
    stream_->open(path, mode);
    
    if (stream_->is_open()) {
        path_ = path;
        mode_ = mode;
        return true;
    }
    
    stream_.reset();
    return false;
}

void FileStream::close() {
    if (stream_ && stream_->is_open()) {
        stream_->close();
    }
    stream_.reset();
    path_.clear();
}

bool FileStream::isOpen() const {
    return stream_ && stream_->is_open();
}

bool FileStream::eof() const {
    return stream_ ? stream_->eof() : true;
}

std::streampos FileStream::tell() {
    return stream_ ? stream_->tellg() : -1;
}

void FileStream::seek(std::streampos pos) {
    if (stream_) {
        stream_->seekg(pos);
        stream_->seekp(pos);
    }
}

void FileStream::seekFromBegin(std::streamoff offset) {
    if (stream_) {
        stream_->seekg(offset, std::ios::beg);
        stream_->seekp(offset, std::ios::beg);
    }
}

void FileStream::seekFromEnd(std::streamoff offset) {
    if (stream_) {
        stream_->seekg(offset, std::ios::end);
        stream_->seekp(offset, std::ios::end);
    }
}

void FileStream::seekFromCurrent(std::streamoff offset) {
    if (stream_) {
        stream_->seekg(offset, std::ios::cur);
        stream_->seekp(offset, std::ios::cur);
    }
}

std::string FileStream::readAll() {
    if (!stream_) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << stream_->rdbuf();
    return buffer.str();
}

std::string FileStream::read(size_t size) {
    if (!stream_) {
        return "";
    }
    
    std::string result(size, '\0');
    stream_->read(&result[0], size);
    result.resize(stream_->gcount());
    return result;
}

std::string FileStream::readLine() {
    if (!stream_) {
        return "";
    }
    
    std::string line;
    std::getline(*stream_, line);
    return line;
}

size_t FileStream::readBytes(void* buffer, size_t size) {
    if (!stream_) {
        return 0;
    }
    
    stream_->read(static_cast<char*>(buffer), size);
    return stream_->gcount();
}

bool FileStream::write(const std::string& data) {
    if (!stream_) {
        return false;
    }
    
    *stream_ << data;
    return stream_->good();
}

bool FileStream::writeLine(const std::string& line) {
    if (!stream_) {
        return false;
    }
    
    *stream_ << line << '\n';
    return stream_->good();
}

bool FileStream::writeBytes(const void* data, size_t size) {
    if (!stream_) {
        return false;
    }
    
    stream_->write(static_cast<const char*>(data), size);
    return stream_->good();
}

void FileStream::flush() {
    if (stream_) {
        stream_->flush();
    }
}

size_t FileStream::size() {
    if (!stream_) {
        return 0;
    }
    
    auto currentPos = tell();
    seekFromEnd(0);
    size_t fileSize = tell();
    seek(currentPos);
    
    return fileSize;
}

// FileWatcher::Impl (简化实现)
class FileWatcher::Impl {
public:
    std::vector<std::string> paths;
    ChangeCallback callback;
    bool watching = false;
};

FileWatcher::FileWatcher() : pImpl(std::make_unique<Impl>()) {}

FileWatcher::~FileWatcher() {
    stop();
}

bool FileWatcher::addPath(const std::string& path, bool recursive) {
    if (!FileSystem::exists(path)) {
        return false;
    }
    
    pImpl->paths.push_back(path);
    return true;
}

void FileWatcher::removePath(const std::string& path) {
    auto it = std::find(pImpl->paths.begin(), pImpl->paths.end(), path);
    if (it != pImpl->paths.end()) {
        pImpl->paths.erase(it);
    }
}

void FileWatcher::setCallback(ChangeCallback callback) {
    pImpl->callback = callback;
}

bool FileWatcher::start() {
    if (pImpl->watching || pImpl->paths.empty() || !pImpl->callback) {
        return false;
    }
    
    pImpl->watching = true;
    // 实际实现需要使用平台特定的文件监视API
    return true;
}

void FileWatcher::stop() {
    pImpl->watching = false;
}

bool FileWatcher::isWatching() const {
    return pImpl->watching;
}

// FileLock::Impl (简化实现)
class FileLock::Impl {
public:
    std::string path;
    bool locked = false;
};

FileLock::FileLock(const std::string& path) : pImpl(std::make_unique<Impl>()) {
    pImpl->path = path;
}

FileLock::~FileLock() {
    unlock();
}

bool FileLock::tryLock() {
    if (pImpl->locked) {
        return true;
    }
    
    // 简化实现：使用锁文件
    std::string lockFile = pImpl->path + ".lock";
    if (FileSystem::exists(lockFile)) {
        return false;
    }
    
    std::ofstream lock(lockFile);
    if (lock.is_open()) {
        lock << std::chrono::system_clock::now().time_since_epoch().count();
        pImpl->locked = true;
        return true;
    }
    
    return false;
}

bool FileLock::lock() {
    while (!tryLock()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return true;
}

void FileLock::unlock() {
    if (pImpl->locked) {
        std::string lockFile = pImpl->path + ".lock";
        FileSystem::removeFile(lockFile);
        pImpl->locked = false;
    }
}

bool FileLock::isLocked() const {
    return pImpl->locked;
}

} // namespace CHTL