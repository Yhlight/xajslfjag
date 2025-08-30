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

// 自定义类型处理器 - 仅用于用户扩展@Vue、@React等自定义类型
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

// 自定义原始嵌入节点
// 只处理用户自定义的类型，不包含任何内置框架实现
class CustomOriginNode : public OriginNode {
private:
    OriginEmbedType embedType;                    // 嵌入类型
    String customTypeIdentifier;                  // 自定义类型标识符 (如 "Vue", "React")
    std::shared_ptr<CustomOriginTypeHandler> handler; // 用户提供的类型处理器

public:
    // 构造函数 - 只接受用户自定义类型
    CustomOriginNode(const String& identifier, const Position& pos);
    
    // 基础操作
    void setHandler(std::shared_ptr<CustomOriginTypeHandler> typeHandler);
    std::shared_ptr<CustomOriginTypeHandler> getHandler() const { return handler; }
    
    OriginEmbedType getEmbedType() const { return embedType; }
    String getCustomTypeIdentifier() const { return customTypeIdentifier; }
    
    // 内容处理 - 委托给用户处理器
    bool validateContent() const override;
    String transformContent(const String& targetFormat = "html") const override;
    String compile(const StringUnorderedMap& options = {}) const override;
    
    // AST操作
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
    // 静态工厂方法 - 只创建自定义类型节点
    static std::unique_ptr<CustomOriginNode> createCustomType(const String& identifier, 
                                                             const String& content, 
                                                             const Position& pos);
    
    // 检查是否为自定义类型标识符
    static bool isCustomTypeIdentifier(const String& identifier);
};

// 自定义类型注册器 - 由用户扩展使用
class CustomOriginTypeRegistry {
public:
    static CustomOriginTypeRegistry& getInstance();
    
    // 用户注册自定义类型处理器
    void registerType(const String& identifier, 
                     std::function<std::shared_ptr<CustomOriginTypeHandler>()> handlerFactory);
    
    void unregisterType(const String& identifier);
    bool isTypeRegistered(const String& identifier) const;
    std::shared_ptr<CustomOriginTypeHandler> getHandler(const String& identifier) const;
    
    StringVector getAllIdentifiers() const;
    size_t getRegisteredTypeCount() const;
    
    // 创建自定义类型节点
    std::unique_ptr<CustomOriginNode> createNode(const String& identifier, 
                                                const String& content, 
                                                const Position& pos) const;

private:
    CustomOriginTypeRegistry() = default;
    ~CustomOriginTypeRegistry() = default;
    
    std::unordered_map<String, std::function<std::shared_ptr<CustomOriginTypeHandler>()>> customHandlers;
};

}  // namespace CHTL