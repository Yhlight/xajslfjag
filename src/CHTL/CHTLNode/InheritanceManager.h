#pragma once

#include "../../Util/Common.h"
#include "BaseNode.h"
#include "TemplateNode.h"
#include "CustomNode.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <set>

namespace CHTL {

// 继承关系类型
enum class InheritanceType {
    TEMPLATE_TO_TEMPLATE,    // 模板继承模板
    CUSTOM_TO_TEMPLATE,      // 自定义继承模板
    TEMPLATE_TO_CUSTOM,      // 模板继承自定义
    CUSTOM_TO_CUSTOM         // 自定义继承自定义
};

// 继承关系描述
struct InheritanceRelation {
    String childName;        // 子对象名称
    String childType;        // 子对象类型 (Style, Element, Var)
    String parentName;       // 父对象名称
    String parentType;       // 父对象类型
    InheritanceType type;    // 继承类型
    bool isExplicit;         // 是否为显性继承 (inherit关键字)
    String inheritanceExpression; // 继承表达式
    
    InheritanceRelation() : type(InheritanceType::TEMPLATE_TO_TEMPLATE), isExplicit(false) {}
    
    String toString() const {
        String result = childName + " (" + childType + ") ";
        result += isExplicit ? "explicitly inherits from " : "inherits from ";
        result += parentName + " (" + parentType + ")";
        return result;
    }
};

// 继承冲突类型
enum class InheritanceConflictType {
    PROPERTY_OVERRIDE,       // 属性覆盖冲突
    CIRCULAR_DEPENDENCY,     // 循环依赖
    TYPE_MISMATCH,          // 类型不匹配
    MULTIPLE_INHERITANCE,    // 多重继承冲突
    UNDEFINED_PARENT        // 未定义的父对象
};

// 继承冲突描述
struct InheritanceConflict {
    InheritanceConflictType type;
    String description;
    StringVector involvedObjects;
    String resolution;
    
    InheritanceConflict(InheritanceConflictType t, const String& desc) 
        : type(t), description(desc) {}
};

// 解析的属性项
struct ResolvedProperty {
    String name;
    String value;
    String sourceObject;     // 来源对象
    int priority;           // 优先级 (数字越大优先级越高)
    bool isOverridden;      // 是否被覆盖
    
    ResolvedProperty() : priority(0), isOverridden(false) {}
    ResolvedProperty(const String& n, const String& v, const String& src, int p = 0)
        : name(n), value(v), sourceObject(src), priority(p), isOverridden(false) {}
};

// 继承解析结果
struct InheritanceResolution {
    String objectName;
    String objectType;
    StringUnorderedMap finalProperties;           // 最终属性集合
    std::vector<ResolvedProperty> propertyHistory; // 属性解析历史
    StringVector inheritanceChain;                // 完整继承链
    std::vector<InheritanceConflict> conflicts;   // 发现的冲突
    bool isValid;
    
    InheritanceResolution() : isValid(false) {}
    
    String toString() const {
        String result = "Inheritance Resolution for " + objectName + " (" + objectType + "):\n";
        
        if (!inheritanceChain.empty()) {
            result += "  Inheritance Chain: ";
            for (size_t i = 0; i < inheritanceChain.size(); ++i) {
                if (i > 0) result += " -> ";
                result += inheritanceChain[i];
            }
            result += "\n";
        }
        
        result += "  Final Properties:\n";
        for (const auto& prop : finalProperties) {
            result += "    " + prop.first + ": " + prop.second + "\n";
        }
        
        if (!conflicts.empty()) {
            result += "  Conflicts:\n";
            for (const auto& conflict : conflicts) {
                result += "    " + conflict.description + "\n";
            }
        }
        
        return result;
    }
};

// 模板继承管理器
class InheritanceManager {
public:
    InheritanceManager();
    ~InheritanceManager() = default;
    
    // 注册模板和自定义对象
    void registerTemplate(std::shared_ptr<TemplateNode> templateNode);
    void registerCustom(std::shared_ptr<CustomNode> customNode);
    
    // 清除注册的对象
    void clearTemplates();
    void clearCustoms();
    void clearAll();
    
    // 继承关系管理
    bool addInheritanceRelation(const InheritanceRelation& relation);
    bool removeInheritanceRelation(const String& childName, const String& childType, 
                                   const String& parentName, const String& parentType);
    StringVector getInheritanceRelations(const String& objectName, const String& objectType) const;
    
    // 解析继承表达式
    InheritanceRelation parseInheritanceExpression(const String& childName, const String& childType,
                                                    const String& expression, bool isExplicit = false);
    
    // 继承链解析
    InheritanceResolution resolveInheritance(const String& objectName, const String& objectType);
    StringVector buildInheritanceChain(const String& objectName, const String& objectType);
    
