#include "CompilerDispatcher.h"
#include "Scanner/UnifiedScanner.h"
#include "Scanner/CodeSlice.h"
#include "Common/SourceFile.h"
#include "CSS/CSSCompiler.h"
#include "JS/JSCompiler.h"
#include <sstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : scanner_(std::make_unique<UnifiedScanner>()),
      // chtlCompiler_(std::make_unique<CHTLCompiler>()),
      // chtljsCompiler_(std::make_unique<CHTLJSCompiler>()),
      cssCompiler_(std::make_unique<CSSCompiler>()),
      jsCompiler_(std::make_unique<JSCompiler>()),
      debugMode_(false) {
    // TODO: 初始化CHTL和CHTLJS编译器
}

CompilerDispatcher::~CompilerDispatcher() = default;

std::string CompilerDispatcher::Compile(const std::shared_ptr<SourceFile>& sourceFile, 
                                       ErrorCollector& errorCollector) {
    // 使用统一扫描器扫描源文件
    auto slices = scanner_->Scan(sourceFile, errorCollector);
    
    if (errorCollector.HasFatalErrors()) {
        return "";
    }
    
    // 分发每个切片到对应的编译器
    std::vector<std::string> results;
    for (const auto& slice : slices) {
        std::string result = DispatchSlice(slice, errorCollector);
        if (!result.empty()) {
            results.push_back(result);
        }
    }
    
    // 合并结果
    return MergeResults(results);
}

void CompilerDispatcher::SetDebugMode(bool debug) {
    debugMode_ = debug;
    // TODO: 设置各个编译器的调试模式
}

std::string CompilerDispatcher::DispatchSlice(const std::shared_ptr<CodeSlice>& slice, 
                                             ErrorCollector& errorCollector) {
    switch (slice->GetType()) {
        case SliceType::CHTL:
            // TODO: 调用CHTL编译器
            return slice->GetContent();
            
        case SliceType::CHTL_JS:
            // TODO: 调用CHTL JS编译器
            return slice->GetContent();
            
        case SliceType::CSS:
            return cssCompiler_->Compile(slice, errorCollector);
            
        case SliceType::JAVASCRIPT:
            return jsCompiler_->Compile(slice, errorCollector);
            
        case SliceType::HTML:
        case SliceType::TEXT:
        default:
            // 直接返回原始内容
            return slice->GetContent();
    }
}

std::string CompilerDispatcher::MergeResults(const std::vector<std::string>& results) {
    std::stringstream ss;
    for (const auto& result : results) {
        ss << result;
    }
    return ss.str();
}

} // namespace CHTL