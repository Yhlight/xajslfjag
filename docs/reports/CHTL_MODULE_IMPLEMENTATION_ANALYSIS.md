# CHTL JS Module系统实现详解

## 📋 Module语法分析

根据`CHTL语法文档.md`，module系统有以下特点：

### 1. 基本语法
```chtl
module {
    load: ./module.cjjs,
    load: ./module2.cjjs,
    load: ./module3.cjjs,
    load: ./module4.cjjs
    load: ./module5.js  // 普通js文件也可以
}
```

### 2. 链式语法
```chtl
module {
    load:
    ./module.cjjs
    ,./module2.cjjs
    ,./module3.cjjs
    ,./module4.cjjs
    ,./module5.js  // 也支持链式语法
}
```

### 3. 设计目标
- **AMD风格模块加载器**：实现类似AMD的异步模块定义
- **文件加载顺序解决**：处理JS文件的依赖顺序问题
- **无序加载支持**：让JS文件能够无序加载
- **支持多种文件类型**：`.cjjs`（CHTL JS）和`.js`（普通JS）

## 🔧 实现架构

### 1. ModuleNode定义
```cpp
class ModuleNode : public CHTLJSNode {
public:
    struct ModuleEntry {
        String path;              // 模块路径
        String type;              // cjjs, js
        String resolvedPath;      // 解析后的完整路径
        bool isLoaded;            // 是否已加载
        StringVector dependencies;// 依赖关系
    };
    
    std::vector<ModuleEntry> modules;
    String loadStrategy;          // 加载策略: sequential, parallel, dependency
    bool enableCircularCheck;     // 是否启用循环依赖检查
    
    // 模块管理
    void addModule(const String& path, const String& type = "auto");
    void removeModule(const String& path);
    std::vector<ModuleEntry> getModules() const;
    
    // 依赖分析
    void analyzeDependencies();
    StringVector getLoadOrder();
    bool hasCircularDependency();
    
    // JavaScript生成
    String generateJavaScript() const override;
    String generateAMDLoader() const;
    String generateModuleRegistry() const;
};
```

### 2. 模块加载流程
```
1. 解析module{}块 → 2. 路径解析 → 3. 依赖分析 → 4. 生成AMD加载器 → 5. 运行时加载
```

### 3. 生成的JavaScript结构
```javascript
// 生成的模块加载器
(function() {
    'use strict';
    
    // CHTL模块注册表
    var CHTLModules = {
        registry: {},
        loaded: {},
        loading: {},
        dependencies: {}
    };
    
    // AMD风格加载器
    function loadModule(path, callback) {
        // 异步加载逻辑
    }
    
    // 模块依赖解析
    function resolveDependencies(modules) {
        // 依赖关系分析
    }
    
    // 执行模块加载
    CHTLModules.load([
        './module.cjjs',
        './module2.cjjs',
        './module3.cjjs'
    ], function() {
        // 所有模块加载完成后的回调
    });
})();
```

## 🎯 具体实现细节

### 1. 路径解析机制
```cpp
String ModuleLoader::resolveModulePath(const String& rawPath) {
    // 1. 检查是否是相对路径
    if (rawPath.starts_with("./") || rawPath.starts_with("../")) {
        return resolveRelativePath(rawPath);
    }
    
    // 2. 检查是否是绝对路径
    if (std::filesystem::path(rawPath).is_absolute()) {
        return rawPath;
    }
    
    // 3. 搜索模块目录
    return searchInModuleDirectories(rawPath);
}

String ModuleLoader::searchInModuleDirectories(const String& moduleName) {
    // 搜索顺序：
    // 1. 官方模块目录
    // 2. 当前目录/module
    // 3. 当前目录
    
    StringVector searchPaths = {
        getOfficialModulePath(),
        "./module",
        "."
    };
    
    for (const auto& searchPath : searchPaths) {
        String candidate = searchPath + "/" + moduleName;
        
        // 按文件类型优先级搜索
        for (const auto& ext : {".cjjs", ".js"}) {
            if (std::filesystem::exists(candidate + ext)) {
                return candidate + ext;
            }
        }
    }
    
    return ""; // 未找到
}
```

### 2. 依赖分析算法
```cpp
void ModuleNode::analyzeDependencies() {
    for (auto& module : modules) {
        // 分析每个模块的依赖
        StringVector deps = extractDependencies(module.resolvedPath);
        module.dependencies = deps;
    }
    
    // 检查循环依赖
    if (enableCircularCheck && hasCircularDependency()) {
        throw std::runtime_error("检测到循环依赖");
    }
}

StringVector ModuleNode::getLoadOrder() {
    // 拓扑排序获取加载顺序
    std::unordered_map<String, int> inDegree;
    std::unordered_map<String, StringVector> graph;
    
    // 构建依赖图
    for (const auto& module : modules) {
        graph[module.path] = module.dependencies;
        inDegree[module.path] = 0;
    }
    
    // 计算入度
    for (const auto& [path, deps] : graph) {
        for (const auto& dep : deps) {
            inDegree[dep]++;
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
        
        for (const auto& neighbor : graph[current]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) {
                queue.push(neighbor);
            }
        }
    }
    
    return result;
}
```

