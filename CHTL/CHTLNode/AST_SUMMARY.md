# CHTL AST节点体系

## 概述

根据CHTL语法文档，设计了完整的AST（抽象语法树）节点体系，涵盖了CHTL和CHTL JS的所有语法特性。

## AST节点分类

### 1. 基础节点 (BasicNodes.h)

- **ProgramNode**: AST根节点，表示整个CHTL程序
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

### 5. CHTL JS节点 (CHTLJSNodes.h)

- **EnhancedSelectorNode**: 增强选择器（{{}}语法）
  - 支持 {{.class}}, {{#id}}, {{tag}}, {{tag[index]}}
- **ListenExpressionNode**: 监听器表达式（listen语法）
- **DelegateExpressionNode**: 事件委托（delegate语法）
- **AnimateExpressionNode**: 动画表达式（animate语法）
- **VirtualObjectNode**: 虚对象声明（vir语法）
- **VirtualObjectAccessNode**: 虚对象访问（->操作符）
- **JSStatementNode**: 通用JS语句
- **FunctionCallNode**: 函数调用
- **SelectorExpressionNode**: 选择器表达式（&引用）

## 设计特点

### 1. 访问者模式

所有节点都实现了访问者模式，便于遍历和转换：

```cpp
class ASTNode {
    virtual void accept(ASTVisitor* visitor) = 0;
};

class CHTLASTVisitor {
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
std::weak_ptr<ASTNode> parent;
std::vector<std::shared_ptr<ASTNode>> children;
```

### 4. 元数据支持

每个节点可以存储额外的元数据：

```cpp
std::unordered_map<std::string, std::string> metadata;
```

### 5. 克隆支持

所有节点都支持深度克隆：

```cpp
virtual std::shared_ptr<ASTNode> clone() const = 0;
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

// 访问AST
MyVisitor visitor;
div->accept(&visitor);
```

## 完整性说明

该AST节点体系完整覆盖了CHTL语法文档中的所有语法特性：

✅ 注释系统（三种注释类型）
✅ 文本和字面量
✅ HTML元素和属性
✅ 局部样式和脚本块
✅ 模板和自定义（包括继承、删除、插入）
✅ 原始嵌入
✅ 配置系统
✅ 导入系统
✅ 命名空间
✅ 约束（except）
✅ CHTL JS所有特性（增强选择器、listen、delegate、animate、vir）

这个AST设计为后续的语法分析、语义分析和代码生成提供了坚实的基础。