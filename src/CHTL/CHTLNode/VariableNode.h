#pragma once

#include "BaseNode.h"
#include "../../Util/Common.h"
#include <unordered_map>
#include <vector>
#include <regex>

namespace CHTL {

// 变量类型枚举
enum class VariableType {
    SIMPLE,         // 简单变量: color: "red"
    FUNCTION,       // 函数式变量: calc(100px + 20px)
    REFERENCE,      // 引用变量: @other.variable
    CONDITIONAL,    // 条件变量: @if(condition, value1, value2)
    COMPUTED,       // 计算变量: @computed(expression)
    DYNAMIC         // 动态变量: 运行时计算
};

// 变量作用域
enum class VariableScope {
    LOCAL,          // 局部作用域
    TEMPLATE,       // 模板作用域
    GLOBAL,         // 全局作用域
    INHERITED       // 继承作用域
};

// 变量组模板节点增强版
class VariableTemplateNode : public BaseNode {
public:
    String templateName;                           // 模板名称
    std::unordered_map<String, String> variables; // 变量映射
    std::unordered_map<String, VariableType> variableTypes; // 变量类型
    std::unordered_map<String, String> defaultValues;       // 默认值
    std::unordered_map<String, String> descriptions;        // 变量描述
    std::unordered_map<String, StringVector> dependencies;  // 变量依赖
    VariableScope scope;                           // 作用域
    bool isCompileTime;                           // 是否编译时处理
    
    explicit VariableTemplateNode(const String& name, const Position& pos = Position());
    ~VariableTemplateNode() override = default;
    
    // 变量管理
    void addVariable(const String& name, const String& value, VariableType type = VariableType::SIMPLE);
    void setVariableType(const String& name, VariableType type);
    void setDefaultValue(const String& name, const String& defaultVal);
    void setDescription(const String& name, const String& desc);
    void addDependency(const String& varName, const String& dependency);
    
    // 变量查询
    bool hasVariable(const String& name) const;
    String getVariable(const String& name) const;
    VariableType getVariableType(const String& name) const;
    String getDefaultValue(const String& name) const;
    StringVector getDependencies(const String& name) const;
    StringVector getAllVariableNames() const;
    
    // 作用域管理
    void setScope(VariableScope newScope) { scope = newScope; }
    VariableScope getScope() const { return scope; }
    void setCompileTimeProcessing(bool compileTime) { isCompileTime = compileTime; }
    
    // 变量解析和替换
    String resolveVariable(const String& name, const std::unordered_map<String, String>& context = {}) const;
    String substituteVariables(const String& expression, const std::unordered_map<String, String>& context = {}) const;
    std::unordered_map<String, String> getAllResolvedVariables(const std::unordered_map<String, String>& context = {}) const;
    
    // 验证和优化
    bool validateVariables() const;
    StringVector getValidationErrors() const;
    void optimizeVariableReferences();
    bool hasCircularDependencies() const;
    
    // BaseNode接口
    NodeType getType() const override { return NodeType::TEMPLATE; }
    String getValue() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
    // 静态工厂方法
    static std::unique_ptr<VariableTemplateNode> createVariableTemplate(const String& name, const Position& pos = Position());
    
    // 解析方法
    static VariableType parseVariableType(const String& value);
    static std::pair<String, StringVector> parseVariableFunction(const String& functionCall);
    static bool isVariableReference(const String& value);
    static bool isComputedVariable(const String& value);
    
private:
    // 内部辅助方法
    String resolveSimpleVariable(const String& name, const std::unordered_map<String, String>& context) const;
    String resolveFunctionVariable(const String& name, const std::unordered_map<String, String>& context) const;
    String resolveReferenceVariable(const String& name, const std::unordered_map<String, String>& context) const;
    String resolveConditionalVariable(const String& name, const std::unordered_map<String, String>& context) const;
    String resolveComputedVariable(const String& name, const std::unordered_map<String, String>& context) const;
    
    // 依赖分析
    void buildDependencyGraph();
    bool hasCyclicDependency(const String& varName, StringUnorderedSet& visited, StringUnorderedSet& recursionStack) const;
    
    // 优化方法
    void inlineSimpleReferences();
    void eliminateUnusedVariables();
    
    // 验证方法
    bool isValidVariableName(const String& name) const;
    bool isValidVariableValue(const String& value) const;
};

// 变量特例化调用节点增强版
class VariableSpecializationCallNode : public BaseNode {
public:
    String templateName;                          // 模板名称
    String variableName;                          // 变量名称
    std::unordered_map<String, String> specializations; // 特例化参数
    std::unordered_map<String, String> contextOverrides; // 上下文覆盖
    bool isGlobalCall;                           // 是否全局调用
    bool isChainedCall;                          // 是否链式调用
    
