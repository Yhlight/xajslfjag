#include "ImportNode.h"
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace CHTL {

// ========== ImportNode 实现 ==========

ImportNode::ImportNode(ImportType importType) 
    : BaseNode(CHTLNodeType::IMPORT_NODE, "import"), 
      m_importType(importType), 
      m_pathType(ImportPathType::FILENAME_ONLY) {
}

ImportType ImportNode::getImportType() const {
    return m_importType;
}

void ImportNode::setImportType(ImportType type) {
    m_importType = type;
}

const std::string& ImportNode::getImportPath() const {
    return m_importPath;
}

void ImportNode::setImportPath(const std::string& path) {
    m_importPath = path;
    
    // 自动推断路径类型
    if (path.find('/') != std::string::npos || path.find('\\') != std::string::npos) {
        m_pathType = ImportPathType::FULL_PATH;
    } else if (path.find('.') != std::string::npos) {
        // 检查是否为文件扩展名
        size_t lastDot = path.find_last_of('.');
        if (lastDot != std::string::npos && lastDot < path.length() - 1) {
            std::string ext = path.substr(lastDot + 1);
            if (ext == "chtl" || ext == "html" || ext == "css" || ext == "js" || 
                ext == "cmod" || ext == "cjmod") {
                m_pathType = ImportPathType::FILENAME_WITH_EXT;
            } else {
                m_pathType = ImportPathType::QUALIFIED_NAME;
            }
        } else {
            m_pathType = ImportPathType::QUALIFIED_NAME;
        }
    } else {
        m_pathType = ImportPathType::FILENAME_ONLY;
    }
}

ImportPathType ImportNode::getImportPathType() const {
    return m_pathType;
}

void ImportNode::setImportPathType(ImportPathType pathType) {
    m_pathType = pathType;
}

const std::string& ImportNode::getSymbolName() const {
    return m_symbolName;
}

void ImportNode::setSymbolName(const std::string& name) {
    m_symbolName = name;
}

const std::string& ImportNode::getSymbolType() const {
    return m_symbolType;
}

void ImportNode::setSymbolType(const std::string& type) {
    m_symbolType = type;
}

const std::string& ImportNode::getAlias() const {
    return m_alias;
}

void ImportNode::setAlias(const std::string& alias) {
    m_alias = alias;
}

bool ImportNode::hasAlias() const {
    return !m_alias.empty();
}

const std::string& ImportNode::getSourceNamespace() const {
    return m_sourceNamespace;
}

void ImportNode::setSourceNamespace(const std::string& namespaceName) {
    m_sourceNamespace = namespaceName;
}

const std::vector<std::string>& ImportNode::getImportedSymbols() const {
    return m_importedSymbols;
}

void ImportNode::addImportedSymbol(const std::string& symbol) {
    if (std::find(m_importedSymbols.begin(), m_importedSymbols.end(), symbol) == m_importedSymbols.end()) {
        m_importedSymbols.push_back(symbol);
    }
}

std::string ImportNode::resolveImportPath() const {
    // 这里简化实现，实际需要根据不同的路径类型和导入类型进行复杂的路径解析
    switch (m_pathType) {
        case ImportPathType::FILENAME_ONLY:
            // 需要添加适当的扩展名并在搜索路径中查找
            return m_importPath + getDefaultExtension();
        case ImportPathType::FILENAME_WITH_EXT:
            return m_importPath;
        case ImportPathType::FULL_PATH:
            return m_importPath;
        case ImportPathType::MODULE_NAME:
            return "modules/" + m_importPath;
        case ImportPathType::QUALIFIED_NAME: {
            // 转换为路径格式
            std::string path = m_importPath;
            std::replace(path.begin(), path.end(), '.', '/');
            return path + getDefaultExtension();
        }
        default:
            return m_importPath;
    }
}

std::string ImportNode::getDefaultExtension() const {
    switch (m_importType) {
        case ImportType::HTML_IMPORT:
            return ".html";
        case ImportType::STYLE_IMPORT:
            return ".css";
        case ImportType::JAVASCRIPT_IMPORT:
            return ".js";
        case ImportType::CHTL_IMPORT:
            return ".chtl";
        case ImportType::CJMOD_IMPORT:
            return ".cjmod";
        default:
            return "";
    }
}

bool ImportNode::isModuleImport() const {
    return m_importType == ImportType::CHTL_IMPORT || m_importType == ImportType::CJMOD_IMPORT;
}

bool ImportNode::isFileImport() const {
    return m_importType == ImportType::HTML_IMPORT || 
           m_importType == ImportType::STYLE_IMPORT || 
           m_importType == ImportType::JAVASCRIPT_IMPORT;
}

bool ImportNode::isSymbolImport() const {
    return m_importType == ImportType::TEMPLATE_IMPORT || 
           m_importType == ImportType::CUSTOM_IMPORT || 
           m_importType == ImportType::ORIGIN_IMPORT;
}

bool ImportNode::isBatchImport() const {
    return m_importType == ImportType::ALL_TEMPLATES_IMPORT || 
           m_importType == ImportType::ALL_CUSTOMS_IMPORT || 
           m_importType == ImportType::ALL_ORIGINS_IMPORT || 
           m_importType == ImportType::ALL_CONFIGS_IMPORT;
}

CHTLNodeType ImportNode::getNodeType() const {
    return getImportNodeType();
}

CHTLNodeType ImportNode::getImportNodeType() const {
    switch (m_importType) {
        case ImportType::HTML_IMPORT:
            return CHTLNodeType::IMPORT_HTML_NODE;
        case ImportType::STYLE_IMPORT:
            return CHTLNodeType::IMPORT_STYLE_NODE;
        case ImportType::JAVASCRIPT_IMPORT:
            return CHTLNodeType::IMPORT_JAVASCRIPT_NODE;
        case ImportType::CHTL_IMPORT:
            return CHTLNodeType::IMPORT_CHTL_NODE;
        case ImportType::CJMOD_IMPORT:
            return CHTLNodeType::IMPORT_CJMOD_NODE;
        case ImportType::CONFIG_IMPORT:
            return CHTLNodeType::IMPORT_CONFIG_NODE;
        case ImportType::TEMPLATE_IMPORT:
            return CHTLNodeType::IMPORT_TEMPLATE_NODE;
        case ImportType::CUSTOM_IMPORT:
            return CHTLNodeType::IMPORT_CUSTOM_NODE;
        case ImportType::ORIGIN_IMPORT:
            return CHTLNodeType::IMPORT_ORIGIN_NODE;
        default:
            return CHTLNodeType::IMPORT_NODE;
    }
}

std::string ImportNode::toString() const {
    std::stringstream ss;
    
    ss << "[Import] ";
    
    // 输出导入类型标识
    switch (m_importType) {
        case ImportType::HTML_IMPORT:
            ss << "@Html";
            break;
        case ImportType::STYLE_IMPORT:
            ss << "@Style";
            break;
        case ImportType::JAVASCRIPT_IMPORT:
            ss << "@JavaScript";
            break;
        case ImportType::CHTL_IMPORT:
            ss << "@Chtl";
            break;
        case ImportType::CJMOD_IMPORT:
            ss << "@CJmod";
            break;
        case ImportType::CONFIG_IMPORT:
            ss << "@Config";
            break;
        case ImportType::TEMPLATE_IMPORT:
            ss << "[Template] " << m_symbolType;
            break;
        case ImportType::CUSTOM_IMPORT:
            ss << "[Custom] " << m_symbolType;
            break;
        case ImportType::ORIGIN_IMPORT:
            ss << "[Origin] " << m_symbolType;
            break;
        case ImportType::ALL_TEMPLATES_IMPORT:
            ss << "[Template]";
            break;
        case ImportType::ALL_CUSTOMS_IMPORT:
            ss << "[Custom]";
            break;
        case ImportType::ALL_ORIGINS_IMPORT:
            ss << "[Origin]";
            break;
        case ImportType::ALL_CONFIGS_IMPORT:
            ss << "[Configuration]";
            break;
    }
    
    // 输出符号名称（如果有）
    if (!m_symbolName.empty()) {
        ss << " " << m_symbolName;
    }
    
    // 输出from路径
    if (!m_importPath.empty()) {
        ss << " from " << m_importPath;
    }
    
    // 输出别名（如果有）
    if (hasAlias()) {
        ss << " as " << m_alias;
    }
    
    ss << ";";
    
    return ss.str();
}

NodePtr ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(m_importType);
    cloned->m_pathType = m_pathType;
    cloned->m_importPath = m_importPath;
    cloned->m_symbolName = m_symbolName;
    cloned->m_symbolType = m_symbolType;
    cloned->m_alias = m_alias;
    cloned->m_sourceNamespace = m_sourceNamespace;
    cloned->m_importedSymbols = m_importedSymbols;
    return cloned;
}

