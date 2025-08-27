#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {
namespace Common {

/**
 * SelectorAutomationManager - 选择器自动化管理器
 * 严格按照"完善选择器自动化与引用规则.ini"实现
 */
class SelectorAutomationManager {
public:
    /**
     * 自动化配置（严格按照规则文档）
     */
    struct AutomationConfig {
        bool DISABLE_STYLE_AUTO_ADD_CLASS = false;      // 禁止局部样式块自动添加类选择器
        bool DISABLE_STYLE_AUTO_ADD_ID = false;         // 禁止局部样式块自动添加id选择器
        bool DISABLE_SCRIPT_AUTO_ADD_CLASS = true;      // 禁止局部脚本自动添加类选择器
        bool DISABLE_SCRIPT_AUTO_ADD_ID = true;         // 禁止局部脚本自动添加id选择器
    };
    
    /**
     * 选择器类型
     */
    enum class SelectorType {
        CLASS,          // .class
        ID,             // #id
        ELEMENT,        // element
        ENHANCED        // {{selector}}
    };
    
    /**
     * 上下文类型
     */
    enum class ContextType {
        LOCAL_STYLE,    // 局部style块
        LOCAL_SCRIPT    // 局部script块
    };
    
    /**
     * 选择器信息
     */
    struct SelectorInfo {
        SelectorType type;
        std::string name;
        std::string full_selector;
        
        SelectorInfo(SelectorType t, const std::string& n, const std::string& full) 
            : type(t), name(n), full_selector(full) {}
    };
    
    /**
     * 分析选择器并应用自动化规则
     * @param selectors 当前元素中的选择器列表
     * @param context 上下文类型
     * @param current_class 当前class属性值
     * @param current_id 当前id属性值
     * @param config 自动化配置
     * @return [需要添加的class, 需要添加的id]
     */
    static std::pair<std::string, std::string> applyAutomationRules(
        const std::vector<std::string>& selectors,
        ContextType context,
        const std::string& current_class,
        const std::string& current_id,
        const AutomationConfig& config
    );
    
    /**
     * 解析选择器
     * @param selector_text 选择器文本
     * @return 选择器信息
     */
    static SelectorInfo parseSelector(const std::string& selector_text);
    
    /**
     * 获取&引用的目标选择器
     * 严格按照规则：style中优先选择class，script中优先选择id
     * @param context 上下文类型
     * @param current_class 当前class属性
     * @param current_id 当前id属性
     * @return &引用的选择器
     */
    static std::string resolveAmpersandReference(
        ContextType context,
        const std::string& current_class,
        const std::string& current_id
    );
    
    /**
     * 检查是否需要自动添加class
     * 规则：当局部style内部存在多组类选择器时，若class属性缺失，则自动添加第一个类选择器
     * @param selectors 选择器列表
     * @param context 上下文类型
     * @param current_class 当前class属性
     * @param config 配置
     * @return 需要添加的class名称，空字符串表示不需要
     */
    static std::string getAutoAddClass(
        const std::vector<SelectorInfo>& selectors,
        ContextType context,
        const std::string& current_class,
        const AutomationConfig& config
    );
    
    /**
     * 检查是否需要自动添加id
     * 规则：当局部style内部存在多组id选择器时，若id属性缺失，则自动添加第一个id选择器
     * @param selectors 选择器列表
     * @param context 上下文类型
     * @param current_id 当前id属性
     * @param config 配置
     * @return 需要添加的id名称，空字符串表示不需要
     */
    static std::string getAutoAddId(
        const std::vector<SelectorInfo>& selectors,
        ContextType context,
        const std::string& current_id,
        const AutomationConfig& config
    );
    
    /**
     * 检查enhanced selector是否能触发自动化
     * 规则：{{.box}}和{{#box}}能够触发自动化添加，但{{box}}不会
     * @param enhanced_selector enhanced selector内容
     * @return 是否能触发自动化
     */
    static bool canTriggerAutomation(const std::string& enhanced_selector);
    
    /**
     * 从enhanced selector提取选择器信息
     * @param enhanced_selector enhanced selector内容
     * @return 选择器信息
     */
    static SelectorInfo extractFromEnhancedSelector(const std::string& enhanced_selector);
    
    /**
     * 处理script中的特殊规则
     * 规则：当局部script内部存在多组类选择器时，若class属性，局部style没有触发class自动化添加缺失，第一个{{.box}}会被自动添加
     * @param enhanced_selectors script中的enhanced selectors
     * @param style_triggered_class style是否已经触发了class自动化
     * @param style_triggered_id style是否已经触发了id自动化
     * @param config 配置
     * @return [需要添加的class, 需要添加的id]
     */
    static std::pair<std::string, std::string> handleScriptSpecialRules(
        const std::vector<std::string>& enhanced_selectors,
        bool style_triggered_class,
        bool style_triggered_id,
        const AutomationConfig& config
    );

private:
    /**
     * 提取类名从选择器
     * @param selector 选择器字符串
     * @return 类名，如果不是class选择器返回空
     */
    static std::string extractClassName(const std::string& selector);
    
    /**
     * 提取ID名从选择器
     * @param selector 选择器字符串
     * @return ID名，如果不是id选择器返回空
     */
    static std::string extractIdName(const std::string& selector);
    
    /**
     * 检查选择器是否为class选择器
     * @param selector 选择器字符串
     * @return 是否为class选择器
     */
    static bool isClassSelector(const std::string& selector);
    
    /**
     * 检查选择器是否为id选择器
     * @param selector 选择器字符串
     * @return 是否为id选择器
     */
    static bool isIdSelector(const std::string& selector);
    
    /**
     * 清理选择器字符串
     * @param selector 原始选择器
     * @return 清理后的选择器
     */
    static std::string cleanSelector(const std::string& selector);
    
    /**
     * 获取第一个类选择器
     * @param selectors 选择器信息列表
     * @return 第一个类选择器的名称
     */
    static std::string getFirstClassSelector(const std::vector<SelectorInfo>& selectors);
    
    /**
     * 获取第一个ID选择器
     * @param selectors 选择器信息列表
     * @return 第一个ID选择器的名称
     */
    static std::string getFirstIdSelector(const std::vector<SelectorInfo>& selectors);
};

/**
 * ConfigurationManager - 配置管理器
 * 处理[Configuration]块中的各种配置选项
 */
class ConfigurationManager {
public:
    /**
     * 全局配置结构
     */
    struct GlobalConfig {
        SelectorAutomationManager::AutomationConfig automation;
        bool DISABLE_DEFAULT_NAMESPACE = false;    // 禁用默认命名空间功能
        
        // 可以添加更多配置选项
    };
    
    /**
     * 解析Configuration块
     * @param config_content 配置内容
     * @return 解析后的配置
     */
    static GlobalConfig parseConfiguration(const std::string& config_content);
    
    /**
     * 获取全局配置
     * @return 当前全局配置
     */
    static const GlobalConfig& getGlobalConfig();
    
    /**
     * 更新全局配置
     * @param config 新的配置
     */
    static void updateGlobalConfig(const GlobalConfig& config);
    
    /**
     * 重置为默认配置
     */
    static void resetToDefault();

private:
    static GlobalConfig global_config_;
    
    /**
     * 解析配置项
     * @param line 配置行
     * @param config 配置对象引用
     */
    static void parseConfigLine(const std::string& line, GlobalConfig& config);
    
    /**
     * 解析布尔值
     * @param value_str 值字符串
     * @return 布尔值
     */
    static bool parseBoolValue(const std::string& value_str);
};

} // namespace Common
} // namespace CHTL