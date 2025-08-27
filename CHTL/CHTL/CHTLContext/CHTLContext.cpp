#include "CHTLContext.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/OriginNode.h"
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace CHTL {

CHTLContext::CHTLContext(const std::string& source_file, GlobalMapPtr global_map)
    : source_file_(source_file), 
      global_map_(global_map ? global_map : std::make_shared<GlobalMap>()),
      error_count_(0), warning_count_(0), strict_mode_(false) {
    
    // 创建全局作用域
    pushScope(ScopeType::GLOBAL, "global");
}

void CHTLContext::pushScope(ScopeType type, const std::string& name, NodePtr owner) {
    auto scope = std::make_shared<Scope>(type, name, owner);
    scope_stack_.push(scope);
}

bool CHTLContext::popScope() {
    if (scope_stack_.empty()) {
        return false;
    }
    
    scope_stack_.pop();
    return true;
}

std::shared_ptr<Scope> CHTLContext::getCurrentScope() {
    return scope_stack_.empty() ? nullptr : scope_stack_.top();
}

std::shared_ptr<Scope> CHTLContext::getScope(ScopeType type) {
    std::stack<std::shared_ptr<Scope>> temp_stack = scope_stack_;
    
    while (!temp_stack.empty()) {
        auto scope = temp_stack.top();
        if (scope->type == type) {
            return scope;
        }
        temp_stack.pop();
    }
    
    return nullptr;
}

bool CHTLContext::defineSymbol(const std::string& name, NodePtr node) {
    auto current_scope = getCurrentScope();
    if (!current_scope) {
        addError("No current scope to define symbol: " + name);
        return false;
    }
    
    // 检查当前作用域中是否已经存在同名符号
    if (current_scope->symbols.find(name) != current_scope->symbols.end()) {
        if (strict_mode_) {
            addError("Symbol '" + name + "' is already defined in current scope");
            return false;
        } else {
            addWarning("Symbol '" + name + "' is being redefined in current scope");
        }
    }
    
    current_scope->symbols[name] = node;
    return true;
}

NodePtr CHTLContext::lookupSymbol(const std::string& name, bool search_parent) {
    if (!search_parent) {
        auto current_scope = getCurrentScope();
        if (current_scope) {
            auto it = current_scope->symbols.find(name);
            if (it != current_scope->symbols.end()) {
                return it->second;
            }
        }
        return nullptr;
    }
    
    // 从当前作用域开始向上搜索
    std::stack<std::shared_ptr<Scope>> temp_stack = scope_stack_;
    
    while (!temp_stack.empty()) {
        auto scope = temp_stack.top();
        auto it = scope->symbols.find(name);
        if (it != scope->symbols.end()) {
            return it->second;
        }
        temp_stack.pop();
    }
    
    return nullptr;
}

bool CHTLContext::isSymbolDefined(const std::string& name, bool current_scope_only) {
    return lookupSymbol(name, !current_scope_only) != nullptr;
}

bool CHTLContext::registerTemplate(std::shared_ptr<TemplateNode> template_node) {
    if (!template_node) {
        addError("Cannot register null template");
        return false;
    }
    
    const std::string& name = template_node->getTemplateName();
    
    // 检查是否已经存在同名模板
    if (templates_.find(name) != templates_.end()) {
        if (strict_mode_) {
            addError("Template '" + name + "' is already defined");
            return false;
        } else {
            addWarning("Template '" + name + "' is being redefined");
        }
    }
    
    templates_[name] = template_node;
    
    // 同时在符号表中定义
    return defineSymbol(name, template_node);
}

std::shared_ptr<TemplateNode> CHTLContext::getTemplate(const std::string& name) {
    auto it = templates_.find(name);
    return (it != templates_.end()) ? it->second : nullptr;
}

bool CHTLContext::registerCustom(std::shared_ptr<CustomNode> custom_node) {
    if (!custom_node) {
        addError("Cannot register null custom node");
        return false;
    }
    
    const std::string& name = custom_node->getName();
    
    if (customs_.find(name) != customs_.end()) {
        if (strict_mode_) {
            addError("Custom '" + name + "' is already defined");
            return false;
        } else {
            addWarning("Custom '" + name + "' is being redefined");
        }
    }
    
    customs_[name] = custom_node;
    
    return defineSymbol(name, custom_node);
}

std::shared_ptr<CustomNode> CHTLContext::getCustom(const std::string& name) {
    auto it = customs_.find(name);
    return (it != customs_.end()) ? it->second : nullptr;
}

bool CHTLContext::registerOrigin(std::shared_ptr<OriginNode> origin_node) {
    if (!origin_node) {
        addError("Cannot register null origin node");
        return false;
    }
    
    const std::string& name = origin_node->getName();
    
    if (origins_.find(name) != origins_.end()) {
        if (strict_mode_) {
            addError("Origin '" + name + "' is already defined");
            return false;
        } else {
            addWarning("Origin '" + name + "' is being redefined");
        }
    }
    
    origins_[name] = origin_node;
    
    return defineSymbol(name, origin_node);
}

std::shared_ptr<OriginNode> CHTLContext::getOrigin(const std::string& name) {
    auto it = origins_.find(name);
    return (it != origins_.end()) ? it->second : nullptr;
}

void CHTLContext::addImport(const ImportInfo& import_info) {
    imports_.push_back(import_info);
}

void CHTLContext::enterNamespace(const std::string& namespace_name) {
    if (!current_namespace_.empty()) {
        current_namespace_ += ".";
    }
    current_namespace_ += namespace_name;
    namespaces_.insert(current_namespace_);
    
    // 创建命名空间作用域
    pushScope(ScopeType::NAMESPACE, namespace_name);
}

void CHTLContext::exitNamespace() {
    // 退出命名空间作用域
    auto current_scope = getCurrentScope();
    if (current_scope && current_scope->type == ScopeType::NAMESPACE) {
        popScope();
    }
    
    // 更新当前命名空间
    size_t last_dot = current_namespace_.find_last_of('.');
    if (last_dot != std::string::npos) {
        current_namespace_ = current_namespace_.substr(0, last_dot);
    } else {
        current_namespace_.clear();
    }
}

bool CHTLContext::namespaceExists(const std::string& namespace_name) const {
    return namespaces_.find(namespace_name) != namespaces_.end();
}

void CHTLContext::addError(const std::string& message, int line, int column) {
    std::string formatted_message = formatMessage(message, line, column);
    error_messages_.push_back(formatted_message);
    error_count_++;
}

void CHTLContext::addWarning(const std::string& message, int line, int column) {
    std::string formatted_message = formatMessage(message, line, column);
    warning_messages_.push_back(formatted_message);
    warning_count_++;
}

void CHTLContext::clearMessages() {
    error_messages_.clear();
    warning_messages_.clear();
    error_count_ = 0;
    warning_count_ = 0;
}

void CHTLContext::reset() {
    // 清空作用域栈，只保留全局作用域
    while (scope_stack_.size() > 1) {
        popScope();
    }
    
    // 清空各种表
    templates_.clear();
    customs_.clear();
    origins_.clear();
    imports_.clear();
    namespaces_.clear();
    
    // 清空消息
    clearMessages();
    
    // 重置状态
    current_namespace_.clear();
    config_ = nullptr;
}

std::string CHTLContext::validate() const {
    std::ostringstream oss;
    
    // 检查作用域栈是否正确
    if (scope_stack_.empty()) {
        oss << "No global scope found; ";
    }
    
    // 检查是否有未闭合的作用域
    if (scope_stack_.size() > 1) {
        oss << "Unclosed scopes detected (count: " << scope_stack_.size() - 1 << "); ";
    }
    
    // 检查模板的有效性
    for (const auto& template_pair : templates_) {
        std::string template_error = template_pair.second->validate();
        if (!template_error.empty()) {
            oss << "Template '" << template_pair.first << "' validation failed: " 
                << template_error << "; ";
        }
    }
    
    // 检查循环依赖
    if (hasCircularDependency()) {
        oss << "Circular dependency detected; ";
    }
    
    std::string result = oss.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back(); // 移除最后的分号和空格
        result.pop_back();
    }
    
    return result;
}

