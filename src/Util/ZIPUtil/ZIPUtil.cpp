#include "ZIPUtil.h"
#include <fstream>
#include <filesystem>
#include <cstring>
#include <algorithm>
#include <sstream>

// 简化版ZIP实现（实际项目中应使用libzip或minizip）
// 这里实现一个基本的ZIP功能模拟

namespace CHTL {

namespace fs = std::filesystem;

// ZIP文件头魔数
// constexpr uint32_t ZIP_LOCAL_FILE_HEADER_SIGNATURE = 0x04034b50;
// constexpr uint32_t ZIP_CENTRAL_DIR_SIGNATURE = 0x02014b50;
// constexpr uint32_t ZIP_END_OF_CENTRAL_DIR_SIGNATURE = 0x06054b50;

// 简单的ZIP结构（用于演示）
struct LocalFileHeader {
    uint32_t signature;
    uint16_t version;
    uint16_t flags;
    uint16_t compression;
    uint16_t modTime;
    uint16_t modDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t filenameLen;
    uint16_t extraLen;
};

// ZIPUtil::Impl 实现
class ZIPUtil::Impl {
public:
    int compressionLevel = 6;
    std::string currentArchive;
    std::vector<std::pair<std::string, std::string>> pendingFiles;
    std::function<void(size_t, size_t)> progressCallback;
    
    bool isCreating = false;
};

ZIPUtil::ZIPUtil() : pImpl(std::make_unique<Impl>()) {}

ZIPUtil::~ZIPUtil() = default;

void ZIPUtil::setCompressionLevel(int level) {
    pImpl->compressionLevel = std::clamp(level, 0, 9);
}

bool ZIPUtil::createArchive(const std::string& zipPath) {
    pImpl->currentArchive = zipPath;
    pImpl->pendingFiles.clear();
    pImpl->isCreating = true;
    
    // 确保目录存在
    fs::path zipFilePath(zipPath);
    if (zipFilePath.has_parent_path()) {
        fs::create_directories(zipFilePath.parent_path());
    }
    
    return true;
}

bool ZIPUtil::addFile(const std::string& filePath, const std::string& entryName) {
    if (!pImpl->isCreating) {
        lastError_ = "No archive is being created";
        return false;
    }
    
    if (!fs::exists(filePath)) {
        lastError_ = "File not found: " + filePath;
        return false;
    }
    
    // 读取文件内容
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        lastError_ = "Failed to open file: " + filePath;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    std::string name = entryName.empty() ? fs::path(filePath).filename().string() : entryName;
    pImpl->pendingFiles.push_back({normalizeEntryName(name), buffer.str()});
    
    return true;
}

bool ZIPUtil::addFromMemory(const std::string& entryName, const std::string& data) {
    if (!pImpl->isCreating) {
        lastError_ = "No archive is being created";
        return false;
    }
    
    pImpl->pendingFiles.push_back({normalizeEntryName(entryName), data});
    return true;
}

bool ZIPUtil::addDirectory(const std::string& dirPath, const std::string& entryPrefix) {
    if (!pImpl->isCreating) {
        lastError_ = "No archive is being created";
        return false;
    }
    
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        lastError_ = "Invalid directory: " + dirPath;
        return false;
    }
    
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            fs::path relativePath = fs::relative(entry.path(), dirPath);
            std::string entryName = entryPrefix.empty() ? 
                relativePath.string() : 
                entryPrefix + "/" + relativePath.string();
            
            if (!addFile(entry.path().string(), entryName)) {
                return false;
            }
        }
    }
    
    return true;
}

