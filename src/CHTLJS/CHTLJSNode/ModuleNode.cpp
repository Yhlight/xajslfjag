#include "ModuleNode.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// ModuleNode实现
void ModuleNode::addModule(const String& path, const String& type) {
    if (path.empty()) return;
    
    // 检查是否已存在
    if (hasModule(path)) {
        return; // 避免重复添加
    }
    
    ModuleEntry entry(path, type);
    
    // 解析路径
    entry.resolvedPath = resolveModulePath(path);
    if (entry.resolvedPath.empty()) {
        // 记录错误但不阻止添加
        jsMetadata["error_" + path] = "无法解析模块路径";
    }
    
    // 检测类型
    if (type == "auto") {
        entry.type = detectModuleType(entry.resolvedPath);
    }
    
    // 提取模块名称
    std::filesystem::path p(path);
    entry.name = p.stem().string();
    
    modules.push_back(entry);
}

void ModuleNode::addModuleWithMetadata(const String& path, const String& type, const StringUnorderedMap& metadata) {
    addModule(path, type);
    
    if (!modules.empty()) {
        ModuleEntry& entry = modules.back();
        entry.metadata = metadata;
    }
}

void ModuleNode::removeModule(const String& path) {
    modules.erase(
        std::remove_if(modules.begin(), modules.end(),
                      [&path](const ModuleEntry& entry) {
                          return entry.path == path;
                      }),
        modules.end());
}

void ModuleNode::clearModules() {
    modules.clear();
}

bool ModuleNode::hasModule(const String& path) const {
    return findModule(path) != nullptr;
}

ModuleEntry* ModuleNode::findModule(const String& path) {
    auto it = std::find_if(modules.begin(), modules.end(),
                          [&path](const ModuleEntry& entry) {
                              return entry.path == path;
                          });
    return (it != modules.end()) ? &(*it) : nullptr;
}

const ModuleEntry* ModuleNode::findModule(const String& path) const {
    auto it = std::find_if(modules.begin(), modules.end(),
                          [&path](const ModuleEntry& entry) {
                              return entry.path == path;
                          });
    return (it != modules.end()) ? &(*it) : nullptr;
}

// 路径解析实现
String ModuleNode::resolveModulePath(const String& rawPath) const {
    if (rawPath.empty()) return "";
    
    // 1. 绝对路径直接返回
    if (std::filesystem::path(rawPath).is_absolute()) {
        return std::filesystem::exists(rawPath) ? rawPath : "";
    }
    
    // 2. 相对路径解析
    if (rawPath.starts_with("./") || rawPath.starts_with("../")) {
        return resolveRelativePath(rawPath);
    }
    
    // 3. 模块名称搜索
    return searchInModuleDirectories(rawPath);
}

String ModuleNode::resolveRelativePath(const String& relativePath) const {
    std::filesystem::path base(basePath);
    std::filesystem::path relative(relativePath);
    std::filesystem::path resolved = base / relative;
    
    try {
        resolved = std::filesystem::canonical(resolved);
        return std::filesystem::exists(resolved) ? resolved.string() : "";
    } catch (const std::filesystem::filesystem_error&) {
        return "";
    }
}

String ModuleNode::searchInModuleDirectories(const String& moduleName) const {
    StringVector searchPaths = getSearchPaths();
    StringVector extensions = getSupportedExtensions();
    
    for (const auto& searchPath : searchPaths) {
        for (const auto& ext : extensions) {
            std::filesystem::path candidate = std::filesystem::path(searchPath) / (moduleName + ext);
            
            if (std::filesystem::exists(candidate)) {
                return candidate.string();
            }
        }
        
        // 尝试不带扩展名的文件
        std::filesystem::path candidate = std::filesystem::path(searchPath) / moduleName;
        if (std::filesystem::exists(candidate)) {
            return candidate.string();
        }
    }
    
    return "";
}

