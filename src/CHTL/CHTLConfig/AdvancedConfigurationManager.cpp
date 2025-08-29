#include "AdvancedConfigurationManager.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <iostream>

namespace CHTL {

// ========== NamedConfigurationGroup 实现 ==========

NamedConfigurationGroup::NamedConfigurationGroup(const String& name)
    : groupName(name), version("1.0.0"), isDefault(false) {
}

void NamedConfigurationGroup::setOption(const String& name, const ConfigValue& value) {
    options[name] = value;
}

ConfigValue NamedConfigurationGroup::getOption(const String& name) const {
    auto it = options.find(name);
    return it != options.end() ? it->second : ConfigValue{};
}

bool NamedConfigurationGroup::hasOption(const String& name) const {
    return options.find(name) != options.end();
}

void NamedConfigurationGroup::removeOption(const String& name) {
    options.erase(name);
}

void NamedConfigurationGroup::setGroupOption(const String& name, const GroupOptionConfig& config) {
    groupOptions[name] = config;
}

GroupOptionConfig NamedConfigurationGroup::getGroupOption(const String& name) const {
    auto it = groupOptions.find(name);
    return it != groupOptions.end() ? it->second : GroupOptionConfig{};
}

bool NamedConfigurationGroup::hasGroupOption(const String& name) const {
    return groupOptions.find(name) != groupOptions.end();
}

void NamedConfigurationGroup::addGroupOptionValue(const String& groupName, const String& value) {
    if (hasGroupOption(groupName)) {
        GroupOptionConfig& config = groupOptions[groupName];
        if (config.options.size() < config.maxCount) {
            if (config.allowDuplicates || 
                std::find(config.options.begin(), config.options.end(), value) == config.options.end()) {
                config.options.push_back(value);
            }
        }
    }
}

StringVector NamedConfigurationGroup::getGroupOptionValues(const String& groupName) const {
    auto it = groupOptions.find(groupName);
    return it != groupOptions.end() ? it->second.options : StringVector{};
}

void NamedConfigurationGroup::setKeywordMapping(const String& keyword, const KeywordMappingConfig& config) {
    keywordMappings[keyword] = config;
}

KeywordMappingConfig NamedConfigurationGroup::getKeywordMapping(const String& keyword) const {
    auto it = keywordMappings.find(keyword);
    return it != keywordMappings.end() ? it->second : KeywordMappingConfig{};
}

bool NamedConfigurationGroup::hasKeywordMapping(const String& keyword) const {
    return keywordMappings.find(keyword) != keywordMappings.end();
}

void NamedConfigurationGroup::addKeywordAlias(const String& keyword, const String& alias) {
    if (hasKeywordMapping(keyword)) {
        KeywordMappingConfig& config = keywordMappings[keyword];
        if (std::find(config.aliases.begin(), config.aliases.end(), alias) == config.aliases.end()) {
            config.aliases.push_back(alias);
        }
    }
}

StringVector NamedConfigurationGroup::getKeywordAliases(const String& keyword) const {
    auto it = keywordMappings.find(keyword);
    return it != keywordMappings.end() ? it->second.aliases : StringVector{};
}

String NamedConfigurationGroup::resolveKeyword(const String& input) const {
    // 直接匹配
    if (hasKeywordMapping(input)) {
        return input;
    }
    
    // 别名匹配
    for (const auto& [keyword, config] : keywordMappings) {
        for (const String& alias : config.aliases) {
            if (config.isCaseSensitive) {
                if (alias == input) return keyword;
            } else {
                if (std::tolower(alias[0]) == std::tolower(input[0])) {
                    String lowerAlias = alias;
                    String lowerInput = input;
                    std::transform(lowerAlias.begin(), lowerAlias.end(), lowerAlias.begin(), ::tolower);
                    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
                    if (lowerAlias == lowerInput) return keyword;
                }
            }
        }
    }
    
    return input; // 未找到匹配，返回原值
}

bool NamedConfigurationGroup::validateConfiguration() const {
    StringVector errors = getValidationErrors();
    return errors.empty();
}

StringVector NamedConfigurationGroup::getValidationErrors() const {
    StringVector errors;
    
    // 验证组选项
    for (const auto& [name, config] : groupOptions) {
        if (config.options.size() < config.minCount) {
            errors.push_back("组选项 " + name + " 值数量不足，最少需要 " + std::to_string(config.minCount) + " 个");
        }
        if (config.options.size() > config.maxCount) {
            errors.push_back("组选项 " + name + " 值数量过多，最多允许 " + std::to_string(config.maxCount) + " 个");
        }
    }
    
    // 验证关键字映射
    for (const auto& [keyword, config] : keywordMappings) {
        if (keyword.empty()) {
            errors.push_back("关键字不能为空");
        }
        if (config.aliases.empty() && !config.isCore) {
            errors.push_back("非核心关键字 " + keyword + " 必须有至少一个别名");
        }
    }
    
    return errors;
}

bool NamedConfigurationGroup::isCompatibleWith(const NamedConfigurationGroup& other) const {
    // 检查关键字冲突
    for (const auto& [keyword, config] : keywordMappings) {
        if (other.hasKeywordMapping(keyword)) {
            const auto& otherConfig = other.getKeywordMapping(keyword);
            if (config.isCore && otherConfig.isCore && config.aliases != otherConfig.aliases) {
                return false; // 核心关键字冲突
            }
        }
    }
    
    return true;
}

String NamedConfigurationGroup::toString() const {
    std::ostringstream oss;
    
    oss << "[Configuration] @Config " << groupName << " {\n";
    oss << "    // 版本: " << version << "\n";
    oss << "    // 作者: " << author << "\n";
    oss << "    // 描述: " << description << "\n\n";
    
    // 基础选项
    for (const auto& [name, value] : options) {
        oss << "    " << name << " = " << configValueToString(value) << ";\n";
    }
    
    if (!options.empty()) oss << "\n";
    
    // Name配置组
    if (!groupOptions.empty() || !keywordMappings.empty()) {
        oss << "    [Name] {\n";
        
        // 组选项
        for (const auto& [name, config] : groupOptions) {
            oss << "        " << name << " = [";
            for (size_t i = 0; i < config.options.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << config.options[i];
            }
            oss << "];\n";
        }
        
        // 关键字映射
        for (const auto& [keyword, config] : keywordMappings) {
            if (config.aliases.size() == 1) {
                oss << "        " << keyword << " = " << config.aliases[0] << ";\n";
            } else if (config.aliases.size() > 1) {
                oss << "        " << keyword << " = [";
                for (size_t i = 0; i < config.aliases.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << config.aliases[i];
                }
                oss << "];\n";
            }
        }
        
        oss << "    }\n";
    }
    
    oss << "}";
    return oss.str();
}

String NamedConfigurationGroup::toJSON() const {
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"name\": \"" << groupName << "\",\n";
    oss << "  \"version\": \"" << version << "\",\n";
    oss << "  \"author\": \"" << author << "\",\n";
    oss << "  \"description\": \"" << description << "\",\n";
    oss << "  \"isDefault\": " << (isDefault ? "true" : "false") << ",\n";
    
    // 选项
    oss << "  \"options\": {\n";
    size_t optionIndex = 0;
    for (const auto& [name, value] : options) {
        if (optionIndex > 0) oss << ",\n";
        oss << "    \"" << name << "\": \"" << configValueToString(value) << "\"";
        optionIndex++;
    }
    oss << "\n  },\n";
    
    // 组选项
    oss << "  \"groupOptions\": {\n";
    size_t groupIndex = 0;
    for (const auto& [name, config] : groupOptions) {
        if (groupIndex > 0) oss << ",\n";
        oss << "    \"" << name << "\": [";
        for (size_t i = 0; i < config.options.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << config.options[i] << "\"";
        }
        oss << "]";
        groupIndex++;
    }
    oss << "\n  },\n";
    
    // 关键字映射
    oss << "  \"keywordMappings\": {\n";
    size_t keywordIndex = 0;
    for (const auto& [keyword, config] : keywordMappings) {
        if (keywordIndex > 0) oss << ",\n";
        oss << "    \"" << keyword << "\": {\n";
        oss << "      \"aliases\": [";
        for (size_t i = 0; i < config.aliases.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << config.aliases[i] << "\"";
        }
        oss << "],\n";
        oss << "      \"category\": \"" << config.category << "\",\n";
        oss << "      \"isCore\": " << (config.isCore ? "true" : "false") << "\n";
        oss << "    }";
        keywordIndex++;
    }
    oss << "\n  }\n";
    
    oss << "}";
    return oss.str();
}

void NamedConfigurationGroup::mergeFrom(const NamedConfigurationGroup& other) {
    // 合并选项
    for (const auto& [name, value] : other.options) {
        options[name] = value;
    }
    
    // 合并组选项
    for (const auto& [name, config] : other.groupOptions) {
        groupOptions[name] = config;
    }
    
    // 合并关键字映射
    for (const auto& [keyword, config] : other.keywordMappings) {
        keywordMappings[keyword] = config;
    }
}

void NamedConfigurationGroup::inheritFrom(const NamedConfigurationGroup& parent) {
    // 继承不会覆盖已有的配置
    for (const auto& [name, value] : parent.options) {
        if (!hasOption(name)) {
            options[name] = value;
        }
    }
    
    for (const auto& [name, config] : parent.groupOptions) {
        if (!hasGroupOption(name)) {
            groupOptions[name] = config;
        }
    }
    
    for (const auto& [keyword, config] : parent.keywordMappings) {
        if (!hasKeywordMapping(keyword)) {
            keywordMappings[keyword] = config;
        }
    }
}

NamedConfigurationGroup NamedConfigurationGroup::createChild(const String& childName) const {
    NamedConfigurationGroup child(childName);
    child.inheritFrom(*this);
    child.version = "1.0.0"; // 重置版本
    child.author = author;   // 继承作者
    return child;
}

String NamedConfigurationGroup::configValueToString(const ConfigValue& value) const {
    return std::visit([](const auto& v) -> String {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, String>) {
            return v;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, StringVector>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << v[i];
            }
            oss << "]";
            return oss.str();
        }
        return String{};
    }, value);
}

