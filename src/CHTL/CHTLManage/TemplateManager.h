#pragma once
#include "../CHTLNode/AdvancedTemplateNode.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace CHTL {

/**
 * 模板注册信息
 */
struct TemplateRegistration {
    std::string name;                   // 模板名称
    std::string type;                   // 模板类型(@Style, @Element, @Var)
    std::string category;               // 类别(Template, Custom)
    NodePtr templateNode;               // 模板节点
    std::string fullyQualifiedName;     // 全缀名
    std::vector<std::string> dependencies; // 依赖的模板
    
    TemplateRegistration() = default;
    TemplateRegistration(const std::string& n, const std::string& t, const std::string& c, NodePtr node)
        : name(n), type(t), category(c), templateNode(node) {
        fullyQualifiedName = "[" + category + "] " + type + " " + name;
    }
};

/**
 * 模板实例化上下文
 */
struct TemplateContext {
    std::unordered_map<std::string, std::string> parameters; // 实例化参数
    std::vector<SpecializationInfo> specializations;         // 特例化操作
    std::unordered_map<std::string, std::string> variables;  // 变量覆盖
    bool enableInheritance;                                  // 是否启用继承
    int maxInheritanceDepth;                                 // 最大继承深度
    
    TemplateContext() : enableInheritance(true), maxInheritanceDepth(10) {}
};

/**
 * CHTL模板管理器
 * 负责模板的注册、查找、继承解析和实例化
 */
class TemplateManager {
public:
    /**
     * 构造函数
     */
    TemplateManager();
    
    /**
     * 析构函数
     */
    ~TemplateManager() = default;
    
    // ========== 模板注册 ==========
    
    /**
     * 注册样式组模板
     */
    bool registerStyleTemplate(const std::string& name, std::shared_ptr<AdvancedStyleTemplateNode> templateNode,
                              bool isCustom = false);
    
    /**
     * 注册元素模板
     */
    bool registerElementTemplate(const std::string& name, std::shared_ptr<AdvancedElementTemplateNode> templateNode,
                                bool isCustom = false);
    
    /**
     * 注册变量组模板
     */
    bool registerVarTemplate(const std::string& name, std::shared_ptr<AdvancedVarTemplateNode> templateNode,
                           bool isCustom = false);
    
    /**
     * 注册模板（通用方法）
     */
    bool registerTemplate(const TemplateRegistration& registration);
    
    /**
     * 取消注册模板
     */
    bool unregisterTemplate(const std::string& name, const std::string& type, const std::string& category = "");
    
    // ========== 模板查找 ==========
    
    /**
     * 查找模板
     */
    TemplateRegistration* findTemplate(const std::string& name, const std::string& type);
    
    /**
     * 查找模板（支持全缀名）
     */
    TemplateRegistration* findTemplate(const std::string& fullyQualifiedName);
    
    /**
     * 检查模板是否存在
     */
    bool hasTemplate(const std::string& name, const std::string& type);
    
    /**
     * 获取所有模板
     */
    std::vector<TemplateRegistration> getAllTemplates() const;
    
    /**
     * 获取指定类型的所有模板
     */
    std::vector<TemplateRegistration> getTemplatesByType(const std::string& type) const;
    
    // ========== 继承解析 ==========
    
    /**
     * 解析模板继承链
     */
    std::vector<TemplateRegistration*> resolveInheritanceChain(const std::string& templateName, 
                                                              const std::string& templateType);
    
    /**
     * 检查循环继承
     */
    bool hasCircularInheritance(const std::string& templateName, const std::string& templateType);
    
    /**
     * 合并继承的模板
     */
    NodePtr mergeInheritedTemplates(const std::vector<TemplateRegistration*>& inheritanceChain,
                                   const TemplateContext& context);
    
    // ========== 模板实例化 ==========
    
    /**
     * 实例化样式组模板
     */
    NodePtr instantiateStyleTemplate(const std::string& templateName, const TemplateContext& context);
    
    /**
     * 实例化元素模板
     */
    NodePtr instantiateElementTemplate(const std::string& templateName, const TemplateContext& context);
    
    /**
     * 实例化变量组模板
     */
    NodePtr instantiateVarTemplate(const std::string& templateName, const TemplateContext& context);
    
