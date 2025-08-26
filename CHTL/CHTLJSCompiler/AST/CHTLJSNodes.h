#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTLJS {

// CHTL JS AST节点类型
enum class CHTLJSNodeType {
    // === 程序结构 ===
    PROGRAM,                     // 程序根节点
    MODULE,                      // 模块
    STATEMENT_LIST,              // 语句列表
    BLOCK_STATEMENT,             // 块语句
    
    // === 声明 ===
    VARIABLE_DECLARATION,        // 变量声明
    FUNCTION_DECLARATION,        // 函数声明
    CLASS_DECLARATION,           // 类声明
    IMPORT_DECLARATION,          // 导入声明
    EXPORT_DECLARATION,          // 导出声明
    
    // === JavaScript基础语句 ===
    EXPRESSION_STATEMENT,        // 表达式语句
    IF_STATEMENT,                // if语句
    WHILE_STATEMENT,             // while语句
    FOR_STATEMENT,               // for语句
    FOR_IN_STATEMENT,            // for...in语句
    FOR_OF_STATEMENT,            // for...of语句
    DO_WHILE_STATEMENT,          // do...while语句
    SWITCH_STATEMENT,            // switch语句
    CASE_CLAUSE,                 // case子句
    DEFAULT_CLAUSE,              // default子句
    BREAK_STATEMENT,             // break语句
    CONTINUE_STATEMENT,          // continue语句
    RETURN_STATEMENT,            // return语句
    THROW_STATEMENT,             // throw语句
    TRY_STATEMENT,               // try语句
    CATCH_CLAUSE,                // catch子句
    FINALLY_CLAUSE,              // finally子句
    
    // === JavaScript基础表达式 ===
    IDENTIFIER,                  // 标识符
    LITERAL,                     // 字面量
    ARRAY_EXPRESSION,            // 数组表达式
    OBJECT_EXPRESSION,           // 对象表达式
    PROPERTY,                    // 属性
    FUNCTION_EXPRESSION,         // 函数表达式
    ARROW_FUNCTION_EXPRESSION,   // 箭头函数表达式
    CLASS_EXPRESSION,            // 类表达式
    MEMBER_EXPRESSION,           // 成员表达式
    CALL_EXPRESSION,             // 调用表达式
    NEW_EXPRESSION,              // new表达式
    UPDATE_EXPRESSION,           // 更新表达式(++/--)
    UNARY_EXPRESSION,            // 一元表达式
    BINARY_EXPRESSION,           // 二元表达式
    ASSIGNMENT_EXPRESSION,       // 赋值表达式
    LOGICAL_EXPRESSION,          // 逻辑表达式
    CONDITIONAL_EXPRESSION,      // 条件表达式
    SEQUENCE_EXPRESSION,         // 序列表达式
    THIS_EXPRESSION,             // this表达式
    
    // === CHTL JS增强选择器表达式 ===
    SELECTOR_EXPRESSION,         // 选择器表达式
    QUERY_EXPRESSION,            // 查询表达式
    TRAVERSAL_EXPRESSION,        // 遍历表达式
    FILTER_EXPRESSION,           // 过滤表达式
    POSITION_EXPRESSION,         // 位置表达式
    STATE_EXPRESSION,            // 状态表达式
    CONTENT_EXPRESSION,          // 内容表达式
    
    // === CHTL JS监听器表达式 ===
    LISTENER_EXPRESSION,         // 监听器表达式
    EVENT_HANDLER,               // 事件处理器
    EVENT_DELEGATION,            // 事件委托
    EVENT_OPTIONS,               // 事件选项
    
    // === CHTL JS动画表达式 ===
    ANIMATION_EXPRESSION,        // 动画表达式
    KEYFRAMES_EXPRESSION,        // 关键帧表达式
    TRANSITION_EXPRESSION,       // 过渡表达式
    TRANSFORM_EXPRESSION,        // 变换表达式
    TIMING_EXPRESSION,           // 时间表达式
    
