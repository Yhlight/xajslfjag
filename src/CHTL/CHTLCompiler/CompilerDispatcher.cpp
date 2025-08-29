#include "CompilerDispatcher.h"
#include "CHTLJSCompiler.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLGenerator/CHTLGenerator.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

// ========== CHTLCompiler 实现 ==========

CHTLCompiler::CHTLCompiler() {
    // 初始化CHTL解析器和生成器
    // 这里需要根据实际的CHTL解析器实现进行初始化
}

CompilationResult CHTLCompiler::compile(const CodeFragment& fragment) {
    try {
        // 使用CHTL解析器解析片段
        // 使用CHTL生成器生成HTML/CSS
        
        // 简化实现：直接处理CHTL语法
        std::string output = fragment.content;
        
        // 基本的CHTL语法转换
        // 这里需要集成实际的CHTL解析器和生成器
        
        return CompilationResult(true, output, "", fragment.type);
    } catch (const std::exception& e) {
        return CompilationResult(false, "", e.what(), fragment.type);
    }
}

// ========== CHTLJSCompiler 实现 ==========

CHTLJSCompiler::CHTLJSCompiler() {
    m_enhancedCompiler = std::make_unique<CHTLJSEnhancedCompiler>();
}

CompilationResult CHTLJSCompiler::compile(const CodeFragment& fragment) {
    try {
        std::string output = m_enhancedCompiler->compile(fragment.content);
        return CompilationResult(true, output, "", fragment.type);
    } catch (const std::exception& e) {
        return CompilationResult(false, "", e.what(), fragment.type);
    }
}



// ========== CSSCompiler 实现 ==========

CSSCompiler::CSSCompiler() {
    // 初始化ANTLR4 CSS编译器
}

CompilationResult CSSCompiler::compile(const CodeFragment& fragment) {
    try {
        // 使用ANTLR4解析CSS
        // 这里需要集成ANTLR4 CSS语法
        
        std::string output = fragment.content;
        return CompilationResult(true, output, "", fragment.type);
    } catch (const std::exception& e) {
        return CompilationResult(false, "", e.what(), fragment.type);
    }
}

// ========== JavaScriptCompiler 实现 ==========

JavaScriptCompiler::JavaScriptCompiler() {
    // 初始化ANTLR4 JavaScript编译器
}

CompilationResult JavaScriptCompiler::compile(const CodeFragment& fragment) {
    try {
        // 使用ANTLR4解析JavaScript
        // 这里需要集成ANTLR4 JavaScript语法
        
        std::string output = fragment.content;
        return CompilationResult(true, output, "", fragment.type);
    } catch (const std::exception& e) {
        return CompilationResult(false, "", e.what(), fragment.type);
    }
}

// ========== CompilerDispatcher 实现 ==========

CompilerDispatcher::CompilerDispatcher() 
    : m_parallelCompilation(false) {
    initializeDefaultCompilers();
}

void CompilerDispatcher::registerCompiler(std::unique_ptr<ICompiler> compiler) {
    m_compilers.push_back(std::move(compiler));
}

ICompiler* CompilerDispatcher::getCompiler(FragmentType type) {
    for (auto& compiler : m_compilers) {
        if (compiler->getSupportedType() == type) {
            return compiler.get();
        }
    }
    return nullptr;
}

std::vector<ICompiler*> CompilerDispatcher::getAllCompilers() const {
    std::vector<ICompiler*> compilers;
    for (const auto& compiler : m_compilers) {
        compilers.push_back(compiler.get());
    }
    return compilers;
}

CompilationResult CompilerDispatcher::compileFragment(const CodeFragment& fragment) {
    ICompiler* compiler = getCompiler(fragment.type);
    if (!compiler) {
        std::string error = "No compiler found for fragment type: " + std::to_string((int)fragment.type);
        handleCompilationError(error, fragment);
        return CompilationResult(false, "", error, fragment.type);
    }
    
    CompilationResult result = compiler->compile(fragment);
    if (!result.success) {
        handleCompilationError(result.errorMessage, fragment);
    }
    
    return result;
}

std::vector<CompilationResult> CompilerDispatcher::compileFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<CompilationResult> results;
    
    // 优化编译顺序
    auto optimizedFragments = optimizeCompilationOrder(fragments);
    
    if (m_parallelCompilation) {
        // 并行编译（需要线程池实现）
        for (const auto& fragment : optimizedFragments) {
            results.push_back(compileFragment(fragment));
        }
    } else {
        // 串行编译
        for (const auto& fragment : optimizedFragments) {
            results.push_back(compileFragment(fragment));
        }
    }
    
    return results;
}

CompilationResult CompilerDispatcher::compileSource(const std::string& source) {
    if (!m_scanner) {
        m_scanner = std::make_shared<CHTLUnifiedScanner>();
    }
    
    // 扫描源代码
    auto fragments = m_scanner->scan(source);
    
    // 编译片段
    auto results = compileFragments(fragments);
    
    // 合并结果
    std::string html = mergeToHTML(results);
    
    // 检查是否有编译错误
    bool success = true;
    std::string errorMessage;
    
    for (const auto& result : results) {
        if (!result.success) {
            success = false;
            if (!errorMessage.empty()) errorMessage += "; ";
            errorMessage += result.errorMessage;
        }
    }
    
    return CompilationResult(success, html, errorMessage, FragmentType::HTML);
}

