#pragma once

#include "TemplateNode.h"
#include "../../Util/Common.h"
#include <unordered_map>
#include <memory>
#include <functional>

namespace CHTL {

// 模板类型增强枚举
enum class AdvancedTemplateType {
    STYLE_BASIC,        // 基础样式模板
    STYLE_RESPONSIVE,   // 响应式样式模板
    STYLE_THEMED,       // 主题化样式模板
    STYLE_ANIMATED,     // 动画样式模板
    ELEMENT_BASIC,      // 基础元素模板
    ELEMENT_COMPONENT,  // 组件化元素模板
    ELEMENT_LAYOUT,     // 布局元素模板
    ELEMENT_INTERACTIVE, // 交互元素模板
    VAR_SIMPLE,         // 简单变量模板
    VAR_COMPUTED,       // 计算变量模板
    VAR_CONTEXTUAL,     // 上下文变量模板
    VAR_REACTIVE,       // 响应式变量模板
    MIXIN,              // 混入模板
    FUNCTION,           // 函数模板
    MACRO,              // 宏模板
    PARTIAL             // 片段模板
};

// 模板参数
struct TemplateParameter {
    String name;                // 参数名
    String type;                // 参数类型
    String defaultValue;        // 默认值
    String description;         // 参数描述
    bool isRequired;            // 是否必需
    bool isVariadic;            // 是否可变参数
    StringVector constraints;   // 参数约束
    StringVector allowedValues; // 允许的值
    
    TemplateParameter() : isRequired(false), isVariadic(false) {}
    TemplateParameter(const String& n, const String& t = "String") 
        : name(n), type(t), isRequired(false), isVariadic(false) {}
};

// 模板继承信息
struct TemplateInheritance {
    String baseTemplate;        // 基础模板名
    StringVector interfaces;    // 实现的接口
    StringUnorderedMap overrides; // 重写的部分
    bool isAbstract;           // 是否抽象模板
    bool isFinal;              // 是否最终模板
    
    TemplateInheritance() : isAbstract(false), isFinal(false) {}
};

// 模板编译选项
struct TemplateCompileOptions {
    bool enableOptimization;    // 启用优化
    bool enableInlining;        // 启用内联
    bool enableCaching;         // 启用缓存
    bool strictTypeChecking;    // 严格类型检查
    bool allowDynamicParams;    // 允许动态参数
    String targetFormat;        // 目标格式
    StringUnorderedMap defines; // 预定义宏
    
    TemplateCompileOptions() : enableOptimization(true), enableInlining(false), 
                             enableCaching(true), strictTypeChecking(true), 
                             allowDynamicParams(false), targetFormat("css") {}
};

// 高级模板节点
class AdvancedTemplateNode : public TemplateNode {
public:
    AdvancedTemplateType advancedType;          // 高级模板类型
    std::vector<TemplateParameter> parameters;  // 模板参数
    TemplateInheritance inheritance;            // 继承信息
    TemplateCompileOptions compileOptions;      // 编译选项
    StringUnorderedMap slots;                   // 模板插槽
    StringUnorderedMap mixins;                  // 混入模板
    StringVector dependencies;                  // 模板依赖
    StringVector conditions;                    // 条件约束
    std::function<String(const StringUnorderedMap&)> dynamicGenerator; // 动态生成器
    bool isCompiledTemplate;                    // 是否已编译
    String compiledContent;                     // 编译后内容
    double version;                             // 模板版本
    String author;                              // 模板作者
    String license;                             // 模板许可证
    
    explicit AdvancedTemplateNode(AdvancedTemplateType type, const String& name, const Position& pos = Position());
    ~AdvancedTemplateNode() override = default;
    
    // 参数管理
    void addParameter(const TemplateParameter& param);
    void addParameter(const String& name, const String& type = "String", const String& defaultValue = "", bool required = false);
    void removeParameter(const String& name);
    TemplateParameter* getParameter(const String& name);
    const TemplateParameter* getParameter(const String& name) const;
    bool hasParameter(const String& name) const;
    StringVector getParameterNames() const;
    StringUnorderedMap getDefaultParameters() const;
    
