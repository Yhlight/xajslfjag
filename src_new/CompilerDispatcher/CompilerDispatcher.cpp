#include "CompilerDispatcher.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CSS/CSSCompiler.h"
#include "../JS/JSCompiler.h"

namespace CHTL {

// 前向声明的编译器实现类
class CHTLCompilerImpl : public ICompiler {
public:
    CompilationResult compile(const CodeFragment& fragment) override {
        // CHTL编译器实现
        return CompilationResult(true, fragment.content, "", fragment.type);
    }
    
    FragmentType getSupportedType() const override { return FragmentType::CHTL; }
    std::string getName() const override { return "CHTL Compiler"; }
};

class CHTLJSCompilerImpl : public ICompiler {
public:
    CompilationResult compile(const CodeFragment& fragment) override {
        // CHTL JS编译器实现
        return CompilationResult(true, fragment.content, "", fragment.type);
    }
    
    FragmentType getSupportedType() const override { return FragmentType::CHTL_JS; }
    std::string getName() const override { return "CHTL JS Compiler"; }
};

class CSSCompilerImpl : public ICompiler {
public:
    CompilationResult compile(const CodeFragment& fragment) override {
        // CSS编译器实现
        return CompilationResult(true, fragment.content, "", fragment.type);
    }
    
    FragmentType getSupportedType() const override { return FragmentType::CSS; }
    std::string getName() const override { return "CSS Compiler (ANTLR)"; }
};

class JSCompilerImpl : public ICompiler {
public:
    CompilationResult compile(const CodeFragment& fragment) override {
        // JS编译器实现
        return CompilationResult(true, fragment.content, "", fragment.type);
    }
    
    FragmentType getSupportedType() const override { return FragmentType::JAVASCRIPT; }
    std::string getName() const override { return "JavaScript Compiler (ANTLR)"; }
};

CompilerDispatcher::CompilerDispatcher() {
    initializeDefaultCompilers();
    m_scanner = std::make_shared<UnifiedScanner>();
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

CompilationResult CompilerDispatcher::compileFragment(const CodeFragment& fragment) {
    ICompiler* compiler = getCompiler(fragment.type);
    if (!compiler) {
        std::string error = "No compiler found for fragment type";
        handleCompilationError(error, fragment);
        return CompilationResult(false, "", error, fragment.type);
    }
    
    return compiler->compile(fragment);
}

std::vector<CompilationResult> CompilerDispatcher::compileFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<CompilationResult> results;
    
    // 正确的编译顺序：
    // 1. 先处理CHTL和CHTL JS片段
    // 2. 合并结果
    // 3. 再处理CSS和JS片段（接收完整代码）
    
    std::vector<CompilationResult> chtlResults;
    std::vector<CompilationResult> chtlJSResults;
    std::vector<CodeFragment> cssFragments;
    std::vector<CodeFragment> jsFragments;
    
    // 第一阶段：分类并处理CHTL和CHTL JS片段
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            chtlResults.push_back(compileFragment(fragment));
        } else if (fragment.type == FragmentType::CHTL_JS) {
            chtlJSResults.push_back(compileFragment(fragment));
        } else if (fragment.type == FragmentType::CSS) {
            cssFragments.push_back(fragment);
        } else if (fragment.type == FragmentType::JAVASCRIPT) {
            jsFragments.push_back(fragment);
        }
    }
    
    // 添加CHTL和CHTL JS结果
    results.insert(results.end(), chtlResults.begin(), chtlResults.end());
    results.insert(results.end(), chtlJSResults.begin(), chtlJSResults.end());
    
    // 第二阶段：处理CSS和JS片段（接收完整代码）
    if (!cssFragments.empty() || !jsFragments.empty()) {
        std::string mergedCSS = mergeCSS(chtlResults);
        std::string mergedJS = mergeJavaScript(chtlJSResults);
        
        // 处理CSS片段
        for (auto cssFragment : cssFragments) {
            cssFragment.content = mergedCSS + "\n" + cssFragment.content;
            results.push_back(compileFragment(cssFragment));
        }
        
        // 处理JS片段
        for (auto jsFragment : jsFragments) {
            jsFragment.content = mergedJS + "\n" + jsFragment.content;
            results.push_back(compileFragment(jsFragment));
        }
    }
    
    return results;
}

CompilationResult CompilerDispatcher::compileSource(const std::string& source) {
    auto fragments = m_scanner->smartSlice(source);
    auto results = compileFragments(fragments);
    
    // 合并所有结果
    std::string finalHTML = mergeToHTML(results);
    
    return CompilationResult(true, finalHTML, "", FragmentType::UNKNOWN);
}

std::string CompilerDispatcher::mergeToHTML(const std::vector<CompilationResult>& results) {
    std::ostringstream html;
    
    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    
    // 添加CSS
    std::string css = mergeCSS(results);
    if (!css.empty()) {
        html << "<style>\n" << css << "\n</style>\n";
    }
    
    html << "</head>\n<body>\n";
    
    // 添加HTML内容
    for (const auto& result : results) {
        if (result.success && (result.sourceType == FragmentType::CHTL || result.sourceType == FragmentType::UNKNOWN)) {
            html << result.output << "\n";
        }
    }
    
    html << "\n";
    
    // 添加JavaScript
    std::string js = mergeJavaScript(results);
    if (!js.empty()) {
        html << "<script>\n" << js << "\n</script>\n";
    }
    
    html << "</body>\n</html>";
    
    return html.str();
}

std::string CompilerDispatcher::mergeCSS(const std::vector<CompilationResult>& results) {
    std::ostringstream css;
    
    for (const auto& result : results) {
        if (result.success && result.sourceType == FragmentType::CSS) {
            css << result.output << "\n";
        }
    }
    
    return css.str();
}

std::string CompilerDispatcher::mergeJavaScript(const std::vector<CompilationResult>& results) {
    std::ostringstream js;
    
    for (const auto& result : results) {
        if (result.success && (result.sourceType == FragmentType::CHTL_JS || result.sourceType == FragmentType::JAVASCRIPT)) {
            js << result.output << "\n";
        }
    }
    
    return js.str();
}

void CompilerDispatcher::initializeDefaultCompilers() {
    registerCompiler(std::make_unique<CHTLCompilerImpl>());
    registerCompiler(std::make_unique<CHTLJSCompilerImpl>());
    registerCompiler(std::make_unique<CSSCompilerImpl>());
    registerCompiler(std::make_unique<JSCompilerImpl>());
}

void CompilerDispatcher::handleCompilationError(const std::string& error, const CodeFragment& fragment) {
    ErrorReporter reporter;
    reporter.error(ErrorType::COMPILATION_ERROR, error, "", fragment.lineNumber, fragment.columnNumber);
}

} // namespace CHTL