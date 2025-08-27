#pragma once

#include <string>
#include <memory>
#include <vector>

namespace CHTLJS {

// CHTL JS编译器主类
class CHTLJSCompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler();
    
    // 编译CHTL JS代码
    std::string Compile(const std::string& source);
    
    // 错误处理
    bool HasErrors() const;
    std::vector<std::string> GetErrors() const;
    std::vector<std::string> GetWarnings() const;
    
    // 配置
    void SetDebugMode(bool debug);
    
    // 重置编译器状态
    void Reset();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace CHTLJS