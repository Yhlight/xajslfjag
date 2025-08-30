#pragma once
#include "../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <regex>

namespace CHTL {

/**
 * CHTL JS语法分析器
 */
class CHTLJSSyntaxAnalyzer {
public:
    struct ParsedFunction {
        std::string name;                           // 函数名
        std::string type;                           // 函数类型 (listen, delegate, animate等)
        std::unordered_map<std::string, std::string> parameters; // 参数映射
        std::string body;                           // 函数体
        bool isVirtual;                             // 是否为虚拟对象
    };
    
    struct EnhancedSelector {
        std::string original;                       // 原始选择器
        std::string type;                           // 选择器类型 (class, id, tag, complex)
        std::string selector;                       // 解析后的选择器
        std::string jsExpression;                   // 对应的JS表达式
        int index;                                  // 索引 (如果有)
    };
    
    /**
     * 解析CHTL JS函数
     */
    ParsedFunction parseFunction(const std::string& content);
    
    /**
     * 解析增强选择器
     */
    std::vector<EnhancedSelector> parseSelectors(const std::string& content);
    
    /**
     * 解析事件绑定操作符
     */
    bool parseEventBinding(const std::string& content, std::string& element, 
                          std::string& event, std::string& handler);
    
    /**
     * 检查是否为虚拟对象声明
     */
    bool isVirtualObjectDeclaration(const std::string& content);
    
    /**
     * 解析虚拟对象
     */
    ParsedFunction parseVirtualObject(const std::string& content);
};

/**
 * CHTL JS代码生成器
 */
class CHTLJSCodeGenerator {
public:
    /**
     * 生成事件监听器代码
     */
    std::string generateEventListener(const CHTLJSSyntaxAnalyzer::ParsedFunction& func);
    
    /**
     * 生成事件委托代码
     */
    std::string generateEventDelegate(const CHTLJSSyntaxAnalyzer::ParsedFunction& func);
    
    /**
     * 生成动画代码
     */
    std::string generateAnimation(const CHTLJSSyntaxAnalyzer::ParsedFunction& func);
    
    /**
     * 生成虚拟对象代码
     */
    std::string generateVirtualObject(const CHTLJSSyntaxAnalyzer::ParsedFunction& func);
    
    /**
     * 转换增强选择器
     */
    std::string transformSelector(const CHTLJSSyntaxAnalyzer::EnhancedSelector& selector);
    
    /**
     * 生成事件绑定代码
     */
    std::string generateEventBinding(const std::string& element, 
                                   const std::string& event, 
                                   const std::string& handler);
    
    /**
     * 生成模块导入代码
     */
    std::string generateModuleImport(const std::string& moduleSpec);
    
    /**
     * 优化生成的代码
     */
    std::string optimizeGeneratedCode(const std::string& code);
};

/**
 * 虚拟对象管理器
 */
class VirtualObjectManager {
public:
    struct VirtualObject {
        std::string name;                           // 对象名称
        std::string type;                           // 对象类型
        std::unordered_map<std::string, std::string> methods; // 方法映射
        std::unordered_map<std::string, std::string> properties; // 属性映射
    };
    
    /**
     * 注册虚拟对象
     */
    void registerVirtualObject(const std::string& name, const VirtualObject& obj);
    
    /**
     * 获取虚拟对象
     */
    VirtualObject* getVirtualObject(const std::string& name);
    
    /**
     * 解析虚拟对象调用
     */
    std::string resolveVirtualCall(const std::string& call);
    
    /**
     * 生成虚拟对象实现代码
     */
    std::string generateImplementation(const VirtualObject& obj);
    
private:
    std::unordered_map<std::string, VirtualObject> m_virtualObjects;
};

/**
 * CHTL JS增强编译器
 */
class CHTLJSEnhancedCompiler {
public:
    CHTLJSEnhancedCompiler();
    ~CHTLJSEnhancedCompiler() = default;
    
    /**
     * 编译CHTL JS代码
     */
    std::string compile(const std::string& content);
    
    /**
     * 编译单个CHTL JS函数
     */
    std::string compileFunction(const std::string& content);
    
    /**
     * 编译选择器表达式
     */
    std::string compileSelectors(const std::string& content);
    
    /**
     * 编译事件绑定
     */
    std::string compileEventBindings(const std::string& content);
    
    /**
     * 编译虚拟对象
     */
    std::string compileVirtualObjects(const std::string& content);
    
    /**
     * 编译模块导入
     */
    std::string compileModuleImports(const std::string& content);
    
    /**
     * 设置调试模式
     */
    void setDebugMode(bool enabled);
    
    /**
     * 获取编译统计
     */
    std::string getCompilationStats() const;
    
    /**
     * 清理编译状态
     */
    void cleanup();

private:
    std::unique_ptr<CHTLJSSyntaxAnalyzer> m_analyzer;
    std::unique_ptr<CHTLJSCodeGenerator> m_generator;
    std::unique_ptr<VirtualObjectManager> m_virtualManager;
    
    bool m_debugMode;
    std::unordered_map<std::string, int> m_stats;
    
    /**
     * 预处理代码
     */
    std::string preprocessCode(const std::string& content);
    
    /**
     * 后处理代码
     */
    std::string postprocessCode(const std::string& code);
    
    /**
     * 更新统计信息
     */
    void updateStats(const std::string& category);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
};

} // namespace CHTL