// ========== AdvancedConfigurationManager 实现 ==========

AdvancedConfigurationManager::AdvancedConfigurationManager()
    : dynamicUpdatesEnabled(false), cachingEnabled(true), debugMode(false) {
    initializeDefaultMetadata();
    initializeDefaultGroupOptions();
    initializeDefaultKeywordMappings();
}

void AdvancedConfigurationManager::registerNamedGroup(const String& name, std::shared_ptr<NamedConfigurationGroup> group) {
    namedGroups[name] = group;
    
    if (group->isDefault) {
        setDefaultGroup(name);
    }
    
    if (dynamicUpdatesEnabled) {
        // 通知配置更新
        for (const auto& [optionName, value] : group->options) {
            notifyConfigChange(optionName, value);
        }
    }
}

void AdvancedConfigurationManager::unregisterNamedGroup(const String& name) {
    namedGroups.erase(name);
    
    if (defaultGroupName == name) {
        defaultGroupName.clear();
    }
}

std::shared_ptr<NamedConfigurationGroup> AdvancedConfigurationManager::getNamedGroup(const String& name) const {
    auto it = namedGroups.find(name);
    return it != namedGroups.end() ? it->second : nullptr;
}

bool AdvancedConfigurationManager::hasNamedGroup(const String& name) const {
    return namedGroups.find(name) != namedGroups.end();
}