bool ZIPUtil::finalize() {
    if (!pImpl->isCreating) {
        lastError_ = "No archive is being created";
        return false;
    }
    
    // 简化实现：将文件写入为TAR格式（演示用途）
    std::ofstream archive(pImpl->currentArchive, std::ios::binary);
    if (!archive.is_open()) {
        lastError_ = "Failed to create archive: " + pImpl->currentArchive;
        return false;
    }
    
    // 写入简单的头部
    const char* magic = "CMODZIP1";
    archive.write(magic, 8);
    
    // 写入文件数量
    uint32_t fileCount = static_cast<uint32_t>(pImpl->pendingFiles.size());
    archive.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));
    
    // 写入每个文件
    size_t current = 0;
    for (const auto& [name, content] : pImpl->pendingFiles) {
        // 文件名长度
        uint32_t nameLen = static_cast<uint32_t>(name.length());
        archive.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        
        // 文件名
        archive.write(name.c_str(), nameLen);
        
        // 文件内容长度
        uint32_t contentLen = static_cast<uint32_t>(content.length());
        archive.write(reinterpret_cast<const char*>(&contentLen), sizeof(contentLen));
        
        // 文件内容
        archive.write(content.c_str(), contentLen);
        
        // 进度回调
        if (pImpl->progressCallback) {
            pImpl->progressCallback(++current, fileCount);
        }
    }
    
    archive.close();
    pImpl->isCreating = false;
    pImpl->pendingFiles.clear();
    
    return archive.good();
}

bool ZIPUtil::extractArchive(const std::string& zipPath, const std::string& destDir) {
    if (!fs::exists(zipPath)) {
        lastError_ = "Archive not found: " + zipPath;
        return false;
    }
    
    std::ifstream archive(zipPath, std::ios::binary);
    if (!archive.is_open()) {
        lastError_ = "Failed to open archive: " + zipPath;
        return false;
    }
    
    // 读取头部
    char magic[8];
    archive.read(magic, 8);
    if (std::strncmp(magic, "CMODZIP1", 8) != 0) {
        lastError_ = "Invalid archive format";
        return false;
    }
    
    // 读取文件数量
    uint32_t fileCount;
    archive.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));
    
    // 创建目标目录
    fs::create_directories(destDir);
    
    // 解压每个文件
    for (uint32_t i = 0; i < fileCount; ++i) {
        // 读取文件名长度
        uint32_t nameLen;
        archive.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        
        // 读取文件名
        std::string name(nameLen, '\0');
        archive.read(&name[0], nameLen);
        
        // 读取内容长度
        uint32_t contentLen;
        archive.read(reinterpret_cast<char*>(&contentLen), sizeof(contentLen));
        
        // 读取内容
        std::string content(contentLen, '\0');
        archive.read(&content[0], contentLen);
        
        // 写入文件
        fs::path filePath = fs::path(destDir) / name;
        fs::create_directories(filePath.parent_path());
        
        std::ofstream outFile(filePath, std::ios::binary);
        outFile.write(content.c_str(), content.length());
        
        if (!outFile.good()) {
            lastError_ = "Failed to extract file: " + name;
            return false;
        }
        
        // 进度回调
        if (pImpl->progressCallback) {
            pImpl->progressCallback(i + 1, fileCount);
        }
    }
    
    return true;
}

bool ZIPUtil::extractFile(const std::string& zipPath, const std::string& entryName, const std::string& destPath) {
    if (!fs::exists(zipPath)) {
        lastError_ = "Archive not found: " + zipPath;
        return false;
    }
    
    std::ifstream archive(zipPath, std::ios::binary);
    if (!archive.is_open()) {
        lastError_ = "Failed to open archive: " + zipPath;
        return false;
    }
    
    // 读取头部
    char magic[8];
    archive.read(magic, 8);
    if (std::strncmp(magic, "CMODZIP1", 8) != 0) {
        lastError_ = "Invalid archive format";
        return false;
    }
    
    // 读取文件数量
    uint32_t fileCount;
    archive.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));
    
    // 查找文件
    for (uint32_t i = 0; i < fileCount; ++i) {
        // 读取文件名长度
        uint32_t nameLen;
        archive.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        
        // 读取文件名
        std::string name(nameLen, '\0');
        archive.read(&name[0], nameLen);
        
        // 读取内容长度
        uint32_t contentLen;
        archive.read(reinterpret_cast<char*>(&contentLen), sizeof(contentLen));
        
        if (name == entryName) {
            // 找到目标文件，读取内容
            std::string content(contentLen, '\0');
            archive.read(&content[0], contentLen);
            
            // 写入文件
            fs::path filePath(destPath);
            fs::create_directories(filePath.parent_path());
            
            std::ofstream outFile(filePath, std::ios::binary);
            outFile.write(content.c_str(), content.length());
            
            return outFile.good();
        } else {
            // 跳过内容
            archive.seekg(contentLen, std::ios::cur);
        }
    }
    
    lastError_ = "File not found in archive: " + entryName;
    return false;
}

