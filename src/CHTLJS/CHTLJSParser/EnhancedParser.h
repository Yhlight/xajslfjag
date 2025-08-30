#pragma once

#include "../../Util/Common.h"
#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSLexer/Lexer.h"
#include "../../CHTL/CHTLNode/BaseNode.h"

using namespace CHTL;

namespace CHTLJS {

// CHTL JS增强语法节点类型
enum class CHTLJSNodeType {
    MODULE_IMPORT,          // 模块导入
    ENHANCED_SELECTOR,      // 增强选择器 {{selector}}
    LISTEN_BLOCK,           // listen块
    DELEGATE_BLOCK,         // delegate块
    ANIMATE_BLOCK,          // animate块
    VIRTUAL_OBJECT,         // vir虚对象
    EVENT_BINDING,          // &-> 事件绑定
    ARROW_OPERATOR,         // -> 操作符
    CHTLJS_FUNCTION        // CHTL JS函数
};

// CHTL JS节点
class CHTLJSNode : public CHTL::BaseNode {
private:
    CHTLJSNodeType chtljsType;
    StringUnorderedMap jsProperties;
    StringVector jsEvents;
    String jsTarget;
    
public:
    CHTLJSNode(CHTLJSNodeType type, const String& value, const CHTL::Position& pos = CHTL::Position())
        : BaseNode(CHTL::NodeType::SCRIPT, value, pos), chtljsType(type) {}
    
    // CHTL JS特有方法
    CHTLJSNodeType getCHTLJSType() const { return chtljsType; }
    void setCHTLJSType(CHTLJSNodeType type) { chtljsType = type; }
    
    // JS属性管理
    void setJSProperty(const String& key, const String& value) { jsProperties[key] = value; }
    String getJSProperty(const String& key) const {
        auto it = jsProperties.find(key);
        return it != jsProperties.end() ? it->second : "";
    }
    StringUnorderedMap getJSProperties() const { return jsProperties; }
    
    // JS事件管理
    void addJSEvent(const String& event) { jsEvents.push_back(event); }
    StringVector getJSEvents() const { return jsEvents; }
    
    // JS目标
    void setJSTarget(const String& target) { jsTarget = target; }
    String getJSTarget() const { return jsTarget; }
    
    // 重写基类方法
    std::unique_ptr<CHTL::BaseNode> clone() const override;
    String toString() const override;
};

// CHTL JS解析器配置
struct CHTLJSParserConfig {
    bool enableModuleImport = true;         // 启用模块导入
    bool enableEnhancedSelectors = true;    // 启用增强选择器
    bool enableAdvancedSyntax = true;       // 启用高级语法
    bool enableVirtualObjects = true;       // 启用虚对象
    bool enableEventDelegation = true;      // 启用事件委托
    bool enableAnimationSystem = true;      // 启用动画系统
    bool enableUnquotedLiterals = true;     // 启用无引号字面量
    bool enableErrorRecovery = true;        // 启用错误恢复
    bool strictMode = false;                // 严格模式
};

// CHTL JS增强解析器
class CHTLJSEnhancedParser {
private:
    std::unique_ptr<CHTL::JS::CHTLJSLexer> lexer;
    CHTLJSParserConfig config;
    CHTL::JS::CHTLJSToken currentToken;
    std::vector<String> errors;
    
    // 状态管理
    int currentPosition = 0;
    bool hasErrors = false;
    
    // 解析方法
    std::unique_ptr<CHTLJSNode> parseModuleBlock();
    std::unique_ptr<CHTLJSNode> parseEnhancedSelector();
    std::unique_ptr<CHTLJSNode> parseListenBlock();
    std::unique_ptr<CHTLJSNode> parseDelegateBlock();
    std::unique_ptr<CHTLJSNode> parseAnimateBlock();
    std::unique_ptr<CHTLJSNode> parseVirtualObject();
    std::unique_ptr<CHTLJSNode> parseEventBinding();
    std::unique_ptr<CHTLJSNode> parseCHTLJSFunction();
    
    // 辅助解析方法
    StringUnorderedMap parseKeyValuePairs();
    StringVector parseEventList();
    String parseSelector();
    String parseJavaScriptExpression();
    String parseJavaScriptBlock();
    