    // === CHTL JS虚拟对象表达式 ===
    VIRTUAL_OBJECT_EXPRESSION,   // 虚拟对象表达式
    COMPONENT_EXPRESSION,        // 组件表达式
    LIFECYCLE_EXPRESSION,        // 生命周期表达式
    STATE_MANAGEMENT,            // 状态管理
    REACTIVE_EXPRESSION,         // 响应式表达式
    
    // === CHTL JS增强操作符 ===
    BIND_EXPRESSION,             // 绑定表达式 ::
    PIPE_EXPRESSION,             // 管道表达式 |>
    CHAIN_EXPRESSION,            // 链式表达式 ?.
    NULL_COALESCE_EXPRESSION,    // 空值合并表达式 ??
    RANGE_EXPRESSION,            // 范围表达式 ..
    
    // === 模板和字符串 ===
    TEMPLATE_LITERAL,            // 模板字面量
    TEMPLATE_ELEMENT,            // 模板元素
    TAGGED_TEMPLATE_EXPRESSION,  // 标记模板表达式
    
    // === 模式和解构 ===
    ARRAY_PATTERN,               // 数组模式
    OBJECT_PATTERN,              // 对象模式
    ASSIGNMENT_PATTERN,          // 赋值模式
    REST_ELEMENT,                // 剩余元素
    SPREAD_ELEMENT,              // 扩展元素
    
    // === 异步编程 ===
    ASYNC_FUNCTION_DECLARATION,  // 异步函数声明
    ASYNC_FUNCTION_EXPRESSION,   // 异步函数表达式
    AWAIT_EXPRESSION,            // await表达式
    YIELD_EXPRESSION,            // yield表达式
    
    // === 类相关 ===
    METHOD_DEFINITION,           // 方法定义
    PROPERTY_DEFINITION,         // 属性定义
    STATIC_BLOCK,                // 静态块
    PRIVATE_IDENTIFIER,          // 私有标识符
    
    // === 模块相关 ===
    IMPORT_SPECIFIER,            // 导入说明符
    IMPORT_DEFAULT_SPECIFIER,    // 默认导入说明符
    IMPORT_NAMESPACE_SPECIFIER,  // 命名空间导入说明符
    EXPORT_SPECIFIER,            // 导出说明符
    EXPORT_DEFAULT_DECLARATION,  // 默认导出声明
    EXPORT_NAMED_DECLARATION,    // 命名导出声明
    EXPORT_ALL_DECLARATION,      // 全部导出声明
    
    // === 注释 ===
    LINE_COMMENT,                // 行注释
    BLOCK_COMMENT,               // 块注释
    JSDOC_COMMENT,               // JSDoc注释
    
    // === 调试和元信息 ===
    DEBUG_INFO,                  // 调试信息
    SOURCE_LOCATION,             // 源码位置
    META_PROPERTY                // 元属性
};

// CHTL JS AST基础节点
class CHTLJSNode : public std::enable_shared_from_this<CHTLJSNode> {
public:
    CHTLJSNodeType type;                 // 节点类型
    std::string value;                   // 节点值
    std::string name;                    // 节点名称
    
    // 位置信息
    size_t line = 0;                     // 行号
    size_t column = 0;                   // 列号
    size_t position = 0;                 // 源码位置
    size_t length = 0;                   // 节点长度
    
    // 树结构
    std::vector<std::shared_ptr<CHTLJSNode>> children; // 子节点
    std::weak_ptr<CHTLJSNode> parent;    // 父节点
    
    // 属性
    std::unordered_map<std::string, std::string> attributes; // 属性
    
    // CHTL JS特有属性
    bool isAsync = false;                // 是否异步
    bool isGenerator = false;            // 是否生成器
    bool isStatic = false;               // 是否静态
    bool isPrivate = false;              // 是否私有
    bool isCHTLJSSpecific = false;       // 是否CHTL JS特有
    