StringVector ModuleNode::getSearchPaths() const {
    return {
        basePath,                           // 当前目录
        basePath + "/module",               // 模块目录
        basePath + "/node_modules",         // Node.js风格
        "/usr/local/lib/chtl/modules",     // 系统模块目录
        "./modules"                         // 相对模块目录
    };
}

StringVector ModuleNode::getSupportedExtensions() const {
    return {".cjjs", ".js", ".mjs"};
}

String ModuleNode::detectModuleType(const String& filePath) const {
    if (filePath.empty()) return "unknown";
    
    std::filesystem::path p(filePath);
    String ext = p.extension().string();
    
    if (ext == ".cjjs") return "cjjs";
    if (ext == ".js" || ext == ".mjs") return "js";
    
    return "unknown";
}

// 依赖分析实现
void ModuleNode::analyzeDependencies() {
    for (auto& module : modules) {
        if (!module.resolvedPath.empty()) {
            // 加载模块内容并分析依赖
            ModuleLoader::LoadResult result = ModuleLoader::loadModule(module.resolvedPath, basePath);
            if (result.success) {
                module.dependencies = result.dependencies;
            }
        }
    }
}

StringVector ModuleNode::getLoadOrder() {
    if (loadStrategy == ModuleLoadStrategy::SEQUENTIAL) {
        // 按添加顺序返回
        StringVector order;
        for (const auto& module : modules) {
            order.push_back(module.path);
        }
        return order;
    } else if (loadStrategy == ModuleLoadStrategy::DEPENDENCY) {
        // 拓扑排序
        return topologicalSort();
    } else {
        // 并行加载，顺序不重要
        StringVector order;
        for (const auto& module : modules) {
            order.push_back(module.path);
        }
        return order;
    }
}

StringVector ModuleNode::topologicalSort() const {
    std::unordered_map<String, StringVector> graph;
    std::unordered_map<String, int> inDegree;
    
    buildDependencyGraph(graph);
    
    // 初始化入度
    for (const auto& module : modules) {
        inDegree[module.path] = 0;
    }
    
    // 计算入度
    for (const auto& [path, deps] : graph) {
        for (const auto& dep : deps) {
            if (inDegree.find(dep) != inDegree.end()) {
                inDegree[dep]++;
            }
        }
    }
    
    // 拓扑排序
    std::queue<String> queue;
    StringVector result;
    
    for (const auto& [path, degree] : inDegree) {
        if (degree == 0) {
            queue.push(path);
        }
    }
    
    while (!queue.empty()) {
        String current = queue.front();
        queue.pop();
        result.push_back(current);
        
        if (graph.find(current) != graph.end()) {
            for (const auto& neighbor : graph[current]) {
                if (inDegree.find(neighbor) != inDegree.end()) {
                    inDegree[neighbor]--;
                    if (inDegree[neighbor] == 0) {
                        queue.push(neighbor);
                    }
                }
            }
        }
    }
    
    return result;
}

void ModuleNode::buildDependencyGraph(std::unordered_map<String, StringVector>& graph) const {
    for (const auto& module : modules) {
        graph[module.path] = module.dependencies;
    }
}

bool ModuleNode::hasCircularDependency() const {
    std::unordered_map<String, StringVector> graph;
    buildDependencyGraph(graph);
    
    std::unordered_set<String> visited;
    std::unordered_set<String> recStack;
    
    for (const auto& module : modules) {
        if (visited.find(module.path) == visited.end()) {
            if (hasCycleDFS(module.path, graph, visited, recStack)) {
                return true;
            }
        }
    }
    
    return false;
}

