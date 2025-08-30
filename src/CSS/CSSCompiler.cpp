#include "CSSCompiler.h"
#include <regex>
#include <fstream>
#include <sstream>

namespace CHTL {
namespace CSS {

// CSSCompiler实现
CSSCompiler::CSSCompiler(const CSSCompilerConfig& config) : config(config) {
}

CSSCompileResult CSSCompiler::compile(const String& source) {
    CSSCompileResult result;
    clearErrors();
    
    try {
        String processed = source;
        
        // 预处理
        processed = preprocessCSS(processed);
        
        // 处理导入
        if (config.enableImports) {
            processed = processImports(processed);
        }
        
        // 处理变量
        if (config.enableVariables) {
            processed = processVariables(processed);
        }
        
        // 处理嵌套
        if (config.enableNesting) {
            processed = processNesting(processed);
        }
        
        // 添加浏览器前缀
        if (config.enableAutoprefixer) {
            processed = processAutoprefixer(processed);
        }
        
        // 后处理
        processed = postprocessCSS(processed);
        
        // 压缩
        if (config.minifyOutput) {
            processed = minifyCSS(processed);
        }
        
        result.css = processed;
        result.errors = errors;
        result.warnings = warnings;
        result.success = errors.empty();
        
    } catch (const std::exception& e) {
        addError("CSS编译异常: " + String(e.what()));
        result.success = false;
        result.errors = errors;
    }
    
    return result;
}

CSSCompileResult CSSCompiler::compileFile(const String& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        CSSCompileResult result;
        result.errors.push_back("无法打开文件: " + filepath);
        result.success = false;
        return result;
    }
    