    CHTLJSNode(CHTLJSNodeType nodeType, const std::string& nodeName = "", const std::string& nodeValue = "");
    virtual ~CHTLJSNode() = default;
    
    // 树操作
    void addChild(std::shared_ptr<CHTLJSNode> child);
    void removeChild(std::shared_ptr<CHTLJSNode> child);
    void insertChild(size_t index, std::shared_ptr<CHTLJSNode> child);
    std::shared_ptr<CHTLJSNode> getChild(size_t index) const;
    size_t getChildCount() const { return children.size(); }
    
    // 查找
    std::shared_ptr<CHTLJSNode> findChild(const std::string& name, CHTLJSNodeType type = CHTLJSNodeType::IDENTIFIER);
    std::vector<std::shared_ptr<CHTLJSNode>> findChildren(CHTLJSNodeType type);
    std::shared_ptr<CHTLJSNode> findParent(CHTLJSNodeType type);
    
    // 属性操作
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    
    // 位置设置
    void setPosition(size_t l, size_t c, size_t p, size_t len = 0);
    void setPositionFromToken(const struct CHTLJSToken& token);
    
    // 遍历
    void traverse(std::function<void(std::shared_ptr<CHTLJSNode>)> visitor);
    void traversePreOrder(std::function<bool(std::shared_ptr<CHTLJSNode>)> visitor);
    void traversePostOrder(std::function<void(std::shared_ptr<CHTLJSNode>)> visitor);
    
    // 类型检查
    bool isExpression() const;
    bool isStatement() const;
    bool isDeclaration() const;
    bool isLiteral() const;
    bool isIdentifier() const;
    bool isComment() const;
    bool isCHTLJSEnhanced() const;
    bool requiresRuntime() const;
    
    // 输出
    std::string getTypeName() const;
    std::string toString(int indent = 0) const;
    std::string toJSON() const;
    
    // 克隆
    virtual std::shared_ptr<CHTLJSNode> clone() const;
    
    // 验证
    virtual bool validate() const;
    
protected:
    virtual std::string getNodeSpecificInfo() const;
};

// CHTL JS程序节点
class CHTLJSProgramNode : public CHTLJSNode {
public:
    std::string sourceType;              // "script" 或 "module"
    std::vector<std::string> imports;    // 导入列表
    std::vector<std::string> exports;    // 导出列表
    
    CHTLJSProgramNode(const std::string& type = "script");
    
    // 程序级别操作
    void addImport(const std::string& importPath);
    void addExport(const std::string& exportName);
    std::vector<std::shared_ptr<CHTLJSNode>> getDeclarations();
    std::vector<std::shared_ptr<CHTLJSNode>> getStatements();
    
    std::shared_ptr<CHTLJSNode> clone() const override;
    bool validate() const override;
    
protected:
    std::string getNodeSpecificInfo() const override;
};

// CHTL JS函数节点
class CHTLJSFunctionNode : public CHTLJSNode {
public:
    std::string functionName;            // 函数名
    std::vector<std::string> parameters; // 参数列表
    std::shared_ptr<CHTLJSNode> body;    // 函数体
    bool isArrowFunction = false;        // 是否箭头函数
    
    CHTLJSFunctionNode(const std::string& name = "", bool arrow = false);
    
    // 函数操作
    void addParameter(const std::string& param);
    void setBody(std::shared_ptr<CHTLJSNode> bodyNode);
    std::vector<std::string> getParameterNames() const;
    
    std::shared_ptr<CHTLJSNode> clone() const override;
    bool validate() const override;
    
protected:
    std::string getNodeSpecificInfo() const override;
};

// CHTL JS类节点
class CHTLJSClassNode : public CHTLJSNode {
public:
    std::string className;               // 类名
    std::shared_ptr<CHTLJSNode> superClass; // 父类
    std::vector<std::shared_ptr<CHTLJSNode>> methods;    // 方法列表
    std::vector<std::shared_ptr<CHTLJSNode>> properties; // 属性列表
    