std::string CompilerDispatcher::mergeToHTML(const std::vector<CompilationResult>& results) {
    std::string html = generateHTMLTemplate();
    std::string css = mergeCSS(results);
    std::string js = mergeJavaScript(results);
    
    // 插入CSS和JavaScript
    html = insertCSSIntoHTML(html, css);
    html = insertJavaScriptIntoHTML(html, js);
    
    // 插入HTML内容
    std::stringstream htmlContent;
    for (const auto& result : results) {
        if (result.success && (result.sourceType == FragmentType::CHTL || 
                              result.sourceType == FragmentType::HTML)) {
            htmlContent << result.output << "\n";
        }
    }
    
    // 替换body内容
    size_t bodyStart = html.find("<body>");
    size_t bodyEnd = html.find("</body>");
    if (bodyStart != std::string::npos && bodyEnd != std::string::npos) {
        bodyStart += 6; // length of "<body>"
        html.replace(bodyStart, bodyEnd - bodyStart, htmlContent.str());
    }
    
    return html;
}

std::string CompilerDispatcher::mergeCSS(const std::vector<CompilationResult>& results) {
    std::stringstream css;
    
    for (const auto& result : results) {
        if (result.success && result.sourceType == FragmentType::CSS) {
            css << result.output << "\n";
        }
    }
    
    return css.str();
}

std::string CompilerDispatcher::mergeJavaScript(const std::vector<CompilationResult>& results) {
    std::stringstream js;
    
    for (const auto& result : results) {
        if (result.success && (result.sourceType == FragmentType::JAVASCRIPT || 
                              result.sourceType == FragmentType::CHTL_JS)) {
            js << result.output << "\n";
        }
    }
    
    return js.str();
}

void CompilerDispatcher::setScanner(std::shared_ptr<CHTLUnifiedScanner> scanner) {
    m_scanner = scanner;
}

void CompilerDispatcher::enableParallelCompilation(bool enabled) {
    m_parallelCompilation = enabled;
}

void CompilerDispatcher::setCompilationOptions(const std::unordered_map<std::string, std::string>& options) {
    m_options = options;
}

std::vector<std::string> CompilerDispatcher::getCompilationErrors() const {
    return m_errors;
}

std::string CompilerDispatcher::generateCompilationReport(const std::vector<CompilationResult>& results) {
    std::stringstream ss;
    
    ss << "Compilation Report:\n";
    ss << "==================\n\n";
    
    size_t successCount = 0;
    size_t errorCount = 0;
    
    for (const auto& result : results) {
        if (result.success) {
            successCount++;
        } else {
            errorCount++;
        }
    }
    
    ss << "Total Fragments: " << results.size() << "\n";
    ss << "Successful: " << successCount << "\n";
    ss << "Failed: " << errorCount << "\n\n";
    
    if (errorCount > 0) {
        ss << "Errors:\n";
        for (const auto& result : results) {
            if (!result.success) {
                ss << "  - " << result.errorMessage << "\n";
            }
        }
    }
    
    return ss.str();
}

void CompilerDispatcher::clearErrors() {
    m_errors.clear();
}

bool CompilerDispatcher::validateFragmentCompatibility(const CodeFragment& fragment) {
    ICompiler* compiler = getCompiler(fragment.type);
    return compiler != nullptr;
}

std::vector<CodeFragment> CompilerDispatcher::optimizeCompilationOrder(const std::vector<CodeFragment>& fragments) {
    std::vector<CodeFragment> optimized = fragments;
    
    // 简单的优化：CHTL片段优先，然后是CSS，最后是JavaScript
    std::sort(optimized.begin(), optimized.end(), [](const CodeFragment& a, const CodeFragment& b) {
        static const std::unordered_map<FragmentType, int> priority = {
            {FragmentType::CHTL, 1},
            {FragmentType::CSS, 2},
            {FragmentType::CHTL_JS, 3},
            {FragmentType::JAVASCRIPT, 4},
            {FragmentType::HTML, 5}
        };
        
        int priorityA = priority.count(a.type) ? priority.at(a.type) : 10;
        int priorityB = priority.count(b.type) ? priority.at(b.type) : 10;
        
        return priorityA < priorityB;
    });
    
    return optimized;
}

void CompilerDispatcher::initializeDefaultCompilers() {
    registerCompiler(std::make_unique<CHTLCompiler>());
    registerCompiler(std::make_unique<CHTLJSCompiler>());
    registerCompiler(std::make_unique<CSSCompiler>());
    registerCompiler(std::make_unique<JavaScriptCompiler>());
}

void CompilerDispatcher::handleCompilationError(const std::string& error, const CodeFragment& fragment) {
    std::string fullError = error + " (Position: " + std::to_string(fragment.startPosition) + 
                           "-" + std::to_string(fragment.endPosition) + ")";
    m_errors.push_back(fullError);
    reportError(fullError);
}

std::string CompilerDispatcher::generateHTMLTemplate() {
    return R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Generated Page</title>
    <style>
    /* CSS_PLACEHOLDER */
    </style>
</head>
<body>
    <!-- HTML_CONTENT_PLACEHOLDER -->
    <script>
    /* JS_PLACEHOLDER */
    </script>
</body>
</html>)";
}

std::string CompilerDispatcher::insertCSSIntoHTML(const std::string& html, const std::string& css) {
    return std::regex_replace(html, std::regex("/\\* CSS_PLACEHOLDER \\*/"), css);
}

std::string CompilerDispatcher::insertJavaScriptIntoHTML(const std::string& html, const std::string& js) {
    return std::regex_replace(html, std::regex("/\\* JS_PLACEHOLDER \\*/"), js);
}

void CompilerDispatcher::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, message);
}

} // namespace CHTL