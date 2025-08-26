#pragma once
#include "../Lexer/CHTLLexer.h"
#include "../AST/CHTLNodes.h"
#include <vector>
#include <memory>
#include <stack>

namespace CHTL {

// CHTL解析状态 - 严格按照语法文档
enum class CHTLParsingState {
    // 默认状态
    GlobalScope,                          // 全局作用域
    
    // 通用状态
    InElementBody,                        // 元素内部
    InAttributeList,                      // 属性列表中
    InTextNodeBody,                       // 文本节点内部
    
    // 样式状态
    InStyleBlock,                         // 样式块中
    InStyleProperty,                      // 样式属性中
    InStyleSelector,                      // 样式选择器中
    
    // 脚本状态
    InScriptBlock,                        // 脚本块中
    
    // 定义状态
    InTemplateDefinition,                 // 模板定义中
    InCustomDefinition,                   // 自定义定义中
    InOriginDefinition,                   // 原始嵌入定义中
    InNamespaceDefinition,                // 命名空间定义中
    InConfigurationDefinition,            // 配置定义中
    InConfigurationNameGroup,             // [Name] 配置组中
    InConfigurationOriginTypeGroup,       // [OriginType] 配置组中
    
    // 特化和使用状态
    InStyleSpecialization,                // 样式特化中
    InElementSpecialization,              // 元素特化中
    InVarSpecialization,                  // 变量特化中
    InTemplateUsage,                      // 模板使用中
    InVariableUsage,                      // 变量使用中
    
    // 导入和使用状态
    InImportStatement,                    // 导入语句中
    InUseStatement,                       // use语句中
    
    // 约束状态
    InExceptClause                        // except子句中
};

// CHTL语法分析器
class CHTLParser {
private:
    std::vector<CHTLToken> tokens;        // Token列表
    size_t current;                       // 当前Token位置
    std::vector<std::string> errors;     // 错误信息
    std::stack<CHTLParsingState> stateStack; // 解析状态栈
    
    // 当前解析上下文
    std::string currentNamespace;        // 当前命名空间
    std::string currentTemplate;         // 当前模板
    std::string currentCustom;           // 当前自定义
    
public:
    // 构造函数
    explicit CHTLParser(const std::vector<CHTLToken>& tokenList);
    
    // 主解析函数
    std::shared_ptr<CHTLDocumentNode> parseDocument();
    
    // 错误处理
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
private:
    // === 核心解析方法 ===
    
    // Token操作
    CHTLToken peek(size_t offset = 0) const;
    CHTLToken advance();
    bool isAtEnd() const;
    bool match(CHTLTokenType type);
    bool check(CHTLTokenType type) const;
    CHTLToken consume(CHTLTokenType type, const std::string& message);
    
    // 状态管理
    void pushState(CHTLParsingState state);
    void popState();
    CHTLParsingState getCurrentState() const;
    bool isInState(CHTLParsingState state) const;
    
    // 错误处理
    void addError(const std::string& message);
    void synchronize();
    
    // === 按语法文档的解析方法 ===
    
    // 顶级声明解析
    std::shared_ptr<CHTLNode> parseDeclaration();
    std::shared_ptr<CHTLNode> parseUseDeclaration();
    std::shared_ptr<CHTLNode> parseNamespaceDeclaration();
    std::shared_ptr<CHTLNode> parseImportStatement();
    std::shared_ptr<CHTLNode> parseConfigurationDefinition();
    
    // 模板系统解析
    std::shared_ptr<CHTLTemplateNode> parseTemplateDefinition();
    std::shared_ptr<CHTLNode> parseTemplateUsage();
    std::shared_ptr<CHTLNode> parseTemplateInheritance();
    
    // 自定义系统解析
    std::shared_ptr<CHTLCustomNode> parseCustomDefinition();
    std::shared_ptr<CHTLNode> parseCustomUsage();
    std::shared_ptr<CHTLNode> parseCustomSpecialization();
    
    // 原始嵌入解析
    std::shared_ptr<CHTLNode> parseOriginDefinition();
    std::shared_ptr<CHTLNode> parseOriginUsage();
    
