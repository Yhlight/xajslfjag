#pragma once

#include "../../Util/Common.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/Context.h"

namespace CHTL {

// 生成器配置
struct GeneratorConfig {
    bool minifyOutput = false;
    bool includeComments = true;
    bool includeDebugInfo = false;
    bool enableOptimizations = true;
    bool generateSourceMaps = false;
    bool useAbsolutePaths = false;
    String indentString = "  ";
    String newlineString = "\n";
    String outputEncoding = "UTF-8";
    size_t maxLineLength = 120;
    
    // HTML特定配置
    bool formatHTML = true;
    bool includeDoctype = true;
    bool selfCloseVoidElements = true;
    bool lowercaseTagNames = true;
    bool lowercaseAttributeNames = true;
    
    // CSS特定配置
    bool formatCSS = true;
    bool includeCSSComments = true;
    bool optimizeCSS = true;
    bool addVendorPrefixes = false;
    bool includeSourceMapCSS = false;
    
    // JavaScript特定配置
    bool formatJS = true;
    bool includeJSComments = true;
    bool optimizeJS = false;
    bool addStrictMode = true;
    bool useESModules = true;
    bool includeSourceMapJS = false;
};

// 生成输出
struct GenerationOutput {
    String html;
    String css;
    String javascript;
    StringUnorderedMap additionalFiles;  // 其他生成的文件
    StringVector warnings;
    StringVector errors;
    double generationTime;
    size_t outputSize;
    bool success;
    
    GenerationOutput() : generationTime(0.0), outputSize(0), success(false) {}
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    size_t getTotalSize() const;
    String getSummary() const;
};

// CHTL生成器
class Generator {
public:
    explicit Generator(const GeneratorConfig& config = GeneratorConfig{});
    ~Generator() = default;
    
    // 主生成方法
    GenerationOutput generate(const BaseNode* ast);
    GenerationOutput generateDocument(const BaseNode* documentNode);
    GenerationOutput generateFragment(const BaseNode* fragmentNode);
    
    // 分离生成
    String generateHTML(const BaseNode* ast);
    String generateCSS(const BaseNode* ast);
    String generateJavaScript(const BaseNode* ast);
    
    // 特定节点生成
    String generateElement(const BaseNode* elementNode);
    String generateText(const BaseNode* textNode);
    String generateStyle(const BaseNode* styleNode);
    String generateScript(const BaseNode* scriptNode);
    String generateTemplate(const BaseNode* templateNode);
    String generateCustom(const BaseNode* customNode);
    String generateOrigin(const BaseNode* originNode);
    String generateImport(const BaseNode* importNode);
    String generateConfiguration(const BaseNode* configNode);
    String generateNamespace(const BaseNode* namespaceNode);
    
    // 模板和自定义实例化
    String instantiateTemplate(const BaseNode* templateNode, const StringUnorderedMap& parameters = {});
    String instantiateCustom(const BaseNode* customNode, const StringUnorderedMap& parameters = {});
    
    // 上下文管理
    void setContext(std::shared_ptr<Context> ctx) { context = ctx; }
    std::shared_ptr<Context> getContext() const { return context; }
    
    // 配置管理
    void setConfig(const GeneratorConfig& config) { this->config = config; }
    const GeneratorConfig& getConfig() const { return config; }
    
    // 错误和警告
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    StringVector getErrors() const { return errors; }
    StringVector getWarnings() const { return warnings; }
    void clearDiagnostics() { errors.clear(); warnings.clear(); }
    
    // 调试支持
    void enableDebugMode(bool enable) { debugMode = enable; }
    bool isDebugMode() const { return debugMode; }
    void setDebugOutput(std::ostream* output) { debugOutput = output; }
    
private:
    GeneratorConfig config;
    std::shared_ptr<Context> context;
    StringVector errors;
    StringVector warnings;
    bool debugMode;
    std::ostream* debugOutput;
    
    // HTML生成器
    class HTMLGenerator {
    public:
        explicit HTMLGenerator(const GeneratorConfig& config, std::shared_ptr<Context> context);
        
