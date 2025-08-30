#include "ANTLRCompiler.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// ========== ANTLRCSSCompiler::Impl 实现 ==========

class ANTLRCSSCompiler::Impl {
public:
    Impl() {
        // 在实际项目中，这里会初始化ANTLR4的CSS词法器和语法分析器
        // 由于我们没有实际的ANTLR4运行时，这里提供模拟实现
    }
    
    std::shared_ptr<CSSASTNode> parseCSS(const std::string& cssCode) {
        // 模拟CSS解析
        auto root = std::make_shared<CSSASTNode>("stylesheet");
        
        // 简化的CSS规则解析
        std::regex rulePattern(R"(([^{]+)\{([^}]*)\})");
        std::sregex_iterator iter(cssCode.begin(), cssCode.end(), rulePattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            std::string selector = match[1].str();
            std::string declarations = match[2].str();
            
            auto ruleNode = std::make_shared<CSSASTNode>("rule");
            
            auto selectorNode = std::make_shared<CSSASTNode>("selector");
            selectorNode->value = selector;
            ruleNode->children.push_back(selectorNode);
            
            auto declarationBlock = std::make_shared<CSSASTNode>("declaration_block");
            
            // 解析声明
            std::regex declPattern(R"(([^:]+):([^;]+);?)");
            std::sregex_iterator declIter(declarations.begin(), declarations.end(), declPattern);
            std::sregex_iterator declEnd;
            
            for (; declIter != declEnd; ++declIter) {
                std::smatch declMatch = *declIter;
                auto declNode = std::make_shared<CSSASTNode>("declaration");
                declNode->attributes["property"] = declMatch[1].str();
                declNode->attributes["value"] = declMatch[2].str();
                declarationBlock->children.push_back(declNode);
            }
            
            ruleNode->children.push_back(declarationBlock);
            root->children.push_back(ruleNode);
        }
        
        return root;
    }
    
    std::string generateCSS(std::shared_ptr<CSSASTNode> astRoot) {
        std::ostringstream output;
        generateFromAST(astRoot, output);
        return output.str();
    }
    
private:
    void generateFromAST(std::shared_ptr<CSSASTNode> node, std::ostringstream& output) {
        if (!node) return;
        
        if (node->type == "rule") {
            // 生成规则
            for (auto& child : node->children) {
                if (child->type == "selector") {
                    output << child->value << " {\n";
                } else if (child->type == "declaration_block") {
                    for (auto& decl : child->children) {
                        if (decl->type == "declaration") {
                            output << "    " << decl->attributes["property"] 
                                   << ": " << decl->attributes["value"] << ";\n";
                        }
                    }
                }
            }
            output << "}\n\n";
        } else {
            // 递归处理子节点
            for (auto& child : node->children) {
                generateFromAST(child, output);
            }
        }
    }
};

// ========== ANTLRJavaScriptCompiler::Impl 实现 ==========

class ANTLRJavaScriptCompiler::Impl {
public:
    Impl() {
        // 在实际项目中，这里会初始化ANTLR4的JavaScript词法器和语法分析器
    }
    
    std::shared_ptr<JSASTNode> parseJavaScript(const std::string& jsCode) {
        // 模拟JavaScript解析
        auto root = std::make_shared<JSASTNode>("program");
        
        // 简化的函数解析
        std::regex functionPattern(R"(function\s+(\w+)\s*\(([^)]*)\)\s*\{([^}]*)\})");
        std::sregex_iterator iter(jsCode.begin(), jsCode.end(), functionPattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            std::string funcName = match[1].str();
            std::string params = match[2].str();
            std::string body = match[3].str();
            
            auto funcNode = std::make_shared<JSASTNode>("function_declaration");
            funcNode->attributes["name"] = funcName;
            funcNode->attributes["parameters"] = params;
            funcNode->attributes["body"] = body;
            
            root->children.push_back(funcNode);
        }
        
        // 简化的变量声明解析
        std::regex varPattern(R"((var|let|const)\s+(\w+)\s*=\s*([^;]+);?)");
        std::sregex_iterator varIter(jsCode.begin(), jsCode.end(), varPattern);
        std::sregex_iterator varEnd;
        
        for (; varIter != varEnd; ++varIter) {
            std::smatch varMatch = *varIter;
            std::string varType = varMatch[1].str();
            std::string varName = varMatch[2].str();
            std::string varValue = varMatch[3].str();
            
            auto varNode = std::make_shared<JSASTNode>("variable_declaration");
            varNode->attributes["type"] = varType;
            varNode->attributes["name"] = varName;
            varNode->attributes["value"] = varValue;
            
            root->children.push_back(varNode);
        }
        
        return root;
    }
    
