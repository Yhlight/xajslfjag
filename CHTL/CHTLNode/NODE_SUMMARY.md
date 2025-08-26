# CHTL节点体系

## 概述

根据CHTL语法文档，设计了完整的CHTL核心语言节点体系。当前专注于CHTL核心功能，暂不包含CHTL JS扩展。

## 节点分类

### 1. 基础节点 (BasicNodes.h)

- **ProgramNode**: 根节点，表示整个CHTL程序
- **CommentNode**: 注释节点（支持 //, /* */, --）
- **TextNode**: 文本节点（text { } 块）
- **LiteralNode**: 字面量节点（无引号、双引号、单引号）
- **IdentifierNode**: 标识符节点
- **BlockNode**: 块节点（{ } 结构）
- **StatementListNode**: 语句列表节点

### 2. HTML元素节点 (ElementNodes.h)

- **ElementNode**: HTML元素节点
  - 支持属性、局部样式块、局部脚本块
  - 自动类名/ID管理
- **AttributeNode**: 属性节点（支持 : 和 = CE对等式）
- **StyleBlockNode**: 样式块节点（全局/局部）
- **CSSRuleNode**: CSS规则节点
- **ScriptBlockNode**: 脚本块节点（全局/局部）
- **PropertyNode**: 属性节点（用于样式和配置）

### 3. 模板和自定义节点 (TemplateNodes.h)

- **TemplateDeclarationNode**: 模板声明（[Template] @Style/@Element/@Var）
- **CustomDeclarationNode**: 自定义声明（[Custom] @Style/@Element/@Var）
- **TemplateUsageNode**: 模板使用
- **CustomUsageNode**: 自定义使用（支持特例化）
- **VariableReferenceNode**: 变量引用（如 ThemeColor(tableColor)）
- **InheritStatementNode**: 继承语句（inherit关键字）
- **DeleteStatementNode**: 删除语句（delete属性/元素/继承）
- **InsertStatementNode**: 插入语句（insert after/before/replace等）
- **IndexAccessNode**: 索引访问（如 div[1]）

### 4. 系统级节点 (SystemNodes.h)

- **OriginEmbedNode**: 原始嵌入（[Origin] @Html/@Style/@JavaScript）
- **ConfigurationNode**: 配置节点（[Configuration]）
- **ConfigOptionNode**: 配置选项
- **NameGroupNode**: Name组配置
- **OriginTypeNode**: OriginType组配置
- **ImportStatementNode**: 导入语句（[Import]）
- **UseStatementNode**: use语句
- **NamespaceDeclarationNode**: 命名空间声明（[Namespace]）
- **ExceptConstraintNode**: except约束
- **FromClauseNode**: from子句（命名空间引用）

## 设计特点

### 1. 访问者模式

所有节点都实现了访问者模式，便于遍历和转换：

```cpp
class Node {
    virtual void accept(Visitor* visitor) = 0;
};

class CHTLVisitor {
    virtual void visitElement(ElementNode* node) = 0;
    // ... 其他visit方法
};
```

### 2. 位置信息

每个节点都保存了在源代码中的位置信息：

```cpp
size_t line;
size_t column;
size_t endLine;
size_t endColumn;
Token startToken;
Token endToken;
```

### 3. 父子关系

支持完整的树形结构：

```cpp
std::weak_ptr<Node> parent;
std::vector<std::shared_ptr<Node>> children;
```

### 4. 元数据支持

每个节点可以存储额外的元数据：

```cpp
std::unordered_map<std::string, std::string> metadata;
```

### 5. 克隆支持

所有节点都支持深度克隆：

```cpp
virtual std::shared_ptr<Node> clone() const = 0;
```

## 使用示例

```cpp
// 创建一个元素节点
auto div = std::make_shared<ElementNode>("div", token);

// 添加属性
auto classAttr = std::make_shared<AttributeNode>("class", token);
classAttr->setValue(std::make_shared<LiteralNode>(
    LiteralNode::UNQUOTED, "container", token
));
div->addAttribute(classAttr);

// 添加局部样式块
auto styleBlock = std::make_shared<StyleBlockNode>(false, token);
div->setLocalStyleBlock(styleBlock);

// 访问节点树
MyCHTLVisitor visitor;
div->accept(&visitor);
```

## CHTL核心功能覆盖

该节点体系完整覆盖了CHTL核心语法特性：

✅ 注释系统（三种注释类型）
✅ 文本和字面量
✅ HTML元素和属性
✅ 局部样式块（含选择器自动化）
✅ 模板系统（[Template]）
✅ 自定义系统（[Custom]）
✅ 继承、删除、插入等特例化操作
✅ 原始嵌入（[Origin]）
✅ 配置系统（[Configuration]）
✅ 导入系统（[Import]）
✅ 命名空间（[Namespace]）
✅ 约束系统（except）

## 下一步

1. 实现节点的具体方法（.cpp文件）
2. 开发CHTL Parser，将Token流转换为节点树
3. 实现语义分析器，基于节点树进行语义检查
4. 开发代码生成器，将节点树转换为HTML/CSS/JS

这个节点设计为CHTL编译器的核心功能提供了坚实的基础。