    // 继承管理
    void setBaseTemplate(const String& baseName);
    void addInterface(const String& interfaceName);
    void addOverride(const String& part, const String& content);
    void setAbstract(bool abstract = true);
    void setFinal(bool final = true);
    bool isAbstract() const { return inheritance.isAbstract; }
    bool isFinal() const { return inheritance.isFinal; }
    
    // 插槽管理
    void addSlot(const String& name, const String& defaultContent = "");
    void removeSlot(const String& name);
    String getSlot(const String& name) const;
    bool hasSlot(const String& name) const;
    StringVector getSlotNames() const;
    void fillSlot(const String& name, const String& content);
    
    // 混入管理
    void addMixin(const String& name, const String& templateName);
    void removeMixin(const String& name);
    String getMixin(const String& name) const;
    bool hasMixin(const String& name) const;
    StringVector getMixinNames() const;
    void applyMixins();
    
    // 依赖管理
    void addDependency(const String& templateName);
    void removeDependency(const String& templateName);
    bool hasDependency(const String& templateName) const;
    StringVector getDependencies() const { return dependencies; }
    void resolveDependencies(const std::unordered_map<String, std::shared_ptr<AdvancedTemplateNode>>& templates);
    
    // 条件约束
    void addCondition(const String& condition);
    void removeCondition(const String& condition);
    bool hasCondition(const String& condition) const;
    StringVector getConditions() const { return conditions; }
    bool evaluateConditions(const StringUnorderedMap& context) const;
    
    // 模板编译
    bool compile(const TemplateCompileOptions& options = TemplateCompileOptions{});
    bool isCompiled() const { return isCompiledTemplate; }
    String getCompiledContent() const { return compiledContent; }
    void clearCompiled();
    
    // 模板实例化
    String instantiate(const StringUnorderedMap& parameters = {}) const;
    String instantiateWithSlots(const StringUnorderedMap& parameters, const StringUnorderedMap& slots) const;
    std::unique_ptr<BaseNode> createInstance(const StringUnorderedMap& parameters = {}) const;
    
    // 模板验证
    bool validate() const;
    StringVector getValidationErrors() const;
    bool validateParameters(const StringUnorderedMap& params) const;
    StringVector getParameterValidationErrors(const StringUnorderedMap& params) const;
    
    // 模板优化
    void optimize();
    void inlineSmallTemplates();
    void cacheCommonPatterns();
    void removeUnusedParameters();
    void simplifyConditions();
    
    // 响应式功能
    void addBreakpoint(const String& name, const String& condition, const String& content);
    void removeBreakpoint(const String& name);
    String getResponsiveCSS() const;
    bool isResponsive() const;
    
    // 主题化功能
    void addTheme(const String& themeName, const StringUnorderedMap& variables);
    void removeTheme(const String& themeName);
    String getThemedContent(const String& themeName) const;
    StringVector getAvailableThemes() const;
    
    // 动画功能
    void addAnimation(const String& name, const String& keyframes);
    void removeAnimation(const String& name);
    String getAnimationCSS() const;
    bool hasAnimations() const;
    
    // 组件化功能
    void addProp(const String& name, const String& type, const String& defaultValue = "");
    void removeProp(const String& name);
    StringUnorderedMap getProps() const;
    String generatePropTypes() const;
    
    // 布局功能
    void setLayoutType(const String& type); // flex, grid, absolute, etc.
    void addLayoutProperty(const String& property, const String& value);
    String getLayoutCSS() const;
    
    // 交互功能
    void addEvent(const String& eventName, const String& handler);
    void removeEvent(const String& eventName);
    StringUnorderedMap getEvents() const;
    String generateEventHandlers() const;
    
    // 计算变量功能
    void addComputedVar(const String& name, const String& expression);
    void removeComputedVar(const String& name);
    String getComputedValue(const String& name, const StringUnorderedMap& context) const;
    
    // 上下文变量功能
    void addContextVar(const String& name, const String& source);
    void removeContextVar(const String& name);
    String getContextValue(const String& name, const StringUnorderedMap& context) const;
    
