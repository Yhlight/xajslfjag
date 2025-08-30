#pragma once
#include "../CHTLLexer/Lexer.h"
#include "../CHTLLexer/Token.h"
#include "../CHTLState/State.h"
#include "../CHTLContext/Context.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/AdvancedTemplateNode.h"
#include "../CHTLNode/OperatorNode.h"
#include "../CHTLNode/ConfigNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/ConstraintNode.h"
#include "../CHTLIOStream/CHTLIOStream.h"
#include "../CHTLManage/TemplateManager.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stack>

namespace CHTL {

/**
 * 解析选项
 */
struct ParseOptions {
    bool enableComments = true;             // 是否解析注释
    bool enableTemplates = true;            // 是否解析模板
    bool enableCustom = true;               // 是否解析自定义
    bool enableImports = true;              // 是否解析导入
    bool enableConfig = true;               // 是否解析配置
    bool enableOrigins = true;              // 是否解析原始嵌入
    bool enableConstraints = true;          // 是否启用约束系统
    bool strictMode = false;                // 严格模式
    bool autoRecovery = true;               // 错误自动恢复
    size_t maxDepth = 1000;                // 最大嵌套深度
    size_t maxTokens = 1000000;            // 最大Token数量
};

/**
 * 解析结果
 */
struct ParseResult {
    NodePtr rootNode;                       // 根节点
    std::vector<NodePtr> templates;         // 模板节点
    std::vector<NodePtr> customNodes;       // 自定义节点
    std::vector<NodePtr> imports;           // 导入节点
    std::vector<NodePtr> configs;           // 配置节点
    std::vector<NodePtr> namespaces;        // 命名空间节点
    bool success;                          // 是否成功
    size_t tokensProcessed;                // 处理的Token数量
    size_t nodesCreated;                   // 创建的节点数量
    
    ParseResult() : success(false), tokensProcessed(0), nodesCreated(0) {}
};

/**
 * CHTL语法解析器
 * 将Token序列转换为AST
 */
class CHTLParser {
public:
    /**
     * 构造函数
     */
    explicit CHTLParser(const ParseOptions& options = ParseOptions());
    
    /**
     * 析构函数
     */
    ~CHTLParser() = default;
    
    // 禁用拷贝和移动
    CHTLParser(const CHTLParser&) = delete;
    CHTLParser& operator=(const CHTLParser&) = delete;
    
    // ========== 解析入口 ==========
    
    /**
     * 解析Token序列
     */
    ParseResult parse(const TokenSequence& tokens);
    
    /**
     * 解析文件
     */
    ParseResult parseFile(const std::string& filePath);
    
    /**
     * 解析字符串
     */
    ParseResult parseString(const std::string& source);
    
    /**
     * 增量解析（基于现有AST）
     */
    ParseResult parseIncremental(const TokenSequence& tokens, NodePtr existingRoot);
    
    // ========== 配置和状态 ==========
    
    /**
     * 设置解析选项
     */
    void setParseOptions(const ParseOptions& options);
    
    /**
     * 获取解析选项
     */
    const ParseOptions& getParseOptions() const;
    
    /**
     * 设置错误报告器
     */
    void setErrorReporter(ErrorReporter* errorReporter);
    
    /**
     * 获取解析统计信息
     */
    std::unordered_map<std::string, size_t> getParseStatistics() const;
    
    /**
     * 重置解析器状态
     */
    void reset();

private:
    ParseOptions m_options;                     // 解析选项
    ErrorReporter* m_errorReporter;             // 错误报告器
    
    // 解析状态
    std::unique_ptr<CHTLStateMachine> m_stateMachine;  // 状态机
    std::unique_ptr<CHTLContext> m_context;            // 上下文
    TokenSequence m_tokens;                     // 当前Token序列
    size_t m_currentIndex;                      // 当前Token索引
    size_t m_currentDepth;                      // 当前嵌套深度
    
    // 解析结果
    ParseResult m_result;                       // 解析结果
    std::stack<NodePtr> m_nodeStack;           // 节点栈
    std::unordered_map<std::string, size_t> m_statistics; // 统计信息
    
    // ========== 核心解析方法 ==========
    
    /**
     * 初始化解析
     */
    void initializeParsing(const TokenSequence& tokens);
    
    /**
     * 完成解析
     */
    void finalizeParsing();
    
    /**
     * 解析主循环
     */
    void parseMain();
    
    /**
     * 解析顶级元素
     */
    NodePtr parseTopLevel();
    
    /**
     * 解析元素节点
     */
    NodePtr parseElement();
    