bool ImportNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    
    // 基本验证
    if (m_importPath.empty() && !isBatchImport()) {
        return false;
    }
    
    if (isSymbolImport() && m_symbolName.empty() && !isBatchImport()) {
        return false;
    }
    
    return true;
}

// ========== ImportManager 实现 ==========

ImportManager::ImportManager() {
    // 添加默认搜索路径
    m_searchPaths.push_back(".");
    m_searchPaths.push_back("./module");
}

void ImportManager::setOfficialModuleDirectory(const std::string& directory) {
    m_officialModuleDirectory = directory;
}

void ImportManager::addSearchPath(const std::string& path) {
    if (std::find(m_searchPaths.begin(), m_searchPaths.end(), path) == m_searchPaths.end()) {
        m_searchPaths.push_back(path);
    }
}

std::string ImportManager::resolveImportPath(const ImportNode& importNode, const std::string& currentFile) const {
    std::string importPath = importNode.getImportPath();
    ImportType importType = importNode.getImportType();
    ImportPathType pathType = importNode.getImportPathType();
    
    // 处理不同的路径类型
    switch (pathType) {
        case ImportPathType::FULL_PATH:
            // 直接使用完整路径
            return importPath;
            
        case ImportPathType::FILENAME_WITH_EXT:
            // 在搜索路径中查找文件
            return searchFile(importPath, getSearchPaths(currentFile));
            
        case ImportPathType::FILENAME_ONLY:
        case ImportPathType::MODULE_NAME: {
            // 添加适当的扩展名并搜索
            std::string extension = getFileExtension(importType);
            std::string filename = importPath + extension;
            return searchFile(filename, getSearchPaths(currentFile));
        }
        
        case ImportPathType::QUALIFIED_NAME: {
            // 转换限定名为路径
            std::string path = importPath;
            std::replace(path.begin(), path.end(), '.', '/');
            std::string extension = getFileExtension(importType);
            std::string filename = path + extension;
            return searchFile(filename, getSearchPaths(currentFile));
        }
        
        default:
            return importPath;
    }
}

