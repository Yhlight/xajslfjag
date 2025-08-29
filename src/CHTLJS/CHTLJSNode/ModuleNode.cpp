#include "ModuleNode.h"
#include <sstream>

namespace CHTL {

// ModuleNode实现
void ModuleNode::accept(Visitor* visitor) {
    if (auto* v = dynamic_cast<ModuleVisitor*>(visitor)) {
        v->visitModuleNode(this);
    }
}

std::string ModuleNode::toString() const {
    std::stringstream ss;
    ss << "ModuleNode {\n";
    ss << "  load: [";
    
    for (size_t i = 0; i < loadItems_.size(); ++i) {
        ss << "\"" << loadItems_[i] << "\"";
        if (i < loadItems_.size() - 1) ss << ", ";
    }
    
    ss << "]\n";
    
    if (!exports_.empty()) {
        ss << "  export: {\n";
        for (const auto& [name, value] : exports_) {
            ss << "    " << name << ": " << value << "\n";
        }
        ss << "  }\n";
    }
    
    if (!moduleType_.empty()) {
        ss << "  type: " << moduleType_ << "\n";
    }
    
    ss << "}";
    return ss.str();
}

void ModuleNode::addExport(const std::string& name, const std::string& value) {
    exports_[name] = value;
}

void ModuleNode::setModuleType(const std::string& type) {
    if (type == "amd" || type == "commonjs" || type == "es6") {
        moduleType_ = type;
    } else {
        moduleType_ = "amd"; // 默认
    }
}

// 解析模块路径，转换为合法的变量名
std::string ModuleNode::toVariableName(const std::string& path) {
    std::string varName;
    
    // 提取最后一部分作为基础名
    size_t lastSlash = path.find_last_of("/");
    std::string baseName = (lastSlash != std::string::npos) 
                          ? path.substr(lastSlash + 1) 
                          : path;
    
    // 移除扩展名
    size_t dotPos = baseName.find_last_of(".");
    if (dotPos != std::string::npos) {
        baseName = baseName.substr(0, dotPos);
    }
    
    // 转换为合法的JS变量名
    for (char c : baseName) {
        if (std::isalnum(c) || c == '_') {
            varName += c;
        } else if (c == '-') {
            varName += '_';
        }
    }
    
    // 确保不以数字开头
    if (!varName.empty() && std::isdigit(varName[0])) {
        varName = "_" + varName;
    }
    
    return varName.empty() ? "module" : varName;
}

// 检查是否为相对路径
bool ModuleNode::isRelativePath(const std::string& path) {
    return path.starts_with("./") || 
           path.starts_with("../") || 
           path.starts_with("/");
}

// 规范化路径
std::string ModuleNode::normalizePath(const std::string& path) {
    std::string normalized = path;
    
    // 处理反斜杠
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // 移除多余的斜杠
    size_t pos = 0;
    while ((pos = normalized.find("//", pos)) != std::string::npos) {
        normalized.replace(pos, 2, "/");
    }
    
    return normalized;
}

} // namespace CHTL