    /**
     * 解析文本节点
     */
    NodePtr parseText();
    
    /**
     * 解析注释节点
     */
    NodePtr parseComment();
    
    /**
     * 解析局部样式块
     */
    NodePtr parseStyleBlock();
    
    /**
     * 解析局部脚本块
     */
    NodePtr parseScriptBlock();
    
    // ========== CHTL JS 解析 ==========
    
    /**
     * 解析CHTL JS语法
     */
    NodePtr parseCHTLJS();
    
    /**
     * 解析模块定义
     */
    NodePtr parseModule();
    
    /**
     * 解析监听器
     */
    NodePtr parseListener();
    
    /**
     * 解析动画
     */
    NodePtr parseAnimation();
    
    /**
     * 解析事件委托
     */
    NodePtr parseDelegate();
    
    /**
     * 解析虚拟对象
     */
    NodePtr parseVirtualObject();
    
    /**
     * 解析增强选择器
     */
    NodePtr parseEnhancedSelector();
    
    /**
     * 解析CHTL JS选择器
     */
    CHTLJSSelector parseCHTLJSSelector();
    
    /**
     * 解析事件绑定操作符
     */
    NodePtr parseEventBinding();
    
    /**
     * 解析属性
     */
    void parseAttributes(ElementNode* element);
    
    /**
     * 解析单个属性
     */
    bool parseAttribute(ElementNode* element);
    
    // ========== 模板和自定义解析 ==========
    
    /**
     * 解析模板定义
     */
    NodePtr parseTemplate();
    
    /**
     * 解析样式组模板
     */
    NodePtr parseStyleTemplate(const std::string& name, bool isCustom = false);
    
    /**
     * 解析元素模板
     */
    NodePtr parseElementTemplate(const std::string& name, bool isCustom = false);
    
    /**
     * 解析变量组模板
     */
    NodePtr parseVarTemplate(const std::string& name, bool isCustom = false);
    
    /**
     * 解析自定义定义
     */
    NodePtr parseCustom();
    
    // ========== 高级模板解析 ==========
    
    /**
     * 解析模板使用
     */
    NodePtr parseTemplateUsage();
    
    /**
     * 解析变量引用
     */
    NodePtr parseVariableReference();
    
    /**
     * 解析继承语句
     */
    NodePtr parseInheritStatement();
    
    /**
     * 解析特例化操作
     */
    std::vector<SpecializationInfo> parseSpecializations();
    
    /**
     * 解析delete操作
     */
    SpecializationInfo parseDeleteOperation();
    
    /**
     * 解析insert操作
     */
    SpecializationInfo parseInsertOperation();
    
    /**
     * 解析索引访问
     */
    SpecializationInfo parseIndexAccess();
    
    /**
     * 解析模板参数
     */
    std::unordered_map<std::string, std::string> parseTemplateParameters();
    
    /**
     * 解析复杂值（支持变量引用）
     */
    std::string parseComplexValue();
    
    /**
     * 解析删除元素操作
     */
    void parseDeleteElementOperation(AdvancedElementTemplateNode* elementTemplate);
    
    /**
     * 解析插入元素操作
     */
    void parseInsertElementOperation(AdvancedElementTemplateNode* elementTemplate);
    
    /**
     * 解析原始嵌入
     */
    NodePtr parseOrigin();
    
    /**
     * 解析原始嵌入使用
     */
    NodePtr parseOriginUsage();
    
    /**
     * 解析原始嵌入内容
     */
    std::string parseOriginContent();
    
    /**
     * 解析自定义样式组
     */
    NodePtr parseCustomStyle(const std::string& name);
    
    /**
     * 解析自定义元素
     */
    NodePtr parseCustomElement(const std::string& name);
    
    /**
     * 解析自定义变量组
     */
    NodePtr parseCustomVar(const std::string& name);
    
    // ========== 操作符解析 ==========
    
    /**
     * 解析use语句
     */
    NodePtr parseUse();
    
    /**
     * 解析inherit操作
     */
    NodePtr parseInherit();
    
    /**
     * 解析delete操作
     */
    NodePtr parseDelete();
    
    /**
     * 解析insert操作
     */
    NodePtr parseInsert();
    
    /**
     * 解析except约束
     */
    NodePtr parseExcept();
    
    // ========== 导入和配置解析 ==========
    
    /**
     * 解析导入语句
     */
    NodePtr parseImport();
    
    /**
     * 解析配置块
     */
    NodePtr parseConfiguration();
    
    /**
     * 解析命名空间
     */
    NodePtr parseNamespace();
    