std::vector<std::string> ImportManager::getSearchPaths(const std::string& currentFile) const {
    std::vector<std::string> searchPaths;
    
    // 对于CHTL和CJMOD导入，优先搜索官方模块目录
    if (!m_officialModuleDirectory.empty()) {
        searchPaths.push_back(m_officialModuleDirectory);
    }
    
    // 添加当前文件所在目录
    if (!currentFile.empty()) {
        std::filesystem::path currentPath(currentFile);
        if (currentPath.has_parent_path()) {
            searchPaths.push_back(currentPath.parent_path().string());
            
            // 添加当前目录的module子目录
            std::string moduleDir = currentPath.parent_path().string() + "/module";
            if (std::filesystem::exists(moduleDir)) {
                searchPaths.push_back(moduleDir);
            }
        }
    }
    
    // 添加配置的搜索路径
    for (const auto& path : m_searchPaths) {
        searchPaths.push_back(path);
    }
    
    return searchPaths;
}

bool ImportManager::loadModule(const std::string& modulePath) {
    // 检查是否已经加载
    if (std::find(m_importedModules.begin(), m_importedModules.end(), modulePath) != m_importedModules.end()) {
        return true;
    }
    
    // 检查文件是否存在
    if (!fileExists(modulePath)) {
        return false;
    }
    
    // 标记为已加载
    m_importedModules.push_back(modulePath);
    
    // 实际的模块加载逻辑在这里实现
    // 这里简化为返回true
    return true;
}

bool ImportManager::fileExists(const std::string& filePath) const {
    return std::filesystem::exists(filePath);
}

