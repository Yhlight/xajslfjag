#include "CHTLCompilerDispatcher.h"
// #include "../CHTL/CHTLParser/CHTLCompiler.h"       // 暂时注释，待实现
// #include "../CHTLJS/CHTLJSParser/CHTLJSCompiler.h"  // 暂时注释，待实现
// #include "../CSS/CSSCompiler.h"                      // 暂时注释，待实现
// #include "../JS/JSCompiler.h"                        // 暂时注释，待实现
#include <fstream>
#include <sstream>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLCompilerDispatcher::CHTLCompilerDispatcher()
    : outputFormat_("html5"), optimizationLevel_(1), debugMode_(false),
      workingDirectory_("."), compiledFragments_(0), lastCompilationTime_(0.0),
      timerStart_(0.0) {
    
    InitializeCompilers();
}

CHTLCompilerDispatcher::~CHTLCompilerDispatcher() = default;

CompilationResultPtr CHTLCompilerDispatcher::CompileFile(const std::string& filePath) {
    std::string input = ReadFile(filePath);
    if (input.empty()) {
        auto result = std::make_shared<CompilationResult>(false);
        result->errors.push_back("Failed to read file: " + filePath);
        return result;
    }
    
    return CompileString(input);
}

CompilationResultPtr CHTLCompilerDispatcher::CompileString(const std::string& input) {
    ClearErrors();
    StartTimer();
    
    auto result = CompileInternal(input);
    
    EndTimer();
    
    if (debugMode_) {
        PrintCompilationStats();
    }
    
    return result;
}

std::vector<CodeFragmentPtr> CHTLCompilerDispatcher::ScanInput(const std::string& input) {
    scanner_->SetInput(input);
    return scanner_->ScanAndSlice();
}

CompilationResultPtr CHTLCompilerDispatcher::CompileFragments(const std::vector<CodeFragmentPtr>& fragments) {
    StartTimer();
    
    std::vector<CompilationResultPtr> results;
    compiledFragments_ = 0;
    
    for (const auto& fragment : fragments) {
        CompilationResultPtr fragmentResult;
        
        switch (fragment->type) {
            case CodeFragmentType::CHTL:
                fragmentResult = CompileCHTLFragment(fragment);
                break;
            case CodeFragmentType::CHTL_JS:
                fragmentResult = CompileCHTLJSFragment(fragment);
                break;
            case CodeFragmentType::CSS:
                fragmentResult = CompileCSSFragment(fragment);
                break;
            case CodeFragmentType::JS:
                fragmentResult = CompileJSFragment(fragment);
                break;
            default:
                // 未知类型直接输出
                fragmentResult = std::make_shared<CompilationResult>(true);
                fragmentResult->output = fragment->content;
                break;
        }
        
        results.push_back(fragmentResult);
        compiledFragments_++;
    }
    
    auto finalResult = MergeResults(results);
    
    EndTimer();
    
    return finalResult;
}

void CHTLCompilerDispatcher::SetOutputFormat(const std::string& format) {
    outputFormat_ = format;
    ConfigureCompilers();
}

void CHTLCompilerDispatcher::SetOptimizationLevel(int level) {
    optimizationLevel_ = std::max(0, std::min(3, level));
    ConfigureCompilers();
}

void CHTLCompilerDispatcher::EnableDebugMode(bool enable) {
    debugMode_ = enable;
    scanner_->EnableDebugMode(enable);
    ConfigureCompilers();
}

void CHTLCompilerDispatcher::SetWorkingDirectory(const std::string& dir) {
    workingDirectory_ = dir;
}

void CHTLCompilerDispatcher::AddModulePath(const std::string& path) {
    modulePaths_.push_back(path);
}

void CHTLCompilerDispatcher::SetOfficialModulePath(const std::string& path) {
    officialModulePath_ = path;
}

bool CHTLCompilerDispatcher::HasErrors() const {
    return !errors_.empty();
}

const std::vector<std::string>& CHTLCompilerDispatcher::GetErrors() const {
    return errors_;
}

const std::vector<std::string>& CHTLCompilerDispatcher::GetWarnings() const {
    return warnings_;
}

void CHTLCompilerDispatcher::ClearErrors() {
    errors_.clear();
    warnings_.clear();
}

size_t CHTLCompilerDispatcher::GetCompiledFragments() const {
    return compiledFragments_;
}

double CHTLCompilerDispatcher::GetLastCompilationTime() const {
    return lastCompilationTime_;
}