    explicit VariableSpecializationCallNode(const String& templateName, const String& varName, const Position& pos = Position());
    ~VariableSpecializationCallNode() override = default;
    
    // 特例化管理
    void addSpecialization(const String& paramName, const String& value);
    void setSpecializations(const std::unordered_map<String, String>& specs);
    void addContextOverride(const String& contextVar, const String& value);
    
    // 特例化查询
    bool hasSpecialization(const String& paramName) const;
    String getSpecialization(const String& paramName) const;
    std::unordered_map<String, String> getAllSpecializations() const { return specializations; }
    size_t getSpecializationCount() const { return specializations.size(); }
    
    // 调用配置
    void setGlobalCall(bool global) { isGlobalCall = global; }
    void setChainedCall(bool chained) { isChainedCall = chained; }
    bool isGlobal() const { return isGlobalCall; }
    bool isChained() const { return isChainedCall; }
    
    // 调用解析
    String resolveCall(const VariableTemplateNode* templateNode) const;
    String generateCallExpression() const;
    String generateCSSVariableCall() const;
    String generateSassVariableCall() const;
    
    // 验证
    bool validateCall(const VariableTemplateNode* templateNode) const;
    StringVector getCallValidationErrors(const VariableTemplateNode* templateNode) const;
    
    // BaseNode接口
    NodeType getType() const override { return NodeType::TEMPLATE; }
    String getValue() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
    // 静态工厂方法
    static std::unique_ptr<VariableSpecializationCallNode> createCall(
        const String& templateName, const String& varName, const Position& pos = Position());
    
    // 解析方法
    static std::tuple<String, String, std::unordered_map<String, String>> parseSpecializationCall(const String& callExpression);
    static bool isSpecializationCall(const String& expression);
    static std::unordered_map<String, String> parseParameterList(const String& paramString);
    
private:
    // 内部解析方法
    String resolveSimpleCall(const VariableTemplateNode* templateNode) const;
    String resolveConditionalCall(const VariableTemplateNode* templateNode) const;
    String resolveComputedCall(const VariableTemplateNode* templateNode) const;
    
    // 参数处理
    String processParameterValue(const String& value, const std::unordered_map<String, String>& context) const;
    bool isParameterExpression(const String& value) const;
    String evaluateParameterExpression(const String& expression, const std::unordered_map<String, String>& context) const;
    
    // 验证方法
    bool isValidParameterName(const String& name) const;
    bool isValidParameterValue(const String& value) const;
};

// 变量上下文管理器
class VariableContextManager {
public:
    VariableContextManager() = default;
    ~VariableContextManager() = default;
    
    // 上下文管理
    void pushContext(const String& contextName = "");
    void popContext();
    void setContextVariable(const String& name, const String& value);
    String getContextVariable(const String& name) const;
    bool hasContextVariable(const String& name) const;
    void clearContext();
    
    // 模板注册
    void registerVariableTemplate(const String& name, std::shared_ptr<VariableTemplateNode> templateNode);
    void unregisterVariableTemplate(const String& name);
    std::shared_ptr<VariableTemplateNode> getVariableTemplate(const String& name) const;
    bool hasVariableTemplate(const String& name) const;
    
    // 变量解析
    String resolveVariableCall(const String& templateName, const String& varName, 
                             const std::unordered_map<String, String>& specializations = {}) const;
    String resolveVariableExpression(const String& expression) const;
    
    // 作用域管理
    void enterScope(const String& scopeName);
    void exitScope();
    String getCurrentScope() const;
    void setScopeVariable(const String& name, const String& value);
    
    // 优化和缓存
    void enableCaching(bool enable) { cachingEnabled = enable; }
    void clearCache();
    void optimizeVariableResolution();
    
    // 调试和诊断
    StringVector getAllContexts() const;
    StringVector getAllTemplates() const;
    std::unordered_map<String, String> getCurrentContextVariables() const;
    void dumpContextState() const;
    
private:
    struct Context {
        String name;
        std::unordered_map<String, String> variables;
        String parentScope;
    };
    
    std::vector<Context> contextStack;
    std::unordered_map<String, std::shared_ptr<VariableTemplateNode>> variableTemplates;
    std::unordered_map<String, String> globalVariables;
    std::unordered_map<String, String> resolvedCache;
    bool cachingEnabled = true;
    
    // 内部方法
    String generateCacheKey(const String& templateName, const String& varName, 
                          const std::unordered_map<String, String>& specializations) const;
    Context* getCurrentContext();
    const Context* getCurrentContext() const;
};

} // namespace CHTL