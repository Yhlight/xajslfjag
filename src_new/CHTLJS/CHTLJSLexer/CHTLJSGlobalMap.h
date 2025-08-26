#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace Lexer {

/**
 * CHTLJSGlobalMap - CHTL JS全局映射表
 * 管理CHTL JS编译过程中的全局状态
 */
class CHTLJSGlobalMap {
private:
    // 单例实例
    static std::shared_ptr<CHTLJSGlobalMap> instance_;
    
    // vir对象映射
    std::unordered_map<std::string, std::string> vir_objects_;
    
    // CHTL JS函数映射
    std::unordered_map<std::string, std::string> chtljs_functions_;
    
    // 增强选择器映射
    std::unordered_map<std::string, std::string> enhanced_selectors_;
    
    // 事件委托映射
    std::unordered_map<std::string, std::vector<std::string>> event_delegates_;
    
    // 动画映射
    std::unordered_map<std::string, std::string> animations_;
    
    // 全局事件注册表
    std::unordered_set<std::string> registered_events_;
    
    // 错误和警告
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 当前编译文件
    std::string current_file_;
    
    // 私有构造函数（单例模式）
    CHTLJSGlobalMap() = default;
    
public:
    /**
     * 获取单例实例
     * @return CHTLJSGlobalMap实例
     */
    static std::shared_ptr<CHTLJSGlobalMap> getInstance();
    
    /**
     * 重置全局状态
     */
    void reset();
    
    // vir对象管理
    /**
     * 注册vir对象
     * @param name vir对象名称
     * @param definition vir对象定义
     */
    void registerVirObject(const std::string& name, const std::string& definition);
    
    /**
     * 获取vir对象
     * @param name vir对象名称
     * @return vir对象定义
     */
    std::string getVirObject(const std::string& name) const;
    
    /**
     * 检查vir对象是否存在
     * @param name vir对象名称
     * @return 是否存在
     */
    bool hasVirObject(const std::string& name) const;
    
    // CHTL JS函数管理
    /**
     * 注册CHTL JS函数
     * @param name 函数名称
     * @param definition 函数定义
     */
    void registerCHTLJSFunction(const std::string& name, const std::string& definition);
    
    /**
     * 获取CHTL JS函数
     * @param name 函数名称
     * @return 函数定义
     */
    std::string getCHTLJSFunction(const std::string& name) const;
    
    /**
     * 检查CHTL JS函数是否存在
     * @param name 函数名称
     * @return 是否存在
     */
    bool hasCHTLJSFunction(const std::string& name) const;
    
    // 增强选择器管理
    /**
     * 注册增强选择器
     * @param selector 选择器
     * @param target 目标元素
     */
    void registerEnhancedSelector(const std::string& selector, const std::string& target);
    
    /**
     * 获取增强选择器目标
     * @param selector 选择器
     * @return 目标元素
     */
    std::string getEnhancedSelectorTarget(const std::string& selector) const;
    
    // 事件委托管理
    /**
     * 注册事件委托
     * @param parent 父元素选择器
     * @param targets 目标元素列表
     */
    void registerEventDelegate(const std::string& parent, const std::vector<std::string>& targets);
    
    /**
     * 获取事件委托目标
     * @param parent 父元素选择器
     * @return 目标元素列表
     */
    std::vector<std::string> getEventDelegateTargets(const std::string& parent) const;
    
    // 动画管理
    /**
     * 注册动画
     * @param name 动画名称
     * @param definition 动画定义
     */
    void registerAnimation(const std::string& name, const std::string& definition);
    
    /**
     * 获取动画定义
     * @param name 动画名称
     * @return 动画定义
     */
    std::string getAnimation(const std::string& name) const;
    
    // 事件注册管理
    /**
     * 注册全局事件
     * @param event_name 事件名称
     */
    void registerGlobalEvent(const std::string& event_name);
    
    /**
     * 检查事件是否已注册
     * @param event_name 事件名称
     * @return 是否已注册
     */
    bool isEventRegistered(const std::string& event_name) const;
    
    // 错误和警告管理
    /**
     * 添加错误
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * 添加警告
     * @param warning 警告信息
     */
    void addWarning(const std::string& warning);
    
    /**
     * 获取错误列表
     * @return 错误列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * 获取警告列表
     * @return 警告列表
     */
    const std::vector<std::string>& getWarnings() const;
    
    /**
     * 清除错误和警告
     */
    void clearMessages();
    
    // 当前文件管理
    /**
     * 设置当前编译文件
     * @param file_path 文件路径
     */
    void setCurrentFile(const std::string& file_path);
    
    /**
     * 获取当前编译文件
     * @return 文件路径
     */
    const std::string& getCurrentFile() const;
};

} // namespace Lexer
} // namespace CHTLJS
} // namespace CHTL