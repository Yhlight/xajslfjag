#ifndef CJMOD_API_H
#define CJMOD_API_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

// CJMOD API版本
#define CJMOD_API_VERSION "1.0.0"

// 导出宏
#ifdef _WIN32
    #ifdef CJMOD_EXPORTS
        #define CJMOD_API __declspec(dllexport)
    #else
        #define CJMOD_API __declspec(dllimport)
    #endif
#else
    #define CJMOD_API __attribute__((visibility("default")))
#endif

namespace CHTL {
namespace CJMOD {

// 前向声明
class Arg;
class AtomArg;
class Syntax;
class CJMODScanner;
class CJMODGenerator;
class CHTLJSFunction;

// 原子参数类型
enum class AtomArgType {
    Placeholder,           // $
    OptionalPlaceholder,   // $?
    VoidPlaceholder,       // $!
    NegatePlaceholder,     // $^
    VarPlaceholder,        // $_
    SpecialPlaceholder,    // $#
    Literal,               // 字面量
    Keyword                // 关键字
};

// 原子参数
class CJMOD_API AtomArg {
public:
    AtomArg();
    AtomArg(const std::string& value, AtomArgType type = AtomArgType::Literal);
    AtomArg(const AtomArg& other);
    AtomArg(AtomArg&& other) noexcept;
    AtomArg& operator=(const AtomArg& other);
    AtomArg& operator=(AtomArg&& other) noexcept;
    ~AtomArg();
    
    // 获取值
    const std::string& getValue() const;
    void setValue(const std::string& value);
    
    // 获取类型
    AtomArgType getType() const;
    void setType(AtomArgType type);
    
    // 是否是占位符
    bool isPlaceholder() const;
    
    // 转换为字符串
    std::string toString() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// 参数列表
class CJMOD_API Arg {
public:
    Arg();
    Arg(const std::vector<std::string>& values);
    Arg(const std::vector<AtomArg>& atoms);
    Arg(Arg&& other) noexcept;
    Arg& operator=(Arg&& other) noexcept;
    ~Arg();
    
    // 打印参数列表
    void print() const;
    
    // 获取参数数量
    size_t size() const;
    
    // 访问参数
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    
    // 添加参数
    void push(const AtomArg& atom);
    void push(const std::string& value, AtomArgType type = AtomArgType::Literal);
    
    // 绑定获取值的函数
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    
    // 填充参数值
    void fillValue(const Arg& values);
    void fillValue(const std::vector<std::string>& values);
    
    // 转换输出
    void transform(const std::string& jsCode);
    
    // 获取转换结果
    const std::string& getTransformResult() const;
    
    // 迭代器支持
    std::vector<AtomArg>::iterator begin();
    std::vector<AtomArg>::iterator end();
    std::vector<AtomArg>::const_iterator begin() const;
    std::vector<AtomArg>::const_iterator end() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// 语法分析器
class CJMOD_API Syntax {
public:
    // 分析语法
    static Arg analyze(const std::string& syntax);
    
    // 判断是否是JS对象
    static bool isObject(const std::string& code);
    
    // 判断是否是JS函数
    static bool isFunction(const std::string& code);
    
    // 判断是否是JS数组
    static bool isArray(const std::string& code);
    
    // 判断是否是CHTL JS函数
    static bool isCHTLJSFunction(const std::string& code);
    
    // 获取函数名
    static std::string getFunctionName(const std::string& code);
    
    // 获取对象属性
    static std::vector<std::pair<std::string, std::string>> getObjectProperties(const std::string& code);
    
    // 获取数组元素
    static std::vector<std::string> getArrayElements(const std::string& code);
};

// 统一扫描器
class CJMOD_API CJMODScanner {
public:
    // 扫描语法片段
    static Arg scan(const Arg& pattern, const std::string& keyword = "");
    
    // 双指针扫描
    static Arg dualPointerScan(const std::string& code, const std::string& startPattern, const std::string& endPattern);
    