    // HTML元素解析
    std::shared_ptr<CHTLElementNode> parseElement();
    std::shared_ptr<CHTLNode> parseTextNode();
    std::shared_ptr<CHTLNode> parseAttribute();
    
    // 样式解析
    std::shared_ptr<CHTLStyleNode> parseStyleBlock();
    std::shared_ptr<CHTLNode> parseStyleRule();
    std::shared_ptr<CHTLNode> parseStyleProperty();
    std::shared_ptr<CHTLNode> parseSelector();
    
    // 脚本解析
    std::shared_ptr<CHTLScriptNode> parseScriptBlock();
    
    // 继承和特化解析
    std::shared_ptr<CHTLNode> parseInheritStatement();
    std::shared_ptr<CHTLNode> parseDeleteStatement();
    std::shared_ptr<CHTLNode> parseInsertStatement();
    
    // 约束解析
    std::shared_ptr<CHTLNode> parseExceptClause();
    std::shared_ptr<CHTLNode> parseConstraintItem();
    
    // 变量解析
    std::shared_ptr<CHTLNode> parseVariableDefinition();
    std::shared_ptr<CHTLNode> parseVariableUsage();
    std::shared_ptr<CHTLNode> parseVariableParameter();
    
    // 配置系统解析
    std::shared_ptr<CHTLNode> parseConfigurationBlock();
    std::shared_ptr<CHTLNode> parseNameConfigurationGroup();
    std::shared_ptr<CHTLNode> parseOriginTypeConfigurationGroup();
    std::shared_ptr<CHTLNode> parseConfigurationProperty();
    
    // 导入系统解析
    std::shared_ptr<CHTLImportNode> parseImportFromStatement();
    std::shared_ptr<CHTLNode> parseImportType();
    std::shared_ptr<CHTLNode> parseImportPath();
    std::shared_ptr<CHTLNode> parseAsClause();
    std::shared_ptr<CHTLNode> parseExceptClauseInImport();
    
    // 注释解析
    std::shared_ptr<CHTLNode> parseComment();
    
    // 字面量解析
    std::shared_ptr<CHTLNode> parseStringLiteral();
    std::shared_ptr<CHTLNode> parseNumberLiteral();
    std::shared_ptr<CHTLNode> parseUnquotedLiteral();
    std::shared_ptr<CHTLNode> parseIdentifier();
    
    // === 辅助方法 ===
    
    // 类型判断
    bool isDeclaration() const;
    bool isElement() const;
    bool isTemplateDefinition() const;
    bool isCustomDefinition() const;
    bool isOriginDefinition() const;
    bool isImportStatement() const;
    bool isUseStatement() const;
    bool isTemplateUsage() const;
    bool isCustomUsage() const;
    bool isStyleBlock() const;
    bool isScriptBlock() const;
    bool isComment() const;
    bool isConstraintKeyword() const;
    bool isInheritanceKeyword() const;
    
    // 前瞻检查
    bool isNextTokenType(CHTLTokenType type) const;
    bool isFollowedBy(CHTLTokenType type) const;
    
    // 语法验证
    bool validateTemplateType(const std::string& type) const;
    bool validateCustomType(const std::string& type) const;
    bool validateOriginType(const std::string& type) const;
    bool validateImportType(const std::string& type) const;
    bool validateUseType(const std::string& type) const;
    bool validateHTMLElement(const std::string& elementName) const;
    
    // 上下文管理
    void enterNamespace(const std::string& namespaceName);
    void exitNamespace();
    void enterTemplate(const std::string& templateName);
    void exitTemplate();
    void enterCustom(const std::string& customName);
    void exitCustom();
    
    // 路径处理
    std::string resolvePath(const std::string& path) const;
    std::string resolveNamespace(const std::string& name) const;
    
    // 特殊语法处理
    std::shared_ptr<CHTLNode> parseMultiWordKeyword(); // "at top", "at bottom"
    std::shared_ptr<CHTLNode> parseIndexAccess();      // div[0], span[1]
    std::shared_ptr<CHTLNode> parseCEEquivalent();     // : 和 = 等价处理
    
    // HTML标签验证
    static const std::vector<std::string>& getBuiltinHTMLTags();
    
    // CHTL关键字验证
    static const std::vector<std::string>& getCHTLKeywords();
};

} // namespace CHTL