#pragma once
#include "../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <variant>

namespace CHTL {

// 前向声明
class CHTLContext;
class VarGroup;

/**
 * CJMOD参数类型
 */
using CJMODValue = std::variant<std::string, int, double, bool>;

/**
 * CJMOD语法分析结果
 */
struct SyntaxAnalysisResult {
    std::string type;                               // 语法类型
    std::unordered_map<std::string, CJMODValue> properties; // 属性
    std::vector<std::string> dependencies;          // 依赖
    bool isValid;                                   // 是否有效
    std::string errorMessage;                       // 错误信息
    
    SyntaxAnalysisResult() : isValid(false) {}
};

/**
 * CJMOD参数处理器
 */
class Arg {
public:
    Arg(const std::string& name = "", const CJMODValue& defaultValue = std::string(""));
    
    /**
     * 绑定参数值
     */
    Arg& bind(const CJMODValue& value);
    
    /**
     * 填充默认值
     */
    Arg& fillValue(const CJMODValue& value);
    
    /**
     * 转换参数类型
     */
    template<typename T>
    Arg& transform(std::function<T(const CJMODValue&)> transformer);
    
    /**
     * 获取字符串值
     */
    std::string asString() const;
    
    /**
     * 获取整数值
     */
    int asInt() const;
    
    /**
     * 获取浮点数值
     */
    double asDouble() const;
    
    /**
     * 获取布尔值
     */
    bool asBool() const;
    
    /**
     * 检查是否有值
     */
    bool hasValue() const;
    
    /**
     * 获取参数名
     */
    const std::string& getName() const { return m_name; }
    
    /**
     * 验证参数
     */
    bool validate() const;

private:
    std::string m_name;
    CJMODValue m_value;
    CJMODValue m_defaultValue;
    bool m_hasValue;
    std::function<CJMODValue(const CJMODValue&)> m_transformer;
};

/**
 * AtomArg占位符处理器
 */
class AtomArg {
public:
    /**
     * 处理基本占位符 $
     */
    static std::string processBasicPlaceholder(const std::string& template_, const std::vector<CJMODValue>& args);
    
    /**
     * 处理可选占位符 $?
     */
    static std::string processOptionalPlaceholder(const std::string& template_, const std::vector<CJMODValue>& args);
    
    /**
     * 处理必需占位符 $!
     */
    static std::string processRequiredPlaceholder(const std::string& template_, const std::vector<CJMODValue>& args);
    
    /**
     * 处理默认占位符 $_
     */
    static std::string processDefaultPlaceholder(const std::string& template_, const CJMODValue& defaultValue);
    
    /**
     * 处理可变参数占位符 ...
     */
    static std::string processVariadicPlaceholder(const std::string& template_, const std::vector<CJMODValue>& args, size_t startIndex);
    
    /**
     * 解析并处理所有占位符
     */
    static std::string processAllPlaceholders(const std::string& template_, const std::vector<CJMODValue>& args);

private:
    /**
     * 查找占位符位置
     */
    static std::vector<std::pair<size_t, std::string>> findPlaceholders(const std::string& template_);
    
    /**
     * 替换占位符
     */
    static std::string replacePlaceholder(const std::string& template_, const std::string& placeholder, const std::string& value);
};

/**
 * CJMOD语法分析器
 */
namespace Syntax {
    /**
     * 分析CJMOD语法
     */
    SyntaxAnalysisResult analyze(const std::string& code);
    
    /**
     * 分析函数声明
     */
    SyntaxAnalysisResult analyzeFunction(const std::string& functionCode);
    
    /**
     * 分析变量声明
     */
    SyntaxAnalysisResult analyzeVariable(const std::string& variableCode);
    
    /**
     * 分析模块导入
     */
    SyntaxAnalysisResult analyzeImport(const std::string& importCode);
    
    /**
     * 分析类型定义
     */
    SyntaxAnalysisResult analyzeType(const std::string& typeCode);
    
    /**
     * 验证语法完整性
     */
    bool validateSyntax(const std::string& code, std::vector<std::string>& errors);
}

/**
 * CJMOD扫描器
 */
class CJMODScanner {
public:
    CJMODScanner();
    ~CJMODScanner() = default;
    
    /**
     * 扫描CJMOD代码
     */
    bool scan(const std::string& code);
    
    /**
     * 获取扫描到的函数
     */
    std::vector<std::string> getFunctions() const;
    
    /**
     * 获取扫描到的变量
     */
    std::vector<std::string> getVariables() const;
    
    /**
     * 获取扫描到的类型
     */
    std::vector<std::string> getTypes() const;
    
    /**
     * 获取扫描到的导入
     */
    std::vector<std::string> getImports() const;
    
    /**
     * 获取扫描统计
     */
    std::string getScanStats() const;
    
    /**
     * 清理扫描状态
     */
    void cleanup();

private:
    std::vector<std::string> m_functions;
    std::vector<std::string> m_variables;
    std::vector<std::string> m_types;
    std::vector<std::string> m_imports;
    std::unordered_map<std::string, int> m_stats;
    
    /**
     * 扫描函数定义
     */
    void scanFunctions(const std::string& code);
    
    /**
     * 扫描变量定义
     */
    void scanVariables(const std::string& code);
    
    /**
     * 扫描类型定义
     */
    void scanTypes(const std::string& code);
    
    /**
     * 扫描导入语句
     */
    void scanImports(const std::string& code);
    
