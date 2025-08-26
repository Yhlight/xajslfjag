#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "Arg.h"
#include "Syntax.h"

namespace CHTL {
namespace CJMOD {

/**
 * FunctionType - CHTL JS函数类型
 */
enum class FunctionType {
    LISTEN,           // listen函数
    ANIMATE,          // animate函数
    DELEGATE,         // delegate函数
    CUSTOM,           // 自定义函数
    VIRTUAL_OBJECT,   // 虚对象函数
    ENHANCED_SELECTOR // 增强选择器函数
};

/**
 * VirtualObjectInfo - 虚对象信息
 */
struct VirtualObjectInfo {
    std::string name;                                      // 虚对象名称
    std::string function_name;                             // 关联的函数名
    std::unordered_map<std::string, std::string> properties; // 属性映射
    std::vector<std::string> methods;                      // 方法列表
    bool is_bound;                                         // 是否已绑定
    
    VirtualObjectInfo() : is_bound(false) {}
};

/**
 * FunctionDefinition - 函数定义
 */
struct FunctionDefinition {
    std::string name;                                      // 函数名
    FunctionType type;                                     // 函数类型
    Arg pattern;                                           // 参数模式
    std::string template_code;                             // 模板代码
    std::unordered_map<std::string, std::string> defaults; // 默认值
    std::vector<std::string> required_params;             // 必须参数
    std::vector<std::string> optional_params;             // 可选参数
    bool supports_virtual_object;                         // 是否支持虚对象
    bool supports_unquoted_literals;                      // 是否支持无修饰字符串
    
    FunctionDefinition() 
        : type(FunctionType::CUSTOM)
        , supports_virtual_object(false)
        , supports_unquoted_literals(false) {}
};

/**
 * CHTLJSFunction - CHTL JS函数管理器
 * 负责创建和管理CHTL JS函数，包括虚对象支持
 */
class CHTLJSFunction {
public:
    /**
     * 创建CHTL JS函数
     * @param function_definition 函数定义字符串，如 "printMyLove {url: $!_, mode: $?_}"
     * @return 函数定义对象
     */
    static FunctionDefinition CreateCHTLJSFunction(const std::string& function_definition);
    
    /**
     * 绑定虚对象vir
     * @param function_name 函数名称
     * @return 是否绑定成功
     */
    static bool bindVirtualObject(const std::string& function_name);
    
    /**
     * 解除虚对象绑定
     * @param function_name 函数名称
     * @return 是否解除成功
     */
    static bool unbindVirtualObject(const std::string& function_name);
    
    /**
     * 检查函数是否支持虚对象
     * @param function_name 函数名称
     * @return 是否支持虚对象
     */
    static bool supportsVirtualObject(const std::string& function_name);
    
    /**
     * 获取虚对象信息
     * @param function_name 函数名称
     * @return 虚对象信息
     */
    static VirtualObjectInfo getVirtualObjectInfo(const std::string& function_name);
    
    /**
     * 注册自定义函数
     * @param definition 函数定义
     * @return 是否注册成功
     */
    static bool registerFunction(const FunctionDefinition& definition);
    
    /**
     * 注销函数
     * @param function_name 函数名称
     * @return 是否注销成功
     */
    static bool unregisterFunction(const std::string& function_name);
    
    /**
     * 检查函数是否已注册
     * @param function_name 函数名称
     * @return 是否已注册
     */
    static bool isFunctionRegistered(const std::string& function_name);
    
    /**
     * 获取函数定义
     * @param function_name 函数名称
     * @return 函数定义
     */
    static FunctionDefinition getFunctionDefinition(const std::string& function_name);
    
    /**
     * 获取所有注册的函数名
     * @return 函数名列表
     */
    static std::vector<std::string> getRegisteredFunctions();
    
    /**
     * 生成函数调用代码
     * @param function_name 函数名
     * @param args 参数
     * @return 生成的JavaScript代码
     */
    static std::string generateFunctionCall(const std::string& function_name, const Arg& args);
    
    /**
     * 生成虚对象声明代码
     * @param vir_name 虚对象名称
     * @param function_call 函数调用
     * @return 生成的JavaScript代码
     */
    static std::string generateVirtualObjectDeclaration(const std::string& vir_name, const std::string& function_call);
    
    /**
     * 生成虚对象访问代码
     * @param vir_name 虚对象名称
     * @param property 属性名
     * @return 生成的JavaScript代码
     */
    static std::string generateVirtualObjectAccess(const std::string& vir_name, const std::string& property);
    