    CHTLJSClassNode(const std::string& name = "");
    
    // 类操作
    void setSuperClass(std::shared_ptr<CHTLJSNode> superClassNode);
    void addMethod(std::shared_ptr<CHTLJSNode> method);
    void addProperty(std::shared_ptr<CHTLJSNode> property);
    std::vector<std::shared_ptr<CHTLJSNode>> getConstructors();
    std::vector<std::shared_ptr<CHTLJSNode>> getStaticMethods();
    std::vector<std::shared_ptr<CHTLJSNode>> getInstanceMethods();
    
    std::shared_ptr<CHTLJSNode> clone() const override;
    bool validate() const override;
    
protected:
    std::string getNodeSpecificInfo() const override;
};

// === CHTL JS增强节点 ===

// CHTL JS选择器节点
class CHTLJSSelectorNode : public CHTLJSNode {
public:
    std::string selectorType;            // 选择器类型
    std::string selectorString;          // 选择器字符串
    std::vector<std::string> methods;    // 选择器方法
    
    CHTLJSSelectorNode(const std::string& type = "", const std::string& selector = "");
    
    // 选择器操作
    void addMethod(const std::string& method);
    bool isTraversalSelector() const;
    bool isFilterSelector() const;
    bool isStateSelector() const;
    
    std::shared_ptr<CHTLJSNode> clone() const override;
    bool validate() const override;
    
protected:
    std::string getNodeSpecificInfo() const override;
};

// CHTL JS监听器节点
class CHTLJSListenerNode : public CHTLJSNode {
public:
    std::string eventType;               // 事件类型
    std::shared_ptr<CHTLJSNode> handler; // 事件处理器
    std::vector<std::string> options;    // 事件选项
    bool isDelegated = false;            // 是否委托
    
    CHTLJSListenerNode(const std::string& event = "");
    
    // 监听器操作
    void setHandler(std::shared_ptr<CHTLJSNode> handlerNode);
    void addOption(const std::string& option);
    void setDelegated(bool delegated);
    bool isPassive() const;
    bool isOnce() const;
    bool isCapture() const;
    
    std::shared_ptr<CHTLJSNode> clone() const override;
    bool validate() const override;
    
protected:
    std::string getNodeSpecificInfo() const override;
};

// CHTL JS动画节点
class CHTLJSAnimationNode : public CHTLJSNode {
public:
    std::string animationType;           // 动画类型
    std::unordered_map<std::string, std::string> properties; // 动画属性
    std::vector<std::shared_ptr<CHTLJSNode>> keyframes; // 关键帧
    
    CHTLJSAnimationNode(const std::string& type = "");
    
    // 动画操作
    void setProperty(const std::string& key, const std::string& value);
    void addKeyframe(std::shared_ptr<CHTLJSNode> keyframe);
    std::string getDuration() const;
    std::string getEasing() const;
    bool isInfinite() const;
    
    std::shared_ptr<CHTLJSNode> clone() const override;
    bool validate() const override;
    
protected:
    std::string getNodeSpecificInfo() const override;
};

// CHTL JS虚拟对象节点
class CHTLJSVirtualObjectNode : public CHTLJSNode {
public:
    std::string objectType;              // 对象类型
    std::shared_ptr<CHTLJSNode> state;   // 状态
    std::shared_ptr<CHTLJSNode> props;   // 属性
    std::vector<std::shared_ptr<CHTLJSNode>> hooks; // 生命周期钩子
    
    CHTLJSVirtualObjectNode(const std::string& type = "");
    
    // 虚拟对象操作
    void setState(std::shared_ptr<CHTLJSNode> stateNode);
    void setProps(std::shared_ptr<CHTLJSNode> propsNode);
    void addHook(std::shared_ptr<CHTLJSNode> hook);
    std::vector<std::shared_ptr<CHTLJSNode>> getLifecycleHooks();
    bool isComponent() const;
    bool isReactive() const;
    
