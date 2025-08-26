#include "CHTLCompiler.h"
#include "Lexer/CHTLLexer.h"
#include "Parser/CHTLParser.h"
#include "Generator/CHTLGenerator.h"
#include <fstream>
#include <sstream>
#include <chrono>

namespace CHTL {

CHTLCompiler::CHTLCompiler(const CHTLCompilerConfig& cfg) 
    : config(cfg), nextCacheId(1) {
    initializeComponents();
}

void CHTLCompiler::initializeComponents() {
    lexer = std::make_unique<CHTLLexer>("");
    parser = std::make_unique<CHTLParser>(std::vector<CHTLToken>());
    
    CHTLGeneratorConfig genConfig;
    genConfig.outputCharset = config.outputCharset;
    genConfig.indentString = config.indentString;
    genConfig.minifyOutput = config.minifyOutput;
    genConfig.enableOptimization = config.enableOptimization;
    genConfig.includeComments = config.includeComments;
    genConfig.validateOutput = config.validateOutput;
    
    generator = std::make_unique<CHTLGenerator>(genConfig);
}

CHTLCompilerResult CHTLCompiler::compile(const std::string& source, const std::string& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CHTLCompilerResult result;
    result.success = false;
    result.filename = filename.empty() ? "anonymous.chtl" : filename;
    
    try {
        // 预处理
        std::string processedSource = preprocessSource(source);
        
        // 检查缓存
        if (config.enableCaching) {
            std::string cacheKey = generateCacheKey(processedSource, filename);
            if (hasCachedResult(cacheKey)) {
                result = getCachedResult(cacheKey);
                result.fromCache = true;
                updateStatistics("cache_hit");
                return result;
            }
        }
        
        // 词法分析
        auto tokens = tokenize(processedSource, filename);
        if (!tokens.empty() && tokens.back().type == CHTLTokenType::INVALID) {
            result.errors.push_back("词法分析失败");
            return result;
        }
        
        // 语法分析
        auto ast = parse(tokens, filename);
        if (!ast) {
            result.errors.push_back("语法分析失败");
            return result;
        }
        
        // 代码生成
        auto generationResult = generate(ast, filename);
        if (!generationResult.success) {
            result.errors = generationResult.errors;
            result.warnings = generationResult.warnings;
            return result;
        }
        
        // 后处理
        result.html = postprocessHTML(generationResult.html);
        result.css = postprocessCSS(generationResult.css);
        result.javascript = postprocessJS(generationResult.javascript);
        result.errors = generationResult.errors;
        result.warnings = generationResult.warnings;
        result.success = true;
        result.metadata = generationResult.metadata;
        
        // 缓存结果
        if (config.enableCaching && result.success) {
            std::string cacheKey = generateCacheKey(processedSource, filename);
            setCachedResult(cacheKey, result);
        }
        
        // 更新统计
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        result.compilationTime = duration.count();
        
        updateStatistics("compile_success");
        totalCompilationTime += duration.count();
        
    } catch (const std::exception& e) {
        result.errors.push_back("编译异常: " + std::string(e.what()));
        updateStatistics("compile_error");
    }
    
    return result;
}

CHTLCompilerResult CHTLCompiler::compileFile(const std::string& filepath) {
    CHTLCompilerResult result;
    result.filename = filepath;
    result.success = false;
    
    // 验证文件路径
    if (!validateFilePath(filepath)) {
        result.errors.push_back("无效的文件路径: " + filepath);
        return result;
    }
    
    // 读取文件
    std::string source;
    if (!readFile(filepath, source)) {
        result.errors.push_back("无法读取文件: " + filepath);
        return result;
    }
    
    // 解析路径信息
    std::string directory = getDirectoryFromPath(filepath);
    std::string filename = getFilenameFromPath(filepath);
    
    // 设置上下文
    setCurrentDirectory(directory);
    
    // 编译
    result = compile(source, filename);
    result.filepath = filepath;
    
    return result;
}

std::vector<CHTLCompilerResult> CHTLCompiler::compileFiles(const std::vector<std::string>& filepaths) {
    std::vector<CHTLCompilerResult> results;
    results.reserve(filepaths.size());
    
    for (const auto& filepath : filepaths) {
        auto result = compileFile(filepath);
        results.push_back(result);
        
        if (!result.success && config.stopOnFirstError) {
            break;
        }
    }
    
    return results;
}

CHTLCompilerResult CHTLCompiler::incrementalCompile(const std::string& source, const std::string& filename, const std::string& previousSource) {
    // 简化的增量编译 - 实际实现需要更复杂的差异分析
    if (source == previousSource) {
        CHTLCompilerResult result;
        result.success = true;
        result.filename = filename;
        result.fromCache = true;
        return result;
    }
    
    return compile(source, filename);
}

std::vector<CHTLToken> CHTLCompiler::tokenize(const std::string& source, const std::string& filename) {
    if (!lexer) {
        initializeComponents();
    }
    
    lexer->reset(source);
    auto tokens = lexer->tokenize();
    
    if (lexer->hasErrors()) {
        // 记录词法错误但继续处理
        auto lexerErrors = lexer->getErrors();
        for (const auto& error : lexerErrors) {
            addDiagnostic("lexer", error, filename);
        }
    }
    
    return tokens;
}

std::shared_ptr<CHTLDocumentNode> CHTLCompiler::parse(const std::vector<CHTLToken>& tokens, const std::string& filename) {
    if (!parser) {
        initializeComponents();
    }
    
    parser = std::make_unique<CHTLParser>(tokens);
    auto ast = parser->parseDocument();
    
    if (parser->hasErrors()) {
        auto parserErrors = parser->getErrors();
        for (const auto& error : parserErrors) {
            addDiagnostic("parser", error, filename);
        }
        return nullptr;
    }
    
    return ast;
}

CHTLGenerationResult CHTLCompiler::generate(std::shared_ptr<CHTLDocumentNode> ast, const std::string& filename) {
    if (!generator) {
        initializeComponents();
    }
    
    auto result = generator->generate(ast);
    
    if (!result.errors.empty()) {
        for (const auto& error : result.errors) {
            addDiagnostic("generator", error, filename);
        }
    }
    
    if (!result.warnings.empty()) {
        for (const auto& warning : result.warnings) {
            addDiagnostic("generator", warning, filename);
        }
    }
    
    return result;
}

// === 配置管理 ===

void CHTLCompiler::setConfig(const CHTLCompilerConfig& newConfig) {
    config = newConfig;
    initializeComponents();
}

CHTLCompilerConfig CHTLCompiler::getConfig() const {
    return config;
}

void CHTLCompiler::setConfigOption(const std::string& key, const std::string& value) {
    if (key == "outputCharset") {
        config.outputCharset = value;
    } else if (key == "indentString") {
        config.indentString = value;
    } else if (key == "minifyOutput") {
        config.minifyOutput = (value == "true");
    } else if (key == "enableOptimization") {
        config.enableOptimization = (value == "true");
    } else if (key == "enableCaching") {
        config.enableCaching = (value == "true");
    } else if (key == "includeComments") {
        config.includeComments = (value == "true");
    } else if (key == "validateOutput") {
        config.validateOutput = (value == "true");
    } else if (key == "stopOnFirstError") {
        config.stopOnFirstError = (value == "true");
    }
    
    // 重新初始化组件以应用新配置
    initializeComponents();
}

// === 缓存管理 ===

std::string CHTLCompiler::generateCacheKey(const std::string& source, const std::string& filename) {
    std::hash<std::string> hasher;
    size_t sourceHash = hasher(source);
    size_t filenameHash = hasher(filename);
    size_t configHash = hasher(config.outputCharset + config.indentString + 
                              (config.minifyOutput ? "1" : "0") + 
                              (config.enableOptimization ? "1" : "0"));
    
    return std::to_string(sourceHash ^ filenameHash ^ configHash);
}

bool CHTLCompiler::hasCachedResult(const std::string& cacheKey) {
    return compilationCache.find(cacheKey) != compilationCache.end();
}

CHTLCompilerResult CHTLCompiler::getCachedResult(const std::string& cacheKey) {
    auto it = compilationCache.find(cacheKey);
    if (it != compilationCache.end()) {
        return it->second;
    }
    return CHTLCompilerResult();
}

void CHTLCompiler::setCachedResult(const std::string& cacheKey, const CHTLCompilerResult& result) {
    if (compilationCache.size() >= config.maxCacheSize) {
        // 简单的LRU淘汰策略
        auto oldest = compilationCache.begin();
        compilationCache.erase(oldest);
    }
    
    compilationCache[cacheKey] = result;
}

void CHTLCompiler::clearCache() {
    compilationCache.clear();
}

// === 诊断和错误处理 ===

void CHTLCompiler::addDiagnostic(const std::string& type, const std::string& message, const std::string& filename) {
    CHTLCompilerDiagnostic diagnostic;
    diagnostic.type = type;
    diagnostic.message = message;
    diagnostic.filename = filename;
    diagnostic.timestamp = std::chrono::system_clock::now();
    
    diagnostics.push_back(diagnostic);
}

std::vector<CHTLCompilerDiagnostic> CHTLCompiler::getDiagnostics() const {
    return diagnostics;
}

void CHTLCompiler::clearDiagnostics() {
    diagnostics.clear();
}

// === 统计和性能 ===

void CHTLCompiler::updateStatistics(const std::string& operation) {
    statistics[operation]++;
}

CHTLCompilerStatistics CHTLCompiler::getStatistics() const {
    CHTLCompilerStatistics stats;
    
    auto it = statistics.find("compile_success");
    stats.successfulCompilations = it != statistics.end() ? it->second : 0;
    
    it = statistics.find("compile_error");
    stats.failedCompilations = it != statistics.end() ? it->second : 0;
    
    it = statistics.find("cache_hit");
    stats.cacheHits = it != statistics.end() ? it->second : 0;
    
    stats.totalCompilationTime = totalCompilationTime;
    stats.averageCompilationTime = stats.successfulCompilations > 0 ? 
        totalCompilationTime / stats.successfulCompilations : 0;
    
    return stats;
}

void CHTLCompiler::resetStatistics() {
    statistics.clear();
    totalCompilationTime = 0;
}

// === 实用方法 ===

std::string CHTLCompiler::getVersion() const {
    return "CHTL Compiler v1.0.0";
}

std::vector<std::string> CHTLCompiler::getSupportedExtensions() const {
    return {".chtl", ".chtlx"};
}

bool CHTLCompiler::isFileSupported(const std::string& filename) const {
    auto extensions = getSupportedExtensions();
    for (const auto& ext : extensions) {
        if (filename.length() >= ext.length() && 
            filename.substr(filename.length() - ext.length()) == ext) {
            return true;
        }
    }
    return false;
}

// === 私有辅助方法 ===

std::string CHTLCompiler::preprocessSource(const std::string& source) {
    // 基本的源码预处理
    std::string processed = source;
    
    // 移除BOM
    if (processed.length() >= 3 && 
        processed.substr(0, 3) == "\xEF\xBB\xBF") {
        processed = processed.substr(3);
    }
    
    // 标准化换行符
    std::string normalized;
    for (size_t i = 0; i < processed.length(); ++i) {
        if (processed[i] == '\r') {
            if (i + 1 < processed.length() && processed[i + 1] == '\n') {
                normalized += '\n';
                ++i; // 跳过\n
            } else {
                normalized += '\n';
            }
        } else {
            normalized += processed[i];
        }
    }
    
    return normalized;
}

std::string CHTLCompiler::postprocessHTML(const std::string& html) {
    std::string processed = html;
    
    if (config.minifyOutput) {
        // 简单的HTML压缩
        processed = minifyHTML(processed);
    }
    
    return processed;
}

std::string CHTLCompiler::postprocessCSS(const std::string& css) {
    std::string processed = css;
    
    if (config.minifyOutput) {
        // 简单的CSS压缩
        processed = minifyCSS(processed);
    }
    
    return processed;
}

std::string CHTLCompiler::postprocessJS(const std::string& js) {
    std::string processed = js;
    
    if (config.minifyOutput) {
        // 简单的JS压缩
        processed = minifyJS(processed);
    }
    
    return processed;
}

std::string CHTLCompiler::minifyHTML(const std::string& html) {
    std::string minified;
    bool inWhitespace = false;
    
    for (char c : html) {
        if (std::isspace(c)) {
            if (!inWhitespace) {
                minified += ' ';
                inWhitespace = true;
            }
        } else {
            minified += c;
            inWhitespace = false;
        }
    }
    
    return minified;
}

std::string CHTLCompiler::minifyCSS(const std::string& css) {
    std::string minified;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < css.length(); ++i) {
        char c = css[i];
        
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
            minified += c;
        } else if (inString && c == stringChar && (i == 0 || css[i-1] != '\\')) {
            inString = false;
            minified += c;
        } else if (!inString && std::isspace(c)) {
            // 跳过空白字符
            continue;
        } else {
            minified += c;
        }
    }
    
    return minified;
}

