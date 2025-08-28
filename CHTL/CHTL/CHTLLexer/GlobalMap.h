#ifndef CHTL_GLOBALMAP_H
#define CHTL_GLOBALMAP_H

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include "Token.h"

namespace CHTL {

// 全局映射表 - 管理关键字、符号和配置
class GlobalMap {
private:
    // 单例模式
    static std::unique_ptr<GlobalMap> instance_;
    
    // 关键字映射表
    std::unordered_map<std::string, TokenType> keywords_;
    
    // 方括号关键字映射表
    std::unordered_map<std::string, TokenType> bracketKeywords_;
    
    // @前缀类型映射表
    std::unordered_map<std::string, TokenType> atTypes_;
    
    // 操作符映射表
    std::unordered_map<std::string, TokenType> operators_;
    
    // 配置选项 - 根据CHTL语法文档中的[Configuration]
    struct Configuration {
        // 索引起始计数
        int INDEX_INITIAL_COUNT = 0;
        
        // 是否禁用Name配置组
        bool DISABLE_NAME_GROUP = false;
        
        // 是否禁用自定义原始嵌入类型
        bool DISABLE_CUSTOM_ORIGIN_TYPE = false;
        
        // 是否禁用默认命名空间
        bool DISABLE_DEFAULT_NAMESPACE = false;
        
        // DEBUG模式
        bool DEBUG_MODE = false;
        
        // 选择器自动化配置
        bool DISABLE_STYLE_AUTO_ADD_CLASS = false;
        bool DISABLE_STYLE_AUTO_ADD_ID = false;
        bool DISABLE_SCRIPT_AUTO_ADD_CLASS = false;
        bool DISABLE_SCRIPT_AUTO_ADD_ID = false;
        
        // 组选项数量限制
        int OPTION_COUNT = 3;
    } config_;
    
    // Name配置组 - 自定义关键字名称
    struct NameConfiguration {
        std::vector<std::string> CUSTOM_STYLE = {"@Style", "@style", "@CSS", "@Css", "@css"};
        std::string CUSTOM_ELEMENT = "@Element";
        std::string CUSTOM_VAR = "@Var";
        std::string TEMPLATE_STYLE = "@Style";
        std::string TEMPLATE_ELEMENT = "@Element";
        std::string TEMPLATE_VAR = "@Var";
        std::string ORIGIN_HTML = "@Html";
        std::string ORIGIN_STYLE = "@Style";
        std::string ORIGIN_JAVASCRIPT = "@JavaScript";
        std::string CONFIGURATION_CONFIG = "@Config";
        std::string IMPORT_HTML = "@Html";
        std::string IMPORT_STYLE = "@Style";
        std::string IMPORT_JAVASCRIPT = "@JavaScript";
        std::string IMPORT_CHTL = "@Chtl";
        std::string IMPORT_CJMOD = "@CJmod";
        std::string IMPORT_CONFIG = "@Config";
        std::string KEYWORD_INHERIT = "inherit";
        std::string KEYWORD_DELETE = "delete";
        std::string KEYWORD_INSERT = "insert";
        std::string KEYWORD_AFTER = "after";
        std::string KEYWORD_BEFORE = "before";
        std::string KEYWORD_REPLACE = "replace";
        std::string KEYWORD_ATTOP = "at top";
        std::string KEYWORD_ATBOTTOM = "at bottom";
        std::string KEYWORD_FROM = "from";
        std::string KEYWORD_AS = "as";
        std::string KEYWORD_EXCEPT = "except";
        std::string KEYWORD_TEXT = "text";
        std::string KEYWORD_STYLE = "style";
        std::string KEYWORD_SCRIPT = "script";
        std::string KEYWORD_CUSTOM = "[Custom]";
        std::string KEYWORD_TEMPLATE = "[Template]";
        std::string KEYWORD_ORIGIN = "[Origin]";
        std::string KEYWORD_IMPORT = "[Import]";
        std::string KEYWORD_NAMESPACE = "[Namespace]";
    } nameConfig_;
    
    // 自定义原始嵌入类型
    std::unordered_map<std::string, std::string> customOriginTypes_;
    
    // 私有构造函数
    GlobalMap();
    
    // 初始化各种映射表
    void initializeKeywords();
    void initializeBracketKeywords();
    void initializeAtTypes();
    void initializeOperators();
    
public:
    // 获取单例实例
    static GlobalMap& getInstance();
    
    // 禁用拷贝和赋值
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 析构函数
    ~GlobalMap() = default;
    
    // 查询方法
    bool isKeyword(const std::string& word) const;
    bool isBracketKeyword(const std::string& word) const;
    bool isAtType(const std::string& word) const;
    bool isOperator(const std::string& op) const;
    
    // 获取Token类型
    TokenType getKeywordType(const std::string& word) const;
    TokenType getBracketKeywordType(const std::string& word) const;
    TokenType getAtType(const std::string& word) const;
    TokenType getOperatorType(const std::string& op) const;
    
    // 配置相关方法
    const Configuration& getConfig() const { return config_; }
    Configuration& getConfig() { return config_; }
    void updateConfiguration(const Configuration& config) { config_ = config; }
    
    // Name配置相关方法
    const NameConfiguration& getNameConfig() const { return nameConfig_; }
    void updateNameConfiguration(const NameConfiguration& nameConfig);
    
    // 自定义原始嵌入类型相关方法
    void addCustomOriginType(const std::string& name, const std::string& type);
    bool isCustomOriginType(const std::string& type) const;
    
    // 重新初始化（用于配置更新后）
    void reinitialize();
    
    // 调试输出
    void printAllMappings() const;
};

} // namespace CHTL

#endif // CHTL_GLOBALMAP_H