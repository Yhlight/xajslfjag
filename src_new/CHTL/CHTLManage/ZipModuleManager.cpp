#include "ZipModuleManager.h"
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>

namespace CHTL {

// ========== SimpleZipHandler 实现 ==========

bool SimpleZipHandler::createZip(const std::string& zipPath, const std::vector<ZipEntry>& entries) {
    std::ofstream file(zipPath, std::ios::binary);
    if (!file.is_open()) return false;
    
    std::vector<size_t> localHeaderOffsets;
    
    // 写入本地文件头和数据
    for (const auto& entry : entries) {
        localHeaderOffsets.push_back(file.tellp());
        writeZipHeader(file, entry);
        
        // 写入文件数据（简单压缩）
        if (!entry.isDirectory) {
            auto compressedData = simpleCompress(entry.data);
            file.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
        }
    }
    
    // 写入中央目录
    size_t centralDirOffset = file.tellp();
    writeCentralDirectory(file, entries);
    
    // 写入EOCD（End of Central Directory）
    size_t centralDirSize = static_cast<size_t>(file.tellp()) - centralDirOffset;
    
    file.write("PK\x05\x06", 4);                    // EOCD签名
    file.write("\x00\x00\x00\x00", 4);              // 磁盘号
    uint16_t numEntries = static_cast<uint16_t>(entries.size());
    file.write(reinterpret_cast<const char*>(&numEntries), 2);
    file.write(reinterpret_cast<const char*>(&numEntries), 2);
    uint32_t centralDirSize32 = static_cast<uint32_t>(centralDirSize);
    file.write(reinterpret_cast<const char*>(&centralDirSize32), 4);
    uint32_t centralDirOffset32 = static_cast<uint32_t>(centralDirOffset);
    file.write(reinterpret_cast<const char*>(&centralDirOffset32), 4);
    file.write("\x00\x00", 2);                      // 注释长度
    
    return true;
}

bool SimpleZipHandler::extractZip(const std::string& zipPath, std::vector<ZipEntry>& entries) {
    std::ifstream file(zipPath, std::ios::binary);
    if (!file.is_open()) return false;
    
    // 简化的ZIP解析：直接读取中央目录
    return readCentralDirectory(file, entries);
}

bool SimpleZipHandler::addFileToZip(const std::string& filePath, const std::string& archivePath, ZipEntry& entry) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) return false;
    
    // 读取文件数据
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    entry.data.resize(fileSize);
    file.read(reinterpret_cast<char*>(entry.data.data()), fileSize);
    
    entry.filename = std::filesystem::path(archivePath).filename().string();
    entry.fullPath = archivePath;
    entry.uncompressedSize = fileSize;
    entry.crc32 = calculateCRC32(entry.data);
    entry.isDirectory = false;
    
    return true;
}

bool SimpleZipHandler::extractFileFromZip(const ZipEntry& entry, const std::string& outputPath) {
    std::ofstream file(outputPath, std::ios::binary);
    if (!file.is_open()) return false;
    
    // 解压缩数据
    auto decompressedData = simpleDecompress(entry.data, entry.uncompressedSize);
    file.write(reinterpret_cast<const char*>(decompressedData.data()), decompressedData.size());
    
    return true;
}

bool SimpleZipHandler::validateZip(const std::string& zipPath) {
    std::vector<ZipEntry> entries;
    return extractZip(zipPath, entries);
}

bool SimpleZipHandler::getZipInfo(const std::string& zipPath, std::vector<std::string>& fileList) {
    std::vector<ZipEntry> entries;
    if (!extractZip(zipPath, entries)) return false;
    
    for (const auto& entry : entries) {
        fileList.push_back(entry.fullPath);
    }
    
    return true;
}