    // BaseNode接口重写
    NodeType getType() const override { return NodeType::TEMPLATE; }
    String getValue() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
    // 序列化扩展
    String toAdvancedString() const;
    String toJSON() const;
    String toXML() const;
    String toYAML() const;
    
    // 静态工厂方法
    static std::unique_ptr<AdvancedTemplateNode> createStyleTemplate(const String& name, const Position& pos = Position());
    static std::unique_ptr<AdvancedTemplateNode> createElementTemplate(const String& name, const Position& pos = Position());
    static std::unique_ptr<AdvancedTemplateNode> createVarTemplate(const String& name, const Position& pos = Position());
    static std::unique_ptr<AdvancedTemplateNode> createResponsiveTemplate(const String& name, const Position& pos = Position());
    static std::unique_ptr<AdvancedTemplateNode> createComponentTemplate(const String& name, const Position& pos = Position());
    static std::unique_ptr<AdvancedTemplateNode> createMixinTemplate(const String& name, const Position& pos = Position());
    
    // 工具方法
    static AdvancedTemplateType parseTemplateType(const String& typeString);
    static String templateTypeToString(AdvancedTemplateType type);
    static bool isCompatibleType(AdvancedTemplateType type1, AdvancedTemplateType type2);
    
private:
    std::unordered_map<String, String> breakpoints;    // 断点映射
    std::unordered_map<String, StringUnorderedMap> themes; // 主题映射
    std::unordered_map<String, String> animations;     // 动画映射
    StringUnorderedMap props;                           // 组件属性
    String layoutType;                                  // 布局类型
    StringUnorderedMap layoutProperties;                // 布局属性
    StringUnorderedMap events;                          // 事件处理
    StringUnorderedMap computedVars;                    // 计算变量
    StringUnorderedMap contextVars;                     // 上下文变量
    
    // 内部编译方法
    String compileStyleTemplate() const;
    String compileElementTemplate() const;
    String compileVarTemplate() const;
    String compileMixinTemplate() const;
    
    // 内部实例化方法
    String instantiateStyle(const StringUnorderedMap& params) const;
    String instantiateElement(const StringUnorderedMap& params) const;
    String instantiateVar(const StringUnorderedMap& params) const;
    
    // 内部验证方法
    bool validateStyleTemplate() const;
    bool validateElementTemplate() const;
    bool validateVarTemplate() const;
    bool validateParameters() const;
    bool validateInheritance() const;
    
    // 内部优化方法
    void optimizeStyleTemplate();
    void optimizeElementTemplate();
    void optimizeVarTemplate();
    
    // 工具方法
    String expandTemplate(const String& content, const StringUnorderedMap& params) const;
    String processConditions(const String& content, const StringUnorderedMap& context) const;
    String processMixins(const String& content) const;
    String processSlots(const String& content, const StringUnorderedMap& slotContents) const;
    bool evaluateCondition(const String& condition, const StringUnorderedMap& context) const;
    String expandVariables(const String& content, const StringUnorderedMap& variables) const;
};

// 模板注册管理器
class AdvancedTemplateRegistry {
public:
    static AdvancedTemplateRegistry& getInstance();
    
    // 模板注册
    void registerTemplate(const String& name, std::shared_ptr<AdvancedTemplateNode> template_);
    void unregisterTemplate(const String& name);
    std::shared_ptr<AdvancedTemplateNode> getTemplate(const String& name) const;
    bool hasTemplate(const String& name) const;
    StringVector getAllTemplateNames() const;
    
    // 类型分类
    StringVector getTemplatesByType(AdvancedTemplateType type) const;
    StringVector getStyleTemplates() const;
    StringVector getElementTemplates() const;
    StringVector getVarTemplates() const;
    
    // 继承查询
    StringVector getChildTemplates(const String& baseName) const;
    StringVector getParentTemplates(const String& templateName) const;
    StringVector getTemplateHierarchy(const String& templateName) const;
    
    // 依赖分析
    StringVector getDependentTemplates(const String& templateName) const;
    StringVector getDependencyChain(const String& templateName) const;
    bool hasCircularDependency(const String& templateName) const;
    
