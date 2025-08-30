#include "CustomOriginNode.h"
#include <sstream>

namespace CHTL {

// ========== CustomOriginNode 实现 ==========
// 严格按照文档实现，只支持三种原生类型：@Html, @Style, @JavaScript
// 其他类型(@Vue, @React等)由用户扩展实现

CustomOriginNode::CustomOriginNode(const String& identifier, const Position& pos) 
    : OriginNode(NodeType::ORIGIN_CUSTOM, pos), customTypeIdentifier(identifier) {
    
    // 根据文档，只有三种原生类型
    if (identifier == "@Html") {
        embedType = OriginEmbedType::HTML;
    } else if (identifier == "@Style") {
        embedType = OriginEmbedType::STYLE;
    } else if (identifier == "@JavaScript") {
        embedType = OriginEmbedType::JAVASCRIPT;
    } else {
        // 所有其他类型都是自定义类型，由用户扩展实现
        embedType = OriginEmbedType::CUSTOM;
    }
}

void CustomOriginNode::setHandler(std::shared_ptr<CustomOriginTypeHandler> typeHandler) {
    handler = typeHandler;
}

bool CustomOriginNode::validateContent() const {
    // 如果是自定义类型且有处理器，使用处理器验证
    if (embedType == OriginEmbedType::CUSTOM && handler) {
        return handler->validateContent(getValue());
    }
    
    // 原生类型不需要特殊验证
    return true;
}

String CustomOriginNode::transformContent(const String& targetFormat) const {
    // 如果是自定义类型且有处理器，使用处理器转换
    if (embedType == OriginEmbedType::CUSTOM && handler) {
        return handler->transformContent(getValue(), targetFormat);
    }
    
    // 原生类型直接返回内容
    return getValue();
}

String CustomOriginNode::compile(const StringUnorderedMap& options) const {
    // 如果是自定义类型且有处理器，使用处理器编译
    if (embedType == OriginEmbedType::CUSTOM && handler) {
        return handler->compile(getValue(), options);
    }
    
    // 原生类型直接返回内容
    return getValue();
}

String CustomOriginNode::toString() const {
    std::ostringstream oss;
    
    oss << "[Origin] " << customTypeIdentifier;
    
    if (!name.empty()) {
        oss << " " << name;
    }
    
    oss << " {\n";
    oss << getValue();
    oss << "\n}";
    
    return oss.str();
}

std::unique_ptr<BaseNode> CustomOriginNode::clone() const {
    auto cloned = std::make_unique<CustomOriginNode>(customTypeIdentifier, position);
    cloned->setValue(getValue());
    cloned->name = name;
    cloned->handler = handler;
    cloned->embedType = embedType;
    
    return cloned;
}

// 静态工厂方法 - 只创建自定义类型节点
std::unique_ptr<CustomOriginNode> CustomOriginNode::createCustomType(const String& identifier, 
                                                                    const String& content, 
                                                                    const Position& pos) {
    auto node = std::make_unique<CustomOriginNode>(identifier, pos);
    node->setValue(content);
    return node;
}

// 检查是否为自定义类型标识符
bool CustomOriginNode::isCustomTypeIdentifier(const String& identifier) {
    // 原生类型 - 文档明确规定的三种类型
    if (identifier == "@Html" || identifier == "@Style" || identifier == "@JavaScript") {
        return false;
    }
    
    // 所有以@开头的其他标识符都是自定义类型，由用户扩展实现
    return identifier.length() > 1 && identifier[0] == '@';
}

// ========== CustomOriginTypeRegistry 实现 ==========
// 注册器只提供接口，不包含任何内置框架实现

CustomOriginTypeRegistry& CustomOriginTypeRegistry::getInstance() {
    static CustomOriginTypeRegistry instance;
    return instance;
}

void CustomOriginTypeRegistry::registerType(const String& identifier, 
                                           std::function<std::shared_ptr<CustomOriginTypeHandler>()> handlerFactory) {
    // 用户注册自定义类型处理器
    customHandlers[identifier] = handlerFactory;
}

void CustomOriginTypeRegistry::unregisterType(const String& identifier) {
    customHandlers.erase(identifier);
}

bool CustomOriginTypeRegistry::isTypeRegistered(const String& identifier) const {
    return customHandlers.find(identifier) != customHandlers.end();
}

std::shared_ptr<CustomOriginTypeHandler> CustomOriginTypeRegistry::getHandler(const String& identifier) const {
    auto it = customHandlers.find(identifier);
    if (it != customHandlers.end()) {
        return it->second();
    }
    return nullptr;
}

StringVector CustomOriginTypeRegistry::getAllIdentifiers() const {
    StringVector identifiers;
    for (const auto& pair : customHandlers) {
        identifiers.push_back(pair.first);
    }
    return identifiers;
}

size_t CustomOriginTypeRegistry::getRegisteredTypeCount() const {
    return customHandlers.size();
}

std::unique_ptr<CustomOriginNode> CustomOriginTypeRegistry::createNode(const String& identifier, 
                                                                      const String& content, 
                                                                      const Position& pos) const {
    auto node = CustomOriginNode::createCustomType(identifier, content, pos);
    
    // 如果有用户注册的处理器，设置它
    auto handler = getHandler(identifier);
    if (handler) {
        node->setHandler(handler);
    }
    
    return node;
}

}  // namespace CHTL