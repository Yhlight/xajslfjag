#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace CHTLJS {

// CHTL JS Token类型枚举 - 完全独立的编程语言
enum class CHTLJSTokenType {
    // === 基础字面量 ===
    STRING_LITERAL,              // "字符串"
    TEMPLATE_LITERAL,            // `模板字符串`
    NUMBER,                      // 123, 3.14
    BIGINT,                      // 123n
    BOOLEAN,                     // true, false
    NULL_LITERAL,                // null
    UNDEFINED_LITERAL,           // undefined
    IDENTIFIER,                  // 标识符
    
    // === JavaScript基础关键字 ===
    CONST, LET, VAR,             // 变量声明
    FUNCTION, ASYNC, AWAIT,      // 函数相关
    CLASS, EXTENDS, SUPER,       // 类相关
    IF, ELSE, SWITCH, CASE, DEFAULT, // 条件语句
    FOR, WHILE, DO, BREAK, CONTINUE, // 循环语句
    TRY, CATCH, FINALLY, THROW,  // 异常处理
    RETURN, YIELD,               // 返回语句
    IMPORT, EXPORT, FROM, AS,    // 模块系统
    NEW, DELETE, TYPEOF, INSTANCEOF, // 操作符关键字
    IN, OF,                      // 迭代关键字
    THIS,                        // this关键字
    
    // === CHTL JS增强选择器关键字 ===
    SELECTOR, SELECT,            // 选择器声明
    QUERY, QUERYALL,             // 查询方法
    CLOSEST, PARENT, CHILD,      // 遍历方法
    SIBLINGS, NEXT, PREV,        // 兄弟节点
    FIND, FILTER, HAS,           // 过滤方法
    FIRST, LAST, INDEX,          // 位置方法
    VISIBLE, HIDDEN,             // 可见性
    ENABLED, DISABLED,           // 状态选择器
    CHECKED, SELECTED,           // 表单状态
    EMPTY, CONTAINS,             // 内容选择器
    
    // === CHTL JS监听器增强关键字 ===
    LISTENER, LISTEN, ON,        // 监听器声明
    EVENT, TRIGGER,              // 事件相关
    CLICK, DBLCLICK, MOUSEDOWN, MOUSEUP, // 鼠标事件
    MOUSEOVER, MOUSEOUT, MOUSEMOVE,      // 鼠标移动
    KEYDOWN, KEYUP, KEYPRESS,            // 键盘事件
    FOCUS, BLUR, CHANGE, INPUT,          // 表单事件
    SUBMIT, RESET,                       // 表单提交
    LOAD, UNLOAD, RESIZE, SCROLL,        // 窗口事件
    DRAG, DROP, DRAGOVER,                // 拖拽事件
    TOUCH, TOUCHSTART, TOUCHEND,         // 触摸事件
    
    // === CHTL JS事件委托关键字 ===
    DELEGATE, DELEGATED,         // 事件委托
    BUBBLE, CAPTURE,             // 事件阶段
    PREVENT, STOP,               // 事件控制
    ONCE, PASSIVE,               // 事件选项
    
    // === CHTL JS动画关键字 ===
    ANIMATE, ANIMATION,          // 动画声明
    TRANSITION, TRANSFORM,       // 过渡和变换
    KEYFRAMES, FRAME,            // 关键帧
    DURATION, DELAY,             // 时间属性
    EASING, TIMING,              // 时间函数
    INFINITE, ALTERNATE,         // 循环模式
    FORWARDS, BACKWARDS,         // 填充模式
    PLAY, PAUSE, STOP_ANIM,      // 动画控制
    FADE, SLIDE, SCALE,          // 预设动画
    ROTATE, TRANSLATE, SKEW,     // 变换类型
    
    // === CHTL JS虚拟对象关键字 ===
    VIRTUAL, VNODE,              // 虚拟对象
    COMPONENT, MOUNT, UNMOUNT,   // 组件生命周期
    RENDER, UPDATE,              // 渲染更新
    STATE, PROPS, COMPUTED,      // 状态管理
    WATCH, REACTIVE,             // 响应式
    REF, REFS,                   // 引用
    EMIT, DISPATCH,              // 事件发射
    LIFECYCLE, HOOK,             // 生命周期钩子
    CREATED, MOUNTED, UPDATED, DESTROYED, // 生命周期状态
    
    // === CHTL JS增强操作符 ===
    BIND_OP,                     // ::绑定操作符
    PIPE_OP,                     // |>管道操作符
    CHAIN_OP,                    // ?.链式操作符
    NULL_COALESCE,               // ??空值合并
    RANGE_OP,                    // ..范围操作符
    SPREAD_OP,                   // ...扩展操作符
    
