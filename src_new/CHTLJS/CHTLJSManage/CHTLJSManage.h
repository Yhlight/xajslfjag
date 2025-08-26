#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace Manage {

/**
 * CHTLJSManage - CHTL JS管理器统一接口
 * 整合所有CHTL JS管理功能
 */
class CHTLJSManage {
public:
    /**
     * CHTL JS管理器类型
     */
    enum class ManagerType {
        VIR_OBJECT,         // vir对象管理
        ENHANCED_SELECTOR,  // 增强选择器管理
        EVENT_DELEGATE,     // 事件委托管理
        ANIMATION,          // 动画管理
        FUNCTION_SYNTAX     // 函数语法管理
    };

public:
    /**
     * 初始化所有管理器
     */
    static void initialize();
    
    /**
     * 清理所有管理器
     */
    static void cleanup();
    
    /**
     * 注册vir对象
     * @param name vir对象名称
     * @param definition vir对象定义
     */
    static void registerVirObject(const std::string& name, const std::string& definition);
    
    /**
     * 获取vir对象
     * @param name vir对象名称
     * @return vir对象定义
     */
    static std::string getVirObject(const std::string& name);
    
    /**
     * 处理增强选择器
     * @param selector 增强选择器
     * @return 处理后的选择器
     */
    static std::string processEnhancedSelector(const std::string& selector);
    
    /**
     * 注册事件委托
     * @param parent 父元素选择器
     * @param targets 目标元素列表
     * @param events 事件列表
     */
    static void registerEventDelegate(const std::string& parent,
                                     const std::vector<std::string>& targets,
                                     const std::vector<std::string>& events);
    
    /**
     * 生成事件委托代码
     * @param parent 父元素选择器
     * @return 事件委托JavaScript代码
     */
    static std::string generateEventDelegateCode(const std::string& parent);
    
    /**
     * 注册动画
     * @param name 动画名称
     * @param config 动画配置
     */
    static void registerAnimation(const std::string& name,
                                 const std::unordered_map<std::string, std::string>& config);
    
    /**
     * 生成动画CSS和JavaScript
     * @param name 动画名称
     * @return [CSS代码, JavaScript代码]
     */
    static std::pair<std::string, std::string> generateAnimationCode(const std::string& name);
    
    /**
     * 处理CHTL JS函数语法（新语法）
     * @param function_name 函数名称
     * @param function_body 函数体
     * @return 处理后的JavaScript代码
     */
    static std::string processCHTLJSFunction(const std::string& function_name,
                                            const std::unordered_map<std::string, std::string>& function_body);
    
    /**
     * 验证事件名称
     * @param event_name 事件名称
     * @return 是否为有效事件名
     */
    static bool isValidEventName(const std::string& event_name);
    
    /**
     * 获取所有支持的事件列表
     * @return 事件名称列表
     */
    static std::vector<std::string> getSupportedEvents();

private:
    static bool initialized_;
    static std::unordered_map<std::string, std::string> vir_objects_;
    static std::unordered_map<std::string, std::string> enhanced_selectors_;
    static std::unordered_map<std::string, std::vector<std::string>> event_delegates_;
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> animations_;
    
    /**
     * 生成vir对象访问代码
     * @param vir_name vir对象名称
     * @param access_key 访问键
     * @return JavaScript代码
     */
    static std::string generateVirAccessCode(const std::string& vir_name, 
                                            const std::string& access_key);
    
    /**
     * 解析增强选择器
     * @param selector 增强选择器
     * @return 解析结果
     */
    static std::string parseEnhancedSelector(const std::string& selector);
    
    /**
     * 生成关键帧CSS
     * @param animation_config 动画配置
     * @return CSS关键帧代码
     */
    static std::string generateKeyframeCSS(const std::unordered_map<std::string, std::string>& animation_config);
};

} // namespace Manage
} // namespace CHTLJS
} // namespace CHTL