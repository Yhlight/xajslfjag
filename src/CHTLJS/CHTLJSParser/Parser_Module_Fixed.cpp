#include "Parser.h"
#include "../CHTLJSNode/ModuleNode.h"
#include "../../CHTL/CHTLLexer/GlobalMap.h"
#include <regex>
#include <algorithm>

namespace CHTLJS {

// 修正后的模块解析实现
std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseModuleBlock() {
    CHTL::Position pos = currentToken.position;
    advance(); // 消费 'module'
    
    auto moduleNode = std::make_unique<CHTL::ModuleNode>("", pos);
    
    consume(CHTL::TokenType::LBRACE, "期望 '{'");
    
    while (!check(CHTL::TokenType::RBRACE) && !isAtEnd()) {
        skipWhitespaceAndComments();
        
        if (check(CHTL::TokenType::LOAD)) {
            parseModuleLoad(moduleNode.get());
        } else if (check(CHTL::TokenType::LITERAL_STRING) || 
                   check(CHTL::TokenType::LITERAL_UNQUOTED)) {
            // 解析简化语法：直接的路径字面量
            parseModuleSimpleSyntax(moduleNode.get());
        } else if (check(CHTL::TokenType::COMMA)) {
            advance(); // 跳过逗号
        } else {
            reportError("模块块中期望 'load:' 或模块路径");
            advance(); // 尝试恢复
        }
    }
    
    consume(CHTL::TokenType::RBRACE, "期望 '}'");
    
    // 配置模块加载策略
    configureModuleStrategy(moduleNode.get());
    
    return std::unique_ptr<CHTL::BaseNode>(moduleNode.release());
}

void CHTLJSParser::parseModuleLoad(CHTL::ModuleNode* moduleNode) {
    advance(); // 消费 'load'
    consume(CHTL::TokenType::COLON, "期望 ':'");
    
    // 支持单个模块和链式语法
    if (check(CHTL::TokenType::NEWLINE)) {
        // 链式语法：load: \n module1 \n ,module2 \n ,module3
        advance(); // 跳过换行
        parseChainedModules(moduleNode);
    } else {
        // 单个模块：load: ./module.js
        CHTL::String modulePath = parseLiteral();
        if (!modulePath.empty()) {
            moduleNode->addModule(modulePath, "auto");
        }
        
        // 检查是否有逗号分隔的多个模块
        while (check(CHTL::TokenType::COMMA)) {
            advance(); // 消费逗号
            skipWhitespaceAndComments();
            
            CHTL::String nextPath = parseLiteral();
            if (!nextPath.empty()) {
                moduleNode->addModule(nextPath, "auto");
            }
        }
    }
}

void CHTLJSParser::parseChainedModules(CHTL::ModuleNode* moduleNode) {
    while (!check(CHTL::TokenType::RBRACE) && !isAtEnd()) {
        skipWhitespaceAndComments();
        
        // 检查逗号前缀（链式语法特征）
        bool hasCommaPrefix = false;
        if (check(CHTL::TokenType::COMMA)) {
            hasCommaPrefix = true;
            advance(); // 消费逗号
            skipWhitespaceAndComments();
        }
        
        if (check(CHTL::TokenType::LITERAL_STRING) || 
            check(CHTL::TokenType::LITERAL_UNQUOTED)) {
            
            CHTL::String modulePath = parseLiteral();
            if (!modulePath.empty()) {
                // 检测模块类型
                CHTL::String moduleType = detectModuleTypeFromPath(modulePath);
                
                // 添加模块
                moduleNode->addModule(modulePath, moduleType);
                
                // 记录链式语法元数据
                if (hasCommaPrefix) {
                    moduleNode->setJSMetadata("syntax_style", "chained");
                }
            }
        } else if (check(CHTL::TokenType::LOAD)) {
            // 嵌套的load语法
            parseModuleLoad(moduleNode);
        } else if (!check(CHTL::TokenType::RBRACE)) {
            break; // 结束解析
        }
    }
}

void CHTLJSParser::parseModuleSimpleSyntax(CHTL::ModuleNode* moduleNode) {
    // 解析直接的模块路径（没有load:前缀）
    CHTL::String modulePath = parseLiteral();
    if (!modulePath.empty()) {
        CHTL::String moduleType = detectModuleTypeFromPath(modulePath);
        moduleNode->addModule(modulePath, moduleType);
        moduleNode->setJSMetadata("syntax_style", "simple");
    }
}

CHTL::String CHTLJSParser::detectModuleTypeFromPath(const CHTL::String& path) {
    if (path.ends_with(".cjjs")) {
        return "cjjs";
    } else if (path.ends_with(".js") || path.ends_with(".mjs")) {
        return "js";
    } else {
        return "auto"; // 自动检测
    }
}

void CHTLJSParser::configureModuleStrategy(CHTL::ModuleNode* moduleNode) {
    // 根据配置设置模块加载策略
    if (config.enableParallelModuleLoading) {
        moduleNode->setLoadStrategy(CHTL::ModuleLoadStrategy::PARALLEL);
    } else if (config.enableDependencyAnalysis) {
        moduleNode->setLoadStrategy(CHTL::ModuleLoadStrategy::DEPENDENCY);
    } else {
        moduleNode->setLoadStrategy(CHTL::ModuleLoadStrategy::SEQUENTIAL);
    }
    
    // 配置其他选项
    moduleNode->setCircularCheck(config.enableCircularDependencyCheck);
    moduleNode->setAsyncLoading(config.enableAsyncModuleLoading);
    moduleNode->setLoadTimeout(config.moduleLoadTimeout);
    
    // 设置基础路径
    if (!config.moduleBasePath.empty()) {
        moduleNode->setBasePath(config.moduleBasePath);
    }
}

// 增强的模块解析支持
bool CHTLJSParser::isModuleStatement() const {
    return check(CHTL::TokenType::MODULE);
}

void CHTLJSParser::validateModuleBlock(CHTL::ModuleNode* moduleNode) {
    if (!moduleNode) return;
    
    // 验证模块路径
    if (!moduleNode->validateModulePaths()) {
        auto errors = moduleNode->getPathValidationErrors();
        for (const auto& error : errors) {
            reportError("模块验证错误: " + error);
        }
    }
    
    // 检查循环依赖
    if (moduleNode->getCircularCheck()) {
        try {
            moduleNode->analyzeDependencies();
            if (moduleNode->hasCircularDependency()) {
                reportError("检测到模块循环依赖");
            }
        } catch (const std::exception& e) {
            reportWarning("依赖分析失败: " + CHTL::String(e.what()));
        }
    }
    
    // 验证模块数量
    if (moduleNode->getModuleCount() == 0) {
        reportWarning("模块块为空，没有指定任何模块");
    } else if (moduleNode->getModuleCount() > config.maxModulesPerBlock) {
        reportWarning("模块数量过多（" + std::to_string(moduleNode->getModuleCount()) + 
                     "），可能影响性能");
    }
}

// 模块生成优化
void CHTLJSParser::optimizeModuleGeneration(CHTL::ModuleNode* moduleNode) {
    if (!moduleNode || !config.enableModuleOptimization) return;
    
    // 优化加载顺序
    if (config.optimizeLoadOrder) {
        try {
            auto loadOrder = moduleNode->getLoadOrder();
            moduleNode->setJSMetadata("optimized_order", CHTL::join(loadOrder, ","));
        } catch (const std::exception& e) {
            reportWarning("加载顺序优化失败: " + CHTL::String(e.what()));
        }
    }
    
    // 合并相同目录的模块
    if (config.mergeModulesByDirectory) {
        mergeSameDirectoryModules(moduleNode);
    }
    
    // 预加载重要模块
    if (config.enableModulePreloading) {
        markCriticalModules(moduleNode);
    }
}

void CHTLJSParser::mergeSameDirectoryModules(CHTL::ModuleNode* moduleNode) {
    auto modules = moduleNode->getModules();
    std::unordered_map<CHTL::String, std::vector<CHTL::String>> directoryGroups;
    
    // 按目录分组
    for (const auto& module : modules) {
        CHTL::String dir = extractDirectory(module.path);
        directoryGroups[dir].push_back(module.path);
    }
    
    // 记录分组信息
    for (const auto& [dir, paths] : directoryGroups) {
        if (paths.size() > 1) {
            CHTL::String groupKey = "directory_group_" + dir;
            moduleNode->setJSMetadata(groupKey, CHTL::join(paths, ","));
        }
    }
}

void CHTLJSParser::markCriticalModules(CHTL::ModuleNode* moduleNode) {
    auto modules = moduleNode->getModules();
    CHTL::StringVector criticalModules;
    
    // 标记关键模块（启发式规则）
    for (const auto& module : modules) {
        if (isCriticalModule(module.path)) {
            criticalModules.push_back(module.path);
        }
    }
    
    if (!criticalModules.empty()) {
        moduleNode->setJSMetadata("critical_modules", CHTL::join(criticalModules, ","));
    }
}

bool CHTLJSParser::isCriticalModule(const CHTL::String& path) const {
    // 启发式判断关键模块
    CHTL::StringVector criticalPatterns = {
        "core", "main", "app", "index", "init", "bootstrap"
    };
    
    CHTL::String lowercasePath = toLowerCase(path);
    for (const auto& pattern : criticalPatterns) {
        if (lowercasePath.find(pattern) != CHTL::String::npos) {
            return true;
        }
    }
    
    return false;
}

CHTL::String CHTLJSParser::extractDirectory(const CHTL::String& path) const {
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != CHTL::String::npos) {
        return path.substr(0, lastSlash);
    }
    return "."; // 当前目录
}