    std::shared_ptr<CHTLJSNode> clone() const override;
    bool validate() const override;
    
protected:
    std::string getNodeSpecificInfo() const override;
};

// CHTL JS字面量节点
class CHTLJSLiteralNode : public CHTLJSNode {
public:
    std::string literalType;             // 字面量类型
    std::string rawValue;                // 原始值
    
    CHTLJSLiteralNode(const std::string& type = "", const std::string& val = "", const std::string& raw = "");
    
    // 字面量操作
    bool isString() const;
    bool isNumber() const;
    bool isBoolean() const;
    bool isNull() const;
    bool isUndefined() const;
    bool isRegex() const;
    bool isBigInt() const;
    
    std::shared_ptr<CHTLJSNode> clone() const override;
    bool validate() const override;
    
protected:
    std::string getNodeSpecificInfo() const override;
};

// CHTL JS标识符节点
class CHTLJSIdentifierNode : public CHTLJSNode {
public:
    std::string identifierName;         // 标识符名称
    std::string scope;                   // 作用域
    
    CHTLJSIdentifierNode(const std::string& name = "");
    
    // 标识符操作
    void setScope(const std::string& scopeName);
    bool isReserved() const;
    bool isCHTLJSKeyword() const;
    
    std::shared_ptr<CHTLJSNode> clone() const override;
    bool validate() const override;
    
protected:
    std::string getNodeSpecificInfo() const override;
};

// === 工厂类 ===

// CHTL JS节点工厂
class CHTLJSNodeFactory {
public:
    // 基础节点创建
    static std::shared_ptr<CHTLJSNode> createNode(CHTLJSNodeType type, const std::string& name = "", const std::string& value = "");
    
    // 程序结构
    static std::shared_ptr<CHTLJSProgramNode> createProgram(const std::string& sourceType = "script");
    static std::shared_ptr<CHTLJSNode> createBlock();
    static std::shared_ptr<CHTLJSNode> createStatementList();
    
    // 声明
    static std::shared_ptr<CHTLJSNode> createVariableDeclaration(const std::string& kind = "let");
    static std::shared_ptr<CHTLJSFunctionNode> createFunctionDeclaration(const std::string& name, bool isAsync = false);
    static std::shared_ptr<CHTLJSClassNode> createClassDeclaration(const std::string& name);
    
    // 表达式
    static std::shared_ptr<CHTLJSIdentifierNode> createIdentifier(const std::string& name);
    static std::shared_ptr<CHTLJSLiteralNode> createLiteral(const std::string& type, const std::string& value, const std::string& raw = "");
    static std::shared_ptr<CHTLJSNode> createBinaryExpression(const std::string& operator_, std::shared_ptr<CHTLJSNode> left, std::shared_ptr<CHTLJSNode> right);
    static std::shared_ptr<CHTLJSNode> createCallExpression(std::shared_ptr<CHTLJSNode> callee);
    static std::shared_ptr<CHTLJSNode> createMemberExpression(std::shared_ptr<CHTLJSNode> object, std::shared_ptr<CHTLJSNode> property, bool computed = false);
    
    // CHTL JS增强节点
    static std::shared_ptr<CHTLJSSelectorNode> createSelector(const std::string& type, const std::string& selector);
    static std::shared_ptr<CHTLJSListenerNode> createListener(const std::string& eventType);
    static std::shared_ptr<CHTLJSAnimationNode> createAnimation(const std::string& type);
    static std::shared_ptr<CHTLJSVirtualObjectNode> createVirtualObject(const std::string& type);
    
