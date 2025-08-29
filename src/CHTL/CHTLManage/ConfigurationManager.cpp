#include "ConfigurationManager.h"
#include <iostream>

namespace CHTL {

ConfigurationManager::ConfigurationManager()
    : m_unnamedConfiguration(nullptr), m_activeConfiguration(nullptr) {
}

void ConfigurationManager::registerConfiguration(std::shared_ptr<ConfigurationNode> configNode) {
    if (!configNode) {
        return;
    }
    
    if (configNode->isNamed()) {
        // 命名配置
        m_namedConfigurations[configNode->getConfigName()] = configNode;
    } else {
        // 无名配置
        if (m_unnamedConfiguration) {
            std::cerr << "Warning: Multiple unnamed configurations found. Using the last one." << std::endl;
        }
        m_unnamedConfiguration = configNode;
    }
    
    // 如果没有活动配置，选择一个
    if (!m_activeConfiguration) {
        selectActiveConfiguration();
    }
}

void ConfigurationManager::setActiveConfiguration(const std::string& configName) {
    auto it = m_namedConfigurations.find(configName);
    if (it != m_namedConfigurations.end()) {
        m_activeConfiguration = it->second;
        applyConfiguration();
    } else {
        std::cerr << "Warning: Configuration '" << configName << "' not found." << std::endl;
    }
}

std::shared_ptr<ConfigurationNode> ConfigurationManager::getActiveConfiguration() const {
    return m_activeConfiguration;
}

std::shared_ptr<ConfigurationNode> ConfigurationManager::getConfiguration(const std::string& name) const {
    auto it = m_namedConfigurations.find(name);
    if (it != m_namedConfigurations.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<ConfigurationNode> ConfigurationManager::getDefaultConfiguration() const {
    // 优先返回无名配置，否则返回第一个有名配置
    if (m_unnamedConfiguration) {
        return m_unnamedConfiguration;
    }
    
    if (!m_namedConfigurations.empty()) {
        return m_namedConfigurations.begin()->second;
    }
    
    return nullptr;
}

void ConfigurationManager::applyConfiguration() {
    if (!m_activeConfiguration) {
        return;
    }
    
    // 这里可以添加将配置应用到全局设置的逻辑
    // 例如更新词法分析器、解析器、生成器的行为
    
    if (isDebugMode()) {
        std::cout << "Configuration applied: " << m_activeConfiguration->getConfigName() << std::endl;
        std::cout << "  - Index initial count: " << getIndexInitialCount() << std::endl;
        std::cout << "  - Name group disabled: " << (isNameGroupDisabled() ? "true" : "false") << std::endl;
        std::cout << "  - Debug mode: " << (isDebugMode() ? "true" : "false") << std::endl;
    }
}

// ========== 配置选项访问器实现 ==========

int ConfigurationManager::getIndexInitialCount() const {
    if (m_activeConfiguration) {
        return m_activeConfiguration->getIndexInitialCount();
    }
    return 0; // 默认值
}

bool ConfigurationManager::isNameGroupDisabled() const {
    if (m_activeConfiguration) {
        return m_activeConfiguration->isNameGroupDisabled();
    }
    return false; // 默认值
}

bool ConfigurationManager::isCustomOriginTypeDisabled() const {
    if (m_activeConfiguration) {
        return m_activeConfiguration->isCustomOriginTypeDisabled();
    }
    return false; // 默认值
}

bool ConfigurationManager::isDebugMode() const {
    if (m_activeConfiguration) {
        return m_activeConfiguration->isDebugMode();
    }
    return false; // 默认值
}

bool ConfigurationManager::isDefaultNamespaceDisabled() const {
    if (m_activeConfiguration) {
        return m_activeConfiguration->isDefaultNamespaceDisabled();
    }
    return false; // 默认值
}

bool ConfigurationManager::isStyleAutoAddClassEnabled() const {
    if (m_activeConfiguration) {
        return m_activeConfiguration->isStyleAutoAddClassEnabled();
    }
    return true; // 默认启用
}

bool ConfigurationManager::isStyleAutoAddIdEnabled() const {
    if (m_activeConfiguration) {
        return m_activeConfiguration->isStyleAutoAddIdEnabled();
    }
    return true; // 默认启用
}

bool ConfigurationManager::isScriptAutoAddClassEnabled() const {
    if (m_activeConfiguration) {
        return m_activeConfiguration->isScriptAutoAddClassEnabled();
    }
    return false; // 默认禁用
}

bool ConfigurationManager::isScriptAutoAddIdEnabled() const {
    if (m_activeConfiguration) {
        return m_activeConfiguration->isScriptAutoAddIdEnabled();
    }
    return false; // 默认禁用
}

std::vector<std::string> ConfigurationManager::getKeywordAliases(const std::string& keyword) const {
    if (m_activeConfiguration) {
        // 查找Name配置中的关键字别名
        for (auto child : m_activeConfiguration->getChildren()) {
            if (auto nameConfig = std::dynamic_pointer_cast<NameConfigNode>(child)) {
                auto aliases = nameConfig->getKeywordAliases(keyword);
                if (!aliases.empty()) {
                    return aliases;
                }
            }
        }
    }
    return {}; // 没有找到别名
}

std::unordered_map<std::string, std::string> ConfigurationManager::getCustomOriginTypes() const {
    std::unordered_map<std::string, std::string> types;
    
    if (m_activeConfiguration) {
        // 查找OriginType配置中的自定义类型
        for (auto child : m_activeConfiguration->getChildren()) {
            if (auto originTypeConfig = std::dynamic_pointer_cast<OriginTypeConfigNode>(child)) {
                auto customTypes = originTypeConfig->getAllCustomOriginTypes();
                types.insert(customTypes.begin(), customTypes.end());
            }
        }
    }
    
    return types;
}

bool ConfigurationManager::hasCustomOriginType(const std::string& typeName) const {
    auto types = getCustomOriginTypes();
    return types.find(typeName) != types.end();
}

std::string ConfigurationManager::getCustomOriginTypeIdentifier(const std::string& typeName) const {
    auto types = getCustomOriginTypes();
    auto it = types.find(typeName);
    if (it != types.end()) {
        return it->second;
    }
    return "";
}

void ConfigurationManager::selectActiveConfiguration() {
    // 选择活动配置的优先级：
    // 1. 无名配置（如果存在）
    // 2. 第一个有名配置
    
    if (m_unnamedConfiguration) {
        m_activeConfiguration = m_unnamedConfiguration;
    } else if (!m_namedConfigurations.empty()) {
        m_activeConfiguration = m_namedConfigurations.begin()->second;
    }
    
    if (m_activeConfiguration) {
        applyConfiguration();
    }
}

void ConfigurationManager::mergeConfigurations() {
    // 配置合并逻辑
    // 根据CHTL语法文档，如果存在多个无名配置组则冲突
    // 这里可以实现配置冲突检测和合并策略
    
    if (m_unnamedConfiguration && !m_namedConfigurations.empty()) {
        std::cout << "Info: Both unnamed and named configurations exist. Using unnamed configuration as default." << std::endl;
    }
}

} // namespace CHTL