    // 模板搜索
    StringVector searchTemplates(const String& pattern) const;
    StringVector filterTemplates(std::function<bool(const AdvancedTemplateNode&)> predicate) const;
    
    // 批量操作
    void registerBuiltinTemplates();
    void clearAllTemplates();
    void validateAllTemplates() const;
    
    // 导入导出
    bool exportTemplates(const String& filePath) const;
    bool importTemplates(const String& filePath);
    String exportTemplatesToString() const;
    bool importTemplatesFromString(const String& data);
    
    // 统计信息
    size_t getTemplateCount() const;
    std::unordered_map<String, size_t> getTypeStatistics() const;
    
private:
    AdvancedTemplateRegistry() = default;
    ~AdvancedTemplateRegistry() = default;
    
    std::unordered_map<String, std::shared_ptr<AdvancedTemplateNode>> templates;
    
    // 内部方法
    void initializeBuiltinTemplates();
    bool hasCircularDependencyDFS(const String& templateName, StringUnorderedSet& visited, StringUnorderedSet& stack) const;
};

// 模板构建器
class AdvancedTemplateBuilder {
public:
    explicit AdvancedTemplateBuilder(const String& templateName);
    ~AdvancedTemplateBuilder() = default;
    
    // 基础配置
    AdvancedTemplateBuilder& type(AdvancedTemplateType templateType);
    AdvancedTemplateBuilder& description(const String& desc);
    AdvancedTemplateBuilder& version(double ver);
    AdvancedTemplateBuilder& author(const String& authorName);
    AdvancedTemplateBuilder& license(const String& licenseName);
    
    // 内容配置
    AdvancedTemplateBuilder& content(const String& templateContent);
    AdvancedTemplateBuilder& fromFile(const String& filePath);
    
    // 参数配置
    AdvancedTemplateBuilder& parameter(const String& name, const String& type = "String", const String& defaultValue = "", bool required = false);
    AdvancedTemplateBuilder& requiredParameter(const String& name, const String& type = "String");
    AdvancedTemplateBuilder& optionalParameter(const String& name, const String& defaultValue, const String& type = "String");
    
    // 继承配置
    AdvancedTemplateBuilder& inherits(const String& baseName);
    AdvancedTemplateBuilder& implements(const String& interfaceName);
    AdvancedTemplateBuilder& abstract(bool isAbstract = true);
    AdvancedTemplateBuilder& final(bool isFinal = true);
    
    // 插槽配置
    AdvancedTemplateBuilder& slot(const String& name, const String& defaultContent = "");
    
    // 混入配置
    AdvancedTemplateBuilder& mixin(const String& name, const String& templateName);
    
    // 依赖配置
    AdvancedTemplateBuilder& dependency(const String& templateName);
    
    // 条件配置
    AdvancedTemplateBuilder& condition(const String& conditionExpression);
    
    // 特化配置
    AdvancedTemplateBuilder& responsive();
    AdvancedTemplateBuilder& breakpoint(const String& name, const String& condition, const String& content);
    AdvancedTemplateBuilder& theme(const String& themeName, const StringUnorderedMap& variables);
    AdvancedTemplateBuilder& animation(const String& name, const String& keyframes);
    AdvancedTemplateBuilder& prop(const String& name, const String& type, const String& defaultValue = "");
    AdvancedTemplateBuilder& event(const String& eventName, const String& handler);
    
    // 编译配置
    AdvancedTemplateBuilder& enableOptimization(bool enable = true);
    AdvancedTemplateBuilder& enableCaching(bool enable = true);
    AdvancedTemplateBuilder& strictTypes(bool strict = true);
    AdvancedTemplateBuilder& targetFormat(const String& format);
    
    // 构建方法
    std::shared_ptr<AdvancedTemplateNode> build();
    String buildToString();
    bool buildToFile(const String& filePath);
    bool registerToGlobal();
    
private:
    std::shared_ptr<AdvancedTemplateNode> template_;
    StringVector buildErrors;
    
    bool validateBuild() const;
    void addBuildError(const String& error);
};

} // namespace CHTL