    // 增强表达式
    static std::shared_ptr<CHTLJSNode> createBindExpression(std::shared_ptr<CHTLJSNode> left, std::shared_ptr<CHTLJSNode> right);
    static std::shared_ptr<CHTLJSNode> createPipeExpression(std::shared_ptr<CHTLJSNode> left, std::shared_ptr<CHTLJSNode> right);
    static std::shared_ptr<CHTLJSNode> createChainExpression(std::shared_ptr<CHTLJSNode> object, std::shared_ptr<CHTLJSNode> property);
    static std::shared_ptr<CHTLJSNode> createNullCoalesceExpression(std::shared_ptr<CHTLJSNode> left, std::shared_ptr<CHTLJSNode> right);
    static std::shared_ptr<CHTLJSNode> createRangeExpression(std::shared_ptr<CHTLJSNode> start, std::shared_ptr<CHTLJSNode> end);
    
    // 注释
    static std::shared_ptr<CHTLJSNode> createLineComment(const std::string& text);
    static std::shared_ptr<CHTLJSNode> createBlockComment(const std::string& text);
    static std::shared_ptr<CHTLJSNode> createJSDocComment(const std::string& text);
};

// CHTL JS AST访问器接口
class CHTLJSASTVisitor {
public:
    virtual ~CHTLJSASTVisitor() = default;
    
    // 访问方法
    virtual void visit(std::shared_ptr<CHTLJSNode> node) = 0;
    virtual void visitProgram(std::shared_ptr<CHTLJSProgramNode> program) {}
    virtual void visitFunction(std::shared_ptr<CHTLJSFunctionNode> function) {}
    virtual void visitClass(std::shared_ptr<CHTLJSClassNode> classNode) {}
    virtual void visitSelector(std::shared_ptr<CHTLJSSelectorNode> selector) {}
    virtual void visitListener(std::shared_ptr<CHTLJSListenerNode> listener) {}
    virtual void visitAnimation(std::shared_ptr<CHTLJSAnimationNode> animation) {}
    virtual void visitVirtualObject(std::shared_ptr<CHTLJSVirtualObjectNode> virtualObject) {}
    virtual void visitLiteral(std::shared_ptr<CHTLJSLiteralNode> literal) {}
    virtual void visitIdentifier(std::shared_ptr<CHTLJSIdentifierNode> identifier) {}
    
    // 访问控制
    virtual bool shouldVisitChildren(std::shared_ptr<CHTLJSNode> node) { return true; }
    virtual void beforeVisitChildren(std::shared_ptr<CHTLJSNode> node) {}
    virtual void afterVisitChildren(std::shared_ptr<CHTLJSNode> node) {}
};

// CHTL JS AST工具类
class CHTLJSASTUtils {
public:
    // AST分析
    static size_t getNodeCount(std::shared_ptr<CHTLJSNode> root);
    static size_t getDepth(std::shared_ptr<CHTLJSNode> root);
    static std::vector<std::shared_ptr<CHTLJSNode>> findNodesByType(std::shared_ptr<CHTLJSNode> root, CHTLJSNodeType type);
    static std::vector<std::shared_ptr<CHTLJSNode>> findCHTLJSSpecificNodes(std::shared_ptr<CHTLJSNode> root);
    
    // AST变换
    static std::shared_ptr<CHTLJSNode> cloneAST(std::shared_ptr<CHTLJSNode> root);
    static void removeNode(std::shared_ptr<CHTLJSNode> node);
    static void replaceNode(std::shared_ptr<CHTLJSNode> oldNode, std::shared_ptr<CHTLJSNode> newNode);
    
    // AST验证
    static bool validateAST(std::shared_ptr<CHTLJSNode> root);
    static std::vector<std::string> getValidationErrors(std::shared_ptr<CHTLJSNode> root);
    
    // 序列化
    static std::string astToJSON(std::shared_ptr<CHTLJSNode> root);
    static std::shared_ptr<CHTLJSNode> astFromJSON(const std::string& json);
    
    // 运行时需求分析
    static std::vector<std::string> getRequiredRuntimeFeatures(std::shared_ptr<CHTLJSNode> root);
    static bool requiresCHTLJSRuntime(std::shared_ptr<CHTLJSNode> root);
};

} // namespace CHTLJS