    // token管理
    void advance();
    bool check(CHTL::JS::CHTLJSTokenType type) const;
    CHTL::JS::CHTLJSToken consume(CHTL::JS::CHTLJSTokenType type, const String& message = "");
    bool isAtEnd() const;
    
    // 错误处理
    void reportError(const String& message, const String& code = "");
    bool synchronize();
    
public:
    // 构造函数
    explicit CHTLJSEnhancedParser(std::unique_ptr<CHTL::JS::CHTLJSLexer> lex, const CHTLJSParserConfig& cfg = CHTLJSParserConfig());
    
    // 主解析方法
    std::unique_ptr<CHTLJSNode> parse();
    std::unique_ptr<CHTLJSNode> parseStatement();
    std::unique_ptr<CHTLJSNode> parseExpression();
    
    // 特定语法解析
    std::unique_ptr<CHTLJSNode> parseModuleImport();
    std::unique_ptr<CHTLJSNode> parseAdvancedSyntax();
    
    // 错误获取
    std::vector<String> getErrors() const { return errors; }
    bool hasParseErrors() const { return hasErrors; }
    
    // 配置管理
    void setConfig(const CHTLJSParserConfig& cfg) { config = cfg; }
    CHTLJSParserConfig getConfig() const { return config; }
};

// CHTL JS语法检测器
class CHTLJSSyntaxDetector {
public:
    // 检测方法
    static bool isModuleImport(const String& code);
    static bool isEnhancedSelector(const String& code);
    static bool isListenBlock(const String& code);
    static bool isDelegateBlock(const String& code);
    static bool isAnimateBlock(const String& code);
    static bool isVirtualObject(const String& code);
    static bool isEventBinding(const String& code);
    static bool isCHTLJSFunction(const String& code);
    
    // 综合检测
    static bool hasCHTLJSSyntax(const String& code);
    static CHTLJSNodeType detectSyntaxType(const String& code);
    static StringVector findCHTLJSBlocks(const String& code);
};

// CHTL JS代码生成器
class CHTLJSCodeGenerator {
private:
    StringUnorderedMap selectorMap;        // 选择器映射
    StringVector moduleImports;            // 模块导入列表
    StringUnorderedMap virtualObjects;     // 虚对象映射
    bool enableOptimization = true;        // 启用优化
    
public:
    // 生成方法
    String generateFromNode(CHTLJSNode* node);
    String generateModuleImport(CHTLJSNode* node);
    String generateEnhancedSelector(CHTLJSNode* node);
    String generateListenBlock(CHTLJSNode* node);
    String generateDelegateBlock(CHTLJSNode* node);
    String generateAnimateBlock(CHTLJSNode* node);
    String generateVirtualObject(CHTLJSNode* node);
    String generateEventBinding(CHTLJSNode* node);
    
    // 优化方法
    String optimizeGeneratedCode(const String& code);
    void collectSelectors(CHTLJSNode* node);
    void optimizeSelectors();
    
    // 配置
    void setEnableOptimization(bool enable) { enableOptimization = enable; }
    
    // 统计信息
    String getGenerationStats() const;
};

// CHTL JS工具函数
namespace CHTLJSUtils {
    // 选择器处理
    String convertSelector(const String& chtljsSelector);
    String extractSelectorFromBraces(const String& bracedSelector);
    bool isValidSelector(const String& selector);
    
    // 事件处理
    StringVector parseEventHandlers(const String& eventBlock);
    String generateEventListener(const String& event, const String& handler);
    
    // 模块处理
    StringVector parseModuleDependencies(const String& moduleBlock);
    String generateModuleLoader(const StringVector& dependencies);
    
    // 动画处理
    StringUnorderedMap parseAnimationProperties(const String& animationBlock);
    String generateAnimationCode(const StringUnorderedMap& properties);
    
    // 虚对象处理
    String generateVirtualObjectCode(const String& name, const StringUnorderedMap& properties);
    StringUnorderedMap parseVirtualObjectProperties(const String& virBlock);
    
    // 代码优化
    String minifyCHTLJSCode(const String& code);
    String beautifyCHTLJSCode(const String& code);
}

} // namespace CHTLJS