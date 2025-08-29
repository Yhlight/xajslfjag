#pragma once
#include "../../Util/FileSystem/FileSystem.h"
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace CHTL {

/**
 * CHTL文件类型枚举
 */
enum class CHTLFileType {
    CHTL_SOURCE,        // .chtl源文件
    CMOD_MODULE,        // .cmod模块文件
    CJMOD_MODULE,       // .cjmod模块文件
    HTML_FILE,          // .html文件
    CSS_FILE,           // .css文件
    JS_FILE,            // .js文件
    CJJS_FILE,          // .cjjs文件
    UNKNOWN_FILE        // 未知文件类型
};

/**
 * CHTL编码格式
 */
enum class CHTLEncoding {
    UTF8,               // UTF-8编码
    UTF16,              // UTF-16编码
    UTF32,              // UTF-32编码
    ASCII,              // ASCII编码
    AUTO_DETECT         // 自动检测
};

/**
 * CHTL文件信息
 */
struct CHTLFileInfo {
    std::string path;           // 文件路径
    std::string name;           // 文件名
    CHTLFileType type;          // 文件类型
    CHTLEncoding encoding;      // 编码格式
    size_t size;               // 文件大小
    std::chrono::system_clock::time_point lastModified; // 最后修改时间
    bool hasUTF8BOM;           // 是否有UTF-8 BOM
    
    CHTLFileInfo() : type(CHTLFileType::UNKNOWN_FILE), encoding(CHTLEncoding::UTF8), 
                     size(0), hasUTF8BOM(false) {}
};

/**
 * CHTL文件读取器
 * 专门用于读取CHTL相关文件，支持编码检测和转换
 */
class CHTLFileReader {
public:
    /**
     * 构造函数
     */
    explicit CHTLFileReader(const std::string& filePath);
    
    /**
     * 析构函数
     */
    ~CHTLFileReader() = default;
    
    /**
     * 打开文件
     */
    bool open();
    
    /**
     * 关闭文件
     */
    void close();
    
    /**
     * 检查是否已打开
     */
    bool isOpen() const;
    
    /**
     * 获取文件信息
     */
    const CHTLFileInfo& getFileInfo() const;
    
    /**
     * 读取整个文件内容
     */
    std::string readAll();
    
    /**
     * 按行读取文件
     */
    std::vector<std::string> readLines();
    
    /**
     * 读取指定行
     */
    std::string readLine(size_t lineNumber);
    
    /**
     * 读取指定范围的行
     */
    std::vector<std::string> readLineRange(size_t startLine, size_t endLine);
    
    /**
     * 获取总行数
     */
    size_t getLineCount();
    
    /**
     * 检测文件编码
     */
    CHTLEncoding detectEncoding();
    
    /**
     * 设置目标编码
     */
    void setTargetEncoding(CHTLEncoding encoding);
    
    /**
     * 检查文件是否有BOM
     */
    bool hasBOM() const;

private:
    std::string m_filePath;
    CHTLFileInfo m_fileInfo;
    std::unique_ptr<Util::FileReader> m_reader;
    CHTLEncoding m_targetEncoding;
    std::vector<std::string> m_cachedLines;
    bool m_linesCached;
    
    /**
     * 分析文件信息
     */
    void analyzeFile();
    
    /**
     * 确定文件类型
     */
    CHTLFileType determineFileType(const std::string& filePath);
    
    /**
     * 转换编码
     */
    std::string convertEncoding(const std::string& content, CHTLEncoding from, CHTLEncoding to);
    
    /**
     * 移除BOM
     */
    std::string removeBOM(const std::string& content);
    
    /**
     * 缓存行数据
     */
    void cacheLines();
};

/**
 * CHTL文件写入器
 * 专门用于写入CHTL相关文件，支持编码和格式化
 */
class CHTLFileWriter {
public:
    /**
     * 写入选项
     */
    struct WriteOptions {
        CHTLEncoding encoding = CHTLEncoding::UTF8;     // 输出编码
        bool addBOM = false;                            // 是否添加BOM
        bool createDirectory = true;                    // 是否自动创建目录
        bool backup = false;                            // 是否备份原文件
        std::string indentation = "    ";               // 缩进字符
        std::string lineEnding = "\n";                  // 行结束符
        bool prettyPrint = true;                        // 是否格式化输出
    };
    
    /**
     * 构造函数
     */
    explicit CHTLFileWriter(const std::string& filePath, const WriteOptions& options = WriteOptions());
    
    /**
     * 析构函数
     */
    ~CHTLFileWriter() = default;
    
    /**
     * 打开文件
     */
    bool open();
    
    /**
     * 关闭文件
     */
    void close();
    
    /**
     * 检查是否已打开
     */
    bool isOpen() const;
    
    /**
     * 写入内容
     */
    bool write(const std::string& content);
    
    /**
     * 写入一行
     */
    bool writeLine(const std::string& line, size_t indentLevel = 0);
    
    /**
     * 写入多行
     */
    bool writeLines(const std::vector<std::string>& lines);
    
    /**
     * 写入CHTL源码（格式化）
     */
    bool writeCHTLSource(const std::string& content);
    
    /**
     * 写入HTML（格式化）
     */
    bool writeHTML(const std::string& content);
    
    /**
     * 写入CSS（格式化）
     */
    bool writeCSS(const std::string& content);
    
    /**
     * 写入JavaScript（格式化）
     */
    bool writeJavaScript(const std::string& content);
    
    /**
     * 设置缩进级别
     */
    void setIndentLevel(size_t level);
    
    /**
     * 增加缩进级别
     */
    void increaseIndent();
    
    /**
     * 减少缩进级别
     */
    void decreaseIndent();
    
    /**
     * 刷新缓冲区
     */
    void flush();

private:
    std::string m_filePath;
    WriteOptions m_options;
    std::unique_ptr<Util::FileWriter> m_writer;
    size_t m_currentIndentLevel;
    
    /**
     * 添加BOM
     */
    std::string addBOM(const std::string& content);
    
    /**
     * 转换编码
     */
    std::string convertToTargetEncoding(const std::string& content);
    
    /**
     * 格式化内容
     */
    std::string formatContent(const std::string& content, CHTLFileType type);
    
    /**
     * 创建备份
     */
    bool createBackup();
    
    /**
     * 获取缩进字符串
     */
    std::string getIndentString(size_t level = 0) const;
};

/**
 * CHTL模块加载器
 * 用于加载和管理CHTL模块
 */
class CHTLModuleLoader {
public:
    /**
     * 模块搜索路径类型
     */
    enum class SearchPathType {
        OFFICIAL_MODULE,        // 官方模块目录
        LOCAL_MODULE,          // 本地模块目录
        CURRENT_DIRECTORY,     // 当前目录
        CUSTOM_PATH           // 自定义路径
    };
    
    /**
     * 模块信息
     */
    struct ModuleInfo {
        std::string name;           // 模块名
        std::string path;           // 模块路径
        CHTLFileType type;          // 模块类型
        std::string version;        // 版本信息
        std::vector<std::string> dependencies; // 依赖关系
        bool isLoaded;             // 是否已加载
        
        ModuleInfo() : type(CHTLFileType::UNKNOWN_FILE), isLoaded(false) {}
    };
    
    /**
     * 构造函数
     */
    CHTLModuleLoader();
    
    /**
     * 添加搜索路径
     */
    void addSearchPath(const std::string& path, SearchPathType type = SearchPathType::CUSTOM_PATH);
    
    /**
     * 设置官方模块目录
     */
    void setOfficialModulePath(const std::string& path);
    
    /**
     * 加载模块
     */
    std::shared_ptr<CHTLFileReader> loadModule(const std::string& moduleName);
    
    /**
     * 查找模块
     */
    std::string findModule(const std::string& moduleName, CHTLFileType preferredType = CHTLFileType::CMOD_MODULE);
    
    /**
     * 获取模块信息
     */
    ModuleInfo getModuleInfo(const std::string& moduleName);
    
    /**
     * 检查模块是否存在
     */
    bool moduleExists(const std::string& moduleName);
    
    /**
     * 列出可用模块
     */
    std::vector<std::string> listAvailableModules();
    
    /**
     * 解析模块依赖
     */
    std::vector<std::string> resolveDependencies(const std::string& moduleName);
    
    /**
     * 检查循环依赖
     */
    bool hasCircularDependency(const std::string& moduleName);

private:
    std::vector<std::pair<std::string, SearchPathType>> m_searchPaths;
    std::unordered_map<std::string, ModuleInfo> m_moduleCache;
    std::string m_officialModulePath;
    
    /**
     * 在指定路径搜索模块
     */
    std::string searchInPath(const std::string& path, const std::string& moduleName, CHTLFileType type);
    
    /**
     * 解析模块元信息
     */
    ModuleInfo parseModuleInfo(const std::string& modulePath);
    
    /**
     * 递归检查依赖
     */
    bool checkDependencyRecursive(const std::string& moduleName, const std::string& target, 
                                 std::unordered_set<std::string>& visited);
};

/**
 * CHTL项目管理器
 * 管理整个CHTL项目的文件结构
 */
class CHTLProjectManager {
public:
    /**
     * 项目结构信息
     */
    struct ProjectInfo {
        std::string name;               // 项目名称
        std::string rootPath;           // 项目根路径
        std::string version;            // 项目版本
        std::vector<std::string> sourceFiles;  // 源文件列表
        std::vector<std::string> dependencies; // 依赖模块
        std::string mainFile;           // 主文件
        std::string outputPath;         // 输出路径
        
        ProjectInfo() = default;
    };
    
    /**
     * 构造函数
     */
    explicit CHTLProjectManager(const std::string& projectPath);
    
    /**
     * 初始化项目
     */
    bool initializeProject(const std::string& projectName);
    
    /**
     * 加载项目配置
     */
    bool loadProject();
    
    /**
     * 保存项目配置
     */
    bool saveProject();
    
    /**
     * 获取项目信息
     */
    const ProjectInfo& getProjectInfo() const;
    
    /**
     * 设置项目信息
     */
    void setProjectInfo(const ProjectInfo& info);
    
    /**
     * 添加源文件
     */
    void addSourceFile(const std::string& filePath);
    
    /**
     * 移除源文件
     */
    void removeSourceFile(const std::string& filePath);
    
    /**
     * 获取所有源文件
     */
    std::vector<std::string> getSourceFiles() const;
    
    /**
     * 查找项目文件
     */
    std::vector<std::string> findProjectFiles(const std::string& pattern = "*.chtl");
    
    /**
     * 创建项目目录结构
     */
    bool createProjectStructure();
    
    /**
     * 验证项目结构
     */
    bool validateProjectStructure();
    
    /**
     * 获取模块加载器
     */
    CHTLModuleLoader& getModuleLoader();

private:
    std::string m_projectPath;
    ProjectInfo m_projectInfo;
    std::unique_ptr<CHTLModuleLoader> m_moduleLoader;
    
    /**
     * 项目配置文件路径
     */
    std::string getConfigFilePath() const;
    
    /**
     * 解析项目配置
     */
    bool parseProjectConfig(const std::string& configContent);
    
    /**
     * 生成项目配置
     */
    std::string generateProjectConfig() const;
};

/**
 * CHTL导入解析器
 * 解析CHTL文件中的导入语句
 */
class CHTLImportResolver {
public:
    /**
     * 导入信息
     */
    struct ImportInfo {
        std::string type;           // 导入类型 (@Html, @Style, @Chtl, etc.)
        std::string target;         // 导入目标
        std::string source;         // 源路径
        std::string alias;          // 别名
        std::vector<std::string> constraints; // 约束条件
        bool hasAs;                 // 是否有as关键字
        
        ImportInfo() : hasAs(false) {}
    };
    
    /**
     * 构造函数
     */
    explicit CHTLImportResolver(CHTLProjectManager* projectManager);
    
    /**
     * 解析导入语句
     */
    std::vector<ImportInfo> parseImports(const std::string& content);
    
    /**
     * 解析单个导入语句
     */
    ImportInfo parseImport(const std::string& importStatement);
    
    /**
     * 解析文件中的所有导入
     */
    std::vector<ImportInfo> parseFileImports(const std::string& filePath);
    
    /**
     * 解析导入依赖关系
     */
    std::vector<std::string> resolveImportDependencies(const std::string& filePath);
    
    /**
     * 检查导入循环依赖
     */
    bool hasImportCircularDependency(const std::string& filePath);
    
    /**
     * 获取导入文件的完整路径
     */
    std::string resolveImportPath(const ImportInfo& importInfo, const std::string& currentFilePath);

private:
    CHTLProjectManager* m_projectManager;
    
    /**
     * 解析导入路径
     */
    std::string parseImportPath(const std::string& pathStr, const std::string& currentDir);
    
    /**
     * 解析导入类型
     */
    std::string parseImportType(const std::string& typeStr);
    
    /**
     * 递归检查循环依赖
     */
    bool checkImportCircularDependency(const std::string& filePath, const std::string& target,
                                      std::unordered_set<std::string>& visited);
};

} // namespace CHTL