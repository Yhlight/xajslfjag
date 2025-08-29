#pragma once

#include "BaseNode.h"
#include "../../Util/Common.h"

namespace CHTL {

// 删除操作类型
enum class DeleteOperationType {
    PROPERTY,       // 删除属性: delete line-height, border;
    INHERITANCE,    // 删除继承: delete @Style WhiteText;
    ELEMENT,        // 删除元素: delete span; delete div[1];
    TEMPLATE,       // 删除模板: delete [Template] @Element Line;
    CUSTOM         // 删除自定义: delete [Custom] @Style Box;
};

// 插入位置类型
enum class InsertPosition {
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    AT_TOP,         // at top
    AT_BOTTOM       // at bottom
};

// 删除操作节点
class DeleteNode : public BaseNode {
public:
    DeleteOperationType operationType;
    StringVector targets;           // 删除目标列表
    String inheritanceTarget;       // 继承目标 (如@Style WhiteText)
    size_t elementIndex;           // 元素索引 (用于删除元素)
    bool isIndexed;                // 是否使用索引访问
    
    explicit DeleteNode(const Position& pos = Position());
    ~DeleteNode() override = default;
    
    // 添加删除目标
    void addTarget(const String& target);
    void addTargets(const StringVector& targets);
    
    // 设置继承删除
    void setInheritanceTarget(const String& target);
    
    // 设置元素删除
    void setElementTarget(const String& elementName, size_t index = SIZE_MAX);
    
    // 检查删除类型
    bool isPropertyDeletion() const { return operationType == DeleteOperationType::PROPERTY; }
    bool isInheritanceDeletion() const { return operationType == DeleteOperationType::INHERITANCE; }
    bool isElementDeletion() const { return operationType == DeleteOperationType::ELEMENT; }
    
    // 获取删除描述
    String getDeletionDescription() const;
    
    // 序列化
    virtual String toString() const override;
    String toHTML() const;
    
    // 静态工厂方法
    static std::unique_ptr<DeleteNode> createPropertyDeletion(const StringVector& properties, const Position& pos = Position());
    static std::unique_ptr<DeleteNode> createInheritanceDeletion(const String& inheritanceTarget, const Position& pos = Position());
    static std::unique_ptr<DeleteNode> createElementDeletion(const String& elementName, size_t index = SIZE_MAX, const Position& pos = Position());
    
    // 解析删除目标
    static DeleteOperationType determineOperationType(const String& targetString);
    static StringVector parseDeleteTargets(const String& targetsString);
    
private:
    void updateOperationType();
    String operationTypeToString() const;
};

// 插入操作节点
class InsertNode : public BaseNode {
public:
    InsertPosition position;
    String targetSelector;          // 目标选择器 (如div[0])
    String elementName;             // 目标元素名
    size_t targetIndex;            // 目标索引
    bool isIndexed;                // 是否使用索引访问
    std::vector<std::unique_ptr<BaseNode>> insertedContent;  // 插入的内容
    
    explicit InsertNode(InsertPosition pos = InsertPosition::AFTER, const Position& nodePos = Position());
    ~InsertNode() override = default;
    
    // 设置插入位置和目标
    void setTarget(const String& target);
    void setIndexedTarget(const String& elementName, size_t index);
    void setPosition(InsertPosition pos) { position = pos; }
    
    // 添加插入内容
    void addContent(std::unique_ptr<BaseNode> content);
    void addContentFromString(const String& content);
    
    // 获取插入信息
    String getTargetDescription() const;
    String getPositionDescription() const;
    size_t getContentCount() const { return insertedContent.size(); }
    
    // 序列化
    virtual String toString() const override;
    String toHTML() const;
    