std::string CHTLCompiler::minifyJS(const std::string& js) {
    // 简单的JS压缩 - 实际实现需要更复杂的逻辑
    return minifyCSS(js); // 暂时使用CSS的压缩逻辑
}

bool CHTLCompiler::validateFilePath(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
    // 检查文件扩展名
    if (!isFileSupported(filepath)) {
        return false;
    }
    
    // 检查路径安全性
    if (filepath.find("..") != std::string::npos) {
        return false;
    }
    
    return true;
}

bool CHTLCompiler::readFile(const std::string& filepath, std::string& content) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    
    return true;
}

std::string CHTLCompiler::getDirectoryFromPath(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filepath.substr(0, lastSlash);
    }
    return ".";
}

std::string CHTLCompiler::getFilenameFromPath(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filepath.substr(lastSlash + 1);
    }
    return filepath;
}

void CHTLCompiler::setCurrentDirectory(const std::string& directory) {
    currentDirectory = directory;
}

// === 便利函数实现 ===

CHTLCompilerResult compileCHTL(const std::string& source, const CHTLCompilerConfig& config) {
    CHTLCompiler compiler(config);
    return compiler.compile(source);
}

CHTLCompilerResult compileCHTLFile(const std::string& filepath, const CHTLCompilerConfig& config) {
    CHTLCompiler compiler(config);
    return compiler.compileFile(filepath);
}

std::vector<CHTLCompilerResult> compileCHTLFiles(const std::vector<std::string>& filepaths, const CHTLCompilerConfig& config) {
    CHTLCompiler compiler(config);
    return compiler.compileFiles(filepaths);
}

} // namespace CHTL