    /**
     * 解析约束语句
     */
    NodePtr parseConstraint();
    
    /**
     * 解析约束项
     */
    ConstraintItem parseConstraintItem();
    
    /**
     * 解析[Name]配置
     */
    NodePtr parseNameConfig();
    
    /**
     * 解析[OriginType]配置
     */
    NodePtr parseOriginTypeConfig();
    
    /**
     * 解析配置项
     */
    void parseConfigurationItem(ConfigurationNode* configNode);
    
    /**
     * 解析名称配置项
     */
    void parseNameConfigItem(NameConfigNode* nameConfig);
    
    /**
     * 解析原始类型配置项
     */
    void parseOriginTypeConfigItem(OriginTypeConfigNode* originTypeConfig);
    
    /**
     * 解析配置值
     */
    ConfigValue parseConfigValue();
    
    // ========== 表达式解析 ==========
    
    /**
     * 解析@表达式
     */
    std::string parseAtExpression();
    
    /**
     * 解析选择器
     */
    std::string parseSelector();
    

    
    // ========== Token操作 ==========
    
    /**
     * 获取当前Token
     */
    const Token& currentToken() const;
    
    /**
     * 获取下一个Token
     */
    const Token& nextToken();
    
    /**
     * 预览Token
     */
    const Token& peekToken(size_t offset = 1) const;
    
    /**
     * 检查是否到达末尾
     */
    bool isAtEnd() const;
    
    /**
     * 前进到下一个Token
     */
    void advance();
    
    /**
     * 跳过指定类型的Token
     */
    void skipTokens(const std::vector<TokenType>& types);
    
    /**
     * 期望特定Token
     */
    bool expect(TokenType type);
    
    /**
     * 匹配特定Token
     */
    bool match(TokenType type);
    
    /**
     * 匹配多个Token类型之一
     */
    bool match(const std::vector<TokenType>& types);
    
    /**
     * 消费特定Token
     */
    bool consume(TokenType type, const std::string& errorMessage = "");
    
    // ========== 错误处理 ==========
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
    
    /**
     * 报告错误（指定位置）
     */
    void reportError(const std::string& message, const Position& position);
    
    /**
     * 报告警告
     */
    void reportWarning(const std::string& message);
    
    /**
     * 同步错误恢复
     */
    void synchronize();
    
    /**
     * 检查解析限制
     */
    bool checkLimits();
    
    // ========== 辅助方法 ==========
    
    /**
     * 创建节点
     */
    NodePtr createNode(CHTLNodeType type, const std::string& name = "");
    
    /**
     * 添加子节点
     */
    void addChild(NodePtr parent, NodePtr child);
    
    /**
     * 推入节点栈
     */
    void pushNode(NodePtr node);
    
    /**
     * 弹出节点栈
     */
    NodePtr popNode();
    
    /**
     * 获取栈顶节点
     */
    NodePtr topNode() const;
    
    /**
     * 更新统计信息
     */
    void updateStatistics(const std::string& key, size_t increment = 1);
    
    /**
     * 检查深度限制
     */
    bool checkDepthLimit();
    
    /**
     * 进入新的解析作用域
     */
    void enterScope(CHTLNodeType nodeType);
    
    /**
     * 退出解析作用域
     */
    void exitScope();
    
    /**
     * 解析块内容
     */
    void parseBlockContent(NodePtr parent);
    
    /**
     * 跳过到匹配的大括号
     */
    void skipToMatchingBrace();
    
    /**
     * 解析字面量值
     */
    std::string parseLiteralValue();
    
    /**
     * 解析字符串值
     */
    std::string parseStringValue();
    
    /**
     * 检查Token是否为关键字
     */
    bool isKeyword(const Token& token) const;
    
    /**
     * 检查Token是否为HTML元素
     */
    bool isHtmlElement(const Token& token) const;
    
    /**
     * 获取Token的字符串表示
     */
    std::string getTokenString(const Token& token) const;
};

/**
 * CHTL解析器工厂
 */
class CHTLParserFactory {
public:
    /**
     * 创建标准解析器
     */
    static std::unique_ptr<CHTLParser> createStandardParser();
    
    /**
     * 创建严格模式解析器
     */
    static std::unique_ptr<CHTLParser> createStrictParser();
    
    /**
     * 创建容错解析器
     */
    static std::unique_ptr<CHTLParser> createTolerantParser();
    
    /**
     * 创建自定义解析器
     */
    static std::unique_ptr<CHTLParser> createCustomParser(const ParseOptions& options);
};

} // namespace CHTL