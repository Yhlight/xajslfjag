#pragma once

#include "BaseNode.h"

namespace CHTL {

// 原始嵌入节点基类
class OriginNode : public BaseNode {
public:
    String originType;     // Html, Style, JavaScript, 或自定义类型
    String originName;     // 可选的名称
    String rawContent;     // 原始内容
    bool isNamed;          // 是否是命名的原始嵌入
    bool isCustomType;     // 是否是自定义类型
    StringUnorderedMap metadata;  // 元数据
    
    OriginNode(NodeType type, const String& oType, const String& content, const String& name = "", const Position& pos = Position())
        : BaseNode(type, name.empty() ? content : name, pos), 
          originType(oType), originName(name), rawContent(content), 
          isNamed(!name.empty()), isCustomType(false) {}
    
    // 基础操作
    void setContent(const String& content) { rawContent = content; }
    String getContent() const { return rawContent; }
    
    void setOriginName(const String& name) { 
        originName = name; 
        isNamed = !name.empty(); 
        value = name.empty() ? rawContent : name;
    }
    String getOriginName() const { return originName; }
    
    void setOriginType(const String& type) { 
        originType = type; 
        // 检查是否是自定义类型
        isCustomType = (type != "Html" && type != "Style" && type != "JavaScript");
    }
    String getOriginType() const { return originType; }
    
    // 元数据管理
    void setMetadata(const String& key, const String& value) { metadata[key] = value; }
    String getMetadata(const String& key, const String& defaultValue = "") const {
        auto it = metadata.find(key);
        return (it != metadata.end()) ? it->second : defaultValue;
    }
    bool hasMetadata(const String& key) const { return metadata.find(key) != metadata.end(); }
    void removeMetadata(const String& key) { metadata.erase(key); }
    
    // 类型检查
    bool isHtmlOrigin() const { return originType == "Html"; }
    bool isStyleOrigin() const { return originType == "Style"; }
    bool isJavaScriptOrigin() const { return originType == "JavaScript"; }
    bool isCustomOrigin() const { return isCustomType; }
    
    // 内容验证
    virtual bool validateContent() const;
    virtual StringVector getContentValidationErrors() const;
    
    // 生成方法
    virtual String generateOutput() const;
    virtual String getOutputType() const;
    
    // 重写基类方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    bool validate() const override;
    StringVector getValidationErrors() const override;
};

// HTML原始嵌入
class HtmlOriginNode : public OriginNode {
public:
    explicit HtmlOriginNode(const String& content, const String& name = "", const Position& pos = Position())
        : OriginNode(NodeType::ORIGIN_HTML, "Html", content, name, pos) {}
    
    // HTML特定验证
    bool validateContent() const override;
    StringVector getContentValidationErrors() const override;
    
    // HTML处理
    bool isWellFormed() const;
    StringVector extractTags() const;
    bool hasScriptTags() const;
    bool hasStyleTags() const;
    
    // 生成
    String generateOutput() const override { return rawContent; }
    String getOutputType() const override { return "text/html"; }
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// CSS原始嵌入
class StyleOriginNode : public OriginNode {
public:
    explicit StyleOriginNode(const String& content, const String& name = "", const Position& pos = Position())
        : OriginNode(NodeType::ORIGIN_STYLE, "Style", content, name, pos) {}
    
    // CSS特定验证
    bool validateContent() const override;
    StringVector getContentValidationErrors() const override;
    
    // CSS处理
    bool isValidCSS() const;
    StringVector extractSelectors() const;
    StringVector extractProperties() const;
    bool hasMediaQueries() const;
    bool hasKeyframes() const;
    
    // CSS优化
    String minifyCSS() const;
    String addVendorPrefixes() const;
    
    // 生成
    String generateOutput() const override;
    String getOutputType() const override { return "text/css"; }
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// JavaScript原始嵌入
class JavaScriptOriginNode : public OriginNode {
public:
    explicit JavaScriptOriginNode(const String& content, const String& name = "", const Position& pos = Position())
        : OriginNode(NodeType::ORIGIN_JAVASCRIPT, "JavaScript", content, name, pos) {}
    
    // JavaScript特定验证
    bool validateContent() const override;
    StringVector getContentValidationErrors() const override;
    
    // JavaScript处理
    bool isValidJavaScript() const;
    StringVector extractFunctions() const;
    StringVector extractVariables() const;
    bool hasESModuleSyntax() const;
    bool hasCommonJSSyntax() const;
    
    // JavaScript优化
    String minifyJavaScript() const;
    String addStrictMode() const;
    
