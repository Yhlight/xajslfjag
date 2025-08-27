#include "CompilerDispatcher.h"
#include "../Scanner/Scanner.h"
#include "CHTL/CHTLCompiler.h"
#include "CHTLJS/CHTLJSCompiler.h"
#include "CSS/CSSCompiler.h"
#include "JS/JSCompiler.h"
#include <sstream>
#include <iostream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : debugMode(false) {
    // 创建编译器实例
    scanner = std::make_unique<Scanner>();
    chtlCompiler = std::make_unique<CHTLCompiler>();
    chtljsCompiler = std::make_unique<CHTLJS::CHTLJSCompiler>();
    cssCompiler = std::make_unique<CSSCompiler>();
    jsCompiler = std::make_unique<JSCompiler>();
}

CompilerDispatcher::~CompilerDispatcher() = default;

std::string CompilerDispatcher::Compile(const std::string& source, const std::string& filename) {
    ClearErrors();
    
    try {
        // 1. 使用Scanner扫描代码，分割成片段
        auto fragments = ScanCode(source);
        
        if (debugMode) {
            std::cout << "扫描到 " << fragments.size() << " 个代码片段\n";
        }
        
        // 2. 对每个片段调用相应的编译器
        std::vector<std::string> results;
        for (const auto& fragment : fragments) {
            if (debugMode) {
                std::cout << "处理片段: 类型=" << static_cast<int>(fragment.type) 
                         << ", 行=" << fragment.startLine << "-" << fragment.endLine << "\n";
            }
            
            std::string result = DispatchFragment(fragment);
            if (!result.empty()) {
                results.push_back(result);
            }
        }
        
        // 3. 合并结果
        return MergeResults(results);
        
    } catch (const std::exception& e) {
        errors.push_back(std::string("编译错误: ") + e.what());
        return "";
    }
}

void CompilerDispatcher::AddModulePath(const std::string& path) {
    modulePaths.push_back(path);
    
    // 通知各编译器新的模块路径
    chtlCompiler->AddModulePath(path);
    // CHTL JS编译器暂不支持模块路径
}

bool CompilerDispatcher::HasErrors() const {
    return !errors.empty() || 
           chtlCompiler->HasErrors() ||
           chtljsCompiler->HasErrors() ||
           cssCompiler->HasErrors() ||
           jsCompiler->HasErrors();
}

std::vector<std::string> CompilerDispatcher::GetErrors() const {
    std::vector<std::string> allErrors = errors;
    
    // 收集各编译器的错误
    auto chtlErrors = chtlCompiler->GetErrors();
    allErrors.insert(allErrors.end(), chtlErrors.begin(), chtlErrors.end());
    
    auto chtljsErrors = chtljsCompiler->GetErrors();
    allErrors.insert(allErrors.end(), chtljsErrors.begin(), chtljsErrors.end());
    
    auto cssErrors = cssCompiler->GetErrors();
    allErrors.insert(allErrors.end(), cssErrors.begin(), cssErrors.end());
    
    auto jsErrors = jsCompiler->GetErrors();
    allErrors.insert(allErrors.end(), jsErrors.begin(), jsErrors.end());
    
    return allErrors;
}

void CompilerDispatcher::ClearErrors() {
    errors.clear();
    chtlCompiler->ClearErrors();
    // CHTL JS编译器使用Reset()来清除错误
    chtljsCompiler->Reset();
    cssCompiler->ClearErrors();
    jsCompiler->ClearErrors();
}

std::vector<CodeFragment> CompilerDispatcher::ScanCode(const std::string& source) {
    // 使用Scanner进行代码切割
    scanner->SetSource(source);
    scanner->SetDebugMode(debugMode);
    
    std::vector<CodeFragment> fragments;
    
    while (!scanner->IsEOF()) {
        auto fragment = scanner->NextFragment();
        if (fragment.type != FragmentType::UNKNOWN) {
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

std::string CompilerDispatcher::DispatchFragment(const CodeFragment& fragment) {
    switch (fragment.type) {
        case FragmentType::CHTL:
            return chtlCompiler->Compile(fragment.content, fragment.context);
            
        case FragmentType::CHTLJS:
            return chtljsCompiler->Compile(fragment.content);
            
        case FragmentType::CSS:
            // 判断是否是局部样式
            if (!fragment.context.empty() && fragment.context.find("local:") == 0) {
                return ProcessLocalStyle(fragment.content, fragment.context.substr(6));
            }
            return cssCompiler->Compile(fragment.content);
            
        case FragmentType::JS:
            // 判断是否是局部脚本
            if (!fragment.context.empty() && fragment.context.find("local:") == 0) {
                return ProcessLocalScript(fragment.content, fragment.context.substr(6));
            }
            return jsCompiler->Compile(fragment.content);
            
        default:
            return "";
    }
}

std::string CompilerDispatcher::MergeResults(const std::vector<std::string>& results) {
    std::stringstream ss;
    
    // HTML5 文档类型声明
    ss << "<!DOCTYPE html>\n";
    
    // 合并所有结果
    for (const auto& result : results) {
        ss << result;
        if (!result.empty() && result.back() != '\n') {
            ss << '\n';
        }
    }
    
    return ss.str();
}

FragmentType CompilerDispatcher::DetermineFragmentType(const std::string& content, const std::string& context) {
    // 根据内容和上下文判断片段类型
    // 这个方法将在Scanner中实现更复杂的逻辑
    
    // 简单的判断逻辑
    if (context == "style") {
        return FragmentType::CSS;
    } else if (context == "script") {
        // 需要进一步判断是JS还是CHTLJS
        if (content.find("{{") != std::string::npos || 
            content.find("->") != std::string::npos) {
            return FragmentType::CHTLJS;
        }
        return FragmentType::JS;
    }
    
    return FragmentType::CHTL;
}

std::string CompilerDispatcher::ProcessLocalStyle(const std::string& content, const std::string& elementContext) {
    // 处理局部样式块
    // 需要CHTL编译器协助处理选择器自动化等功能
    return chtlCompiler->ProcessLocalStyle(content, elementContext);
}

std::string CompilerDispatcher::ProcessLocalScript(const std::string& content, const std::string& elementContext) {
    // 处理局部脚本块
    // 可能包含CHTLJS语法
    if (content.find("{{") != std::string::npos || 
        content.find("->") != std::string::npos) {
        // 局部脚本直接通过CHTL JS编译器处理
        return chtljsCompiler->Compile(content);
    }
    return jsCompiler->Compile(content);
}

} // namespace CHTL