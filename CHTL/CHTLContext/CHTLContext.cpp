#include "CHTLContext.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <algorithm>  // 添加这个头文件

namespace CHTL {

// 线程局部存储的上下文
thread_local CHTLContext* current_context = nullptr;

CHTLContext& getCurrentContext() {
    if (!current_context) {
        static thread_local CHTLContext default_context;
        current_context = &default_context;
    }
    return *current_context;
}

// ContextGuard 实现
ContextGuard::ContextGuard(CHTLContext* ctx, ContextType type, const std::string& name)
    : context(ctx), released(false) {
    if (context) {
        context->pushContext(type, name);
    }
}

ContextGuard::~ContextGuard() {
    if (context && !released) {
        context->popContext();
    }
}

ContextGuard::ContextGuard(ContextGuard&& other) noexcept
    : context(other.context), released(other.released) {
    other.context = nullptr;
    other.released = true;
}

ContextGuard& ContextGuard::operator=(ContextGuard&& other) noexcept {
    if (this != &other) {
        if (context && !released) {
            context->popContext();
        }
        context = other.context;
        released = other.released;
        other.context = nullptr;
        other.released = true;
    }
    return *this;
}

// CHTLContext 实现
CHTLContext::CHTLContext() 
    : current_file(""),
      current_directory("."),
      state(std::make_unique<CHTLState>()), 
      global_map(nullptr) {  // 先设为nullptr
    current_context = this;
    
    // 延迟获取global_map，确保getGlobalMap()已经正确初始化
    // 使用全限定名调用全局函数
    global_map = &::CHTL::getGlobalMap();  // 使用全限定名
    
    // 确保所有成员都已初始化后再推入文件上下文
    // 直接推入而不调用pushContext，避免在初始化期间的复杂操作
    context_stack.push(ContextInfo(ContextType::FILE, "main"));
}

void CHTLContext::pushContext(ContextType type, const std::string& name) {
    // 先创建新的上下文信息
    ContextInfo newContext(type, name);
    
    // 确保state已初始化
    if (!state) {
        std::cerr << "Error: State not initialized!" << std::endl;
        return;
    }
    
    // 推入上下文
    context_stack.push(newContext);
    
    // 同步更新状态
    switch (type) {
        case ContextType::NAMESPACE:
            state->pushState(StateType::NAMESPACE, name);
            global_map->enterNamespace(name);
            break;
        case ContextType::ELEMENT:
            state->pushState(StateType::ELEMENT, name);
            break;
        case ContextType::STYLE:
            // 根据当前是否在元素内判断是局部还是全局style
            // 注意：这里调用isInElementContext时，上下文栈已经包含了新推入的上下文
            // 所以需要检查栈中是否有ELEMENT类型的上下文（不包括当前推入的）
            {
                bool inElement = false;
                std::stack<ContextInfo> temp = context_stack;
                temp.pop(); // 移除刚推入的STYLE上下文
                while (!temp.empty()) {
                    if (temp.top().type == ContextType::ELEMENT) {
                        inElement = true;
                        break;
                    }
                    temp.pop();
                }
                
                if (inElement) {
                    state->pushState(StateType::STYLE_LOCAL, name);
                } else {
                    state->pushState(StateType::STYLE_GLOBAL, name);
                }
            }
            break;
        case ContextType::SCRIPT:
            // 同样的逻辑用于SCRIPT
            {
                bool inElement = false;
                std::stack<ContextInfo> temp = context_stack;
                temp.pop(); // 移除刚推入的SCRIPT上下文
                while (!temp.empty()) {
                    if (temp.top().type == ContextType::ELEMENT) {
                        inElement = true;
                        break;
                    }
                    temp.pop();
                }
                
                if (inElement) {
                    state->pushState(StateType::SCRIPT_LOCAL, name);
                } else {
                    state->pushState(StateType::SCRIPT_GLOBAL, name);
                }
            }
            break;
        case ContextType::IMPORT:
            state->pushState(StateType::IMPORT_DECLARATION, name);
            break;
        case ContextType::CONFIGURATION:
            state->pushState(StateType::CONFIGURATION, name);
            break;
        case ContextType::FILE:
            // FILE上下文不需要更新状态
            break;
    }
}

void CHTLContext::popContext() {
    if (!context_stack.empty() && context_stack.size() > 1) {  // 保留文件上下文
        ContextType type = context_stack.top().type;
        context_stack.pop();
        
        // 同步更新状态
        state->popState();
        
        // 特殊处理命名空间
        if (type == ContextType::NAMESPACE) {
            global_map->exitNamespace();
        }
    }
}

ContextInfo& CHTLContext::getCurrentContext() {
    // 确保栈中至少有一个元素
    if (context_stack.empty()) {
        // 创建并推入默认的文件上下文
        context_stack.push(ContextInfo(ContextType::FILE, "default"));
    }
    return context_stack.top();
}

const ContextInfo& CHTLContext::getCurrentContext() const {
    // 对于const版本，不能修改栈，所以返回静态对象
    if (context_stack.empty()) {
        static const ContextInfo default_context(ContextType::FILE, "default");
        return default_context;
    }
    return context_stack.top();
}

ContextGuard CHTLContext::enterContext(ContextType type, const std::string& name) {
    return ContextGuard(this, type, name);
}

void CHTLContext::setCurrentFile(const std::string& file) {
    current_file = file;
    
    // 提取目录 - 添加异常处理
    try {
        std::filesystem::path p(file);
        if (p.has_parent_path()) {
            current_directory = p.parent_path().string();
        }
        if (current_directory.empty()) {
            current_directory = ".";
        }
    } catch (const std::exception& e) {
        // 如果filesystem操作失败，使用默认目录
        current_directory = ".";
    }
    
    // 更新文件上下文
    if (!context_stack.empty() && context_stack.size() > 0) {
        // 需要修改栈顶元素，但stack不支持直接修改
        // 所以我们暂时跳过这个操作
    }
}

bool CHTLContext::addLocalSymbol(const std::string& name, const SymbolInfo& info) {
    auto& ctx = getCurrentContext();
    
    // 检查局部符号表中是否已存在
    if (ctx.local_symbols.find(name) != ctx.local_symbols.end()) {
        return false;
    }
    
    ctx.local_symbols[name] = info;
    return true;
}

bool CHTLContext::hasLocalSymbol(const std::string& name) const {
    const auto& ctx = getCurrentContext();
    return ctx.local_symbols.find(name) != ctx.local_symbols.end();
}

std::optional<SymbolInfo> CHTLContext::getLocalSymbol(const std::string& name) const {
    const auto& ctx = getCurrentContext();
    auto it = ctx.local_symbols.find(name);
    if (it != ctx.local_symbols.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<SymbolInfo> CHTLContext::findSymbol(const std::string& name) const {
    // 1. 先在局部符号表中查找
    auto local_sym = getLocalSymbol(name);
    if (local_sym.has_value()) {
        return local_sym;
    }
    
    // 2. 再在全局符号表中查找
    return global_map->findSymbol(name);
}

void CHTLContext::setAttribute(const std::string& name, const std::string& value) {
    auto& ctx = getCurrentContext();
    ctx.attributes[name] = value;
}

std::string CHTLContext::getAttribute(const std::string& name) const {
    const auto& ctx = getCurrentContext();
    auto it = ctx.attributes.find(name);
    return it != ctx.attributes.end() ? it->second : "";
}

bool CHTLContext::hasAttribute(const std::string& name) const {
    const auto& ctx = getCurrentContext();
    return ctx.attributes.find(name) != ctx.attributes.end();
}

std::unordered_map<std::string, std::string> CHTLContext::getAttributes() const {
    return getCurrentContext().attributes;
}

void CHTLContext::setMetadata(const std::string& key, const std::string& value) {
    auto& ctx = getCurrentContext();
    ctx.metadata[key] = value;
}

std::string CHTLContext::getMetadata(const std::string& key) const {
    const auto& ctx = getCurrentContext();
    auto it = ctx.metadata.find(key);
    return it != ctx.metadata.end() ? it->second : "";
}

bool CHTLContext::hasMetadata(const std::string& key) const {
    const auto& ctx = getCurrentContext();
    return ctx.metadata.find(key) != ctx.metadata.end();
}

void CHTLContext::addError(const std::string& message, int line, int column) {
    std::stringstream ss;
    ss << current_file;
    if (line >= 0) {
        ss << ":" << line;
        if (column >= 0) {
            ss << ":" << column;
        }
    }
    ss << ": error: " << message;
    errors.push_back(ss.str());
}

void CHTLContext::addWarning(const std::string& message, int line, int column) {
    std::stringstream ss;
    ss << current_file;
    if (line >= 0) {
        ss << ":" << line;
        if (column >= 0) {
            ss << ":" << column;
        }
    }
    ss << ": warning: " << message;
    warnings.push_back(ss.str());
}

std::string CHTLContext::resolveModulePath(const std::string& module_name, 
                                           const std::string& type) const {
    namespace fs = std::filesystem;
    
    // 1. 首先检查官方模块目录
    if (!options.official_module_directory.empty()) {
        fs::path official_path = fs::path(options.official_module_directory);
        
        // 检查分类目录结构
        if (type == "@Chtl" || type == "cmod") {
            fs::path cmod_dir = official_path / "CMOD";
            if (fs::exists(cmod_dir)) {
                // 优先查找.cmod文件
                fs::path cmod_file = cmod_dir / (module_name + ".cmod");
                if (fs::exists(cmod_file)) {
                    return cmod_file.string();
                }
                // 其次查找.chtl文件
                fs::path chtl_file = cmod_dir / (module_name + ".chtl");
                if (fs::exists(chtl_file)) {
                    return chtl_file.string();
                }
            }
        } else if (type == "@CJmod" || type == "cjmod") {
            fs::path cjmod_dir = official_path / "CJMOD";
            if (fs::exists(cjmod_dir)) {
                fs::path cjmod_file = cjmod_dir / (module_name + ".cjmod");
                if (fs::exists(cjmod_file)) {
                    return cjmod_file.string();
                }
            }
        }
        
        // 检查混合目录结构
        if (type == "@Chtl" || type == "cmod") {
            fs::path cmod_file = official_path / (module_name + ".cmod");
            if (fs::exists(cmod_file)) {
                return cmod_file.string();
            }
            fs::path chtl_file = official_path / (module_name + ".chtl");
            if (fs::exists(chtl_file)) {
                return chtl_file.string();
            }
        } else if (type == "@CJmod" || type == "cjmod") {
            fs::path cjmod_file = official_path / (module_name + ".cjmod");
            if (fs::exists(cjmod_file)) {
                return cjmod_file.string();
            }
        }
    }
    
    // 2. 检查当前目录的module文件夹
    fs::path current_module_dir = fs::path(current_directory) / "module";
    if (fs::exists(current_module_dir)) {
        // 检查分类目录结构
        if (type == "@Chtl" || type == "cmod") {
            fs::path cmod_dir = current_module_dir / "CMOD";
            if (fs::exists(cmod_dir)) {
                fs::path cmod_file = cmod_dir / (module_name + ".cmod");
                if (fs::exists(cmod_file)) {
                    return cmod_file.string();
                }
                fs::path chtl_file = cmod_dir / (module_name + ".chtl");
                if (fs::exists(chtl_file)) {
                    return chtl_file.string();
                }
            }
        } else if (type == "@CJmod" || type == "cjmod") {
            fs::path cjmod_dir = current_module_dir / "CJMOD";
            if (fs::exists(cjmod_dir)) {
                fs::path cjmod_file = cjmod_dir / (module_name + ".cjmod");
                if (fs::exists(cjmod_file)) {
                    return cjmod_file.string();
                }
            }
        }
        
        // 检查混合目录结构
        if (type == "@Chtl" || type == "cmod") {
            fs::path cmod_file = current_module_dir / (module_name + ".cmod");
            if (fs::exists(cmod_file)) {
                return cmod_file.string();
            }
            fs::path chtl_file = current_module_dir / (module_name + ".chtl");
            if (fs::exists(chtl_file)) {
                return chtl_file.string();
            }
        } else if (type == "@CJmod" || type == "cjmod") {
            fs::path cjmod_file = current_module_dir / (module_name + ".cjmod");
            if (fs::exists(cjmod_file)) {
                return cjmod_file.string();
            }
        }
    }
    
    // 3. 检查当前目录
    if (type == "@Chtl" || type == "cmod") {
        fs::path cmod_file = fs::path(current_directory) / (module_name + ".cmod");
        if (fs::exists(cmod_file)) {
            return cmod_file.string();
        }
        fs::path chtl_file = fs::path(current_directory) / (module_name + ".chtl");
        if (fs::exists(chtl_file)) {
            return chtl_file.string();
        }
    } else if (type == "@CJmod" || type == "cjmod") {
        fs::path cjmod_file = fs::path(current_directory) / (module_name + ".cjmod");
        if (fs::exists(cjmod_file)) {
            return cjmod_file.string();
        }
    }
    
    return "";  // 未找到
}

std::string CHTLContext::resolveImportPath(const std::string& path, 
                                           const std::string& type) const {
    namespace fs = std::filesystem;
    
    // 如果是绝对路径，直接返回
    if (fs::path(path).is_absolute()) {
        if (fs::exists(path)) {
            return path;
        }
        return "";
    }
    
    // 如果是具体文件（带后缀名）
    if (path.find('.') != std::string::npos) {
        fs::path file_path = fs::path(current_directory) / path;
        if (fs::exists(file_path)) {
            return file_path.string();
        }
        return "";
    }
    
    // 如果是文件名（不带后缀），根据类型查找
    if (type == "@Html") {
        fs::path html_file = fs::path(current_directory) / (path + ".html");
        if (fs::exists(html_file)) {
            return html_file.string();
        }
    } else if (type == "@Style") {
        fs::path css_file = fs::path(current_directory) / (path + ".css");
        if (fs::exists(css_file)) {
            return css_file.string();
        }
    } else if (type == "@JavaScript") {
        fs::path js_file = fs::path(current_directory) / (path + ".js");
        if (fs::exists(js_file)) {
            return js_file.string();
        }
    }
    
    return "";
}

bool CHTLContext::isInContext(ContextType type) const {
    std::stack<ContextInfo> temp = context_stack;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            return true;
        }
        temp.pop();
    }
    return false;
}

int CHTLContext::getContextDepth() const {
    return static_cast<int>(context_stack.size());
}

std::vector<ContextType> CHTLContext::getContextStack() const {
    std::vector<ContextType> result;
    std::stack<ContextInfo> temp = context_stack;
    while (!temp.empty()) {
        result.push_back(temp.top().type);
        temp.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}

bool CHTLContext::isInFileContext() const {
    return isInContext(ContextType::FILE);
}

bool CHTLContext::isInNamespaceContext() const {
    return isInContext(ContextType::NAMESPACE);
}

bool CHTLContext::isInElementContext() const {
    return isInContext(ContextType::ELEMENT);
}

bool CHTLContext::isInStyleContext() const {
    return isInContext(ContextType::STYLE);
}

bool CHTLContext::isInScriptContext() const {
    return isInContext(ContextType::SCRIPT);
}

std::optional<ContextInfo> CHTLContext::findContext(ContextType type) const {
    std::stack<ContextInfo> temp = context_stack;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            return temp.top();
        }
        temp.pop();
    }
    return std::nullopt;
}