    // 生成
    String generateOutput() const override { return rawContent; }
    String getOutputType() const override { return "application/javascript"; }
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 自定义类型原始嵌入
class CustomOriginNode : public OriginNode {
public:
    StringUnorderedMap customProperties;
    std::function<String(const String&)> contentProcessor;
    String outputMimeType;
    
    explicit CustomOriginNode(const String& type, const String& content, const String& name = "", const Position& pos = Position())
        : OriginNode(NodeType::ORIGIN_CUSTOM, type, content, name, pos) {
        isCustomType = true;
        outputMimeType = "text/plain";
    }
    
    // 自定义属性管理
    void setCustomProperty(const String& key, const String& value) { customProperties[key] = value; }
    String getCustomProperty(const String& key, const String& defaultValue = "") const {
        auto it = customProperties.find(key);
        return (it != customProperties.end()) ? it->second : defaultValue;
    }
    bool hasCustomProperty(const String& key) const { return customProperties.find(key) != customProperties.end(); }
    
    // 内容处理器
    void setContentProcessor(std::function<String(const String&)> processor) { contentProcessor = processor; }
    void setOutputMimeType(const String& mimeType) { outputMimeType = mimeType; }
    
    // 验证
    bool validateContent() const override;
    StringVector getContentValidationErrors() const override;
    
    // 生成
    String generateOutput() const override;
    String getOutputType() const override { return outputMimeType; }
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 原始嵌入管理器
class OriginManager {
public:
    // 注册自定义类型
    static void registerCustomType(const String& typeName, const String& defaultMimeType = "text/plain");
    static void unregisterCustomType(const String& typeName);
    static bool isRegisteredCustomType(const String& typeName);
    static StringVector getRegisteredCustomTypes();
    
    // 内容处理器注册
    static void registerContentProcessor(const String& typeName, std::function<String(const String&)> processor);
    static void unregisterContentProcessor(const String& typeName);
    static bool hasContentProcessor(const String& typeName);
    
    // 验证器注册
    static void registerValidator(const String& typeName, std::function<bool(const String&)> validator);
    static void unregisterValidator(const String& typeName);
    static bool hasValidator(const String& typeName);
    
    // 工厂方法
    static std::unique_ptr<OriginNode> createOriginNode(const String& type, const String& content, const String& name = "");
    
    // 批量处理
    static StringVector processAllOrigins(const std::vector<std::unique_ptr<OriginNode>>& origins);
    static String combineOrigins(const std::vector<std::unique_ptr<OriginNode>>& origins, const String& type);
    
private:
    static StringUnorderedMap registeredTypes;
    static std::unordered_map<String, std::function<String(const String&)>> contentProcessors;
    static std::unordered_map<String, std::function<bool(const String&)>> validators;
};

// 原始嵌入验证器
class OriginValidator {
public:
    // 基础验证
    static bool validateHTML(const String& content);
    static bool validateCSS(const String& content);
    static bool validateJavaScript(const String& content);
    
    // 详细验证
    static StringVector getHTMLValidationErrors(const String& content);
    static StringVector getCSSValidationErrors(const String& content);
    static StringVector getJavaScriptValidationErrors(const String& content);
    
    // 安全验证
    static bool hasXSSVulnerabilities(const String& htmlContent);
    static bool hasCSSInjection(const String& cssContent);
    static bool hasJSInjection(const String& jsContent);
    
    // 性能验证
    static bool hasPerformanceIssues(const String& content, const String& type);
    static StringVector getPerformanceWarnings(const String& content, const String& type);
    
private:
    static bool isValidHTMLTag(const String& tag);
    static bool isValidCSSProperty(const String& property);
    static bool isValidJSIdentifier(const String& identifier);
};

// 原始嵌入工厂
class OriginFactory {
public:
    static std::unique_ptr<HtmlOriginNode> createHtmlOrigin(const String& content, const String& name = "");
    static std::unique_ptr<StyleOriginNode> createStyleOrigin(const String& content, const String& name = "");
    static std::unique_ptr<JavaScriptOriginNode> createJavaScriptOrigin(const String& content, const String& name = "");
    static std::unique_ptr<CustomOriginNode> createCustomOrigin(const String& type, const String& content, const String& name = "");
    
    // 从文件创建
    static std::unique_ptr<OriginNode> createFromFile(const String& filePath, const String& type = "", const String& name = "");
    
    // 批量创建
    static std::vector<std::unique_ptr<OriginNode>> createFromDirectory(const String& dirPath, const String& type = "");
    
private:
    static String detectTypeFromExtension(const String& filePath);
    static String readFileContent(const String& filePath);
};

} // namespace CHTL