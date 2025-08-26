#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>

namespace CHTL {
namespace CJMOD {

/**
 * CJMOD API - 简单强大的设计
 * 按照记忆中的简单API重新实现
 */

// ============================================================================
// Arg - 参数列表类
// ============================================================================

class Arg {
private:
    std::vector<std::string> tokens_;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> bindings_;
    std::string transform_template_;
    
public:
    /**
     * 构造函数
     */
    Arg() = default;
    
    /**
     * 构造函数（从字符串列表）
     * @param tokens Token列表
     */
    Arg(const std::vector<std::string>& tokens) : tokens_(tokens) {}
    
    /**
     * 打印参数列表
     * 输出格式：["$", "**", "$"]
     */
    void print() const {
        std::cout << "[";
        for (size_t i = 0; i < tokens_.size(); ++i) {
            std::cout << "\"" << tokens_[i] << "\"";
            if (i < tokens_.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }
    
    /**
     * 绑定占位符处理函数
     * @param placeholder 占位符（如 "$", "**"）
     * @param func 处理函数
     */
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
        bindings_[placeholder] = func;
    }
    
    /**
     * 填充值
     * @param result 结果Arg对象
     */
    void fillValue(const Arg& result) {
        if (result.tokens_.size() == tokens_.size()) {
            tokens_ = result.tokens_;
        }
    }
    
    /**
     * 设置转换模板
     * @param template_str 模板字符串
     */
    void transform(const std::string& template_str) {
        transform_template_ = template_str;
    }
    
    /**
     * 获取指定索引的值
     * @param index 索引
     * @return 值
     */
    std::string operator[](size_t index) const {
        if (index < tokens_.size()) {
            return tokens_[index];
        }
        return "";
    }
    
    /**
     * 访问器（非const版本）
     */
    struct ValueAccessor {
        std::string value;
        ValueAccessor(const std::string& v) : value(v) {}
    };
    
    ValueAccessor operator[](size_t index) {
        if (index < tokens_.size()) {
            return ValueAccessor(tokens_[index]);
        }
        return ValueAccessor("");
    }
    
    /**
     * 获取Token列表
     * @return Token列表
     */
    const std::vector<std::string>& getTokens() const { return tokens_; }
    
    /**
     * 设置Token列表
     * @param tokens Token列表
     */
    void setTokens(const std::vector<std::string>& tokens) { tokens_ = tokens; }
    
    /**
     * 获取转换模板
     * @return 转换模板
     */
    const std::string& getTransformTemplate() const { return transform_template_; }
    
    /**
     * 应用绑定函数处理值
     * @param placeholder 占位符
     * @param value 输入值
     * @return 处理后的值
     */
    std::string applyBinding(const std::string& placeholder, const std::string& value) {
        auto it = bindings_.find(placeholder);
        if (it != bindings_.end()) {
            return it->second(value);
        }
        return value;
    }
};

// ============================================================================
// Syntax - 语法分析类
// ============================================================================

class Syntax {
public:
    /**
     * 分析语法，返回一个Arg对象
     * @param pattern 语法模式字符串，如 "$ ** $"
     * @return 解析出的Arg对象，包含参数列表
     */
    static Arg analyze(const std::string& pattern) {
        std::vector<std::string> tokens;
        std::string current_token;
        
        for (char c : pattern) {
            if (c == ' ') {
                if (!current_token.empty()) {
                    tokens.push_back(current_token);
                    current_token.clear();
                }
            } else {
                current_token += c;
            }
        }
        
        if (!current_token.empty()) {
            tokens.push_back(current_token);
        }
        
        return Arg(tokens);
    }
    
    /**
     * 判断是否是JavaScript对象
     * @param code JavaScript代码
     * @return 是否为对象
     */
    static bool isObject(const std::string& code) {
        std::string trimmed = code;
        // 简单的对象检测
        return !trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}';
    }
    
    /**
     * 判断是否是JavaScript函数
     * @param code JavaScript代码
     * @return 是否为函数
     */
    static bool isFunction(const std::string& code) {
        return code.find("function") != std::string::npos || 
               code.find("=>") != std::string::npos;
    }
    
    /**
     * 判断是否是JavaScript数组
     * @param code JavaScript代码
     * @return 是否为数组
     */
    static bool isArray(const std::string& code) {
        std::string trimmed = code;
        return !trimmed.empty() && trimmed.front() == '[' && trimmed.back() == ']';
    }
    
