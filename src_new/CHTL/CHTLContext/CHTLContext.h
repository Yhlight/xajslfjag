#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stack>

namespace CHTL {
namespace Context {

/**
 * CHTLContext - CHTL上下文管理器
 * 管理编译过程中的上下文信息
 */
class CHTLContext {
public:
    /**
     * 上下文类型
     */
    enum class ContextType {
        GLOBAL,         // 全局上下文
        NAMESPACE,      // 命名空间上下文
        TEMPLATE,       // 模板上下文
        CUSTOM,         // 自定义上下文
        ELEMENT,        // 元素上下文
        STYLE,          // 样式上下文
        SCRIPT,         // 脚本上下文
        ORIGIN,         // 原始嵌入上下文
        IMPORT,         // 导入上下文
        CONFIG          // 配置上下文
    };

private:
    ContextType context_type_;
    std::string context_name_;
    std::unordered_map<std::string, std::string> variables_;
    std::unordered_map<std::string, std::string> attributes_;
    std::stack<ContextType> context_stack_;
    std::string current_file_;
    int current_line_;
    int current_column_;
    
public:
    /**
     * 构造函数
     * @param type 上下文类型
     * @param name 上下文名称
     */
    CHTLContext(ContextType type = ContextType::GLOBAL, 
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
     * 获取当前上下文名称
     * @return 上下文名称
     */
    const std::string& getCurrentContextName() const;
    
    /**
     * 设置变量
     * @param key 变量名
     * @param value 变量值
     */
    void setVariable(const std::string& key, const std::string& value);
    
    /**
     * 获取变量
     * @param key 变量名
     * @return 变量值
     */
    std::string getVariable(const std::string& key) const;
    
    /**
     * 设置属性
     * @param key 属性名
     * @param value 属性值
     */
    void setAttribute(const std::string& key, const std::string& value);
    
    /**
     * 获取属性
     * @param key 属性名
     * @return 属性值
     */
    std::string getAttribute(const std::string& key) const;
    
    /**
     * 设置当前文件位置
     * @param file 文件名
     * @param line 行号
     * @param column 列号
     */
    void setCurrentPosition(const std::string& file, int line, int column);
    
    /**
     * 获取当前文件
     * @return 文件名
     */
    const std::string& getCurrentFile() const;
    
    /**
     * 获取当前行号
     * @return 行号
     */
    int getCurrentLine() const;
    
    /**
     * 获取当前列号
     * @return 列号
     */
    int getCurrentColumn() const;
    
    /**
     * 检查是否在指定上下文中
     * @param type 上下文类型
     * @return 是否在该上下文中
     */
    bool isInContext(ContextType type) const;
    
    /**
     * 获取上下文深度
     * @return 上下文深度
     */
    size_t getContextDepth() const;
    
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
} // namespace CHTL