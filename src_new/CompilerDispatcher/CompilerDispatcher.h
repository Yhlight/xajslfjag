#pragma once
#include "../Scanner/UnifiedScanner.h"
#include "../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

/**
 * 编译结果
 */
struct CompilationResult {
    bool success;                   // 编译是否成功
    std::string output;             // 编译输出
    std::string errorMessage;       // 错误信息
    FragmentType sourceType;        // 源片段类型
    
    CompilationResult(bool s = false, const std::string& o = "", 
                     const std::string& e = "", FragmentType t = FragmentType::UNKNOWN)
        : success(s), output(o), errorMessage(e), sourceType(t) {}
};

/**
 * 编译器接口
 */
class ICompiler {
public:
    virtual ~ICompiler() = default;
    
    /**
     * 编译代码片段
     */
    virtual CompilationResult compile(const CodeFragment& fragment) = 0;
    
    /**
     * 获取支持的片段类型
     */
    virtual FragmentType getSupportedType() const = 0;
    
    /**
     * 获取编译器名称
     */
    virtual std::string getName() const = 0;
};

/**
 * 编译器调度器
 * 严格按照目标规划.ini的架构设计
 * 管理四个独立编译器：CHTL(手写)、CHTL JS(手写)、CSS(ANTLR)、JavaScript(ANTLR)
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher() = default;
    
    // ========== 编译器管理 ==========
    
    /**
     * 注册编译器
     */
    void registerCompiler(std::unique_ptr<ICompiler> compiler);
    
    /**
     * 获取支持指定类型的编译器
     */
    ICompiler* getCompiler(FragmentType type);
    
    // ========== 编译调度 ==========
    
    /**
     * 编译单个代码片段
     */
    CompilationResult compileFragment(const CodeFragment& fragment);
    
    /**
     * 编译多个代码片段 - 正确的编译顺序
     * 1. 先处理CHTL和CHTL JS片段
     * 2. 合并CHTL和CHTL JS的结果
     * 3. 再将合并后的完整代码交给CSS和JS编译器
     */
    std::vector<CompilationResult> compileFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 编译完整源代码（包含扫描）
     */
    CompilationResult compileSource(const std::string& source);
    
    // ========== 结果合并 ==========
    
    /**
     * 合并编译结果为HTML输出
     */
    std::string mergeToHTML(const std::vector<CompilationResult>& results);
    
    /**
     * 合并CSS结果
     */
    std::string mergeCSS(const std::vector<CompilationResult>& results);
    
    /**
     * 合并JavaScript结果
     */
    std::string mergeJavaScript(const std::vector<CompilationResult>& results);

private:
    std::vector<std::unique_ptr<ICompiler>> m_compilers;       // 编译器列表
    std::shared_ptr<UnifiedScanner> m_scanner;                // 统一扫描器
    
    /**
     * 初始化默认编译器
     */
    void initializeDefaultCompilers();
    
    /**
     * 处理编译错误
     */
    void handleCompilationError(const std::string& error, const CodeFragment& fragment);
};

} // namespace CHTL