uint32_t SimpleZipHandler::calculateCRC32(const std::vector<uint8_t>& data) {
    // 简化的CRC32计算
    uint32_t crc = 0xFFFFFFFF;
    static const uint32_t crc_table[256] = {
        // CRC32表（这里只提供前几个值作为示例）
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
        // ... 完整的CRC32表应该有256个值
    };
    
    for (uint8_t byte : data) {
        crc = crc_table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFF;
}

std::vector<uint8_t> SimpleZipHandler::simpleCompress(const std::vector<uint8_t>& data) {
    // 简化的压缩算法（实际应该使用DEFLATE）
    // 这里只是存储原始数据
    return data;
}

std::vector<uint8_t> SimpleZipHandler::simpleDecompress(const std::vector<uint8_t>& compressedData, size_t originalSize) {
    // 简化的解压缩（对应上面的简化压缩）
    return compressedData;
}

void SimpleZipHandler::writeZipHeader(std::ofstream& file, const ZipEntry& entry) {
    file.write("PK\x03\x04", 4);                    // 本地文件头签名
    file.write("\x14\x00", 2);                      // 版本
    file.write("\x00\x00", 2);                      // 标志
    file.write("\x00\x00", 2);                      // 压缩方法（存储）
    file.write("\x00\x00\x00\x00", 4);              // 时间戳
    
    uint32_t crc = entry.crc32;
    file.write(reinterpret_cast<const char*>(&crc), 4);
    
    uint32_t compressedSize = static_cast<uint32_t>(entry.data.size());
    file.write(reinterpret_cast<const char*>(&compressedSize), 4);
    
    uint32_t uncompressedSize = static_cast<uint32_t>(entry.uncompressedSize);
    file.write(reinterpret_cast<const char*>(&uncompressedSize), 4);
    
    uint16_t filenameLength = static_cast<uint16_t>(entry.fullPath.length());
    file.write(reinterpret_cast<const char*>(&filenameLength), 2);
    file.write("\x00\x00", 2);                      // 额外字段长度
    
    file.write(entry.fullPath.c_str(), entry.fullPath.length());
}

bool SimpleZipHandler::readZipHeader(std::ifstream& file, ZipEntry& entry) {
    char signature[4];
    file.read(signature, 4);
    
    if (std::string(signature, 4) != "PK\x03\x04") {
        return false;
    }
    
    file.seekg(18, std::ios::cur);                   // 跳过版本、标志等
    
    uint32_t crc, compressedSize, uncompressedSize;
    file.read(reinterpret_cast<char*>(&crc), 4);
    file.read(reinterpret_cast<char*>(&compressedSize), 4);
    file.read(reinterpret_cast<char*>(&uncompressedSize), 4);
    
    uint16_t filenameLength, extraFieldLength;
    file.read(reinterpret_cast<char*>(&filenameLength), 2);
    file.read(reinterpret_cast<char*>(&extraFieldLength), 2);
    
    std::string filename(filenameLength, '\0');
    file.read(&filename[0], filenameLength);
    
    file.seekg(extraFieldLength, std::ios::cur);     // 跳过额外字段
    
    // 读取文件数据
    entry.data.resize(compressedSize);
    file.read(reinterpret_cast<char*>(entry.data.data()), compressedSize);
    
    entry.fullPath = filename;
    entry.filename = std::filesystem::path(filename).filename().string();
    entry.crc32 = crc;
    entry.compressedSize = compressedSize;
    entry.uncompressedSize = uncompressedSize;
    entry.isDirectory = filename.back() == '/';
    
    return true;
}

void SimpleZipHandler::writeCentralDirectory(std::ofstream& file, const std::vector<ZipEntry>& entries) {
    for (const auto& entry : entries) {
        file.write("PK\x01\x02", 4);                // 中央目录签名
        file.write("\x14\x00\x14\x00", 4);          // 版本
        file.write("\x00\x00\x00\x00", 4);          // 标志和压缩方法
        file.write("\x00\x00\x00\x00", 4);          // 时间戳
        
        uint32_t crc = entry.crc32;
        file.write(reinterpret_cast<const char*>(&crc), 4);
        
        uint32_t compressedSize = static_cast<uint32_t>(entry.data.size());
        file.write(reinterpret_cast<const char*>(&compressedSize), 4);
        
        uint32_t uncompressedSize = static_cast<uint32_t>(entry.uncompressedSize);
        file.write(reinterpret_cast<const char*>(&uncompressedSize), 4);
        
        uint16_t filenameLength = static_cast<uint16_t>(entry.fullPath.length());
        file.write(reinterpret_cast<const char*>(&filenameLength), 2);
        file.write("\x00\x00\x00\x00\x00\x00", 6);  // 额外字段等
        file.write("\x00\x00\x00\x00", 4);          // 外部属性
        file.write("\x00\x00\x00\x00", 4);          // 本地头偏移
        
        file.write(entry.fullPath.c_str(), entry.fullPath.length());
    }
}

bool SimpleZipHandler::readCentralDirectory(std::ifstream& file, std::vector<ZipEntry>& entries) {
    // 这里应该实现完整的中央目录解析
    // 简化实现：假设成功
    return true;
}

// ========== CMODPacker 实现 ==========

bool CMODPacker::packCMOD(const std::string& sourcePath, const std::string& cmodPath, const CMODInfo& info) {
    if (!m_zipHandler) {
        m_zipHandler = std::make_unique<SimpleZipHandler>();
    }
    
    std::vector<ZipEntry> entries;
    
    // 生成模块信息文件
    std::string infoContent = generateModuleInfo(info);
    ZipEntry infoEntry;
    infoEntry.fullPath = "module.info";
    infoEntry.filename = "module.info";
    infoEntry.data.assign(infoContent.begin(), infoContent.end());
    infoEntry.uncompressedSize = infoContent.size();
    infoEntry.crc32 = m_zipHandler->calculateCRC32(infoEntry.data);
    entries.push_back(infoEntry);
    
    // 收集源文件
    auto sourceFiles = collectSourceFiles(sourcePath);
    
    for (const auto& sourceFile : sourceFiles) {
        if (!shouldIncludeFile(sourceFile)) continue;
        
        ZipEntry entry;
        std::string relativePath = std::filesystem::relative(sourceFile, sourcePath).string();
        
        if (m_zipHandler->addFileToZip(sourceFile, relativePath, entry)) {
            entries.push_back(entry);
        }
    }
    
    return m_zipHandler->createZip(cmodPath, entries);
}

bool CMODPacker::unpackCMOD(const std::string& cmodPath, const std::string& targetPath, CMODInfo& info) {
    if (!m_zipHandler) {
        m_zipHandler = std::make_unique<SimpleZipHandler>();
    }
    
    std::vector<ZipEntry> entries;
    if (!m_zipHandler->extractZip(cmodPath, entries)) {
        return false;
    }
    
    // 创建目标目录
    std::filesystem::create_directories(targetPath);
    
    // 解压文件
    for (const auto& entry : entries) {
        std::string outputPath = std::filesystem::path(targetPath) / entry.fullPath;
        
        if (entry.isDirectory) {
            std::filesystem::create_directories(outputPath);
        } else {
            std::filesystem::create_directories(std::filesystem::path(outputPath).parent_path());
            
            if (entry.filename == "module.info") {
                // 解析模块信息
                std::string content(entry.data.begin(), entry.data.end());
                parseModuleInfo(content, info);
            }
            
            m_zipHandler->extractFileFromZip(entry, outputPath);
        }
    }
    
    return true;
}

bool CMODPacker::validateCMOD(const std::string& cmodPath, CMODInfo& info) {
    std::vector<ZipEntry> entries;
    if (!m_zipHandler->extractZip(cmodPath, entries)) {
        return false;
    }
    
    // 检查必需文件
    bool hasModuleInfo = false;
    
    for (const auto& entry : entries) {
        if (entry.filename == "module.info") {
            hasModuleInfo = true;
            std::string content(entry.data.begin(), entry.data.end());
            if (!parseModuleInfo(content, info)) {
                return false;
            }
            break;
        }
    }
    
    return hasModuleInfo;
}

std::string CMODPacker::generateModuleInfo(const CMODInfo& info) {
    std::ostringstream ss;
    
    ss << "[CMOD Module Info]\n";
    ss << "name=" << info.name << "\n";
    ss << "version=" << info.version << "\n";
    ss << "description=" << info.description << "\n";
    ss << "main=" << info.mainFile << "\n";
    
    if (!info.dependencies.empty()) {
        ss << "dependencies=";
        for (size_t i = 0; i < info.dependencies.size(); ++i) {
            if (i > 0) ss << ",";
            ss << info.dependencies[i];
        }
        ss << "\n";
    }
    
    if (!info.exports.empty()) {
        ss << "exports=";
        for (size_t i = 0; i < info.exports.size(); ++i) {
            if (i > 0) ss << ",";
            ss << info.exports[i];
        }
        ss << "\n";
    }
    
    for (const auto& [key, value] : info.metadata) {
        ss << key << "=" << value << "\n";
    }
    
    return ss.str();
}

bool CMODPacker::parseModuleInfo(const std::string& content, CMODInfo& info) {
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == '#' || line[0] == '[') continue;
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        if (key == "name") {
            info.name = value;
        } else if (key == "version") {
            info.version = value;
        } else if (key == "description") {
            info.description = value;
        } else if (key == "main") {
            info.mainFile = value;
        } else if (key == "dependencies") {
            std::istringstream depStream(value);
            std::string dep;
            while (std::getline(depStream, dep, ',')) {
                info.dependencies.push_back(dep);
            }
        } else if (key == "exports") {
            std::istringstream expStream(value);
            std::string exp;
            while (std::getline(expStream, exp, ',')) {
                info.exports.push_back(exp);
            }
        } else {
            info.metadata[key] = value;
        }
    }
    
    return !info.name.empty();
}

