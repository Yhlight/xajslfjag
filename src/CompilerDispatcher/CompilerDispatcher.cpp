#include "CompilerDispatcher.h"
#include "../Scanner/UnifiedScanner.h"
#include "../CHTL/CHTLManage/ModuleManager.h"
#include "../CHTL/CMODSystem/CMODApi.h"
#include "../CHTL JS/CJMODSystem/CJMODApi.h"
#include "../Error/ErrorReporter.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace CHTL {

// ICompiler接口实现
class CHTLCompilerImpl : public ICompiler {
public:
    CompilationResult compile(const CodeFragment& fragment) override {
        CompilationResult result;
        result.success = true;
        result.outputType = "html";
        
        try {
            // CHTL编译逻辑
            std::string htmlOutput = compileCHTLToHTML(fragment.content);
            std::string cssOutput = extractCSSFromCHTL(fragment.content);
            
            result.output = htmlOutput;
            result.additionalOutputs["css"] = cssOutput;
            result.fragmentType = fragment.type;
            
            if (m_debugMode) {
                std::cout << "  ✓ CHTL编译完成: " << htmlOutput.length() << " 字符HTML" << std::endl;
            }
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = "CHTL编译错误: " + std::string(e.what());
            reportError(result.errorMessage);
        }
        
        return result;
    }
    
    std::string getCompilerName() const override {
        return "CHTL Compiler";
    }
    
    std::vector<FragmentType> getSupportedTypes() const override {
        return {FragmentType::CHTL};
    }
    
private:
    bool m_debugMode = false;
    
    std::string compileCHTLToHTML(const std::string& chtlCode) {
        std::ostringstream html;
        
        // 基本CHTL到HTML转换
        std::string processedCode = chtlCode;
        
        // 处理模板展开
        processedCode = expandTemplates(processedCode);
        
        // 处理自定义组件
        processedCode = expandCustomComponents(processedCode);
        
        // 处理元素节点
        processedCode = convertElementsToHTML(processedCode);
        
        // 处理文本节点
        processedCode = convertTextNodesToHTML(processedCode);
        
        // 处理属性
        processedCode = convertAttributesToHTML(processedCode);
        
        return processedCode;
    }
    
    std::string extractCSSFromCHTL(const std::string& chtlCode) {
        std::ostringstream css;
        
        // 提取局部样式块
        std::regex styleBlockRegex(R"(style\s*\{([^}]*)\})");
        std::sregex_iterator iter(chtlCode.begin(), chtlCode.end(), styleBlockRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string styleContent = (*iter)[1].str();
            css << styleContent << "\n";
        }
        
        return css.str();
    }
    
    std::string expandTemplates(const std::string& code) {
        // 模板展开逻辑
        std::string result = code;
        
        // 处理@Style模板使用
        std::regex templateStyleRegex(R"(@Style\s+(\w+))");
        result = std::regex_replace(result, templateStyleRegex, "/* Template: $1 */");
        
        // 处理@Element模板使用
        std::regex templateElementRegex(R"(@Element\s+(\w+))");
        result = std::regex_replace(result, templateElementRegex, "<!-- Template: $1 -->");
        
        return result;
    }
    
    std::string expandCustomComponents(const std::string& code) {
        // 自定义组件展开逻辑
        std::string result = code;
        
        // 处理索引访问
        std::regex indexAccessRegex(R"((\w+)\[(\d+)\])");
        result = std::regex_replace(result, indexAccessRegex, "$1_$2");
        
        return result;
    }
    
