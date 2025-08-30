#pragma once

#include <string>
#include <regex>

namespace CHTL {

class JSCompiler {
public:
    JSCompiler();
    ~JSCompiler();
    
    // 主编译接口
    std::string compile(const std::string& jsCode);
    
    // 编译选项
    void enableDebugMode(bool enable) { m_debugMode = enable; }
    void enableMinification(bool enable) { m_enableMinification = enable; }
    
    // JavaScript处理
    std::string formatJavaScript(const std::string& js);
    std::string minifyJavaScript(const std::string& js);
    
    // 验证
    bool validateJavaScript(const std::string& js);
    
private:
    bool m_debugMode;
    bool m_enableMinification;
    
    void reportError(const std::string& message);
};

}