bool ModuleNode::hasCycleDFS(const String& node,
                           const std::unordered_map<String, StringVector>& graph,
                           std::unordered_set<String>& visited,
                           std::unordered_set<String>& recStack) const {
    visited.insert(node);
    recStack.insert(node);
    
    auto it = graph.find(node);
    if (it != graph.end()) {
        for (const auto& neighbor : it->second) {
            if (visited.find(neighbor) == visited.end()) {
                if (hasCycleDFS(neighbor, graph, visited, recStack)) {
                    return true;
                }
            } else if (recStack.find(neighbor) != recStack.end()) {
                return true;
            }
        }
    }
    
    recStack.erase(node);
    return false;
}

// JavaScript生成实现
String ModuleNode::generateJavaScript() const {
    std::ostringstream js;
    
    js << "// CHTL Module System\n";
    js << "(function(global) {\n";
    js << "    'use strict';\n\n";
    
    // 生成模块注册表
    js << generateModuleRegistry();
    
    // 生成AMD加载器
    js << generateAMDLoader();
    
    // 生成加载序列
    js << generateLoadSequence();
    
    // 生成错误处理
    js << generateErrorHandling();
    
    js << "})(typeof window !== 'undefined' ? window : this);\n";
    
    return js.str();
}

String ModuleNode::generateModuleRegistry() const {
    std::ostringstream js;
    
    js << "    // 模块注册表\n";
    js << "    var CHTLModules = {\n";
    js << "        registry: {},\n";
    js << "        loaded: {},\n";
    js << "        loading: {},\n";
    js << "        dependencies: {},\n";
    js << "        errors: {},\n";
    js << "        config: {\n";
    js << "            timeout: " << loadTimeout << ",\n";
    js << "            async: " << (enableAsyncLoading ? "true" : "false") << ",\n";
    js << "            checkCircular: " << (enableCircularCheck ? "true" : "false") << "\n";
    js << "        }\n";
    js << "    };\n\n";
    
    return js.str();
}

String ModuleNode::generateAMDLoader() const {
    std::ostringstream js;
    
    js << generateDefineFunction();
    js << generateLoadFunction();
    js << generateDependencyLoader();
    js << generateModuleExecutor();
    
    return js.str();
}

String ModuleNode::generateDefineFunction() const {
    std::ostringstream js;
    
    js << "    // 定义模块\n";
    js << "    function define(name, deps, factory) {\n";
    js << "        if (typeof name !== 'string') {\n";
    js << "            factory = deps;\n";
    js << "            deps = name;\n";
    js << "            name = getCurrentModuleName();\n";
    js << "        }\n";
    js << "        if (!Array.isArray(deps)) {\n";
    js << "            factory = deps;\n";
    js << "            deps = [];\n";
    js << "        }\n";
    js << "        CHTLModules.registry[name] = {\n";
    js << "            name: name,\n";
    js << "            deps: deps,\n";
    js << "            factory: factory,\n";
    js << "            loaded: false\n";
    js << "        };\n";
    js << "        CHTLModules.dependencies[name] = deps;\n";
    js << "    }\n\n";
    
    return js.str();
}

String ModuleNode::generateLoadFunction() const {
    std::ostringstream js;
    
    js << "    // 加载模块\n";
    js << "    function load(name, callback, errorCallback) {\n";
    js << "        callback = callback || function() {};\n";
    js << "        errorCallback = errorCallback || function(err) { console.error('模块加载失败:', err); };\n\n";
    
    js << "        // 检查是否已加载\n";
    js << "        if (CHTLModules.loaded[name]) {\n";
    js << "            callback(CHTLModules.loaded[name]);\n";
    js << "            return;\n";
    js << "        }\n\n";
    
    js << "        // 检查是否正在加载\n";
    js << "        if (CHTLModules.loading[name]) {\n";
    js << "            CHTLModules.loading[name].push({success: callback, error: errorCallback});\n";
    js << "            return;\n";
    js << "        }\n\n";
    
    js << "        CHTLModules.loading[name] = [{success: callback, error: errorCallback}];\n\n";
    
    js << "        // 加载脚本\n";
    js << "        loadScript(name, function() {\n";
    js << "            executeModule(name);\n";
    js << "        }, function(err) {\n";
    js << "            CHTLModules.errors[name] = err;\n";
    js << "            var callbacks = CHTLModules.loading[name] || [];\n";
    js << "            delete CHTLModules.loading[name];\n";
    js << "            callbacks.forEach(function(cb) {\n";
    js << "                cb.error(err);\n";
    js << "            });\n";
    js << "        });\n";
    js << "    }\n\n";
    
    return js.str();
}

