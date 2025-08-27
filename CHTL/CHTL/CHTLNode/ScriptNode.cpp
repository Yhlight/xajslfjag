#include "ScriptNode.h"
#include <sstream>

namespace CHTL {

ScriptNode::ScriptNode(bool isLocal)
    : BaseNode(NodeType::SCRIPT), isLocal(isLocal) {
}

std::string ScriptNode::Generate() const {
    if (isLocal) {
        // 局部脚本需要包装
        return WrapLocalScript();
    } else {
        // 全局脚本
        std::stringstream ss;
        ss << "<script>\n";
        ss << content;
        ss << "\n</script>\n";
        return ss.str();
    }
}

NodePtr ScriptNode::Clone() const {
    auto clone = std::make_shared<ScriptNode>(isLocal);
    clone->SetPosition(line, column);
    clone->content = content;
    return clone;
}

void ScriptNode::Accept(NodeVisitor* visitor) {
    visitor->VisitScriptNode(this);
}

std::vector<std::string> ScriptNode::ExtractClassSelectors() const {
    // 查找 {{.className}} 模式
    return ExtractSelectorsWithPattern(R"(\{\{\.([a-zA-Z_][\w-]*)\}\})");
}

std::vector<std::string> ScriptNode::ExtractIdSelectors() const {
    // 查找 {{#idName}} 模式
    return ExtractSelectorsWithPattern(R"(\{\{#([a-zA-Z_][\w-]*)\}\})");
}

std::string ScriptNode::GetFirstClassSelector() const {
    auto selectors = ExtractClassSelectors();
    return selectors.empty() ? "" : selectors[0];
}

std::string ScriptNode::GetFirstIdSelector() const {
    auto selectors = ExtractIdSelectors();
    return selectors.empty() ? "" : selectors[0];
}

bool ScriptNode::ContainsCHTLJSFeatures() const {
    // 检查CHTL JS特征
    if (content.find("{{") != std::string::npos && content.find("}}") != std::string::npos) {
        return true;
    }
    
    if (content.find("->") != std::string::npos) {
        // 需要更精确的检查，避免字符串中的->
        // TODO: 实现更精确的检测
        return true;
    }
    
    // 检查CHTL JS函数
    std::regex chtljsFunctions(R"(\b(listen|delegate|animate|vir)\s*\{)");
    if (std::regex_search(content, chtljsFunctions)) {
        return true;
    }
    
    if (content.find("&->") != std::string::npos) {
        return true;
    }
    
    return false;
}

std::string ScriptNode::WrapLocalScript() const {
    // 局部脚本包装成立即执行函数，避免全局污染
    std::stringstream ss;
    ss << "<script>\n";
    ss << "(function() {\n";
    ss << "    'use strict';\n";
    
    // 添加缩进
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        ss << "    " << line << "\n";
    }
    
    ss << "})();\n";
    ss << "</script>\n";
    
    return ss.str();
}

std::vector<std::string> ScriptNode::ExtractSelectorsWithPattern(const std::string& pattern) const {
    std::vector<std::string> selectors;
    std::regex selectorRegex(pattern);
    
    auto begin = std::sregex_iterator(content.begin(), content.end(), selectorRegex);
    auto end = std::sregex_iterator();
    
    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        if (match.size() > 1) {
            selectors.push_back(match[1].str());
        }
    }
    
    return selectors;
}

} // namespace CHTL