    /**
     * 验证函数定义字符串
     * @param function_definition 函数定义字符串
     * @return 是否有效
     */
    static bool validateFunctionDefinition(const std::string& function_definition);
    
    /**
     * 解析函数定义字符串
     * @param function_definition 函数定义字符串
     * @return 解析结果
     */
    static FunctionDefinition parseFunctionDefinition(const std::string& function_definition);

private:
    // 静态数据成员
    static std::unordered_map<std::string, FunctionDefinition> registered_functions;
    static std::unordered_map<std::string, VirtualObjectInfo> virtual_objects;
    static std::unordered_set<std::string> bound_virtual_objects;
    
    // 初始化方法
    static void initializeBuiltinFunctions();
    static void initializeVirtualObjectSupport();
    
    // 解析方法
    static std::string extractFunctionName(const std::string& definition);
    static Arg extractParameterPattern(const std::string& definition);
    static std::unordered_map<std::string, std::string> extractDefaultValues(const std::string& definition);
    static FunctionType determineFunctionType(const std::string& function_name);
    
    // 代码生成方法
    static std::string generateListenFunction(const std::string& function_name, const Arg& args);
    static std::string generateAnimateFunction(const std::string& function_name, const Arg& args);
    static std::string generateDelegateFunction(const std::string& function_name, const Arg& args);
    static std::string generateCustomFunction(const std::string& function_name, const Arg& args);
    
    // 虚对象处理方法
    static VirtualObjectInfo createVirtualObjectInfo(const std::string& function_name, const FunctionDefinition& definition);
    static std::string generateVirtualObjectWrapper(const std::string& function_name, const FunctionDefinition& definition);
    static std::string generatePropertyAccessor(const std::string& property_name, const std::string& property_type);
    
    // 验证方法
    static bool validateParameterPattern(const Arg& pattern);
    static bool validateFunctionName(const std::string& name);
    static bool validateDefaultValues(const std::unordered_map<std::string, std::string>& defaults);
    
    // 工具方法
    static std::string sanitizeFunctionName(const std::string& name);
    static std::string generateUniqueVirtualObjectId(const std::string& base_name);
    static std::vector<std::string> extractRequiredParameters(const Arg& pattern);
    static std::vector<std::string> extractOptionalParameters(const Arg& pattern);
};

/**
 * VirtualObjectManager - 虚对象管理器
 */
class VirtualObjectManager {
public:
    /**
     * 创建虚对象
     * @param name 虚对象名称
     * @param function_name 关联函数名
     * @param properties 属性映射
     * @return 是否创建成功
     */
    static bool createVirtualObject(const std::string& name, const std::string& function_name, 
                                   const std::unordered_map<std::string, std::string>& properties);
    
    /**
     * 删除虚对象
     * @param name 虚对象名称
     * @return 是否删除成功
     */
    static bool deleteVirtualObject(const std::string& name);
    
    /**
     * 检查虚对象是否存在
     * @param name 虚对象名称
     * @return 是否存在
     */
    static bool hasVirtualObject(const std::string& name);
    
    /**
     * 获取虚对象信息
     * @param name 虚对象名称
     * @return 虚对象信息
     */
    static VirtualObjectInfo getVirtualObject(const std::string& name);
    
    /**
     * 更新虚对象属性
     * @param name 虚对象名称
     * @param property 属性名
     * @param value 属性值
     * @return 是否更新成功
     */
    static bool updateVirtualObjectProperty(const std::string& name, const std::string& property, const std::string& value);
    
    /**
     * 添加虚对象方法
     * @param name 虚对象名称
     * @param method 方法名
     * @return 是否添加成功
     */
    static bool addVirtualObjectMethod(const std::string& name, const std::string& method);
    
    /**
     * 获取所有虚对象名称
     * @return 虚对象名称列表
     */
    static std::vector<std::string> getAllVirtualObjects();
    
    /**
     * 清除所有虚对象
     */
    static void clearAllVirtualObjects();
    
    /**
     * 生成虚对象管理代码
     * @return JavaScript代码
     */
    static std::string generateManagementCode();

private:
    static std::unordered_map<std::string, VirtualObjectInfo> virtual_objects;
    
