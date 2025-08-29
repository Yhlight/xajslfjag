#pragma once

#include "OriginNode.h"
#include "../../Util/Common.h"
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {

// 自定义原始嵌入类型
enum class CustomOriginType {
    VUE,                // Vue.js 组件
    REACT,              // React 组件
    ANGULAR,            // Angular 组件
    SVELTE,             // Svelte 组件
    MARKDOWN,           // Markdown 文档
    YAML,               // YAML 配置
    JSON,               // JSON 数据
    XML,                // XML 文档
    SQL,                // SQL 查询
    PYTHON,             // Python 脚本
    TYPESCRIPT,         // TypeScript 代码
    SCSS,               // SCSS 样式
    LESS,               // Less 样式
    GRAPHQL,            // GraphQL 查询
    WASM,               // WebAssembly
    GLSL,               // OpenGL 着色器
    HLSL,               // DirectX 着色器
    CUSTOM              // 用户自定义类型
};

// 自定义类型处理器
class CustomOriginTypeHandler {
public:
    virtual ~CustomOriginTypeHandler() = default;
    
    // 验证内容格式
    virtual bool validateContent(const String& content) const = 0;
    
    // 转换为目标格式
    virtual String transformContent(const String& content, const String& targetFormat = "html") const = 0;
    
    // 提取依赖
    virtual StringVector extractDependencies(const String& content) const = 0;
    
    // 获取支持的文件扩展名
    virtual StringVector getSupportedExtensions() const = 0;
    
    // 获取MIME类型
    virtual String getMimeType() const = 0;
    
    // 是否需要编译
    virtual bool requiresCompilation() const = 0;
    
    // 编译内容
    virtual String compile(const String& content, const StringUnorderedMap& options = {}) const = 0;
    
    // 获取语法高亮信息
    virtual String getSyntaxHighlighting() const = 0;
};

// Vue.js 类型处理器
class VueTypeHandler : public CustomOriginTypeHandler {
public:
    bool validateContent(const String& content) const override;
    String transformContent(const String& content, const String& targetFormat = "html") const override;
    StringVector extractDependencies(const String& content) const override;
    StringVector getSupportedExtensions() const override { return {".vue"}; }
    String getMimeType() const override { return "text/x-vue"; }
    bool requiresCompilation() const override { return true; }
    String compile(const String& content, const StringUnorderedMap& options = {}) const override;
    String getSyntaxHighlighting() const override { return "vue"; }
    
private:
    String extractTemplate(const String& content) const;
    String extractScript(const String& content) const;
    String extractStyle(const String& content) const;
    StringUnorderedMap extractProps(const String& script) const;
};

// React 类型处理器
class ReactTypeHandler : public CustomOriginTypeHandler {
public:
    bool validateContent(const String& content) const override;
    String transformContent(const String& content, const String& targetFormat = "html") const override;
    StringVector extractDependencies(const String& content) const override;
    StringVector getSupportedExtensions() const override { return {".jsx", ".tsx"}; }
    String getMimeType() const override { return "text/jsx"; }
    bool requiresCompilation() const override { return true; }
    String compile(const String& content, const StringUnorderedMap& options = {}) const override;
    String getSyntaxHighlighting() const override { return "jsx"; }
    
private:
    StringVector extractImports(const String& content) const;
    String extractComponent(const String& content) const;
    StringUnorderedMap extractProps(const String& content) const;
};

// Markdown 类型处理器
class MarkdownTypeHandler : public CustomOriginTypeHandler {
public:
    bool validateContent(const String& content) const override;
    String transformContent(const String& content, const String& targetFormat = "html") const override;
    StringVector extractDependencies(const String& content) const override;
    StringVector getSupportedExtensions() const override { return {".md", ".markdown"}; }
    String getMimeType() const override { return "text/markdown"; }
    bool requiresCompilation() const override { return true; }
    String compile(const String& content, const StringUnorderedMap& options = {}) const override;
    String getSyntaxHighlighting() const override { return "markdown"; }
    
private:
    String parseMarkdownToHTML(const String& content) const;
    StringVector extractCodeBlocks(const String& content) const;
    StringVector extractLinks(const String& content) const;
};

// YAML 类型处理器
class YamlTypeHandler : public CustomOriginTypeHandler {
public:
    bool validateContent(const String& content) const override;
    String transformContent(const String& content, const String& targetFormat = "json") const override;
    StringVector extractDependencies(const String& content) const override;
    StringVector getSupportedExtensions() const override { return {".yml", ".yaml"}; }
    String getMimeType() const override { return "application/x-yaml"; }
    bool requiresCompilation() const override { return false; }
    String compile(const String& content, const StringUnorderedMap& options = {}) const override;
    String getSyntaxHighlighting() const override { return "yaml"; }
    
private:
    String yamlToJson(const String& yaml) const;
    bool validateYamlSyntax(const String& yaml) const;
};

// 自定义原始嵌入节点
class CustomOriginNode : public OriginNode {
public:
    CustomOriginType customType;               // 自定义类型
    String typeIdentifier;                     // 类型标识符 (如 @Vue, @React)
    std::shared_ptr<CustomOriginTypeHandler> handler; // 类型处理器
    StringUnorderedMap compileOptions;         // 编译选项
    StringVector dependencies;                 // 依赖列表
    bool isCompiled;                          // 是否已编译
    String compiledContent;                   // 编译后的内容
    String sourceFile;                        // 源文件路径
    String targetFormat;                      // 目标格式
    
    explicit CustomOriginNode(CustomOriginType type, const String& identifier, const Position& pos = Position());
    ~CustomOriginNode() override = default;
    
    // 设置处理器
    void setHandler(std::shared_ptr<CustomOriginTypeHandler> typeHandler);
    std::shared_ptr<CustomOriginTypeHandler> getHandler() const { return handler; }
    
    // 内容管理
    void setContent(const String& content) override;
    String getContent() const override;
    String getTransformedContent(const String& format = "") const;
    
    // 编译管理
    bool compile(const StringUnorderedMap& options = {});
    bool isCompilationRequired() const;
    String getCompiledContent() const { return compiledContent; }
    void setCompileOptions(const StringUnorderedMap& options);
    StringUnorderedMap getCompileOptions() const { return compileOptions; }
    
    // 依赖管理
    StringVector extractDependencies();
    void addDependency(const String& dependency);
    void setDependencies(const StringVector& deps);
    StringVector getDependencies() const { return dependencies; }
    
    // 验证
    bool validateContent() const;
    StringVector getValidationErrors() const;
    
    // 文件操作
    bool loadFromFile(const String& filePath);
    bool saveToFile(const String& filePath) const;
    void setSourceFile(const String& filePath) { sourceFile = filePath; }
    String getSourceFile() const { return sourceFile; }
    
    // 格式转换
    void setTargetFormat(const String& format) { targetFormat = format; }
    String getTargetFormat() const { return targetFormat; }
    String convertToFormat(const String& format) const;
    
    // BaseNode接口
    NodeType getType() const override { return NodeType::ORIGIN; }
    String getValue() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
    // 序列化
    String toString() const override;
    String toHTML() const;
    String toJSON() const;
    String toXML() const;
    
    // 静态工厂方法
    static std::unique_ptr<CustomOriginNode> createVueComponent(const String& content, const Position& pos = Position());
    static std::unique_ptr<CustomOriginNode> createReactComponent(const String& content, const Position& pos = Position());
    static std::unique_ptr<CustomOriginNode> createMarkdownDocument(const String& content, const Position& pos = Position());
    static std::unique_ptr<CustomOriginNode> createYamlConfig(const String& content, const Position& pos = Position());
    static std::unique_ptr<CustomOriginNode> createCustomType(CustomOriginType type, const String& identifier, 
                                                             const String& content, const Position& pos = Position());
    
    // 解析方法
    static CustomOriginType parseCustomType(const String& typeIdentifier);
    static String customTypeToString(CustomOriginType type);
    static String customTypeToIdentifier(CustomOriginType type);
    
private:
    // 内部辅助方法
    void initializeHandler();
    bool isValidTypeIdentifier(const String& identifier) const;
    String generateDefaultContent() const;
    
    // 编译辅助
    String preprocessContent(const String& content) const;
    String postprocessContent(const String& content) const;
    
    // 文件操作辅助
    String detectFileType(const String& filePath) const;
    bool isValidFile(const String& filePath) const;
};

// 自定义类型注册管理器
class CustomOriginTypeRegistry {
public:
    static CustomOriginTypeRegistry& getInstance();
    
    // 类型注册
    void registerType(const String& identifier, CustomOriginType type, 
                     std::function<std::shared_ptr<CustomOriginTypeHandler>()> handlerFactory);
    void unregisterType(const String& identifier);
    bool isTypeRegistered(const String& identifier) const;
    
    // 处理器获取
    std::shared_ptr<CustomOriginTypeHandler> getHandler(const String& identifier) const;
    std::shared_ptr<CustomOriginTypeHandler> getHandler(CustomOriginType type) const;
    
    // 类型查询
    CustomOriginType getType(const String& identifier) const;
    String getIdentifier(CustomOriginType type) const;
    StringVector getAllIdentifiers() const;
    StringVector getIdentifiersByType(CustomOriginType type) const;
    
    // 扩展名映射
    void registerExtension(const String& extension, const String& identifier);
    String getIdentifierByExtension(const String& extension) const;
    StringVector getSupportedExtensions() const;
    
    // 批量注册
    void registerBuiltinTypes();
    void registerWebFrameworkTypes();
    void registerMarkupTypes();
    void registerDataTypes();
    void registerShaderTypes();
    
    // 自定义类型工厂
    std::unique_ptr<CustomOriginNode> createNode(const String& identifier, const String& content, 
                                                const Position& pos = Position()) const;
    
    // 调试和诊断
    void dumpRegisteredTypes() const;
    size_t getRegisteredTypeCount() const;
    bool validateRegistry() const;
    
private:
    CustomOriginTypeRegistry() = default;
    ~CustomOriginTypeRegistry() = default;
    
    struct TypeRegistration {
        String identifier;
        CustomOriginType type;
        std::function<std::shared_ptr<CustomOriginTypeHandler>()> handlerFactory;
        StringVector supportedExtensions;
        String description;
    };
    
    std::unordered_map<String, TypeRegistration> registrations;
    std::unordered_map<String, String> extensionMapping; // extension -> identifier
    std::unordered_map<CustomOriginType, String> typeToIdentifier;
    
    // 内部方法
    void registerBuiltinType(const String& identifier, CustomOriginType type,
                           std::function<std::shared_ptr<CustomOriginTypeHandler>()> factory,
                           const StringVector& extensions = {});
};

// 自定义类型配置
struct CustomOriginTypeConfig {
    String identifier;                         // 类型标识符
    String displayName;                        // 显示名称
    String description;                        // 描述
    StringVector fileExtensions;               // 支持的文件扩展名
    String mimeType;                          // MIME类型
    bool requiresCompilation;                 // 是否需要编译
    StringUnorderedMap defaultCompileOptions; // 默认编译选项
    StringVector dependencies;                // 默认依赖
    String syntaxHighlighting;                // 语法高亮
    String iconPath;                          // 图标路径
    String documentationUrl;                  // 文档链接
    
    CustomOriginTypeConfig() : requiresCompilation(false) {}
};

// 自定义类型构建器
class CustomOriginTypeBuilder {
public:
    explicit CustomOriginTypeBuilder(const String& identifier);
    ~CustomOriginTypeBuilder() = default;
    
    // 流式配置
    CustomOriginTypeBuilder& displayName(const String& name);
    CustomOriginTypeBuilder& description(const String& desc);
    CustomOriginTypeBuilder& fileExtensions(const StringVector& extensions);
    CustomOriginTypeBuilder& mimeType(const String& mime);
    CustomOriginTypeBuilder& requiresCompilation(bool requires = true);
    CustomOriginTypeBuilder& defaultCompileOptions(const StringUnorderedMap& options);
    CustomOriginTypeBuilder& dependencies(const StringVector& deps);
    CustomOriginTypeBuilder& syntaxHighlighting(const String& syntax);
    CustomOriginTypeBuilder& icon(const String& iconPath);
    CustomOriginTypeBuilder& documentation(const String& docUrl);
    
    // 处理器设置
    CustomOriginTypeBuilder& handler(std::function<std::shared_ptr<CustomOriginTypeHandler>()> handlerFactory);
    
    // 构建和注册
    CustomOriginTypeConfig build();
    bool registerToGlobal();
    
private:
    CustomOriginTypeConfig config;
    std::function<std::shared_ptr<CustomOriginTypeHandler>()> handlerFactory;
    StringVector buildErrors;
    
    bool validateConfig() const;
    void addBuildError(const String& error);
};

// 工具函数
namespace CustomOriginUtils {
    // 内容检测
    CustomOriginType detectContentType(const String& content);
    String detectFileTypeByExtension(const String& filePath);
    String detectMimeType(const String& content);
    
    // 内容转换
    String convertContent(const String& content, CustomOriginType fromType, CustomOriginType toType);
    String normalizeContent(const String& content, CustomOriginType type);
    
    // 依赖分析
    StringVector analyzeContentDependencies(const String& content, CustomOriginType type);
    StringVector extractImportStatements(const String& content, CustomOriginType type);
    
    // 验证工具
    bool validateContentSyntax(const String& content, CustomOriginType type);
    StringVector getContentValidationErrors(const String& content, CustomOriginType type);
    
    // 元数据提取
    StringUnorderedMap extractContentMetadata(const String& content, CustomOriginType type);
    String extractTitle(const String& content, CustomOriginType type);
    String extractDescription(const String& content, CustomOriginType type);
    
    // 优化工具
    String optimizeContent(const String& content, CustomOriginType type);
    String minifyContent(const String& content, CustomOriginType type);
    String beautifyContent(const String& content, CustomOriginType type);
}

} // namespace CHTL