std::string CHTLContext::getCurrentNamespace() const {
    auto ns_ctx = findContext(ContextType::NAMESPACE);
    return ns_ctx.has_value() ? ns_ctx->name : "";
}

std::string CHTLContext::getCurrentElement() const {
    auto elem_ctx = findContext(ContextType::ELEMENT);
    return elem_ctx.has_value() ? elem_ctx->name : "";
}

void CHTLContext::clear() {
    while (!context_stack.empty()) {
        context_stack.pop();
    }
    current_file.clear();
    current_directory = ".";
    state->clear();
    errors.clear();
    warnings.clear();
    
    // 重新初始化文件上下文
    pushContext(ContextType::FILE, "main");
}

void CHTLContext::clearContextStack() {
    while (context_stack.size() > 1) {  // 保留文件上下文
        popContext();
    }
}

void CHTLContext::dumpContext() const {
    std::cout << "=== Context Stack ===" << std::endl;
    std::cout << "Current File: " << current_file << std::endl;
    std::cout << "Current Directory: " << current_directory << std::endl;
    
    std::cout << "\nContext Stack (bottom to top):" << std::endl;
    std::stack<ContextInfo> temp = context_stack;
    std::vector<ContextInfo> contexts;
    while (!temp.empty()) {
        contexts.push_back(temp.top());
        temp.pop();
    }
    
    for (int i = contexts.size() - 1; i >= 0; i--) {
        const auto& ctx = contexts[i];
        std::cout << "  [" << (contexts.size() - 1 - i) << "] ";
        switch (ctx.type) {
            case ContextType::FILE: std::cout << "FILE"; break;
            case ContextType::NAMESPACE: std::cout << "NAMESPACE"; break;
            case ContextType::ELEMENT: std::cout << "ELEMENT"; break;
            case ContextType::STYLE: std::cout << "STYLE"; break;
            case ContextType::SCRIPT: std::cout << "SCRIPT"; break;
            case ContextType::IMPORT: std::cout << "IMPORT"; break;
            case ContextType::CONFIGURATION: std::cout << "CONFIGURATION"; break;
        }
        std::cout << " (" << ctx.name << ")";
        if (!ctx.file_path.empty()) {
            std::cout << " [" << ctx.file_path << "]";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n=== State Stack ===" << std::endl;
    state->dumpStack();
}

std::string CHTLContext::getContextString() const {
    std::stringstream ss;
    std::stack<ContextInfo> temp = context_stack;
    std::vector<std::string> parts;
    
    while (!temp.empty()) {
        const auto& ctx = temp.top();
        if (ctx.type != ContextType::FILE) {
            parts.push_back(ctx.name);
        }
        temp.pop();
    }
    
    for (size_t i = parts.size(); i > 0; i--) {
        if (i < parts.size()) ss << "::";
        ss << parts[i - 1];
    }
    
    return ss.str();
}

} // namespace CHTL