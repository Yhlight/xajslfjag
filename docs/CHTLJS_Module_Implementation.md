# CHTL JS Module系统实现详解

## 概述

CHTL JS的module系统提供了模块化编程支持，使用类似AMD风格的语法，但编译到不同的目标格式。

## 语法设计

### 基本语法
```javascript
module {
    load: ["jquery", "utils", "./components/header"]
}
```

### 扩展语法
```javascript
module {
    load: ["dep1", "dep2"],
    export: {
        myFunction: function() {},
        myVariable: value
    },
    config: {
        type: "amd",      // amd | commonjs | es6
        async: true,
        baseUrl: "./src"
    }
}
```

## 实现架构

### 1. 词法分析阶段

```cpp
// CHTLJSLexer中识别module关键字
TokenType CHTLJSLexer::identifyKeyword(const std::string& text) {
    if (text == "module") return TokenType::MODULE;
    if (text == "load") return TokenType::LOAD;
    if (text == "export") return TokenType::EXPORT;
    // ...
}
```

### 2. AST节点设计

```cpp
class ModuleNode : public ASTNode {
private:
    std::vector<std::string> loadItems_;        // 依赖列表
    std::map<std::string, Node*> exports_;      // 导出项
    std::string moduleType_ = "amd";            // 模块类型
    bool async_ = false;                        // 异步加载
    std::string baseUrl_ = "";                  // 基础路径
    
public:
    // 依赖管理
    void addLoadItem(const std::string& path);
    const std::vector<std::string>& getLoadItems() const;
    
    // 导出管理
    void addExport(const std::string& name, Node* value);
    const std::map<std::string, Node*>& getExports() const;
    
    // 配置管理
    void setModuleType(const std::string& type);
    void setAsync(bool async);
    void setBaseUrl(const std::string& url);
};
```

### 3. 解析器实现

```cpp
std::unique_ptr<ModuleNode> Parser::parseModule() {
    // 1. 匹配module关键字
    expect(TokenType::MODULE);
    
    // 2. 创建节点
    auto node = std::make_unique<ModuleNode>(current().location);
    
    // 3. 解析块内容
    expect(TokenType::LBRACE);
    parseModuleBody(node.get());
    expect(TokenType::RBRACE);
    
    return node;
}

void Parser::parseModuleBody(ModuleNode* module) {
    while (!isAtEnd() && !check(TokenType::RBRACE)) {
        if (match(TokenType::LOAD)) {
            parseLoadList(module);
        } else if (match(TokenType::EXPORT)) {
            parseExportMap(module);
        } else if (match(TokenType::CONFIG)) {
            parseConfig(module);
        }
    }
}
```

### 4. 代码生成阶段

#### AMD格式生成器
```cpp
void ModuleGenerator::generateAMD(ModuleNode* module) {
    output_ << "define([";
    
    // 生成依赖列表
    const auto& deps = module->getLoadItems();
    for (size_t i = 0; i < deps.size(); ++i) {
        output_ << "'" << resolvePath(deps[i]) << "'";
        if (i < deps.size() - 1) output_ << ", ";
    }
    
    output_ << "], function(";
    
    // 生成参数列表
    for (size_t i = 0; i < deps.size(); ++i) {
        output_ << toVariableName(deps[i]);
        if (i < deps.size() - 1) output_ << ", ";
    }
    
    output_ << ") {\n";
    
    // 生成模块体
    generateModuleBody(module);
    
    // 生成导出
    if (module->hasExports()) {
        output_ << "\n  return {\n";
        generateExports(module);
        output_ << "  };\n";
    }
    
    output_ << "});\n";
}
```

#### CommonJS格式生成器
```cpp
void ModuleGenerator::generateCommonJS(ModuleNode* module) {
    // 生成require语句
    for (const auto& dep : module->getLoadItems()) {
        std::string varName = toVariableName(dep);
        output_ << "const " << varName << " = require('"
                << resolvePath(dep) << "');\n";
    }
    
    output_ << "\n";
    
    // 生成模块体
    generateModuleBody(module);
    
    // 生成导出
    if (module->hasExports()) {
        for (const auto& [name, value] : module->getExports()) {
            output_ << "exports." << name << " = " << name << ";\n";
        }
    }
}
```

