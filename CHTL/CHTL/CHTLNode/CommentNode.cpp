#include "CommentNode.hpp"
#include "../CHTLState/CHTLState.hpp"
#include <sstream>

namespace CHTL {

CommentNode::CommentNode(CommentType type, const std::string& content)
    : BaseNode(NodeType::COMMENT, "comment"), commentType(type), commentContent(content),
      shouldGenerate(type == CommentType::GENERATOR), targetLanguage("") {
}

CommentType CommentNode::getCommentType() const {
    return commentType;
}

void CommentNode::setCommentType(CommentType type) {
    commentType = type;
    shouldGenerate = (type == CommentType::GENERATOR);
}

const std::string& CommentNode::getCommentContent() const {
    return commentContent;
}

void CommentNode::setCommentContent(const std::string& content) {
    commentContent = content;
}

bool CommentNode::getShouldGenerate() const {
    return shouldGenerate;
}

void CommentNode::setShouldGenerate(bool generate) {
    shouldGenerate = generate;
}

const std::string& CommentNode::getTargetLanguage() const {
    return targetLanguage;
}

void CommentNode::setTargetLanguage(const std::string& language) {
    targetLanguage = language;
}

std::string CommentNode::generateComment(const std::string& language) const {
    if (!shouldGenerate) {
        return "";
    }
    
    if (language == "html") {
        return "<!-- " + commentContent + " -->";
    } else if (language == "css") {
        return "/* " + commentContent + " */";
    } else if (language == "javascript" || language == "js") {
        if (commentType == CommentType::SINGLE_LINE) {
            return "// " + commentContent;
        } else {
            return "/* " + commentContent + " */";
        }
    } else if (language == "cpp" || language == "c++") {
        if (commentType == CommentType::SINGLE_LINE) {
            return "// " + commentContent;
        } else {
            return "/* " + commentContent + " */";
        }
    }
    
    // 默认返回HTML注释
    return "<!-- " + commentContent + " -->";
}

std::string CommentNode::toString() const {
    std::stringstream ss;
    ss << "CommentNode{type=";
    switch (commentType) {
        case CommentType::SINGLE_LINE:
            ss << "SingleLine";
            break;
        case CommentType::MULTI_LINE:
            ss << "MultiLine";
            break;
        case CommentType::GENERATOR:
            ss << "Generator";
            break;
    }
    ss << ", content=\"" << commentContent << "\"";
    if (shouldGenerate) {
        ss << ", generate=true";
    }
    if (!targetLanguage.empty()) {
        ss << ", target=" << targetLanguage;
    }
    ss << "}";
    return ss.str();
}

std::string CommentNode::toHTML() const {
    if (shouldGenerate && commentType == CommentType::GENERATOR) {
        return generateComment("html");
    }
    return ""; // 普通注释不生成到HTML中
}

std::string CommentNode::toCSS() const {
    if (shouldGenerate && commentType == CommentType::GENERATOR) {
        return generateComment("css");
    }
    return "";
}

std::string CommentNode::toJavaScript() const {
    if (shouldGenerate && commentType == CommentType::GENERATOR) {
        return generateComment("javascript");
    }
    return "";
}

void CommentNode::validate(CHTLState& state) {
    BaseNode::validate(state);
    
    // 注释内容验证
    if (commentContent.empty() && commentType != CommentType::GENERATOR) {
        setMetadata("warning", "注释内容为空");
    }
    
    // 生成器注释的特殊验证
    if (commentType == CommentType::GENERATOR) {
        shouldGenerate = true;
        
        // 根据当前上下文确定目标语言
        auto currentContext = state.getCurrentContext();
        switch (currentContext) {
            case ContextType::LOCAL_STYLE:
            case ContextType::TEMPLATE_STYLE:
            case ContextType::CUSTOM_STYLE:
                targetLanguage = "css";
                break;
            case ContextType::LOCAL_SCRIPT:
                targetLanguage = "javascript";
                break;
            default:
                targetLanguage = "html";
                break;
        }
    }
}

std::shared_ptr<BaseNode> CommentNode::clone() const {
    auto cloned = std::make_shared<CommentNode>(commentType, commentContent);
    
    // 复制基类数据
    cloned->setPosition(getLine(), getColumn(), getPosition());
    cloned->setValid(getIsValid());
    cloned->setErrorMessage(getErrorMessage());
    
    // 复制CommentNode特有数据
    cloned->shouldGenerate = shouldGenerate;
    cloned->targetLanguage = targetLanguage;
    
    return cloned;
}

}