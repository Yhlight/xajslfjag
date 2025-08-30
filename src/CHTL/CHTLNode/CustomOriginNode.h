#pragma once

#include "OriginNode.h"
#include "../../Util/Common.h"
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {

// 原始嵌入只支持三种原生类型：@Html, @Style, @JavaScript
// @Vue, @React等都归为自定义类型，由用户扩展实现
enum class OriginEmbedType {
    HTML,               // @Html - HTML原始嵌入
    STYLE,              // @Style - CSS原始嵌入  
    JAVASCRIPT,         // @JavaScript - JavaScript原始嵌入
    CUSTOM              // 自定义类型 (@Vue, @React等用户扩展)
};

// 自定义类型处理器 - 仅用于扩展@Vue、@React等自定义类型
class CustomOriginTypeHandler {
public:
    virtual ~CustomOriginTypeHandler() = default;
    
    // 验证内容格式
    virtual bool validateContent(const String& content) const = 0;
    
    // 转换为目标格式  
    virtual String transformContent(const String& content, const String& targetFormat = "html") const = 0;
    
    // 编译内容
    virtual String compile(const String& content, const StringUnorderedMap& options = {}) const = 0;
    
    // 获取类型标识符
    virtual String getTypeIdentifier() const = 0;
};

// Vue.js 自定义类型处理器示例
class VueTypeHandler : public CustomOriginTypeHandler {
public:
    bool validateContent(const String& content) const override;
    String transformContent(const String& content, const String& targetFormat = "html") const override;
    String compile(const String& content, const StringUnorderedMap& options = {}) const override;
    String getTypeIdentifier() const override { return "Vue"; }
    
private:
    String extractTemplate(const String& content) const;
    String extractScript(const String& content) const;
    String extractStyle(const String& content) const;
};

// React 自定义类型处理器示例
class ReactTypeHandler : public CustomOriginTypeHandler {
public:
    bool validateContent(const String& content) const override;
    String transformContent(const String& content, const String& targetFormat = "html") const override;
    String compile(const String& content, const StringUnorderedMap& options = {}) const override;
    String getTypeIdentifier() const override { return "React"; }
    
private:
    String extractJSX(const String& content) const;
    String extractImports(const String& content) const;
};

// 自定义原始嵌入节点 - 仅处理非原生的自定义类型
class CustomOriginNode : public OriginNode {
public:
    String customTypeIdentifier;              // 自定义类型标识符 (如 "Vue", "React")
    std::shared_ptr<CustomOriginTypeHandler> handler; // 类型处理器
    StringUnorderedMap compileOptions;        // 编译选项
    bool isCompiled;                         // 是否已编译
    String compiledContent;                  // 编译后的内容
    
    explicit CustomOriginNode(const String& typeIdentifier, const String& content, 
                             const String& name = "", const Position& pos = Position());
    ~CustomOriginNode() override = default;
    
    // 设置处理器
    void setHandler(std::shared_ptr<CustomOriginTypeHandler> typeHandler);
    std::shared_ptr<CustomOriginTypeHandler> getHandler() const { return handler; }
    
    // 编译管理
    bool compile(const StringUnorderedMap& options = {});
    String getCompiledContent() const { return compiledContent; }
    void setCompileOptions(const StringUnorderedMap& options);
    
    // 验证
    bool validateContent() const;
    
    // BaseNode接口
    NodeType getType() const override { return NodeType::ORIGIN_CUSTOM; }
    String getValue() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
    // 序列化
    String toString() const override;
    String toHTML() const;
    
    // 静态工厂方法
    static std::unique_ptr<CustomOriginNode> createVueComponent(const String& content, 
                                                               const String& name = "", 
                                                               const Position& pos = Position());
    static std::unique_ptr<CustomOriginNode> createReactComponent(const String& content, 
                                                                 const String& name = "", 
                                                                 const Position& pos = Position());
    
    // 解析方法
    static bool isCustomType(const String& typeIdentifier);
    static String normalizeTypeIdentifier(const String& typeIdentifier);
    
private:
    void initializeHandler();
    bool isValidCustomType() const;
};

// 自定义类型注册管理器 - 简化版本
class CustomOriginTypeRegistry {
public:
    static CustomOriginTypeRegistry& getInstance();
    
    // 类型注册 - 仅用于自定义类型
    void registerCustomType(const String& identifier, 
                           std::function<std::shared_ptr<CustomOriginTypeHandler>()> handlerFactory);
    void unregisterCustomType(const String& identifier);
    bool isCustomTypeRegistered(const String& identifier) const;
    
    // 处理器获取
    std::shared_ptr<CustomOriginTypeHandler> getHandler(const String& identifier) const;
    
    // 自定义类型工厂
    std::unique_ptr<CustomOriginNode> createCustomNode(const String& identifier, 
                                                      const String& content, 
                                                      const String& name = "",
                                                      const Position& pos = Position()) const;
    
    // 内置自定义类型注册
    void registerBuiltinCustomTypes();
    
private:
    CustomOriginTypeRegistry() = default;
    ~CustomOriginTypeRegistry() = default;
    
    std::unordered_map<String, std::function<std::shared_ptr<CustomOriginTypeHandler>()>> customHandlers;
    
    void registerBuiltinType(const String& identifier,
                           std::function<std::shared_ptr<CustomOriginTypeHandler>()> factory);
};

} // namespace CHTL