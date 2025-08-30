#pragma once

#include <string>
#include <regex>

namespace CHTL {

class CSSCompiler {
public:
    CSSCompiler();
    ~CSSCompiler();
    
    // 主编译接口
    std::string compile(const std::string& cssCode);
    
    // 编译选项
    void enableDebugMode(bool enable) { m_debugMode = enable; }
    void enableMinification(bool enable) { m_enableMinification = enable; }
    
    // CSS处理
    std::string removeExcessWhitespace(const std::string& css);
    std::string formatCSS(const std::string& css);
    std::string minifyCSS(const std::string& css);
    
    // 验证
    bool validateCSS(const std::string& css);
    
private:
    bool m_debugMode;
    bool m_enableMinification;
    
    void reportError(const std::string& message);
};

}