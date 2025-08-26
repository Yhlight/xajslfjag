#pragma once

#include <string>

namespace CHTL {
namespace Common {

/**
 * SimpleIndexAccess - 简单的索引访问处理
 * 严格按照语法文档：只处理 element[index] 的简单索引访问
 */
class SimpleIndexAccess {
public:
    /**
     * 索引访问信息
     */
    struct IndexAccessInfo {
        std::string element_name;       // 元素名称
        int index;                      // 索引值
        bool is_valid;                  // 是否有效
        
        IndexAccessInfo() : index(-1), is_valid(false) {}
        IndexAccessInfo(const std::string& name, int idx) 
            : element_name(name), index(idx), is_valid(true) {}
    };
    
    /**
     * 解析索引访问表达式
     * @param expression 表达式，如 "div[1]"
     * @return 索引访问信息
     */
    static IndexAccessInfo parseIndexAccess(const std::string& expression);
    
    /**
     * 检查是否为索引访问表达式
     * @param expression 表达式
     * @return 是否为索引访问
     */
    static bool isIndexAccess(const std::string& expression);
    
    /**
     * 生成索引访问的唯一ID
     * @param element_name 元素名称
     * @param index 索引
     * @return 唯一ID
     */
    static std::string generateIndexId(const std::string& element_name, int index);

private:
    /**
     * 提取索引值
     * @param index_text 索引文本
     * @return 索引值，失败返回-1
     */
    static int extractIndex(const std::string& index_text);
};

} // namespace Common
} // namespace CHTL