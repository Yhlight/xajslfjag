#include "JSCompiler.h"
#include <regex>
#include <fstream>
#include <sstream>

namespace CHTL {
namespace JS {

// JSCompiler实现
JSCompiler::JSCompiler(const JSCompilerConfig& config) : config(config) {
}

JSCompileResult JSCompiler::compile(const String& source) {
    JSCompileResult result;
    clearErrors();
    
    try {
        String processed = source;
        
        // 预处理
        processed = preprocessJS(processed);
        
        // 语法验证
        if (config.strictMode && !validateSyntax(processed)) {
            result.success = false;
            result.errors = errors;
            return result;
        }
        
        // ES6转译
        if (config.targetVersion == "ES5") {
            processed = transpileES6(processed);
        }
        
        // 模块处理
        if (config.enableModules) {
            processed = processModules(processed);
        }
        
        // 代码优化
        if (config.enableOptimization) {
            processed = optimizeCode(processed);
        }
        
        // 后处理
        processed = postprocessJS(processed);
        
        // 压缩
        if (config.minifyOutput) {
            processed = minifyJS(processed);
        }
        
        // 生成源映射
        if (config.enableSourceMaps) {
            result.sourceMap = generateSourceMap(source, processed);
        }
        
        result.javascript = processed;
        result.errors = errors;
        result.warnings = warnings;
        result.success = errors.empty();
        
    } catch (const std::exception& e) {
        addError("JavaScript编译异常: " + String(e.what()));
        result.success = false;
        result.errors = errors;
    }
    
    return result;
}

JSCompileResult JSCompiler::compileFile(const String& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        JSCompileResult result;
        result.errors.push_back("无法打开文件: " + filepath);
        result.success = false;
        return result;
    }
    