    static bool validateVirtualObjectName(const std::string& name);
    static std::string generateVirtualObjectCode(const VirtualObjectInfo& info);
};

/**
 * FunctionTemplateEngine - 函数模板引擎
 */
class FunctionTemplateEngine {
public:
    /**
     * 注册函数模板
     * @param function_type 函数类型
     * @param template_code 模板代码
     */
    static void registerTemplate(FunctionType function_type, const std::string& template_code);
    
    /**
     * 获取函数模板
     * @param function_type 函数类型
     * @return 模板代码
     */
    static std::string getTemplate(FunctionType function_type);
    
    /**
     * 渲染函数模板
     * @param function_type 函数类型
     * @param variables 变量映射
     * @return 渲染后的代码
     */
    static std::string renderTemplate(FunctionType function_type, const std::unordered_map<std::string, std::string>& variables);
    
    /**
     * 检查模板是否存在
     * @param function_type 函数类型
     * @return 是否存在
     */
    static bool hasTemplate(FunctionType function_type);

private:
    static std::unordered_map<FunctionType, std::string> templates;
    static void initializeDefaultTemplates();
};

/**
 * CHTLJSFunctionUtils - CHTL JS函数工具类
 */
class CHTLJSFunctionUtils {
public:
    /**
     * 转换函数类型为字符串
     * @param type 函数类型
     * @return 字符串表示
     */
    static std::string functionTypeToString(FunctionType type);
    
    /**
     * 从字符串解析函数类型
     * @param type_str 类型字符串
     * @return 函数类型
     */
    static FunctionType stringToFunctionType(const std::string& type_str);
    
    /**
     * 检查函数名是否为CHTL JS内置函数
     * @param function_name 函数名
     * @return 是否为内置函数
     */
    static bool isBuiltinFunction(const std::string& function_name);
    
    /**
     * 获取所有内置函数名
     * @return 内置函数名列表
     */
    static std::vector<std::string> getBuiltinFunctionNames();
    
    /**
     * 验证参数是否符合函数要求
     * @param function_name 函数名
     * @param args 参数
     * @return 是否符合要求
     */
    static bool validateFunctionArguments(const std::string& function_name, const Arg& args);
    
    /**
     * 获取函数参数建议
     * @param function_name 函数名
     * @return 参数建议列表
     */
    static std::vector<std::string> getFunctionArgumentSuggestions(const std::string& function_name);
    
    /**
     * 格式化函数定义
     * @param definition 函数定义
     * @return 格式化后的定义字符串
     */
    static std::string formatFunctionDefinition(const FunctionDefinition& definition);
    
    /**
     * 比较两个函数定义
     * @param a 函数定义a
     * @param b 函数定义b
     * @return 是否相等
     */
    static bool compareFunctionDefinitions(const FunctionDefinition& a, const FunctionDefinition& b);
    
    /**
     * 复制函数定义
     * @param source 源定义
     * @return 复制的定义
     */
    static FunctionDefinition copyFunctionDefinition(const FunctionDefinition& source);
};

/**
 * CHTLJSEnhancedFeatures - CHTL JS增强特性管理器
 */
class CHTLJSEnhancedFeatures {
public:
    /**
     * 启用无修饰字符串支持
     * @param function_name 函数名
     */
    static void enableUnquotedLiterals(const std::string& function_name);
    
    /**
     * 禁用无修饰字符串支持
     * @param function_name 函数名
     */
    static void disableUnquotedLiterals(const std::string& function_name);
    
    /**
     * 检查函数是否支持无修饰字符串
     * @param function_name 函数名
     * @return 是否支持
     */
    static bool supportsUnquotedLiterals(const std::string& function_name);
    
    /**
     * 启用增强选择器支持
     * @param function_name 函数名
     */
    static void enableEnhancedSelectors(const std::string& function_name);
    
    /**
     * 处理增强选择器
     * @param selector 选择器字符串
     * @return 处理后的选择器
     */
    static std::string processEnhancedSelector(const std::string& selector);
    
    /**
     * 处理无修饰字符串
     * @param literal 字面量
     * @return 处理后的字符串
     */
    static std::string processUnquotedLiteral(const std::string& literal);
    
    /**
     * 生成增强特性支持代码
     * @return JavaScript代码
     */
    static std::string generateEnhancedFeaturesCode();

private:
    static std::unordered_set<std::string> unquoted_literal_enabled_functions;
    static std::unordered_set<std::string> enhanced_selector_enabled_functions;
    
    static bool isValidUnquotedLiteral(const std::string& literal);
    static bool isValidEnhancedSelector(const std::string& selector);
};

} // namespace CJMOD
} // namespace CHTL