String ModuleNode::generateDependencyLoader() const {
    std::ostringstream js;
    
    js << "    // 加载依赖\n";
    js << "    function loadDependencies(deps, callback, errorCallback) {\n";
    js << "        if (!deps || deps.length === 0) {\n";
    js << "            callback([]);\n";
    js << "            return;\n";
    js << "        }\n\n";
    
    js << "        var results = new Array(deps.length);\n";
    js << "        var loaded = 0;\n";
    js << "        var hasError = false;\n\n";
    
    js << "        deps.forEach(function(dep, index) {\n";
    js << "            load(dep, function(result) {\n";
    js << "                if (hasError) return;\n";
    js << "                results[index] = result;\n";
    js << "                loaded++;\n";
    js << "                if (loaded === deps.length) {\n";
    js << "                    callback(results);\n";
    js << "                }\n";
    js << "            }, function(err) {\n";
    js << "                if (hasError) return;\n";
    js << "                hasError = true;\n";
    js << "                errorCallback(err);\n";
    js << "            });\n";
    js << "        });\n";
    js << "    }\n\n";
    
    return js.str();
}

String ModuleNode::generateLoadSequence() const {
    std::ostringstream js;
    
    js << "    // 模块加载序列\n";
    js << "    var moduleSequence = [\n";
    
    StringVector loadOrder;
    if (loadStrategy == ModuleLoadStrategy::DEPENDENCY) {
        // 需要先分析依赖
        loadOrder = const_cast<ModuleNode*>(this)->getLoadOrder();
    } else {
        for (const auto& module : modules) {
            loadOrder.push_back(module.resolvedPath);
        }
    }
    
    for (size_t i = 0; i < loadOrder.size(); ++i) {
        js << "        '" << loadOrder[i] << "'";
        if (i < loadOrder.size() - 1) js << ",";
        js << "\n";
    }
    
    js << "    ];\n\n";
    
    js << "    // 执行加载\n";
    if (loadStrategy == ModuleLoadStrategy::PARALLEL) {
        js << "    // 并行加载所有模块\n";
        js << "    loadDependencies(moduleSequence, function(results) {\n";
        js << "        console.log('所有模块并行加载完成:', results.length);\n";
        js << "    }, function(err) {\n";
        js << "        console.error('模块并行加载失败:', err);\n";
        js << "    });\n\n";
    } else {
        js << "    // 顺序加载模块\n";
        js << "    function loadSequentially(index) {\n";
        js << "        if (index >= moduleSequence.length) {\n";
        js << "            console.log('所有模块顺序加载完成');\n";
        js << "            return;\n";
        js << "        }\n";
        js << "        load(moduleSequence[index], function() {\n";
        js << "            loadSequentially(index + 1);\n";
        js << "        }, function(err) {\n";
        js << "            console.error('模块加载失败:', moduleSequence[index], err);\n";
        js << "        });\n";
        js << "    }\n";
        js << "    loadSequentially(0);\n\n";
    }
    
    return js.str();
}

