#ifndef IMPORTMANAGER_H
#define IMPORTMANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include "ConfigurationManager.h"

namespace CHTL {

/**
 * @brief 导入类型枚举
 */
enum class ImportType {
    HTML,           // @Html
    STYLE,          // @Style
    JAVASCRIPT,     // @JavaScript
    CHTL,           // @Chtl
    CJMOD,          // @CJmod
    CONFIG,         // @Config
    CUSTOM          // 自定义类型
};

/**
 * @brief 导入语句结构
 */
struct ImportStatement {
    ImportType type;
    std::string path;                    // 导入路径
    std::string alias;                   // 别名（as语法）
    std::string configGroup;             // 配置组名称
    bool isWildcard;                     // 是否通配符导入
    std::string wildcardPattern;         // 通配符模式
    std::vector<std::string> subModules; // 子模块列表
    int lineNumber;                      // 行号
    int columnNumber;                    // 列号
    
    ImportStatement() : type(ImportType::HTML), isWildcard(false), lineNumber(0), columnNumber(0) {}
};

/**
 * @brief 导入结果结构
 */
struct ImportResult {
    bool success;                        // 是否成功
    std::string filePath;                // 实际文件路径
    std::string content;                 // 文件内容
    std::string errorMessage;            // 错误信息
    std::vector<std::string> warnings;   // 警告信息
    std::vector<std::string> importedFiles; // 导入的文件列表（通配符导入时）
    
    ImportResult() : success(false) {}
};

/**
 * @brief 导入管理器
 * 负责管理CHTL项目的所有导入操作
 */
class ImportManager {
public:
    /**
     * @brief 构造函数
     * @param configManager 配置管理器指针
     * @param basePath 基础路径
     */
    ImportManager(std::shared_ptr<ConfigurationManager> configManager, const std::string& basePath = "");
    
    /**
     * @brief 析构函数
     */
    ~ImportManager();
    
    /**
     * @brief 解析导入语句
     * @param source 源代码
     * @return 解析结果列表
     */
    std::vector<std::shared_ptr<ImportStatement>> parse(const std::string& source);
    
    /**
     * @brief 执行导入操作
     * @param statement 导入语句
     * @return 导入结果
     */
    ImportResult executeImport(const std::shared_ptr<ImportStatement>& statement);
    
    /**
     * @brief 解析通配符路径
     * @param pattern 通配符模式
     * @param basePath 基础路径
     * @return 匹配的文件列表
     */
    std::vector<std::string> resolveWildcardPath(const std::string& pattern, const std::string& basePath);
    
    /**
     * @brief 解析子模块路径
     * @param modulePath 模块路径
     * @param basePath 基础路径
     * @return 子模块文件列表
     */
    std::vector<std::string> resolveSubModulePath(const std::string& modulePath, const std::string& basePath);
    
    /**
     * @brief 搜索文件
     * @param fileName 文件名
     * @param importType 导入类型
     * @param searchPaths 搜索路径列表
     * @return 找到的文件路径
     */
    std::string searchFile(const std::string& fileName, ImportType importType, const std::vector<std::string>& searchPaths);
    
    /**
     * @brief 获取搜索路径
     * @param importType 导入类型
     * @return 搜索路径列表
     */
    std::vector<std::string> getSearchPaths(ImportType importType);
    
    /**
     * @brief 检查循环依赖
     * @param filePath 文件路径
     * @param importedFile 导入的文件
     * @return 是否存在循环依赖
     */
    bool checkCircularDependency(const std::string& filePath, const std::string& importedFile);
    
    /**
     * @brief 检查重复导入
     * @param filePath 文件路径
     * @param importedFile 导入的文件
     * @return 是否重复导入
     */
    bool checkDuplicateImport(const std::string& filePath, const std::string& importedFile);
    
    /**
     * @brief 设置基础路径
     * @param basePath 基础路径
     */
    void setBasePath(const std::string& basePath);
    
    /**
     * @brief 获取基础路径
     * @return 基础路径
     */
    std::string getBasePath() const;
    
    /**
     * @brief 获取导入历史
     * @return 导入历史映射
     */
    const std::unordered_map<std::string, std::vector<std::string>>& getImportHistory() const;
    
    /**
     * @brief 清除导入历史
     */
    void clearImportHistory();
    
    /**
     * @brief 获取错误信息
     * @return 错误信息列表
     */
    std::vector<std::string> getErrors() const;
    
    /**
     * @brief 清除错误信息
     */
    void clearErrors();

private:
    std::shared_ptr<ConfigurationManager> configManager_;
    std::string basePath_;
    std::vector<std::string> errors_;
    
    // 导入历史：文件路径 -> 导入的文件列表
    std::unordered_map<std::string, std::vector<std::string>> importHistory_;
    
    // 循环依赖检测：当前导入链
    std::vector<std::string> currentImportChain_;
    
    /**
     * @brief 添加错误信息
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * @brief 解析导入语句行
     * @param line 导入语句行
     * @param lineNumber 行号
     * @return 导入语句
     */
    std::shared_ptr<ImportStatement> parseImportLine(const std::string& line, int lineNumber);
    
    /**
     * @brief 解析导入类型
     * @param typeText 类型文本
     * @return 导入类型
     */
    ImportType parseImportType(const std::string& typeText);
    
    /**
     * @brief 解析路径和别名
     * @param pathText 路径文本
     * @param statement 导入语句
     */
    void parsePathAndAlias(const std::string& pathText, std::shared_ptr<ImportStatement>& statement);
    
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    bool fileExists(const std::string& filePath);
    
    /**
     * @brief 读取文件内容
     * @param filePath 文件路径
     * @return 文件内容
     */
    std::string readFileContent(const std::string& filePath);
    
    /**
     * @brief 获取文件扩展名
     * @param filePath 文件路径
     * @return 文件扩展名
     */
    std::string getFileExtension(const std::string& filePath);
    
    /**
     * @brief 标准化路径
     * @param path 路径
     * @return 标准化后的路径
     */
    std::string normalizePath(const std::string& path);
    
    /**
     * @brief 合并路径
     * @param base 基础路径
     * @param relative 相对路径
     * @return 合并后的路径
     */
    std::string combinePath(const std::string& base, const std::string& relative);
    
    /**
     * @brief 检查是否为目录
     * @param path 路径
     * @return 是否为目录
     */
    bool isDirectory(const std::string& path);
    
    /**
     * @brief 获取目录中的文件
     * @param dirPath 目录路径
     * @param extensions 文件扩展名列表
     * @return 文件列表
     */
    std::vector<std::string> getFilesInDirectory(const std::string& dirPath, const std::vector<std::string>& extensions);
    
    /**
     * @brief 匹配通配符模式
     * @param pattern 通配符模式
     * @param fileName 文件名
     * @return 是否匹配
     */
    bool matchWildcardPattern(const std::string& pattern, const std::string& fileName);
    
    /**
     * @brief 解析模块路径
     * @param modulePath 模块路径
     * @return 解析后的路径组件
     */
    std::vector<std::string> parseModulePath(const std::string& modulePath);
};

} // namespace CHTL

#endif // IMPORTMANAGER_H