    std::string convertElementsToHTML(const std::string& code) {
        std::string result = code;
        
        // 简单的元素转换
        std::regex elementRegex(R"((\w+)\s*\{([^}]*)\})");
        std::sregex_iterator iter(result.begin(), result.end(), elementRegex);
        std::sregex_iterator end;
        
        std::ostringstream html;
        size_t lastPos = 0;
        
        for (; iter != end; ++iter) {
            // 添加匹配前的内容
            html << result.substr(lastPos, iter->position() - lastPos);
            
            std::string tagName = (*iter)[1].str();
            std::string content = (*iter)[2].str();
            
            html << "<" << tagName << ">" << content << "</" << tagName << ">";
            
            lastPos = iter->position() + iter->length();
        }
        
        // 添加剩余内容
        html << result.substr(lastPos);
        
        return html.str();
    }
    
    std::string convertTextNodesToHTML(const std::string& code) {
        std::string result = code;
        
        // 处理text{}节点
        std::regex textRegex(R"(text\s*\{\s*([^}]*)\s*\})");
        result = std::regex_replace(result, textRegex, "$1");
        
        return result;
    }
    
    std::string convertAttributesToHTML(const std::string& code) {
        std::string result = code;
        
        // 处理属性语法
        std::regex attrRegex(R"((\w+)\s*:\s*([^;]+);)");
        result = std::regex_replace(result, attrRegex, "$1=\"$2\"");
        
        return result;
    }
    
    void reportError(const std::string& message) {
        ErrorReporter reporter;
        reporter.error("CHTLCompiler", message);
    }
};

class CHTLJSCompilerImpl : public ICompiler {
public:
    CompilationResult compile(const CodeFragment& fragment) override {
        CompilationResult result;
        result.success = true;
        result.outputType = "javascript";
        
        try {
            // CHTL JS编译逻辑
            std::string jsOutput = compileCHTLJSToJS(fragment.content);
            
            result.output = jsOutput;
            result.fragmentType = fragment.type;
            
            if (m_debugMode) {
                std::cout << "  ✓ CHTL JS编译完成: " << jsOutput.length() << " 字符JS" << std::endl;
            }
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = "CHTL JS编译错误: " + std::string(e.what());
            reportError(result.errorMessage);
        }
        
        return result;
    }
    
    std::string getCompilerName() const override {
        return "CHTL JS Compiler";
    }
    
    std::vector<FragmentType> getSupportedTypes() const override {
        return {FragmentType::CHTL_JS};
    }
    
private:
    bool m_debugMode = false;
    
    std::string compileCHTLJSToJS(const std::string& chtlJSCode) {
        std::ostringstream js;
        
        std::string processedCode = chtlJSCode;
        
        // 处理增强选择器
        processedCode = convertEnhancedSelectors(processedCode);
        
        // 处理事件绑定
        processedCode = convertEventBinding(processedCode);
        
        // 处理动画块
        processedCode = convertAnimateBlocks(processedCode);
        
        // 处理虚拟对象
        processedCode = convertVirtualObjects(processedCode);
        
        // 处理模块导入
        processedCode = convertModuleImports(processedCode);
        
        return processedCode;
    }
    
    std::string convertEnhancedSelectors(const std::string& code) {
        std::string result = code;
        
        // {{.box}} -> document.querySelector('.box')
        std::regex classSelector(R"(\{\{\.(\w+)\}\})");
        result = std::regex_replace(result, classSelector, "document.querySelector('.$1')");
        
        // {{#box}} -> document.getElementById('box')
        std::regex idSelector(R"(\{\{#(\w+)\}\})");
        result = std::regex_replace(result, idSelector, "document.getElementById('$1')");
        
        // {{button[0]}} -> document.querySelectorAll('button')[0]
        std::regex indexSelector(R"(\{\{(\w+)\[(\d+)\]\}\})");
        result = std::regex_replace(result, indexSelector, "document.querySelectorAll('$1')[$2]");
        
        // {{button}} -> document.querySelectorAll('button')
        std::regex generalSelector(R"(\{\{(\w+)\}\})");
        result = std::regex_replace(result, generalSelector, "document.querySelectorAll('$1')");
        
        return result;
    }
    
