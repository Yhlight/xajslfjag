#include "SpecializationNode.h"
#include "../CHTLLexer/GlobalMap.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

// ========== DeleteNode 实现 ==========

DeleteNode::DeleteNode(const Position& pos)
    : BaseNode(NodeType::DELETE, "", pos), operationType(DeleteOperationType::PROPERTY), 
      elementIndex(SIZE_MAX), isIndexed(false) {
}

void DeleteNode::addTarget(const String& target) {
    targets.push_back(target);
    updateOperationType();
}

void DeleteNode::addTargets(const StringVector& targetList) {
    targets.insert(targets.end(), targetList.begin(), targetList.end());
    updateOperationType();
}

void DeleteNode::setInheritanceTarget(const String& target) {
    inheritanceTarget = target;
    operationType = DeleteOperationType::INHERITANCE;
}

void DeleteNode::setElementTarget(const String& elementName, size_t index) {
    targets.clear();
    targets.push_back(elementName);
    elementIndex = index;
    isIndexed = (index != SIZE_MAX);
    operationType = DeleteOperationType::ELEMENT;
}

String DeleteNode::getDeletionDescription() const {
    std::ostringstream oss;
    oss << "Delete " << operationTypeToString() << ": ";
    
    switch (operationType) {
        case DeleteOperationType::PROPERTY:
            for (size_t i = 0; i < targets.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << targets[i];
            }
            break;
            
        case DeleteOperationType::INHERITANCE:
            oss << inheritanceTarget;
            break;
            
        case DeleteOperationType::ELEMENT:
            if (!targets.empty()) {
                oss << targets[0];
                if (isIndexed) {
                    oss << "[" << elementIndex << "]";
                }
            }
            break;
            
        case DeleteOperationType::TEMPLATE:
        case DeleteOperationType::CUSTOM:
            for (size_t i = 0; i < targets.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << targets[i];
            }
            break;
    }
    
    return oss.str();
}

String DeleteNode::toString() const {
    return "delete " + getDeletionDescription();
}

String DeleteNode::toHTML() const {
    return "<!-- " + toString() + " -->";
}

std::unique_ptr<DeleteNode> DeleteNode::createPropertyDeletion(const StringVector& properties, const Position& pos) {
    auto node = std::make_unique<DeleteNode>(pos);
    node->addTargets(properties);
    node->operationType = DeleteOperationType::PROPERTY;
    return node;
}

std::unique_ptr<DeleteNode> DeleteNode::createInheritanceDeletion(const String& inheritanceTarget, const Position& pos) {
    auto node = std::make_unique<DeleteNode>(pos);
    node->setInheritanceTarget(inheritanceTarget);
    return node;
}

std::unique_ptr<DeleteNode> DeleteNode::createElementDeletion(const String& elementName, size_t index, const Position& pos) {
    auto node = std::make_unique<DeleteNode>(pos);
    node->setElementTarget(elementName, index);
    return node;
}

DeleteOperationType DeleteNode::determineOperationType(const String& targetString) {
    String trimmed = targetString;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
    
    // 检查是否为继承删除
    if (trimmed.find("@Style") != String::npos || 
        trimmed.find("@Element") != String::npos || 
        trimmed.find("@Var") != String::npos) {
        return DeleteOperationType::INHERITANCE;
    }
    
    // 检查是否为模板删除
    if (trimmed.find("[Template]") != String::npos) {
        return DeleteOperationType::TEMPLATE;
    }
    
    // 检查是否为自定义删除
    if (trimmed.find("[Custom]") != String::npos) {
        return DeleteOperationType::CUSTOM;
    }
    
    // 检查是否为HTML元素
    if (HTMLElementMap::isHTMLElement(trimmed) || 
        trimmed.find("[") != String::npos) {  // 可能包含索引
        return DeleteOperationType::ELEMENT;
    }
    
    // 默认为属性删除
    return DeleteOperationType::PROPERTY;
}

StringVector DeleteNode::parseDeleteTargets(const String& targetsString) {
    StringVector targets;
    std::istringstream iss(targetsString);
    String token;
    
    while (std::getline(iss, token, ',')) {
        // 移除首尾空白
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        
        if (!token.empty()) {
            targets.push_back(token);
        }
    }
    
    return targets;
}

void DeleteNode::updateOperationType() {
    if (targets.empty()) return;
    
    // 根据第一个目标确定操作类型
    operationType = determineOperationType(targets[0]);
}

