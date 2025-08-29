#include "ModuleLoader.h"
#include "../CHTLLexer/Lexer.h"
#include "../CHTLParser/Parser.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// ModuleLoader实现
ModuleLoader::ModuleLoader(const ModuleLoaderConfig& config) 
    : config(config) {
    // 初始化搜索路径
    initializeSearchPaths();
    
    // 设置支持的文件扩展名
    supportedExtensions[".chtl"] = ModuleType::CHTL;
    supportedExtensions[".cmod"] = ModuleType::CMOD;
    supportedExtensions[".cjjs"] = ModuleType::CHTL_JS;
    supportedExtensions[".html"] = ModuleType::HTML;
    supportedExtensions[".css"] = ModuleType::CSS;
    supportedExtensions[".js"] = ModuleType::JAVASCRIPT;
}

LoadResult ModuleLoader::loadModule(const String& modulePath, ModuleType expectedType) {
    LoadResult result;
    result.success = false;
    
    try {
        // 检查循环依赖
        if (isCircularDependency(modulePath)) {
            result.errors.push_back("检测到循环依赖: " + modulePath);
            return result;
        }
        
        // 添加到依赖栈
        dependencyStack.push_back(modulePath);
        
        // 检查缓存
        if (config.enableCaching && isModuleCached(modulePath)) {
            result = getCachedModule(modulePath);
            dependencyStack.pop_back();
            return result;
        }
        
        // 解析模块路径
        String resolvedPath = resolveModulePath(modulePath, expectedType);
        if (resolvedPath.empty()) {
            result.errors.push_back("无法解析模块路径: " + modulePath);
            dependencyStack.pop_back();
            return result;
        }
        
        // 检测模块类型
        ModuleType actualType = detectModuleType(resolvedPath);
        if (expectedType != ModuleType::AUTO && actualType != expectedType) {
            result.warnings.push_back("模块类型不匹配: 期望 " + moduleTypeToString(expectedType) + 
                                    ", 实际 " + moduleTypeToString(actualType));
        }
        
        // 加载模块内容
        String content = loadFileContent(resolvedPath);
        if (content.empty()) {
            result.errors.push_back("无法加载模块内容: " + resolvedPath);
            dependencyStack.pop_back();
            return result;
        }
        
        // 解析模块
        result = parseModule(content, actualType, resolvedPath);
        
        // 处理模块依赖
        if (result.success && config.loadDependencies) {
            loadModuleDependencies(result);
        }
        
        // 缓存结果
        if (config.enableCaching && result.success) {
            cacheModule(modulePath, result);
        }
        
        dependencyStack.pop_back();
        
    } catch (const std::exception& e) {
        result.errors.push_back("模块加载异常: " + String(e.what()));
        if (!dependencyStack.empty()) {
            dependencyStack.pop_back();
        }
    }
    
    return result;
}

String ModuleLoader::resolveModulePath(const String& modulePath, ModuleType expectedType) {
    // 检查是否是绝对路径
    if (std::filesystem::path(modulePath).is_absolute()) {
        if (std::filesystem::exists(modulePath)) {
            return modulePath;
        }
        return "";
    }
    
    // 检查是否包含文件扩展名
    std::filesystem::path path(modulePath);
    bool hasExtension = !path.extension().empty();
    
    if (hasExtension) {
        // 具体文件名（带后缀）
        return resolveExactFile(modulePath);
    } else {
        // 文件名（不带后缀）
        return resolveFileByName(modulePath, expectedType);
    }
}

String ModuleLoader::resolveExactFile(const String& filePath) {
    // 按照搜索路径顺序查找
    for (const auto& searchPath : searchPaths) {
        std::filesystem::path fullPath = std::filesystem::path(searchPath) / filePath;
        if (std::filesystem::exists(fullPath)) {
            return fullPath.string();
        }
    }
    
    return "";
}

String ModuleLoader::resolveFileByName(const String& fileName, ModuleType expectedType) {
    // 根据期望类型确定扩展名优先级
    StringVector extensions = getExtensionsByPriority(expectedType);
    
    for (const auto& searchPath : searchPaths) {
        for (const auto& ext : extensions) {
            std::filesystem::path fullPath = std::filesystem::path(searchPath) / (fileName + ext);
            if (std::filesystem::exists(fullPath)) {
                return fullPath.string();
            }
        }
    }
    
    return "";
}