    // 冲突检测
    bool hasCircularDependency(const String& objectName, const String& objectType);
    StringVector getCircularDependencyChain(const String& objectName, const String& objectType);
    std::vector<InheritanceConflict> detectConflicts(const String& objectName, const String& objectType);
    
    // 属性解析
    StringUnorderedMap resolveProperties(const String& objectName, const String& objectType);
    std::vector<ResolvedProperty> resolvePropertyHistory(const String& objectName, const String& objectType);
    
    // 查询功能
    bool isTemplateRegistered(const String& name, const String& type) const;
    bool isCustomRegistered(const String& name, const String& type) const;
    bool hasInheritanceRelation(const String& childName, const String& childType) const;
    
    // 获取注册的对象
    std::shared_ptr<TemplateNode> getTemplate(const String& name, const String& type) const;
    std::shared_ptr<CustomNode> getCustom(const String& name, const String& type) const;
    
    // 统计信息
    size_t getTemplateCount() const { return templates.size(); }
    size_t getCustomCount() const { return customs.size(); }
    size_t getInheritanceRelationCount() const { return inheritanceRelations.size(); }
    
    // 验证整个继承体系
    bool validateInheritanceSystem();
    std::vector<InheritanceConflict> getAllConflicts();
    
    // 调试和输出
    String toString() const;
    String getInheritanceTree(const String& objectName, const String& objectType) const;
    void printInheritanceGraph() const;
    
private:
    // 存储注册的模板和自定义对象
    std::unordered_map<String, std::shared_ptr<TemplateNode>> templates;
    std::unordered_map<String, std::shared_ptr<CustomNode>> customs;
    
    // 继承关系存储
    std::vector<InheritanceRelation> inheritanceRelations;
    
    // 内部辅助方法
    String makeObjectKey(const String& name, const String& type) const;
    bool isValidInheritanceType(const String& childType, const String& parentType) const;
    
    // 循环依赖检测
    bool detectCircularDependencyRecursive(const String& objectName, const String& objectType,
                                           std::set<String>& visiting, std::set<String>& visited,
                                           StringVector& path) const;
    
    // 属性合并
    StringUnorderedMap mergeProperties(const StringUnorderedMap& base, const StringUnorderedMap& override) const;
    void applyPropertyOverrides(StringUnorderedMap& properties, const String& objectName, const String& objectType) const;
    
    // 获取直接继承关系
    StringVector getDirectParents(const String& objectName, const String& objectType) const;
    StringVector getDirectChildren(const String& objectName, const String& objectType) const;
    
    // 解析优先级计算
    int calculatePropertyPriority(const String& sourceObject, int depth) const;
    
    // 错误处理
    void reportConflict(const InheritanceConflict& conflict);
    std::vector<InheritanceConflict> currentConflicts;
};

// 继承表达式解析器
class InheritanceExpressionParser {
public:
    // 解析各种继承表达式
    static InheritanceRelation parseExplicitInheritance(const String& childName, const String& childType,
                                                        const String& expression);
    static InheritanceRelation parseImplicitInheritance(const String& childName, const String& childType,
                                                        const String& expression);
    
    // 验证继承表达式
    static bool isValidInheritanceExpression(const String& expression);
    static StringVector validateInheritanceExpression(const String& expression);
    
private:
    // 表达式解析辅助
    static bool parseTypeAndName(const String& expression, String& type, String& name);
    static String extractTypeFromExpression(const String& expression);
    static String extractNameFromExpression(const String& expression);
    static bool isExplicitInheritanceKeyword(const String& expression);
};

// 冲突解决器
class InheritanceConflictResolver {
public:
    // 冲突解决策略
    enum class ResolutionStrategy {
        CHILD_PRIORITY,     // 子对象优先
        PARENT_PRIORITY,    // 父对象优先
        EXPLICIT_PRIORITY,  // 显性声明优先
        ORDER_PRIORITY,     // 声明顺序优先
        TYPE_PRIORITY       // 类型优先级
    };
    
    // 解决属性冲突
    static ResolvedProperty resolvePropertyConflict(const std::vector<ResolvedProperty>& conflictingProperties,
                                                    ResolutionStrategy strategy = ResolutionStrategy::CHILD_PRIORITY);
    
    // 解决继承冲突
    static InheritanceResolution resolveInheritanceConflicts(const InheritanceResolution& resolution,
                                                            ResolutionStrategy strategy = ResolutionStrategy::CHILD_PRIORITY);
    
    // 生成冲突解决建议
    static String generateResolutionSuggestion(const InheritanceConflict& conflict);
    
private:
    static int getStrategyPriority(const ResolvedProperty& property, ResolutionStrategy strategy);
};

} // namespace CHTL