std::string CHTLContext::generateDiagnosticReport() const {
    std::ostringstream oss;
    
    oss << "=== CHTL Context Diagnostic Report ===" << std::endl;
    oss << "Source File: " << source_file_ << std::endl;
    oss << "Current Namespace: " << (current_namespace_.empty() ? "(global)" : current_namespace_) << std::endl;
    oss << "Strict Mode: " << (strict_mode_ ? "ON" : "OFF") << std::endl;
    oss << std::endl;
    
    // 作用域信息
    oss << "Scope Chain: " << getScopeChain() << std::endl;
    oss << std::endl;
    
    // 注册的组件统计
    oss << "Registered Components:" << std::endl;
    oss << "  Templates: " << templates_.size() << std::endl;
    oss << "  Customs: " << customs_.size() << std::endl;
    oss << "  Origins: " << origins_.size() << std::endl;
    oss << "  Imports: " << imports_.size() << std::endl;
    oss << "  Namespaces: " << namespaces_.size() << std::endl;
    oss << std::endl;
    
    // 错误和警告统计
    oss << "Messages:" << std::endl;
    oss << "  Errors: " << error_count_ << std::endl;
    oss << "  Warnings: " << warning_count_ << std::endl;
    oss << std::endl;
    
    // 详细错误信息
    if (!error_messages_.empty()) {
        oss << "Error Details:" << std::endl;
        for (const auto& error : error_messages_) {
            oss << "  " << error << std::endl;
        }
        oss << std::endl;
    }
    
    // 详细警告信息
    if (!warning_messages_.empty()) {
        oss << "Warning Details:" << std::endl;
        for (const auto& warning : warning_messages_) {
            oss << "  " << warning << std::endl;
        }
        oss << std::endl;
    }
    
    // 验证结果
    std::string validation_result = validate();
    if (!validation_result.empty()) {
        oss << "Validation Issues:" << std::endl;
        oss << "  " << validation_result << std::endl;
    } else {
        oss << "Validation: PASSED" << std::endl;
    }
    
    return oss.str();
}