StringVector ModuleLoader::getExtensionsByPriority(ModuleType type) {
    switch (type) {
        case ModuleType::CHTL:
            return {".cmod", ".chtl"};
        case ModuleType::CMOD:
            return {".cmod"};
        case ModuleType::CHTL_JS:
            return {".cjjs", ".js"};
        case ModuleType::HTML:
            return {".html", ".htm"};
        case ModuleType::CSS:
            return {".css"};
        case ModuleType::JAVASCRIPT:
            return {".js"};
        default:
            return {".chtl", ".cmod", ".cjjs", ".html", ".css", ".js"};
    }
}

ModuleType ModuleLoader::detectModuleType(const String& filePath) {
    std::filesystem::path path(filePath);
    String extension = path.extension().string();
    
    auto it = supportedExtensions.find(extension);
    if (it != supportedExtensions.end()) {
        return it->second;
    }
    
    return ModuleType::UNKNOWN;
}

String ModuleLoader::loadFileContent(const String& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

LoadResult ModuleLoader::parseModule(const String& content, ModuleType type, const String& filePath) {
    LoadResult result;
    result.modulePath = filePath;
    result.moduleType = type;
    result.success = false;
    
    try {
        switch (type) {
            case ModuleType::CHTL:
            case ModuleType::CMOD:
                result = parseCHTLModule(content, filePath);
                break;
            case ModuleType::CHTL_JS:
                result = parseCHTLJSModule(content, filePath);
                break;
            case ModuleType::HTML:
                result = parseHTMLModule(content, filePath);
                break;
            case ModuleType::CSS:
                result = parseCSSModule(content, filePath);
                break;
            case ModuleType::JAVASCRIPT:
                result = parseJavaScriptModule(content, filePath);
                break;
            default:
                result.errors.push_back("不支持的模块类型: " + moduleTypeToString(type));
                break;
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back("模块解析异常: " + String(e.what()));
    }
    
    return result;
}

LoadResult ModuleLoader::parseCHTLModule(const String& content, const String& filePath) {
    LoadResult result;
    result.success = false;
    
    try {
        // 创建CHTL词法分析器
        auto lexer = std::make_unique<Lexer>(content);
        
        // 创建CHTL解析器
        auto parser = std::make_unique<Parser>(std::move(lexer));
        
        // 解析AST
        result.ast = parser->parse();
        
        if (parser->hasErrors()) {
            for (const auto& error : parser->getErrors()) {
                result.errors.push_back("CHTL解析错误: " + error.toString());
            }
        } else {
            result.success = true;
            
            // 提取模块信息
            extractModuleExports(result);
            extractModuleImports(result);
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back("CHTL模块解析异常: " + String(e.what()));
    }
    
    return result;
}

LoadResult ModuleLoader::parseCHTLJSModule(const String& content, const String& filePath) {
    LoadResult result;
    result.success = true; // CHTL JS模块暂时直接成功
    result.rawContent = content;
    
    // 简化的CHTL JS解析
    extractCHTLJSFeatures(result);
    
    return result;
}

LoadResult ModuleLoader::parseHTMLModule(const String& content, const String& filePath) {
    LoadResult result;
    result.success = true;
    result.rawContent = content;
    
    // 提取HTML中的资源引用
    extractHTMLReferences(result);
    
    return result;
}

LoadResult ModuleLoader::parseCSSModule(const String& content, const String& filePath) {
    LoadResult result;
    result.success = true;
    result.rawContent = content;
    
    // 提取CSS中的@import引用
    extractCSSImports(result);
    
    return result;
}

LoadResult ModuleLoader::parseJavaScriptModule(const String& content, const String& filePath) {
    LoadResult result;
    result.success = true;
    result.rawContent = content;
    
    // 提取JavaScript中的import/require
    extractJavaScriptImports(result);
    
    return result;
}

void ModuleLoader::extractModuleExports(LoadResult& result) {
    if (!result.ast) return;
    
    // 遍历AST提取导出信息
    extractExportsFromNode(result.ast.get(), result);
}

void ModuleLoader::extractExportsFromNode(BaseNode* node, LoadResult& result) {
    if (!node) return;
    
    // 检查是否是导出节点
    if (node->type == NodeType::TEMPLATE || 
        node->type == NodeType::CUSTOM || 
        node->type == NodeType::ORIGIN) {
        
        ModuleExport exportInfo;
        exportInfo.name = node->value;
        exportInfo.type = nodeTypeToString(node->type);
        exportInfo.isPublic = true; // 默认公开
        
        result.exports.push_back(exportInfo);
    }
    
    // 递归处理子节点
    for (const auto& child : node->children) {
        extractExportsFromNode(child.get(), result);
    }
}

void ModuleLoader::extractModuleImports(LoadResult& result) {
    if (!result.ast) return;
    
    // 遍历AST提取导入信息
    extractImportsFromNode(result.ast.get(), result);
}

void ModuleLoader::extractImportsFromNode(BaseNode* node, LoadResult& result) {
    if (!node) return;
    
    // 检查是否是导入节点
    if (node->type == NodeType::IMPORT) {
        ModuleDependency dependency;
        dependency.modulePath = node->getAttribute("source");
        dependency.importType = node->getAttribute("import_type");
        dependency.alias = node->getAttribute("alias");
        
        result.dependencies.push_back(dependency);
    }
    
    // 递归处理子节点
    for (const auto& child : node->children) {
        extractImportsFromNode(child.get(), result);
    }
}

void ModuleLoader::extractCHTLJSFeatures(LoadResult& result) {
    const String& content = result.rawContent;
    
    // 提取module块中的load语句
    std::regex moduleRegex(R"(module\s*\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, moduleRegex)) {
        String moduleContent = match[1].str();
        
        // 提取load语句
        std::regex loadRegex(R"(load:\s*([^,\}]+))");
        std::sregex_iterator loadBegin(moduleContent.begin(), moduleContent.end(), loadRegex);
        std::sregex_iterator loadEnd;
        
        for (std::sregex_iterator i = loadBegin; i != loadEnd; ++i) {
            ModuleDependency dependency;
            dependency.modulePath = (*i)[1].str();
            dependency.importType = "CHTL_JS";
            
            result.dependencies.push_back(dependency);
        }
    }
}

void ModuleLoader::extractHTMLReferences(LoadResult& result) {
    const String& content = result.rawContent;
    
    // 提取link标签中的CSS引用
    std::regex linkRegex(R"(<link[^>]+href=['"]([^'"]+)['"][^>]*>)");
    std::sregex_iterator linkBegin(content.begin(), content.end(), linkRegex);
    std::sregex_iterator linkEnd;
    
    for (std::sregex_iterator i = linkBegin; i != linkEnd; ++i) {
        ModuleDependency dependency;
        dependency.modulePath = (*i)[1].str();
        dependency.importType = "CSS";
        
        result.dependencies.push_back(dependency);
    }
    
    // 提取script标签中的JS引用
    std::regex scriptRegex(R"(<script[^>]+src=['"]([^'"]+)['"][^>]*>)");
    std::sregex_iterator scriptBegin(content.begin(), content.end(), scriptRegex);
    
    for (std::sregex_iterator i = scriptBegin; i != linkEnd; ++i) {
        ModuleDependency dependency;
        dependency.modulePath = (*i)[1].str();
        dependency.importType = "JAVASCRIPT";
        
        result.dependencies.push_back(dependency);
    }
}

void ModuleLoader::extractCSSImports(LoadResult& result) {
    const String& content = result.rawContent;
    
    // 提取@import语句
    std::regex importRegex(R"(@import\s+['"]([^'"]+)['"])");
    std::sregex_iterator importBegin(content.begin(), content.end(), importRegex);
    std::sregex_iterator importEnd;
    
    for (std::sregex_iterator i = importBegin; i != importEnd; ++i) {
        ModuleDependency dependency;
        dependency.modulePath = (*i)[1].str();
        dependency.importType = "CSS";
        
        result.dependencies.push_back(dependency);
    }
}

void ModuleLoader::extractJavaScriptImports(LoadResult& result) {
    const String& content = result.rawContent;
    
    // 提取ES6 import语句
    std::regex importRegex(R"(import\s+.*?from\s+['"]([^'"]+)['"])");
    std::sregex_iterator importBegin(content.begin(), content.end(), importRegex);
    std::sregex_iterator importEnd;
    
    for (std::sregex_iterator i = importBegin; i != importEnd; ++i) {
        ModuleDependency dependency;
        dependency.modulePath = (*i)[1].str();
        dependency.importType = "JAVASCRIPT";
        
        result.dependencies.push_back(dependency);
    }
    
    // 提取CommonJS require语句
    std::regex requireRegex(R"(require\(['"]([^'"]+)['"]\))");
    std::sregex_iterator requireBegin(content.begin(), content.end(), requireRegex);
    
    for (std::sregex_iterator i = requireBegin; i != importEnd; ++i) {
        ModuleDependency dependency;
        dependency.modulePath = (*i)[1].str();
        dependency.importType = "JAVASCRIPT";
        
        result.dependencies.push_back(dependency);
    }
}

void ModuleLoader::loadModuleDependencies(LoadResult& result) {
    for (const auto& dependency : result.dependencies) {
        if (!dependency.modulePath.empty()) {
            ModuleType depType = stringToModuleType(dependency.importType);
            LoadResult depResult = loadModule(dependency.modulePath, depType);
            
            if (depResult.success) {
                result.loadedDependencies.push_back(depResult);
            } else {
                for (const auto& error : depResult.errors) {
                    result.warnings.push_back("依赖加载警告 (" + dependency.modulePath + "): " + error);
                }
            }
        }
    }
}

bool ModuleLoader::isCircularDependency(const String& modulePath) {
    return std::find(dependencyStack.begin(), dependencyStack.end(), modulePath) != dependencyStack.end();
}

bool ModuleLoader::isModuleCached(const String& modulePath) {
    return moduleCache.find(modulePath) != moduleCache.end();
}

LoadResult ModuleLoader::getCachedModule(const String& modulePath) {
    auto it = moduleCache.find(modulePath);
    if (it != moduleCache.end()) {
        return it->second;
    }
    
    LoadResult result;
    result.success = false;
    result.errors.push_back("缓存中未找到模块: " + modulePath);
    return result;
}

void ModuleLoader::cacheModule(const String& modulePath, const LoadResult& result) {
    if (moduleCache.size() >= config.maxCacheSize) {
        // 简单的LRU清理策略
        moduleCache.clear();
    }
    
    moduleCache[modulePath] = result;
}

void ModuleLoader::initializeSearchPaths() {
    searchPaths.clear();
    
    // 添加当前目录
    searchPaths.push_back(".");
    
    // 添加当前目录的module文件夹
    searchPaths.push_back("./module");
    
    // 添加官方模块目录
    if (!config.officialModulePath.empty()) {
        searchPaths.push_back(config.officialModulePath);
    }
    
    // 添加配置中的额外搜索路径
    for (const auto& path : config.additionalSearchPaths) {
        searchPaths.push_back(path);
    }
}

String ModuleLoader::moduleTypeToString(ModuleType type) {
    switch (type) {
        case ModuleType::CHTL: return "CHTL";
        case ModuleType::CMOD: return "CMOD";
        case ModuleType::CHTL_JS: return "CHTL_JS";
        case ModuleType::HTML: return "HTML";
        case ModuleType::CSS: return "CSS";
        case ModuleType::JAVASCRIPT: return "JAVASCRIPT";
        case ModuleType::AUTO: return "AUTO";
        default: return "UNKNOWN";
    }
}

ModuleType ModuleLoader::stringToModuleType(const String& typeStr) {
    if (typeStr == "CHTL") return ModuleType::CHTL;
    if (typeStr == "CMOD") return ModuleType::CMOD;
    if (typeStr == "CHTL_JS") return ModuleType::CHTL_JS;
    if (typeStr == "HTML") return ModuleType::HTML;
    if (typeStr == "CSS") return ModuleType::CSS;
    if (typeStr == "JAVASCRIPT") return ModuleType::JAVASCRIPT;
    return ModuleType::UNKNOWN;
}

String ModuleLoader::nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::TEMPLATE: return "Template";
        case NodeType::CUSTOM: return "Custom";
        case NodeType::ORIGIN: return "Origin";
        case NodeType::IMPORT: return "Import";
        default: return "Unknown";
    }
}

void ModuleLoader::clearCache() {
    moduleCache.clear();
}

void ModuleLoader::setConfig(const ModuleLoaderConfig& newConfig) {
    config = newConfig;
    initializeSearchPaths();
}

ModuleLoaderConfig ModuleLoader::getConfig() const {
    return config;
}

StringVector ModuleLoader::getSearchPaths() const {
    return searchPaths;
}

size_t ModuleLoader::getCacheSize() const {
    return moduleCache.size();
}

} // namespace CHTL