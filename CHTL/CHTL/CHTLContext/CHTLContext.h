#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include "../CHTLNode/BaseNode.h"
#include "../CHTLLexer/GlobalMap.h"

namespace CHTL {

// 前向声明
class TemplateNode;
class CustomNode;
class OriginNode;
class ConfigNode;
class NamespaceNode;

/**
 * @brief 作用域类型枚举
 */
enum class ScopeType {
    GLOBAL,         // 全局作用域
    NAMESPACE,      // 命名空间作用域
    TEMPLATE,       // 模板作用域
    CUSTOM,         // 自定义作用域
    ELEMENT,        // 元素作用域
    FUNCTION        // 函数作用域（用于CHTL JS）
};

/**
 * @brief 作用域结构
 */
struct Scope {
    ScopeType type;
    std::string name;
    std::unordered_map<std::string, NodePtr> symbols;  // 符号表
    NodePtr owner;  // 作用域所有者节点
    
    Scope(ScopeType t, const std::string& n, NodePtr o = nullptr)
        : type(t), name(n), owner(o) {}
};

/**
 * @brief 导入信息结构
 */
struct ImportInfo {
    std::string file_path;      // 文件路径
    std::string import_type;    // 导入类型（@Chtl, @Html等）
    std::string alias;          // 别名
    std::vector<std::string> specific_items; // 特定导入项
    bool is_wildcard;          // 是否为通配符导入
    
    ImportInfo() : is_wildcard(false) {}
};

/**
 * @brief CHTL编译上下文
 * 管理编译过程中的状态、作用域、符号表等
 */
class CHTLContext {
private:
    std::string source_file_;                                    // 源文件路径
    std::string source_content_;                                 // 源代码内容
    std::stack<std::shared_ptr<Scope>> scope_stack_;            // 作用域栈
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> templates_; // 模板表
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> customs_;     // 自定义表
    std::unordered_map<std::string, std::shared_ptr<OriginNode>> origins_;     // 原始嵌入表
    std::vector<ImportInfo> imports_;                            // 导入列表
    std::unordered_set<std::string> namespaces_;               // 命名空间集合
    std::shared_ptr<ConfigNode> config_;                        // 配置节点
    GlobalMapPtr global_map_;                                   // 全局映射表
    std::vector<std::string> error_messages_;                  // 错误消息列表
    std::vector<std::string> warning_messages_;                // 警告消息列表
    int error_count_;                                           // 错误计数
    int warning_count_;                                         // 警告计数
    bool strict_mode_;                                          // 严格模式
    std::string current_namespace_;                             // 当前命名空间

public:
    /**
     * @brief 构造函数
     * @param source_file 源文件路径
     * @param global_map 全局映射表
     */
    CHTLContext(const std::string& source_file = "", GlobalMapPtr global_map = nullptr);

    /**
     * @brief 析构函数
     */
    ~CHTLContext() = default;

    // Getter方法
    const std::string& getSourceFile() const { return source_file_; }
    const std::string& getSourceContent() const { return source_content_; }
    const std::vector<std::string>& getErrorMessages() const { return error_messages_; }
    const std::vector<std::string>& getWarningMessages() const { return warning_messages_; }
    int getErrorCount() const { return error_count_; }
    int getWarningCount() const { return warning_count_; }
    bool isStrictMode() const { return strict_mode_; }
    const std::string& getCurrentNamespace() const { return current_namespace_; }
    GlobalMapPtr getGlobalMap() const { return global_map_; }

    // Setter方法
    void setSourceFile(const std::string& file) { source_file_ = file; }
    void setSourceContent(const std::string& content) { source_content_ = content; }
    void setStrictMode(bool strict) { strict_mode_ = strict; }
    void setCurrentNamespace(const std::string& ns) { current_namespace_ = ns; }
    void setConfig(std::shared_ptr<ConfigNode> config) { config_ = config; }

    /**
     * @brief 进入新作用域
     * @param type 作用域类型
     * @param name 作用域名称
     * @param owner 作用域所有者节点
     */
    void pushScope(ScopeType type, const std::string& name, NodePtr owner = nullptr);

    /**
     * @brief 退出当前作用域
     * @return 是否成功退出
     */
    bool popScope();

    /**
     * @brief 获取当前作用域
     * @return 当前作用域，如果没有返回nullptr
     */
    std::shared_ptr<Scope> getCurrentScope();

    /**
     * @brief 获取指定类型的作用域
     * @param type 作用域类型
     * @return 作用域，如果找不到返回nullptr
     */
    std::shared_ptr<Scope> getScope(ScopeType type);

