#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <variant>

namespace Chtholly {
namespace CJMOD {

/**
 * INeverAway - 珂朵莉的标记函数系统
 * 严格按照语法文档实现：iNeverAway用于创建一组标记函数，允许开发者定义键而不是使用键，
 * 并可以使用状态区分同名的键，需要与虚对象共用
 */
class INeverAway {
public:
    /**
     * 函数状态类型
     */
    enum class FunctionState {
        A,              // 状态A
        B,              // 状态B  
        DEFAULT,        // 默认状态
        CUSTOM          // 自定义状态
    };
    
    /**
     * 参数类型
     */
    using ParamType = std::variant<int, double, std::string, bool>;
    using ParamList = std::vector<ParamType>;
    
    /**
     * 函数签名
     */
    struct FunctionSignature {
        std::string name;                           // 函数名
        FunctionState state;                        // 状态
        std::vector<std::string> param_types;       // 参数类型列表
        std::string return_type;                    // 返回类型
        std::function<std::string(const ParamList&)> handler;  // 处理函数
        
        FunctionSignature() : state(FunctionState::DEFAULT) {}
    };
    
    /**
     * 虚对象函数定义
     */
    struct VirtualObjectFunction {
        std::string key;                            // 键名
        FunctionState state;                        // 状态（用于同名重载）
        std::string function_body;                  // 函数体
        std::vector<std::string> param_types;       // 参数类型
        std::string return_type;                    // 返回类型
        bool is_function;                           // 是否为函数（否则为对象）
        
        VirtualObjectFunction() : state(FunctionState::DEFAULT), is_function(true) {}
    };
    
    /**
     * 虚对象定义
     */
    struct VirtualObject {
        std::string name;                                           // 虚对象名称
        std::unordered_map<std::string, VirtualObjectFunction> functions;  // 函数映射
        std::string generated_js_name;                             // 生成的JS名称
        
        VirtualObject() = default;
        VirtualObject(const std::string& obj_name) : name(obj_name) {}
    };

public:
    /**
     * 注册虚对象函数
     * @param vir_name 虚对象名称
     * @param key 函数键名
     * @param state 函数状态
     * @param param_types 参数类型列表
     * @param return_type 返回类型
     * @param function_body 函数体
     * @return 是否注册成功
     */
    static bool registerVirtualFunction(const std::string& vir_name,
                                      const std::string& key,
                                      FunctionState state,
                                      const std::vector<std::string>& param_types,
                                      const std::string& return_type,
                                      const std::string& function_body);
    
    /**
     * 注册虚对象属性
     * @param vir_name 虚对象名称
     * @param key 属性键名
     * @param object_body 对象体
     * @return 是否注册成功
     */
    static bool registerVirtualProperty(const std::string& vir_name,
                                      const std::string& key,
                                      const std::string& object_body);
    
    /**
     * 生成虚对象的JavaScript全局函数
     * @param vir_name 虚对象名称
     * @return 生成的JavaScript代码
     */
    static std::string generateGlobalFunctions(const std::string& vir_name);
    
    /**
     * 生成虚对象访问器代码
     * @param vir_name 虚对象名称
     * @param access_key 访问的键名
     * @param state 状态（可选）
     * @return JavaScript函数引用代码
     */
    static std::string generateAccessorCode(const std::string& vir_name,
                                          const std::string& access_key,
                                          FunctionState state = FunctionState::DEFAULT);
    
    /**
     * 检查虚对象是否存在
     * @param vir_name 虚对象名称
     * @return 是否存在
     */
    static bool hasVirtualObject(const std::string& vir_name);
    
    /**
     * 获取虚对象
     * @param vir_name 虚对象名称
     * @return 虚对象指针，不存在则返回nullptr
     */
    static std::shared_ptr<VirtualObject> getVirtualObject(const std::string& vir_name);
    
    /**
     * 删除虚对象
     * @param vir_name 虚对象名称
     * @return 是否删除成功
     */
    static bool removeVirtualObject(const std::string& vir_name);
    
    /**
     * 清空所有虚对象
     */
    static void clearAllVirtualObjects();
    
    /**
     * 获取所有虚对象名称
     * @return 虚对象名称列表
     */
    static std::vector<std::string> getAllVirtualObjectNames();

public:
    /**
     * 解析CHTL JS的iNeverAway定义
     * 支持语法：
     * vir Test = iNeverAway {
     *     Void<A>: function(int, int) { ... },
     *     Void<B>: function(int, int) { ... },
     *     Void: { ... },
     *     Ax: { ... }
     * };
     */
    static bool parseCHTLJSDefinition(const std::string& vir_name, const std::string& definition);
    
    /**
     * 解析函数状态标记（如 <A>, <B>）
     * @param key_with_state 带状态的键名（如 "Void<A>"）
     * @return [键名, 状态]
     */
    static std::pair<std::string, FunctionState> parseStateAnnotation(const std::string& key_with_state);
    
    /**
     * 解析函数签名
     * @param signature 函数签名字符串（如 "function(int, int)"）
     * @return [参数类型列表, 返回类型]
     */
    static std::pair<std::vector<std::string>, std::string> parseFunctionSignature(const std::string& signature);
    
