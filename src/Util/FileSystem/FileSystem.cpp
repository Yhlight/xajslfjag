#include "FileSystem.h"
#include <filesystem>
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#endif

namespace CHTL {
namespace Util {

// ========== Path 实现 ==========

#ifdef _WIN32
const char Path::SEPARATOR = '\\';
#else
const char Path::SEPARATOR = '/';
#endif

std::string Path::normalize(const std::string& path) {
    if (path.empty()) return ".";
    
    std::filesystem::path fsPath(path);
    std::filesystem::path normalizedPath = fsPath.lexically_normal();
    return normalizedPath.string();
}

std::string Path::join(const std::string& path1, const std::string& path2) {
    if (path1.empty()) return path2;
    if (path2.empty()) return path1;
    
    std::filesystem::path fsPath1(path1);
    std::filesystem::path fsPath2(path2);
    std::filesystem::path joinedPath = fsPath1 / fsPath2;
    return joinedPath.string();
}

std::string Path::join(const std::vector<std::string>& paths) {
    if (paths.empty()) return "";
    
    std::filesystem::path result(paths[0]);
    for (size_t i = 1; i < paths.size(); ++i) {
        result /= paths[i];
    }
    return result.string();
}

std::string Path::getParent(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.parent_path().string();
}

std::string Path::getFileName(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.filename().string();
}

std::string Path::getBaseName(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.stem().string();
}

std::string Path::getExtension(const std::string& path) {
    std::filesystem::path fsPath(path);
    std::string ext = fsPath.extension().string();
    if (!ext.empty() && ext[0] == '.') {
        ext = ext.substr(1); // 移除前导点
    }
    return ext;
}

bool Path::isAbsolute(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.is_absolute();
}

bool Path::isRelative(const std::string& path) {
    return !isAbsolute(path);
}

std::string Path::toAbsolute(const std::string& path) {
    std::filesystem::path fsPath(path);
    return std::filesystem::absolute(fsPath).string();
}

std::string Path::toRelative(const std::string& path, const std::string& base) {
    std::filesystem::path fsPath(path);
    std::filesystem::path basePath(base);
    
    try {
        std::filesystem::path relativePath = std::filesystem::relative(fsPath, basePath);
        return relativePath.string();
    } catch (const std::exception&) {
        return path; // 如果无法转换，返回原路径
    }
}

std::string Path::getCurrentDirectory() {
    return std::filesystem::current_path().string();
}

std::string Path::getTempDirectory() {
    return std::filesystem::temp_directory_path().string();
}

std::string Path::getHomeDirectory() {
#ifdef _WIN32
    const char* homeDir = std::getenv("USERPROFILE");
    if (!homeDir) {
        homeDir = std::getenv("HOMEDRIVE");
        const char* homePath = std::getenv("HOMEPATH");
        if (homeDir && homePath) {
            return std::string(homeDir) + std::string(homePath);
        }
    }
    return homeDir ? std::string(homeDir) : "";
#else
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) {
        struct passwd* pw = getpwuid(getuid());
        if (pw) {
            homeDir = pw->pw_dir;
        }
    }
    return homeDir ? std::string(homeDir) : "";
#endif
}

std::vector<std::string> Path::splitPath(const std::string& path) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    while (std::getline(ss, part, SEPARATOR)) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

// ========== FileSystem 实现 ==========

bool FileSystem::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileSystem::isFile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

bool FileSystem::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

bool FileSystem::isSymbolicLink(const std::string& path) {
    return std::filesystem::is_symlink(path);
}

FileInfo FileSystem::getFileInfo(const std::string& path) {
    FileInfo info;
    info.path = path;
    info.name = Path::getFileName(path);
    info.extension = Path::getExtension(path);
    
    if (!exists(path)) {
        return info;
    }
    
    std::filesystem::path fsPath(path);
    std::error_code ec;
    
    // 文件类型
    if (std::filesystem::is_regular_file(fsPath, ec)) {
        info.type = FileType::REGULAR_FILE;
    } else if (std::filesystem::is_directory(fsPath, ec)) {
        info.type = FileType::DIRECTORY;
    } else if (std::filesystem::is_symlink(fsPath, ec)) {
        info.type = FileType::SYMBOLIC_LINK;
    } else {
        info.type = FileType::UNKNOWN;
    }
    
    // 文件大小
    if (info.type == FileType::REGULAR_FILE) {
        info.size = std::filesystem::file_size(fsPath, ec);
        if (ec) info.size = 0;
    }
    
    // 修改时间
    auto lastWriteTime = std::filesystem::last_write_time(fsPath, ec);
    if (!ec) {
        info.lastModified = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastWriteTime - std::filesystem::file_time_type::clock::now() + 
            std::chrono::system_clock::now());
    }
    
    // 权限（简化实现）
    auto status = std::filesystem::status(fsPath, ec);
    if (!ec) {
        auto perms = status.permissions();
        info.permissions = static_cast<int>(perms) & 0777;
    }
    