#### ES6格式生成器
```cpp
void ModuleGenerator::generateES6(ModuleNode* module) {
    // 生成import语句
    for (const auto& dep : module->getLoadItems()) {
        if (isRelativePath(dep)) {
            output_ << "import * as " << toVariableName(dep)
                    << " from '" << dep << "';\n";
        } else {
            // npm包
            output_ << "import " << dep << " from '" << dep << "';\n";
        }
    }
    
    output_ << "\n";
    
    // 生成模块体
    generateModuleBody(module);
    
    // 生成导出
    if (module->hasExports()) {
        output_ << "\nexport {\n";
        for (const auto& [name, _] : module->getExports()) {
            output_ << "  " << name << ",\n";
        }
        output_ << "};\n";
    }
}
```

### 5. 路径解析

```cpp
class ModuleResolver {
private:
    std::string baseUrl_;
    std::map<std::string, std::string> paths_;  // 路径映射
    std::vector<std::string> searchPaths_;       // 搜索路径
    
public:
    std::string resolvePath(const std::string& moduleId) {
        // 1. 检查是否为相对路径
        if (isRelativePath(moduleId)) {
            return resolveRelativePath(moduleId);
        }
        
        // 2. 检查路径映射
        auto it = paths_.find(moduleId);
        if (it != paths_.end()) {
            return it->second;
        }
        
        // 3. 在搜索路径中查找
        for (const auto& searchPath : searchPaths_) {
            std::string fullPath = joinPath(searchPath, moduleId);
            if (fileExists(fullPath + ".js") || 
                fileExists(fullPath + ".cjjs")) {
                return fullPath;
            }
        }
        
        // 4. 作为npm包处理
        return moduleId;
    }
    
    bool isRelativePath(const std::string& path) {
        return path.starts_with("./") || 
               path.starts_with("../") || 
               path.starts_with("/");
    }
};
```

### 6. 依赖分析

```cpp
class DependencyAnalyzer {
public:
    struct DependencyGraph {
        std::map<std::string, std::set<std::string>> dependencies;
        std::map<std::string, ModuleInfo> modules;
    };
    
    DependencyGraph analyze(const std::string& entryFile) {
        DependencyGraph graph;
        std::queue<std::string> toProcess;
        std::set<std::string> processed;
        
        toProcess.push(entryFile);
        
        while (!toProcess.empty()) {
            std::string current = toProcess.front();
            toProcess.pop();
            
            if (processed.count(current)) continue;
            processed.insert(current);
            
            // 解析模块
            auto moduleInfo = parseModule(current);
            graph.modules[current] = moduleInfo;
            
            // 添加依赖
            for (const auto& dep : moduleInfo.dependencies) {
                graph.dependencies[current].insert(dep);
                toProcess.push(dep);
            }
        }
        
        // 检查循环依赖
        checkCircularDependencies(graph);
        
        return graph;
    }
    
private:
    void checkCircularDependencies(const DependencyGraph& graph) {
        // 使用DFS检测循环
        std::set<std::string> visited;
        std::set<std::string> recursionStack;
        
        for (const auto& [module, _] : graph.modules) {
            if (!visited.count(module)) {
                if (hasCycle(module, graph, visited, recursionStack)) {
                    ErrorBuilder(ErrorLevel::ERROR, ErrorType::REFERENCE_ERROR)
                        .withMessage("Circular dependency detected")
                        .withDetail("Module: " + module)
                        .report();
                }
            }
        }
    }
};
```

### 7. 异步加载支持

```cpp
void ModuleGenerator::generateAsyncLoad(ModuleNode* module) {
    if (!module->isAsync()) return;
    
    output_ << "// Async module loading\n";
    output_ << "(async function() {\n";
    
    for (const auto& dep : module->getLoadItems()) {
        if (shouldLoadAsync(dep)) {
            output_ << "  const " << toVariableName(dep) 
                    << " = await import('" << dep << "');\n";
        }
    }
    
    output_ << "\n";
    generateModuleBody(module);
    output_ << "})();\n";
}
```