    /**
     * 生成唯一的全局函数名
     * @param vir_name 虚对象名称
     * @param key 函数键名
     * @param state 函数状态
     * @return 唯一的全局函数名
     */
    static std::string generateGlobalFunctionName(const std::string& vir_name,
                                                 const std::string& key,
                                                 FunctionState state);

private:
    // 全局虚对象注册表
    static std::unordered_map<std::string, std::shared_ptr<VirtualObject>> virtual_objects_;
    
    // 全局函数名计数器（确保唯一性）
    static std::unordered_map<std::string, int> function_name_counters_;
    
    /**
     * 状态枚举转字符串
     * @param state 函数状态
     * @return 状态字符串
     */
    static std::string stateToString(FunctionState state);
    
    /**
     * 字符串转状态枚举
     * @param state_str 状态字符串
     * @return 函数状态
     */
    static FunctionState stringToState(const std::string& state_str);
    
    /**
     * 验证函数名称有效性
     * @param name 函数名称
     * @return 是否有效
     */
    static bool isValidFunctionName(const std::string& name);
    
    /**
     * 验证参数类型有效性
     * @param param_types 参数类型列表
     * @return 是否有效
     */
    static bool isValidParameterTypes(const std::vector<std::string>& param_types);
};

/**
 * INeverAwayJS - JavaScript接口封装
 * 用于CJMOD API的JavaScript绑定
 */
class INeverAwayJS {
public:
    /**
     * 创建CHTL JS函数绑定
     * @return JavaScript函数代码
     */
    static std::string createCHTLJSFunction();
    
    /**
     * 处理虚对象访问
     * @param vir_name 虚对象名称
     * @param access_expression 访问表达式（如 "Test->Void<A>()"）
     * @return 处理结果JavaScript代码
     */
    static std::string handleVirtualObjectAccess(const std::string& vir_name,
                                                const std::string& access_expression);
    
    /**
     * 解析访问表达式
     * @param expression 访问表达式
     * @return [键名, 状态, 参数列表, 是否为函数调用]
     */
    static std::tuple<std::string, INeverAway::FunctionState, std::vector<std::string>, bool> 
           parseAccessExpression(const std::string& expression);
    
    /**
     * 生成虚对象属性访问代码
     * @param vir_name 虚对象名称
     * @param key 属性键名
     * @return JavaScript属性访问代码
     */
    static std::string generatePropertyAccess(const std::string& vir_name, const std::string& key);
    
    /**
     * 生成虚对象函数调用代码
     * @param vir_name 虚对象名称
     * @param key 函数键名
     * @param state 函数状态
     * @param params 参数列表
     * @return JavaScript函数调用代码
     */
    static std::string generateFunctionCall(const std::string& vir_name,
                                          const std::string& key,
                                          INeverAway::FunctionState state,
                                          const std::vector<std::string>& params);
    
    /**
     * 验证虚对象访问的有效性
     * @param vir_name 虚对象名称
     * @param key 访问键名
     * @param state 状态
     * @return 是否有效
     */
    static bool validateVirtualAccess(const std::string& vir_name,
                                    const std::string& key,
                                    INeverAway::FunctionState state);

private:
    /**
     * 解析函数参数列表
     * @param params_str 参数字符串
     * @return 参数列表
     */
    static std::vector<std::string> parseParameterList(const std::string& params_str);
    
    /**
     * 清理和格式化JavaScript代码
     * @param code JavaScript代码
     * @return 格式化后的代码
     */
    static std::string formatJavaScriptCode(const std::string& code);
};

/**
 * VirtualObjectManager - 虚对象管理器
 * 统一管理所有iNeverAway创建的虚对象
 */
class VirtualObjectManager {
public:
    /**
     * 初始化虚对象管理器
     */
    static void initialize();
    
    /**
     * 关闭虚对象管理器
     */
    static void shutdown();
    
    /**
     * 编译时处理所有虚对象
     * @return 生成的JavaScript代码
     */
    static std::string compileAllVirtualObjects();
    
    /**
     * 获取虚对象统计信息
     * @return 统计信息字符串
     */
    static std::string getStatistics();
    
    /**
     * 验证所有虚对象的完整性
     * @return 验证结果
     */
    static bool validateAllVirtualObjects();
    
    /**
     * 导出虚对象定义到文件
     * @param file_path 文件路径
     * @return 是否成功
     */
    static bool exportToFile(const std::string& file_path);
    
    /**
     * 从文件导入虚对象定义
     * @param file_path 文件路径
     * @return 是否成功
     */
    static bool importFromFile(const std::string& file_path);

private:
    static bool initialized_;
    static std::string global_js_code_;
    
    /**
     * 生成全局JavaScript前缀代码
     * @return JavaScript代码
     */
    static std::string generateGlobalPrefix();
    
    /**
     * 生成全局JavaScript后缀代码
     * @return JavaScript代码
     */
    static std::string generateGlobalSuffix();
};

} // namespace CJMOD
} // namespace Chtholly