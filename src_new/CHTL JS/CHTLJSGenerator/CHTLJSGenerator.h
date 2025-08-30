#pragma once
#include "../CHTLJSNode/BaseNode.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <unordered_map>

namespace CHTLJS {

/**
 * CHTL JS代码生成选项
 */
struct GenerateOptions {
    bool optimizeOutput = true;             // 是否优化输出
    bool generateComments = false;          // 是否生成注释
    bool minifyOutput = false;              // 是否压缩输出
    std::string indentation = "    ";       // 缩进字符
};

/**
 * CHTL JS代码生成器
 * 将CHTL JS AST转换为标准JavaScript代码
 * 严格按照CHTL语法文档中的CHTL JS转换规则
 */
class CHTLJSGenerator {
public:
    /**
     * 构造函数
     */
    explicit CHTLJSGenerator(const GenerateOptions& options = GenerateOptions());
    
    /**
     * 析构函数
     */
    ~CHTLJSGenerator() = default;
    
    // ========== 主要生成方法 ==========
    
    /**
     * 从AST生成JavaScript代码
     */
    std::string generateJS(const NodePtr& root);
    
    /**
     * 生成模块加载代码
     */
    std::string generateModuleCode(const NodePtr& moduleNode);
    
    /**
     * 生成增强选择器代码
     * {{CSS选择器}} -> document.querySelector/querySelectorAll
     */
    std::string generateEnhancedSelector(const NodePtr& selectorNode);
    
    /**
     * 生成事件监听器代码
     * listen -> addEventListener
     */
    std::string generateEventListener(const NodePtr& listenerNode);
    
    /**
     * 生成事件委托代码
     * delegate -> 事件委托实现
     */
    std::string generateEventDelegate(const NodePtr& delegateNode);
    
    /**
     * 生成动画代码
     * animate -> requestAnimationFrame实现
     */
    std::string generateAnimation(const NodePtr& animationNode);
    
    /**
     * 生成虚拟对象代码
     * vir -> 编译期语法糖处理
     */
    std::string generateVirtualObject(const NodePtr& virtualNode);
    
    /**
     * 生成事件绑定代码
     * &-> -> addEventListener简化语法
     */
    std::string generateEventBinding(const NodePtr& bindingNode);

private:
    GenerateOptions m_options;              // 生成选项
    std::unordered_map<std::string, std::string> m_virtualObjectCache; // 虚拟对象缓存
    
    // ========== 辅助生成方法 ==========
    
    /**
     * 生成节点的JavaScript代码
     */
    std::string generateNodeJS(const NodePtr& node);
    
    /**
     * 处理增强选择器转换
     */
    std::string convertEnhancedSelector(const std::string& selector);
    
    /**
     * 处理CHTL JS箭头操作符转换
     * -> 等价于 .
     */
    std::string convertArrowOperator(const std::string& code);
    
    /**
     * 生成缩进
     */
    std::string generateIndentation(size_t level) const;
    
    /**
     * 优化生成的代码
     */
    std::string optimizeGeneratedCode(const std::string& code);
};

} // namespace CHTLJS