    std::string convertEventBinding(const std::string& code) {
        std::string result = code;
        
        // &-> click {} -> addEventListener('click', function() {})
        std::regex eventBindingRegex(R"(&->\s*(\w+)\s*\{([^}]*)\})");
        result = std::regex_replace(result, eventBindingRegex, "addEventListener('$1', function() { $2 })");
        
        return result;
    }
    
    std::string convertAnimateBlocks(const std::string& code) {
        std::string result = code;
        
        // animate {} -> 转换为requestAnimationFrame
        std::regex animateRegex(R"(animate\s*\{([^}]*)\})");
        std::sregex_iterator iter(result.begin(), result.end(), animateRegex);
        std::sregex_iterator end;
        
        std::ostringstream converted;
        size_t lastPos = 0;
        
        for (; iter != end; ++iter) {
            converted << result.substr(lastPos, iter->position() - lastPos);
            
            std::string animateContent = (*iter)[1].str();
            converted << "requestAnimationFrame(function() { /* " << animateContent << " */ });";
            
            lastPos = iter->position() + iter->length();
        }
        
        converted << result.substr(lastPos);
        
        return converted.str();
    }
    
    std::string convertVirtualObjects(const std::string& code) {
        std::string result = code;
        
        // vir obj = listen {} -> const obj = {}
        std::regex virRegex(R"(vir\s+(\w+)\s*=\s*listen\s*\{([^}]*)\})");
        result = std::regex_replace(result, virRegex, "const $1 = { $2 };");
        
        return result;
    }
    
    std::string convertModuleImports(const std::string& code) {
        std::string result = code;
        
        // module { load: ... } -> 转换为动态导入
        std::regex moduleRegex(R"(module\s*\{([^}]*)\})");
        std::sregex_iterator iter(result.begin(), result.end(), moduleRegex);
        std::sregex_iterator end;
        
        std::ostringstream converted;
        size_t lastPos = 0;
        
        for (; iter != end; ++iter) {
            converted << result.substr(lastPos, iter->position() - lastPos);
            
            std::string moduleContent = (*iter)[1].str();
            converted << "/* AMD Module: " << moduleContent << " */";
            
            lastPos = iter->position() + iter->length();
        }
        
        converted << result.substr(lastPos);
        
        return converted.str();
    }
    
    void reportError(const std::string& message) {
        ErrorReporter reporter;
        reporter.error("CHTLJSCompiler", message);
    }
};

// CompilerDispatcher实现
CompilerDispatcher::CompilerDispatcher() {
    // 注册编译器
    m_compilers[FragmentType::CHTL] = std::make_unique<CHTLCompilerImpl>();
    m_compilers[FragmentType::CHTL_JS] = std::make_unique<CHTLJSCompilerImpl>();
    
    // 初始化模块管理器
    m_moduleManager = std::make_unique<ModuleManager>();
    m_cmodApi = std::make_unique<CMODApi>();
    m_cjmodApi = std::make_unique<CJMODApi>();
    
    std::cout << "✓ 编译器调度器初始化完成" << std::endl;
}

CompilerDispatcher::~CompilerDispatcher() = default;

std::vector<CompilationResult> CompilerDispatcher::compileCode(const std::string& code) {
    if (m_debugMode) {
        std::cout << "🚀 开始编译调度，代码长度: " << code.length() << std::endl;
    }
    
    try {
        // 第一阶段：统一扫描
        UnifiedScanner scanner;
        scanner.enableDebugMode(m_debugMode);
        std::vector<CodeFragment> fragments = scanner.scanCode(code);
        
        if (m_debugMode) {
            std::cout << "  ✓ 扫描完成，生成 " << fragments.size() << " 个片段" << std::endl;
        }
        
        // 第二阶段：优化编译顺序
        optimizeCompilationOrder(fragments);
        
        // 第三阶段：分阶段编译
        return compileFragments(fragments);
        
    } catch (const std::exception& e) {
        reportError("编译调度过程中发生错误: " + std::string(e.what()));
        return {};
    }
}

