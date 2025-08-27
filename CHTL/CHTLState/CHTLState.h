#pragma once
#include "../CHTLLexer/Token.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL解析状态枚举
 */
enum class ParseState {
    INITIAL,                    // 初始状态
    PARSING_ELEMENT,           // 解析元素
    PARSING_ATTRIBUTES,        // 解析属性
    PARSING_TEXT_BLOCK,        // 解析文本块
    PARSING_STYLE_BLOCK,       // 解析样式块
    PARSING_SCRIPT_BLOCK,      // 解析脚本块
    PARSING_TEMPLATE,          // 解析模板
    PARSING_CUSTOM,            // 解析自定义
    PARSING_ORIGIN,            // 解析原始嵌入
    PARSING_IMPORT,            // 解析导入
    PARSING_CONFIG,            // 解析配置
    PARSING_NAMESPACE,         // 解析命名空间
    PARSING_USE,               // 解析use语句
    PARSING_EXCEPT,            // 解析约束
    PARSING_COMMENT,           // 解析注释
    ERROR_STATE,               // 错误状态
    END_STATE                  // 结束状态
};

/**
 * 语法约束类型
 */
enum class ConstraintType {
    ELEMENT_CONSTRAINT,        // 元素约束
    TEMPLATE_CONSTRAINT,       // 模板约束
    CUSTOM_CONSTRAINT,         // 自定义约束
    TYPE_CONSTRAINT            // 类型约束
};

/**
 * 约束规则
 */
struct ConstraintRule {
    ConstraintType type;
    std::string target;
    std::string context;
    bool isGlobal;
    
    ConstraintRule(ConstraintType t, const std::string& tgt, 
                  const std::string& ctx = "", bool global = false)
        : type(t), target(tgt), context(ctx), isGlobal(global) {}
};

/**
 * CHTL状态管理器
 * 使用RAII模式自动管理解析状态
 */
class CHTLState {
public:
    CHTLState();
    ~CHTLState() = default;
    
    // 状态管理
    void pushState(ParseState state);
    void popState();
    ParseState getCurrentState() const;
    ParseState getPreviousState() const;
    size_t getStateDepth() const;
    bool hasState() const;
    
    // 状态查询
    bool isInState(ParseState state) const;
    bool isInErrorState() const { return getCurrentState() == ParseState::ERROR_STATE; }
    bool isInitialState() const { return getCurrentState() == ParseState::INITIAL; }
    
    // 花括号深度管理
    void incrementBraceDepth() { ++braceDepth_; }
    void decrementBraceDepth() { if (braceDepth_ > 0) --braceDepth_; }
    size_t getBraceDepth() const { return braceDepth_; }
    void resetBraceDepth() { braceDepth_ = 0; }
    
    // 方括号深度管理
    void incrementBracketDepth() { ++bracketDepth_; }
    void decrementBracketDepth() { if (bracketDepth_ > 0) --bracketDepth_; }
    size_t getBracketDepth() const { return bracketDepth_; }
    void resetBracketDepth() { bracketDepth_ = 0; }
    
    // 圆括号深度管理
    void incrementParenDepth() { ++parenDepth_; }
    void decrementParenDepth() { if (parenDepth_ > 0) --parenDepth_; }
    size_t getParenDepth() const { return parenDepth_; }
    void resetParenDepth() { parenDepth_ = 0; }
    
    // 约束管理
    void addConstraint(const ConstraintRule& rule);
    void removeConstraint(const std::string& target);
    bool hasConstraint(const std::string& target) const;
    std::vector<ConstraintRule> getConstraints() const;
    void clearConstraints();
    
    // 特定约束检查
    bool canParseElement(const std::string& elementName) const;
    bool canParseTemplate(const std::string& templateType) const;
    bool canParseCustom(const std::string& customType) const;
    bool canParseOrigin() const;
    
    // 标志管理
    void setFlag(const std::string& flag, bool value = true);
    bool getFlag(const std::string& flag) const;
    void clearFlag(const std::string& flag);
    void clearAllFlags();
    
    // 常用标志
    void setInLocalStyleBlock(bool value = true) { setFlag("in_local_style", value); }
    void setInLocalScriptBlock(bool value = true) { setFlag("in_local_script", value); }
    void setInGlobalStyleBlock(bool value = true) { setFlag("in_global_style", value); }
    void setInTemplateBlock(bool value = true) { setFlag("in_template", value); }
    void setInCustomBlock(bool value = true) { setFlag("in_custom", value); }
    
    bool isInLocalStyleBlock() const { return getFlag("in_local_style"); }
    bool isInLocalScriptBlock() const { return getFlag("in_local_script"); }
    bool isInGlobalStyleBlock() const { return getFlag("in_global_style"); }
    bool isInTemplateBlock() const { return getFlag("in_template"); }
    bool isInCustomBlock() const { return getFlag("in_custom"); }
    
    // 自动化类名/ID管理
    void enableAutoClass(bool enable = true) { setFlag("auto_class", enable); }
    void enableAutoId(bool enable = true) { setFlag("auto_id", enable); }
    bool isAutoClassEnabled() const { return getFlag("auto_class"); }
    bool isAutoIdEnabled() const { return getFlag("auto_id"); }
    
    // 选择器收集
    void addClassSelector(const std::string& selector);
    void addIdSelector(const std::string& selector);
    std::vector<std::string> getClassSelectors() const;
    std::vector<std::string> getIdSelectors() const;
    void clearSelectors();
    
    // 当前处理的标识符
    void setCurrentIdentifier(const std::string& identifier) { currentIdentifier_ = identifier; }
    const std::string& getCurrentIdentifier() const { return currentIdentifier_; }
    
    // 当前处理的类型
    void setCurrentType(const std::string& type) { currentType_ = type; }
    const std::string& getCurrentType() const { return currentType_; }
    
    // 重置状态
    void reset();
    
    // 调试信息
    std::string getStateInfo() const;
    void dumpState() const;
    
private:
    std::vector<ParseState> stateStack_;
    std::vector<ConstraintRule> constraints_;
    std::unordered_map<std::string, bool> flags_;
    std::vector<std::string> classSelectors_;
    std::vector<std::string> idSelectors_;
    
    // 深度计数器
    size_t braceDepth_ = 0;
    size_t bracketDepth_ = 0;
    size_t parenDepth_ = 0;
    
    // 当前处理信息
    std::string currentIdentifier_;
    std::string currentType_;
};

/**
 * 解析状态到字符串转换
 */
std::string parseStateToString(ParseState state);

/**
 * RAII状态管理器
 * 自动管理状态的推入和弹出
 */
class StateGuard {
public:
    StateGuard(CHTLState& state, ParseState newState);
    ~StateGuard();
    
    // 禁止复制
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
private:
    CHTLState& state_;
};

/**
 * RAII深度管理器
 * 自动管理各种括号深度
 */
class DepthGuard {
public:
    enum class DepthType {
        BRACE,
        BRACKET,
        PAREN
    };
    
    DepthGuard(CHTLState& state, DepthType type);
    ~DepthGuard();
    
    // 禁止复制
    DepthGuard(const DepthGuard&) = delete;
    DepthGuard& operator=(const DepthGuard&) = delete;
    
private:
    CHTLState& state_;
    DepthType type_;
};

} // namespace CHTL