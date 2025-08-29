#include "AdvancedConfigurationManager.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>
#include <regex>
#include <numeric>

namespace CHTL {

// ========== StandardConfigValidator 实现 ==========

bool StandardConfigValidator::validateOption(const String& name, const ConfigValue& value, 
                                            const ConfigOptionMetadata& metadata) const {
    // 验证值类型
    if (!validateValueType(value, metadata.type)) {
        return false;
    }
    
    // 验证值范围
    if (metadata.type == ConfigOptionType::INTEGER || metadata.type == ConfigOptionType::DOUBLE) {
        if (!validateValueRange(value, metadata.minValue, metadata.maxValue)) {
            return false;
        }
    }
    
    // 验证允许的值
    if (!metadata.allowedValues.empty()) {
        if (!validateAllowedValues(value, metadata.allowedValues)) {
            return false;
        }
    }
    
    // 安全检查
    if (!isSecureOption(name, value)) {
        return false;
    }
    
    return true;
}

bool StandardConfigValidator::validateGroup(const NamedConfigurationGroup& group) const {
    // 基本完整性检查
    if (group.groupName.empty()) {
        return false;
    }
    
    // 验证依赖关系
    if (!validateDependencies(group)) {
        return false;
    }
    
    // 验证冲突
    if (!validateConflicts(group)) {
        return false;
    }
    
    // 安全验证
    if (!validateSecurity(group)) {
        return false;
    }
    
    // 性能验证
    if (!validatePerformance(group)) {
        return false;
    }
    
    return true;
}

bool StandardConfigValidator::validateDependencies(const NamedConfigurationGroup& group) const {
    // 检查每个选项的依赖是否满足
    for (const auto& [optionName, metadata] : group.optionMetadata) {
        for (const String& dependency : metadata.dependencies) {
            if (!group.hasOption(dependency)) {
                return false; // 依赖项不存在
            }
        }
    }
    
    return true;
}

bool StandardConfigValidator::validateConflicts(const NamedConfigurationGroup& group) const {
    // 检查冲突的选项是否同时存在
    for (const auto& [optionName, metadata] : group.optionMetadata) {
        if (group.hasOption(optionName)) {
            for (const String& conflict : metadata.conflicts) {
                if (group.hasOption(conflict)) {
                    return false; // 发现冲突
                }
            }
        }
    }
    
    return true;
}

StringVector StandardConfigValidator::getValidationErrors(const NamedConfigurationGroup& group) const {
    StringVector errors;
    
    // 检查必需选项
    for (const auto& [optionName, metadata] : group.optionMetadata) {
        if (metadata.isRequired && !group.hasOption(optionName)) {
            errors.push_back("Required option missing: " + optionName);
        }
    }
    
    // 检查依赖
    if (!validateDependencies(group)) {
        errors.push_back("Dependency validation failed");
    }
    
    // 检查冲突
    if (!validateConflicts(group)) {
        errors.push_back("Conflicting options detected");
    }
    
    return errors;
}

StringVector StandardConfigValidator::getValidationWarnings(const NamedConfigurationGroup& group) const {
    StringVector warnings;
    
    // 检查弃用的选项
    for (const auto& [optionName, value] : group.options) {
        auto metaIt = group.optionMetadata.find(optionName);
        if (metaIt != group.optionMetadata.end() && metaIt->second.isDeprecated) {
            warnings.push_back("Deprecated option used: " + optionName + 
                              (metaIt->second.deprecationMessage.empty() ? "" : 
                               " - " + metaIt->second.deprecationMessage));
        }
    }
    
    return warnings;
}

bool StandardConfigValidator::validateSecurity(const NamedConfigurationGroup& group) const {
    for (const auto& [optionName, value] : group.options) {
        if (!isSecureOption(optionName, value)) {
            return false;
        }
    }
    return true;
}

bool StandardConfigValidator::validatePerformance(const NamedConfigurationGroup& group) const {
    // 检查性能关键选项
    for (const auto& [optionName, value] : group.options) {
        if (isPerformanceCritical(optionName)) {
            // 可以添加具体的性能验证逻辑
        }
    }
    return true;
}

bool StandardConfigValidator::validateValueType(const ConfigValue& value, ConfigOptionType expectedType) const {
    switch (expectedType) {
        case ConfigOptionType::STRING:
            return std::holds_alternative<String>(value);
        case ConfigOptionType::INTEGER:
            return std::holds_alternative<int>(value);
        case ConfigOptionType::DOUBLE:
            return std::holds_alternative<double>(value);
        case ConfigOptionType::BOOLEAN:
            return std::holds_alternative<bool>(value);
        case ConfigOptionType::STRING_ARRAY:
            return std::holds_alternative<StringVector>(value);
        default:
            return true;
    }
}

bool StandardConfigValidator::validateValueRange(const ConfigValue& value, 
                                                const ConfigValue& min, 
                                                const ConfigValue& max) const {
    if (std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        int minVal = std::holds_alternative<int>(min) ? std::get<int>(min) : INT_MIN;
        int maxVal = std::holds_alternative<int>(max) ? std::get<int>(max) : INT_MAX;
        return val >= minVal && val <= maxVal;
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        double minVal = std::holds_alternative<double>(min) ? std::get<double>(min) : -DBL_MAX;
        double maxVal = std::holds_alternative<double>(max) ? std::get<double>(max) : DBL_MAX;
        return val >= minVal && val <= maxVal;
    }
    return true;
}

bool StandardConfigValidator::validateAllowedValues(const ConfigValue& value, 
                                                   const StringVector& allowedValues) const {
    if (std::holds_alternative<String>(value)) {
        String val = std::get<String>(value);
        return std::find(allowedValues.begin(), allowedValues.end(), val) != allowedValues.end();
    }
    return true;
}

bool StandardConfigValidator::isSecureOption(const String& name, const ConfigValue& value) const {
    // 检查危险的配置选项
    if (name.find("password") != String::npos || name.find("secret") != String::npos) {
        if (std::holds_alternative<String>(value)) {
            String val = std::get<String>(value);
            return !val.empty() && val != "default" && val != "admin";
        }
    }
    
    // 检查路径遍历
    if (name.find("path") != String::npos || name.find("file") != String::npos) {
        if (std::holds_alternative<String>(value)) {
            String val = std::get<String>(value);
            return val.find("..") == String::npos;
        }
    }
    
    return true;
}

bool StandardConfigValidator::isPerformanceCritical(const String& name) const {
    StringVector criticalOptions = {"cache_size", "max_connections", "timeout", "buffer_size"};
    return std::find(criticalOptions.begin(), criticalOptions.end(), name) != criticalOptions.end();
}

// ========== ConfigurationMigrator 实现 ==========

ConfigurationMigrator::ConfigurationMigrator(const String& version) : currentVersion(version) {}

void ConfigurationMigrator::addMigrationRule(const MigrationRule& rule) {
    migrationRules.push_back(rule);
}

void ConfigurationMigrator::addMigrationRule(const String& fromVersion, const String& toVersion,
                                           const String& oldName, const String& newName,
                                           std::function<ConfigValue(const ConfigValue&)> transformer) {
    MigrationRule rule;
    rule.fromVersion = fromVersion;
    rule.toVersion = toVersion;
    rule.oldOptionName = oldName;
    rule.newOptionName = newName;
    rule.transformer = transformer;
    rule.description = "Migrate " + oldName + " to " + newName;
    
    addMigrationRule(rule);
}

bool ConfigurationMigrator::migrateConfiguration(NamedConfigurationGroup& group, const String& targetVersion) {
    if (group.version == targetVersion) {
        return true; // 已经是目标版本
    }
    
    auto migrationPath = getMigrationPath(group.version, targetVersion);
    if (migrationPath.empty()) {
        return false; // 无法找到迁移路径
    }
    
    // 应用迁移规则
    for (const String& version : migrationPath) {
        for (const auto& rule : migrationRules) {
            if (rule.fromVersion == group.version && rule.toVersion == version) {
                // 应用迁移规则
                if (group.hasOption(rule.oldOptionName)) {
                    ConfigValue oldValue = group.getOption(rule.oldOptionName);
                    ConfigValue newValue = rule.transformer ? rule.transformer(oldValue) : oldValue;
                    
                    group.removeOption(rule.oldOptionName);
                    group.setOption(rule.newOptionName, newValue);
                }
            }
        }
        group.version = version;
    }
    
    return true;
}

std::unique_ptr<NamedConfigurationGroup> ConfigurationMigrator::migrateConfiguration(
    const NamedConfigurationGroup& group, const String& targetVersion) const {
    auto migrated = std::make_unique<NamedConfigurationGroup>(group.groupName);
    *migrated = group; // 复制原始配置
    
    const_cast<ConfigurationMigrator*>(this)->migrateConfiguration(*migrated, targetVersion);
    
    return migrated;
}

bool ConfigurationMigrator::needsMigration(const NamedConfigurationGroup& group, const String& targetVersion) const {
    return group.version != targetVersion && !getMigrationPath(group.version, targetVersion).empty();
}

StringVector ConfigurationMigrator::getMigrationPath(const String& fromVersion, const String& toVersion) const {
    // 简化的版本路径计算
    StringVector path;
    
    // 假设版本格式为 "1.0", "1.1", "2.0" 等
    std::regex versionRegex(R"((\d+)\.(\d+))");
    std::smatch fromMatch, toMatch;
    
    if (std::regex_match(fromVersion, fromMatch, versionRegex) && 
        std::regex_match(toVersion, toMatch, versionRegex)) {
        
        int fromMajor = std::stoi(fromMatch[1].str());
        int fromMinor = std::stoi(fromMatch[2].str());
        int toMajor = std::stoi(toMatch[1].str());
        int toMinor = std::stoi(toMatch[2].str());
        
        // 生成版本路径
        for (int major = fromMajor; major <= toMajor; ++major) {
            int startMinor = (major == fromMajor) ? fromMinor + 1 : 0;
            int endMinor = (major == toMajor) ? toMinor : 9; // 假设最大小版本号为9
            
            for (int minor = startMinor; minor <= endMinor; ++minor) {
                path.push_back(std::to_string(major) + "." + std::to_string(minor));
                if (major == toMajor && minor == toMinor) {
                    break;
                }
            }
        }
    }
    
    return path;
}

bool ConfigurationMigrator::validateMigration(const NamedConfigurationGroup& original, 
                                            const NamedConfigurationGroup& migrated) const {
    // 验证迁移是否成功
    return migrated.version != original.version;
}

String ConfigurationMigrator::getMigrationReport(const String& fromVersion, const String& toVersion) const {
    std::ostringstream oss;
    oss << "Migration Report: " << fromVersion << " -> " << toVersion << "\n";
    
    auto path = getMigrationPath(fromVersion, toVersion);
    if (path.empty()) {
        oss << "No migration path available.\n";
    } else {
        oss << "Migration path: ";
        for (size_t i = 0; i < path.size(); ++i) {
            if (i > 0) oss << " -> ";
            oss << path[i];
        }
        oss << "\n";
        
        oss << "Applied rules:\n";
        for (const auto& rule : migrationRules) {
            if (std::find(path.begin(), path.end(), rule.toVersion) != path.end()) {
                oss << "- " << rule.description << "\n";
            }
        }
    }
    
    return oss.str();
}

// ========== ConfigurationTemplate 实现 ==========

ConfigurationTemplate::ConfigurationTemplate(const String& name) : templateName(name) {}

bool ConfigurationTemplate::applyToGroup(NamedConfigurationGroup& group) const {
    // 应用模板选项到配置组
    for (const auto& [optionName, metadata] : templateOptions) {
        group.optionMetadata[optionName] = metadata;
        
        // 如果没有设置值，使用默认值
        if (!group.hasOption(optionName)) {
            group.setOption(optionName, metadata.defaultValue);
        }
    }
    
    return true;
}

std::unique_ptr<NamedConfigurationGroup> ConfigurationTemplate::createFromTemplate() const {
    auto group = std::make_unique<NamedConfigurationGroup>(templateName + "_instance");
    applyToGroup(*group);
    return group;
}

bool ConfigurationTemplate::validateGroup(const NamedConfigurationGroup& group) const {
    // 验证所有必需选项是否存在
    for (const String& requiredOption : requiredOptions) {
        if (!group.hasOption(requiredOption)) {
            return false;
        }
    }
    
    return true;
}

StringVector ConfigurationTemplate::getComplianceErrors(const NamedConfigurationGroup& group) const {
    StringVector errors;
    
    for (const String& requiredOption : requiredOptions) {
        if (!group.hasOption(requiredOption)) {
            errors.push_back("Missing required option: " + requiredOption);
        }
    }
    
    return errors;
}

ConfigurationTemplate ConfigurationTemplate::merge(const ConfigurationTemplate& other) const {
    ConfigurationTemplate merged(templateName + "_" + other.templateName);
    
    // 合并模板选项
    merged.templateOptions = templateOptions;
    for (const auto& [name, metadata] : other.templateOptions) {
        merged.templateOptions[name] = metadata;
    }
    
    // 合并必需选项
    merged.requiredOptions = requiredOptions;
    for (const String& option : other.requiredOptions) {
        if (std::find(merged.requiredOptions.begin(), merged.requiredOptions.end(), option) == 
            merged.requiredOptions.end()) {
            merged.requiredOptions.push_back(option);
        }
    }
    
    // 合并可选选项
    merged.optionalOptions = optionalOptions;
    for (const String& option : other.optionalOptions) {
        if (std::find(merged.optionalOptions.begin(), merged.optionalOptions.end(), option) == 
            merged.optionalOptions.end()) {
            merged.optionalOptions.push_back(option);
        }
    }
    
    return merged;
}

String ConfigurationTemplate::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"templateName\": \"" << templateName << "\",\n";
    oss << "  \"description\": \"" << description << "\",\n";
    oss << "  \"category\": \"" << category << "\",\n";
    oss << "  \"requiredOptions\": [";
    for (size_t i = 0; i < requiredOptions.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << requiredOptions[i] << "\"";
    }
    oss << "],\n";
    oss << "  \"optionalOptions\": [";
    for (size_t i = 0; i < optionalOptions.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << optionalOptions[i] << "\"";
    }
    oss << "]\n";
    oss << "}";
    
