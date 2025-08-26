#pragma once
#include "../AST/CHTLNodes.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

// CHTL代码生成器配置
struct CHTLGeneratorConfig {
    bool minifyOutput = false;          // 压缩输出
    bool includeComments = true;        // 包含注释
    bool generateSourceMap = false;     // 生成源映射
    bool enableOptimization = true;     // 启用优化
    std::string indentString = "  ";    // 缩进字符串
    std::string outputCharset = "utf-8"; // 输出字符集
    bool enableCaching = true;          // 启用缓存
    bool validateOutput = true;         // 验证输出
};

// 生成结果
struct CHTLGenerationResult {
    std::string html;                   // 生成的HTML
    std::string css;                    // 生成的CSS
    std::string javascript;             // 生成的JavaScript
    std::vector<std::string> errors;   // 错误信息
    std::vector<std::string> warnings; // 警告信息
    std::unordered_map<std::string, std::string> metadata; // 元数据
    bool success = true;                // 是否成功
};

// CHTL样式作用域管理
class CHTLStyleScope {
private:
    std::string scopeId;
    std::unordered_map<std::string, std::string> localSelectors;
    bool isGlobal;
    
public:
    explicit CHTLStyleScope(const std::string& id, bool global = false);
    
    std::string getScopedSelector(const std::string& selector);
    void addLocalSelector(const std::string& original, const std::string& scoped);
    bool isGlobalScope() const { return isGlobal; }
    const std::string& getScopeId() const { return scopeId; }
};

// CHTL模板管理器
class CHTLTemplateManager {
private:
    std::unordered_map<std::string, std::shared_ptr<CHTLTemplateNode>> templates;
    std::unordered_map<std::string, std::shared_ptr<CHTLCustomNode>> customs;
    std::unordered_map<std::string, std::shared_ptr<CHTLNode>> origins;
    
public:
    void registerTemplate(const std::string& name, std::shared_ptr<CHTLTemplateNode> templateNode);
    void registerCustom(const std::string& name, std::shared_ptr<CHTLCustomNode> customNode);
    void registerOrigin(const std::string& name, std::shared_ptr<CHTLNode> originNode);
    
    std::shared_ptr<CHTLTemplateNode> getTemplate(const std::string& name);
    std::shared_ptr<CHTLCustomNode> getCustom(const std::string& name);
    std::shared_ptr<CHTLNode> getOrigin(const std::string& name);
    
    bool hasTemplate(const std::string& name) const;
    bool hasCustom(const std::string& name) const;
    bool hasOrigin(const std::string& name) const;
};

// CHTL变量解析器
class CHTLVariableResolver {
private:
    std::unordered_map<std::string, std::string> variables;
    std::unordered_map<std::string, std::string> globalVariables;
    
public:
    void setVariable(const std::string& name, const std::string& value);
    void setGlobalVariable(const std::string& name, const std::string& value);
    
    std::string resolveVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    std::string interpolateString(const std::string& input) const;
};

// CHTL代码生成器主类
class CHTLGenerator {
private:
    CHTLGeneratorConfig config;
    CHTLTemplateManager templateManager;
    CHTLVariableResolver variableResolver;
    std::vector<std::unique_ptr<CHTLStyleScope>> styleScopes;
    std::ostringstream htmlStream;
    std::ostringstream cssStream;
    std::ostringstream jsStream;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    int indentLevel;
    size_t nextScopeId;
    
    // 缓存系统
    std::unordered_map<std::string, std::string> generationCache;
    
public:
    explicit CHTLGenerator(const CHTLGeneratorConfig& cfg = CHTLGeneratorConfig());
    
    // 主生成方法
    CHTLGenerationResult generate(std::shared_ptr<CHTLDocumentNode> document);
    
    // 配置管理
    void setConfig(const CHTLGeneratorConfig& cfg) { config = cfg; }
    const CHTLGeneratorConfig& getConfig() const { return config; }
    
    // 错误和警告
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
private:
    // === 核心生成方法 ===
    
    // 文档级别生成
    void generateDocument(std::shared_ptr<CHTLDocumentNode> document);
    void generateDocumentHead(std::shared_ptr<CHTLDocumentNode> document);
    void generateDocumentBody(std::shared_ptr<CHTLDocumentNode> document);
    
    // 节点生成调度
    void generateNode(std::shared_ptr<CHTLNode> node);
    
    // HTML元素生成
    void generateElement(std::shared_ptr<CHTLElementNode> element);
    void generateElementTag(std::shared_ptr<CHTLElementNode> element, bool isClosing = false);
    void generateElementAttributes(std::shared_ptr<CHTLElementNode> element);
    void generateElementChildren(std::shared_ptr<CHTLElementNode> element);
    void generateTextNode(std::shared_ptr<CHTLNode> textNode);
    
    // 模板系统生成
    void generateTemplateDefinition(std::shared_ptr<CHTLTemplateNode> templateNode);
    void generateTemplateUsage(std::shared_ptr<CHTLNode> templateUsage);
    void generateTemplateInheritance(std::shared_ptr<CHTLTemplateNode> templateNode, std::shared_ptr<CHTLNode> inheritNode);
    