String ModuleNode::generateErrorHandling() const {
    std::ostringstream js;
    
    js << "    // 辅助函数\n";
    js << "    function loadScript(src, onload, onerror) {\n";
    js << "        var script = document.createElement('script');\n";
    js << "        script.src = src;\n";
    js << "        script.async = " << (enableAsyncLoading ? "true" : "false") << ";\n";
    js << "        script.onload = onload;\n";
    js << "        script.onerror = function() {\n";
    js << "            onerror(new Error('Failed to load script: ' + src));\n";
    js << "        };\n";
    
    if (loadTimeout > 0) {
        js << "        var timeout = setTimeout(function() {\n";
        js << "            script.onerror = null;\n";
        js << "            script.onload = null;\n";
        js << "            onerror(new Error('Script load timeout: ' + src));\n";
        js << "        }, " << loadTimeout << ");\n";
        js << "        var originalOnload = script.onload;\n";
        js << "        script.onload = function() {\n";
        js << "            clearTimeout(timeout);\n";
        js << "            originalOnload();\n";
        js << "        };\n";
    }
    
    js << "        document.head.appendChild(script);\n";
    js << "    }\n\n";
    
    js << "    function getCurrentModuleName() {\n";
    js << "        var scripts = document.getElementsByTagName('script');\n";
    js << "        var current = scripts[scripts.length - 1];\n";
    js << "        return current.src;\n";
    js << "    }\n\n";
    
    js << "    function executeModule(name) {\n";
    js << "        var module = CHTLModules.registry[name];\n";
    js << "        if (!module || module.loaded) return;\n\n";
    
    js << "        loadDependencies(module.deps, function(deps) {\n";
    js << "            try {\n";
    js << "                var result = module.factory.apply(null, deps);\n";
    js << "                CHTLModules.loaded[name] = result || {};\n";
    js << "                module.loaded = true;\n";
    js << "                var callbacks = CHTLModules.loading[name] || [];\n";
    js << "                delete CHTLModules.loading[name];\n";
    js << "                callbacks.forEach(function(cb) {\n";
    js << "                    cb.success(CHTLModules.loaded[name]);\n";
    js << "                });\n";
    js << "            } catch (err) {\n";
    js << "                CHTLModules.errors[name] = err;\n";
    js << "                var callbacks = CHTLModules.loading[name] || [];\n";
    js << "                delete CHTLModules.loading[name];\n";
    js << "                callbacks.forEach(function(cb) {\n";
    js << "                    cb.error(err);\n";
    js << "                });\n";
    js << "            }\n";
    js << "        }, function(err) {\n";
    js << "            CHTLModules.errors[name] = err;\n";
    js << "            var callbacks = CHTLModules.loading[name] || [];\n";
    js << "            delete CHTLModules.loading[name];\n";
    js << "            callbacks.forEach(function(cb) {\n";
    js << "                cb.error(err);\n";
    js << "            });\n";
    js << "        });\n";
    js << "    }\n\n";
    
    js << "    // 暴露全局API\n";
    js << "    global.CHTLModule = {\n";
    js << "        define: define,\n";
    js << "        load: load,\n";
    js << "        registry: CHTLModules.registry,\n";
    js << "        loaded: CHTLModules.loaded,\n";
    js << "        errors: CHTLModules.errors\n";
    js << "    };\n\n";
    
    return js.str();
}

// 验证实现
bool ModuleNode::validateCHTLJSSyntax() const {
    return !modules.empty() && validateModulePaths();
}

StringVector ModuleNode::getCHTLJSValidationErrors() const {
    StringVector errors;
    
    if (modules.empty()) {
        errors.push_back("模块列表为空");
    }
    
    StringVector pathErrors = getPathValidationErrors();
    errors.insert(errors.end(), pathErrors.begin(), pathErrors.end());
    
    if (enableCircularCheck && hasCircularDependency()) {
        errors.push_back("检测到循环依赖");
    }
    
    return errors;
}

bool ModuleNode::validateModulePaths() const {
    for (const auto& module : modules) {
        if (!isValidModulePath(module.path)) {
            return false;
        }
    }
    return true;
}

