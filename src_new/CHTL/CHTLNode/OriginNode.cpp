#include "OriginNode.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL {

// ========== OriginNode 实现 ==========

OriginNode::OriginNode(OriginType type, const std::string& typeName, const std::string& name)
    : BaseNode(determineNodeType(), name), m_originType(type), m_typeName(typeName), m_originName(name) {
}

OriginType OriginNode::getOriginType() const {
    return m_originType;
}

const std::string& OriginNode::getTypeName() const {
    return m_typeName;
}

void OriginNode::setTypeName(const std::string& typeName) {
    m_typeName = typeName;
}

const std::string& OriginNode::getOriginName() const {
    return m_originName;
}

void OriginNode::setOriginName(const std::string& name) {
    m_originName = name;
}

const std::string& OriginNode::getContent() const {
    return m_content;
}

void OriginNode::setContent(const std::string& content) {
    m_content = content;
}

void OriginNode::addContentLine(const std::string& line) {
    if (!m_content.empty()) {
        m_content += "\n";
    }
    m_content += line;
}

bool OriginNode::isNamed() const {
    return !m_originName.empty();
}

bool OriginNode::isCustomType() const {
    return m_originType == OriginType::CUSTOM;
}

std::string OriginNode::getGeneratorOutputType() const {
    switch (m_originType) {
        case OriginType::HTML: return "html";
        case OriginType::STYLE: return "css";
        case OriginType::JAVASCRIPT: return "javascript";
        case OriginType::CUSTOM: {
            // 移除@前缀并转为小写
            std::string type = m_typeName;
            if (type.length() > 1 && type[0] == '@') {
                type = type.substr(1);
            }
            std::transform(type.begin(), type.end(), type.begin(), ::tolower);
            return type;
        }
        default: return "unknown";
    }
}

CHTLNodeType OriginNode::determineNodeType() const {
    switch (m_originType) {
        case OriginType::HTML: return CHTLNodeType::ORIGIN_HTML_NODE;
        case OriginType::STYLE: return CHTLNodeType::ORIGIN_STYLE_NODE;
        case OriginType::JAVASCRIPT: return CHTLNodeType::ORIGIN_JAVASCRIPT_NODE;
        case OriginType::CUSTOM: return CHTLNodeType::ORIGIN_CUSTOM_NODE;
        default: return CHTLNodeType::ORIGIN_HTML_NODE;
    }
}

std::string OriginNode::toString() const {
    std::stringstream ss;
    
    // 输出原始嵌入定义
    ss << "[Origin] " << m_typeName;
    
    if (isNamed()) {
        ss << " " << m_originName;
    }
    
    ss << " {\n";
    
    // 输出内容，保持原样
    if (!m_content.empty()) {
        ss << m_content;
        if (m_content.back() != '\n') {
            ss << "\n";
        }
    }
    
    ss << "}";
    
    return ss.str();
}

NodePtr OriginNode::clone() const {
    auto cloned = std::make_shared<OriginNode>(m_originType, m_typeName, m_originName);
    cloned->setContent(m_content);
    return cloned;
}

bool OriginNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    
    // 检查类型名称
    if (m_typeName.empty()) {
        return false;
    }
    
    // 检查类型名称格式（必须以@开头）
    if (m_typeName[0] != '@') {
        return false;
    }
    
    // 自定义类型需要有内容或名称
    if (m_originType == OriginType::CUSTOM && m_content.empty() && m_originName.empty()) {
        return false;
    }
    
    return true;
}

// ========== OriginUsageNode 实现 ==========

OriginUsageNode::OriginUsageNode(const std::string& typeName, const std::string& name)
    : BaseNode(CHTLNodeType::ORIGIN_HTML_NODE, ""), m_typeName(typeName), m_referenceName(name), m_fullyQualified(false) {
}

const std::string& OriginUsageNode::getTypeName() const {
    return m_typeName;
}

const std::string& OriginUsageNode::getReferenceName() const {
    return m_referenceName;
}

void OriginUsageNode::setFullyQualified(bool fullyQualified) {
    m_fullyQualified = fullyQualified;
}

bool OriginUsageNode::isFullyQualified() const {
    return m_fullyQualified;
}

std::string OriginUsageNode::toString() const {
    std::stringstream ss;
    
    if (m_fullyQualified) {
        ss << "[Origin] ";
    }
    
    ss << m_typeName << " " << m_referenceName << ";";
    
    return ss.str();
}

NodePtr OriginUsageNode::clone() const {
    auto cloned = std::make_shared<OriginUsageNode>(m_typeName, m_referenceName);
    cloned->setFullyQualified(m_fullyQualified);
    return cloned;
}

bool OriginUsageNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    
    // 检查类型名称和引用名称
    return !m_typeName.empty() && !m_referenceName.empty();
}

} // namespace CHTL