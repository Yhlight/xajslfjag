#pragma once

#include <string>
#include <vector>
#include <unordered_set>

namespace CHTL {
namespace Common {

/**
 * SimpleConstraints - 简单的约束处理
 * 严格按照语法文档：只处理except关键字的简单约束
 */
class SimpleConstraints {
public:
    /**
     * 约束类型（严格按照语法文档）
     */
    enum class ConstraintType {
        HTML_ELEMENT,       // HTML元素约束：except span
        CUSTOM_ELEMENT,     // 自定义元素约束：except [Custom] @Element Box
        TEMPLATE_ELEMENT,   // 模板元素约束：except [Template] @Element Box
        HTML_TYPE,          // HTML类型约束：except @Html
        CUSTOM_TYPE,        // 自定义类型约束：except [Custom]
        TEMPLATE_TYPE       // 模板类型约束：except [Template]
    };
    
    /**
     * 约束项
     */
    struct ConstraintItem {
        ConstraintType type;
        std::string name;           // 具体名称（如果有）
        
        ConstraintItem(ConstraintType t, const std::string& n = "") 
            : type(t), name(n) {}
    };
    
    /**
     * 解析except语句
     * @param except_statement except语句内容
     * @return 约束项列表
     */
    static std::vector<ConstraintItem> parseExceptStatement(const std::string& except_statement);
    
    /**
     * 检查是否被约束禁止
     * @param constraints 约束列表
     * @param element_type 要检查的元素类型
     * @param element_name 要检查的元素名称
     * @return 是否被禁止
     */
    static bool isProhibited(const std::vector<ConstraintItem>& constraints,
                           const std::string& element_type,
                           const std::string& element_name = "");
    
private:
    /**
     * 解析单个约束项
     * @param constraint_text 约束文本
     * @return 约束项
     */
    static ConstraintItem parseConstraintItem(const std::string& constraint_text);
    
    /**
     * 清理和规范化文本
     * @param text 输入文本
     * @return 清理后的文本
     */
    static std::string cleanText(const std::string& text);
};

} // namespace Common
} // namespace CHTL