std::vector<ZIPFileInfo> ZIPUtil::listFiles(const std::string& zipPath) {
    std::vector<ZIPFileInfo> files;
    
    if (!fs::exists(zipPath)) {
        return files;
    }
    
    std::ifstream archive(zipPath, std::ios::binary);
    if (!archive.is_open()) {
        return files;
    }
    
    // 读取头部
    char magic[8];
    archive.read(magic, 8);
    if (std::strncmp(magic, "CMODZIP1", 8) != 0) {
        return files;
    }
    
    // 读取文件数量
    uint32_t fileCount;
    archive.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));
    
    // 读取文件信息
    for (uint32_t i = 0; i < fileCount; ++i) {
        ZIPFileInfo info;
        
        // 读取文件名长度
        uint32_t nameLen;
        archive.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        
        // 读取文件名
        info.filename.resize(nameLen);
        archive.read(&info.filename[0], nameLen);
        
        // 读取内容长度
        uint32_t contentLen;
        archive.read(reinterpret_cast<char*>(&contentLen), sizeof(contentLen));
        
        info.uncompressedSize = contentLen;
        info.compressedSize = contentLen; // 简化实现，无压缩
        info.isDirectory = false;
        info.compressionMethod = 0;
        info.modificationTime = std::time(nullptr);
        
        files.push_back(info);
        
        // 跳过内容
        archive.seekg(contentLen, std::ios::cur);
    }
    
    return files;
}

bool ZIPUtil::fileExists(const std::string& zipPath, const std::string& entryName) {
    auto files = listFiles(zipPath);
    return std::any_of(files.begin(), files.end(), 
        [&](const ZIPFileInfo& info) { return info.filename == entryName; });
}

std::string ZIPUtil::readFileToString(const std::string& zipPath, const std::string& entryName) {
    if (!fs::exists(zipPath)) {
        return "";
    }
    
    std::ifstream archive(zipPath, std::ios::binary);
    if (!archive.is_open()) {
        return "";
    }
    
    // 读取头部
    char magic[8];
    archive.read(magic, 8);
    if (std::strncmp(magic, "CMODZIP1", 8) != 0) {
        return "";
    }
    
    // 读取文件数量
    uint32_t fileCount;
    archive.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));
    
    // 查找文件
    for (uint32_t i = 0; i < fileCount; ++i) {
        // 读取文件名长度
        uint32_t nameLen;
        archive.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        
        // 读取文件名
        std::string name(nameLen, '\0');
        archive.read(&name[0], nameLen);
        
        // 读取内容长度
        uint32_t contentLen;
        archive.read(reinterpret_cast<char*>(&contentLen), sizeof(contentLen));
        
        if (name == entryName) {
            // 找到目标文件，读取内容
            std::string content(contentLen, '\0');
            archive.read(&content[0], contentLen);
            return content;
        } else {
            // 跳过内容
            archive.seekg(contentLen, std::ios::cur);
        }
    }
    
    return "";
}

void ZIPUtil::setProgressCallback(std::function<void(size_t, size_t)> callback) {
    pImpl->progressCallback = callback;
}