    // === JavaScript基础操作符 ===
    // 算术操作符
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO, EXPONENT,
    // 赋值操作符
    ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MULTIPLY_ASSIGN, DIVIDE_ASSIGN, MODULO_ASSIGN,
    // 比较操作符
    EQUAL, NOT_EQUAL, STRICT_EQUAL, STRICT_NOT_EQUAL,
    LESS_THAN, GREATER_THAN, LESS_EQUAL, GREATER_EQUAL,
    // 逻辑操作符
    LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT,
    // 位操作符
    BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT,
    LEFT_SHIFT, RIGHT_SHIFT, UNSIGNED_RIGHT_SHIFT,
    // 递增递减
    INCREMENT, DECREMENT,
    
    // === 结构符号 ===
    LBRACE, RBRACE,              // { }
    LPAREN, RPAREN,              // ( )
    LBRACKET, RBRACKET,          // [ ]
    SEMICOLON, COLON,            // ; :
    COMMA, DOT,                  // , .
    QUESTION, EXCLAMATION,       // ? !
    ARROW,                       // =>
    
    // === CHTL JS特殊符号 ===
    AT_SYMBOL,                   // @装饰器
    HASH,                        // #私有字段
    DOLLAR,                      // $选择器前缀
    PERCENT,                     // %特殊操作
    
    // === 字符串模板相关 ===
    TEMPLATE_START,              // `开始
    TEMPLATE_MIDDLE,             // 模板中间部分
    TEMPLATE_END,                // `结束
    TEMPLATE_EXPR_START,         // ${
    TEMPLATE_EXPR_END,           // }
    
    // === 正则表达式 ===
    REGEX_LITERAL,               // /pattern/flags
    
    // === 注释 ===
    LINE_COMMENT,                // //注释
    BLOCK_COMMENT,               // /*注释*/
    JSDOC_COMMENT,               // /**JSDoc注释*/
    
    // === 特殊Token ===
    NEWLINE,                     // 换行符
    WHITESPACE,                  // 空白符
    EOF_TOKEN,                   // 文件结束
    INVALID                      // 无效Token
};

// CHTL JS Token结构
struct CHTLJSToken {
    CHTLJSTokenType type;        // Token类型
    std::string value;           // Token值
    size_t line;                 // 行号
    size_t column;               // 列号
    size_t position;             // 在源码中的位置
    size_t length;               // Token长度
    
    // 额外属性
    bool isKeyword = false;      // 是否为关键字
    bool isOperator = false;     // 是否为操作符
    bool isLiteral = false;      // 是否为字面量
    bool isCHTLJSSpecific = false; // 是否为CHTL JS特有
    
    CHTLJSToken(CHTLJSTokenType t = CHTLJSTokenType::INVALID, 
                const std::string& v = "", 
                size_t l = 1, size_t c = 1, size_t p = 0, size_t len = 0)
        : type(t), value(v), line(l), column(c), position(p), length(len) {
        updateProperties();
    }
    
    std::string getTypeName() const;
    std::string getDescription() const;
    bool isValid() const { return type != CHTLJSTokenType::INVALID; }
    bool isEndOfFile() const { return type == CHTLJSTokenType::EOF_TOKEN; }
    bool isWhitespace() const { return type == CHTLJSTokenType::WHITESPACE || type == CHTLJSTokenType::NEWLINE; }
    bool isComment() const;
    bool isSelectorKeyword() const;
    bool isListenerKeyword() const;
    bool isAnimationKeyword() const;
    bool isVirtualObjectKeyword() const;
    bool isEventType() const;
    bool isLifecycleKeyword() const;
    
private:
    void updateProperties();
};

// CHTL JS Token工具类
class CHTLJSTokenUtils {
public:
    // Token类型转换
    static CHTLJSTokenType stringToTokenType(const std::string& str);
    static std::string tokenTypeToString(CHTLJSTokenType type);
    
    // 关键字检查
    static bool isKeyword(const std::string& str);
    static bool isJavaScriptKeyword(const std::string& str);
    static bool isCHTLJSKeyword(const std::string& str);
    static bool isSelectorKeyword(const std::string& str);
    static bool isListenerKeyword(const std::string& str);
    static bool isAnimationKeyword(const std::string& str);
    static bool isVirtualObjectKeyword(const std::string& str);
    static bool isEventType(const std::string& str);
    
    // 操作符检查
    static bool isOperator(const std::string& str);
    static bool isAssignmentOperator(CHTLJSTokenType type);
    static bool isComparisonOperator(CHTLJSTokenType type);
    static bool isArithmeticOperator(CHTLJSTokenType type);
    static bool isLogicalOperator(CHTLJSTokenType type);
    static bool isBitwiseOperator(CHTLJSTokenType type);
    
