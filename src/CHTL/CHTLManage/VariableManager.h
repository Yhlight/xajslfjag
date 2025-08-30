#pragma once
#include "../CHTLNode/AdvancedTemplateNode.h"
#include "TemplateManager.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <regex>

namespace CHTL {

/**
 * 变量替换信息
 */
struct VariableReplacement {
    std::string varGroupName;       // 变量组名称
    std::string varName;            // 变量名称
    std::string defaultValue;       // 默认值
    std::string overrideValue;      // 覆盖值
    std::string resolvedValue;      // 解析后的值
    
    VariableReplacement() = default;
    VariableReplacement(const std::string& groupName, const std::string& name)
        : varGroupName(groupName), varName(name) {}
};

/**
 * 变量上下文
 */
class VariableContext {
public:
    /**
     * 构造函数
     */
    VariableContext();
    
    /**
     * 析构函数
     */
    ~VariableContext() = default;
    
    /**
     * 设置变量值
     */
    void setVariable(const std::string& groupName, const std::string& varName, const std::string& value);
    
    /**
     * 获取变量值
     */
    std::string getVariable(const std::string& groupName, const std::string& varName) const;
    
    /**
     * 检查变量是否存在
     */
    bool hasVariable(const std::string& groupName, const std::string& varName) const;
    
    /**
     * 覆盖变量值（特化操作）
     */
    void overrideVariable(const std::string& groupName, const std::string& varName, const std::string& value);
    
    /**
     * 获取变量组的所有变量
     */
    std::unordered_map<std::string, std::string> getVariableGroup(const std::string& groupName) const;
    
    /**
     * 合并变量上下文
     */
    void merge(const VariableContext& other);
    
    /**
     * 清除所有变量
     */
    void clear();
    
    /**
     * 创建子上下文（继承当前上下文）
     */
    std::unique_ptr<VariableContext> createChild() const;

private:
    // 格式: m_variables[groupName][varName] = value
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_variables;
    // 覆盖值优先级更高
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_overrides;
};

/**
 * 变量管理器
 * 负责处理CHTL的变量系统，包括@Var变量组、变量替换、继承合并等
 */
class VariableManager {
public:
    /**
     * 构造函数
     */
    VariableManager();
    
    /**
     * 析构函数
     */
    ~VariableManager() = default;
    
    /**
     * 设置模板管理器
     */
    void setTemplateManager(TemplateManager* templateManager);
    
    /**
     * 设置错误报告器
     */
    void setErrorReporter(ErrorReporter* errorReporter);
    
    // ========== 变量组管理 ==========
    
    /**
     * 注册变量组模板
     */
    void registerVariableGroup(std::shared_ptr<AdvancedVarTemplateNode> varTemplate);
    
    /**
     * 获取变量组模板
     */
    std::shared_ptr<AdvancedVarTemplateNode> getVariableGroup(const std::string& groupName);
    
    /**
     * 检查变量组是否存在
     */
    bool hasVariableGroup(const std::string& groupName) const;
    
    // ========== 变量解析 ==========
    
    /**
     * 解析变量引用 VarGroup(varName)
     */
    std::string resolveVariableReference(const std::string& reference);
    
    /**
     * 解析变量引用（指定上下文）
     */
    std::string resolveVariableReference(const std::string& reference, const VariableContext& context);
    
    /**
     * 解析内容中的所有变量引用
     */
    std::string resolveVariableReferences(const std::string& content);
    
    /**
     * 解析内容中的所有变量引用（指定上下文）
     */
    std::string resolveVariableReferences(const std::string& content, const VariableContext& context);
    
    // ========== 变量继承与合并 ==========
    
    /**
     * 解析变量组的继承链
     */
    std::vector<std::shared_ptr<AdvancedVarTemplateNode>> resolveInheritanceChain(const std::string& groupName);
    
    /**
     * 合并继承链中的变量
     */
    VariableContext mergeInheritanceChain(const std::vector<std::shared_ptr<AdvancedVarTemplateNode>>& inheritanceChain);
    
    /**
     * 应用变量特化操作
     */
    void applyVariableSpecializations(VariableContext& context, const std::vector<SpecializationInfo>& specializations);
    
    // ========== 变量查找与解析 ==========
    
    /**
     * 查找内容中的变量引用
     */
    std::vector<VariableReplacement> findVariableReferences(const std::string& content);
    
    /**
     * 验证变量引用的有效性
     */
    bool validateVariableReference(const VariableReplacement& replacement);
    
    /**
     * 生成变量使用报告
     */
    std::string generateVariableReport() const;
    
    // ========== 实用方法 ==========
    
    /**
     * 解析变量引用字符串（如 "VarGroup(varName)"）
     */
    VariableReplacement parseVariableReference(const std::string& reference);
    
    /**
     * 创建变量上下文（从变量组）
     */
    VariableContext createContextFromVariableGroup(const std::string& groupName);
    
    /**
     * 创建变量上下文（从参数映射）
     */
    VariableContext createContextFromParameters(const std::unordered_map<std::string, std::string>& parameters);
    
    /**
     * 清除所有变量组
     */
    void clear();

private:
    TemplateManager* m_templateManager;                                         // 模板管理器
    ErrorReporter* m_errorReporter;                                            // 错误报告器
    
    // 变量组存储
    std::unordered_map<std::string, std::shared_ptr<AdvancedVarTemplateNode>> m_variableGroups;
    
    // 变量引用模式
    std::regex m_variablePattern;
    
    // 统计信息
    std::unordered_map<std::string, size_t> m_statistics;
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
    
    /**
     * 更新统计信息
     */
    void updateStatistics(const std::string& key);
    
    /**
     * 检查循环继承
     */
    bool hasCircularInheritance(const std::string& groupName, std::unordered_set<std::string>& visited);
};

} // namespace CHTL