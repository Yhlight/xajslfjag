#pragma once

#include "Syntax.h"
#include "../Util/Common.h"

namespace CJMOD {

// 生成结果结构
struct GenerationResult {
    bool success;
    CHTL::String jsCode;
    CHTL::String sourceMap;
    CHTL::StringVector errors;
    CHTL::StringVector warnings;
    CHTL::StringUnorderedMap metadata;
    
    GenerationResult() : success(false) {}
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    void addError(const CHTL::String& error) { 
        errors.push_back(error); 
        success = false; 
    }
    
    void addWarning(const CHTL::String& warning) { 
        warnings.push_back(warning); 
    }
    
    CHTL::String getErrorSummary() const {
        if (errors.empty()) return "No errors";
        CHTL::String summary = "Errors:\n";
        for (const auto& error : errors) {
            summary += "- " + error + "\n";
        }
        return summary;
    }
};

// 生成器配置
struct GeneratorConfig {
    bool enableSourceMap;
    bool enableMinification;
    bool enableOptimization;
    bool enableDebugInfo;
    CHTL::String outputFormat;  // "es5", "es6", "commonjs", "amd"
    CHTL::String indentStyle;   // "spaces", "tabs"
    int indentSize;
    bool strictMode;
    
    GeneratorConfig() 
        : enableSourceMap(false), enableMinification(false), 
          enableOptimization(true), enableDebugInfo(false),
          outputFormat("es6"), indentStyle("spaces"), 
          indentSize(2), strictMode(true) {}
};

// CJMODGenerator类 - 生成器
class CJMODGenerator {
public:
    static GeneratorConfig defaultConfig;
    
    // 核心静态方法
    static GenerationResult exportResult(const Arg& args);
    static GenerationResult exportResult(const Arg& args, const GeneratorConfig& config);
    static GenerationResult exportMultiple(const std::vector<Arg>& argsList);
    static GenerationResult exportMultiple(const std::vector<Arg>& argsList, const GeneratorConfig& config);
    
    // 配置管理
    static void setDefaultConfig(const GeneratorConfig& config);
    static GeneratorConfig getDefaultConfig();
    
    // 实例方法
    explicit CJMODGenerator(const GeneratorConfig& config = defaultConfig);
    
    GenerationResult generate(const Arg& args);
    GenerationResult generateBatch(const std::vector<Arg>& argsList);
    
    // 配置
    void setConfig(const GeneratorConfig& config) { this->config = config; }
    GeneratorConfig getConfig() const { return config; }
    
    // 生成选项
    void enableSourceMap(bool enable) { config.enableSourceMap = enable; }
    void enableMinification(bool enable) { config.enableMinification = enable; }
    void enableOptimization(bool enable) { config.enableOptimization = enable; }
    void setOutputFormat(const CHTL::String& format) { config.outputFormat = format; }
    
private:
    GeneratorConfig config;
    
    // 核心生成方法
    GenerationResult generateFromArg(const Arg& args) const;
    CHTL::String generateJavaScriptCode(const Arg& args) const;
    CHTL::String generateSourceMap(const Arg& args) const;
    
    // 格式化和优化
    CHTL::String formatCode(const CHTL::String& code) const;
    CHTL::String minifyCode(const CHTL::String& code) const;
    CHTL::String optimizeCode(const CHTL::String& code) const;
    CHTL::String addStrictMode(const CHTL::String& code) const;
    
    // 不同输出格式
    CHTL::String generateES5(const CHTL::String& code) const;
    CHTL::String generateES6(const CHTL::String& code) const;
    CHTL::String generateCommonJS(const CHTL::String& code) const;
    CHTL::String generateAMD(const CHTL::String& code) const;
    
    // 代码转换
    CHTL::String transformToTargetFormat(const CHTL::String& code) const;
    CHTL::String applyIndentation(const CHTL::String& code) const;
    CHTL::String addDebugInfo(const CHTL::String& code, const Arg& args) const;
    
    // 验证和检查
    bool validateArgument(const Arg& args) const;
    CHTL::StringVector checkCodeQuality(const CHTL::String& code) const;
    bool isValidJavaScript(const CHTL::String& code) const;
    
    // 工具方法
    CHTL::String getIndentString() const;
    CHTL::String escapeString(const CHTL::String& str) const;
    CHTL::String generateUniqueId() const;
    
    // 错误处理
    void addGenerationError(GenerationResult& result, const CHTL::String& error) const;
    void addGenerationWarning(GenerationResult& result, const CHTL::String& warning) const;
};

// 工厂类
class GeneratorFactory {
public:
    static std::unique_ptr<CJMODGenerator> createGenerator(const GeneratorConfig& config = GeneratorConfig());
    static std::unique_ptr<CJMODGenerator> createMinifyingGenerator();
    static std::unique_ptr<CJMODGenerator> createDebugGenerator();
    static std::unique_ptr<CJMODGenerator> createProductionGenerator();
    
    // 预设配置
    static GeneratorConfig getMinifyConfig();
    static GeneratorConfig getDebugConfig();
    static GeneratorConfig getProductionConfig();
};

// 辅助函数
namespace Utils {
    CHTL::String joinArgs(const Arg& args, const CHTL::String& separator = " ");
    CHTL::String quotePlaceholders(const Arg& args);
    bool isES6Compatible(const CHTL::String& code);
    CHTL::String addCodeComments(const CHTL::String& code, const CHTL::StringVector& comments);
    CHTL::String removeCodeComments(const CHTL::String& code);
}

} // namespace CJMOD