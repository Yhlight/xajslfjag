#pragma once
#include "../Scanner/UnifiedScanner.h"
#include "../Error/ErrorReport.h"
#include <string>

namespace CHTL {

/**
 * CSS编译器
 * 基于ANTLR4实现，处理完整的合并后CSS代码
 * 严格按照目标规划.ini要求：CSS编译器(ANTLR)
 */
class CSSCompiler {
public:
    CSSCompiler();
    ~CSSCompiler() = default;
    
    /**
     * 编译CSS代码
     * 接收来自CHTL编译器合并后的完整CSS代码
     */
    std::string compile(const std::string& cssCode);
    
    /**
     * 验证CSS语法
     */
    bool validateCSS(const std::string& cssCode);
    
    /**
     * 优化CSS代码
     */
    std::string optimizeCSS(const std::string& cssCode);
    
    /**
     * 获取编译器信息
     */
    std::string getCompilerInfo() const;

private:
    bool m_initialized;
    
    /**
     * 初始化ANTLR4 CSS解析器
     */
    void initializeANTLR();
    
    /**
     * 使用ANTLR4解析CSS
     */
    std::string parseWithANTLR(const std::string& cssCode);
    
    /**
     * 处理CSS语法错误
     */
    void handleCSSError(const std::string& error, size_t line, size_t column);
};

} // namespace CHTL