        String generate(const BaseNode* ast);
        String generateElement(const BaseNode* elementNode);
        String generateAttributes(const StringUnorderedMap& attributes);
        String generateChildren(const BaseNode* parentNode);
        
    private:
        const GeneratorConfig& config;
        std::shared_ptr<Context> context;
        size_t indentLevel;
        
        String getIndent() const;
        String formatTagName(const String& tagName) const;
        String formatAttributeName(const String& attrName) const;
        String escapeAttributeValue(const String& value) const;
        String escapeTextContent(const String& text) const;
        bool isVoidElement(const String& tagName) const;
        bool shouldSelfClose(const String& tagName) const;
    };
    
    // CSS生成器
    class CSSGenerator {
    public:
        explicit CSSGenerator(const GeneratorConfig& config, std::shared_ptr<Context> context);
        
        String generate(const BaseNode* ast);
        String generateStyleBlock(const BaseNode* styleNode);
        String generateSelector(const String& selector);
        String generateProperties(const StringUnorderedMap& properties);
        String generateMediaQuery(const String& media, const String& content);
        String generateKeyframes(const String& name, const String& content);
        
    private:
        const GeneratorConfig& config;
        std::shared_ptr<Context> context;
        size_t indentLevel;
        StringVector generatedSelectors;
        
        String getIndent() const;
        String formatPropertyName(const String& property) const;
        String formatPropertyValue(const String& value) const;
        String optimizeSelector(const String& selector) const;
        String addVendorPrefixes(const String& property, const String& value) const;
        bool needsVendorPrefix(const String& property) const;
    };
    
    // JavaScript生成器
    class JavaScriptGenerator {
    public:
        explicit JavaScriptGenerator(const GeneratorConfig& config, std::shared_ptr<Context> context);
        
        String generate(const BaseNode* ast);
        String generateScriptBlock(const BaseNode* scriptNode);
        String generateCHTLJSFunction(const BaseNode* chtljsNode);
        String generateEventListener(const String& selector, const String& event, const String& handler);
        String generateDOMQuery(const String& selector);
        String generateModuleExports(const StringVector& exports);
        String generateModuleImports(const StringVector& imports);
        
    private:
        const GeneratorConfig& config;
        std::shared_ptr<Context> context;
        size_t indentLevel;
        StringVector generatedFunctions;
        StringUnorderedMap selectorCache;
        
        String getIndent() const;
        String generateSelector(const String& chtlSelector);
        String generateEventBinding(const String& target, const String& event, const String& handler);
        String generateAnimation(const BaseNode* animateNode);
        String generateDelegate(const BaseNode* delegateNode);
        String generateVirtualObject(const BaseNode* virNode);
        String optimizeJavaScript(const String& js) const;
        String addStrictModeDirective(const String& js) const;
    };
    
    std::unique_ptr<HTMLGenerator> htmlGenerator;
    std::unique_ptr<CSSGenerator> cssGenerator;
    std::unique_ptr<JavaScriptGenerator> jsGenerator;
    
    // 生成流程控制
    void initializeGenerators();
    void preprocessAST(BaseNode* ast);
    void postprocessOutput(GenerationOutput& output);
    
    // 模板处理
    void resolveTemplates(BaseNode* ast);
    void instantiateTemplates(BaseNode* ast);
    void resolveCustoms(BaseNode* ast);
    void instantiateCustoms(BaseNode* ast);
    
    // 原始嵌入处理
    void processOrigins(BaseNode* ast, GenerationOutput& output);
    void mergeOriginContent(const BaseNode* originNode, GenerationOutput& output);
    
    // 导入处理
    void processImports(BaseNode* ast, GenerationOutput& output);
    void resolveImportedContent(const BaseNode* importNode, GenerationOutput& output);
    
    // 配置处理
    void applyConfiguration(const BaseNode* configNode);
    void updateGeneratorConfig(const StringUnorderedMap& configValues);
    
    // 命名空间处理
    void processNamespaces(BaseNode* ast);
    void resolveNamespacedReferences(BaseNode* ast);
    