StringVector AdvancedConfigurationManager::getAllGroupNames() const {
    StringVector names;
    for (const auto& [name, group] : namedGroups) {
        names.push_back(name);
    }
    return names;
}

void AdvancedConfigurationManager::setDefaultGroup(const String& groupName) {
    if (hasNamedGroup(groupName)) {
        defaultGroupName = groupName;
        if (cachingEnabled) {
            clearConfigurationCache();
        }
    }
}

String AdvancedConfigurationManager::getDefaultGroupName() const {
    return defaultGroupName;
}

std::shared_ptr<NamedConfigurationGroup> AdvancedConfigurationManager::getDefaultGroup() const {
    return defaultGroupName.empty() ? nullptr : getNamedGroup(defaultGroupName);
}

void AdvancedConfigurationManager::registerOptionMetadata(const String& optionName, const ConfigOptionMetadata& metadata) {
    optionMetadata[optionName] = metadata;
}

ConfigOptionMetadata AdvancedConfigurationManager::getOptionMetadata(const String& optionName) const {
    auto it = optionMetadata.find(optionName);
    return it != optionMetadata.end() ? it->second : ConfigOptionMetadata{};
}

bool AdvancedConfigurationManager::hasOptionMetadata(const String& optionName) const {
    return optionMetadata.find(optionName) != optionMetadata.end();
}

StringVector AdvancedConfigurationManager::getAllOptionNames() const {
    StringVector names;
    for (const auto& [name, metadata] : optionMetadata) {
        names.push_back(name);
    }
    return names;
}

void AdvancedConfigurationManager::registerGroupOptionType(const String& typeName, const GroupOptionConfig& config) {
    groupOptionTypes[typeName] = config;
}

GroupOptionConfig AdvancedConfigurationManager::getGroupOptionType(const String& typeName) const {
    auto it = groupOptionTypes.find(typeName);
    return it != groupOptionTypes.end() ? it->second : GroupOptionConfig{};
}