    /**
     * 更新统计信息
     */
    void updateStats(const std::string& category);
};

/**
 * CJMOD代码生成器
 */
class CJMODGenerator {
public:
    CJMODGenerator();
    ~CJMODGenerator() = default;
    
    /**
     * 导出生成结果
     */
    std::string exportResult(const SyntaxAnalysisResult& analysis);
    
    /**
     * 生成函数绑定代码
     */
    std::string generateFunctionBinding(const std::string& functionName, const std::vector<Arg>& args);
    
    /**
     * 生成变量绑定代码
     */
    std::string generateVariableBinding(const std::string& variableName, const CJMODValue& value);
    
    /**
     * 生成类型定义代码
     */
    std::string generateTypeDefinition(const std::string& typeName, const std::unordered_map<std::string, std::string>& properties);
    
    /**
     * 生成模块导出代码
     */
    std::string generateModuleExport(const std::vector<std::string>& exports);
    
    /**
     * 优化生成的代码
     */
    std::string optimizeCode(const std::string& code);
    
    /**
     * 设置生成选项
     */
    void setGenerationOptions(const std::unordered_map<std::string, CJMODValue>& options);

private:
    std::unordered_map<std::string, CJMODValue> m_options;
    
    /**
     * 格式化代码
     */
    std::string formatCode(const std::string& code);
    
    /**
     * 添加头部注释
     */
    std::string addHeader(const std::string& code);
    
    /**
     * 处理代码模板
     */
    std::string processTemplate(const std::string& template_, const std::unordered_map<std::string, std::string>& variables);
};

/**
 * CHTL JS函数创建器
 */
namespace CHTLJSFunction {
    /**
     * 创建CHTL JS函数
     */
    std::string CreateCHTLJSFunction(const std::string& functionName, 
                                   const std::vector<Arg>& parameters,
                                   const std::string& body);
    
    /**
     * 创建事件监听器函数
     */
    std::string CreateEventListener(const std::string& eventName, 
                                  const std::string& selector,
                                  const std::string& handler);
    
    /**
     * 创建事件委托函数
     */
    std::string CreateEventDelegate(const std::string& parentSelector,
                                  const std::string& childSelector,
                                  const std::string& eventName,
                                  const std::string& handler);
    
    /**
     * 创建动画函数
     */
    std::string CreateAnimation(const std::string& targetSelector,
                              const std::string& animationProperties,
                              int duration = 300);
    
    /**
     * 创建虚拟对象函数
     */
    std::string CreateVirtualObject(const std::string& objectName,
                                  const std::unordered_map<std::string, std::string>& methods);
}

/**
 * 虚拟对象绑定器
 */
class VirtualObjectBinder {
public:
    /**
     * 绑定虚拟对象
     */
    bool bindVirtualObject(const std::string& objectName, 
                          const std::unordered_map<std::string, std::function<CJMODValue(const std::vector<CJMODValue>&)>>& methods);
    
    /**
     * 调用虚拟对象方法
     */
    CJMODValue callVirtualMethod(const std::string& objectName, 
                               const std::string& methodName,
                               const std::vector<CJMODValue>& args);
    
    /**
     * 检查虚拟对象是否存在
     */
    bool hasVirtualObject(const std::string& objectName) const;
    
    /**
     * 获取虚拟对象方法列表
     */
    std::vector<std::string> getVirtualObjectMethods(const std::string& objectName) const;
    
    /**
     * 生成虚拟对象JavaScript代码
     */
    std::string generateVirtualObjectJS(const std::string& objectName);
    
    /**
     * 清理虚拟对象
     */
    void cleanup();

private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::function<CJMODValue(const std::vector<CJMODValue>&)>>> m_virtualObjects;
    
    /**
     * 验证方法调用
     */
    bool validateMethodCall(const std::string& objectName, const std::string& methodName) const;
};

/**
 * CJMOD API管理器
 * 统一管理所有CJMOD API功能
 */
class CJMODAPIManager {
public:
    CJMODAPIManager();
    ~CJMODAPIManager() = default;
    
    /**
     * 初始化API系统
     */
    bool initialize();
    
    /**
     * 分析CJMOD代码
     */
    SyntaxAnalysisResult analyzeSyntax(const std::string& code);
    
    /**
     * 扫描CJMOD代码
     */
    bool scanCode(const std::string& code);
    
    /**
     * 生成绑定代码
     */
    std::string generateBindings(const SyntaxAnalysisResult& analysis);
    
    /**
     * 处理参数
     */
    std::vector<Arg> processArguments(const std::vector<std::string>& argNames, 
                                    const std::vector<CJMODValue>& values);
    
    /**
     * 绑定虚拟对象
     */
    bool bindVirtualObject(const std::string& name, 
                          const std::unordered_map<std::string, std::function<CJMODValue(const std::vector<CJMODValue>&)>>& methods);
    
    /**
     * 创建CHTL JS函数
     */
    std::string createCHTLJSFunction(const std::string& type, 
                                   const std::unordered_map<std::string, std::string>& parameters);
    
    /**
     * 获取API统计
     */
    std::string getAPIStats() const;
    
    /**
     * 清理API状态
     */
    void cleanup();
    
    /**
     * 设置调试模式
     */
    void setDebugMode(bool enabled);

private:
    std::unique_ptr<CJMODScanner> m_scanner;
    std::unique_ptr<CJMODGenerator> m_generator;
    std::unique_ptr<VirtualObjectBinder> m_virtualBinder;
    
    bool m_initialized;
    bool m_debugMode;
    std::unordered_map<std::string, int> m_stats;
    
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