StringVector ModuleNode::getPathValidationErrors() const {
    StringVector errors;
    
    for (const auto& module : modules) {
        if (!isValidModulePath(module.path)) {
            errors.push_back("无效的模块路径: " + module.path);
        }
        
        if (module.resolvedPath.empty()) {
            errors.push_back("无法解析模块路径: " + module.path);
        }
    }
    
    return errors;
}

bool ModuleNode::isValidModulePath(const String& path) const {
    if (path.empty()) return false;
    
    // 检查路径格式
    static const std::regex pathPattern(R"(^[\.\/\w\-_]+\.(cjjs|js|mjs)$|^[\.\/\w\-_]+$)");
    return std::regex_match(path, pathPattern);
}

String ModuleNode::toString() const {
    std::ostringstream ss;
    ss << "ModuleNode(modules=" << modules.size() 
       << ", strategy=" << static_cast<int>(loadStrategy)
       << ", circular_check=" << enableCircularCheck << ")";
    return ss.str();
}

std::unique_ptr<BaseNode> ModuleNode::clone() const {
    auto cloned = std::make_unique<ModuleNode>(value, position);
    cloned->modules = modules;
    cloned->loadStrategy = loadStrategy;
    cloned->enableCircularCheck = enableCircularCheck;
    cloned->enableAsyncLoading = enableAsyncLoading;
    cloned->loadTimeout = loadTimeout;
    cloned->basePath = basePath;
    return std::unique_ptr<BaseNode>(cloned.release());
}

// ModuleLoader实现
ModuleLoader::LoadResult ModuleLoader::loadModule(const String& path, const String& basePath) {
    LoadResult result;
    result.success = false;
    result.resolvedPath = path;
    result.type = resolveModuleType(path);
    
    try {
        result.content = readFileContent(path);
        if (!result.content.empty()) {
            result.dependencies = StringVector(); // TODO: 实现依赖提取
            result.success = validateModuleContent(result.content, result.type);
        }
    } catch (const std::exception& e) {
        result.errors.push_back(String(e.what()));
    }
    
    return result;
}

String ModuleLoader::readFileContent(const String& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filePath);
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

String ModuleLoader::resolveModuleType(const String& filePath) {
    std::filesystem::path p(filePath);
    String ext = p.extension().string();
    
    if (ext == ".cjjs") return "cjjs";
    if (ext == ".js" || ext == ".mjs") return "js";
    
    return "unknown";
}

bool ModuleLoader::validateModuleContent(const String& content, const String& type) {
    if (content.empty()) return false;
    
    if (type == "cjjs") {
        // 验证CHTL JS语法
        return content.find("{{") != String::npos || 
               content.find("listen") != String::npos ||
               content.find("delegate") != String::npos;
    } else if (type == "js") {
        // 验证JavaScript语法（简化）
        return true; // 暂时简化处理
    }
    
    return false;
}

// 兼容性方法实现
StringVector ModuleNode::getDependencies() const {
    return dependencies;
}

StringVector ModuleNode::getExports() const {
    return exports;
}

String ModuleNode::getContent() const {
    return moduleContent;
}

String ModuleNode::getPath() const {
    return modulePath;
}

String ModuleNode::getLoadStrategy() const {
    return loadStrategyStr;
}

void ModuleNode::setContent(const String& content) {
    moduleContent = content;
}

void ModuleNode::setPath(const String& path) {
    modulePath = path;
}

void ModuleNode::setLoadStrategy(const String& strategy) {
    loadStrategyStr = strategy;
}

void ModuleNode::addDependency(const String& dependency) {
    if (std::find(dependencies.begin(), dependencies.end(), dependency) == dependencies.end()) {
        dependencies.push_back(dependency);
    }
}

void ModuleNode::addExport(const String& exportName) {
    if (std::find(exports.begin(), exports.end(), exportName) == exports.end()) {
        exports.push_back(exportName);
    }
}

String ModuleNode::getModuleName() const {
    return getName();  // 使用基类的getName方法
}

} // namespace CHTL