    return info;
}

size_t FileSystem::getFileSize(const std::string& path) {
    std::error_code ec;
    auto size = std::filesystem::file_size(path, ec);
    return ec ? 0 : size;
}

std::chrono::system_clock::time_point FileSystem::getLastModified(const std::string& path) {
    std::error_code ec;
    auto lastWriteTime = std::filesystem::last_write_time(path, ec);
    if (ec) {
        return std::chrono::system_clock::time_point{};
    }
    
    return std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        lastWriteTime - std::filesystem::file_time_type::clock::now() + 
        std::chrono::system_clock::now());
}

bool FileSystem::createDirectory(const std::string& path, bool recursive) {
    std::error_code ec;
    if (recursive) {
        return std::filesystem::create_directories(path, ec);
    } else {
        return std::filesystem::create_directory(path, ec);
    }
}

bool FileSystem::createFile(const std::string& path) {
    if (exists(path)) {
        return false;
    }
    
    std::ofstream file(path);
    return file.is_open();
}

bool FileSystem::deleteFile(const std::string& path) {
    std::error_code ec;
    return std::filesystem::remove(path, ec);
}

bool FileSystem::deleteDirectory(const std::string& path, bool recursive) {
    std::error_code ec;
    if (recursive) {
        return std::filesystem::remove_all(path, ec) > 0;
    } else {
        return std::filesystem::remove(path, ec);
    }
}

bool FileSystem::copyFile(const std::string& source, const std::string& destination) {
    std::error_code ec;
    std::filesystem::copy_file(source, destination, 
                              std::filesystem::copy_options::overwrite_existing, ec);
    return !ec;
}

bool FileSystem::copyDirectory(const std::string& source, const std::string& destination) {
    std::error_code ec;
    std::filesystem::copy(source, destination, 
                         std::filesystem::copy_options::recursive |
                         std::filesystem::copy_options::overwrite_existing, ec);
    return !ec;
}

bool FileSystem::move(const std::string& source, const std::string& destination) {
    std::error_code ec;
    std::filesystem::rename(source, destination, ec);
    return !ec;
}

bool FileSystem::rename(const std::string& oldPath, const std::string& newPath) {
    return move(oldPath, newPath);
}

std::vector<std::string> FileSystem::listDirectory(const std::string& path) {
    std::vector<std::string> result;
    std::error_code ec;
    
    for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
        if (!ec) {
            result.push_back(entry.path().string());
        }
    }
    
    return result;
}

std::vector<FileInfo> FileSystem::listDirectoryDetailed(const std::string& path) {
    std::vector<FileInfo> result;
    std::error_code ec;
    
    for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
        if (!ec) {
            result.push_back(getFileInfo(entry.path().string()));
        }
    }
    
    return result;
}

std::vector<std::string> FileSystem::listDirectoryRecursive(const std::string& path) {
    std::vector<std::string> result;
    std::error_code ec;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path, ec)) {
        if (!ec) {
            result.push_back(entry.path().string());
        }
    }
    
    return result;
}

std::vector<std::string> FileSystem::findFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> result;
    
    // 简化的模式匹配（只支持 * 通配符）
    auto files = listDirectoryRecursive(directory);
    for (const auto& file : files) {
        if (isFile(file)) {
            std::string fileName = Path::getFileName(file);
            if (pattern == "*" || fileName.find(pattern) != std::string::npos) {
                result.push_back(file);
            }
        }
    }
    
    return result;
}

std::vector<std::string> FileSystem::findFiles(const std::string& directory,
                                              std::function<bool(const FileInfo&)> predicate) {
    std::vector<std::string> result;
    
    auto files = listDirectoryRecursive(directory);
    for (const auto& file : files) {
        if (isFile(file)) {
            FileInfo info = getFileInfo(file);
            if (predicate(info)) {
                result.push_back(file);
            }
        }
    }
    
    return result;
}

size_t FileSystem::getAvailableSpace(const std::string& path) {
    std::error_code ec;
    auto space = std::filesystem::space(path, ec);
    return ec ? 0 : space.available;
}

size_t FileSystem::getTotalSpace(const std::string& path) {
    std::error_code ec;
    auto space = std::filesystem::space(path, ec);
    return ec ? 0 : space.capacity;
}

bool FileSystem::setPermissions(const std::string& path, int permissions) {
    std::error_code ec;
    std::filesystem::permissions(path, static_cast<std::filesystem::perms>(permissions), ec);
    return !ec;
}

int FileSystem::getPermissions(const std::string& path) {
    std::error_code ec;
    auto status = std::filesystem::status(path, ec);
    if (ec) return 0;
    
    return static_cast<int>(status.permissions()) & 0777;
}

// ========== FileReader 实现 ==========

FileReader::FileReader(const std::string& filePath)
    : m_filePath(filePath)
    , m_stream(nullptr) {
}

FileReader::~FileReader() {
    close();
}