bool ZIPUtil::ensureDirectoryExists(const std::string& path) {
    try {
        fs::create_directories(path);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string ZIPUtil::normalizeEntryName(const std::string& name) {
    std::string normalized = name;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // 移除开头的斜杠
    if (!normalized.empty() && normalized[0] == '/') {
        normalized = normalized.substr(1);
    }
    
    return normalized;
}

// ZIPReader::Impl
class ZIPReader::Impl {
public:
    std::ifstream archive;
    std::string currentFile;
    std::vector<ZIPFileInfo> fileList;
    size_t currentPos = 0;
};

ZIPReader::ZIPReader() : pImpl(std::make_unique<Impl>()) {}

ZIPReader::~ZIPReader() {
    close();
}

bool ZIPReader::open(const std::string& zipPath) {
    close();
    
    pImpl->archive.open(zipPath, std::ios::binary);
    if (!pImpl->archive.is_open()) {
        return false;
    }
    
    // 读取文件列表
    ZIPUtil util;
    pImpl->fileList = util.listFiles(zipPath);
    
    return !pImpl->fileList.empty();
}

void ZIPReader::close() {
    if (pImpl->archive.is_open()) {
        pImpl->archive.close();
    }
    pImpl->fileList.clear();
    pImpl->currentPos = 0;
}

std::vector<ZIPFileInfo> ZIPReader::getFileList() {
    return pImpl->fileList;
}

bool ZIPReader::locateFile(const std::string& entryName) {
    auto it = std::find_if(pImpl->fileList.begin(), pImpl->fileList.end(),
        [&](const ZIPFileInfo& info) { return info.filename == entryName; });
    
    if (it != pImpl->fileList.end()) {
        pImpl->currentPos = std::distance(pImpl->fileList.begin(), it);
        return true;
    }
    
    return false;
}

std::vector<uint8_t> ZIPReader::readCurrentFile() {
    // 简化实现
    return std::vector<uint8_t>();
}

size_t ZIPReader::read(void* /*buffer*/, size_t /*size*/) {
    // 简化实现
    return 0;
}

bool ZIPReader::isOpen() const {
    return pImpl->archive.is_open();
}

ZIPFileInfo ZIPReader::getCurrentFileInfo() {
    if (pImpl->currentPos < pImpl->fileList.size()) {
        return pImpl->fileList[pImpl->currentPos];
    }
    return ZIPFileInfo();
}

// ZIPWriter::Impl
class ZIPWriter::Impl {
public:
    std::ofstream archive;
    std::vector<std::pair<std::string, std::string>> files;
    std::string currentFileName;
    std::stringstream currentFileData;
    int compressionLevel = 6;
    std::string comment;
};

ZIPWriter::ZIPWriter() : pImpl(std::make_unique<Impl>()) {}

ZIPWriter::~ZIPWriter() {
    close();
}

bool ZIPWriter::create(const std::string& zipPath) {
    close();
    
    pImpl->archive.open(zipPath, std::ios::binary);
    if (!pImpl->archive.is_open()) {
        return false;
    }
    
    // 写入头部
    const char* magic = "CMODZIP1";
    pImpl->archive.write(magic, 8);
    
    // 预留文件数量位置
    uint32_t fileCount = 0;
    pImpl->archive.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));
    
    return true;
}

bool ZIPWriter::beginFile(const std::string& entryName, int compressionLevel) {
    if (!pImpl->currentFileName.empty()) {
        endFile();
    }
    
    pImpl->currentFileName = entryName;
    pImpl->compressionLevel = compressionLevel;
    pImpl->currentFileData.str("");
    pImpl->currentFileData.clear();
    
    return true;
}

bool ZIPWriter::write(const void* data, size_t size) {
    if (pImpl->currentFileName.empty()) {
        return false;
    }
    
    pImpl->currentFileData.write(static_cast<const char*>(data), size);
    return pImpl->currentFileData.good();
}

bool ZIPWriter::endFile() {
    if (pImpl->currentFileName.empty()) {
        return false;
    }
    
    pImpl->files.push_back({pImpl->currentFileName, pImpl->currentFileData.str()});
    pImpl->currentFileName.clear();
    
    return true;
}

bool ZIPWriter::close() {
    if (!pImpl->archive.is_open()) {
        return false;
    }
    
    // 结束当前文件
    if (!pImpl->currentFileName.empty()) {
        endFile();
    }
    
    // 写入所有文件
    for (const auto& [name, content] : pImpl->files) {
        // 文件名长度
        uint32_t nameLen = static_cast<uint32_t>(name.length());
        pImpl->archive.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        
        // 文件名
        pImpl->archive.write(name.c_str(), nameLen);
        
        // 文件内容长度
        uint32_t contentLen = static_cast<uint32_t>(content.length());
        pImpl->archive.write(reinterpret_cast<const char*>(&contentLen), sizeof(contentLen));
        
        // 文件内容
        pImpl->archive.write(content.c_str(), contentLen);
    }
    
    // 更新文件数量
    pImpl->archive.seekp(8);
    uint32_t fileCount = static_cast<uint32_t>(pImpl->files.size());
    pImpl->archive.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));
    
    pImpl->archive.close();
    pImpl->files.clear();
    
    return true;
}

bool ZIPWriter::isOpen() const {
    return pImpl->archive.is_open();
}

void ZIPWriter::setComment(const std::string& comment) {
    pImpl->comment = comment;
}

} // namespace CHTL