    std::string generateJavaScript(std::shared_ptr<JSASTNode> astRoot) {
        std::ostringstream output;
        generateFromAST(astRoot, output);
        return output.str();
    }
    
private:
    void generateFromAST(std::shared_ptr<JSASTNode> node, std::ostringstream& output) {
        if (!node) return;
        
        if (node->type == "function_declaration") {
            output << "function " << node->attributes["name"] 
                   << "(" << node->attributes["parameters"] << ") {\n";
            output << "    " << node->attributes["body"] << "\n";
            output << "}\n\n";
        } else if (node->type == "variable_declaration") {
            output << node->attributes["type"] << " " << node->attributes["name"]
                   << " = " << node->attributes["value"] << ";\n";
        } else {
            // 递归处理子节点
            for (auto& child : node->children) {
                generateFromAST(child, output);
            }
        }
    }
};

// ========== ANTLRCSSCompiler 实现 ==========

ANTLRCSSCompiler::ANTLRCSSCompiler() {
    m_impl = std::make_unique<Impl>();
}

ANTLRCSSCompiler::~ANTLRCSSCompiler() = default;

std::shared_ptr<CSSASTNode> ANTLRCSSCompiler::parseCSS(const std::string& cssCode) {
    return m_impl->parseCSS(cssCode);
}

std::string ANTLRCSSCompiler::generateCSS(std::shared_ptr<CSSASTNode> astRoot) {
    return m_impl->generateCSS(astRoot);
}

bool ANTLRCSSCompiler::validateCSS(const std::string& cssCode, std::vector<std::string>& errors) {
    try {
        auto ast = parseCSS(cssCode);
        return ast != nullptr;
    } catch (const std::exception& e) {
        errors.push_back(e.what());
        return false;
    }
}

std::string ANTLRCSSCompiler::optimizeCSS(const std::string& cssCode) {
    auto ast = parseCSS(cssCode);
    if (!ast) return cssCode;
    
    // 这里可以实现各种CSS优化
    // 如：移除重复规则、合并选择器、压缩等
    
    return generateCSS(ast);
}

std::vector<std::string> ANTLRCSSCompiler::extractSelectors(const std::string& cssCode) {
    std::vector<std::string> selectors;
    
    std::regex selectorPattern(R"(([^{]+)\{)");
    std::sregex_iterator iter(cssCode.begin(), cssCode.end(), selectorPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string selector = match[1].str();
        
        // 清理选择器
        selector.erase(0, selector.find_first_not_of(" \t\r\n"));
        selector.erase(selector.find_last_not_of(" \t\r\n") + 1);
        
        selectors.push_back(selector);
    }
    
    return selectors;
}

std::unordered_map<std::string, std::string> ANTLRCSSCompiler::extractProperties(const std::string& ruleContent) {
    std::unordered_map<std::string, std::string> properties;
    
    std::regex propPattern(R"(([^:]+):([^;]+);?)");
    std::sregex_iterator iter(ruleContent.begin(), ruleContent.end(), propPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string prop = match[1].str();
        std::string value = match[2].str();
        
        // 清理属性名和值
        prop.erase(0, prop.find_first_not_of(" \t\r\n"));
        prop.erase(prop.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        properties[prop] = value;
    }
    
    return properties;
}

std::string ANTLRCSSCompiler::formatCSS(const std::string& cssCode) {
    auto ast = parseCSS(cssCode);
    if (!ast) return cssCode;
    
    return generateCSS(ast);
}

void ANTLRCSSCompiler::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "ANTLR CSS Compiler: " + message);
}

// ========== ANTLRJavaScriptCompiler 实现 ==========

ANTLRJavaScriptCompiler::ANTLRJavaScriptCompiler() {
    m_impl = std::make_unique<Impl>();
}

ANTLRJavaScriptCompiler::~ANTLRJavaScriptCompiler() = default;

std::shared_ptr<JSASTNode> ANTLRJavaScriptCompiler::parseJavaScript(const std::string& jsCode) {
    return m_impl->parseJavaScript(jsCode);
}

std::string ANTLRJavaScriptCompiler::generateJavaScript(std::shared_ptr<JSASTNode> astRoot) {
    return m_impl->generateJavaScript(astRoot);
}

bool ANTLRJavaScriptCompiler::validateJavaScript(const std::string& jsCode, std::vector<std::string>& errors) {
    try {
        auto ast = parseJavaScript(jsCode);
        return ast != nullptr;
    } catch (const std::exception& e) {
        errors.push_back(e.what());
        return false;
    }
}

std::string ANTLRJavaScriptCompiler::optimizeJavaScript(const std::string& jsCode) {
    auto ast = parseJavaScript(jsCode);
    if (!ast) return jsCode;
    
    // 这里可以实现各种JS优化
    // 如：死代码消除、变量重命名、表达式简化等
    
    return generateJavaScript(ast);
}

std::vector<std::string> ANTLRJavaScriptCompiler::extractFunctions(const std::string& jsCode) {
    std::vector<std::string> functions;
    
    std::regex funcPattern(R"(function\s+(\w+))");
    std::sregex_iterator iter(jsCode.begin(), jsCode.end(), funcPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        functions.push_back(match[1].str());
    }
    
    return functions;
}