void CompilerDispatcher::optimizeCompilationOrder(std::vector<CodeFragment>& fragments) {
    // 按照语法文档要求的编译优先级排序：
    // 1. CHTL片段（最高优先级）
    // 2. CHTL JS片段
    // 3. CSS片段
    // 4. JavaScript片段
    
    std::sort(fragments.begin(), fragments.end(), 
        [](const CodeFragment& a, const CodeFragment& b) {
            int priorityA = getCompilationPriority(a.type);
            int priorityB = getCompilationPriority(b.type);
            
            if (priorityA != priorityB) {
                return priorityA < priorityB;  // 数字越小优先级越高
            }
            
            // 相同优先级按位置排序
            return a.startPos < b.startPos;
        });
    
    if (m_debugMode) {
        std::cout << "  ✓ 编译顺序优化完成" << std::endl;
    }
}

int CompilerDispatcher::getCompilationPriority(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return 1;
        case FragmentType::CHTL_JS: return 2;
        case FragmentType::CSS: return 3;
        case FragmentType::JAVASCRIPT: return 4;
        case FragmentType::HTML: return 5;
        case FragmentType::COMMENT_GENERATOR: return 0;  // 最高优先级
        case FragmentType::COMMENT_NORMAL: return 6;
        default: return 7;
    }
}

std::vector<CompilationResult> CompilerDispatcher::compileFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<CompilationResult> results;
    
    // 第一阶段：处理CHTL和CHTL JS片段
    std::vector<CompilationResult> chtlResults;
    std::vector<CompilationResult> chtlJSResults;
    std::vector<CodeFragment> cssFragments;
    std::vector<CodeFragment> jsFragments;
    std::vector<CodeFragment> otherFragments;
    
    if (m_debugMode) {
        std::cout << "  📋 第一阶段：分类和处理CHTL/CHTL JS片段" << std::endl;
    }
    
    for (const auto& fragment : fragments) {
        switch (fragment.type) {
            case FragmentType::CHTL: {
                auto result = compileFragment(fragment);
                chtlResults.push_back(result);
                break;
            }
            case FragmentType::CHTL_JS: {
                auto result = compileFragment(fragment);
                chtlJSResults.push_back(result);
                break;
            }
            case FragmentType::CSS:
                cssFragments.push_back(fragment);
                break;
            case FragmentType::JAVASCRIPT:
                jsFragments.push_back(fragment);
                break;
            default:
                otherFragments.push_back(fragment);
                break;
        }
    }
    
    // 添加CHTL和CHTL JS结果
    results.insert(results.end(), chtlResults.begin(), chtlResults.end());
    results.insert(results.end(), chtlJSResults.begin(), chtlJSResults.end());
    
    if (m_debugMode) {
        std::cout << "    ✓ CHTL片段: " << chtlResults.size() << " 个" << std::endl;
        std::cout << "    ✓ CHTL JS片段: " << chtlJSResults.size() << " 个" << std::endl;
    }
    
    // 第二阶段：合并CHTL和CHTL JS输出，然后处理CSS和JS
    if (!cssFragments.empty() || !jsFragments.empty()) {
        if (m_debugMode) {
            std::cout << "  📋 第二阶段：合并输出并处理CSS/JS片段" << std::endl;
        }
        
        std::string mergedCSS = mergeCSS(chtlResults);
        std::string mergedJS = mergeJavaScript(chtlJSResults);
        
        // 处理CSS片段
        for (auto& cssFragment : cssFragments) {
            cssFragment.content = mergedCSS + "\n" + cssFragment.content;
            auto result = compileFragment(cssFragment);
            results.push_back(result);
        }
        
        // 处理JavaScript片段
        for (auto& jsFragment : jsFragments) {
            jsFragment.content = mergedJS + "\n" + jsFragment.content;
            auto result = compileFragment(jsFragment);
            results.push_back(result);
        }
        
        if (m_debugMode) {
            std::cout << "    ✓ CSS片段: " << cssFragments.size() << " 个" << std::endl;
            std::cout << "    ✓ JS片段: " << jsFragments.size() << " 个" << std::endl;
        }
    }
    
    // 处理其他片段
    for (const auto& fragment : otherFragments) {
        auto result = compileFragment(fragment);
        results.push_back(result);
    }
    
    if (m_debugMode) {
        std::cout << "  ✓ 分阶段编译完成，总结果: " << results.size() << " 个" << std::endl;
    }
    
    return results;
}

