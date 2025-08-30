#pragma once
#include "../Scanner/UnifiedScanner.h"
#include "../Error/ErrorReport.h"
#include <string>

namespace CHTL {

/**
 * JavaScript编译器
 * 基于ANTLR4实现，处理完整的合并后JavaScript代码
 * 严格按照目标规划.ini要求：JavaScript编译器(ANTLR)
 */
class JSCompiler {
public:
    JSCompiler();
    ~JSCompiler() = default;
    
    /**
     * 编译JavaScript代码
     * 接收来自CHTL JS编译器合并后的完整JS代码
     */
    std::string compile(const std::string& jsCode);
    
    /**
     * 验证JavaScript语法
     */
    bool validateJS(const std::string& jsCode);
    
    /**
     * 优化JavaScript代码
     */
    std::string optimizeJS(const std::string& jsCode);
    
    /**
     * 获取编译器信息
     */
    std::string getCompilerInfo() const;

private:
    bool m_initialized;
    
    /**
     * 初始化ANTLR4 JavaScript解析器
     */
    void initializeANTLR();
    
    /**
     * 使用ANTLR4解析JavaScript
     */
    std::string parseWithANTLR(const std::string& jsCode);
    
    /**
     * 处理JavaScript语法错误
     */
    void handleJSError(const std::string& error, size_t line, size_t column);
};

} // namespace CHTL