    /**
     * 实例化模板（通用方法）
     */
    NodePtr instantiateTemplate(const std::string& templateName, const std::string& templateType,
                               const TemplateContext& context);
    
    /**
     * 实例化模板使用节点
     */
    NodePtr instantiateTemplateUsage(const TemplateUsageNode* usageNode);
    
    // ========== 变量处理 ==========
    
    /**
     * 解析变量引用
     */
    std::string resolveVariableReference(const std::string& varGroupName, const std::string& varName,
                                        const std::string& overrideValue = "");
    
    /**
     * 替换变量引用
     */
    std::string replaceVariableReferences(const std::string& content, const TemplateContext& context);
    
    // ========== 特例化处理 ==========
    
    /**
     * 应用特例化操作
     */
    void applySpecializations(NodePtr node, const std::vector<SpecializationInfo>& specializations);
    
    /**
     * 应用删除操作
     */
    void applyDeleteOperation(NodePtr node, const SpecializationInfo& operation);
    
    /**
     * 应用插入操作
     */
    void applyInsertOperation(NodePtr node, const SpecializationInfo& operation);
    
    /**
     * 应用索引访问
     */
    NodePtr applyIndexAccess(NodePtr node, const SpecializationInfo& operation);
    
    // ========== 验证和诊断 ==========
    
    /**
     * 验证模板定义
     */
    bool validateTemplate(const TemplateRegistration& registration);
    
    /**
     * 验证模板使用
     */
    bool validateTemplateUsage(const TemplateUsageNode* usageNode);
    
    /**
     * 获取模板依赖图
     */
    std::unordered_map<std::string, std::vector<std::string>> getDependencyGraph() const;
    
    /**
     * 检查模板系统一致性
     */
    std::vector<std::string> checkConsistency();
    
    // ========== 配置和状态 ==========
    
    /**
     * 设置错误报告器
     */
    void setErrorReporter(ErrorReporter* errorReporter);
    
    /**
     * 获取统计信息
     */
    std::unordered_map<std::string, size_t> getStatistics() const;
    
    /**
     * 清除所有模板
     */
    void clear();
    
    /**
     * 重置统计信息
     */
    void resetStatistics();

private:
    ErrorReporter* m_errorReporter;         // 错误报告器
    
    // 模板存储 - 按类型和名称索引
    std::unordered_map<std::string, std::unordered_map<std::string, TemplateRegistration>> m_templates;
    // 格式: m_templates[type][name] = registration
    
    // 全缀名索引
    std::unordered_map<std::string, TemplateRegistration*> m_fullyQualifiedIndex;
    
    // 依赖缓存
    std::unordered_map<std::string, std::vector<TemplateRegistration*>> m_inheritanceCache;
    
    // 统计信息
    std::unordered_map<std::string, size_t> m_statistics;
    
    // ========== 内部辅助方法 ==========
    
    /**
     * 生成模板键
     */
    std::string generateTemplateKey(const std::string& name, const std::string& type, 
                                   const std::string& category = "") const;
    
    /**
     * 更新索引
     */
    void updateIndices(const TemplateRegistration& registration);
    
    /**
     * 移除索引
     */
    void removeFromIndices(const TemplateRegistration& registration);
    
    /**
     * 递归解析继承
     */
    void resolveInheritanceRecursive(const std::string& templateName, const std::string& templateType,
                                   std::vector<TemplateRegistration*>& chain,
                                   std::unordered_set<std::string>& visited, int depth);
    
    /**
     * 合并样式模板
     */
    NodePtr mergeStyleTemplates(const std::vector<TemplateRegistration*>& chain, const TemplateContext& context);
    
    /**
     * 合并元素模板
     */
    NodePtr mergeElementTemplates(const std::vector<TemplateRegistration*>& chain, const TemplateContext& context);
    
    /**
     * 合并变量模板
     */
    NodePtr mergeVarTemplates(const std::vector<TemplateRegistration*>& chain, const TemplateContext& context);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
    
    /**
     * 报告警告
     */
    void reportWarning(const std::string& message);
    
    /**
     * 更新统计信息
     */
    void updateStatistics(const std::string& key, size_t increment = 1);
};

/**
 * 全局模板管理器实例
 */
TemplateManager& getGlobalTemplateManager();

} // namespace CHTL