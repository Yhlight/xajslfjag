#pragma once

#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace Common {

// 前向声明
class GlobalMap;

// 上下文类型
enum class ContextType {
    ROOT,                // 根上下文
    HTML_ELEMENT,        // HTML元素上下文
    TEMPLATE_STYLE,      // 模板样式组上下文
    TEMPLATE_ELEMENT,    // 模板元素上下文
    TEMPLATE_VAR,        // 模板变量组上下文
    CUSTOM_STYLE,        // 自定义样式组上下文
    CUSTOM_ELEMENT,      // 自定义元素上下文
    CUSTOM_VAR,          // 自定义变量组上下文
    LOCAL_STYLE,         // 局部样式块上下文
    LOCAL_SCRIPT,        // 局部脚本块上下文
    GLOBAL_STYLE,        // 全局样式上下文
    GLOBAL_SCRIPT,       // 全局脚本上下文
    ORIGIN_BLOCK,        // 原始嵌入块上下文
    CONFIGURATION,       // 配置块上下文
    NAMESPACE,           // 命名空间上下文
    IMPORT,              // 导入块上下文
    TEXT_NODE,           // 文本节点上下文
    COMMENT,             // 注释上下文
    UNKNOWN
};

// 作用域类型
enum class ScopeType {
    GLOBAL,              // 全局作用域
    NAMESPACE,           // 命名空间作用域
    TEMPLATE,            // 模板作用域
    CUSTOM,              // 自定义作用域
    ELEMENT,             // 元素作用域
    STYLE,               // 样式作用域
    SCRIPT,              // 脚本作用域
    LOCAL                // 局部作用域
};

// 约束类型 (根据CHTL语法文档的except语法)
enum class ConstraintType {
    NONE,                // 无约束
    ELEMENT_BLACKLIST,   // 元素黑名单
    TYPE_BLACKLIST,      // 类型黑名单
    GLOBAL_CONSTRAINT    // 全局约束
};

// 变量信息
struct Variable {
    std::string name;
    std::string value;
    std::string type;
    bool is_template_var; // 是否是模板变量
    
    Variable() : is_template_var(false) {}
    Variable(const std::string& n, const std::string& v, const std::string& t = "")
        : name(n), value(v), type(t), is_template_var(false) {}
};

// 上下文节点
class ContextNode {
public:
    ContextType type;
    std::string name;
    ScopeType scope;
    ConstraintType constraint;
    std::string namespace_path;
    std::vector<std::string> constraints; // 约束列表
    
    // 父子关系
    std::shared_ptr<ContextNode> parent;
    std::vector<std::shared_ptr<ContextNode>> children;
    
    // 变量和属性
    std::unordered_map<std::string, Variable> variables;
    std::unordered_map<std::string, std::string> attributes;
    
    // 位置信息
    std::string file_path;
    size_t line;
    size_t column;
    
    ContextNode(ContextType t, const std::string& n = "", ScopeType s = ScopeType::LOCAL)
        : type(t), name(n), scope(s), constraint(ConstraintType::NONE), line(0), column(0) {}
    
    // 添加子节点
    void addChild(std::shared_ptr<ContextNode> child) {
        child->parent = shared_from_this();
        children.push_back(child);
    }
    
    // 设置变量
    void setVariable(const std::string& name, const std::string& value, const std::string& type = "") {
        variables[name] = Variable(name, value, type);
    }
    
    // 获取变量
    Variable* getVariable(const std::string& name) {
        auto it = variables.find(name);
        return it != variables.end() ? &it->second : nullptr;
    }
    
    // 设置属性
    void setAttribute(const std::string& name, const std::string& value) {
        attributes[name] = value;
    }
    
    // 获取属性
    std::string getAttribute(const std::string& name, const std::string& default_value = "") const {
        auto it = attributes.find(name);
        return it != attributes.end() ? it->second : default_value;
    }
    
    // 添加约束
    void addConstraint(const std::string& constraint) {
        constraints.push_back(constraint);
    }
    
    // 检查约束
    bool isConstrained(const std::string& element) const {
        for (const auto& c : constraints) {
            if (c == element) return true;
        }
        return false;
    }
    