void CHTLCompilerDispatcher::PrintCompilationStats() const {
    std::cout << "=== CHTL Compilation Statistics ===" << std::endl;
    std::cout << "Compiled Fragments: " << compiledFragments_ << std::endl;
    std::cout << "Compilation Time: " << lastCompilationTime_ << " ms" << std::endl;
    std::cout << "Output Format: " << outputFormat_ << std::endl;
    std::cout << "Optimization Level: " << optimizationLevel_ << std::endl;
    std::cout << "Errors: " << errors_.size() << std::endl;
    std::cout << "Warnings: " << warnings_.size() << std::endl;
    std::cout << "=== End Statistics ===" << std::endl;
}

CompilationResultPtr CHTLCompilerDispatcher::CompileInternal(const std::string& input) {
    // 步骤1: 扫描和分割
    auto fragments = ScanInput(input);
    
    if (scanner_->HasErrors()) {
        auto result = std::make_shared<CompilationResult>(false);
        result->errors = scanner_->GetErrors();
        return result;
    }
    
    // 步骤2: 编译各个片段
    auto result = CompileFragments(fragments);
    
    // 步骤3: 优化输出
    if (result->success && optimizationLevel_ > 0) {
        result->output = OptimizeOutput(result->output);
    }
    
    // 步骤4: 收集编译器错误
    CollectCompilerErrors();
    
    return result;
}

