#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace Loader {

/**
 * CHTLLoader - CHTL文件加载器
 * 负责加载和预处理CHTL文件
 */
class CHTLLoader {
public:
    /**
     * 加载结果
     */
    struct LoadResult {
        bool success;
        std::string content;
        std::string file_path;
        std::vector<std::string> dependencies;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        LoadResult() : success(false) {}
    };

public:
    /**
     * 加载CHTL文件
     * @param file_path 文件路径
     * @return 加载结果
     */
    static LoadResult loadFile(const std::string& file_path);
    
    /**
     * 加载多个CHTL文件
     * @param file_paths 文件路径列表
     * @return 加载结果列表
     */
    static std::vector<LoadResult> loadFiles(const std::vector<std::string>& file_paths);
    
    /**
     * 预处理CHTL内容
     * @param content 原始内容
     * @param file_path 文件路径
     * @return 预处理后的内容
     */
    static std::string preprocessContent(const std::string& content, 
                                        const std::string& file_path);
    
    /**
     * 检查文件是否存在
     * @param file_path 文件路径
     * @return 是否存在
     */
    static bool fileExists(const std::string& file_path);
    
    /**
     * 解析文件依赖
     * @param content 文件内容
     * @return 依赖文件列表
     */
    static std::vector<std::string> parseDependencies(const std::string& content);

private:
    /**
     * 读取文件内容
     * @param file_path 文件路径
     * @return 文件内容
     */
    static std::string readFileContent(const std::string& file_path);
    
    /**
     * 处理包含指令
     * @param content 内容
     * @param base_path 基础路径
     * @return 处理后的内容
     */
    static std::string processIncludes(const std::string& content, 
                                      const std::string& base_path);
    
    /**
     * 处理导入指令
     * @param content 内容
     * @param base_path 基础路径
     * @return 处理后的内容和依赖列表
     */
    static std::pair<std::string, std::vector<std::string>> processImports(
        const std::string& content, 
        const std::string& base_path);
};

} // namespace Loader
} // namespace CHTL