std::string ImportManager::getFileExtension(const ImportType& importType) const {
    switch (importType) {
        case ImportType::HTML_IMPORT:
            return ".html";
        case ImportType::STYLE_IMPORT:
            return ".css";
        case ImportType::JAVASCRIPT_IMPORT:
            return ".js";
        case ImportType::CHTL_IMPORT:
            return ".chtl";
        case ImportType::CJMOD_IMPORT:
            return ".cjmod";
        default:
            return "";
    }
}

std::string ImportManager::searchModuleFile(const std::string& moduleName, const ImportType& importType) const {
    std::string extension = getFileExtension(importType);
    std::string filename = moduleName + extension;
    
    // 在搜索路径中查找
    for (const auto& searchPath : m_searchPaths) {
        std::string fullPath = searchPath + "/" + filename;
        if (fileExists(fullPath)) {
            return fullPath;
        }
        
        // 对于CMOD文件，也检查.cmod扩展名
        if (importType == ImportType::CHTL_IMPORT) {
            std::string cmodPath = searchPath + "/" + moduleName + ".cmod";
            if (fileExists(cmodPath)) {
                return cmodPath;
            }
        }
    }
    
    return "";
}

bool ImportManager::processImport(std::shared_ptr<ImportNode> importNode, const std::string& currentFile) {
    if (!importNode) {
        return false;
    }
    
    std::string resolvedPath = resolveImportPath(*importNode, currentFile);
    if (resolvedPath.empty()) {
        return false;
    }
    
    // 检查循环导入
    if (hasCircularImport(resolvedPath)) {
        return false;
    }
    
    // 加载模块
    return loadModule(resolvedPath);
}

const std::vector<std::string>& ImportManager::getImportedModules() const {
    return m_importedModules;
}

bool ImportManager::hasCircularImport(const std::string& filePath) const {
    // 简化的循环导入检测
    return std::find(m_importedModules.begin(), m_importedModules.end(), filePath) != m_importedModules.end();
}

std::string ImportManager::searchFile(const std::string& filename, const std::vector<std::string>& searchDirs) const {
    for (const auto& dir : searchDirs) {
        std::string fullPath = dir + "/" + filename;
        if (fileExists(fullPath)) {
            return fullPath;
        }
    }
    return "";
}

std::string ImportManager::normalizePath(const std::string& path) const {
    // 简化的路径规范化
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

bool ImportManager::checkModuleStructure(const std::string& directory) const {
    // 检查模块目录结构是否符合CMOD/CJMOD标准
    return std::filesystem::exists(directory) && std::filesystem::is_directory(directory);
}

// ========== UseNode 实现 ==========

UseNode::UseNode() : BaseNode(CHTLNodeType::USE_NODE, "use") {
}

void UseNode::setUseContent(const std::string& content) {
    m_useContent = content;
}

const std::string& UseNode::getUseContent() const {
    return m_useContent;
}

bool UseNode::isHtml5Declaration() const {
    return m_useContent == "html5";
}

bool UseNode::isConfigUsage() const {
    return m_useContent.find("@Config") != std::string::npos || 
           m_useContent.find("[Configuration]") != std::string::npos;
}

std::string UseNode::getConfigName() const {
    if (!isConfigUsage()) {
        return "";
    }
    
    // 解析配置名称
    size_t configPos = m_useContent.find("@Config");
    if (configPos != std::string::npos) {
        size_t nameStart = configPos + 7; // "@Config".length()
        while (nameStart < m_useContent.length() && std::isspace(m_useContent[nameStart])) {
            nameStart++;
        }
        
        size_t nameEnd = nameStart;
        while (nameEnd < m_useContent.length() && 
               (std::isalnum(m_useContent[nameEnd]) || m_useContent[nameEnd] == '_')) {
            nameEnd++;
        }
        
        if (nameEnd > nameStart) {
            return m_useContent.substr(nameStart, nameEnd - nameStart);
        }
    }
    
    return "";
}

std::string UseNode::toString() const {
    return "use " + m_useContent + ";";
}

NodePtr UseNode::clone() const {
    auto cloned = std::make_shared<UseNode>();
    cloned->m_useContent = m_useContent;
    return cloned;
}

bool UseNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    return !m_useContent.empty();
}

} // namespace CHTL