bool AdvancedConfigurationManager::parseGroupOption(const String& input, const String& typeName, StringVector& result) const {
    if (!groupOptionTypes.count(typeName)) {
        return false;
    }
    
    const GroupOptionConfig& config = groupOptionTypes.at(typeName);
    
    // 解析格式: [@Style, @style, @CSS, @Css, @css]
    std::regex groupRegex(R"(\[([^\]]+)\])");
    std::smatch match;
    
    if (std::regex_match(input, match, groupRegex)) {
        String content = match[1].str();
        
        // 按分隔符分割
        std::regex itemRegex(R"([^,]+)");
        std::sregex_iterator iter(content.begin(), content.end(), itemRegex);
        std::sregex_iterator end;
        
        StringVector items;
        for (; iter != end; ++iter) {
            String item = iter->str();
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            if (!item.empty()) {
                items.push_back(item);
            }
        }
        
        // 验证数量限制
        if (items.size() >= config.minCount && items.size() <= config.maxCount) {
            result = items;
            return true;
        }
    }
    
    return false;
}

String AdvancedConfigurationManager::formatGroupOption(const StringVector& values, const String& typeName) const {
    if (!groupOptionTypes.count(typeName)) {
        return "";
    }
    
    const GroupOptionConfig& config = groupOptionTypes.at(typeName);
    
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) oss << config.separator << " ";
        oss << values[i];
    }
    oss << "]";
    
    return oss.str();
}

void AdvancedConfigurationManager::registerKeywordCategory(const String& category, const StringVector& keywords) {
    keywordCategories[category] = keywords;
}

StringVector AdvancedConfigurationManager::getKeywordCategory(const String& category) const {
    auto it = keywordCategories.find(category);
    return it != keywordCategories.end() ? it->second : StringVector{};
}

String AdvancedConfigurationManager::resolveKeywordAlias(const String& alias) const {
    // 检查全局别名
    auto it = globalKeywordAliases.find(alias);
    if (it != globalKeywordAliases.end()) {
        return it->second;
    }
    
    // 检查当前配置组的别名
    auto defaultGroup = getDefaultGroup();
    if (defaultGroup) {
        return defaultGroup->resolveKeyword(alias);
    }
    
    return alias;
}

void AdvancedConfigurationManager::addGlobalKeywordAlias(const String& keyword, const String& alias) {
    globalKeywordAliases[alias] = keyword;
}

StringVector AdvancedConfigurationManager::getKeywordAliases(const String& keyword) const {
    StringVector aliases;
    
    // 收集全局别名
    for (const auto& [alias, target] : globalKeywordAliases) {
        if (target == keyword) {
            aliases.push_back(alias);
        }
    }
    
    // 收集配置组别名
    auto defaultGroup = getDefaultGroup();
    if (defaultGroup) {
        auto groupAliases = defaultGroup->getKeywordAliases(keyword);
        aliases.insert(aliases.end(), groupAliases.begin(), groupAliases.end());
    }
    
    return aliases;
}

void AdvancedConfigurationManager::setConfigurationHierarchy(const StringVector& hierarchy) {
    configurationHierarchy = hierarchy;
    if (cachingEnabled) {
        clearConfigurationCache();
    }
}

StringVector AdvancedConfigurationManager::getConfigurationHierarchy() const {
    return configurationHierarchy;
}

ConfigValue AdvancedConfigurationManager::resolveOptionWithInheritance(const String& optionName) const {
    // 按层次结构查找选项
    for (const String& groupName : configurationHierarchy) {
        auto group = getNamedGroup(groupName);
        if (group && group->hasOption(optionName)) {
            return group->getOption(optionName);
        }
    }
    
    // 检查默认组
    auto defaultGroup = getDefaultGroup();
    if (defaultGroup && defaultGroup->hasOption(optionName)) {
        return defaultGroup->getOption(optionName);
    }
    
    // 返回元数据中的默认值
    if (hasOptionMetadata(optionName)) {
        return getOptionMetadata(optionName).defaultValue;
    }
    
    return ConfigValue{};
}

bool AdvancedConfigurationManager::validateAllConfigurations() const {
    for (const auto& [name, group] : namedGroups) {
        if (!group->validateConfiguration()) {
            return false;
        }
    }
    return true;
}

StringVector AdvancedConfigurationManager::getAllValidationErrors() const {
    StringVector allErrors;
    
    for (const auto& [name, group] : namedGroups) {
        auto errors = group->getValidationErrors();
        for (const String& error : errors) {
            allErrors.push_back("[" + name + "] " + error);
        }
    }
    
    return allErrors;
}

