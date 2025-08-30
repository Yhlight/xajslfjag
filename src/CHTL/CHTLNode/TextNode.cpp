#include "TextNode.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {

TextNode::TextNode(const std::string& content)
    : BaseNode(CHTLNodeType::TEXT_NODE, "text")
    , m_content(content)
    , m_rawContent(content)
    , m_isLiteral(false)
    , m_isDoubleQuoted(false)
    , m_isSingleQuoted(false) {
    setValue(content);
}

const std::string& TextNode::getContent() const {
    return m_content;
}

void TextNode::setContent(const std::string& content) {
    m_content = content;
    setValue(content);
}

void TextNode::appendContent(const std::string& content) {
    m_content += content;
    setValue(m_content);
}

const std::string& TextNode::getRawContent() const {
    return m_rawContent;
}

void TextNode::setRawContent(const std::string& rawContent) {
    m_rawContent = rawContent;
    m_content = processEscapeSequences(rawContent);
    setValue(m_content);
}

bool TextNode::isLiteral() const {
    return m_isLiteral;
}

void TextNode::setLiteral(bool literal) {
    m_isLiteral = literal;
    if (literal) {
        m_isDoubleQuoted = false;
        m_isSingleQuoted = false;
    }
}

bool TextNode::isDoubleQuoted() const {
    return m_isDoubleQuoted;
}

void TextNode::setDoubleQuoted(bool doubleQuoted) {
    m_isDoubleQuoted = doubleQuoted;
    if (doubleQuoted) {
        m_isLiteral = false;
        m_isSingleQuoted = false;
    }
}

bool TextNode::isSingleQuoted() const {
    return m_isSingleQuoted;
}

void TextNode::setSingleQuoted(bool singleQuoted) {
    m_isSingleQuoted = singleQuoted;
    if (singleQuoted) {
        m_isLiteral = false;
        m_isDoubleQuoted = false;
    }
}

std::string TextNode::getProcessedContent() const {
    return processEscapeSequences(m_content);
}

bool TextNode::hasVariables() const {
    // 检查是否包含变量模式，如 ThemeColor(tableColor)
    std::regex variablePattern(R"(\w+\s*\(\s*\w+\s*\))");
    return std::regex_search(m_content, variablePattern);
}

std::vector<std::string> TextNode::getVariables() const {
    std::vector<std::string> variables;
    std::regex variablePattern(R"((\w+)\s*\(\s*(\w+)\s*\))");
    std::sregex_iterator iter(m_content.begin(), m_content.end(), variablePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        variables.push_back(match.str());
    }
    
    return variables;
}

void TextNode::replaceVariable(const std::string& varName, const std::string& value) {
    // 替换变量引用
    std::regex varPattern(varName);
    m_content = std::regex_replace(m_content, varPattern, value);
    setValue(m_content);
}

size_t TextNode::getLength() const {
    return m_content.length();
}

bool TextNode::isWhitespace() const {
    return std::all_of(m_content.begin(), m_content.end(), 
                      [](char c) { return std::isspace(c); });
}

void TextNode::trim() {
    // 移除开头和结尾的空白字符
    auto start = m_content.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        m_content.clear();
    } else {
        auto end = m_content.find_last_not_of(" \t\n\r\f\v");
        m_content = m_content.substr(start, end - start + 1);
    }
    setValue(m_content);
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(m_content);
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_rawContent = m_rawContent;
    cloned->m_isLiteral = m_isLiteral;
    cloned->m_isDoubleQuoted = m_isDoubleQuoted;
    cloned->m_isSingleQuoted = m_isSingleQuoted;
    return cloned;
}

std::shared_ptr<BaseNode> TextNode::deepClone() const {
    // 文本节点没有子节点，深度克隆等同于克隆
    return clone();
}

std::string TextNode::toString() const {
    std::stringstream ss;
    ss << "Text";
    
    if (m_isLiteral) {
        ss << "(literal)";
    } else if (m_isDoubleQuoted) {
        ss << "(double-quoted)";
    } else if (m_isSingleQuoted) {
        ss << "(single-quoted)";
    }
    
    ss << ": \"" << m_content << "\"";
    return ss.str();
}

std::string TextNode::toHtmlText() const {
    return htmlEscape(getProcessedContent());
}

bool TextNode::validate(ErrorReporter* errorReporter) const {
    if (!BaseNode::validate(errorReporter)) {
        return false;
    }
    
    return internalValidate(errorReporter);
}

bool TextNode::internalValidate(ErrorReporter* errorReporter) const {
    // 验证引号类型的一致性
    int quoteTypes = 0;
    if (m_isLiteral) quoteTypes++;
    if (m_isDoubleQuoted) quoteTypes++;
    if (m_isSingleQuoted) quoteTypes++;
    
    if (quoteTypes > 1) {
        if (errorReporter) {
            ErrorPosition pos("", m_position.line, m_position.column);
            errorReporter->error(ErrorType::SEMANTIC_ERROR, 
                               "Text node has conflicting quote types", pos);
        }
        return false;
    }
    
    // 验证转义序列
    if ((m_isDoubleQuoted || m_isSingleQuoted) && m_rawContent != m_content) {
        try {
            processEscapeSequences(m_rawContent);
        } catch (const std::exception& e) {
            if (errorReporter) {
                ErrorPosition pos("", m_position.line, m_position.column);
                errorReporter->error(ErrorType::INVALID_ESCAPE_SEQUENCE, 
                                   "Invalid escape sequence in text: " + std::string(e.what()), pos);
            }
            return false;
        }
    }
    
    return true;
}

std::string TextNode::processEscapeSequences(const std::string& text) const {
    std::string result;
    result.reserve(text.length());
    
    for (size_t i = 0; i < text.length(); ++i) {
        if (text[i] == '\\' && i + 1 < text.length()) {
            char nextChar = text[i + 1];
            switch (nextChar) {
                case 'n': result += '\n'; ++i; break;
                case 't': result += '\t'; ++i; break;
                case 'r': result += '\r'; ++i; break;
                case '\\': result += '\\'; ++i; break;
                case '"': result += '"'; ++i; break;
                case '\'': result += '\''; ++i; break;
                case '0': result += '\0'; ++i; break;
                default: 
                    result += text[i];
                    break;
            }
        } else {
            result += text[i];
        }
    }
    
    return result;
}

std::string TextNode::htmlEscape(const std::string& text) const {
    std::string result;
    result.reserve(text.length() * 1.1); // 预留一些空间给转义字符
    
    for (char c : text) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    
    return result;
}

} // namespace CHTL