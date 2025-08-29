#include "CJMODGenerator.h"
#include <regex>
#include <sstream>
#include <chrono>
#include <random>

namespace CJMOD {

// 静态成员初始化
GeneratorConfig CJMODGenerator::defaultConfig;

// 静态方法实现
GenerationResult CJMODGenerator::exportResult(const Arg& args) {
    CJMODGenerator generator(defaultConfig);
    return generator.generate(args);
}

GenerationResult CJMODGenerator::exportResult(const Arg& args, const GeneratorConfig& config) {
    CJMODGenerator generator(config);
    return generator.generate(args);
}

GenerationResult CJMODGenerator::exportMultiple(const std::vector<Arg>& argsList) {
    CJMODGenerator generator(defaultConfig);
    return generator.generateBatch(argsList);
}

GenerationResult CJMODGenerator::exportMultiple(const std::vector<Arg>& argsList, const GeneratorConfig& config) {
    CJMODGenerator generator(config);
    return generator.generateBatch(argsList);
}

void CJMODGenerator::setDefaultConfig(const GeneratorConfig& config) {
    defaultConfig = config;
}

GeneratorConfig CJMODGenerator::getDefaultConfig() {
    return defaultConfig;
}

// 构造函数和实例方法
CJMODGenerator::CJMODGenerator(const GeneratorConfig& config) : config(config) {}

GenerationResult CJMODGenerator::generate(const Arg& args) {
    return generateFromArg(args);
}

GenerationResult CJMODGenerator::generateBatch(const std::vector<Arg>& argsList) {
    GenerationResult batchResult;
    batchResult.success = true;
    
    std::ostringstream combinedCode;
    
    for (size_t i = 0; i < argsList.size(); ++i) {
        GenerationResult singleResult = generateFromArg(argsList[i]);
        
        if (!singleResult.success) {
            batchResult.success = false;
            // 添加错误信息，包含索引
            for (const auto& error : singleResult.errors) {
                batchResult.addError("Arg[" + std::to_string(i) + "]: " + error);
            }
        }
        
        // 合并警告
        for (const auto& warning : singleResult.warnings) {
            batchResult.addWarning("Arg[" + std::to_string(i) + "]: " + warning);
        }
        
        // 合并代码
        if (!singleResult.jsCode.empty()) {
            combinedCode << singleResult.jsCode;
            if (i < argsList.size() - 1) {
                combinedCode << "\n";
            }
        }
    }
    
    batchResult.jsCode = combinedCode.str();
    
    // 应用批处理后的格式化
    if (batchResult.success && !batchResult.jsCode.empty()) {
        batchResult.jsCode = formatCode(batchResult.jsCode);
        
        if (config.enableMinification) {
            batchResult.jsCode = minifyCode(batchResult.jsCode);
        }
    }
    
    return batchResult;
}

// 私有方法实现
GenerationResult CJMODGenerator::generateFromArg(const Arg& args) const {
    GenerationResult result;
    
    // 验证参数
    if (!validateArgument(args)) {
        result.addError("Invalid argument structure");
        return result;
    }
    
    try {
        // 生成基础JavaScript代码
        CHTL::String jsCode = generateJavaScriptCode(args);
        
        // 应用转换和格式化
        jsCode = transformToTargetFormat(jsCode);
        jsCode = formatCode(jsCode);
        
        if (config.enableOptimization) {
            jsCode = optimizeCode(jsCode);
        }
        
        if (config.enableMinification) {
            jsCode = minifyCode(jsCode);
        }
        
        if (config.strictMode) {
            jsCode = addStrictMode(jsCode);
        }
        
        if (config.enableDebugInfo) {
            jsCode = addDebugInfo(jsCode, args);
        }
        
        result.jsCode = jsCode;
        
        // 生成Source Map（如果启用）
        if (config.enableSourceMap) {
            result.sourceMap = generateSourceMap(args);
        }
        
        // 代码质量检查
        CHTL::StringVector qualityWarnings = checkCodeQuality(jsCode);
        for (const auto& warning : qualityWarnings) {
            result.addWarning(warning);
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.addError("Code generation failed: " + CHTL::String(e.what()));
    }
    
    return result;
}

CHTL::String CJMODGenerator::generateJavaScriptCode(const Arg& args) const {
    if (args.isTransformed) {
        return args.getTransformedResult();
    }
    
    // 默认生成：简单连接所有参数
    return args.toString();
}

CHTL::String CJMODGenerator::generateSourceMap(const Arg& args) const {
    // 简化的Source Map生成
    std::ostringstream sourceMap;
    sourceMap << "{\n";
    sourceMap << "  \"version\": 3,\n";
    sourceMap << "  \"sources\": [\"<cjmod-generated>\"],\n";
    sourceMap << "  \"names\": [],\n";
    sourceMap << "  \"mappings\": \"AAAA\",\n";
    sourceMap << "  \"sourcesContent\": [\"" << escapeString(args.toString()) << "\"]\n";
    sourceMap << "}";
    return sourceMap.str();
}

CHTL::String CJMODGenerator::formatCode(const CHTL::String& code) const {
    CHTL::String formatted = code;
    
    // 应用缩进
    formatted = applyIndentation(formatted);
    
    return formatted;
}

CHTL::String CJMODGenerator::minifyCode(const CHTL::String& code) const {
    CHTL::String minified = code;
    
    // 移除多余的空白
    minified = std::regex_replace(minified, std::regex(R"(\s+)"), " ");
    
    // 移除行尾空白
    minified = std::regex_replace(minified, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除注释
    minified = std::regex_replace(minified, std::regex(R"(//.*$)"), "", std::regex_constants::match_any);
    minified = std::regex_replace(minified, std::regex(R"(/\*.*?\*/)"), "", std::regex_constants::match_any);
    
    return minified;
}

CHTL::String CJMODGenerator::optimizeCode(const CHTL::String& code) const {
    CHTL::String optimized = code;
    
    // 简单的优化：合并连续的字符串
    optimized = std::regex_replace(optimized, std::regex(R"('([^']*)'\\s*\+\\s*'([^']*)')"), "'$1$2'");
    
    return optimized;
}

CHTL::String CJMODGenerator::addStrictMode(const CHTL::String& code) const {
    if (code.find("'use strict'") == CHTL::String::npos) {
        return "'use strict';\n" + code;
    }
    return code;
}

CHTL::String CJMODGenerator::generateES5(const CHTL::String& code) const {
    // 简化的ES6 -> ES5转换
    CHTL::String es5Code = code;
    
    // 转换箭头函数
    es5Code = std::regex_replace(es5Code, std::regex(R"(\([^)]*\)\s*=>\s*)"), "function$1");
    
    // 转换let/const为var
    es5Code = std::regex_replace(es5Code, std::regex(R"(\b(let|const)\b)"), "var");
    
    return es5Code;
}

CHTL::String CJMODGenerator::generateES6(const CHTL::String& code) const {
    return code; // ES6是默认格式
}

CHTL::String CJMODGenerator::generateCommonJS(const CHTL::String& code) const {
    return "module.exports = (function() {\n" + code + "\n})();";
}

CHTL::String CJMODGenerator::generateAMD(const CHTL::String& code) const {
    return "define(function() {\n" + code + "\n});";
}

CHTL::String CJMODGenerator::transformToTargetFormat(const CHTL::String& code) const {
    if (config.outputFormat == "es5") {
        return generateES5(code);
    } else if (config.outputFormat == "es6") {
        return generateES6(code);
    } else if (config.outputFormat == "commonjs") {
        return generateCommonJS(code);
    } else if (config.outputFormat == "amd") {
        return generateAMD(code);
    }
    return code;
}

CHTL::String CJMODGenerator::applyIndentation(const CHTL::String& code) const {
    CHTL::String indentStr = getIndentString();
    CHTL::String indented = code;
    
    // 简化的缩进处理
    indented = std::regex_replace(indented, std::regex("\\{"), "{\n" + indentStr);
    indented = std::regex_replace(indented, std::regex("\\}"), "\n}");
    indented = std::regex_replace(indented, std::regex(";"), ";\n" + indentStr);
    
    return indented;
}

CHTL::String CJMODGenerator::addDebugInfo(const CHTL::String& code, const Arg& args) const {
    std::ostringstream debug;
    debug << "/* CJMOD Generated Code */\n";
    debug << "/* Args: " << args.toDebugString() << " */\n";
    debug << code;
    return debug.str();
}

bool CJMODGenerator::validateArgument(const Arg& args) const {
    try {
        // 检查必需的占位符是否已填充
        if (args.hasUnfilledRequired()) {
            return false;
        }
        
        // 检查是否有未绑定的占位符（可选）
        // 这里只是警告，不影响生成
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

CHTL::StringVector CJMODGenerator::checkCodeQuality(const CHTL::String& code) const {
    CHTL::StringVector warnings;
    
    // 检查潜在问题
    if (code.find("eval(") != CHTL::String::npos) {
        warnings.push_back("Code contains eval(), which may be unsafe");
    }
    
    if (code.find("with(") != CHTL::String::npos) {
        warnings.push_back("Code contains with statement, which is deprecated");
    }
    
    // 检查代码长度
    if (code.length() > 10000) {
        warnings.push_back("Generated code is very long (" + std::to_string(code.length()) + " chars)");
    }
    
    return warnings;
}

bool CJMODGenerator::isValidJavaScript(const CHTL::String& code) const {
    // 简单的JavaScript语法检查
    int braceCount = 0;
    int parenCount = 0;
    
    for (char c : code) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        else if (c == '(') parenCount++;
        else if (c == ')') parenCount--;
    }
    
    return braceCount == 0 && parenCount == 0;
}

CHTL::String CJMODGenerator::getIndentString() const {
    if (config.indentStyle == "tabs") {
        return CHTL::String(config.indentSize, '\t');
    } else {
        return CHTL::String(config.indentSize, ' ');
    }
}

CHTL::String CJMODGenerator::escapeString(const CHTL::String& str) const {
    CHTL::String escaped = str;
    escaped = std::regex_replace(escaped, std::regex("\\\\"), "\\\\");
    escaped = std::regex_replace(escaped, std::regex("\""), "\\\"");
    escaped = std::regex_replace(escaped, std::regex("\n"), "\\n");
    escaped = std::regex_replace(escaped, std::regex("\r"), "\\r");
    escaped = std::regex_replace(escaped, std::regex("\t"), "\\t");
    return escaped;
}

CHTL::String CJMODGenerator::generateUniqueId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    return "cjmod_" + std::to_string(timestamp) + "_" + std::to_string(dis(gen));
}

void CJMODGenerator::addGenerationError(GenerationResult& result, const CHTL::String& error) const {
    result.addError(error);
}

void CJMODGenerator::addGenerationWarning(GenerationResult& result, const CHTL::String& warning) const {
    result.addWarning(warning);
}

// GeneratorFactory实现
std::unique_ptr<CJMODGenerator> GeneratorFactory::createGenerator(const GeneratorConfig& config) {
    return std::make_unique<CJMODGenerator>(config);
}

std::unique_ptr<CJMODGenerator> GeneratorFactory::createMinifyingGenerator() {
    return std::make_unique<CJMODGenerator>(getMinifyConfig());
}

std::unique_ptr<CJMODGenerator> GeneratorFactory::createDebugGenerator() {
    return std::make_unique<CJMODGenerator>(getDebugConfig());
}

std::unique_ptr<CJMODGenerator> GeneratorFactory::createProductionGenerator() {
    return std::make_unique<CJMODGenerator>(getProductionConfig());
}

GeneratorConfig GeneratorFactory::getMinifyConfig() {
    GeneratorConfig config;
    config.enableMinification = true;
    config.enableOptimization = true;
    config.enableSourceMap = false;
    config.enableDebugInfo = false;
    config.outputFormat = "es5";
    return config;
}

GeneratorConfig GeneratorFactory::getDebugConfig() {
    GeneratorConfig config;
    config.enableMinification = false;
    config.enableOptimization = false;
    config.enableSourceMap = true;
    config.enableDebugInfo = true;
    config.indentSize = 4;
    return config;
}

GeneratorConfig GeneratorFactory::getProductionConfig() {
    GeneratorConfig config;
    config.enableMinification = true;
    config.enableOptimization = true;
    config.enableSourceMap = true;
    config.enableDebugInfo = false;
    config.outputFormat = "es6";
    config.strictMode = true;
    return config;
}

// Utils命名空间实现
namespace Utils {

CHTL::String joinArgs(const Arg& args, const CHTL::String& separator) {
    std::ostringstream oss;
    for (size_t i = 0; i < args.size(); ++i) {
        oss << args[i].getValue();
        if (i < args.size() - 1) {
            oss << separator;
        }
    }
    return oss.str();
}

CHTL::String quotePlaceholders(const Arg& args) {
    Arg quoted = args;
    for (auto& atom : quoted.atoms) {
        if (!atom.getValue().empty() && atom.getValue()[0] != '"' && atom.getValue()[0] != '\'') {
            atom.fillValue("\"" + atom.getValue() + "\"");
        }
    }
    return quoted.toString();
}

bool isES6Compatible(const CHTL::String& code) {
    // 检查ES6特性
    return code.find("=>") != CHTL::String::npos ||
           code.find("let ") != CHTL::String::npos ||
           code.find("const ") != CHTL::String::npos ||
           code.find("class ") != CHTL::String::npos;
}

CHTL::String addCodeComments(const CHTL::String& code, const CHTL::StringVector& comments) {
    std::ostringstream oss;
    for (const auto& comment : comments) {
        oss << "// " << comment << "\n";
    }
    oss << code;
    return oss.str();
}

CHTL::String removeCodeComments(const CHTL::String& code) {
    CHTL::String result = code;
    result = std::regex_replace(result, std::regex(R"(//.*$)"), "", std::regex_constants::match_any);
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "", std::regex_constants::match_any);
    return result;
}

} // namespace Utils

} // namespace CJMOD