    // 自定义系统生成
    void generateCustomDefinition(std::shared_ptr<CHTLCustomNode> customNode);
    void generateCustomUsage(std::shared_ptr<CHTLNode> customUsage);
    void generateCustomSpecialization(std::shared_ptr<CHTLCustomNode> customNode);
    
    // 样式生成
    void generateStyleBlock(std::shared_ptr<CHTLStyleNode> styleNode);
    void generateStyleRule(std::shared_ptr<CHTLNode> styleRule, const std::string& scopeId = "");
    void generateStyleProperty(std::shared_ptr<CHTLNode> styleProperty);
    void generateGlobalStyles();
    
    // 脚本生成
    void generateScriptBlock(std::shared_ptr<CHTLScriptNode> scriptNode);
    void generateGlobalScripts();
    
    // 原始嵌入生成
    void generateOriginDefinition(std::shared_ptr<CHTLNode> originNode);
    
    // 导入和命名空间
    void generateImportStatement(std::shared_ptr<CHTLImportNode> importNode);
    void generateNamespaceDeclaration(std::shared_ptr<CHTLNode> namespaceNode);
    void generateUseDeclaration(std::shared_ptr<CHTLNode> useNode);
    
    // 配置生成
    void generateConfigurationDefinition(std::shared_ptr<CHTLNode> configNode);
    
    // 注释生成
    void generateComment(std::shared_ptr<CHTLNode> commentNode);
    
    // === 辅助方法 ===
    
    // 样式作用域管理
    std::string createStyleScope(bool isGlobal = false);
    CHTLStyleScope* getCurrentStyleScope();
    void pushStyleScope(std::unique_ptr<CHTLStyleScope> scope);
    void popStyleScope();
    
    // 选择器处理
    std::string processCHTLSelector(const std::string& selector, const std::string& scopeId = "");
    std::string generateScopedSelector(const std::string& selector, const std::string& scopeId);
    std::string processReferenceSelector(const std::string& selector);
    std::string processAmpersandSelector(const std::string& selector, const std::string& parent);
    
    // 模板处理
    std::shared_ptr<CHTLNode> instantiateTemplate(const std::string& templateName, const std::string& templateType);
    void applyTemplateInheritance(std::shared_ptr<CHTLTemplateNode> child, std::shared_ptr<CHTLTemplateNode> parent);
    void applyCustomSpecialization(std::shared_ptr<CHTLCustomNode> custom, std::shared_ptr<CHTLNode> target);
    
    // 变量处理
    void processVariableDefinitions(std::shared_ptr<CHTLNode> node);
    std::string resolveVariableUsage(const std::string& variableName);
    std::string interpolateVariables(const std::string& content);
    
    // 输出格式化
    void writeIndent();
    void increaseIndent();
    void decreaseIndent();
    void writeLine(const std::string& content = "");
    void writeHTML(const std::string& content);
    void writeCSS(const std::string& content);
    void writeJS(const std::string& content);
    
    // 错误和警告处理
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    void addError(const std::string& message, std::shared_ptr<CHTLNode> node);
    void addWarning(const std::string& message, std::shared_ptr<CHTLNode> node);
    
    // 验证和优化
    bool validateHTML(const std::string& html);
    bool validateCSS(const std::string& css);
    std::string optimizeHTML(const std::string& html);
    std::string optimizeCSS(const std::string& css);
    std::string minifyHTML(const std::string& html);
    std::string minifyCSS(const std::string& css);
    
    // 缓存管理
    std::string getCacheKey(std::shared_ptr<CHTLNode> node);
    bool hasCachedResult(const std::string& key);
    std::string getCachedResult(const std::string& key);
    void setCachedResult(const std::string& key, const std::string& result);
    
    // HTML实体处理
    std::string escapeHTML(const std::string& text) const;
    std::string unescapeHTML(const std::string& text);
    
    // CSS处理
    std::string normalizeCSSProperty(const std::string& property);
    std::string normalizeCSSValue(const std::string& value);
    bool isValidCSSProperty(const std::string& property);
    bool isValidCSSValue(const std::string& property, const std::string& value);
    
    // 路径处理
    std::string resolveImportPath(const std::string& path);
    std::string resolveAssetPath(const std::string& path);
    
    // 工具方法
    void collectDefinitions(std::shared_ptr<CHTLDocumentNode> document);
    std::string getIndent() const;
    std::string addIndent(const std::string& text, int level) const;
    bool isSelfClosingTag(const std::string& tagName) const;
    bool hasBlockChildElements(std::shared_ptr<CHTLElementNode> element) const;
    void preprocessDocument(std::shared_ptr<CHTLDocumentNode> document);
    CHTLGenerationResult createFailedResult();
    std::string generateUniqueId(const std::string& prefix = "chtl");
    std::string sanitizeClassName(const std::string& className);
    std::string sanitizeId(const std::string& id);
    bool isEmptyOrWhitespace(const std::string& str);
    std::string trim(const std::string& str) const;
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string join(const std::vector<std::string>& parts, const std::string& separator);
    
    // 静态HTML标签验证
    static const std::vector<std::string>& getVoidElements();
    static const std::vector<std::string>& getBlockElements();
    static const std::vector<std::string>& getInlineElements();
    static bool isVoidElement(const std::string& tagName);
    static bool isBlockElement(const std::string& tagName);
    static bool isInlineElement(const std::string& tagName);
};

} // namespace CHTL