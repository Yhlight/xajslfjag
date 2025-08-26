#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <string>
#include <memory>
#include <stack>
#include <unordered_set>

namespace CHTL {

// CHTL解析状态
enum class ParseState {
    // 顶层状态
    TOP_LEVEL,                  // 文件顶层
    
    // 声明状态
    IN_USE_DECLARATION,         // 在use声明中
    IN_NAMESPACE,               // 在命名空间中
    IN_CONFIGURATION,           // 在配置块中
    IN_TEMPLATE,                // 在模板定义中
    IN_CUSTOM,                  // 在自定义定义中
    IN_ORIGIN,                  // 在原始嵌入中
    IN_IMPORT,                  // 在导入声明中
    IN_INFO,                    // 在信息块中
    IN_EXPORT,                  // 在导出块中
    
    // 元素状态
    IN_ELEMENT,                 // 在元素内部
    IN_TEXT,                    // 在text块中
    IN_STYLE,                   // 在style块中（局部或全局）
    IN_SCRIPT,                  // 在script块中（局部或全局）
    
    // 样式相关状态
    IN_STYLE_RULE,              // 在样式规则中
    IN_STYLE_SELECTOR,          // 在样式选择器中
    IN_STYLE_PROPERTY,          // 在样式属性中
    
    // 特殊状态
    IN_ATTRIBUTE,               // 在属性定义中
    IN_STRING_LITERAL,          // 在字符串字面量中
    IN_COMMENT,                 // 在注释中
    IN_SPECIAL_BLOCK,           // 在特殊块中（如[Name], [OriginType]等）
    
    // 特例化状态
    IN_SPECIALIZATION,          // 在特例化块中
    IN_DELETE_STATEMENT,        // 在delete语句中
    IN_INSERT_STATEMENT,        // 在insert语句中
    IN_EXCEPT_STATEMENT         // 在except语句中
};

// 块类型
enum class BlockType {
    NONE,
    NAMESPACE_BLOCK,
    CONFIGURATION_BLOCK,
    TEMPLATE_BLOCK,
    CUSTOM_BLOCK,
    ORIGIN_BLOCK,
    ELEMENT_BLOCK,
    TEXT_BLOCK,
    STYLE_BLOCK,
    SCRIPT_BLOCK,
    INFO_BLOCK,
    EXPORT_BLOCK,
    NAME_BLOCK,         // [Configuration]中的[Name]块
    ORIGIN_TYPE_BLOCK,  // [Configuration]中的[OriginType]块
    SPECIALIZATION_BLOCK // 特例化块
};

// 状态上下文信息
struct StateContext {
    ParseState state;
    BlockType blockType;
    std::string blockName;       // 块的名称（如模板名、元素名等）
    std::string blockTypeStr;    // 块的类型字符串（如@Style, @Element等）
    size_t startLine;
    size_t startColumn;
    bool isGlobalStyle;          // 是否为全局样式块
    bool isGlobalScript;         // 是否为全局脚本块
    bool hasSpecialization;      // 是否有特例化
    
    StateContext(ParseState s = ParseState::TOP_LEVEL, BlockType bt = BlockType::NONE)
        : state(s), blockType(bt), startLine(0), startColumn(0),
          isGlobalStyle(false), isGlobalScript(false), hasSpecialization(false) {}
};

// RAII状态管理器
class StateGuard {
private:
    class CHTLState* stateManager;
    bool committed;
    
public:
    StateGuard(CHTLState* manager, ParseState newState, BlockType blockType = BlockType::NONE);
    ~StateGuard();
    
    void commit() { committed = true; }
    void setBlockInfo(const std::string& name, const std::string& type);
};

// CHTL状态机
class CHTLState {
private:
    std::stack<StateContext> stateStack;
    ParseState currentState;
    
    // 约束相关
    std::unordered_set<std::string> allowedInCurrentContext;
    std::unordered_set<std::string> exceptedInCurrentContext;
    
    // 验证方法
    bool isValidTransition(ParseState from, ParseState to) const;
    void updateConstraints();
    
public:
    CHTLState();
    ~CHTLState() = default;
    
    // 状态管理
    void pushState(ParseState newState, BlockType blockType = BlockType::NONE);
    void popState();
    ParseState getCurrentState() const { return currentState; }
    StateContext getCurrentContext() const;
    
    // RAII状态管理
    std::unique_ptr<StateGuard> createStateGuard(ParseState newState, BlockType blockType = BlockType::NONE);
    
    // 状态查询
    bool isInState(ParseState state) const;
    bool isInBlock(BlockType blockType) const;
    bool isInGlobalScope() const;
    bool isInLocalScope() const;
    bool isInStyleContext() const;
    bool isInScriptContext() const;
    bool isInTemplateDefinition() const;
    bool isInCustomDefinition() const;
    bool isInSpecialization() const;
    
    // 约束检查
    bool isAllowedInCurrentContext(const std::string& element) const;
    void addException(const std::string& element);
    void clearExceptions();
    
    // 特殊状态检查
    bool canUseTemplateVariable() const;
    bool canUseCustomVariable() const;
    bool canUseStyleGroup() const;
    bool canUseFrom() const;
    bool canUseDelete() const;
    bool canUseInherit() const;
    bool canUseSpecialization() const;
    
    // 获取当前块信息
    std::string getCurrentBlockName() const;
    std::string getCurrentBlockType() const;
    BlockType getCurrentBlockTypeEnum() const;
    
    // 调试
    void dumpStateStack() const;
    std::string stateToString(ParseState state) const;
    std::string blockTypeToString(BlockType type) const;
    
    // 重置
    void reset();
    
    friend class StateGuard;
};

} // namespace CHTL

#endif // CHTL_STATE_H