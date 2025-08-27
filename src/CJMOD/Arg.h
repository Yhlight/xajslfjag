#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// 前向声明
class AtomArg;

/**
 * PlaceholderType - 占位符类型
 */
enum class PlaceholderType {
    NORMAL,           // $ - 普通占位符
    OPTIONAL,         // $? - 可选占位符
    REQUIRED,         // $! - 必须占位符
    UNORDERED,        // $_ - 无序占位符
    VARIADIC,         // ... - 不定参数占位符
    OPTIONAL_UNORDERED,   // $?_ - 可选无序占位符
    REQUIRED_UNORDERED,   // $!_ - 必须无序占位符
    CUSTOM            // 自定义占位符
};

/**
 * ValueType - 参数值类型
 */
using ArgValue = std::variant<
    std::string,
    int,
    double,
    bool,
    std::vector<std::string>,
    std::unordered_map<std::string, std::string>
>;

/**
 * BindFunction - 绑定函数类型
 */
using BindFunction = std::function<std::string(const std::string&)>;

/**
 * AtomArg - 原子参数类
 * 代表单个参数或占位符
 */
class AtomArg {
public:
    AtomArg();
    AtomArg(const std::string& pattern);
    AtomArg(const std::string& pattern, PlaceholderType type);
    
    // 基本属性
    std::string pattern;           // 原始模式
    std::string value;             // 当前值
    PlaceholderType type;          // 占位符类型
    bool is_filled;                // 是否已填充
    bool is_bound;                 // 是否已绑定
    
    /**
     * 绑定获取值的函数
     * @param func 绑定函数
     */
    void bind(BindFunction func);
    
    /**
     * 填充参数值
     * @param new_value 新值
     */
    void fillValue(const std::string& new_value);
    void fillValue(int new_value);
    void fillValue(double new_value);
    void fillValue(bool new_value);
    void fillValue(const ArgValue& new_value);
    
    /**
     * 获取值
     * @return 当前值
     */
    std::string getValue() const;
    
    /**
     * 检查是否为占位符
     * @return 是否为占位符
     */
    bool isPlaceholder() const;
    
    /**
     * 检查是否为操作符
     * @return 是否为操作符
     */
    bool isOperator() const;
    
    /**
     * 检查是否为关键字
     * @return 是否为关键字
     */
    bool isKeyword() const;
    
    /**
     * 检查是否为字面量
     * @return 是否为字面量
     */
    bool isLiteral() const;
    
    /**
     * 检查是否为可选参数
     * @return 是否为可选参数
     */
    bool isOptional() const;
    
    /**
     * 检查是否为必须参数
     * @return 是否为必须参数
     */
    bool isRequired() const;
    
    /**
     * 检查是否为无序参数
     * @return 是否为无序参数
     */
    bool isUnordered() const;
    
    /**
     * 检查是否为不定参数
     * @return 是否为不定参数
     */
    bool isVariadic() const;
    
    /**
     * 验证值
     * @return 验证是否通过
     */
    bool validate() const;
    
    /**
     * 获取类型字符串
     * @return 类型描述
     */
    std::string getTypeString() const;
    
    /**
     * 重置状态
     */
    void reset();
    
    /**
     * 复制
     * @return 复制的AtomArg对象
     */
    AtomArg copy() const;

private:
    BindFunction bind_function;
    ArgValue stored_value;
    
    // 内部方法
    PlaceholderType parsePlaceholderType(const std::string& pattern) const;
    std::string valueToString(const ArgValue& value) const;
    bool isValidPattern(const std::string& pattern) const;
};

/**
 * Arg - 参数列表类
 * 包含多个AtomArg，提供参数的绑定、填充、转换等操作
 */
class Arg {
public:
    Arg();
    Arg(const std::vector<std::string>& patterns);
    Arg(const std::vector<AtomArg>& atoms);
    
    /**
     * 添加原子参数
     * @param atom 原子参数
     */
    void addAtom(const AtomArg& atom);
    
    /**
     * 添加原子参数（通过模式）
     * @param pattern 模式字符串
     */
    void addAtom(const std::string& pattern);
    
    /**
     * 绑定指定模式的获取值函数
     * @param pattern 模式
     * @param func 绑定函数
     */
    void bind(const std::string& pattern, BindFunction func);
    
    /**
     * 绑定指定索引的获取值函数
     * @param index 索引
     * @param func 绑定函数
     */
    void bind(size_t index, BindFunction func);
    
    /**
     * 填充参数列表的值
     * @param values 值列表
     */
    void fillValue(const std::vector<std::string>& values);
    void fillValue(const Arg& other);
    
    /**
     * 填充指定索引的值
     * @param index 索引
     * @param value 值
     */
    void fillValue(size_t index, const std::string& value);
    void fillValue(size_t index, const ArgValue& value);
    
    /**
     * 参数最终输出什么JS代码
     * @param js_code 输出的JavaScript代码
     */
    void transform(const std::string& js_code);
    
    /**
     * 获取转换后的代码
     * @return JavaScript代码
     */
    std::string getTransformedCode() const;
    
    /**
     * 打印参数列表
     */
    void print() const;
    