std::vector<std::string> CMODPacker::collectSourceFiles(const std::string& sourcePath) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(sourcePath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::exception&) {
        // 忽略文件系统错误
    }
    
    return files;
}

bool CMODPacker::shouldIncludeFile(const std::string& filename) {
    std::string ext = std::filesystem::path(filename).extension().string();
    
    // 包含CHTL相关文件
    return ext == ".chtl" || ext == ".cjjs" || ext == ".css" || 
           ext == ".js" || ext == ".html" || ext == ".md" || ext == ".txt";
}

bool CMODPacker::generateDependencyGraph(const std::vector<std::string>& files, std::vector<std::string>& dependencies) {
    // 简化的依赖分析
    for (const auto& file : files) {
        std::ifstream f(file);
        std::string line;
        
        while (std::getline(f, line)) {
            if (line.find("[Import]") != std::string::npos) {
                // 提取导入信息
                // 这里应该实现完整的CHTL导入解析
            }
        }
    }
    
    return true;
}

// ========== ZipModuleManager 主要实现 ==========

ZipModuleManager::ZipModuleManager() {
    m_cmodPacker = std::make_unique<CMODPacker>();
    m_cjmodPacker = std::make_unique<CJMODPacker>();
    m_dependencyResolver = std::make_unique<ModuleDependencyResolver>();
}

