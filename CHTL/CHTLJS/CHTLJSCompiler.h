#pragma once

#include <string>
#include <memory>
#include <vector>

namespace CHTL {

// CHTL JS编译器主类
class CHTLJSCompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler();
    
    // 编译CHTL JS代码
    std::string Compile(const std::string& source, const std::string& context = "");
    
    // 处理局部脚本
    std::string ProcessLocalScript(const std::string& content, const std::string& elementContext);
    
    // 模块路径管理
    void AddModulePath(const std::string& path);
    
    // 错误处理
    bool HasErrors() const { return !errors.empty(); }
    std::vector<std::string> GetErrors() const { return errors; }
    void ClearErrors() { errors.clear(); }
    
    // 配置
    void SetDebugMode(bool debug) { debugMode = debug; }
    
private:
    std::vector<std::string> modulePaths;
    std::vector<std::string> errors;
    bool debugMode;
    
    void ReportError(const std::string& message);
};

} // namespace CHTL