    // 静态工厂方法
    static std::unique_ptr<InsertNode> createAfterInsertion(const String& target, const Position& pos = Position());
    static std::unique_ptr<InsertNode> createBeforeInsertion(const String& target, const Position& pos = Position());
    static std::unique_ptr<InsertNode> createReplaceInsertion(const String& target, const Position& pos = Position());
    static std::unique_ptr<InsertNode> createTopInsertion(const Position& pos = Position());
    static std::unique_ptr<InsertNode> createBottomInsertion(const Position& pos = Position());
    
    // 解析插入语法
    static InsertPosition parseInsertPosition(const String& positionString);
    static String parseTargetSelector(const String& selectorString);
    
private:
    String positionToString() const;
    void parseTargetSelector(const String& selector);
};

// 索引访问节点
class IndexAccessNode : public BaseNode {
public:
    String elementName;             // 元素名 (div, span等)
    size_t index;                  // 索引值
    std::unique_ptr<BaseNode> content;  // 索引访问的内容
    
    explicit IndexAccessNode(const String& element, size_t idx, const Position& pos = Position());
    ~IndexAccessNode() override = default;
    
    // 设置内容
    void setContent(std::unique_ptr<BaseNode> content);
    
    // 获取访问信息
    String getAccessDescription() const;
    bool hasContent() const { return content != nullptr; }
    
    // 序列化
    virtual String toString() const override;
    String toHTML() const;
    
    // 静态工厂方法
    static std::unique_ptr<IndexAccessNode> createIndexAccess(const String& elementName, size_t index, const Position& pos = Position());
    
    // 解析索引访问语法 (div[1])
    static std::pair<String, size_t> parseIndexAccess(const String& accessString);
    static bool isIndexAccessSyntax(const String& input);
    
private:
    // 辅助方法
    bool isValidIndex() const { return index != SIZE_MAX; }
};

// 无值样式组节点
class NoValueStyleNode : public BaseNode {
public:
    StringVector properties;        // 无值属性列表
    bool isOptional;               // 是否为可选属性
    
    explicit NoValueStyleNode(const Position& pos = Position());
    ~NoValueStyleNode() override = default;
    
    // 属性管理
    void addProperty(const String& property);
    void addProperties(const StringVector& properties);
    void removeProperty(const String& property);
    bool hasProperty(const String& property) const;
    
    // 验证
    bool isEmpty() const { return properties.empty(); }
    size_t getPropertyCount() const { return properties.size(); }
    
    // 序列化
    virtual String toString() const override;
    String toHTML() const;
    String toCSSDeclaration() const;  // 生成CSS声明格式
    
    // 静态工厂方法
    static std::unique_ptr<NoValueStyleNode> createNoValueStyle(const StringVector& properties, const Position& pos = Position());
    
    // 解析无值样式组语法 (color, font-size;)
    static StringVector parseNoValueProperties(const String& propertiesString);
    static bool isNoValueStyleSyntax(const String& input);
    
private:
    // 辅助方法
    bool isValidCSSProperty(const String& property) const;
};

// 特例化操作管理器
class SpecializationManager {
public:
    // 验证删除操作
    static bool validateDeleteOperation(const DeleteNode* deleteNode, const BaseNode* context);
    
    // 验证插入操作
    static bool validateInsertOperation(const InsertNode* insertNode, const BaseNode* context);
    
    // 验证索引访问
    static bool validateIndexAccess(const IndexAccessNode* indexNode, const BaseNode* context);
    
    // 验证无值样式组
    static bool validateNoValueStyle(const NoValueStyleNode* styleNode);
    
    // 应用特例化操作
    static bool applyDeleteOperation(DeleteNode* deleteNode, BaseNode* target);
    static bool applyInsertOperation(InsertNode* insertNode, BaseNode* target);
    static bool applyIndexAccess(IndexAccessNode* indexNode, BaseNode* target);
    
    // 错误报告
    static String generateValidationError(const BaseNode* node, const String& reason);
    
    // 冲突检测
    static bool hasConflict(const BaseNode* node1, const BaseNode* node2);
    static StringVector detectConflicts(const std::vector<BaseNode*>& specializationNodes);
};

} // namespace CHTL