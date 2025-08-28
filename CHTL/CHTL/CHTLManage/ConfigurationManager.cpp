#include "ConfigurationManager.h"
#include <iostream>

namespace CHTL {

// 静态成员初始化
std::unique_ptr<ConfigurationManager> ConfigurationManager::instance_ = nullptr;

// 获取单例实例
ConfigurationManager& ConfigurationManager::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<ConfigurationManager>(new ConfigurationManager());
    }
    return *instance_;
}

// 调试输出
void ConfigurationManager::printConfiguration() const {
    std::cout << "=== CHTL Configuration ===" << std::endl;
    
    std::cout << "[Main Configuration]" << std::endl;
    std::cout << "  INDEX_INITIAL_COUNT: " << config_.INDEX_INITIAL_COUNT << std::endl;
    std::cout << "  DISABLE_NAME_GROUP: " << (config_.DISABLE_NAME_GROUP ? "true" : "false") << std::endl;
    std::cout << "  DISABLE_CUSTOM_ORIGIN_TYPE: " << (config_.DISABLE_CUSTOM_ORIGIN_TYPE ? "true" : "false") << std::endl;
    std::cout << "  DISABLE_DEFAULT_NAMESPACE: " << (config_.DISABLE_DEFAULT_NAMESPACE ? "true" : "false") << std::endl;
    std::cout << "  DEBUG_MODE: " << (config_.DEBUG_MODE ? "true" : "false") << std::endl;
    std::cout << "  DISABLE_STYLE_AUTO_ADD_CLASS: " << (config_.DISABLE_STYLE_AUTO_ADD_CLASS ? "true" : "false") << std::endl;
    std::cout << "  DISABLE_STYLE_AUTO_ADD_ID: " << (config_.DISABLE_STYLE_AUTO_ADD_ID ? "true" : "false") << std::endl;
    std::cout << "  DISABLE_SCRIPT_AUTO_ADD_CLASS: " << (config_.DISABLE_SCRIPT_AUTO_ADD_CLASS ? "true" : "false") << std::endl;
    std::cout << "  DISABLE_SCRIPT_AUTO_ADD_ID: " << (config_.DISABLE_SCRIPT_AUTO_ADD_ID ? "true" : "false") << std::endl;
    std::cout << "  OPTION_COUNT: " << config_.OPTION_COUNT << std::endl;
    
    std::cout << "\n[Name Configuration]" << std::endl;
    std::cout << "  CUSTOM_STYLE: ";
    for (const auto& style : nameConfig_.CUSTOM_STYLE) {
        std::cout << style << " ";
    }
    std::cout << std::endl;
    std::cout << "  KEYWORD_TEXT: " << nameConfig_.KEYWORD_TEXT << std::endl;
    std::cout << "  KEYWORD_STYLE: " << nameConfig_.KEYWORD_STYLE << std::endl;
    std::cout << "  KEYWORD_SCRIPT: " << nameConfig_.KEYWORD_SCRIPT << std::endl;
    // ... 其他配置项可以根据需要添加
    
    std::cout << "==========================" << std::endl;
}

} // namespace CHTL