## 运行时机制

### 1. 模块加载器

```javascript
// 生成的运行时加载器代码
(function() {
    const modules = {};
    const loading = {};
    
    window.chtlRequire = function(deps, callback) {
        const loaded = [];
        let pending = deps.length;
        
        deps.forEach((dep, index) => {
            if (modules[dep]) {
                loaded[index] = modules[dep];
                if (--pending === 0) callback(...loaded);
            } else {
                loadModule(dep, function(module) {
                    loaded[index] = module;
                    if (--pending === 0) callback(...loaded);
                });
            }
        });
    };
    
    function loadModule(path, callback) {
        if (loading[path]) {
            loading[path].push(callback);
            return;
        }
        
        loading[path] = [callback];
        
        const script = document.createElement('script');
        script.src = resolvePath(path);
        script.onload = function() {
            const callbacks = loading[path];
            delete loading[path];
            callbacks.forEach(cb => cb(modules[path]));
        };
        document.head.appendChild(script);
    }
})();
```

### 2. 模块缓存

```cpp
class ModuleCache {
private:
    struct CacheEntry {
        std::string compiledCode;
        std::time_t timestamp;
        std::vector<std::string> dependencies;
    };
    
    std::map<std::string, CacheEntry> cache_;
    
public:
    bool isCached(const std::string& modulePath) {
        auto it = cache_.find(modulePath);
        if (it == cache_.end()) return false;
        
        // 检查文件是否已修改
        auto fileTime = getFileModificationTime(modulePath);
        return fileTime <= it->second.timestamp;
    }
    
    std::string getCached(const std::string& modulePath) {
        return cache_[modulePath].compiledCode;
    }
    
    void cache(const std::string& modulePath, 
               const std::string& code,
               const std::vector<std::string>& deps) {
        cache_[modulePath] = {
            code,
            std::time(nullptr),
            deps
        };
    }
};
```

## 优化策略

### 1. 打包优化
```cpp
class ModuleBundler {
public:
    std::string bundle(const std::vector<std::string>& modules) {
        std::stringstream output;
        
        // 生成模块定义
        output << "(function() {\n";
        output << "  const modules = {};\n\n";
        
        // 合并所有模块
        for (const auto& modulePath : modules) {
            output << "  modules['" << modulePath << "'] = ";
            output << "function(require, exports, module) {\n";
            output << getModuleCode(modulePath);
            output << "\n  };\n\n";
        }
        
        // 生成引导代码
        output << generateBootstrap();
        output << "})();\n";
        
        return output.str();
    }
};
```

### 2. Tree Shaking
```cpp
class TreeShaker {
public:
    std::set<std::string> getUsedExports(ModuleNode* module) {
        std::set<std::string> used;
        
        // 分析哪些导出被实际使用
        for (const auto& [name, node] : module->getExports()) {
            if (isExportUsed(name, module)) {
                used.insert(name);
            }
        }
        
        return used;
    }
    
    void removeUnusedExports(ModuleNode* module) {
        auto used = getUsedExports(module);
        
        // 移除未使用的导出
        auto& exports = module->getExports();
        for (auto it = exports.begin(); it != exports.end();) {
            if (!used.count(it->first)) {
                it = exports.erase(it);
            } else {
                ++it;
            }
        }
    }
};
```

## 总结

CHTL JS的module系统通过以下方式实现：

1. **语法层面**：提供简洁的AMD风格语法
2. **编译层面**：支持编译到多种模块格式（AMD/CommonJS/ES6）
3. **路径解析**：智能的模块路径解析和映射
4. **依赖管理**：自动分析和管理模块依赖
5. **优化支持**：打包、Tree Shaking等优化
6. **运行时支持**：提供模块加载器和缓存机制

这种设计让CHTL JS能够无缝集成现代JavaScript生态系统，同时保持语法的简洁性。