    /**
     * 获取大小
     * @return 参数数量
     */
    size_t size() const;
    
    /**
     * 检查是否为空
     * @return 是否为空
     */
    bool empty() const;
    
    /**
     * 清空参数列表
     */
    void clear();
    
    /**
     * 获取指定索引的原子参数
     * @param index 索引
     * @return 原子参数引用
     */
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    
    /**
     * 获取所有值
     * @return 值列表
     */
    std::vector<std::string> getValues() const;
    
    /**
     * 获取所有模式
     * @return 模式列表
     */
    std::vector<std::string> getPatterns() const;
    
    /**
     * 查找模式
     * @param pattern 模式
     * @return 索引，未找到返回-1
     */
    int findPattern(const std::string& pattern) const;
    
    /**
     * 验证所有参数
     * @return 验证是否通过
     */
    bool validate() const;
    
    /**
     * 获取验证错误
     * @return 错误列表
     */
    std::vector<std::string> getValidationErrors() const;
    
    /**
     * 检查是否所有必须参数都已填充
     * @return 是否完整
     */
    bool isComplete() const;
    
    /**
     * 获取未填充的必须参数
     * @return 未填充参数列表
     */
    std::vector<std::string> getMissingRequiredArgs() const;
    
    /**
     * 复制
     * @return 复制的Arg对象
     */
    Arg copy() const;
    
    /**
     * 合并其他Arg
     * @param other 其他Arg对象
     */
    void merge(const Arg& other);
    
    /**
     * 过滤指定类型的参数
     * @param type 占位符类型
     * @return 过滤后的Arg对象
     */
    Arg filter(PlaceholderType type) const;
    
    /**
     * 映射转换
     * @param func 转换函数
     * @return 转换后的Arg对象
     */
    Arg map(std::function<AtomArg(const AtomArg&)> func) const;
    
    /**
     * 生成调试信息
     * @return 调试信息字符串
     */
    std::string debug() const;

private:
    std::vector<AtomArg> atoms;
    std::string transformed_code;
    
    // 内部方法
    void updateTransformedCode();
    std::string interpolateCode(const std::string& template_code) const;
    std::string escapeJavaScript(const std::string& str) const;
};

/**
 * ArgBuilder - Arg构建器
 */
class ArgBuilder {
public:
    ArgBuilder();
    
    /**
     * 添加占位符
     * @param type 占位符类型
     * @return 构建器引用
     */
    ArgBuilder& placeholder(PlaceholderType type = PlaceholderType::NORMAL);
    
    /**
     * 添加操作符
     * @param op 操作符
     * @return 构建器引用
     */
    ArgBuilder& operator_(const std::string& op);
    
    /**
     * 添加关键字
     * @param keyword 关键字
     * @return 构建器引用
     */
    ArgBuilder& keyword(const std::string& keyword);
    
    /**
     * 添加字面量
     * @param literal 字面量
     * @return 构建器引用
     */
    ArgBuilder& literal(const std::string& literal);
    
    /**
     * 添加自定义模式
     * @param pattern 模式
     * @return 构建器引用
     */
    ArgBuilder& custom(const std::string& pattern);
    
    /**
     * 构建Arg对象
     * @return Arg对象
     */
    Arg build();

private:
    std::vector<AtomArg> atoms;
};

/**
 * ArgUtils - Arg工具类
 */
class ArgUtils {
public:
    /**
     * 从字符串解析Arg
     * @param pattern_str 模式字符串
     * @return Arg对象
     */
    static Arg parseFromString(const std::string& pattern_str);
    
    /**
     * 转换为字符串
     * @param arg Arg对象
     * @return 字符串表示
     */
    static std::string toString(const Arg& arg);
    
    /**
     * 创建空Arg
     * @return 空Arg对象
     */
    static Arg createEmpty();
    
    /**
     * 创建单个占位符Arg
     * @param type 占位符类型
     * @return Arg对象
     */
    static Arg createSinglePlaceholder(PlaceholderType type = PlaceholderType::NORMAL);
    
    /**
     * 验证模式字符串
     * @param pattern 模式字符串
     * @return 是否有效
     */
    static bool validatePattern(const std::string& pattern);
    
    /**
     * 解析占位符类型
     * @param placeholder 占位符字符串
     * @return 占位符类型
     */
    static PlaceholderType parsePlaceholderType(const std::string& placeholder);
    
    /**
     * 占位符类型转字符串
     * @param type 占位符类型
     * @return 字符串表示
     */
    static std::string placeholderTypeToString(PlaceholderType type);
    
    /**
     * 比较两个Arg对象
     * @param a Arg对象a
     * @param b Arg对象b
     * @return 是否相等
     */
    static bool compare(const Arg& a, const Arg& b);
    
    /**
     * 深度复制Arg对象
     * @param source 源Arg对象
     * @return 复制的Arg对象
     */
    static Arg deepCopy(const Arg& source);
};

// 操作符重载
std::ostream& operator<<(std::ostream& os, const AtomArg& atom);
std::ostream& operator<<(std::ostream& os, const Arg& arg);
std::ostream& operator<<(std::ostream& os, PlaceholderType type);

} // namespace CJMOD
} // namespace CHTL