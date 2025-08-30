#pragma once
#include <string>
#include <vector>
#include <functional>

namespace CHTLJS {

/**
 * 原子参数类 - 严格按照原始API.md定义
 */
class AtomArg {
public:
    std::string value;
    std::string placeholder;
    
    AtomArg(const std::string& ph = "", const std::string& val = "") 
        : value(val), placeholder(ph) {}
    
    /**
     * 绑定获取值的函数
     */
    void bind(std::function<std::string(const std::string&)> func);
    
    /**
     * 填充参数值
     */
    void fillValue(const std::string& val);
    void fillValue(int val);
};

/**
 * 参数列表类 - 严格按照原始API.md定义
 */
class Arg {
public:
    std::vector<AtomArg> args;
    
    Arg() = default;
    Arg(const std::vector<std::string>& values);
    
    /**
     * 打印参数列表
     */
    void print() const;
    
    /**
     * 绑定参数
     */
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    
    /**
     * 填充值 - 用扫描结果填充
     */
    void fillValue(const Arg& result);
    
    /**
     * 转换输出
     */
    void transform(const std::string& pattern);
    
    /**
     * 索引访问
     */
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    
private:
    std::string m_transformPattern;
};

/**
 * 语法分析 - 严格按照原始API.md定义
 */
namespace Syntax {
    /**
     * 分析语法，返回Arg对象
     */
    Arg analyze(const std::string& syntax);
    
    /**
     * 判断是否是JS对象
     */
    bool isObject(const std::string& code);
    
    /**
     * 判断是否是JS函数
     */
    bool isFunction(const std::string& code);
    
    /**
     * 判断是否是JS数组
     */
    bool isArray(const std::string& code);
    
    /**
     * 判断是否是CHTL JS函数
     */
    bool isCHTLJSFunction(const std::string& code);
}

/**
 * CJMOD扫描器 - 严格按照原始API.md定义
 * 阻塞式扫描，等待获取代码片段后填充参数
 * 作为统一扫描器的辅助机制，提供前置截取和双指针扫描
 */
namespace CJMODScanner {
    /**
     * 扫描语法片段 - 阻塞过程
     * 等待扫描代码，获取实际的代码片段值
     */
    Arg scan(const Arg& args, const std::string& keyword);
    
    /**
     * 设置当前扫描的源代码
     */
    void setSourceCode(const std::string& sourceCode);
    
    /**
     * 前置截取机制
     * 处理 arg ** arg2 这样的语法，截取关键字前的片段
     */
    std::vector<std::string> prefixCut(const std::string& code, const std::string& keyword);
    
    /**
     * 双指针扫描机制
     * 两个指针同时位于0位置，预先扫描片段是否存在关键字
     * 滑动窗口算法同步向前，确保语法片段正确返回
     */
    bool dualPointerScan(const std::string& code, const std::string& keyword);
}

/**
 * CJMOD生成器 - 严格按照原始API.md定义
 */
namespace CJMODGenerator {
    /**
     * 导出最终的JS代码
     */
    std::string exportResult(const Arg& args);
}

/**
 * CHTL JS函数 - 严格按照原始API.md定义
 */
namespace CHTLJSFunction {
    /**
     * 创建CHTL JS函数
     * 封装了原始API构建语法的流程，能够快速构建CHTL JS函数
     * 这些CHTL JS函数天然支持虚对象vir以及无修饰字符串
     */
    void CreateCHTLJSFunction(const std::string& syntax);
    
    /**
     * 绑定虚拟对象
     * 对于不使用CreateCHTLJSFunction创建的，但符合CHTL JS函数语法
     * 可以使用bindVirtualObject手动绑定虚对象vir，获得虚对象的支持
     */
    void bindVirtualObject(const std::string& functionName);
}

} // namespace CHTLJS