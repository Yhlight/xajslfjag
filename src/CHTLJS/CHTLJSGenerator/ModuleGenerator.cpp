#include "Generator.h"
#include "../CHTLJSNode/ModuleNode.h"
#include "../../Error/ErrorReport.h"

namespace CHTL {

// 访问ModuleNode并生成代码
void CHTLJSGenerator::visitModuleNode(ModuleNode* node) {
    // 根据配置的模块类型生成不同格式
    std::string moduleType = node->getModuleType();
    
    if (moduleType == "amd") {
        generateAMDModule(node);
    } else if (moduleType == "commonjs") {
        generateCommonJSModule(node);
    } else if (moduleType == "es6") {
        generateES6Module(node);
    } else {
        // 默认使用AMD
        generateAMDModule(node);
    }
}

// 生成AMD格式模块
void CHTLJSGenerator::generateAMDModule(ModuleNode* node) {
    const auto& deps = node->getLoadItems();
    
    // define(['dep1', 'dep2'], function(dep1, dep2) {
    output_ << "define([";
    
    // 依赖列表
    for (size_t i = 0; i < deps.size(); ++i) {
        output_ << "'" << resolveDependencyPath(deps[i]) << "'";
        if (i < deps.size() - 1) output_ << ", ";
    }
    
    output_ << "], function(";
    
    // 参数列表
    for (size_t i = 0; i < deps.size(); ++i) {
        output_ << ModuleNode::toVariableName(deps[i]);
        if (i < deps.size() - 1) output_ << ", ";
    }
    
    output_ << ") {\n";
    increaseIndent();
    
    // 如果是异步模块，包装在async函数中
    if (node->isAsync()) {
        writeIndent();
        output_ << "return (async function() {\n";
        increaseIndent();
    }
    
    // 生成模块主体（后续的CHTL JS代码）
    generateModuleBody();
    
    // 生成导出
    if (node->hasExports()) {
        generateExports(node);
    }
    
    if (node->isAsync()) {
        decreaseIndent();
        writeIndent();
        output_ << "})();\n";
    }
    
    decreaseIndent();
    output_ << "});\n";
}

// 生成CommonJS格式模块
void CHTLJSGenerator::generateCommonJSModule(ModuleNode* node) {
    const auto& deps = node->getLoadItems();
    
    // 生成require语句
    for (const auto& dep : deps) {
        std::string varName = ModuleNode::toVariableName(dep);
        std::string depPath = resolveDependencyPath(dep);
        
        output_ << "const " << varName << " = require('" << depPath << "');\n";
    }
    
    if (!deps.empty()) {
        output_ << "\n";
    }
    
    // 异步包装
    if (node->isAsync()) {
        output_ << "(async function() {\n";
        increaseIndent();
    }
    
    // 生成模块主体
    generateModuleBody();
    
    // 生成exports
    if (node->hasExports()) {
        output_ << "\n";
        for (const auto& [name, value] : node->getExports()) {
            output_ << "module.exports." << name << " = " << name << ";\n";
        }
    }
    
    if (node->isAsync()) {
        decreaseIndent();
        output_ << "})();\n";
    }
}

// 生成ES6格式模块
void CHTLJSGenerator::generateES6Module(ModuleNode* node) {
    const auto& deps = node->getLoadItems();
    
    // 生成import语句
    for (const auto& dep : deps) {
        if (ModuleNode::isRelativePath(dep)) {
            // 相对路径：import * as name from './path'
            std::string varName = ModuleNode::toVariableName(dep);
            output_ << "import * as " << varName << " from '" << dep << "';\n";
        } else if (dep.find('/') != std::string::npos) {
            // 深层导入：import name from 'package/module'
            std::string varName = ModuleNode::toVariableName(dep);
            output_ << "import " << varName << " from '" << dep << "';\n";
        } else {
            // 默认导入：import package from 'package'
            output_ << "import " << dep << " from '" << dep << "';\n";
        }
    }
    
    if (!deps.empty()) {
        output_ << "\n";
    }
    
    // 生成模块主体
    generateModuleBody();
    
    // 生成export语句
    if (node->hasExports()) {
        output_ << "\n";
        
        const auto& exports = node->getExports();
        if (exports.size() == 1 && exports.begin()->first == "default") {
            // 默认导出
            output_ << "export default " << exports.begin()->first << ";\n";
        } else {
            // 命名导出
            output_ << "export {\n";
            increaseIndent();
            
            for (const auto& [name, value] : exports) {
                writeIndent();
                output_ << name;
                if (name != value) {
                    output_ << " as " << value;
                }
                output_ << ",\n";
            }
            
            decreaseIndent();
            output_ << "};\n";
        }
    }
}

// 解析依赖路径
std::string CHTLJSGenerator::resolveDependencyPath(const std::string& dep) {
    // 如果设置了baseUrl，处理相对路径
    if (!currentModule_->getBaseUrl().empty() && !ModuleNode::isRelativePath(dep)) {
        return currentModule_->getBaseUrl() + "/" + dep;
    }
    
    // 处理CHTL模块（.chtl -> .js）
    if (dep.ends_with(".chtl")) {
        return dep.substr(0, dep.length() - 5) + ".js";
    }
    
    // 处理CHTL JS模块（.cjjs -> .js）
    if (dep.ends_with(".cjjs")) {
        return dep.substr(0, dep.length() - 5) + ".js";
    }
    
    // 添加.js扩展名（如果没有）
    if (ModuleNode::isRelativePath(dep) && 
        dep.find('.') == std::string::npos) {
        return dep + ".js";
    }
    
    return dep;
}

// 生成模块体（占位，实际由后续代码填充）
void CHTLJSGenerator::generateModuleBody() {
    // 这里将插入module块之后的所有CHTL JS代码
    // Parser会继续解析module块后的内容
    
    // 设置模块上下文标记
    inModuleContext_ = true;
    currentModule_ = currentNode_;
}

// 生成导出
void CHTLJSGenerator::generateExports(ModuleNode* node) {
    const auto& exports = node->getExports();
    
    writeIndent();
    output_ << "// Module exports\n";
    
    if (node->getModuleType() == "amd") {
        // AMD返回导出对象
        writeIndent();
        output_ << "return {\n";
        increaseIndent();
        
        for (const auto& [name, value] : exports) {
            writeIndent();
            output_ << name << ": " << value << ",\n";
        }
        
        decreaseIndent();
        writeIndent();
        output_ << "};\n";
    }
}

// 生成运行时加载器（用于开发模式）
void CHTLJSGenerator::generateRuntimeLoader() {
    output_ << R"(
// CHTL JS Runtime Module Loader
(function(global) {
    'use strict';
    
    const modules = {};
    const loading = {};
    const callbacks = {};
    
    // 模块定义函数（AMD风格）
    global.define = function(deps, factory) {
        const currentScript = document.currentScript;
        const moduleId = currentScript ? currentScript.src : 'anonymous';
        
        // 等待依赖加载
        loadDependencies(deps, function(...args) {
            const module = factory.apply(null, args);
            modules[moduleId] = module;
            
            // 触发回调
            if (callbacks[moduleId]) {
                callbacks[moduleId].forEach(cb => cb(module));
                delete callbacks[moduleId];
            }
        });
    };
    
    // 加载依赖
    function loadDependencies(deps, callback) {
        let pending = deps.length;
        const loaded = new Array(deps.length);
        
        if (pending === 0) {
            callback();
            return;
        }
        
        deps.forEach((dep, index) => {
            loadModule(dep, function(module) {
                loaded[index] = module;
                if (--pending === 0) {
                    callback(...loaded);
                }
            });
        });
    }
    
    // 加载模块
    function loadModule(id, callback) {
        // 检查缓存
        if (modules[id]) {
            callback(modules[id]);
            return;
        }
        
        // 检查是否正在加载
        if (loading[id]) {
            if (!callbacks[id]) {
                callbacks[id] = [];
            }
            callbacks[id].push(callback);
            return;
        }
        
        // 开始加载
        loading[id] = true;
        
        const script = document.createElement('script');
        script.src = resolveModulePath(id);
        script.async = true;
        
        script.onload = function() {
            delete loading[id];
            // define函数会处理回调
        };
        
        script.onerror = function() {
            delete loading[id];
            console.error('Failed to load module:', id);
        };
        
        document.head.appendChild(script);
    }
    
    // 解析模块路径
    function resolveModulePath(id) {
        // 简单实现，实际应该更复杂
        if (id.startsWith('./') || id.startsWith('../')) {
            return id;
        }
        return '/modules/' + id + '.js';
    }
    
})(window);
)";
}

} // namespace CHTL