std::vector<std::string> ANTLRJavaScriptCompiler::extractVariables(const std::string& jsCode) {
    std::vector<std::string> variables;
    
    std::regex varPattern(R"((var|let|const)\s+(\w+))");
    std::sregex_iterator iter(jsCode.begin(), jsCode.end(), varPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        variables.push_back(match[2].str());
    }
    
    return variables;
}

std::string ANTLRJavaScriptCompiler::formatJavaScript(const std::string& jsCode) {
    auto ast = parseJavaScript(jsCode);
    if (!ast) return jsCode;
    
    return generateJavaScript(ast);
}

std::string ANTLRJavaScriptCompiler::transpileToES5(const std::string& es6Code) {
    // 简化的ES6到ES5转换
    std::string es5Code = es6Code;
    
    // 转换箭头函数
    es5Code = std::regex_replace(es5Code, 
        std::regex(R"((\w+)\s*=>\s*\{([^}]*)\})"), 
        "function($1) { $2 }");
    
    // 转换const/let为var
    es5Code = std::regex_replace(es5Code, std::regex(R"(\b(const|let)\b)"), "var");
    
    // 转换模板字符串（简化版）
    es5Code = std::regex_replace(es5Code, 
        std::regex(R"(`([^`]*)`)"),"'$1'");
    
    return es5Code;
}

void ANTLRJavaScriptCompiler::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "ANTLR JavaScript Compiler: " + message);
}

// ========== ANTLRErrorListener 实现 ==========

void ANTLRErrorListener::syntaxError(const std::string& recognizerName,
                                   size_t line, size_t charPositionInLine,
                                   const std::string& message) {
    std::ostringstream error;
    error << "Line " << line << ":" << charPositionInLine 
          << " - " << message << " (in " << recognizerName << ")";
    m_errors.push_back(error.str());
}

// ========== ANTLRIntegrationManager 实现 ==========

ANTLRIntegrationManager::ANTLRIntegrationManager()
    : m_initialized(false), m_debugMode(false) {
}

bool ANTLRIntegrationManager::initialize() {
    if (m_initialized) return true;
    
    try {
        m_cssCompiler = std::make_unique<ANTLRCSSCompiler>();
        m_jsCompiler = std::make_unique<ANTLRJavaScriptCompiler>();
        m_errorListener = std::make_unique<ANTLRErrorListener>();
        
        m_initialized = true;
        updateStats("initialization_success");
        
        return true;
    } catch (const std::exception& e) {
        reportError("Failed to initialize ANTLR4 system: " + std::string(e.what()));
        updateStats("initialization_failure");
        return false;
    }
}

std::string ANTLRIntegrationManager::compileCSS(const std::string& cssCode) {
    if (!m_initialized) {
        reportError("ANTLR4 system not initialized");
        return cssCode;
    }
    
    updateStats("css_compilations");
    
    std::vector<std::string> errors;
    if (!m_cssCompiler->validateCSS(cssCode, errors)) {
        for (const auto& error : errors) {
            reportError("CSS validation error: " + error);
        }
        return cssCode;
    }
    
    return m_cssCompiler->optimizeCSS(cssCode);
}

std::string ANTLRIntegrationManager::compileJavaScript(const std::string& jsCode) {
    if (!m_initialized) {
        reportError("ANTLR4 system not initialized");
        return jsCode;
    }
    
    updateStats("js_compilations");
    
    std::vector<std::string> errors;
    if (!m_jsCompiler->validateJavaScript(jsCode, errors)) {
        for (const auto& error : errors) {
            reportError("JavaScript validation error: " + error);
        }
        return jsCode;
    }
    
    return m_jsCompiler->optimizeJavaScript(jsCode);
}

bool ANTLRIntegrationManager::validateSyntax(const std::string& code, const std::string& type,
                                            std::vector<std::string>& errors) {
    if (!m_initialized) {
        errors.push_back("ANTLR4 system not initialized");
        return false;
    }
    
    if (type == "css") {
        return m_cssCompiler->validateCSS(code, errors);
    } else if (type == "javascript" || type == "js") {
        return m_jsCompiler->validateJavaScript(code, errors);
    } else {
        errors.push_back("Unsupported validation type: " + type);
        return false;
    }
}

std::string ANTLRIntegrationManager::getCompilationStats() const {
    std::ostringstream ss;
    
    ss << "ANTLR4 Integration Statistics:\n";
    ss << "==============================\n";
    ss << "Initialized: " << (m_initialized ? "Yes" : "No") << "\n";
    ss << "Debug Mode: " << (m_debugMode ? "Yes" : "No") << "\n\n";
    
    for (const auto& [category, count] : m_stats) {
        ss << category << ": " << count << "\n";
    }
    
    return ss.str();
}

void ANTLRIntegrationManager::cleanup() {
    m_stats.clear();
    if (m_errorListener) {
        m_errorListener->clearErrors();
    }
}

void ANTLRIntegrationManager::setDebugMode(bool enabled) {
    m_debugMode = enabled;
}

void ANTLRIntegrationManager::updateStats(const std::string& category) {
    m_stats[category]++;
}

void ANTLRIntegrationManager::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "ANTLR Integration: " + message);
}

} // namespace CHTL