    String content((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
    file.close();
    
    return compile(content);
}

String JSCompiler::compileToString(const String& source) {
    auto result = compile(source);
    return result.success ? result.javascript : "";
}

bool JSCompiler::validateSyntax(const String& source) {
    // 简化的JavaScript语法验证
    StringVector syntaxErrors = checkSyntax(source);
    for (const auto& error : syntaxErrors) {
        addError(error);
    }
    return syntaxErrors.empty();
}

StringVector JSCompiler::checkSyntax(const String& source) {
    return findSyntaxErrors(source);
}

void JSCompiler::setConfig(const JSCompilerConfig& config) {
    this->config = config;
}

const JSCompilerConfig& JSCompiler::getConfig() const {
    return config;
}

const StringVector& JSCompiler::getErrors() const {
    return errors;
}

const StringVector& JSCompiler::getWarnings() const {
    return warnings;
}

bool JSCompiler::hasErrors() const {
    return !errors.empty();
}

String JSCompiler::getErrorSummary() const {
    if (errors.empty()) return "没有错误";
    
    String summary = "JavaScript编译错误 (" + std::to_string(errors.size()) + " 个):\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        summary += std::to_string(i + 1) + ". " + errors[i] + "\n";
    }
    return summary;
}

void JSCompiler::clearErrors() {
    errors.clear();
    warnings.clear();
}

String JSCompiler::preprocessJS(const String& source) {
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

String JSCompiler::transpileES6(const String& source) {
    String transpiled = source;
    
    // 转换箭头函数
    transpiled = convertArrowFunctions(transpiled);
    
    // 转换let/const
    transpiled = convertLetConst(transpiled);
    
    // 转换模板字符串
    transpiled = convertTemplateStrings(transpiled);
    
    // 转换类
    transpiled = convertClasses(transpiled);
    
    // 转换模块导入
    transpiled = convertModuleImports(transpiled);
    
    // 转换async/await
    if (config.enableAsync) {
        transpiled = convertAsyncAwait(transpiled);
    }
    
    return transpiled;
}

String JSCompiler::processModules(const String& source) {
    String processed = source;
    
    if (config.moduleFormat == "AMD") {
        // 简化的AMD模块包装
        if (source.find("import") != String::npos || source.find("export") != String::npos) {
            processed = "define(function(require, exports, module) {\n" + processed + "\n});";
            addWarning("应用AMD模块包装");
        }
    } else if (config.moduleFormat == "CommonJS") {
        // CommonJS处理
        addWarning("CommonJS模块格式处理 (简化实现)");
    }
    
    return processed;
}

String JSCompiler::optimizeCode(const String& source) {
    String optimized = source;
    
    // 死代码消除
    optimized = deadCodeElimination(optimized);
    
    // 常量折叠
    optimized = constantFolding(optimized);
    
    // 变量重命名（仅在压缩模式下）
    if (config.minifyOutput) {
        optimized = variableRenaming(optimized);
    }
    
    return optimized;
}

String JSCompiler::postprocessJS(const String& source) {
    String processed = source;
    
    // 清理多余的空白行
    processed = std::regex_replace(processed, std::regex(R"(\n\s*\n\s*\n)"), "\n\n");
    
    // 标准化缩进（非压缩模式）
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

String JSCompiler::minifyJS(const String& source) {
    String minified = source;
    
    // 移除注释
    if (!config.preserveComments) {
        minified = std::regex_replace(minified, std::regex(R"(//[^\n]*\n)"), "\n");
        minified = std::regex_replace(minified, std::regex(R"(/\*.*?\*/)"), "");
    }
    
    // 移除多余空白
    minified = std::regex_replace(minified, std::regex(R"(\s+)"), " ");
    
    // 移除分号前后的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除大括号前后的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*\{\s*)"), "{");
    minified = std::regex_replace(minified, std::regex(R"(\s*\}\s*)"), "}");
    
    // 移除操作符前后的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*=\s*)"), "=");
    minified = std::regex_replace(minified, std::regex(R"(\s*\+\s*)"), "+");
    minified = std::regex_replace(minified, std::regex(R"(\s*-\s*)"), "-");
    
    return minified;
}

String JSCompiler::generateSourceMap(const String& original, const String& compiled) {
    // 简化的源映射生成
    // 实际实现需要完整的源映射规范
    return "//# sourceMappingURL=compiled.js.map";
}

bool JSCompiler::isValidJavaScript(const String& source) {
    StringVector errors = findSyntaxErrors(source);
    return errors.empty();
}

StringVector JSCompiler::findSyntaxErrors(const String& source) {
    StringVector syntaxErrors;
    
    // 简化的语法检查
    // 检查括号匹配
    int parenCount = 0;
    int braceCount = 0;
    int bracketCount = 0;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < source.length(); ++i) {
        char c = source[i];
        
        if (!inString) {
            if (c == '"' || c == '\'' || c == '`') {
                inString = true;
                stringChar = c;
            } else if (c == '(') {
                parenCount++;
            } else if (c == ')') {
                parenCount--;
                if (parenCount < 0) {
                    syntaxErrors.push_back("第" + std::to_string(i) + "位置: 多余的右括号");
                }
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount < 0) {
                    syntaxErrors.push_back("第" + std::to_string(i) + "位置: 多余的右大括号");
                }
            } else if (c == '[') {
                bracketCount++;
            } else if (c == ']') {
                bracketCount--;
                if (bracketCount < 0) {
                    syntaxErrors.push_back("第" + std::to_string(i) + "位置: 多余的右方括号");
                }
            }
        } else {
            if (c == stringChar && (i == 0 || source[i-1] != '\\')) {
                inString = false;
            }
        }
    }
    
    if (parenCount != 0) {
        syntaxErrors.push_back("括号不匹配");
    }
    if (braceCount != 0) {
        syntaxErrors.push_back("大括号不匹配");
    }
    if (bracketCount != 0) {
        syntaxErrors.push_back("方括号不匹配");
    }
    if (inString) {
        syntaxErrors.push_back("字符串未正确结束");
    }
    
    return syntaxErrors;
}

// 转换方法的简化实现
String JSCompiler::convertArrowFunctions(const String& source) {
    String converted = source;
    
    // 简化的箭头函数转换
    std::regex arrowPattern(R"((\w+)\s*=>\s*(.+))");
    converted = std::regex_replace(converted, arrowPattern, "function($1) { return $2; }");
    
    return converted;
}

String JSCompiler::convertLetConst(const String& source) {
    String converted = source;
    
    // 将let和const转换为var
    converted = std::regex_replace(converted, std::regex(R"(\blet\b)"), "var");
    converted = std::regex_replace(converted, std::regex(R"(\bconst\b)"), "var");
    
    return converted;
}

String JSCompiler::convertTemplateStrings(const String& source) {
    String converted = source;
    
    // 简化的模板字符串转换
    if (source.find('`') != String::npos) {
        addWarning("检测到模板字符串，需要转换 (简化实现)");
    }
    
    return converted;
}

String JSCompiler::convertClasses(const String& source) {
    String converted = source;
    
    // 简化的类转换
    if (source.find("class ") != String::npos) {
        addWarning("检测到ES6类，需要转换 (简化实现)");
    }
    
    return converted;
}

String JSCompiler::convertModuleImports(const String& source) {
    String converted = source;
    
    // 简化的模块导入转换
    if (source.find("import ") != String::npos) {
        addWarning("检测到ES6模块导入，需要转换 (简化实现)");
    }
    
    return converted;
}

String JSCompiler::convertAsyncAwait(const String& source) {
    String converted = source;
    
    // 简化的async/await转换
    if (source.find("async ") != String::npos || source.find("await ") != String::npos) {
        addWarning("检测到async/await，需要转换 (简化实现)");
    }
    
    return converted;
}

// 优化方法的简化实现
String JSCompiler::deadCodeElimination(const String& source) {
    String optimized = source;
    
    // 移除永远不会执行的代码
    optimized = std::regex_replace(optimized, std::regex(R"(if\s*\(\s*false\s*\)\s*\{[^}]*\})"), "");
    
    return optimized;
}

String JSCompiler::constantFolding(const String& source) {
    String optimized = source;
    
    // 简化的常量折叠
    optimized = std::regex_replace(optimized, std::regex(R"(1\s*\+\s*1)"), "2");
    optimized = std::regex_replace(optimized, std::regex(R"(2\s*\*\s*3)"), "6");
    
    return optimized;
}

String JSCompiler::variableRenaming(const String& source) {
    String renamed = source;
    
    // 简化的变量重命名（仅用于演示）
    if (config.minifyOutput) {
        addWarning("变量重命名优化 (简化实现)");
    }
    
    return renamed;
}

void JSCompiler::addError(const String& message) {
    errors.push_back(message);
}

void JSCompiler::addWarning(const String& message) {
    warnings.push_back(message);
}

// JSCompilerFactory实现
std::unique_ptr<JSCompiler> JSCompilerFactory::createCompiler(const JSCompilerConfig& config) {
    return std::make_unique<JSCompiler>(config);
}

std::unique_ptr<JSCompiler> JSCompilerFactory::createES5Compiler() {
    return createCompiler(getES5Config());
}

std::unique_ptr<JSCompiler> JSCompilerFactory::createES6Compiler() {
    return createCompiler(getES6Config());
}

std::unique_ptr<JSCompiler> JSCompilerFactory::createMinifyingCompiler() {
    return createCompiler(getMinifyConfig());
}

std::unique_ptr<JSCompiler> JSCompilerFactory::createDevelopmentCompiler() {
    return createCompiler(getDevelopmentConfig());
}

std::unique_ptr<JSCompiler> JSCompilerFactory::createProductionCompiler() {
    return createCompiler(getProductionConfig());
}

JSCompilerConfig JSCompilerFactory::getDefaultConfig() {
    return JSCompilerConfig{};
}

JSCompilerConfig JSCompilerFactory::getES5Config() {
    JSCompilerConfig config;
    config.targetVersion = "ES5";
    config.enableModules = false;
    config.enableAsync = false;
    return config;
}

JSCompilerConfig JSCompilerFactory::getES6Config() {
    JSCompilerConfig config;
    config.targetVersion = "ES2015";
    config.enableModules = true;
    config.enableAsync = true;
    return config;
}

JSCompilerConfig JSCompilerFactory::getMinifyConfig() {
    JSCompilerConfig config;
    config.minifyOutput = true;
    config.preserveComments = false;
    config.enableOptimization = true;
    return config;
}

JSCompilerConfig JSCompilerFactory::getDevelopmentConfig() {
    JSCompilerConfig config;
    config.preserveComments = true;
    config.enableSourceMaps = true;
    config.strictMode = false;
    return config;
}

JSCompilerConfig JSCompilerFactory::getProductionConfig() {
    JSCompilerConfig config;
    config.minifyOutput = true;
    config.preserveComments = false;
    config.enableOptimization = true;
    config.enableSourceMaps = false;
    config.strictMode = true;
    return config;
}

} // namespace JS
} // namespace CHTL