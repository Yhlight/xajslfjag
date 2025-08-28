#include "CHTLTextNode.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace CHTL {

CHTLTextNode::CHTLTextNode(const std::string& text) 
    : CHTLBaseNode(CHTLNodeType::TEXT), text(text) {
}

std::string CHTLTextNode::getProcessedText() const {
    std::string processed = text;
    
    // 处理转义字符
    std::string result;
    result.reserve(processed.length());
    
    bool escaped = false;
    for (size_t i = 0; i < processed.length(); ++i) {
        char c = processed[i];
        
        if (escaped) {
            switch (c) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'v': result += '\v'; break;
                case '0': result += '\0'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                default: 
                    result += '\\';
                    result += c;
                    break;
            }
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else {
            result += c;
        }
    }
    
    return result;
}

void CHTLTextNode::appendText(const std::string& appendText) {
    text += appendText;
}

void CHTLTextNode::prependText(const std::string& prependText) {
    text = prependText + text;
}

bool CHTLTextNode::isWhitespaceOnly() const {
    return std::all_of(text.begin(), text.end(), [](char c) {
        return std::isspace(c);
    });
}

std::string CHTLTextNode::getTrimmedText() const {
    std::string trimmed = text;
    
    // 移除前导空白
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](char c) {
        return !std::isspace(c);
    }));
    
    // 移除尾随空白
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](char c) {
        return !std::isspace(c);
    }).base(), trimmed.end());
    
    return trimmed;
}

void CHTLTextNode::trimText() {
    text = getTrimmedText();
}

std::string CHTLTextNode::toHTML() const {
    return escapeHTML(getProcessedText());
}

std::string CHTLTextNode::toPlainText() const {
    return getProcessedText();
}

std::string CHTLTextNode::escapeHTML(const std::string& input) const {
    static const std::unordered_map<char, std::string> htmlEscapes = {
        {'&', "&amp;"},
        {'<', "&lt;"},
        {'>', "&gt;"},
        {'"', "&quot;"},
        {'\'', "&#39;"}
    };
    
    std::string result;
    result.reserve(input.length() * 1.2); // 预留一些空间用于转义字符
    
    for (char c : input) {
        auto it = htmlEscapes.find(c);
        if (it != htmlEscapes.end()) {
            result += it->second;
        } else {
            result += c;
        }
    }
    
    return result;
}

std::string CHTLTextNode::unescapeHTML(const std::string& input) const {
    static const std::unordered_map<std::string, char> htmlUnescapes = {
        {"&amp;", '&'},
        {"&lt;", '<'},
        {"&gt;", '>'},
        {"&quot;", '"'},
        {"&#39;", '\''},
        {"&apos;", '\''}
    };
    
    std::string result;
    result.reserve(input.length());
    
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '&') {
            bool found = false;
            
            // 尝试匹配HTML实体
            for (const auto& unescape : htmlUnescapes) {
                const std::string& entity = unescape.first;
                if (i + entity.length() <= input.length() && 
                    input.substr(i, entity.length()) == entity) {
                    result += unescape.second;
                    i += entity.length() - 1; // -1 因为for循环会自动+1
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                result += input[i];
            }
        } else {
            result += input[i];
        }
    }
    
    return result;
}

void CHTLTextNode::accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

std::shared_ptr<CHTLBaseNode> CHTLTextNode::clone() const {
    auto cloned = std::make_shared<CHTLTextNode>(text);
    
    // 复制基础属性
    cloned->attributes = attributes;
    cloned->sourceLine = sourceLine;
    cloned->sourceColumn = sourceColumn;
    cloned->token = token;
    
    return cloned;
}

std::string CHTLTextNode::toString() const {
    std::ostringstream oss;
    oss << "TextNode[\"";
    
    // 显示文本内容的前50个字符
    std::string displayText = text;
    if (displayText.length() > 50) {
        displayText = displayText.substr(0, 47) + "...";
    }
    
    // 转义换行符等用于显示
    std::string escaped;
    for (char c : displayText) {
        switch (c) {
            case '\n': escaped += "\\n"; break;
            case '\t': escaped += "\\t"; break;
            case '\r': escaped += "\\r"; break;
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            default: escaped += c; break;
        }
    }
    
    oss << escaped << "\"]";
    
    if (sourceLine > 0) {
        oss << " @" << sourceLine << ":" << sourceColumn;
    }
    
    return oss.str();
}

} // namespace CHTL