    /**
     * @brief 在当前作用域中定义符号
     * @param name 符号名称
     * @param node 节点
     * @return 是否成功定义
     */
    bool defineSymbol(const std::string& name, NodePtr node);

    /**
     * @brief 查找符号
     * @param name 符号名称
     * @param search_parent 是否搜索父作用域
     * @return 找到的节点，如果没有返回nullptr
     */
    NodePtr lookupSymbol(const std::string& name, bool search_parent = true);

    /**
     * @brief 检查符号是否已定义
     * @param name 符号名称
     * @param current_scope_only 是否只检查当前作用域
     * @return 是否已定义
     */
    bool isSymbolDefined(const std::string& name, bool current_scope_only = false);

    /**
     * @brief 注册模板
     * @param template_node 模板节点
     * @return 是否成功注册
     */
    bool registerTemplate(std::shared_ptr<TemplateNode> template_node);

    /**
     * @brief 获取模板
     * @param name 模板名称
     * @return 模板节点，如果没有返回nullptr
     */
    std::shared_ptr<TemplateNode> getTemplate(const std::string& name);

    /**
     * @brief 注册自定义
     * @param custom_node 自定义节点
     * @return 是否成功注册
     */
    bool registerCustom(std::shared_ptr<CustomNode> custom_node);

    /**
     * @brief 获取自定义
     * @param name 自定义名称
     * @return 自定义节点，如果没有返回nullptr
     */
    std::shared_ptr<CustomNode> getCustom(const std::string& name);

    /**
     * @brief 注册原始嵌入
     * @param origin_node 原始嵌入节点
     * @return 是否成功注册
     */
    bool registerOrigin(std::shared_ptr<OriginNode> origin_node);

    /**
     * @brief 获取原始嵌入
     * @param name 原始嵌入名称
     * @return 原始嵌入节点，如果没有返回nullptr
     */
    std::shared_ptr<OriginNode> getOrigin(const std::string& name);

    /**
     * @brief 添加导入
     * @param import_info 导入信息
     */
    void addImport(const ImportInfo& import_info);

    /**
     * @brief 获取所有导入
     * @return 导入列表
     */
    const std::vector<ImportInfo>& getImports() const { return imports_; }

    /**
     * @brief 进入命名空间
     * @param namespace_name 命名空间名称
     */
    void enterNamespace(const std::string& namespace_name);

    /**
     * @brief 退出命名空间
     */
    void exitNamespace();

    /**
     * @brief 检查命名空间是否存在
     * @param namespace_name 命名空间名称
     * @return 是否存在
     */
    bool namespaceExists(const std::string& namespace_name) const;

    /**
     * @brief 添加错误消息
     * @param message 错误消息
     * @param line 行号（可选）
     * @param column 列号（可选）
     */
    void addError(const std::string& message, int line = -1, int column = -1);

    /**
     * @brief 添加警告消息
     * @param message 警告消息
     * @param line 行号（可选）
     * @param column 列号（可选）
     */
    void addWarning(const std::string& message, int line = -1, int column = -1);

    /**
     * @brief 清空错误和警告
     */
    void clearMessages();

    /**
     * @brief 检查是否有错误
     * @return 是否有错误
     */
    bool hasErrors() const { return error_count_ > 0; }

    /**
     * @brief 检查是否有警告
     * @return 是否有警告
     */
    bool hasWarnings() const { return warning_count_ > 0; }

    /**
     * @brief 重置上下文
     */
    void reset();

    /**
     * @brief 验证上下文状态
     * @return 验证结果消息
     */
    std::string validate() const;

    /**
     * @brief 生成诊断报告
     * @return 诊断报告字符串
     */
    std::string generateDiagnosticReport() const;

    /**
     * @brief 解析完全限定名称
     * @param name 名称
     * @return 解析后的完全限定名称
     */
    std::string resolveFullyQualifiedName(const std::string& name) const;

    /**
     * @brief 检查是否存在循环依赖
     * @return 是否存在循环依赖
     */
    bool hasCircularDependency() const;

private:
    /**
     * @brief 格式化消息
     * @param message 消息内容
     * @param line 行号
     * @param column 列号
     * @return 格式化后的消息
     */
    std::string formatMessage(const std::string& message, int line, int column) const;

    /**
     * @brief 获取作用域链
     * @return 作用域链的字符串表示
     */
    std::string getScopeChain() const;
};

/**
 * @brief CHTLContext智能指针类型定义
 */
using CHTLContextPtr = std::shared_ptr<CHTLContext>;

} // namespace CHTL