CompilationResultPtr CHTLCompilerDispatcher::CompileCHTLFragment(const CodeFragmentPtr& fragment) {
    auto result = std::make_shared<CompilationResult>();
    
    // TODO: 待CHTL编译器实现后启用
    // if (!chtlCompiler_) {
    //     result->success = false;
    //     result->errors.push_back("CHTL compiler not initialized");
    //     return result;
    // }
    
    try {
        // 这里应该调用实际的CHTL编译器
        // 目前先返回模拟结果
        result->success = true;
        result->output = "<!-- CHTL Compiled: " + fragment->content.substr(0, 50) + "... -->";
        
        if (debugMode_) {
            result->output += "\n<!-- Fragment Type: CHTL -->";
        }
    } catch (const std::exception& e) {
        result->success = false;
        result->errors.push_back("CHTL compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompilationResultPtr CHTLCompilerDispatcher::CompileCHTLJSFragment(const CodeFragmentPtr& fragment) {
    auto result = std::make_shared<CompilationResult>();
    
    // TODO: 待CHTL JS编译器实现后启用
    // if (!chtljsCompiler_) {
    //     result->success = false;
    //     result->errors.push_back("CHTL JS compiler not initialized");
    //     return result;
    // }
    
    try {
        // 这里应该调用实际的CHTL JS编译器
        // 目前先返回模拟结果
        result->success = true;
        result->output = "/* CHTL JS Compiled: " + fragment->content.substr(0, 50) + "... */";
        
        if (debugMode_) {
            result->output += "\n/* Fragment Type: CHTL JS */";
        }
    } catch (const std::exception& e) {
        result->success = false;
        result->errors.push_back("CHTL JS compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompilationResultPtr CHTLCompilerDispatcher::CompileCSSFragment(const CodeFragmentPtr& fragment) {
    auto result = std::make_shared<CompilationResult>();
    
    // TODO: 待CSS编译器实现后启用
    // if (!cssCompiler_) {
    //     result->success = false;
    //     result->errors.push_back("CSS compiler not initialized");
    //     return result;
    // }
    
    try {
        // 这里应该调用实际的CSS编译器(ANTLR4)
        // 目前先返回模拟结果
        result->success = true;
        result->output = "/* CSS Compiled */ " + fragment->content;
        
        if (debugMode_) {
            result->output += "\n/* Fragment Type: CSS */";
        }
    } catch (const std::exception& e) {
        result->success = false;
        result->errors.push_back("CSS compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompilationResultPtr CHTLCompilerDispatcher::CompileJSFragment(const CodeFragmentPtr& fragment) {
    auto result = std::make_shared<CompilationResult>();
    
    // TODO: 待JS编译器实现后启用
    // if (!jsCompiler_) {
    //     result->success = false;
    //     result->errors.push_back("JS compiler not initialized");
    //     return result;
    // }
    
    try {
        // 这里应该调用实际的JS编译器(ANTLR4)
        // 目前先返回模拟结果
        result->success = true;
        result->output = "/* JS Compiled */ " + fragment->content;
        
        if (debugMode_) {
            result->output += "\n/* Fragment Type: JS */";
        }
    } catch (const std::exception& e) {
        result->success = false;
        result->errors.push_back("JS compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompilationResultPtr CHTLCompilerDispatcher::MergeResults(const std::vector<CompilationResultPtr>& results) {
    auto mergedResult = std::make_shared<CompilationResult>(true);
    
    // 合并输出
    mergedResult->output = CombineOutputs(results);
    
    // 合并错误和警告
    for (const auto& result : results) {
        if (!result->success) {
            mergedResult->success = false;
        }
        
        mergedResult->errors.insert(mergedResult->errors.end(),
                                   result->errors.begin(), result->errors.end());
        mergedResult->warnings.insert(mergedResult->warnings.end(),
                                     result->warnings.begin(), result->warnings.end());
    }
    
    return mergedResult;
}

std::string CHTLCompilerDispatcher::CombineOutputs(const std::vector<CompilationResultPtr>& results) {
    std::ostringstream combined;
    
    // 根据输出格式添加文档声明
    if (outputFormat_ == "html5") {
        combined << "<!DOCTYPE html>\n";
    }
    
    for (const auto& result : results) {
        if (result->success && !result->output.empty()) {
            combined << result->output;
        }
    }
    
    return combined.str();
}

std::string CHTLCompilerDispatcher::OptimizeOutput(const std::string& output) {
    std::string optimized = output;
    
    if (optimizationLevel_ >= 1) {
        RemoveComments(optimized);
    }
    
    if (optimizationLevel_ >= 2) {
        CompressWhitespace(optimized);
    }
    
    if (optimizationLevel_ >= 3) {
        ApplyMinification(optimized);
    }
    
    return optimized;
}

void CHTLCompilerDispatcher::ApplyMinification(std::string& output) {
    // 实现代码压缩
    // 移除不必要的空格、换行等
    std::regex whitespacePattern(R"(\s+)");
    output = std::regex_replace(output, whitespacePattern, " ");
}

void CHTLCompilerDispatcher::RemoveComments(std::string& output) {
    // 移除HTML注释
    std::regex htmlCommentPattern(R"(<!--.*?-->)");
    output = std::regex_replace(output, htmlCommentPattern, "");
    
    // 移除CSS注释
    std::regex cssCommentPattern(R"(/\*.*?\*/)");
    output = std::regex_replace(output, cssCommentPattern, "");
    
    // 移除JS单行注释
    std::regex jsCommentPattern(R"(//.*?$)");
    output = std::regex_replace(output, jsCommentPattern, "");
}

void CHTLCompilerDispatcher::CompressWhitespace(std::string& output) {
    // 压缩连续空白字符
    std::regex multiSpacePattern(R"(\s{2,})");
    output = std::regex_replace(output, multiSpacePattern, " ");
    
    // 移除行首尾空白
    std::regex lineSpacePattern(R"(^\s+|\s+$)");
    output = std::regex_replace(output, lineSpacePattern, "");
}

std::string CHTLCompilerDispatcher::ReadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        ReportError("Cannot open file: " + filePath);
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLCompilerDispatcher::WriteFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        ReportError("Cannot write to file: " + filePath);
        return false;
    }
    
    file << content;
    return true;
}

std::string CHTLCompilerDispatcher::ResolveIncludePath(const std::string& path) {
    // 路径解析逻辑
    return workingDirectory_ + "/" + path;
}

void CHTLCompilerDispatcher::ReportError(const std::string& message) {
    errors_.push_back(message);
}

void CHTLCompilerDispatcher::ReportWarning(const std::string& message) {
    warnings_.push_back(message);
}

void CHTLCompilerDispatcher::CollectCompilerErrors() {
    // 收集各个编译器的错误信息
    // 这里需要各个编译器提供错误接口
}

void CHTLCompilerDispatcher::InitializeCompilers() {
    scanner_ = std::make_unique<CHTLUnifiedScanner>();
    
    // TODO: 待各编译器实现后再启用
    // chtlCompiler_ = std::make_unique<CHTLCompiler>();
    // chtljsCompiler_ = std::make_unique<CHTLJSCompiler>();
    // cssCompiler_ = std::make_unique<CSSCompiler>();
    // jsCompiler_ = std::make_unique<JSCompiler>();
    
    ConfigureCompilers();
}

void CHTLCompilerDispatcher::ConfigureCompilers() {
    if (scanner_) {
        scanner_->EnableDebugMode(debugMode_);
    }
    
    // 配置各个编译器
    // 这里需要各个编译器提供配置接口
}

double CHTLCompilerDispatcher::GetCurrentTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0;
}

void CHTLCompilerDispatcher::StartTimer() {
    timerStart_ = GetCurrentTime();
}

void CHTLCompilerDispatcher::EndTimer() {
    lastCompilationTime_ = GetCurrentTime() - timerStart_;
}

} // namespace CHTL