void AdvancedConfigurationManager::performCompatibilityCheck() const {
    StringVector groupNames = getAllGroupNames();
    
    for (size_t i = 0; i < groupNames.size(); ++i) {
        for (size_t j = i + 1; j < groupNames.size(); ++j) {
            auto group1 = getNamedGroup(groupNames[i]);
            auto group2 = getNamedGroup(groupNames[j]);
            
            if (group1 && group2 && !group1->isCompatibleWith(*group2)) {
                logConfigurationWarning("配置组 " + groupNames[i] + " 与 " + groupNames[j] + " 不兼容");
            }
        }
    }
}

void AdvancedConfigurationManager::generateConfigurationReport() const {
    std::cout << "=== 配置管理器报告 ===\n";
    std::cout << "注册的配置组数量: " << namedGroups.size() << "\n";
    std::cout << "默认配置组: " << (defaultGroupName.empty() ? "未设置" : defaultGroupName) << "\n";
    std::cout << "选项元数据数量: " << optionMetadata.size() << "\n";
    std::cout << "组选项类型数量: " << groupOptionTypes.size() << "\n";
    std::cout << "关键字类别数量: " << keywordCategories.size() << "\n";
    std::cout << "全局关键字别名数量: " << globalKeywordAliases.size() << "\n";
    
    if (cachingEnabled) {
        std::cout << "缓存条目数量: " << resolvedOptionCache.size() << "\n";
    }
    
    std::cout << "\n配置组详情:\n";
    for (const auto& [name, group] : namedGroups) {
        std::cout << "  " << name << ": " << group->options.size() << " 选项, "
                  << group->groupOptions.size() << " 组选项, "
                  << group->keywordMappings.size() << " 关键字映射\n";
    }
}

void AdvancedConfigurationManager::subscribeToConfigChanges(const String& optionName, std::function<void(const ConfigValue&)> callback) {
    configChangeCallbacks[optionName].push_back(callback);
}

void AdvancedConfigurationManager::notifyConfigChange(const String& optionName, const ConfigValue& newValue) {
    auto it = configChangeCallbacks.find(optionName);
    if (it != configChangeCallbacks.end()) {
        for (const auto& callback : it->second) {
            callback(newValue);
        }
    }
    
    if (cachingEnabled) {
        invalidateCache(optionName);
    }
}

void AdvancedConfigurationManager::clearConfigurationCache() {
    resolvedOptionCache.clear();
    resolvedGroupOptionCache.clear();
}

void AdvancedConfigurationManager::optimizeConfigurationAccess() {
    // 预加载常用配置
    StringVector commonOptions = {
        "INDEX_INITIAL_COUNT", "DISABLE_NAME_GROUP", "DEBUG_MODE",
        "DISABLE_STYLE_AUTO_ADD_CLASS", "DISABLE_SCRIPT_AUTO_ADD_CLASS"
    };
    
    for (const String& option : commonOptions) {
        String cacheKey = generateCacheKey(option);
        resolvedOptionCache[cacheKey] = resolveOptionWithInheritance(option);
    }
}

void AdvancedConfigurationManager::dumpConfigurationState() const {
    std::cout << "\n=== 配置状态转储 ===\n";
    generateConfigurationReport();
    
    std::cout << "\n配置层次结构: ";
    for (const String& name : configurationHierarchy) {
        std::cout << name << " -> ";
    }
    std::cout << "默认组\n";
    
    if (debugMode) {
        std::cout << "\n全局关键字别名:\n";
        for (const auto& [alias, keyword] : globalKeywordAliases) {
            std::cout << "  " << alias << " -> " << keyword << "\n";
        }
        
        if (cachingEnabled && !resolvedOptionCache.empty()) {
            std::cout << "\n缓存条目:\n";
            for (const auto& [key, value] : resolvedOptionCache) {
                std::cout << "  " << key << "\n";
            }
        }
    }
}

void AdvancedConfigurationManager::validateConfigurationIntegrity() const {
    StringVector errors = getAllValidationErrors();
    
    if (!errors.empty()) {
        std::cout << "=== 配置完整性问题 ===\n";
        for (const String& error : errors) {
            std::cout << "错误: " << error << "\n";
        }
    } else {
        std::cout << "配置完整性检查通过\n";
    }
    
    performCompatibilityCheck();
}