    String content((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
    file.close();
    
    return compile(content);
}

String CSSCompiler::compileToString(const String& source) {
    auto result = compile(source);
    return result.success ? result.css : "";
}

void CSSCompiler::setConfig(const CSSCompilerConfig& config) {
    this->config = config;
}

const CSSCompilerConfig& CSSCompiler::getConfig() const {
    return config;
}

const StringVector& CSSCompiler::getErrors() const {
    return errors;
}

const StringVector& CSSCompiler::getWarnings() const {
    return warnings;
}

bool CSSCompiler::hasErrors() const {
    return !errors.empty();
}

String CSSCompiler::getErrorSummary() const {
    if (errors.empty()) return "没有错误";
    
    String summary = "CSS编译错误 (" + std::to_string(errors.size()) + " 个):\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        summary += std::to_string(i + 1) + ". " + errors[i] + "\n";
    }
    return summary;
}

void CSSCompiler::clearErrors() {
    errors.clear();
    warnings.clear();
}

String CSSCompiler::preprocessCSS(const String& source) {
    String processed = source;
    
    // 移除BOM
    if (processed.size() >= 3 && 
        processed.substr(0, 3) == "\xEF\xBB\xBF") {
        processed = processed.substr(3);
    }
    
    // 标准化换行符
    processed = std::regex_replace(processed, std::regex("\r\n"), "\n");
    processed = std::regex_replace(processed, std::regex("\r"), "\n");
    
    return processed;
}

String CSSCompiler::processImports(const String& source) {
    String processed = source;
    
    // 简化的@import处理
    std::regex importPattern(R"(@import\s+['"]([^'"]+)['"];?)");
    std::sregex_iterator iter(source.begin(), source.end(), importPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        String importPath = (*iter)[1].str();
        addWarning("@import处理: " + importPath + " (简化实现)");
    }
    
    return processed;
}

String CSSCompiler::processVariables(const String& source) {
    String processed = source;
    
    // 简化的CSS变量处理
    // 这里可以添加CSS自定义属性的处理逻辑
    std::regex varPattern(R"(:root\s*\{[^}]*\})");
    if (std::regex_search(source, varPattern)) {
        addWarning("检测到CSS变量定义");
    }
    
    return processed;
}

String CSSCompiler::processNesting(const String& source) {
    String processed = source;
    
    // 简化的嵌套处理
    // 实际实现需要完整的CSS解析器
    if (source.find("&") != String::npos) {
        addWarning("检测到嵌套语法 (简化实现)");
    }
    
    return processed;
}

String CSSCompiler::processAutoprefixer(const String& source) {
    String processed = source;
    
    // 简化的自动前缀添加
    // 为一些常见属性添加浏览器前缀
    processed = std::regex_replace(processed, std::regex(R"(transform\s*:)"), 
                                 "-webkit-transform: $&\n  -moz-transform: $&\n  -ms-transform: $&\n  transform:");
    
    processed = std::regex_replace(processed, std::regex(R"(transition\s*:)"), 
                                 "-webkit-transition: $&\n  -moz-transition: $&\n  transition:");
    
    return processed;
}

String CSSCompiler::postprocessCSS(const String& source) {
    String processed = source;
    
    // 清理多余的空白行
    processed = std::regex_replace(processed, std::regex(R"(\n\s*\n\s*\n)"), "\n\n");
    
    // 标准化缩进
    if (!config.minifyOutput) {
        std::istringstream iss(processed);
        std::ostringstream oss;
        String line;
        int braceLevel = 0;
        
        while (std::getline(iss, line)) {
            // 简化的缩进处理
            if (line.find('}') != String::npos) braceLevel--;
            
            for (int i = 0; i < braceLevel; ++i) {
                oss << config.indentString;
            }
            oss << line << config.newlineString;
            
            if (line.find('{') != String::npos) braceLevel++;
        }
        
        processed = oss.str();
    }
    
    return processed;
}

String CSSCompiler::minifyCSS(const String& source) {
    String minified = source;
    
    // 移除注释
    if (!config.preserveComments) {
        minified = std::regex_replace(minified, std::regex(R"(/\*.*?\*/)"), "");
    }
    
    // 移除多余空白
    minified = std::regex_replace(minified, std::regex(R"(\s+)"), " ");
    
    // 移除分号前后的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除大括号前后的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*\{\s*)"), "{");
    minified = std::regex_replace(minified, std::regex(R"(\s*\}\s*)"), "}");
    
    // 移除冒号前后的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*:\s*)"), ":");
    
    // 移除逗号后的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*,\s*)"), ",");
    
    return minified;
}

void CSSCompiler::addError(const String& message) {
    errors.push_back(message);
}

void CSSCompiler::addWarning(const String& message) {
    warnings.push_back(message);
}

// CSSCompilerFactory实现
std::unique_ptr<CSSCompiler> CSSCompilerFactory::createCompiler(const CSSCompilerConfig& config) {
    return std::make_unique<CSSCompiler>(config);
}

std::unique_ptr<CSSCompiler> CSSCompilerFactory::createMinifyingCompiler() {
    return createCompiler(getMinifyConfig());
}

std::unique_ptr<CSSCompiler> CSSCompilerFactory::createDevelopmentCompiler() {
    return createCompiler(getDevelopmentConfig());
}

std::unique_ptr<CSSCompiler> CSSCompilerFactory::createProductionCompiler() {
    return createCompiler(getProductionConfig());
}

CSSCompilerConfig CSSCompilerFactory::getDefaultConfig() {
    return CSSCompilerConfig{};
}

CSSCompilerConfig CSSCompilerFactory::getMinifyConfig() {
    CSSCompilerConfig config;
    config.minifyOutput = true;
    config.preserveComments = false;
    return config;
}

CSSCompilerConfig CSSCompilerFactory::getDevelopmentConfig() {
    CSSCompilerConfig config;
    config.preserveComments = true;
    config.strictMode = false;
    return config;
}

CSSCompilerConfig CSSCompilerFactory::getProductionConfig() {
    CSSCompilerConfig config;
    config.minifyOutput = true;
    config.preserveComments = false;
    config.enableAutoprefixer = true;
    config.strictMode = true;
    return config;
}

} // namespace CSS
} // namespace CHTL