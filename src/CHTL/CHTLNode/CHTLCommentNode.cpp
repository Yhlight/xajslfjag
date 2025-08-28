#include "CHTLCommentNode.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL {

CHTLCommentNode::CHTLCommentNode(const std::string& content, CHTLCommentType type) 
    : CHTLBaseNode(CHTLNodeType::COMMENT), content(content), commentType(type) {
}

std::string CHTLCommentNode::getCommentTypeName() const {
    switch (commentType) {
        case CHTLCommentType::SINGLE_LINE: return "SingleLine";
        case CHTLCommentType::MULTI_LINE: return "MultiLine";
        case CHTLCommentType::GENERATOR: return "Generator";
        default: return "Unknown";
    }
}

std::string CHTLCommentNode::getProcessedContent() const {
    std::string processed = content;
    
    // 移除注释标记符号
    switch (commentType) {
        case CHTLCommentType::SINGLE_LINE:
            if (processed.length() >= 2 && processed.substr(0, 2) == "//") {
                processed = processed.substr(2);
            }
            break;
        case CHTLCommentType::MULTI_LINE:
            if (processed.length() >= 4 && 
                processed.substr(0, 2) == "/*" && 
                processed.substr(processed.length() - 2) == "*/") {
                processed = processed.substr(2, processed.length() - 4);
            }
            break;
        case CHTLCommentType::GENERATOR:
            if (processed.length() >= 2 && processed.substr(0, 2) == "--") {
                processed = processed.substr(2);
            }
            break;
    }
    
    return processed;
}

void CHTLCommentNode::appendContent(const std::string& appendContent) {
    content += appendContent;
}

void CHTLCommentNode::prependContent(const std::string& prependContent) {
    content = prependContent + content;
}

bool CHTLCommentNode::isWhitespaceOnly() const {
    std::string processed = getProcessedContent();
    return std::all_of(processed.begin(), processed.end(), [](char c) {
        return std::isspace(c);
    });
}

std::string CHTLCommentNode::getTrimmedContent() const {
    std::string trimmed = getProcessedContent();
    
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

void CHTLCommentNode::trimContent() {
    content = getTrimmedContent();
}

std::string CHTLCommentNode::toHTML() const {
    if (commentType == CHTLCommentType::GENERATOR) {
        return generateContextualComment("html");
    }
    
    std::string processedContent = getProcessedContent();
    return "<!-- " + processedContent + " -->";
}

std::string CHTLCommentNode::toCSS() const {
    if (commentType == CHTLCommentType::GENERATOR) {
        return generateContextualComment("css");
    }
    
    std::string processedContent = getProcessedContent();
    
    if (commentType == CHTLCommentType::SINGLE_LINE) {
        // CSS不支持单行注释，转换为多行注释
        return "/* " + processedContent + " */";
    } else {
        return "/* " + processedContent + " */";
    }
}

std::string CHTLCommentNode::toJavaScript() const {
    if (commentType == CHTLCommentType::GENERATOR) {
        return generateContextualComment("javascript");
    }
    
    std::string processedContent = getProcessedContent();
    
    switch (commentType) {
        case CHTLCommentType::SINGLE_LINE:
            return "// " + processedContent;
        case CHTLCommentType::MULTI_LINE:
            return "/* " + processedContent + " */";
        default:
            return "// " + processedContent;
    }
}

std::string CHTLCommentNode::toOriginalFormat() const {
    return content; // 返回原始格式
}

std::string CHTLCommentNode::generateContextualComment(const std::string& context) const {
    std::string processedContent = getProcessedContent();
    
    if (context == "html") {
        return "<!-- " + processedContent + " -->";
    } else if (context == "css") {
        return "/* " + processedContent + " */";
    } else if (context == "javascript" || context == "js") {
        // 检查内容是否包含多行，如果是，使用多行注释
        if (processedContent.find('\n') != std::string::npos) {
            return "/* " + processedContent + " */";
        } else {
            return "// " + processedContent;
        }
    } else {
        // 默认使用原格式
        return content;
    }
}

std::string CHTLCommentNode::escapeComment(const std::string& input, const std::string& format) const {
    std::string result = input;
    
    if (format == "html") {
        // HTML注释中不能包含 "--"
        std::string::size_type pos = 0;
        while ((pos = result.find("--", pos)) != std::string::npos) {
            result.replace(pos, 2, "- -");
            pos += 3;
        }
    } else if (format == "css" || format == "javascript") {
        // 多行注释中不能包含 "*/"
        std::string::size_type pos = 0;
        while ((pos = result.find("*/", pos)) != std::string::npos) {
            result.replace(pos, 2, "* /");
            pos += 3;
        }
    }
    
    return result;
}

std::vector<std::string> CHTLCommentNode::splitLines(const std::string& input) const {
    std::vector<std::string> lines;
    std::istringstream stream(input);
    std::string line;
    
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

void CHTLCommentNode::accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

std::shared_ptr<CHTLBaseNode> CHTLCommentNode::clone() const {
    auto cloned = std::make_shared<CHTLCommentNode>(content, commentType);
    
    // 复制基础属性
    cloned->attributes = attributes;
    cloned->sourceLine = sourceLine;
    cloned->sourceColumn = sourceColumn;
    cloned->token = token;
    
    return cloned;
}

std::string CHTLCommentNode::toString() const {
    std::ostringstream oss;
    oss << "CommentNode[" << getCommentTypeName() << ": \"";
    
    // 显示注释内容的前30个字符
    std::string displayContent = getProcessedContent();
    if (displayContent.length() > 30) {
        displayContent = displayContent.substr(0, 27) + "...";
    }
    
    // 转义换行符等用于显示
    std::string escaped;
    for (char c : displayContent) {
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