String DeleteNode::operationTypeToString() const {
    switch (operationType) {
        case DeleteOperationType::PROPERTY: return "Properties";
        case DeleteOperationType::INHERITANCE: return "Inheritance";
        case DeleteOperationType::ELEMENT: return "Element";
        case DeleteOperationType::TEMPLATE: return "Template";
        case DeleteOperationType::CUSTOM: return "Custom";
        default: return "Unknown";
    }
}

// ========== InsertNode 实现 ==========

InsertNode::InsertNode(InsertPosition pos, const Position& nodePos)
    : BaseNode(NodeType::INSERT, "", nodePos), position(pos), 
      targetIndex(SIZE_MAX), isIndexed(false) {
}

void InsertNode::setTarget(const String& target) {
    targetSelector = target;
    parseTargetSelector(target);
}

void InsertNode::setIndexedTarget(const String& elementName, size_t index) {
    this->elementName = elementName;
    this->targetIndex = index;
    this->isIndexed = true;
    this->targetSelector = elementName + "[" + std::to_string(index) + "]";
}

void InsertNode::addContent(std::unique_ptr<BaseNode> content) {
    if (content) {
        insertedContent.push_back(std::move(content));
    }
}

void InsertNode::addContentFromString(const String& content) {
    // 创建一个文本节点来保存内容
    auto textNode = std::make_unique<BaseNode>(NodeType::TEXT, content);
    addContent(std::move(textNode));
}

String InsertNode::getTargetDescription() const {
    if (isIndexed) {
        return elementName + "[" + std::to_string(targetIndex) + "]";
    }
    return targetSelector;
}

String InsertNode::getPositionDescription() const {
    return positionToString();
}

String InsertNode::toString() const {
    std::ostringstream oss;
    oss << "insert " << positionToString();
    
    if (!targetSelector.empty()) {
        oss << " " << targetSelector;
    }
    
    oss << " { " << insertedContent.size() << " items }";
    return oss.str();
}

String InsertNode::toHTML() const {
    return "<!-- " + toString() + " -->";
}

std::unique_ptr<InsertNode> InsertNode::createAfterInsertion(const String& target, const Position& pos) {
    auto node = std::make_unique<InsertNode>(InsertPosition::AFTER, pos);
    node->setTarget(target);
    return node;
}

std::unique_ptr<InsertNode> InsertNode::createBeforeInsertion(const String& target, const Position& pos) {
    auto node = std::make_unique<InsertNode>(InsertPosition::BEFORE, pos);
    node->setTarget(target);
    return node;
}

std::unique_ptr<InsertNode> InsertNode::createReplaceInsertion(const String& target, const Position& pos) {
    auto node = std::make_unique<InsertNode>(InsertPosition::REPLACE, pos);
    node->setTarget(target);
    return node;
}

std::unique_ptr<InsertNode> InsertNode::createTopInsertion(const Position& pos) {
    return std::make_unique<InsertNode>(InsertPosition::AT_TOP, pos);
}

std::unique_ptr<InsertNode> InsertNode::createBottomInsertion(const Position& pos) {
    return std::make_unique<InsertNode>(InsertPosition::AT_BOTTOM, pos);
}

InsertPosition InsertNode::parseInsertPosition(const String& positionString) {
    String trimmed = positionString;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
    
    if (trimmed == "after") return InsertPosition::AFTER;
    if (trimmed == "before") return InsertPosition::BEFORE;
    if (trimmed == "replace") return InsertPosition::REPLACE;
    if (trimmed == "at top") return InsertPosition::AT_TOP;
    if (trimmed == "at bottom") return InsertPosition::AT_BOTTOM;
    
    return InsertPosition::AFTER; // 默认
}

String InsertNode::parseTargetSelector(const String& selectorString) {
    // 解析类似 "div[0]" 的选择器
    std::regex indexRegex(R"((\w+)\[(\d+)\])");
    std::smatch match;
    
    if (std::regex_match(selectorString, match, indexRegex)) {
        elementName = match[1].str();
        targetIndex = std::stoul(match[2].str());
        isIndexed = true;
        return elementName;
    }
    
    // 简单选择器
    elementName = selectorString;
    isIndexed = false;
    return selectorString;
}

String InsertNode::positionToString() const {
    switch (position) {
        case InsertPosition::AFTER: return "after";
        case InsertPosition::BEFORE: return "before";
        case InsertPosition::REPLACE: return "replace";
        case InsertPosition::AT_TOP: return "at top";
        case InsertPosition::AT_BOTTOM: return "at bottom";
        default: return "unknown";
    }
}