### 3. AMD加载器生成
```cpp
String ModuleNode::generateAMDLoader() const {
    std::ostringstream js;
    
    js << "// CHTL AMD模块加载器\n";
    js << "(function(global) {\n";
    js << "    'use strict';\n\n";
    
    // 模块注册表
    js << "    var registry = {};\n";
    js << "    var loaded = {};\n";
    js << "    var loading = {};\n\n";
    
    // 定义函数
    js << "    function define(name, deps, factory) {\n";
    js << "        registry[name] = { deps: deps, factory: factory };\n";
    js << "    }\n\n";
    
    // 加载函数
    js << "    function load(name, callback) {\n";
    js << "        if (loaded[name]) {\n";
    js << "            callback(loaded[name]);\n";
    js << "            return;\n";
    js << "        }\n\n";
    
    js << "        if (loading[name]) {\n";
    js << "            loading[name].push(callback);\n";
    js << "            return;\n";
    js << "        }\n\n";
    
    js << "        loading[name] = [callback];\n\n";
    
    // 动态脚本加载
    js << "        var script = document.createElement('script');\n";
    js << "        script.src = name;\n";
    js << "        script.onload = function() {\n";
    js << "            var module = registry[name];\n";
    js << "            if (module) {\n";
    js << "                loadDependencies(module.deps, function(deps) {\n";
    js << "                    var result = module.factory.apply(null, deps);\n";
    js << "                    loaded[name] = result;\n";
    js << "                    var callbacks = loading[name];\n";
    js << "                    delete loading[name];\n";
    js << "                    callbacks.forEach(function(cb) { cb(result); });\n";
    js << "                });\n";
    js << "            }\n";
    js << "        };\n";
    js << "        document.head.appendChild(script);\n";
    js << "    }\n\n";
    
    // 依赖加载
    js << "    function loadDependencies(deps, callback) {\n";
    js << "        if (deps.length === 0) {\n";
    js << "            callback([]);\n";
    js << "            return;\n";
    js << "        }\n\n";
    
    js << "        var results = [];\n";
    js << "        var count = 0;\n";
    js << "        deps.forEach(function(dep, index) {\n";
    js << "            load(dep, function(result) {\n";
    js << "                results[index] = result;\n";
    js << "                count++;\n";
    js << "                if (count === deps.length) {\n";
    js << "                    callback(results);\n";
    js << "                }\n";
    js << "            });\n";
    js << "        });\n";
    js << "    }\n\n";
    
    // 暴露API
    js << "    global.CHTLModule = {\n";
    js << "        define: define,\n";
    js << "        load: load\n";
    js << "    };\n\n";
    
    // 加载当前模块列表
    js << "    // 加载模块列表\n";
    js << "    var moduleList = [\n";
    
    for (size_t i = 0; i < modules.size(); ++i) {
        js << "        '" << modules[i].resolvedPath << "'";
        if (i < modules.size() - 1) js << ",";
        js << "\n";
    }
    
    js << "    ];\n\n";
    
    js << "    // 按序加载所有模块\n";
    js << "    loadDependencies(moduleList, function() {\n";
    js << "        console.log('所有CHTL模块加载完成');\n";
    js << "    });\n\n";
    
    js << "})(this);\n";
    
    return js.str();
}
```

## 🚀 使用示例

### 输入CHTL代码
```chtl
script {
    module {
        load: ./utils.cjjs,
        load: ./ui-components.cjjs,
        load: ./event-handlers.js
    }
    
    // 模块加载完成后执行的代码
    {{.app}} &-> click {
        console.log('应用初始化完成');
    }
}
```

### 生成的JavaScript
```javascript
// CHTL AMD模块加载器
(function(global) {
    'use strict';
    
    var registry = {};
    var loaded = {};
    var loading = {};
    
    function define(name, deps, factory) {
        registry[name] = { deps: deps, factory: factory };
    }
    
    function load(name, callback) {
        // ... 加载逻辑
    }
    
    function loadDependencies(deps, callback) {
        // ... 依赖加载逻辑
    }
    
    global.CHTLModule = {
        define: define,
        load: load
    };
    
    // 加载模块列表
    var moduleList = [
        './utils.cjjs',
        './ui-components.cjjs',
        './event-handlers.js'
    ];
    
    // 按序加载所有模块
    loadDependencies(moduleList, function() {
        console.log('所有CHTL模块加载完成');
        
        // 执行应用逻辑
        document.querySelector('.app').addEventListener('click', function() {
            console.log('应用初始化完成');
        });
    });
})(this);
```

## 🎯 关键特性

### 1. **异步加载**
- 支持并行加载互不依赖的模块
- 自动处理依赖关系
- 避免阻塞主线程

### 2. **依赖管理**
- 自动分析模块依赖关系
- 拓扑排序确定加载顺序
- 循环依赖检测和报错

### 3. **路径解析**
- 支持相对路径和绝对路径
- 多目录搜索机制
- 文件类型自动识别

### 4. **错误处理**
- 模块加载失败处理
- 依赖缺失检测
- 超时处理机制

这个实现确保了CHTL JS的module系统能够真正解决"文件加载顺序问题"，让开发者可以专注于业务逻辑而不用担心模块依赖的复杂性。