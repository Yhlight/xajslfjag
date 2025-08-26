#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace Loader {

/**
 * CHTLJSLoader - CHTL JS代码加载器
 * 专门处理CHTL JS代码的加载和预处理
 */
class CHTLJSLoader {
public:
    /**
     * 加载结果
     */
    struct LoadResult {
        bool success;
        std::string content;
        std::string file_path;
        std::vector<std::string> chtljs_fragments;
        std::vector<std::string> pure_js_fragments;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        LoadResult() : success(false) {}
    };

public:
    /**
     * 加载CHTL JS代码
     * @param js_content JavaScript内容
     * @param file_path 文件路径
     * @return 加载结果
     */
    static LoadResult loadCHTLJS(const std::string& js_content, 
                                const std::string& file_path);
    
    /**
     * 检测CHTL JS语法
     * @param js_content JavaScript内容
     * @return 是否包含CHTL JS语法
     */
    static bool detectCHTLJSSyntax(const std::string& js_content);
    
    /**
     * 分离CHTL JS和纯JavaScript
     * @param mixed_content 混合内容
     * @return [CHTL JS片段, 纯JS片段]
     */
    static std::pair<std::vector<std::string>, std::vector<std::string>> 
           separateCHTLJSAndJS(const std::string& mixed_content);
    
    /**
     * 预处理CHTL JS内容
     * @param content 原始内容
     * @param file_path 文件路径
     * @return 预处理后的内容
     */
    static std::string preprocessCHTLJS(const std::string& content, 
                                       const std::string& file_path);

private:
    /**
     * 检测增强选择器
     * @param content 内容
     * @return 是否包含增强选择器
     */
    static bool hasEnhancedSelectors(const std::string& content);
    
    /**
     * 检测CHTL JS函数
     * @param content 内容
     * @return 是否包含CHTL JS函数
     */
    static bool hasCHTLJSFunctions(const std::string& content);
    
    /**
     * 检测vir对象
     * @param content 内容
     * @return 是否包含vir对象
     */
    static bool hasVirObjects(const std::string& content);
    
    /**
     * 提取CHTL JS片段
     * @param content 内容
     * @return CHTL JS片段列表
     */
    static std::vector<std::string> extractCHTLJSFragments(const std::string& content);
    
    /**
     * 提取纯JavaScript片段
     * @param content 内容
     * @return 纯JavaScript片段列表
     */
    static std::vector<std::string> extractPureJSFragments(const std::string& content);
};

} // namespace Loader
} // namespace CHTLJS
} // namespace CHTL