    return oss.str();
}

bool ConfigurationTemplate::fromJSON(const String& json) {
    // 简化的JSON解析实现
    // 在实际应用中应该使用专业的JSON库
    
    std::regex nameRegex(R"("templateName":\s*"([^"]+)")");
    std::regex descRegex(R"("description":\s*"([^"]+)")");
    std::regex categoryRegex(R"("category":\s*"([^"]+)")");
    
    std::smatch match;
    
    if (std::regex_search(json, match, nameRegex)) {
        templateName = match[1].str();
    }
    
    if (std::regex_search(json, match, descRegex)) {
        description = match[1].str();
    }
    
    if (std::regex_search(json, match, categoryRegex)) {
        category = match[1].str();
    }
    
    return true;
}

// ========== ConfigurationPresetManager 实现 ==========

ConfigurationPresetManager::ConfigurationPresetManager(const String& presetsDir) 
    : presetsDirectory(presetsDir) {}

void ConfigurationPresetManager::addPreset(const String& name, std::unique_ptr<NamedConfigurationGroup> preset) {
    presets[name] = std::move(preset);
}

std::unique_ptr<NamedConfigurationGroup> ConfigurationPresetManager::getPreset(const String& name) const {
    auto it = presets.find(name);
    if (it != presets.end()) {
        // 返回预设的深拷贝
        auto copy = std::make_unique<NamedConfigurationGroup>(it->second->groupName);
        *copy = *(it->second);
        return copy;
    }
    return nullptr;
}

bool ConfigurationPresetManager::hasPreset(const String& name) const {
    return presets.find(name) != presets.end();
}

void ConfigurationPresetManager::removePreset(const String& name) {
    presets.erase(name);
}

StringVector ConfigurationPresetManager::getPresetNames() const {
    StringVector names;
    for (const auto& [name, preset] : presets) {
        names.push_back(name);
    }
    return names;
}

void ConfigurationPresetManager::addTemplate(const String& name, std::unique_ptr<ConfigurationTemplate> template_) {
    templates[name] = std::move(template_);
}

std::unique_ptr<ConfigurationTemplate> ConfigurationPresetManager::getTemplate(const String& name) const {
    auto it = templates.find(name);
    if (it != templates.end()) {
        return std::make_unique<ConfigurationTemplate>(*(it->second));
    }
    return nullptr;
}

bool ConfigurationPresetManager::hasTemplate(const String& name) const {
    return templates.find(name) != templates.end();
}

void ConfigurationPresetManager::removeTemplate(const String& name) {
    templates.erase(name);
}

StringVector ConfigurationPresetManager::getTemplateNames() const {
    StringVector names;
    for (const auto& [name, template_] : templates) {
        names.push_back(name);
    }
    return names;
}

bool ConfigurationPresetManager::loadPresetsFromDirectory() {
    // 简化实现 - 在实际应用中应该扫描目录并加载文件
    return true;
}

bool ConfigurationPresetManager::savePresetToFile(const String& name) const {
    auto preset = getPreset(name);
    if (!preset) {
        return false;
    }
    
    String filePath = presetsDirectory + "/" + name + ".json";
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    // 简化的JSON序列化
    file << "{\n";
    file << "  \"groupName\": \"" << preset->groupName << "\",\n";
    file << "  \"description\": \"" << preset->description << "\",\n";
    file << "  \"version\": \"" << preset->version << "\"\n";
    file << "}";
    
    return true;
}

bool ConfigurationPresetManager::loadPresetFromFile(const String& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    String content((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
    
    // 简化的JSON解析
    std::regex nameRegex(R"("groupName":\s*"([^"]+)")");
    std::smatch match;
    
    if (std::regex_search(content, match, nameRegex)) {
        String name = match[1].str();
        auto preset = std::make_unique<NamedConfigurationGroup>(name);
        addPreset(name, std::move(preset));
        return true;
    }
    
    return false;
}

StringVector ConfigurationPresetManager::searchPresets(const String& query) const {
    StringVector results;
    String lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& [name, preset] : presets) {
        String lowerName = name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName.find(lowerQuery) != String::npos) {
            results.push_back(name);
        }
    }
    
    return results;
}

StringVector ConfigurationPresetManager::getPresetsByCategory(const String& category) const {
    StringVector results;
    
    for (const auto& [name, preset] : presets) {
        // 假设预设有 category 属性（需要扩展 NamedConfigurationGroup）
        // 这里使用简化实现
        results.push_back(name);
    }
    
    return results;
}

StringVector ConfigurationPresetManager::getPresetsByTag(const String& tag) const {
    StringVector results;
    
    // 简化实现 - 在实际应用中应该检查预设的标签
    for (const auto& [name, preset] : presets) {
        results.push_back(name);
    }
    
    return results;
}

std::unique_ptr<NamedConfigurationGroup> ConfigurationPresetManager::createPresetFromTemplate(
    const String& templateName, const StringUnorderedMap& customOptions) const {
    
    auto template_ = getTemplate(templateName);
    if (!template_) {
        return nullptr;
    }
    
    auto preset = template_->createFromTemplate();
    
    // 应用自定义选项
    for (const auto& [key, value] : customOptions) {
        preset->setOption(key, ConfigValue(value));
    }
    
    return preset;
}

// ========== AdvancedConfigurationMonitor 实现 ==========

AdvancedConfigurationMonitor::AdvancedConfigurationMonitor() : isMonitoring(false) {}

void AdvancedConfigurationMonitor::addListener(std::shared_ptr<ConfigurationListener> listener) {
    listeners.push_back(listener);
}

void AdvancedConfigurationMonitor::removeListener(std::shared_ptr<ConfigurationListener> listener) {
    listeners.erase(
        std::remove(listeners.begin(), listeners.end(), listener),
        listeners.end()
    );
}

void AdvancedConfigurationMonitor::clearListeners() {
    listeners.clear();
}

void AdvancedConfigurationMonitor::notifyConfigurationChanged(const String& groupName, 
                                                            const String& optionName, 
                                                            const ConfigValue& oldValue, 
                                                            const ConfigValue& newValue) {
    if (!isMonitoring) return;
    
    changeTimestamps[groupName] = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    
    for (auto& listener : listeners) {
        if (listener) {
            listener->onConfigurationChanged(groupName, optionName, oldValue, newValue);
        }
    }
}

void AdvancedConfigurationMonitor::notifyConfigurationGroupCreated(const String& groupName) {
    if (!isMonitoring) return;
    
    for (auto& listener : listeners) {
        if (listener) {
            listener->onConfigurationGroupCreated(groupName);
        }
    }
}

void AdvancedConfigurationMonitor::notifyConfigurationGroupDeleted(const String& groupName) {
    if (!isMonitoring) return;
    
    changeTimestamps.erase(groupName);
    
    for (auto& listener : listeners) {
        if (listener) {
            listener->onConfigurationGroupDeleted(groupName);
        }
    }
}

void AdvancedConfigurationMonitor::notifyValidationFailed(const String& groupName, const StringVector& errors) {
    if (!isMonitoring) return;
    
    for (auto& listener : listeners) {
        if (listener) {
            listener->onValidationFailed(groupName, errors);
        }
    }
}

void AdvancedConfigurationMonitor::startMonitoring() {
    isMonitoring = true;
}

void AdvancedConfigurationMonitor::stopMonitoring() {
    isMonitoring = false;
}

std::chrono::milliseconds AdvancedConfigurationMonitor::getLastChangeTime(const String& groupName) const {
    auto it = changeTimestamps.find(groupName);
    return it != changeTimestamps.end() ? it->second : std::chrono::milliseconds(0);
}

StringVector AdvancedConfigurationMonitor::getRecentChanges(std::chrono::milliseconds since) const {
    StringVector recentChanges;
    
    for (const auto& [groupName, timestamp] : changeTimestamps) {
        if (timestamp >= since) {
            recentChanges.push_back(groupName);
        }
    }
    
    return recentChanges;
}

} // namespace CHTL