    // 字面量检查
    static bool isLiteral(CHTLJSTokenType type);
    static bool isNumericLiteral(CHTLJSTokenType type);
    static bool isStringLiteral(CHTLJSTokenType type);
    
    // 获取关键字列表
    static std::vector<std::string> getAllKeywords();
    static std::vector<std::string> getJavaScriptKeywords();
    static std::vector<std::string> getCHTLJSKeywords();
    static std::vector<std::string> getSelectorKeywords();
    static std::vector<std::string> getListenerKeywords();
    static std::vector<std::string> getAnimationKeywords();
    static std::vector<std::string> getVirtualObjectKeywords();
    static std::vector<std::string> getEventTypes();
    
    // 操作符优先级
    static int getOperatorPrecedence(CHTLJSTokenType type);
    static bool isRightAssociative(CHTLJSTokenType type);
    
    // 验证方法
    static bool isValidIdentifier(const std::string& identifier);
    static bool isValidNumber(const std::string& number);
    static bool isValidString(const std::string& str);
    static bool isValidRegex(const std::string& regex);
    
    // CHTL JS特有特性检查
    static bool isCHTLJSSpecificToken(CHTLJSTokenType type);
    static bool requiresRuntimeSupport(CHTLJSTokenType type);
    static std::string getRuntimeFeature(CHTLJSTokenType type);
    
    // 上下文相关检查
    static bool canStartExpression(CHTLJSTokenType type);
    static bool canEndExpression(CHTLJSTokenType type);
    static bool isStatementKeyword(CHTLJSTokenType type);
    static bool isDeclarationKeyword(CHTLJSTokenType type);
    
private:
    // 静态映射表
    static const std::unordered_map<std::string, CHTLJSTokenType>& getKeywordMap();
    static const std::unordered_map<std::string, CHTLJSTokenType>& getOperatorMap();
    static const std::unordered_map<CHTLJSTokenType, std::string>& getTokenNameMap();
    static const std::unordered_map<CHTLJSTokenType, int>& getPrecedenceMap();
    
    // 分类集合
    static const std::unordered_set<CHTLJSTokenType>& getJavaScriptKeywordSet();
    static const std::unordered_set<CHTLJSTokenType>& getCHTLJSKeywordSet();
    static const std::unordered_set<CHTLJSTokenType>& getSelectorKeywordSet();
    static const std::unordered_set<CHTLJSTokenType>& getListenerKeywordSet();
    static const std::unordered_set<CHTLJSTokenType>& getAnimationKeywordSet();
    static const std::unordered_set<CHTLJSTokenType>& getVirtualObjectKeywordSet();
    static const std::unordered_set<CHTLJSTokenType>& getEventTypeSet();
    static const std::unordered_set<CHTLJSTokenType>& getLiteralSet();
    static const std::unordered_set<CHTLJSTokenType>& getOperatorSet();
    static const std::unordered_set<CHTLJSTokenType>& getCHTLJSSpecificSet();
};

// CHTL JS关键字分类枚举
enum class CHTLJSKeywordCategory {
    JAVASCRIPT_CORE,             // JavaScript核心关键字
    SELECTOR_ENHANCED,           // 选择器增强
    LISTENER_ENHANCED,           // 监听器增强
    EVENT_DELEGATION,            // 事件委托
    ANIMATION_SYSTEM,            // 动画系统
    VIRTUAL_OBJECT,              // 虚拟对象
    LIFECYCLE_HOOK,              // 生命周期钩子
    ENHANCED_OPERATOR,           // 增强操作符
    SPECIAL_SYNTAX               // 特殊语法
};

// CHTL JS特性信息结构
struct CHTLJSFeatureInfo {
    std::string name;            // 特性名称
    std::string description;     // 特性描述
    CHTLJSKeywordCategory category; // 特性分类
    std::vector<std::string> keywords; // 相关关键字
    bool requiresRuntime;        // 是否需要运行时支持
    std::string runtimeFeature;  // 运行时特性名称
    std::string documentation;   // 文档链接
};

// CHTL JS特性管理器
class CHTLJSFeatureManager {
public:
    static CHTLJSFeatureInfo getFeatureInfo(CHTLJSTokenType type);
    static std::vector<CHTLJSFeatureInfo> getAllFeatures();
    static std::vector<CHTLJSFeatureInfo> getFeaturesByCategory(CHTLJSKeywordCategory category);
    static bool isFeatureEnabled(const std::string& featureName);
    static std::vector<std::string> getRequiredRuntimeFeatures(const std::vector<CHTLJSTokenType>& tokens);
};

} // namespace CHTLJS