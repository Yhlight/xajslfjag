#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace Context {

/**
 * CHTLJSContext - CHTL JS上下文管理器
 * 管理CHTL JS编译过程中的上下文信息
 */
class CHTLJSContext {
public:
    /**
     * CHTL JS上下文类型
     */
    enum class ContextType {
        GLOBAL,             // 全局上下文
        SCRIPT_BLOCK,       // script块上下文
        LISTEN_FUNCTION,    // listen函数上下文
        DELEGATE_FUNCTION,  // delegate函数上下文
        ANIMATE_FUNCTION,   // animate函数上下文
        VIR_OBJECT,         // vir对象上下文
        ENHANCED_SELECTOR,  // 增强选择器上下文
        EVENT_HANDLER       // 事件处理器上下文
    };

private:
    ContextType context_type_;
    std::string context_name_;
    std::unordered_map<std::string, std::string> variables_;
    std::unordered_map<std::string, std::string> functions_;
    std::unordered_map<std::string, std::string> vir_objects_;
    std::vector<std::string> enhanced_selectors_;
    
public:
    /**
     * 构造函数
     * @param type 上下文类型
     * @param name 上下文名称
     */
    CHTLJSContext(ContextType type = ContextType::GLOBAL, 
                  const std::string& name = "");
    
    /**
     * 进入新上下文
     * @param type 上下文类型
     * @param name 上下文名称
     */
    void enterContext(ContextType type, const std::string& name = "");
    
    /**
     * 离开当前上下文
     */
    void exitContext();
    
    /**
     * 获取当前上下文类型
     * @return 上下文类型
     */
    ContextType getCurrentContextType() const;
    
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
     * 添加增强选择器
     * @param selector 增强选择器
     */
    void addEnhancedSelector(const std::string& selector);
    
    /**
     * 获取增强选择器列表
     * @return 增强选择器列表
     */
    const std::vector<std::string>& getEnhancedSelectors() const;
    
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
     * 重置上下文
     */
    void reset();
    
    /**
     * 上下文类型转字符串
     * @param type 上下文类型
     * @return 字符串表示
     */
    static std::string contextTypeToString(ContextType type);
};

} // namespace Context
} // namespace CHTLJS
} // namespace CHTL