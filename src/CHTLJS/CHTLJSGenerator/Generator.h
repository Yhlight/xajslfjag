#pragma once

#include "../../Util/Common.h"
#include "../CHTLJSNode/CHTLJSNodes.h"
#include "../CHTLJSLexer/Token.h"
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {
namespace JS {

// 前置声明
class BaseNode;
class ModuleNode;

// CHTL JS代码生成器配置
struct CHTLJSGeneratorConfig {
    bool minifyOutput = false;
    bool generateSourceMap = false;
    bool enableOptimization = true;
    bool preserveComments = false;
    bool strictMode = true;
    String indentString = "  ";  // 两个空格
    String newlineString = "\n";
    size_t maxLineLength = 120;
    bool enableAMDWrapper = true;
    bool enableDebugInfo = false;
    StringVector customModules;
    std::unordered_map<String, String> moduleAliases;
};

// CHTL JS代码生成器上下文
class CHTLJSGeneratorContext {
public:
    CHTLJSGeneratorContext();
    
    // 作用域管理
    void enterScope(const String& scopeName = "");
    void exitScope();
    String getCurrentScope() const;
    size_t getScopeDepth() const;
    
    // 变量管理
    void declareVariable(const String& name, const String& type = "var");
    bool isVariableDeclared(const String& name) const;
    String getVariableScope(const String& name) const;
    void addImport(const String& module, const String& name);
    
    // 函数管理
    void declareFunction(const String& name, const StringVector& params);
    bool isFunctionDeclared(const String& name) const;
    StringVector getFunctionParams(const String& name) const;
    
    // 事件管理
    void registerEventHandler(const String& selector, const String& event, const String& handler);
    StringVector getEventHandlers(const String& selector) const;
    
    // 模块管理
    void registerModule(const String& name, const StringVector& exports);
    StringVector getModuleExports(const String& name) const;
    bool isModuleRegistered(const String& name) const;
    
    // 辅助方法
    String generateUniqueId(const String& prefix = "id");
    void addDependency(const String& dependency);
    StringVector getDependencies() const;
    
private:
    StringVector scopes;
    std::unordered_map<String, String> variables;
    std::unordered_map<String, StringVector> functions;
    std::unordered_map<String, std::unordered_map<String, String>> eventHandlers;
    std::unordered_map<String, StringVector> modules;
    StringVector dependencies;
    size_t uniqueIdCounter;
};

// CHTL JS代码生成器主类
class CHTLJSGenerator {
public:
    explicit CHTLJSGenerator(const CHTLJSGeneratorConfig& config = CHTLJSGeneratorConfig{});
    
    // 主要生成接口
    String generate(const std::shared_ptr<BaseNode>& node);
    String generateFromTokens(const std::vector<CHTLJSToken>& tokens);
    String generateFromAST(const std::shared_ptr<BaseNode>& root);
    
    // 特定节点生成
    String generateListen(const std::shared_ptr<CHTLJSListenNode>& node);
    String generateDelegate(const std::shared_ptr<CHTLJSDelegateNode>& node);
    String generateAnimate(const std::shared_ptr<CHTLJSAnimateNode>& node);
    String generateVir(const std::shared_ptr<CHTLJSVirNode>& node);
    String generateModule(const std::shared_ptr<ModuleNode>& node);
    String generateEventBinding(const std::shared_ptr<CHTLJSEventBindingNode>& node);
    String generateSelector(const std::shared_ptr<CHTLJSSelectorNode>& node);
    
    // JavaScript代码生成
    String generateFunction(const String& name, const StringVector& params, const String& body);
    String generateVariableDeclaration(const String& name, const String& value, const String& type = "const");
    String generateEventListener(const String& target, const String& event, const String& handler);
    String generateDOMQuery(const String& selector);
    
    // CHTL JS特殊功能生成
    String generateListenFunction(const String& target, const std::unordered_map<String, String>& events);
    String generateDelegateFunction(const String& parent, const String& target, 
                                   const std::unordered_map<String, String>& events);
    String generateAnimateFunction(const std::unordered_map<String, String>& properties);
    String generateVirtualObject(const String& name, const std::unordered_map<String, String>& properties);
    
    // 模块系统生成
    String generateAMDModule(const String& name, const StringVector& dependencies, const String& content);
    String generateModuleLoader(const StringVector& modules);
    String generateModuleWrapper(const String& content, const StringVector& exports);
    
    // 优化和工具
    String optimizeCode(const String& code);
    String minifyCode(const String& code);
    String addSourceMap(const String& code);
    String formatCode(const String& code);
    
    // 配置和状态
    void setConfig(const CHTLJSGeneratorConfig& config);
    const CHTLJSGeneratorConfig& getConfig() const;
    CHTLJSGeneratorContext& getContext();
    const CHTLJSGeneratorContext& getContext() const;
    
    // 错误处理
    const StringVector& getErrors() const;
    const StringVector& getWarnings() const;
    bool hasErrors() const;
    String getErrorSummary() const;
    void clearErrors();
    
private:
    CHTLJSGeneratorConfig config;
    CHTLJSGeneratorContext context;
    StringVector errors;
    StringVector warnings;
    size_t indentLevel;
    
    // 生成器注册表
    std::unordered_map<String, std::function<String(const std::shared_ptr<BaseNode>&)>> nodeGenerators;
    
    // 核心生成方法
    String generateNode(const std::shared_ptr<BaseNode>& node);
    String generateExpression(const String& expression);
    String generateStatement(const String& statement);
    String generateBlock(const String& content);
    
    // 辅助生成方法
    String generateIndent() const;
    String generateNewline() const;
    String generateComment(const String& comment);
    String generateDocComment(const String& doc);
    
    // 选择器处理
    String translateSelector(const String& selector);
    String optimizeSelector(const String& selector);
    String generateSelectorCode(const String& selector);
    
    // 事件处理
    String generateEventHandler(const String& eventName, const String& handler);
    String generateEventDelegation(const String& parent, const String& child, 
                                  const String& event, const String& handler);
    
    // 动画处理
    String generateKeyframes(const std::unordered_map<String, String>& keyframes);
    String generateAnimationProperties(const std::unordered_map<String, String>& properties);
    String generateRequestAnimationFrame(const String& callback);
    
    // 虚对象处理
    String generateVirObjectDeclaration(const String& name);
    String generateVirPropertyAccess(const String& object, const String& property);
    String generateVirMethodCall(const String& object, const String& method, const StringVector& args);
    
    // 模块处理
    String generateModuleDeclaration(const String& name, const StringVector& dependencies);
    String generateModuleExports(const StringVector& exports);
    String generateModuleImports(const StringVector& imports);
    String generateDependencyGraph(const StringVector& modules);
    
    // 代码优化
    String removeUnnecessaryWhitespace(const String& code);
    String combineAdjacentStrings(const String& code);
    String optimizeVariableDeclarations(const String& code);
    String inlineSingleUseVariables(const String& code);
    
    // 验证和检查
    bool validateGeneratedCode(const String& code);
    bool checkSyntaxValidity(const String& code);
    StringVector findUnusedVariables(const String& code);
    StringVector findMissingDependencies(const String& code);
    
    // 模板生成方法
    String generateListenTemplate();
    String generateDelegateTemplate();
    String generateAnimateTemplate();
    String generateVirTemplate();
    String generateModuleTemplate();
    
    // 错误处理
    void addError(const String& message);
    void addWarning(const String& message);
    
    // 初始化方法
    void initializeGenerators();
    void setupDefaultGenerators();
    void registerNodeGenerator(const String& nodeType, 
                              std::function<String(const std::shared_ptr<BaseNode>&)> generator);
};

// CHTL JS代码生成器工厂
class CHTLJSGeneratorFactory {
public:
    static std::unique_ptr<CHTLJSGenerator> createGenerator(const CHTLJSGeneratorConfig& config = CHTLJSGeneratorConfig{});
    static std::unique_ptr<CHTLJSGenerator> createMinifyingGenerator();
    static std::unique_ptr<CHTLJSGenerator> createDebugGenerator();
    static std::unique_ptr<CHTLJSGenerator> createProductionGenerator();
    
    // 配置预设
    static CHTLJSGeneratorConfig getDefaultConfig();
    static CHTLJSGeneratorConfig getMinifyConfig();
    static CHTLJSGeneratorConfig getDebugConfig();
    static CHTLJSGeneratorConfig getProductionConfig();
};

// CHTL JS代码模板管理器
class CHTLJSTemplateManager {
public:
    // 模板注册
    static void registerTemplate(const String& name, const String& template_);
    static String getTemplate(const String& name);
    static bool hasTemplate(const String& name);
    
    // 模板填充
    static String fillTemplate(const String& template_, const std::unordered_map<String, String>& placeholders);
    static String fillTemplate(const String& name, const std::unordered_map<String, String>& placeholders);
    
    // 预定义模板
    static void initializeDefaultTemplates();
    
private:
    static std::unordered_map<String, String> templates;
};

} // namespace JS
} // namespace CHTL