CompilationResult CompilerDispatcher::compileFragment(const CodeFragment& fragment) {
    CompilationResult result;
    result.fragmentType = fragment.type;
    
    // 查找对应的编译器
    auto it = m_compilers.find(fragment.type);
    if (it != m_compilers.end()) {
        result = it->second->compile(fragment);
    } else {
        // 使用默认处理
        result.success = true;
        result.output = fragment.content;
        result.outputType = "text";
        
        if (m_debugMode) {
            std::cout << "  ⚠️ 使用默认处理: " << static_cast<int>(fragment.type) << std::endl;
        }
    }
    
    return result;
}

std::string CompilerDispatcher::mergeCSS(const std::vector<CompilationResult>& chtlResults) {
    std::ostringstream mergedCSS;
    
    mergedCSS << "/* CHTL生成的CSS */\n";
    
    for (const auto& result : chtlResults) {
        if (result.success && result.additionalOutputs.count("css")) {
            mergedCSS << result.additionalOutputs.at("css") << "\n";
        }
    }
    
    return mergedCSS.str();
}

std::string CompilerDispatcher::mergeJavaScript(const std::vector<CompilationResult>& chtlJSResults) {
    std::ostringstream mergedJS;
    
    mergedJS << "/* CHTL JS生成的JavaScript */\n";
    mergedJS << "(function() {\n";  // IIFE避免全局污染
    
    for (const auto& result : chtlJSResults) {
        if (result.success) {
            mergedJS << "  " << result.output << "\n";
        }
    }
    
    mergedJS << "})();\n";
    
    return mergedJS.str();
}

bool CompilerDispatcher::loadModule(const std::string& modulePath) {
    if (!m_moduleManager) {
        reportError("模块管理器未初始化");
        return false;
    }
    
    return m_moduleManager->loadModule(modulePath);
}

bool CompilerDispatcher::loadCMOD(const std::string& cmodPath) {
    if (!m_cmodApi) {
        reportError("CMOD API未初始化");
        return false;
    }
    
    return m_cmodApi->loadCMOD(cmodPath);
}

bool CompilerDispatcher::loadCJMOD(const std::string& cjmodPath) {
    if (!m_cjmodApi) {
        reportError("CJMOD API未初始化");
        return false;
    }
    
    return m_cjmodApi->loadCJMOD(cjmodPath);
}

void CompilerDispatcher::registerCompiler(FragmentType type, std::unique_ptr<ICompiler> compiler) {
    m_compilers[type] = std::move(compiler);
    
    if (m_debugMode) {
        std::cout << "✓ 注册编译器: " << static_cast<int>(type) << std::endl;
    }
}

std::vector<std::string> CompilerDispatcher::getLoadedModules() const {
    if (m_moduleManager) {
        return m_moduleManager->getLoadedModules();
    }
    return {};
}

void CompilerDispatcher::enableDebugMode(bool enable) {
    m_debugMode = enable;
    
    // 为所有编译器启用调试模式
    for (auto& pair : m_compilers) {
        // 这里可以为每个编译器设置调试模式
        // 由于接口限制，暂时通过其他方式处理
    }
}

void CompilerDispatcher::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error("CompilerDispatcher", message);
}

void CompilerDispatcher::reportWarning(const std::string& message) {
    ErrorReporter reporter;
    reporter.warning("CompilerDispatcher", message);
}

}