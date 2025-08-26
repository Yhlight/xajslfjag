#pragma once

#include <string>

namespace CHTL {
namespace Common {

/**
 * SimpleUseSyntax - 简单的use语法处理
 * 严格按照语法文档：只处理 use html5; 和 use @Config Basic; 的简单语法
 */
class SimpleUseSyntax {
public:
    /**
     * Use语法类型（严格按照语法文档）
     */
    enum class UseType {
        HTML5,              // use html5;
        CONFIG_GROUP        // use @Config Basic; 或 use [Configuration] @Config Basic;
    };
    
    /**
     * Use语法信息
     */
    struct UseSyntaxInfo {
        UseType type;
        std::string config_name;        // 配置组名称（如果是CONFIG_GROUP类型）
        bool is_valid;
        
        UseSyntaxInfo() : type(UseType::HTML5), is_valid(false) {}
        UseSyntaxInfo(UseType t, const std::string& name = "") 
            : type(t), config_name(name), is_valid(true) {}
    };
    
    /**
     * 解析use语句
     * @param use_statement use语句，如 "use html5;" 或 "use @Config Basic;"
     * @return use语法信息
     */
    static UseSyntaxInfo parseUseStatement(const std::string& use_statement);
    
    /**
     * 检查是否为use语句
     * @param statement 语句
     * @return 是否为use语句
     */
    static bool isUseStatement(const std::string& statement);
    
    /**
     * 生成HTML5声明
     * @return HTML5文档声明字符串
     */
    static std::string generateHTML5Declaration();
    
    /**
     * 验证use语句位置（必须在文件开头）
     * @param line_number 行号
     * @return 是否在正确位置
     */
    static bool validatePosition(int line_number);

private:
    /**
     * 解析配置组名称
     * @param config_text 配置文本
     * @return 配置组名称
     */
    static std::string parseConfigName(const std::string& config_text);
    
    /**
     * 清理语句（去除分号和空白）
     * @param statement 语句
     * @return 清理后的语句
     */
    static std::string cleanStatement(const std::string& statement);
};

} // namespace Common
} // namespace CHTL