    // 自动化处理
    void applySelectorAutomation(BaseNode* ast);
    void applyClassAutomation(BaseNode* elementNode);
    void applyIdAutomation(BaseNode* elementNode);
    void applyScriptAutomation(BaseNode* scriptNode);
    
    // 优化
    void optimizeOutput(GenerationOutput& output);
    void optimizeHTML(String& html);
    void optimizeCSS(String& css);
    void optimizeJavaScript(String& js);
    void removeRedundantCode(GenerationOutput& output);
    void mergeStyles(String& css);
    void mergeScripts(String& js);
    
    // 验证
    bool validateOutput(const GenerationOutput& output);
    void validateHTML(const String& html);
    void validateCSS(const String& css);
    void validateJavaScript(const String& js);
    
    // 错误处理
    void reportError(const String& message);
    void reportWarning(const String& message);
    void debugLog(const String& message) const;
    
    // 工具方法
    String minifyHTML(const String& html) const;
    String minifyCSS(const String& css) const;
    String minifyJavaScript(const String& js) const;
    String formatOutput(const String& content, const String& type) const;
    String generateSourceMap(const String& content, const String& type) const;
    size_t calculateOutputSize(const GenerationOutput& output) const;
};

// 生成器工厂
class GeneratorFactory {
public:
    static std::unique_ptr<Generator> createHTMLGenerator();
    static std::unique_ptr<Generator> createCSSGenerator();
    static std::unique_ptr<Generator> createJavaScriptGenerator();
    static std::unique_ptr<Generator> createFullGenerator();
    static std::unique_ptr<Generator> createMinifyingGenerator();
    static std::unique_ptr<Generator> createDebugGenerator();
    static std::unique_ptr<Generator> createProductionGenerator();
    
    // 配置预设
    static GeneratorConfig getDefaultConfig();
    static GeneratorConfig getMinifyConfig();
    static GeneratorConfig getDebugConfig();
    static GeneratorConfig getProductionConfig();
    static GeneratorConfig getDevelopmentConfig();
};

// 生成器管理器
class GeneratorManager {
public:
    explicit GeneratorManager(const GeneratorConfig& config = GeneratorConfig{});
    
    // 生成方法
    GenerationOutput generateFile(const BaseNode* ast, const String& outputPath = "");
    GenerationOutput generateToDirectory(const BaseNode* ast, const String& outputDir);
    GenerationOutput generateInMemory(const BaseNode* ast);
    
    // 批量生成
    std::vector<GenerationOutput> generateFiles(const std::vector<BaseNode*>& asts, const StringVector& outputPaths = {});
    std::vector<GenerationOutput> generateToDirectories(const std::vector<BaseNode*>& asts, const StringVector& outputDirs);
    
    // 并行生成
    std::vector<GenerationOutput> generateFilesParallel(const std::vector<BaseNode*>& asts, const StringVector& outputPaths = {}, size_t threadCount = 0);
    
    // 文件写入
    bool writeOutputToFile(const GenerationOutput& output, const String& basePath);
    bool writeOutputToDirectory(const GenerationOutput& output, const String& outputDir);
    
    // 配置管理
    void setConfig(const GeneratorConfig& config) { this->config = config; }
    const GeneratorConfig& getConfig() const { return config; }
    
    // 统计信息
    struct GenerationStats {
        size_t totalGenerations = 0;
        size_t successfulGenerations = 0;
        size_t failedGenerations = 0;
        size_t totalOutputSize = 0;
        size_t totalErrors = 0;
        size_t totalWarnings = 0;
        double totalTime = 0.0;
        double averageTime = 0.0;
    };
    
    GenerationStats getStats() const { return stats; }
    void resetStats() { stats = GenerationStats{}; }
    
private:
    GeneratorConfig config;
    GenerationStats stats;
    
    void updateStats(const GenerationOutput& output);
    String getOutputFileName(const String& basePath, const String& extension) const;
    bool ensureDirectoryExists(const String& dirPath) const;
};

} // namespace CHTL