// 初始化方法实现
void AdvancedConfigurationManager::initializeDefaultMetadata() {
    // 核心配置选项元数据
    ConfigOptionMetadata indexCount;
    indexCount.name = "INDEX_INITIAL_COUNT";
    indexCount.description = "索引的起始计数";
    indexCount.type = ConfigOptionType::INTEGER;
    indexCount.defaultValue = 0;
    indexCount.minValue = 0;
    indexCount.maxValue = 10;
    registerOptionMetadata("INDEX_INITIAL_COUNT", indexCount);
    
    ConfigOptionMetadata disableNameGroup;
    disableNameGroup.name = "DISABLE_NAME_GROUP";
    disableNameGroup.description = "是否禁用Name配置组";
    disableNameGroup.type = ConfigOptionType::BOOLEAN;
    disableNameGroup.defaultValue = true;
    registerOptionMetadata("DISABLE_NAME_GROUP", disableNameGroup);
    
    ConfigOptionMetadata debugMode;
    debugMode.name = "DEBUG_MODE";
    debugMode.description = "DEBUG模式";
    debugMode.type = ConfigOptionType::BOOLEAN;
    debugMode.defaultValue = false;
    registerOptionMetadata("DEBUG_MODE", debugMode);
}

void AdvancedConfigurationManager::initializeDefaultGroupOptions() {
    // 自定义样式组选项
    GroupOptionConfig customStyleConfig;
    customStyleConfig.groupName = "CUSTOM_STYLE";
    customStyleConfig.options = {"@Style", "@style", "@CSS", "@Css", "@css"};
    customStyleConfig.maxCount = 5;
    customStyleConfig.minCount = 1;
    registerGroupOptionType("CUSTOM_STYLE", customStyleConfig);
    
    // 模板元素组选项
    GroupOptionConfig templateElementConfig;
    templateElementConfig.groupName = "TEMPLATE_ELEMENT";
    templateElementConfig.options = {"@Element", "@Elem"};
    templateElementConfig.maxCount = 3;
    templateElementConfig.minCount = 1;
    registerGroupOptionType("TEMPLATE_ELEMENT", templateElementConfig);
}

void AdvancedConfigurationManager::initializeDefaultKeywordMappings() {
    // 核心关键字类别
    registerKeywordCategory("template", {"[Template]", "Template"});
    registerKeywordCategory("custom", {"[Custom]", "Custom"});
    registerKeywordCategory("origin", {"[Origin]", "Origin"});
    registerKeywordCategory("import", {"[Import]", "Import"});
    registerKeywordCategory("namespace", {"[Namespace]", "Namespace"});
    registerKeywordCategory("configuration", {"[Configuration]", "Configuration"});
    
    // 全局别名
    addGlobalKeywordAlias("[Template]", "Template");
    addGlobalKeywordAlias("[Custom]", "Custom");
    addGlobalKeywordAlias("@Style", "@style");
    addGlobalKeywordAlias("@Element", "@Elem");
}

ConfigValue AdvancedConfigurationManager::parseConfigValue(const String& valueStr, ConfigOptionType expectedType) const {
    switch (expectedType) {
        case ConfigOptionType::STRING:
            return valueStr;
        case ConfigOptionType::INTEGER:
            return std::stoi(valueStr);
        case ConfigOptionType::DOUBLE:
            return std::stod(valueStr);
        case ConfigOptionType::BOOLEAN:
            return valueStr == "true" || valueStr == "1";
        case ConfigOptionType::STRING_ARRAY: {
            StringVector result;
            // 简单的数组解析
            if (valueStr.front() == '[' && valueStr.back() == ']') {
                String content = valueStr.substr(1, valueStr.length() - 2);
                std::istringstream iss(content);
                String item;
                while (std::getline(iss, item, ',')) {
                    item.erase(0, item.find_first_not_of(" \t"));
                    item.erase(item.find_last_not_of(" \t") + 1);
                    result.push_back(item);
                }
            }
            return result;
        }
        default:
            return valueStr;
    }
}

String AdvancedConfigurationManager::formatConfigValue(const ConfigValue& value) const {
    return std::visit([](const auto& v) -> String {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, String>) {
            return v;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, StringVector>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << v[i];
            }
            oss << "]";
            return oss.str();
        }
        return String{};
    }, value);
}

void AdvancedConfigurationManager::invalidateCache(const String& optionName) {
    // 移除相关的缓存条目
    auto it = resolvedOptionCache.begin();
    while (it != resolvedOptionCache.end()) {
        if (it->first.find(optionName) != String::npos) {
            it = resolvedOptionCache.erase(it);
        } else {
            ++it;
        }
    }
}

String AdvancedConfigurationManager::generateCacheKey(const String& optionName, const StringVector& context) const {
    std::ostringstream oss;
    oss << optionName;
    for (const String& ctx : context) {
        oss << "::" << ctx;
    }
    return oss.str();
}

