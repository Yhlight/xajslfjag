#pragma once
#include "../CHTLLexer/GlobalMap.h"
#include "../CHTLNode/BaseNode/BaseNode.h"
#include <stack>
#include <memory>

namespace CHTL {

/**
 * CHTL上下文类型
 */
enum class ContextType {
    GLOBAL,         // 全局上下文
    ELEMENT,        // 元素上下文
    TEMPLATE,       // 模板上下文
    CUSTOM,         // 自定义上下文
    STYLE,          // 样式上下文
    SCRIPT,         // 脚本上下文
    ORIGIN,         // 原始嵌入上下文
    IMPORT,         // 导入上下文
    CONFIG,         // 配置上下文
    NAMESPACE       // 命名空间上下文
};

/**
 * 上下文信息
 */
struct Context {
    ContextType type;
    std::shared_ptr<BaseNode> node;
    std::string name;
    size_t depth;
    
    Context(ContextType t, std::shared_ptr<BaseNode> n = nullptr, 
            const std::string& nm = "", size_t d = 0)
        : type(t), node(n), name(nm), depth(d) {}
};

/**
 * CHTL编译上下文管理器
 * 负责管理编译过程中的上下文状态
 */
class CHTLContext {
public:
    explicit CHTLContext(std::shared_ptr<GlobalMap> globalMap = nullptr);
    ~CHTLContext() = default;
    
    // 全局映射管理
    std::shared_ptr<GlobalMap> getGlobalMap() const { return globalMap_; }
    void setGlobalMap(std::shared_ptr<GlobalMap> globalMap) { globalMap_ = globalMap; }
    
    // 上下文栈管理
    void pushContext(ContextType type, std::shared_ptr<BaseNode> node = nullptr, 
                    const std::string& name = "");
    void popContext();
    Context getCurrentContext() const;
    Context getParentContext() const;
    size_t getContextDepth() const;
    bool hasContext() const;
    
    // 上下文查询
    bool isInContext(ContextType type) const;
    std::shared_ptr<BaseNode> findContextNode(ContextType type) const;
    std::string findContextName(ContextType type) const;
    
    // 特定上下文检查
    bool isInGlobalContext() const { return isInContext(ContextType::GLOBAL); }
    bool isInElementContext() const { return isInContext(ContextType::ELEMENT); }
    bool isInTemplateContext() const { return isInContext(ContextType::TEMPLATE); }
    bool isInCustomContext() const { return isInContext(ContextType::CUSTOM); }
    bool isInStyleContext() const { return isInContext(ContextType::STYLE); }
    bool isInScriptContext() const { return isInContext(ContextType::SCRIPT); }
    bool isInOriginContext() const { return isInContext(ContextType::ORIGIN); }
    bool isInNamespaceContext() const { return isInContext(ContextType::NAMESPACE); }
    
    // 约束检查
    void addConstraint(const std::string& constraint);
    void removeConstraint(const std::string& constraint);
    bool hasConstraint(const std::string& constraint) const;
    std::vector<std::string> getConstraints() const;
    void clearConstraints();
    
    // 语法约束验证
    bool canUseElement(const std::string& elementName) const;
    bool canUseTemplate(const std::string& templateType) const;
    bool canUseCustom(const std::string& customType) const;
    bool canUseOrigin() const;
    
    // 自动化功能状态
    struct AutomationState {
        bool styleAutoAddClass = true;
        bool styleAutoAddId = true;
        bool scriptAutoAddClass = false;
        bool scriptAutoAddId = false;
        bool disableDefaultNamespace = false;
        bool disableNameGroup = false;
        bool disableCustomOriginType = false;
        bool debugMode = false;
        size_t indexInitialCount = 0;
    };
    
    const AutomationState& getAutomationState() const { return automationState_; }
    void updateAutomationState(const AutomationState& state) { automationState_ = state; }
    
    // 当前编译文件信息
    void setCurrentFile(const std::string& filename);
    const std::string& getCurrentFile() const { return currentFile_; }
    
    // 错误处理
    void reportError(const std::string& message, const Position& pos = Position{});
    void reportWarning(const std::string& message, const Position& pos = Position{});
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    bool hasErrors() const { return !errors_.empty(); }
    bool hasWarnings() const { return !warnings_.empty(); }
    void clearErrors() { errors_.clear(); }
    void clearWarnings() { warnings_.clear(); }
    
    // 重置上下文
    void reset();
    
private:
    std::shared_ptr<GlobalMap> globalMap_;
    std::stack<Context> contextStack_;
    std::vector<std::string> constraints_;
    AutomationState automationState_;
    std::string currentFile_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 辅助方法
    std::string formatError(const std::string& message, const Position& pos) const;
    std::string formatWarning(const std::string& message, const Position& pos) const;
};

/**
 * 上下文类型到字符串转换
 */
std::string contextTypeToString(ContextType type);

/**
 * RAII上下文管理器
 * 自动管理上下文的推入和弹出
 */
class ContextGuard {
public:
    ContextGuard(CHTLContext& context, ContextType type, 
                std::shared_ptr<BaseNode> node = nullptr, 
                const std::string& name = "");
    ~ContextGuard();
    
    // 禁止复制
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
    
private:
    CHTLContext& context_;
};

} // namespace CHTL