    // 预截断扫描
    static Arg preTruncateScan(const std::string& code, const std::string& delimiter);
    
    // 设置扫描上下文
    static void setContext(const std::string& code);
    
    // 获取当前位置
    static size_t getCurrentPosition();
    
    // 设置当前位置
    static void setCurrentPosition(size_t pos);
    
    // 重置扫描器
    static void reset();
};

// 生成器
class CJMOD_API CJMODGenerator {
public:
    // 导出结果
    static void exportResult(const Arg& args);
    
    // 导出JavaScript代码
    static void exportJavaScript(const std::string& jsCode);
    
    // 导出CSS代码
    static void exportCSS(const std::string& cssCode);
    
    // 导出HTML代码
    static void exportHTML(const std::string& htmlCode);
    
    // 设置输出模式
    static void setOutputMode(const std::string& mode);
    
    // 获取生成的代码
    static std::string getGeneratedCode();
    
    // 清空生成的代码
    static void clearGeneratedCode();
    
    // 添加源映射
    static void addSourceMapping(size_t srcLine, size_t srcCol, size_t dstLine, size_t dstCol);
};

// CHTL JS函数
class CJMOD_API CHTLJSFunction {
public:
    // 创建CHTL JS函数
    static void create(const std::string& name, const std::string& params, const std::string& body);
    
    // 绑定虚拟对象
    static void bindVirtualObject(const std::string& functionName);
    
    // 获取虚拟对象
    static std::string getVirtualObject(const std::string& functionName);
    
    // 检查函数是否存在
    static bool exists(const std::string& functionName);
    
    // 调用函数
    static std::string call(const std::string& functionName, const std::vector<std::string>& args);
    
    // 获取函数参数
    static std::vector<std::string> getParameters(const std::string& functionName);
    
    // 获取函数体
    static std::string getBody(const std::string& functionName);
};

// 扩展注册接口
class CJMOD_API Extension {
public:
    // 注册语法扩展
    static void registerSyntax(const std::string& pattern, std::function<void(const Arg&)> handler);
    
    // 注册选择器
    static void registerSelector(const std::string& selector, std::function<std::string(const std::string&)> handler);
    
    // 注册操作符
    static void registerOperator(const std::string& op, std::function<std::string(const std::string&, const std::string&)> handler);
    
    // 注册虚拟对象
    static void registerVirtualObject(const std::string& name, std::function<std::string()> getter);
    
    // 注册函数
    static void registerFunction(const std::string& name, std::function<std::string(const std::vector<std::string>&)> func);
    
    // 取消注册
    static void unregisterSyntax(const std::string& pattern);
    static void unregisterSelector(const std::string& selector);
    static void unregisterOperator(const std::string& op);
    static void unregisterVirtualObject(const std::string& name);
    static void unregisterFunction(const std::string& name);
};

// 工具函数
namespace Util {
    // 转义字符串
    CJMOD_API std::string escapeString(const std::string& str);
    
    // 反转义字符串
    CJMOD_API std::string unescapeString(const std::string& str);
    
    // 分割字符串
    CJMOD_API std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    
    // 连接字符串
    CJMOD_API std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
    
    // 去除空白
    CJMOD_API std::string trim(const std::string& str);
    
    // 替换字符串
    CJMOD_API std::string replace(const std::string& str, const std::string& from, const std::string& to);
}

} // namespace CJMOD
} // namespace CHTL

// C接口（用于动态库导出）
extern "C" {
    // 模块初始化
    CJMOD_API void CJMOD_Initialize();
    
    // 模块清理
    CJMOD_API void CJMOD_Cleanup();
    
    // 获取版本
    CJMOD_API const char* CJMOD_GetVersion();
    
    // 获取模块名称
    CJMOD_API const char* CJMOD_GetModuleName();
    
    // 获取模块描述
    CJMOD_API const char* CJMOD_GetModuleDescription();
}

#endif // CJMOD_API_H