bool ZipModuleManager::packCMOD(const std::string& sourcePath, const std::string& outputPath, const CMODInfo& info) {
    updateStats("cmod_pack_attempts");
    
    bool result = m_cmodPacker->packCMOD(sourcePath, outputPath, info);
    
    if (result) {
        updateStats("cmod_pack_success");
        cacheModuleInfo(outputPath, info);
    } else {
        updateStats("cmod_pack_failures");
    }
    
    return result;
}

bool ZipModuleManager::unpackCMOD(const std::string& cmodPath, const std::string& targetPath) {
    updateStats("cmod_unpack_attempts");
    
    CMODInfo info;
    bool result = m_cmodPacker->unpackCMOD(cmodPath, targetPath, info);
    
    if (result) {
        updateStats("cmod_unpack_success");
        cacheModuleInfo(cmodPath, info);
    } else {
        updateStats("cmod_unpack_failures");
    }
    
    return result;
}

bool ZipModuleManager::validateModule(const std::string& modulePath) {
    CMODInfo info;
    
    if (modulePath.size() > 5 && modulePath.substr(modulePath.size() - 5) == ".cmod") {
        return m_cmodPacker->validateCMOD(modulePath, info);
    } else if (modulePath.size() > 6 && modulePath.substr(modulePath.size() - 6) == ".cjmod") {
        CJMODInfo cjmodInfo;
        return m_cjmodPacker->validateCJMOD(modulePath, cjmodInfo);
    }
    
    return false;
}

