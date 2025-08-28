#ifndef CHTL_CONFIGURATION_MANAGER_H
#define CHTL_CONFIGURATION_MANAGER_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// 配置管理器 - 负责管理CHTL的所有配置选项
class ConfigurationManager {
private:
    // 单例模式
    static std::unique_ptr<ConfigurationManager> instance_;
    
    // 主配置选项 - 根据CHTL语法文档中的[Configuration]
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
        std::string KEYWORD_CONFIGURATION = "[Configuration]";
    } nameConfig_;
    
    // 私有构造函数
    ConfigurationManager() = default;
    
public:
    // 获取单例实例
    static ConfigurationManager& getInstance();
    
    // 禁用拷贝和赋值
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    
    // 析构函数
    ~ConfigurationManager() = default;
    
    // 配置相关方法
    const Configuration& getConfig() const { return config_; }
    Configuration& getConfig() { return config_; }
    void updateConfiguration(const Configuration& config) { config_ = config; }
    
    // Name配置相关方法
    const NameConfiguration& getNameConfig() const { return nameConfig_; }
    NameConfiguration& getNameConfig() { return nameConfig_; }
    void updateNameConfiguration(const NameConfiguration& nameConfig) { nameConfig_ = nameConfig; }
    
    // 获取特定的配置值
    int getIndexInitialCount() const { return config_.INDEX_INITIAL_COUNT; }
    bool isNameGroupDisabled() const { return config_.DISABLE_NAME_GROUP; }
    bool isCustomOriginTypeDisabled() const { return config_.DISABLE_CUSTOM_ORIGIN_TYPE; }
    bool isDefaultNamespaceDisabled() const { return config_.DISABLE_DEFAULT_NAMESPACE; }
    bool isDebugMode() const { return config_.DEBUG_MODE; }
    int getOptionCount() const { return config_.OPTION_COUNT; }
    
    // 调试输出
    void printConfiguration() const;
};

} // namespace CHTL

#endif // CHTL_CONFIGURATION_MANAGER_H