bool FileReader::open() {
    if (m_stream) {
        close();
    }
    
    m_stream = std::make_unique<std::ifstream>(m_filePath, std::ios::binary);
    return m_stream->is_open();
}

void FileReader::close() {
    if (m_stream) {
        m_stream->close();
        m_stream.reset();
    }
}

bool FileReader::isOpen() const {
    return m_stream && m_stream->is_open();
}

bool FileReader::isEOF() const {
    return !m_stream || m_stream->eof();
}

std::string FileReader::readAll() {
    if (!isOpen()) return "";
    
    m_stream->seekg(0, std::ios::end);
    size_t size = m_stream->tellg();
    m_stream->seekg(0, std::ios::beg);
    
    std::string content(size, '\0');
    m_stream->read(&content[0], size);
    
    return content;
}

std::string FileReader::readLine() {
    if (!isOpen()) return "";
    
    std::string line;
    std::getline(*m_stream, line);
    return line;
}

std::string FileReader::readBytes(size_t count) {
    if (!isOpen()) return "";
    
    std::string data(count, '\0');
    m_stream->read(&data[0], count);
    size_t actualRead = m_stream->gcount();
    data.resize(actualRead);
    
    return data;
}

char FileReader::readChar() {
    if (!isOpen()) return '\0';
    
    char c;
    m_stream->read(&c, 1);
    return m_stream->gcount() > 0 ? c : '\0';
}

size_t FileReader::getPosition() const {
    if (!isOpen()) return 0;
    return m_stream->tellg();
}

bool FileReader::setPosition(size_t position) {
    if (!isOpen()) return false;
    
    m_stream->seekg(position, std::ios::beg);
    return !m_stream->fail();
}

size_t FileReader::getSize() const {
    return FileSystem::getFileSize(m_filePath);
}

// ========== FileWriter 实现 ==========

FileWriter::FileWriter(const std::string& filePath, WriteMode mode)
    : m_filePath(filePath)
    , m_mode(mode)
    , m_stream(nullptr) {
}

FileWriter::~FileWriter() {
    close();
}

bool FileWriter::open() {
    if (m_stream) {
        close();
    }
    
    std::ios::openmode openMode = std::ios::binary;
    if (m_mode == WriteMode::APPEND) {
        openMode |= std::ios::app;
    } else {
        openMode |= std::ios::trunc;
    }
    
    m_stream = std::make_unique<std::ofstream>(m_filePath, openMode);
    return m_stream->is_open();
}

void FileWriter::close() {
    if (m_stream) {
        m_stream->close();
        m_stream.reset();
    }
}

bool FileWriter::isOpen() const {
    return m_stream && m_stream->is_open();
}

bool FileWriter::write(const std::string& content) {
    if (!isOpen()) return false;
    
    *m_stream << content;
    return !m_stream->fail();
}

bool FileWriter::writeLine(const std::string& line) {
    return write(line + "\n");
}

bool FileWriter::writeBytes(const char* data, size_t size) {
    if (!isOpen()) return false;
    
    m_stream->write(data, size);
    return !m_stream->fail();
}

bool FileWriter::writeChar(char c) {
    if (!isOpen()) return false;
    
    m_stream->put(c);
    return !m_stream->fail();
}

void FileWriter::flush() {
    if (isOpen()) {
        m_stream->flush();
    }
}

size_t FileWriter::getPosition() const {
    if (!isOpen()) return 0;
    return m_stream->tellp();
}

// ========== TemporaryFile 实现 ==========

TemporaryFile::TemporaryFile(const std::string& prefix, const std::string& suffix)
    : m_deleted(false) {
    std::string tempDir = Path::getTempDirectory();
    m_path = Path::join(tempDir, generateUniqueName(prefix, suffix));
    
    // 创建临时文件
    std::ofstream file(m_path);
    file.close();
}

TemporaryFile::~TemporaryFile() {
    if (!m_deleted) {
        remove();
    }
}

const std::string& TemporaryFile::getPath() const {
    return m_path;
}

bool TemporaryFile::writeContent(const std::string& content) {
    FileWriter writer(m_path);
    if (!writer.open()) return false;
    
    return writer.write(content);
}

std::string TemporaryFile::readContent() {
    FileReader reader(m_path);
    if (!reader.open()) return "";
    
    return reader.readAll();
}

void TemporaryFile::remove() {
    if (!m_deleted && FileSystem::exists(m_path)) {
        FileSystem::deleteFile(m_path);
        m_deleted = true;
    }
}

std::string TemporaryFile::generateUniqueName(const std::string& prefix, const std::string& suffix) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << prefix << "_";
    
    // 添加时间戳
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    ss << std::hex << time_t << "_";
    
    // 添加随机字符
    for (int i = 0; i < 8; ++i) {
        ss << std::hex << dis(gen);
    }
    
    if (!suffix.empty()) {
        ss << "." << suffix;
    }
    
    return ss.str();
}

} // namespace Util
} // namespace CHTL