    // 获取完整路径
    std::string getFullPath() const {
        if (!parent) return name;
        std::string parent_path = parent->getFullPath();
        return parent_path.empty() ? name : parent_path + "." + name;
    }
    
private:
    // 使 ContextNode 可以被 shared_ptr 管理
    friend class std::shared_ptr<ContextNode>;
};

// 需要让 ContextNode 能够使用 shared_from_this
class ContextNodeImpl : public ContextNode, public std::enable_shared_from_this<ContextNodeImpl> {
public:
    ContextNodeImpl(ContextType t, const std::string& n = "", ScopeType s = ScopeType::LOCAL)
        : ContextNode(t, n, s) {}
};

// 上下文管理器
class Context {
public:
    Context();
    ~Context();
    
    // 上下文栈管理
    void pushContext(ContextType type, const std::string& name = "", ScopeType scope = ScopeType::LOCAL);
    
    void popContext();
    
    std::shared_ptr<ContextNode> getCurrentContext() const;
    
    std::shared_ptr<ContextNode> getRootContext() const;
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value, const std::string& type = "");
    
    std::string getVariable(const std::string& name) const;
    
    bool hasVariable(const std::string& name) const;
    
    void clearVariables();
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    
    std::string getAttribute(const std::string& name, const std::string& default_value = "") const;
    
    bool hasAttribute(const std::string& name) const;
    
    // 作用域管理
    ScopeType getCurrentScope() const;
    
    bool isInScope(ScopeType scope) const;
    
    bool isInContext(ContextType type) const;
    
    // 约束管理 (根据CHTL语法文档的except语法)
    void addConstraint(const std::string& element);
    
    bool isElementAllowed(const std::string& element) const;
    
    bool isTypeAllowed(const std::string& type) const;
    
    void setGlobalConstraint(const std::string& constraint);
    
    // 命名空间管理
    void enterNamespace(const std::string& namespace_name);
    
    void exitNamespace();
    
    std::string getCurrentNamespace() const;
    
    std::string resolveNamespacePath(const std::string& name) const;
    
    // 模板和自定义管理
    bool isInTemplateContext() const;
    
    bool isInCustomContext() const;
    
    std::string getCurrentTemplateType() const;
    
    std::string getCurrentCustomType() const;
    
    // 位置信息
    void setPosition(const std::string& file_path, size_t line, size_t column);
    
    std::string getCurrentFile() const;
    
    size_t getCurrentLine() const;
    
    size_t getCurrentColumn() const;
    
    // 语法验证 (根据CHTL语法文档的约束)
    bool canUseTemplateVar() const;       // 是否可以使用模板变量
    
    bool canUseCustomVar() const;         // 是否可以使用自定义变量
    
    bool canUseLocalStyle() const;        // 是否可以使用局部样式
    
    bool canUseLocalScript() const;       // 是否可以使用局部脚本
    
    bool canUseOriginBlock() const;       // 是否可以使用原始嵌入
    
    bool canUseHTMLElement() const;       // 是否可以使用HTML元素
    
    bool canUseGeneratorComment() const;  // 是否可以使用生成器注释
    
    // 自动化功能管理
    bool shouldAutoAddClass() const;      // 是否应该自动添加类名
    
    bool shouldAutoAddId() const;         // 是否应该自动添加ID
    
    void markClassAdded(const std::string& class_name);
    
    void markIdAdded(const std::string& id_name);
    
    // 选择器引用管理
    void setCurrentSelector(const std::string& selector);
    
    std::string getCurrentSelector() const;
    
    std::string resolveReference(const std::string& reference) const; // 解析&引用
    
    // 调试和诊断
    void dumpContext() const;
    
    std::string getContextPath() const;
    
    std::vector<ContextType> getContextStack() const;
    
    // 清理
    void clear();

private:
    std::stack<std::shared_ptr<ContextNode>> context_stack;
    std::shared_ptr<ContextNode> root_context;
    
    // 当前状态
    std::string current_namespace;
    std::string current_selector;
    std::vector<std::string> added_classes;
    std::vector<std::string> added_ids;
    
    // 全局引用
    GlobalMap* global_map;
    
    // 内部辅助方法
    std::shared_ptr<ContextNode> findVariableInScope(const std::string& name) const;
    
    bool checkConstraintRecursive(const std::string& element) const;
    
    std::string buildContextPath() const;
};

} // namespace Common
} // namespace CHTL