#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Common/Error.h"

namespace CHTL {

class SourceFile;
class CodeSlice;
class UnifiedScanner;
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JSCompiler;

// 编译器调度器 - 协调四个编译器的工作
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 编译CHTL文件
    std::string Compile(const std::shared_ptr<SourceFile>& sourceFile, ErrorCollector& errorCollector);
    
    // 设置调试模式
    void SetDebugMode(bool debug);
    
private:
    // 分发代码片段到对应的编译器
    std::string DispatchSlice(const std::shared_ptr<CodeSlice>& slice, ErrorCollector& errorCollector);
    
    // 合并编译结果
    std::string MergeResults(const std::vector<std::string>& results);
    
private:
    std::unique_ptr<UnifiedScanner> scanner_;
    std::unique_ptr<CHTLCompiler> chtlCompiler_;
    std::unique_ptr<CHTLJSCompiler> chtljsCompiler_;
    std::unique_ptr<CSSCompiler> cssCompiler_;
    std::unique_ptr<JSCompiler> jsCompiler_;
    bool debugMode_;
};

} // namespace CHTL