void AdvancedConfigurationManager::logConfigurationError(const String& message) const {
    if (debugMode) {
        std::cerr << "[CONFIG ERROR] " << message << std::endl;
    }
}

void AdvancedConfigurationManager::logConfigurationWarning(const String& message) const {
    if (debugMode) {
        std::cout << "[CONFIG WARNING] " << message << std::endl;
    }
}

// ========== ConfigurationBuilder 实现 ==========

ConfigurationBuilder::ConfigurationBuilder(const String& groupName) {
    configGroup = std::make_shared<NamedConfigurationGroup>(groupName);
}

ConfigurationBuilder& ConfigurationBuilder::description(const String& desc) {
    configGroup->description = desc;
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::version(const String& ver) {
    configGroup->version = ver;
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::author(const String& authorName) {
    configGroup->author = authorName;
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::setDefault(bool isDefault) {
    configGroup->isDefault = isDefault;
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::option(const String& name, const ConfigValue& value) {
    configGroup->setOption(name, value);
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::stringOption(const String& name, const String& value, const String& description) {
    configGroup->setOption(name, value);
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::intOption(const String& name, int value, int minVal, int maxVal) {
    if (value >= minVal && value <= maxVal) {
        configGroup->setOption(name, value);
    } else {
        addBuildError("整数选项 " + name + " 值超出范围 [" + std::to_string(minVal) + ", " + std::to_string(maxVal) + "]");
    }
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::boolOption(const String& name, bool value, const String& description) {
    configGroup->setOption(name, value);
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::arrayOption(const String& name, const StringVector& values) {
    configGroup->setOption(name, values);
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::groupOption(const String& name, const StringVector& options, size_t maxCount) {
    GroupOptionConfig config;
    config.groupName = name;
    config.options = options;
    config.maxCount = maxCount;
    config.minCount = 1;
    configGroup->setGroupOption(name, config);
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::keywordGroup(const String& name, const StringVector& keywords) {
    return groupOption(name, keywords, keywords.size());
}

ConfigurationBuilder& ConfigurationBuilder::keywordMapping(const String& keyword, const StringVector& aliases) {
    KeywordMappingConfig config;
    config.keyword = keyword;
    config.aliases = aliases;
    config.isCore = false;
    config.isExtensible = true;
    configGroup->setKeywordMapping(keyword, config);
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::coreKeyword(const String& keyword, const StringVector& aliases) {
    KeywordMappingConfig config;
    config.keyword = keyword;
    config.aliases = aliases;
    config.isCore = true;
    config.isExtensible = false;
    configGroup->setKeywordMapping(keyword, config);
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::extensibleKeyword(const String& keyword, const StringVector& aliases) {
    KeywordMappingConfig config;
    config.keyword = keyword;
    config.aliases = aliases;
    config.isCore = false;
    config.isExtensible = true;
    configGroup->setKeywordMapping(keyword, config);
    return *this;
}

std::shared_ptr<NamedConfigurationGroup> ConfigurationBuilder::build() {
    if (validateBuild()) {
        return configGroup;
    }
    return nullptr;
}

String ConfigurationBuilder::buildToString() {
    auto group = build();
    return group ? group->toString() : "";
}

bool ConfigurationBuilder::buildToFile(const String& filePath) {
    String content = buildToString();
    if (content.empty()) {
        return false;
    }
    
    // 这里应该实现文件写入逻辑
    // 为了简化，我们只是验证内容不为空
    return true;
}

bool ConfigurationBuilder::validateBuild() const {
    return buildErrors.empty() && configGroup && !configGroup->groupName.empty();
}

void ConfigurationBuilder::addBuildError(const String& error) {
    buildErrors.push_back(error);
}

// ========== ConfigPresets 实现 ==========

namespace ConfigPresets {

std::shared_ptr<NamedConfigurationGroup> createBasicPreset() {
    ConfigurationBuilder builder("Basic");
    return builder
        .description("基础CHTL配置预设")
        .version("1.0.0")
        .author("CHTL Team")
        .setDefault(true)
        .intOption("INDEX_INITIAL_COUNT", 0, 0, 10)
        .boolOption("DISABLE_NAME_GROUP", true)
        .boolOption("DEBUG_MODE", false)
        .boolOption("DISABLE_STYLE_AUTO_ADD_CLASS", false)
        .boolOption("DISABLE_SCRIPT_AUTO_ADD_CLASS", true)
        .groupOption("CUSTOM_STYLE", {"@Style", "@style", "@CSS"}, 3)
        .coreKeyword("KEYWORD_TEMPLATE", {"[Template]"})
        .coreKeyword("KEYWORD_CUSTOM", {"[Custom]"})
        .build();
}

std::shared_ptr<NamedConfigurationGroup> createAdvancedPreset() {
    ConfigurationBuilder builder("Advanced");
    return builder
        .description("高级CHTL配置预设")
        .version("1.0.0")
        .author("CHTL Team")
        .intOption("INDEX_INITIAL_COUNT", 1, 0, 10)
        .boolOption("DISABLE_NAME_GROUP", false)
        .boolOption("DEBUG_MODE", true)
        .boolOption("DISABLE_CUSTOM_ORIGIN_TYPE", false)
        .intOption("OPTION_COUNT", 5, 1, 10)
        .groupOption("CUSTOM_STYLE", {"@Style", "@style", "@CSS", "@Css", "@css"}, 5)
        .groupOption("TEMPLATE_ELEMENT", {"@Element", "@Elem"}, 2)
        .keywordMapping("KEYWORD_INHERIT", {"inherit"})
        .keywordMapping("KEYWORD_DELETE", {"delete"})
        .keywordMapping("KEYWORD_INSERT", {"insert"})
        .build();
}

std::shared_ptr<NamedConfigurationGroup> createPerformancePreset() {
    ConfigurationBuilder builder("Performance");
    return builder
        .description("性能优化CHTL配置预设")
        .version("1.0.0")
        .author("CHTL Team")
        .intOption("INDEX_INITIAL_COUNT", 0, 0, 10)
        .boolOption("DEBUG_MODE", false)
        .boolOption("DISABLE_NAME_GROUP", true)
        .boolOption("DISABLE_STYLE_AUTO_ADD_CLASS", true)
        .boolOption("DISABLE_SCRIPT_AUTO_ADD_CLASS", true)
        .intOption("OPTION_COUNT", 1, 1, 3)
        .groupOption("CUSTOM_STYLE", {"@Style"}, 1)
        .build();
}

std::shared_ptr<NamedConfigurationGroup> createDebugPreset() {
    ConfigurationBuilder builder("Debug");
    return builder
        .description("调试CHTL配置预设")
        .version("1.0.0")
        .author("CHTL Team")
        .boolOption("DEBUG_MODE", true)
        .boolOption("DISABLE_NAME_GROUP", false)
        .boolOption("DISABLE_CUSTOM_ORIGIN_TYPE", false)
        .intOption("OPTION_COUNT", 10, 1, 20)
        .groupOption("CUSTOM_STYLE", {"@Style", "@style", "@CSS", "@Css", "@css"}, 10)
        .groupOption("TEMPLATE_ELEMENT", {"@Element", "@Elem", "@Component"}, 5)
        .extensibleKeyword("KEYWORD_DEBUG", {"debug", "dbg"})
        .build();
}

std::shared_ptr<NamedConfigurationGroup> createWebAppPreset() {
    ConfigurationBuilder builder("WebApp");
    return builder
        .description("Web应用CHTL配置预设")
        .version("1.0.0")
        .author("CHTL Team")
        .intOption("INDEX_INITIAL_COUNT", 0, 0, 10)
        .boolOption("DEBUG_MODE", false)
        .boolOption("DISABLE_STYLE_AUTO_ADD_CLASS", false)
        .boolOption("DISABLE_SCRIPT_AUTO_ADD_CLASS", false)
        .groupOption("CUSTOM_STYLE", {"@Style", "@CSS", "@Component"}, 3)
        .groupOption("TEMPLATE_ELEMENT", {"@Element", "@Component", "@Widget"}, 3)
        .keywordMapping("KEYWORD_COMPONENT", {"@Component", "@Widget"})
        .build();
}

std::shared_ptr<NamedConfigurationGroup> createMobilePreset() {
    ConfigurationBuilder builder("Mobile");
    return builder
        .description("移动端CHTL配置预设")
        .version("1.0.0")
        .author("CHTL Team")
        .intOption("INDEX_INITIAL_COUNT", 0, 0, 10)
        .boolOption("DEBUG_MODE", false)
        .boolOption("DISABLE_STYLE_AUTO_ADD_CLASS", false)
        .boolOption("DISABLE_SCRIPT_AUTO_ADD_CLASS", true)
        .groupOption("CUSTOM_STYLE", {"@Style", "@Mobile"}, 2)
        .keywordMapping("KEYWORD_MOBILE", {"@Mobile", "@Touch"})
        .build();
}

} // namespace ConfigPresets

} // namespace CHTL