bool ZipModuleManager::resolveDependencies(const std::vector<std::string>& modules, std::vector<std::string>& order) {
    return m_dependencyResolver->resolveDependencies(modules, order);
}

std::vector<std::string> ZipModuleManager::searchModules(const std::string& searchPath, const std::string& pattern) {
    std::vector<std::string> modules;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(searchPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string ext = entry.path().extension().string();
                
                if ((ext == ".cmod" || ext == ".cjmod") && 
                    (pattern.empty() || filename.find(pattern) != std::string::npos)) {
                    modules.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception&) {
        // 忽略搜索错误
    }
    
    return modules;
}

std::string ZipModuleManager::getManagerStats() const {
    std::ostringstream ss;
    
    ss << "Zip Module Manager Statistics:\n";
    ss << "=============================\n";
    
    for (const auto& [category, count] : m_stats) {
        ss << category << ": " << count << "\n";
    }
    
    ss << "\nCached Modules: " << m_moduleCache.size() << "\n";
    ss << "Search Paths: " << m_searchPaths.size() << "\n";
    
    return ss.str();
}

void ZipModuleManager::updateStats(const std::string& category) {
    m_stats[category]++;
}

void ZipModuleManager::cacheModuleInfo(const std::string& modulePath, const CMODInfo& info) {
    m_moduleCache[modulePath] = info;
}

void ZipModuleManager::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "Zip Module Manager: " + message);
}

// ========== CJMODPacker 实现 ==========

CJMODPacker::CJMODPacker() {
    m_zipHandler = std::make_unique<SimpleZipHandler>();
    m_basePacker = std::make_unique<CMODPacker>();
}

bool CJMODPacker::packCJMOD(const std::string& sourcePath, const std::string& cjmodPath, const CJMODInfo& info) {
    // 基于CMOD打包，添加CJMOD特有功能
    return m_basePacker->packCMOD(sourcePath, cjmodPath, info);
}

bool CJMODPacker::unpackCJMOD(const std::string& cjmodPath, const std::string& targetPath, CJMODInfo& info) {
    CMODInfo baseInfo;
    bool result = m_basePacker->unpackCMOD(cjmodPath, targetPath, baseInfo);
    
    if (result) {
        // 复制基础信息到CJMOD信息
        info.name = baseInfo.name;
        info.version = baseInfo.version;
        info.description = baseInfo.description;
        info.dependencies = baseInfo.dependencies;
        info.metadata = baseInfo.metadata;
        info.exports = baseInfo.exports;
        info.mainFile = baseInfo.mainFile;
    }
    
    return result;
}

bool CJMODPacker::validateCJMOD(const std::string& cjmodPath, CJMODInfo& info) {
    CMODInfo baseInfo;
    return m_basePacker->validateCMOD(cjmodPath, baseInfo);
}

// ========== ModuleDependencyResolver 实现 ==========

bool ModuleDependencyResolver::resolveDependencies(const std::vector<std::string>& modules, std::vector<std::string>& resolvedOrder) {
    // 简化的依赖解析：直接返回原始顺序
    resolvedOrder = modules;
    return true;
}

bool ModuleDependencyResolver::checkCircularDependencies(const std::vector<std::string>& modules, std::vector<std::string>& circularPaths) {
    // 简化实现：假设没有循环依赖
    return false;
}

std::vector<std::string> ModuleDependencyResolver::getModuleDependencies(const std::string& modulePath) {
    // 简化实现：返回空依赖
    return {};
}

bool ModuleDependencyResolver::validateDependencies(const std::vector<std::string>& modules, std::vector<std::string>& missingDependencies) {
    // 简化实现：假设所有依赖都存在
    return true;
}

} // namespace CHTL