    /**
     * 判断是否是CHTL JS函数
     * @param code CHTL JS代码
     * @return 是否为CHTL JS函数
     */
    static bool isCHTLJSFunction(const std::string& code) {
        // 检测新语法：functionName { key: value }
        return code.find('{') != std::string::npos && 
               code.find(':') != std::string::npos &&
               code.find(';') != std::string::npos;
    }
};

// ============================================================================
// CJMODScanner - 统一扫描器用于CJMOD API的接口
// ============================================================================

class CJMODScanner {
public:
    /**
     * 扫描语法片段
     * @param args 参数模式
     * @param keyword 扫描的关键字
     * @return 扫描结果Arg对象
     */
    static Arg scan(const Arg& args, const std::string& keyword = "") {
        // 简化实现：模拟扫描过程
        // 在实际使用中，这里会调用真正的扫描器
        
        // 示例：如果扫描 "**" 关键字，返回具体的值
        if (keyword == "**") {
            return Arg({"3", "**", "4"});
        }
        
        // 默认返回原参数
        return args;
    }
    
    /**
     * 扫描代码片段并提取值
     * @param code 代码片段
     * @param pattern 模式
     * @return 扫描结果
     */
    static Arg scanCode(const std::string& code, const std::string& pattern) {
        // 简化实现：基于模式提取值
        Arg result;
        
        // 这里会实现真正的代码扫描逻辑
        // 现在返回示例结果
        result.setTokens({"value1", "operator", "value2"});
        
        return result;
    }
};

// ============================================================================
// CJMODGenerator - 生成器用于CJMOD API的接口
// ============================================================================

class CJMODGenerator {
public:
    /**
     * 导出最终的JavaScript代码
     * @param args 参数对象
     * @return 生成的JavaScript代码
     */
    static std::string exportResult(const Arg& args) {
        const auto& transform_template = args.getTransformTemplate();
        
        if (!transform_template.empty()) {
            // 使用转换模板生成代码
            return transform_template;
        }
        
        // 默认：将Token连接成代码
        std::string result;
        const auto& tokens = args.getTokens();
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            result += tokens[i];
            if (i < tokens.size() - 1) {
                result += " ";
            }
        }
        
        return result;
    }
    
    /**
     * 生成HTML script标签
     * @param js_code JavaScript代码
     * @return HTML代码
     */
    static std::string wrapInScriptTag(const std::string& js_code) {
        return "<script>\n    " + js_code + "\n</script>";
    }
};

// ============================================================================
// CHTLJSFunction - CHTL JS函数，用于CJMOD API的接口
// ============================================================================

class CHTLJSFunction {
private:
    static std::unordered_map<std::string, bool> virtual_object_bindings_;
    
public:
    /**
     * 创建CHTL JS函数
     * 封装了原始API构建语法的流程，能够快速构建CHTL JS函数
     * @param function_definition 函数定义，如 "printMyLove {url: $!_, mode: $?_}"
     * @return 函数是否创建成功
     */
    static bool CreateCHTLJSFunction(const std::string& function_definition) {
        // 解析函数定义
        size_t space_pos = function_definition.find(' ');
        if (space_pos != std::string::npos) {
            std::string function_name = function_definition.substr(0, space_pos);
            
            // 自动绑定虚对象支持
            virtual_object_bindings_[function_name] = true;
            
            return true;
        }
        
        return false;
    }
    
    /**
     * 绑定虚对象vir
     * 对于不使用CreateCHTLJSFunction创建的，但是符合CHTL JS函数的语法
     * 可以使用bindVirtualObject手动绑定虚对象vir，获得虚对象的支持
     * @param function_name 函数名称
     * @return 是否绑定成功
     */
    static bool bindVirtualObject(const std::string& function_name) {
        virtual_object_bindings_[function_name] = true;
        return true;
    }
    
    /**
     * 检查函数是否支持虚对象
     * @param function_name 函数名称
     * @return 是否支持虚对象
     */
    static bool supportsVirtualObject(const std::string& function_name) {
        auto it = virtual_object_bindings_.find(function_name);
        return it != virtual_object_bindings_.end() && it->second;
    }
};

// 静态成员定义
std::unordered_map<std::string, bool> CHTLJSFunction::virtual_object_bindings_;

} // namespace CJMOD
} // namespace CHTL