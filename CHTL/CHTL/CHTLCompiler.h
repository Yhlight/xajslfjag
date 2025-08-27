#pragma once

#include <string>
#include <memory>
#include <vector>

namespace CHTL {

class GlobalMap;
class CHTLContext;
class CHTLParser;
class CHTLGenerator;

// CHTL编译器主类
class CHTLCompiler {
public:
    CHTLCompiler();
    ~CHTLCompiler();
    
    // 编译CHTL代码
    std::string Compile(const std::string& source, const std::string& context = "");
    
    // 处理局部样式
    std::string ProcessLocalStyle(const std::string& content, const std::string& elementContext);
    
    // 模块路径管理
    void AddModulePath(const std::string& path);
    std::vector<std::string> GetModulePaths() const { return modulePaths; }
    
    // 错误处理
    bool HasErrors() const { return !errors.empty(); }
    std::vector<std::string> GetErrors() const { return errors; }
    void ClearErrors() { errors.clear(); }
    
    // 配置
    void SetDebugMode(bool debug) { debugMode = debug; }
    
private:
    std::shared_ptr<GlobalMap> globalMap;
    std::shared_ptr<CHTLContext> context;
    std::shared_ptr<CHTLParser> parser;
    std::shared_ptr<CHTLGenerator> generator;
    
    std::vector<std::string> modulePaths;
    std::vector<std::string> errors;
    bool debugMode;
    
    void ReportError(const std::string& message);
};

} // namespace CHTL