void InsertNode::parseTargetSelector(const String& selector) {
    parseTargetSelector(selector);
}

// ========== IndexAccessNode 实现 ==========

IndexAccessNode::IndexAccessNode(const String& element, size_t idx, const Position& pos)
    : BaseNode(NodeType::INDEX_ACCESS, element, pos), elementName(element), index(idx) {
}

void IndexAccessNode::setContent(std::unique_ptr<BaseNode> nodeContent) {
    content = std::move(nodeContent);
}

String IndexAccessNode::getAccessDescription() const {
    return elementName + "[" + std::to_string(index) + "]";
}

String IndexAccessNode::toString() const {
    std::ostringstream oss;
    oss << getAccessDescription();
    if (hasContent()) {
        oss << " { content }";
    }
    return oss.str();
}

String IndexAccessNode::toHTML() const {
    return "<!-- Index Access: " + toString() + " -->";
}

std::unique_ptr<IndexAccessNode> IndexAccessNode::createIndexAccess(const String& elementName, size_t index, const Position& pos) {
    return std::make_unique<IndexAccessNode>(elementName, index, pos);
}

std::pair<String, size_t> IndexAccessNode::parseIndexAccess(const String& accessString) {
    std::regex indexRegex(R"((\w+)\[(\d+)\])");
    std::smatch match;
    
    if (std::regex_match(accessString, match, indexRegex)) {
        String elementName = match[1].str();
        size_t index = std::stoul(match[2].str());
        return std::make_pair(elementName, index);
    }
    
    return std::make_pair("", SIZE_MAX);
}

bool IndexAccessNode::isIndexAccessSyntax(const String& input) {
    std::regex indexRegex(R"(\w+\[\d+\])");
    return std::regex_match(input, indexRegex);
}

// ========== NoValueStyleNode 实现 ==========

NoValueStyleNode::NoValueStyleNode(const Position& pos)
    : BaseNode(NodeType::LITERAL, "", pos), isOptional(true) {
}

void NoValueStyleNode::addProperty(const String& property) {
    if (std::find(properties.begin(), properties.end(), property) == properties.end()) {
        properties.push_back(property);
    }
}

void NoValueStyleNode::addProperties(const StringVector& propertyList) {
    for (const auto& property : propertyList) {
        addProperty(property);
    }
}

void NoValueStyleNode::removeProperty(const String& property) {
    properties.erase(
        std::remove(properties.begin(), properties.end(), property),
        properties.end()
    );
}

bool NoValueStyleNode::hasProperty(const String& property) const {
    return std::find(properties.begin(), properties.end(), property) != properties.end();
}

String NoValueStyleNode::toString() const {
    std::ostringstream oss;
    for (size_t i = 0; i < properties.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << properties[i];
    }
    oss << ";";
    return oss.str();
}

String NoValueStyleNode::toHTML() const {
    return "<!-- No Value Style: " + toString() + " -->";
}

String NoValueStyleNode::toCSSDeclaration() const {
    std::ostringstream oss;
    for (size_t i = 0; i < properties.size(); ++i) {
        if (i > 0) oss << "\n";
        oss << "  " << properties[i] << ": /* value required */;";
    }
    return oss.str();
}

std::unique_ptr<NoValueStyleNode> NoValueStyleNode::createNoValueStyle(const StringVector& properties, const Position& pos) {
    auto node = std::make_unique<NoValueStyleNode>(pos);
    node->addProperties(properties);
    return node;
}

StringVector NoValueStyleNode::parseNoValueProperties(const String& propertiesString) {
    StringVector properties;
    std::istringstream iss(propertiesString);
    String token;
    
    while (std::getline(iss, token, ',')) {
        // 移除首尾空白和分号
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n;") + 1);
        
        if (!token.empty()) {
            properties.push_back(token);
        }
    }
    
    return properties;
}

bool NoValueStyleNode::isNoValueStyleSyntax(const String& input) {
    // 检查是否以逗号分隔的属性列表结尾有分号
    // 例如: "color, font-size;"
    return input.find(',') != String::npos && 
           input.back() == ';' && 
           input.find(':') == String::npos;  // 不包含冒号，说明没有值
}

bool NoValueStyleNode::isValidCSSProperty(const String& property) const {
    return CSSPropertyMap::isCSSProperty(property);
}

// ========== SpecializationManager 实现 ==========

