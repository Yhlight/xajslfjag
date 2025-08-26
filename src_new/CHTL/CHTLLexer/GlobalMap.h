#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTL {
namespace Lexer {

/**
 * GlobalMap - CHTL全局映射表
 * 新目录结构下的全局状态管理
 */
class GlobalMap {
private:
    // 单例实例
    static std::shared_ptr<GlobalMap> instance_;
    
    // 命名空间映射
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> namespaces_;
    
    // 文件依赖映射
    std::unordered_map<std::string, std::unordered_set<std::string>> file_dependencies_;
    
    // 模板映射
    std::unordered_map<std::string, std::string> templates_;
    
    // 自定义元素映射
    std::unordered_map<std::string, std::string> custom_elements_;
    
    // 配置映射
    std::unordered_map<std::string, std::string> configurations_;
    
    // 生成的名称映射（避免冲突）
    std::unordered_set<std::string> generated_names_;
    
    // 错误和警告
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 当前编译文件
    std::string current_file_;
    
    // 私有构造函数（单例模式）
    GlobalMap() = default;
    
public:
    /**
     * 获取单例实例
     * @return GlobalMap实例
     */
    static std::shared_ptr<GlobalMap> getInstance();
    
    /**
     * 重置全局状态
     */
    void reset();
    
    // 命名空间管理
    /**
     * 注册命名空间
     * @param namespace_name 命名空间名称
     * @param symbols 符号映射
     */
    void registerNamespace(const std::string& namespace_name, 
                          const std::unordered_map<std::string, std::string>& symbols);
    
    /**
     * 获取命名空间符号
     * @param namespace_name 命名空间名称
     * @param symbol_name 符号名称
     * @return 符号值
     */
    std::string getNamespaceSymbol(const std::string& namespace_name, 
                                  const std::string& symbol_name) const;
    
    /**
     * 检查命名空间是否存在
     * @param namespace_name 命名空间名称
     * @return 是否存在
     */
    bool hasNamespace(const std::string& namespace_name) const;
    
    // 文件依赖管理
    /**
     * 添加文件依赖
     * @param file_path 文件路径
     * @param dependency 依赖文件
     */
    void addFileDependency(const std::string& file_path, const std::string& dependency);
    
    /**
     * 检查循环依赖
     * @param file_path 文件路径
     * @return 是否有循环依赖
     */
    bool hasCircularDependency(const std::string& file_path) const;
    
    /**
     * 获取文件依赖列表
     * @param file_path 文件路径
     * @return 依赖列表
     */
    std::unordered_set<std::string> getFileDependencies(const std::string& file_path) const;
    
    // 模板管理
    /**
     * 注册模板
     * @param template_name 模板名称
     * @param template_definition 模板定义
     */
    void registerTemplate(const std::string& template_name, const std::string& template_definition);
    
    /**
     * 获取模板定义
     * @param template_name 模板名称
     * @return 模板定义
     */
    std::string getTemplate(const std::string& template_name) const;
    
    /**
     * 检查模板是否存在
     * @param template_name 模板名称
     * @return 是否存在
     */
    bool hasTemplate(const std::string& template_name) const;
    
    // 自定义元素管理
    /**
     * 注册自定义元素
     * @param element_name 元素名称
     * @param element_definition 元素定义
     */
    void registerCustomElement(const std::string& element_name, const std::string& element_definition);
    
    /**
     * 获取自定义元素定义
     * @param element_name 元素名称
     * @return 元素定义
     */
    std::string getCustomElement(const std::string& element_name) const;
    
    /**
     * 检查自定义元素是否存在
     * @param element_name 元素名称
     * @return 是否存在
     */
    bool hasCustomElement(const std::string& element_name) const;
    
    // 配置管理
    /**
     * 设置配置
     * @param key 配置键
     * @param value 配置值
     */
    void setConfiguration(const std::string& key, const std::string& value);
    
    /**
     * 获取配置
     * @param key 配置键
     * @return 配置值
     */
    std::string getConfiguration(const std::string& key) const;
    
    /**
     * 检查配置是否存在
     * @param key 配置键
     * @return 是否存在
     */
    bool hasConfiguration(const std::string& key) const;
    
    // 名称生成
    /**
     * 生成唯一名称
     * @param base_name 基础名称
     * @return 唯一名称
     */
    std::string generateUniqueName(const std::string& base_name);
    
    /**
     * 检查名称是否已使用
     * @param name 名称
     * @return 是否已使用
     */
    bool isNameUsed(const std::string& name) const;
    
    // 错误和警告管理
    /**
     * 添加错误
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * 添加警告
     * @param warning 警告信息
     */
    void addWarning(const std::string& warning);
    
    /**
     * 获取错误列表
     * @return 错误列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * 获取警告列表
     * @return 警告列表
     */
    const std::vector<std::string>& getWarnings() const;
    
    /**
     * 清除错误和警告
     */
    void clearMessages();
    
    // 当前文件管理
    /**
     * 设置当前编译文件
     * @param file_path 文件路径
     */
    void setCurrentFile(const std::string& file_path);
    
    /**
     * 获取当前编译文件
     * @return 文件路径
     */
    const std::string& getCurrentFile() const;
};

} // namespace Lexer
} // namespace CHTL