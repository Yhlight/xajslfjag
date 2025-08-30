#include "CMODApi.h"
#include "../../Util/FileSystem/FileSystem.h"
#include "../../Util/ZIPUtil/ZIPUtil.h"

namespace CHTL {

CMODPacker::CMODPacker() {
}

bool CMODPacker::packCMOD(const std::string& sourceDir, const std::string& outputPath, const CMODInfo& info) {
    std::vector<Util::ZIPEntry> entries;
    
    // 生成info文件
    std::string infoContent = generateInfoFile(info);
    entries.emplace_back("info/" + info.name + ".chtl", infoContent);
    
    // 添加源文件
    auto sourceFiles = Util::FileSystem::listFiles(sourceDir, "chtl");
    for (const auto& file : sourceFiles) {
        std::string filePath = Util::FileSystem::combinePath(sourceDir, file);
        std::string content = Util::FileSystem::readFile(filePath);
        entries.emplace_back("src/" + file, content);
    }
    
    return Util::ZIPUtil::createZIP(entries, outputPath);
}

bool CMODPacker::unpackCMOD(const std::string& cmodPath, const std::string& outputDir) {
    auto entries = Util::ZIPUtil::extractZIP(cmodPath);
    
    for (const auto& entry : entries) {
        std::string outputPath = Util::FileSystem::combinePath(outputDir, entry.filename);
        std::string dir = Util::FileSystem::getDirectoryPath(outputPath);
        
        if (!Util::FileSystem::directoryExists(dir)) {
            Util::FileSystem::createDirectory(dir);
        }
        
        if (!Util::FileSystem::writeFile(outputPath, entry.content)) {
            return false;
        }
    }
    
    return true;
}

bool CMODPacker::validateCMOD(const std::string& cmodPath, CMODInfo& info) {
    if (!Util::ZIPUtil::validateZIP(cmodPath)) {
        return false;
    }
    
    auto entries = Util::ZIPUtil::extractZIP(cmodPath);
    
    // 查找info文件
    for (const auto& entry : entries) {
        if (entry.filename.find("info/") == 0 && entry.filename.find(".chtl") != std::string::npos) {
            info = parseInfoFile(entry.content);
            return true;
        }
    }
    
    return false;
}

std::string CMODPacker::generateInfoFile(const CMODInfo& info) {
    std::ostringstream oss;
    oss << "[Info]\n";
    oss << "{\n";
    oss << "    NAME = \"" << info.name << "\";\n";
    oss << "    VERSION = \"" << info.version << "\";\n";
    oss << "    DESCRIPTION = \"" << info.description << "\";\n";
    oss << "    AUTHOR = \"" << info.author << "\";\n";
    oss << "    LICENSE = \"" << info.license << "\";\n";
    
    if (!info.dependencies.empty()) {
        oss << "    DEPENDENCIES = [";
        for (size_t i = 0; i < info.dependencies.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << info.dependencies[i] << "\"";
        }
        oss << "];\n";
    }
    
    oss << "}\n";
    return oss.str();
}

CMODInfo CMODPacker::parseInfoFile(const std::string& infoContent) {
    CMODInfo info;
    
    // 简化的info文件解析
    size_t namePos = infoContent.find("NAME = \"");
    if (namePos != std::string::npos) {
        namePos += 8;
        size_t endPos = infoContent.find("\"", namePos);
        if (endPos != std::string::npos) {
            info.name = infoContent.substr(namePos, endPos - namePos);
        }
    }
    
    size_t versionPos = infoContent.find("VERSION = \"");
    if (versionPos != std::string::npos) {
        versionPos += 11;
        size_t endPos = infoContent.find("\"", versionPos);
        if (endPos != std::string::npos) {
            info.version = infoContent.substr(versionPos, endPos - versionPos);
        }
    }
    
    return info;
}

CMODManager::CMODManager() {
}

bool CMODManager::loadCMOD(const std::string& modulePath) {
    CMODInfo info;
    CMODPacker packer;
    
    if (packer.validateCMOD(modulePath, info)) {
        m_loadedModules[info.name] = info;
        return true;
    }
    
    return false;
}

CMODInfo CMODManager::getModuleInfo(const std::string& moduleName) const {
    auto it = m_loadedModules.find(moduleName);
    if (it != m_loadedModules.end()) {
        return it->second;
    }
    return CMODInfo();
}

std::vector<std::string> CMODManager::resolveDependencies(const std::string& moduleName) {
    auto it = m_loadedModules.find(moduleName);
    if (it != m_loadedModules.end()) {
        return it->second.dependencies;
    }
    return {};
}

bool CMODManager::hasCircularDependency(const std::string& moduleName) {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    
    return checkCircularDependencyRecursive(moduleName, visited, recursionStack);
}

bool CMODManager::checkCircularDependencyRecursive(const std::string& moduleName, 
                                                  std::unordered_set<std::string>& visited,
                                                  std::unordered_set<std::string>& recursionStack) {
    visited.insert(moduleName);
    recursionStack.insert(moduleName);
    
    auto dependencies = resolveDependencies(moduleName);
    for (const auto& dep : dependencies) {
        if (recursionStack.find(dep) != recursionStack.end()) {
            return true; // 发现循环依赖
        }
        
        if (visited.find(dep) == visited.end()) {
            if (checkCircularDependencyRecursive(dep, visited, recursionStack)) {
                return true;
            }
        }
    }
    
    recursionStack.erase(moduleName);
    return false;
}

} // namespace CHTL