CHTL::String CHTLJSParser::toLowerCase(const CHTL::String& str) const {
    CHTL::String result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// 生成增强的AMD模块系统
CHTL::String CHTLJSParser::generateEnhancedModuleSystem(const CHTL::ModuleNode* moduleNode) const {
    if (!moduleNode) return "";
    
    std::ostringstream js;
    
    js << "// Enhanced CHTL Module System\n";
    js << "// Generated from CHTL JS module block\n";
    js << "(function(global) {\n";
    js << "    'use strict';\n\n";
    
    // 生成配置信息
    js << "    // Module Configuration\n";
    js << "    var config = {\n";
    js << "        strategy: '" << moduleStrategyToString(moduleNode->getLoadStrategy()) << "',\n";
    js << "        async: " << (moduleNode->getAsyncLoading() ? "true" : "false") << ",\n";
    js << "        timeout: " << moduleNode->getLoadTimeout() << ",\n";
    js << "        checkCircular: " << (moduleNode->getCircularCheck() ? "true" : "false") << ",\n";
    js << "        basePath: '" << moduleNode->getBasePath() << "'\n";
    js << "    };\n\n";
    
    // 生成模块列表
    js << "    // Module List\n";
    js << "    var modules = [\n";
    auto modules = moduleNode->getModules();
    for (size_t i = 0; i < modules.size(); ++i) {
        js << "        {\n";
        js << "            path: '" << modules[i].path << "',\n";
        js << "            resolved: '" << modules[i].resolvedPath << "',\n";
        js << "            type: '" << modules[i].type << "',\n";
        js << "            name: '" << modules[i].name << "'\n";
        js << "        }";
        if (i < modules.size() - 1) js << ",";
        js << "\n";
    }
    js << "    ];\n\n";
    
    // 生成核心模块系统
    js << moduleNode->generateJavaScript();
    
    js << "})(typeof window !== 'undefined' ? window : this);\n";
    
    return js.str();
}

CHTL::String CHTLJSParser::moduleStrategyToString(CHTL::ModuleLoadStrategy strategy) const {
    switch (strategy) {
        case CHTL::ModuleLoadStrategy::SEQUENTIAL: return "sequential";
        case CHTL::ModuleLoadStrategy::PARALLEL: return "parallel";
        case CHTL::ModuleLoadStrategy::DEPENDENCY: return "dependency";
        default: return "sequential";
    }
}

} // namespace CHTLJS