#include "CHTLContext.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLContext::CHTLContext(std::shared_ptr<GlobalMap> globalMap)
    : globalMap_(globalMap) {
    if (!globalMap_) {
        globalMap_ = std::make_shared<GlobalMap>();
    }
    
    // 初始化全局上下文
    pushContext(ContextType::GLOBAL);
}

void CHTLContext::pushContext(ContextType type, std::shared_ptr<BaseNode> node, 
                             const std::string& name) {
    size_t depth = contextStack_.size();
    contextStack_.emplace(type, node, name, depth);
}

void CHTLContext::popContext() {
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
}

Context CHTLContext::getCurrentContext() const {
    if (!contextStack_.empty()) {
        return contextStack_.top();
    }
    return Context(ContextType::GLOBAL);
}

Context CHTLContext::getParentContext() const {
    if (contextStack_.size() >= 2) {
        auto current = contextStack_;
        current.pop();
        return current.top();
    }
    return Context(ContextType::GLOBAL);
}

size_t CHTLContext::getContextDepth() const {
    return contextStack_.size();
}

bool CHTLContext::hasContext() const {
    return !contextStack_.empty();
}

bool CHTLContext::isInContext(ContextType type) const {
    std::stack<Context> temp = contextStack_;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            return true;
        }
        temp.pop();
    }
    return false;
}

std::shared_ptr<BaseNode> CHTLContext::findContextNode(ContextType type) const {
    std::stack<Context> temp = contextStack_;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            return temp.top().node;
        }
        temp.pop();
    }
    return nullptr;
}

std::string CHTLContext::findContextName(ContextType type) const {
    std::stack<Context> temp = contextStack_;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            return temp.top().name;
        }
        temp.pop();
    }
    return "";
}

void CHTLContext::addConstraint(const std::string& constraint) {
    constraints_.push_back(constraint);
}

void CHTLContext::removeConstraint(const std::string& constraint) {
    constraints_.erase(
        std::remove(constraints_.begin(), constraints_.end(), constraint),
        constraints_.end()
    );
}

bool CHTLContext::hasConstraint(const std::string& constraint) const {
    return std::find(constraints_.begin(), constraints_.end(), constraint) != constraints_.end();
}

std::vector<std::string> CHTLContext::getConstraints() const {
    return constraints_;
}

void CHTLContext::clearConstraints() {
    constraints_.clear();
}

bool CHTLContext::canUseElement(const std::string& elementName) const {
    // 检查是否被约束禁用
    if (hasConstraint("@Html")) {
        return false;
    }
    
    if (hasConstraint(elementName)) {
        return false;
    }
    
    return true;
}

bool CHTLContext::canUseTemplate(const std::string& templateType) const {
    // 检查模板约束
    if (hasConstraint("[Template]")) {
        return false;
    }
    
    if (hasConstraint("[Template] " + templateType)) {
        return false;
    }
    
    return true;
}

bool CHTLContext::canUseCustom(const std::string& customType) const {
    // 检查自定义约束
    if (hasConstraint("[Custom]")) {
        return false;
    }
    
    if (hasConstraint("[Custom] " + customType)) {
        return false;
    }
    
    return true;
}

bool CHTLContext::canUseOrigin() const {
    // 原始嵌入在任何地方都可以使用（按照文档说明）
    return true;
}

void CHTLContext::setCurrentFile(const std::string& filename) {
    currentFile_ = filename;
}

void CHTLContext::reportError(const std::string& message, const Position& pos) {
    errors_.push_back(formatError(message, pos));
}

void CHTLContext::reportWarning(const std::string& message, const Position& pos) {
    warnings_.push_back(formatWarning(message, pos));
}

void CHTLContext::reset() {
    while (!contextStack_.empty()) {
        contextStack_.pop();
    }
    pushContext(ContextType::GLOBAL);
    
    constraints_.clear();
    errors_.clear();
    warnings_.clear();
    currentFile_.clear();
}

std::string CHTLContext::formatError(const std::string& message, const Position& pos) const {
    std::ostringstream oss;
    if (!currentFile_.empty()) {
        oss << currentFile_ << ":";
    }
    oss << pos.line << ":" << pos.column << ": error: " << message;
    return oss.str();
}

std::string CHTLContext::formatWarning(const std::string& message, const Position& pos) const {
    std::ostringstream oss;
    if (!currentFile_.empty()) {
        oss << currentFile_ << ":";
    }
    oss << pos.line << ":" << pos.column << ": warning: " << message;
    return oss.str();
}

std::string contextTypeToString(ContextType type) {
    switch (type) {
        case ContextType::GLOBAL: return "GLOBAL";
        case ContextType::ELEMENT: return "ELEMENT";
        case ContextType::TEMPLATE: return "TEMPLATE";
        case ContextType::CUSTOM: return "CUSTOM";
        case ContextType::STYLE: return "STYLE";
        case ContextType::SCRIPT: return "SCRIPT";
        case ContextType::ORIGIN: return "ORIGIN";
        case ContextType::IMPORT: return "IMPORT";
        case ContextType::CONFIG: return "CONFIG";
        case ContextType::NAMESPACE: return "NAMESPACE";
        default: return "UNKNOWN";
    }
}

// ContextGuard 实现
ContextGuard::ContextGuard(CHTLContext& context, ContextType type, 
                          std::shared_ptr<BaseNode> node, const std::string& name)
    : context_(context) {
    context_.pushContext(type, node, name);
}

ContextGuard::~ContextGuard() {
    context_.popContext();
}

} // namespace CHTL