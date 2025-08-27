#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace CHTL {

// 前向声明
class Scanner;
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JSCompiler;

// 代码片段类型
enum class FragmentType {
    CHTL,
    CHTLJS,
    CSS,
    JS,
    UNKNOWN
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    std::string content;
    int startLine;
    int startColumn;
    int endLine;
    int endColumn;
    std::string context;  // 上下文信息（如在哪个节点内）
};

// 编译器调度器
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 编译主入口
    std::string Compile(const std::string& source, const std::string& filename);
    
    // 配置
    void SetDebugMode(bool debug) { debugMode = debug; }
    void AddModulePath(const std::string& path);
    void SetOutputPath(const std::string& path) { outputPath = path; }
    
    // 错误处理
    bool HasErrors() const;
    std::vector<std::string> GetErrors() const;
    void ClearErrors();
    
private:
    // 编译器实例
    std::unique_ptr<Scanner> scanner;
    std::unique_ptr<CHTLCompiler> chtlCompiler;
    std::unique_ptr<CHTLJSCompiler> chtljsCompiler;
    std::unique_ptr<CSSCompiler> cssCompiler;
    std::unique_ptr<JSCompiler> jsCompiler;
    
    // 配置
    bool debugMode;
    std::string outputPath;
    std::vector<std::string> modulePaths;
    
    // 错误信息
    std::vector<std::string> errors;
    
    // 内部方法
    std::vector<CodeFragment> ScanCode(const std::string& source);
    std::string DispatchFragment(const CodeFragment& fragment);
    std::string MergeResults(const std::vector<std::string>& results);
    
    // 判断片段类型
    FragmentType DetermineFragmentType(const std::string& content, const std::string& context);
    
    // 处理特殊情况
    std::string ProcessLocalStyle(const std::string& content, const std::string& elementContext);
    std::string ProcessLocalScript(const std::string& content, const std::string& elementContext);
};

} // namespace CHTL