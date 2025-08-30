#include "CHTLIOStream.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <fstream>

namespace CHTL {

// ========== CHTLFileReader 实现 ==========

CHTLFileReader::CHTLFileReader(const std::string& filePath)
    : m_filePath(filePath)
    , m_targetEncoding(CHTLEncoding::UTF8)
    , m_linesCached(false) {
    analyzeFile();
}

bool CHTLFileReader::open() {
    m_reader = std::make_unique<Util::FileReader>(m_filePath);
    return m_reader->open();
}

void CHTLFileReader::close() {
    if (m_reader) {
        m_reader->close();
        m_reader.reset();
    }
}

bool CHTLFileReader::isOpen() const {
    return m_reader && m_reader->isOpen();
}

const CHTLFileInfo& CHTLFileReader::getFileInfo() const {
    return m_fileInfo;
}

std::string CHTLFileReader::readAll() {
    if (!isOpen()) return "";
    
    std::string content = m_reader->readAll();
    
    // 移除BOM
    content = removeBOM(content);
    
    // 编码转换
    if (m_fileInfo.encoding != m_targetEncoding) {
        content = convertEncoding(content, m_fileInfo.encoding, m_targetEncoding);
    }
    
    return content;
}

std::vector<std::string> CHTLFileReader::readLines() {
    if (!m_linesCached) {
        cacheLines();
    }
    return m_cachedLines;
}

std::string CHTLFileReader::readLine(size_t lineNumber) {
    auto lines = readLines();
    if (lineNumber >= lines.size()) {
        return "";
    }
    return lines[lineNumber];
}

std::vector<std::string> CHTLFileReader::readLineRange(size_t startLine, size_t endLine) {
    auto lines = readLines();
    if (startLine >= lines.size()) {
        return {};
    }
    
    endLine = std::min(endLine, lines.size() - 1);
    if (startLine > endLine) {
        return {};
    }
    
    return std::vector<std::string>(lines.begin() + startLine, lines.begin() + endLine + 1);
}

size_t CHTLFileReader::getLineCount() {
    return readLines().size();
}

CHTLEncoding CHTLFileReader::detectEncoding() {
    if (!isOpen()) return CHTLEncoding::UTF8;
    
    // 简化的编码检测
    std::string content = m_reader->readAll();
    
    // 检查BOM
    if (content.size() >= 3) {
        if (content.substr(0, 3) == "\xEF\xBB\xBF") {
            return CHTLEncoding::UTF8;
        }
    }
    
    if (content.size() >= 2) {
        if (content.substr(0, 2) == "\xFF\xFE" || content.substr(0, 2) == "\xFE\xFF") {
            return CHTLEncoding::UTF16;
        }
    }
    
    // 默认为UTF8
    return CHTLEncoding::UTF8;
}

void CHTLFileReader::setTargetEncoding(CHTLEncoding encoding) {
    m_targetEncoding = encoding;
}

bool CHTLFileReader::hasBOM() const {
    return m_fileInfo.hasUTF8BOM;
}

void CHTLFileReader::analyzeFile() {
    m_fileInfo.path = m_filePath;
    m_fileInfo.name = Util::Path::getFileName(m_filePath);
    m_fileInfo.type = determineFileType(m_filePath);
    
    if (Util::FileSystem::exists(m_filePath)) {
        auto sysFileInfo = Util::FileSystem::getFileInfo(m_filePath);
        m_fileInfo.size = sysFileInfo.size;
        m_fileInfo.lastModified = sysFileInfo.lastModified;
        
        // 检测编码
        if (open()) {
            m_fileInfo.encoding = detectEncoding();
            close();
        }
    }
}

CHTLFileType CHTLFileReader::determineFileType(const std::string& filePath) {
    std::string extension = Util::Path::getExtension(filePath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "chtl") return CHTLFileType::CHTL_SOURCE;
    if (extension == "cmod") return CHTLFileType::CMOD_MODULE;
    if (extension == "cjmod") return CHTLFileType::CJMOD_MODULE;
    if (extension == "html" || extension == "htm") return CHTLFileType::HTML_FILE;
    if (extension == "css") return CHTLFileType::CSS_FILE;
    if (extension == "js") return CHTLFileType::JS_FILE;
    if (extension == "cjjs") return CHTLFileType::CJJS_FILE;
    
    return CHTLFileType::UNKNOWN_FILE;
}

std::string CHTLFileReader::convertEncoding(const std::string& content, CHTLEncoding from, CHTLEncoding to) {
    // 简化实现 - 实际项目中需要使用专门的编码转换库
    if (from == to) return content;
    
    // 目前只支持UTF8到其他编码的基本转换
    return content;
}

std::string CHTLFileReader::removeBOM(const std::string& content) {
    if (content.size() >= 3 && content.substr(0, 3) == "\xEF\xBB\xBF") {
        m_fileInfo.hasUTF8BOM = true;
        return content.substr(3);
    }
    return content;
}

void CHTLFileReader::cacheLines() {
    std::string content = readAll();
    std::stringstream ss(content);
    std::string line;
    
    m_cachedLines.clear();
    while (std::getline(ss, line)) {
        m_cachedLines.push_back(line);
    }
    
    m_linesCached = true;
}

// ========== CHTLFileWriter 实现 ==========

CHTLFileWriter::CHTLFileWriter(const std::string& filePath, const WriteOptions& options)
    : m_filePath(filePath)
    , m_options(options)
    , m_currentIndentLevel(0) {
}

bool CHTLFileWriter::open() {
    // 创建目录
    if (m_options.createDirectory) {
        std::string parentDir = Util::Path::getParent(m_filePath);
        if (!parentDir.empty() && !Util::FileSystem::exists(parentDir)) {
            Util::FileSystem::createDirectory(parentDir, true);
        }
    }
    
    // 创建备份
    if (m_options.backup && Util::FileSystem::exists(m_filePath)) {
        createBackup();
    }
    
    Util::FileWriter::WriteMode mode = Util::FileWriter::WriteMode::OVERWRITE;
    m_writer = std::make_unique<Util::FileWriter>(m_filePath, mode);
    return m_writer->open();
}

void CHTLFileWriter::close() {
    if (m_writer) {
        m_writer->close();
        m_writer.reset();
    }
}

bool CHTLFileWriter::isOpen() const {
    return m_writer && m_writer->isOpen();
}

bool CHTLFileWriter::write(const std::string& content) {
    if (!isOpen()) return false;
    
    std::string processedContent = content;
    
    // 添加BOM
    if (m_options.addBOM && m_options.encoding == CHTLEncoding::UTF8) {
        processedContent = addBOM(processedContent);
    }
    
    // 编码转换
    processedContent = convertToTargetEncoding(processedContent);
    
    return m_writer->write(processedContent);
}

bool CHTLFileWriter::writeLine(const std::string& line, size_t indentLevel) {
    if (!isOpen()) return false;
    
    std::string indentedLine = getIndentString(indentLevel) + line + m_options.lineEnding;
    return write(indentedLine);
}

bool CHTLFileWriter::writeLines(const std::vector<std::string>& lines) {
    if (!isOpen()) return false;
    
    for (const auto& line : lines) {
        if (!writeLine(line, m_currentIndentLevel)) {
            return false;
        }
    }
    
    return true;
}

bool CHTLFileWriter::writeCHTLSource(const std::string& content) {
    if (!isOpen()) return false;
    
    std::string formattedContent = content;
    if (m_options.prettyPrint) {
        formattedContent = formatContent(content, CHTLFileType::CHTL_SOURCE);
    }
    
    return write(formattedContent);
}

bool CHTLFileWriter::writeHTML(const std::string& content) {
    if (!isOpen()) return false;
    
    std::string formattedContent = content;
    if (m_options.prettyPrint) {
        formattedContent = formatContent(content, CHTLFileType::HTML_FILE);
    }
    
    return write(formattedContent);
}

bool CHTLFileWriter::writeCSS(const std::string& content) {
    if (!isOpen()) return false;
    
    std::string formattedContent = content;
    if (m_options.prettyPrint) {
        formattedContent = formatContent(content, CHTLFileType::CSS_FILE);
    }
    
    return write(formattedContent);
}

bool CHTLFileWriter::writeJavaScript(const std::string& content) {
    if (!isOpen()) return false;
    
    std::string formattedContent = content;
    if (m_options.prettyPrint) {
        formattedContent = formatContent(content, CHTLFileType::JS_FILE);
    }
    
    return write(formattedContent);
}

void CHTLFileWriter::setIndentLevel(size_t level) {
    m_currentIndentLevel = level;
}

void CHTLFileWriter::increaseIndent() {
    m_currentIndentLevel++;
}

void CHTLFileWriter::decreaseIndent() {
    if (m_currentIndentLevel > 0) {
        m_currentIndentLevel--;
    }
}

void CHTLFileWriter::flush() {
    if (isOpen()) {
        m_writer->flush();
    }
}

std::string CHTLFileWriter::addBOM(const std::string& content) {
    return "\xEF\xBB\xBF" + content;
}

std::string CHTLFileWriter::convertToTargetEncoding(const std::string& content) {
    // 简化实现
    return content;
}

std::string CHTLFileWriter::formatContent(const std::string& content, CHTLFileType type) {
    // 简化的格式化实现
    std::string formatted = content;
    
    // 统一行结束符
    std::regex lineEndingRegex("\r\n|\r|\n");
    formatted = std::regex_replace(formatted, lineEndingRegex, m_options.lineEnding);
    
    return formatted;
}

bool CHTLFileWriter::createBackup() {
    std::string backupPath = m_filePath + ".bak";
    return Util::FileSystem::copyFile(m_filePath, backupPath);
}

std::string CHTLFileWriter::getIndentString(size_t level) const {
    if (level == 0) level = m_currentIndentLevel;
    
    std::string indent;
    for (size_t i = 0; i < level; ++i) {
        indent += m_options.indentation;
    }
    return indent;
}

// ========== CHTLModuleLoader 实现 ==========

CHTLModuleLoader::CHTLModuleLoader() {
    // 添加默认搜索路径
    addSearchPath(".", SearchPathType::CURRENT_DIRECTORY);
    
    // 尝试添加标准模块路径
    std::string homeDir = Util::Path::getHomeDirectory();
    if (!homeDir.empty()) {
        addSearchPath(Util::Path::join(homeDir, ".chtl/modules"), SearchPathType::OFFICIAL_MODULE);
    }
}

void CHTLModuleLoader::addSearchPath(const std::string& path, SearchPathType type) {
    m_searchPaths.emplace_back(path, type);
}

void CHTLModuleLoader::setOfficialModulePath(const std::string& path) {
    m_officialModulePath = path;
    
    // 更新搜索路径
    auto it = std::find_if(m_searchPaths.begin(), m_searchPaths.end(),
                          [](const auto& pair) { return pair.second == SearchPathType::OFFICIAL_MODULE; });
    
    if (it != m_searchPaths.end()) {
        it->first = path;
    } else {
        addSearchPath(path, SearchPathType::OFFICIAL_MODULE);
    }
}

std::shared_ptr<CHTLFileReader> CHTLModuleLoader::loadModule(const std::string& moduleName) {
    std::string modulePath = findModule(moduleName);
    if (modulePath.empty()) {
        return nullptr;
    }
    
    auto reader = std::make_shared<CHTLFileReader>(modulePath);
    if (reader->open()) {
        // 缓存模块信息
        ModuleInfo info = parseModuleInfo(modulePath);
        info.isLoaded = true;
        m_moduleCache[moduleName] = info;
        
        return reader;
    }
    
    return nullptr;
}

std::string CHTLModuleLoader::findModule(const std::string& moduleName, CHTLFileType preferredType) {
    // 按优先级搜索
    std::vector<CHTLFileType> searchOrder = {preferredType};
    if (preferredType != CHTLFileType::CMOD_MODULE) {
        searchOrder.push_back(CHTLFileType::CMOD_MODULE);
    }
    if (preferredType != CHTLFileType::CHTL_SOURCE) {
        searchOrder.push_back(CHTLFileType::CHTL_SOURCE);
    }
    
    for (const auto& [path, type] : m_searchPaths) {
        for (CHTLFileType fileType : searchOrder) {
            std::string found = searchInPath(path, moduleName, fileType);
            if (!found.empty()) {
                return found;
            }
        }
    }
    
    return "";
}

CHTLModuleLoader::ModuleInfo CHTLModuleLoader::getModuleInfo(const std::string& moduleName) {
    auto it = m_moduleCache.find(moduleName);
    if (it != m_moduleCache.end()) {
        return it->second;
    }
    
    std::string modulePath = findModule(moduleName);
    if (!modulePath.empty()) {
        auto info = parseModuleInfo(modulePath);
        m_moduleCache[moduleName] = info;
        return info;
    }
    
    return ModuleInfo();
}

bool CHTLModuleLoader::moduleExists(const std::string& moduleName) {
    return !findModule(moduleName).empty();
}

std::vector<std::string> CHTLModuleLoader::listAvailableModules() {
    std::vector<std::string> modules;
    
    for (const auto& [path, type] : m_searchPaths) {
        if (Util::FileSystem::exists(path) && Util::FileSystem::isDirectory(path)) {
            auto files = Util::FileSystem::listDirectory(path);
            for (const auto& file : files) {
                std::string ext = Util::Path::getExtension(file);
                if (ext == "cmod" || ext == "chtl") {
                    std::string moduleName = Util::Path::getBaseName(file);
                    if (std::find(modules.begin(), modules.end(), moduleName) == modules.end()) {
                        modules.push_back(moduleName);
                    }
                }
            }
        }
    }
    
    return modules;
}

std::vector<std::string> CHTLModuleLoader::resolveDependencies(const std::string& moduleName) {
    std::vector<std::string> dependencies;
    std::unordered_set<std::string> visited;
    
    ModuleInfo info = getModuleInfo(moduleName);
    for (const auto& dep : info.dependencies) {
        if (visited.find(dep) == visited.end()) {
            visited.insert(dep);
            dependencies.push_back(dep);
            
            // 递归解析依赖
            auto subDeps = resolveDependencies(dep);
            for (const auto& subDep : subDeps) {
                if (visited.find(subDep) == visited.end()) {
                    visited.insert(subDep);
                    dependencies.push_back(subDep);
                }
            }
        }
    }
    
    return dependencies;
}

bool CHTLModuleLoader::hasCircularDependency(const std::string& moduleName) {
    std::unordered_set<std::string> visited;
    return checkDependencyRecursive(moduleName, moduleName, visited);
}

std::string CHTLModuleLoader::searchInPath(const std::string& path, const std::string& moduleName, CHTLFileType type) {
    if (!Util::FileSystem::exists(path) || !Util::FileSystem::isDirectory(path)) {
        return "";
    }
    
    std::string extension;
    switch (type) {
        case CHTLFileType::CMOD_MODULE: extension = "cmod"; break;
        case CHTLFileType::CHTL_SOURCE: extension = "chtl"; break;
        case CHTLFileType::CJMOD_MODULE: extension = "cjmod"; break;
        default: return "";
    }
    
    std::string fullPath = Util::Path::join(path, moduleName + "." + extension);
    if (Util::FileSystem::exists(fullPath)) {
        return fullPath;
    }
    
    return "";
}

CHTLModuleLoader::ModuleInfo CHTLModuleLoader::parseModuleInfo(const std::string& modulePath) {
    ModuleInfo info;
    info.path = modulePath;
    info.name = Util::Path::getBaseName(modulePath);
    
    CHTLFileReader reader(modulePath);
    if (reader.open()) {
        info.type = reader.getFileInfo().type;
        
        // 简化的模块信息解析
        std::string content = reader.readAll();
        
        // 解析依赖 - 查找import语句
        std::regex importRegex(R"(\[Import\].*?from\s+(\w+))");
        std::sregex_iterator iter(content.begin(), content.end(), importRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            if (match.size() > 1) {
                info.dependencies.push_back(match[1].str());
            }
        }
        
        reader.close();
    }
    
    return info;
}

bool CHTLModuleLoader::checkDependencyRecursive(const std::string& moduleName, const std::string& target,
                                               std::unordered_set<std::string>& visited) {
    if (visited.find(moduleName) != visited.end()) {
        return moduleName == target;
    }
    
    visited.insert(moduleName);
    
    ModuleInfo info = getModuleInfo(moduleName);
    for (const auto& dep : info.dependencies) {
        if (checkDependencyRecursive(dep, target, visited)) {
            return true;
        }
    }
    
    return false;
}

// ========== CHTLProjectManager 实现 ==========

CHTLProjectManager::CHTLProjectManager(const std::string& projectPath)
    : m_projectPath(projectPath)
    , m_moduleLoader(std::make_unique<CHTLModuleLoader>()) {
}

bool CHTLProjectManager::initializeProject(const std::string& projectName) {
    m_projectInfo.name = projectName;
    m_projectInfo.rootPath = m_projectPath;
    m_projectInfo.version = "1.0.0";
    m_projectInfo.mainFile = "main.chtl";
    m_projectInfo.outputPath = "dist";
    
    return createProjectStructure() && saveProject();
}

bool CHTLProjectManager::loadProject() {
    std::string configPath = getConfigFilePath();
    if (!Util::FileSystem::exists(configPath)) {
        return false;
    }
    
    CHTLFileReader reader(configPath);
    if (!reader.open()) {
        return false;
    }
    
    std::string configContent = reader.readAll();
    reader.close();
    
    return parseProjectConfig(configContent);
}

bool CHTLProjectManager::saveProject() {
    std::string configPath = getConfigFilePath();
    std::string configContent = generateProjectConfig();
    
    CHTLFileWriter writer(configPath);
    if (!writer.open()) {
        return false;
    }
    
    bool success = writer.write(configContent);
    writer.close();
    
    return success;
}

const CHTLProjectManager::ProjectInfo& CHTLProjectManager::getProjectInfo() const {
    return m_projectInfo;
}

void CHTLProjectManager::setProjectInfo(const CHTLProjectManager::ProjectInfo& info) {
    m_projectInfo = info;
}

void CHTLProjectManager::addSourceFile(const std::string& filePath) {
    auto it = std::find(m_projectInfo.sourceFiles.begin(), m_projectInfo.sourceFiles.end(), filePath);
    if (it == m_projectInfo.sourceFiles.end()) {
        m_projectInfo.sourceFiles.push_back(filePath);
    }
}

void CHTLProjectManager::removeSourceFile(const std::string& filePath) {
    auto it = std::find(m_projectInfo.sourceFiles.begin(), m_projectInfo.sourceFiles.end(), filePath);
    if (it != m_projectInfo.sourceFiles.end()) {
        m_projectInfo.sourceFiles.erase(it);
    }
}

std::vector<std::string> CHTLProjectManager::getSourceFiles() const {
    return m_projectInfo.sourceFiles;
}

std::vector<std::string> CHTLProjectManager::findProjectFiles(const std::string& pattern) {
    return Util::FileSystem::findFiles(m_projectPath, pattern);
}

bool CHTLProjectManager::createProjectStructure() {
    // 创建基本目录结构
    std::vector<std::string> directories = {
        "src",
        "src/assets",
        "src/components",
        "src/styles",
        "src/scripts",
        "dist",
        "modules"
    };
    
    for (const auto& dir : directories) {
        std::string fullPath = Util::Path::join(m_projectPath, dir);
        if (!Util::FileSystem::createDirectory(fullPath, true)) {
            return false;
        }
    }
    
    // 创建主文件
    std::string mainFilePath = Util::Path::join(Util::Path::join(m_projectPath, "src"), m_projectInfo.mainFile);
    if (!Util::FileSystem::exists(mainFilePath)) {
        CHTLFileWriter writer(mainFilePath);
        if (writer.open()) {
            writer.writeLine("// " + m_projectInfo.name + " - Main CHTL File");
            writer.writeLine("");
            writer.writeLine("html");
            writer.writeLine("{");
            writer.writeLine("    head");
            writer.writeLine("    {");
            writer.writeLine("        title: \"" + m_projectInfo.name + "\";");
            writer.writeLine("    }");
            writer.writeLine("");
            writer.writeLine("    body");
            writer.writeLine("    {");
            writer.writeLine("        text");
            writer.writeLine("        {");
            writer.writeLine("            \"Hello, " + m_projectInfo.name + "!\"");
            writer.writeLine("        }");
            writer.writeLine("    }");
            writer.writeLine("}");
            writer.close();
        }
    }
    
    return true;
}

bool CHTLProjectManager::validateProjectStructure() {
    // 检查必要的目录是否存在
    std::vector<std::string> requiredDirs = {"src"};
    
    for (const auto& dir : requiredDirs) {
        std::string fullPath = Util::Path::join(m_projectPath, dir);
        if (!Util::FileSystem::exists(fullPath) || !Util::FileSystem::isDirectory(fullPath)) {
            return false;
        }
    }
    
    // 检查主文件是否存在
    if (!m_projectInfo.mainFile.empty()) {
        std::string mainFilePath = Util::Path::join(Util::Path::join(m_projectPath, "src"), m_projectInfo.mainFile);
        if (!Util::FileSystem::exists(mainFilePath)) {
            return false;
        }
    }
    
    return true;
}

CHTLModuleLoader& CHTLProjectManager::getModuleLoader() {
    return *m_moduleLoader;
}

std::string CHTLProjectManager::getConfigFilePath() const {
    return Util::Path::join(m_projectPath, "chtl.project");
}

bool CHTLProjectManager::parseProjectConfig(const std::string& configContent) {
    // 简化的配置解析 - 实际项目中可能使用JSON或YAML
    std::stringstream ss(configContent);
    std::string line;
    
    while (std::getline(ss, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // 移除空白字符
            key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (key == "name") m_projectInfo.name = value;
            else if (key == "version") m_projectInfo.version = value;
            else if (key == "mainFile") m_projectInfo.mainFile = value;
            else if (key == "outputPath") m_projectInfo.outputPath = value;
        }
    }
    
    return true;
}

std::string CHTLProjectManager::generateProjectConfig() const {
    std::stringstream ss;
    ss << "name = " << m_projectInfo.name << "\n";
    ss << "version = " << m_projectInfo.version << "\n";
    ss << "mainFile = " << m_projectInfo.mainFile << "\n";
    ss << "outputPath = " << m_projectInfo.outputPath << "\n";
    
    return ss.str();
}

} // namespace CHTL