std::string CHTLContext::resolveFullyQualifiedName(const std::string& name) const {
    if (current_namespace_.empty()) {
        return name;
    }
    return current_namespace_ + "." + name;
}

bool CHTLContext::hasCircularDependency() const {
    // 简单的循环依赖检测
    // 在实际实现中，需要构建依赖图并进行深度优先搜索
    
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> rec_stack;
    
    for (const auto& template_pair : templates_) {
        if (visited.find(template_pair.first) == visited.end()) {
            // 这里需要实现具体的循环检测算法
            // 由于模板继承关系的复杂性，这里先简单返回false
        }
    }
    
    return false;
}

std::string CHTLContext::formatMessage(const std::string& message, int line, int column) const {
    std::ostringstream oss;
    
    if (!source_file_.empty()) {
        std::filesystem::path file_path(source_file_);
        oss << file_path.filename().string();
    } else {
        oss << "<unknown>";
    }
    
    if (line >= 0) {
        oss << ":" << line;
        if (column >= 0) {
            oss << ":" << column;
        }
    }
    
    oss << ": " << message;
    
    return oss.str();
}

std::string CHTLContext::getScopeChain() const {
    std::ostringstream oss;
    std::stack<std::shared_ptr<Scope>> temp_stack = scope_stack_;
    std::vector<std::string> scope_names;
    
    while (!temp_stack.empty()) {
        scope_names.push_back(temp_stack.top()->name);
        temp_stack.pop();
    }
    
    // 反转顺序，从全局到当前
    std::reverse(scope_names.begin(), scope_names.end());
    
    for (size_t i = 0; i < scope_names.size(); ++i) {
        if (i > 0) oss << " -> ";
        oss << scope_names[i];
    }
    
    return oss.str();
}

} // namespace CHTL