bool SpecializationManager::validateDeleteOperation(const DeleteNode* deleteNode, const BaseNode* context) {
    if (!deleteNode || !context) return false;
    
    switch (deleteNode->operationType) {
        case DeleteOperationType::PROPERTY:
            // 验证CSS属性的有效性
            for (const auto& prop : deleteNode->targets) {
                if (!CSSPropertyMap::isCSSProperty(prop)) {
                    return false; // 无效的CSS属性
                }
            }
            return true;
            
        case DeleteOperationType::ELEMENT:
            // 验证元素名称和索引
            if (deleteNode->targets.empty()) return false;
            
            if (deleteNode->isIndexed) {
                // 验证索引是否有效
                return deleteNode->elementIndex != SIZE_MAX;
            }
            
            // 验证元素名称
            return HTMLElementMap::isHTMLElement(deleteNode->targets[0]);
            
        case DeleteOperationType::INHERITANCE:
            // 验证继承目标格式
            return !deleteNode->inheritanceTarget.empty() &&
                   (deleteNode->inheritanceTarget.find("@Style") != String::npos ||
                    deleteNode->inheritanceTarget.find("@Element") != String::npos ||
                    deleteNode->inheritanceTarget.find("@Var") != String::npos);
            
        default:
            return true; // 其他类型暂时认为有效
    }
}

bool SpecializationManager::validateInsertOperation(const InsertNode* insertNode, const BaseNode* context) {
    if (!insertNode || !context) return false;
    
    // 验证插入位置
    if (insertNode->position != InsertPosition::AT_TOP && 
        insertNode->position != InsertPosition::AT_BOTTOM) {
        // 需要目标选择器
        if (insertNode->targetSelector.empty()) {
            return false;
        }
        
        // 验证索引访问
        if (insertNode->isIndexed) {
            return insertNode->targetIndex != SIZE_MAX;
        }
    }
    
    // 验证插入内容
    return insertNode->getContentCount() > 0;
}

bool SpecializationManager::validateIndexAccess(const IndexAccessNode* indexNode, const BaseNode* context) {
    if (!indexNode || !context) return false;
    
    // 验证元素名称
    if (!HTMLElementMap::isHTMLElement(indexNode->elementName)) {
        return false;
    }
    
    // 验证索引
    return indexNode->index != SIZE_MAX;
}

bool SpecializationManager::validateNoValueStyle(const NoValueStyleNode* styleNode) {
    if (!styleNode) return false;
    
    // 验证至少有一个属性
    if (styleNode->isEmpty()) return false;
    
    // 验证所有属性都是有效的CSS属性
    for (const auto& prop : styleNode->properties) {
        if (!CSSPropertyMap::isCSSProperty(prop)) {
            return false;
        }
    }
    
    return true;
}

bool SpecializationManager::applyDeleteOperation(DeleteNode* deleteNode, BaseNode* target) {
    // 实际应用删除操作的逻辑
    // 这里只是框架，具体实现需要根据目标节点类型来处理
    return validateDeleteOperation(deleteNode, target);
}

bool SpecializationManager::applyInsertOperation(InsertNode* insertNode, BaseNode* target) {
    // 实际应用插入操作的逻辑
    return validateInsertOperation(insertNode, target);
}

bool SpecializationManager::applyIndexAccess(IndexAccessNode* indexNode, BaseNode* target) {
    // 实际应用索引访问的逻辑
    return validateIndexAccess(indexNode, target);
}

String SpecializationManager::generateValidationError(const BaseNode* node, const String& reason) {
    std::ostringstream oss;
    oss << "特例化操作验证失败 at line " << node->position.line 
        << ", column " << node->position.column << ": " << reason;
    return oss.str();
}

bool SpecializationManager::hasConflict(const BaseNode* node1, const BaseNode* node2) {
    // 检查两个特例化操作是否冲突
    if (!node1 || !node2) return false;
    
    // 简化的冲突检测：相同位置的相同操作类型
    return node1->type == node2->type && 
           node1->position.line == node2->position.line &&
           node1->position.column == node2->position.column;
}

StringVector SpecializationManager::detectConflicts(const std::vector<BaseNode*>& specializationNodes) {
    StringVector conflicts;
    
    for (size_t i = 0; i < specializationNodes.size(); ++i) {
        for (size_t j = i + 1; j < specializationNodes.size(); ++j) {
            if (hasConflict(specializationNodes[i], specializationNodes[j])) {
                conflicts.push_back(
                    "冲突检测: 节点 " + std::to_string(i) + 
                    " 与节点 " + std::to_string(j) + " 存在冲突"
                );
            }
        }
    }
    
    return conflicts;
}

} // namespace CHTL