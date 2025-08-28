#include "ConstraintParser.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <regex>

namespace CHTL {

ConstraintParser::ConstraintParser() {
    clearStatistics();
}

std::vector<ConstraintGroup> ConstraintParser::parse(const std::string& source, const std::string& sourceFile) {
    clearErrors();
    clearStatistics();
    
    std::vector<ConstraintGroup> groups;
    
    if (source.empty()) {
        return groups;
    }
    
    std::istringstream iss(source);
    std::string line;
    size_t lineNumber = 1;
    
    while (std::getline(iss, line)) {
        if (isConstraintStatement(line)) {
            auto group = parseSingleConstraintStatement(line, lineNumber, 1, sourceFile);
            if (!group.constraints.empty()) {
                groups.push_back(group);
                statistics_.totalConstraints += group.constraints.size();
            }
        }
        lineNumber++;
    }
    
    return groups;
}

ConstraintGroup ConstraintParser::parseSingleConstraintStatement(const std::string& line, size_t lineNumber,
                                                               size_t columnNumber, const std::string& sourceFile) {
    ConstraintGroup group(lineNumber, columnNumber, sourceFile);
    
    if (isExceptClause(line)) {
        auto constraints = parseExceptClause(line, lineNumber, columnNumber, sourceFile);
        group.constraints = constraints;
        
        // 更新统计
        for (const auto& constraint : constraints) {
            switch (constraint.type) {
                case ConstraintType::ELEMENT:
                    statistics_.elementConstraints++;
                    break;
                case ConstraintType::TYPE:
                    statistics_.typeConstraints++;
                    break;
                case ConstraintType::GLOBAL:
                    statistics_.globalConstraints++;
                    break;
            }
        }
    }
    
    return group;
}

std::vector<ConstraintInfo> ConstraintParser::parseExceptClause(const std::string& text, size_t line,
                                                               size_t column, const std::string& sourceFile) {
    std::vector<ConstraintInfo> constraints;
    
    // 查找 "except" 关键字
    size_t pos = text.find("except");
    if (pos == std::string::npos) {
        return constraints;
    }
    
    pos += 6; // 跳过 "except"
    
    // 跳过空白字符
    while (pos < text.length() && std::isspace(text[pos])) {
        pos++;
    }
    
    // 解析约束列表
    std::string currentConstraint;
    while (pos < text.length()) {
        char ch = text[pos];
        
        if (ch == ',' || ch == ';' || ch == '{') {
            // 处理当前约束
            if (!currentConstraint.empty()) {
                auto constraint = parseConstraintText(currentConstraint, line, column, sourceFile);
                if (constraint.type != ConstraintType::GLOBAL) {
                    constraints.push_back(constraint);
                }
                currentConstraint.clear();
            }
            
            if (ch == ';' || ch == '{') {
                break;
            }
        } else {
            currentConstraint += ch;
        }
        
        pos++;
    }
    
    // 处理最后一个约束
    if (!currentConstraint.empty()) {
        auto constraint = parseConstraintText(currentConstraint, line, column, sourceFile);
        if (constraint.type != ConstraintType::GLOBAL) {
            constraints.push_back(constraint);
        }
    }
    
    return constraints;
}

ConstraintInfo ConstraintParser::parseConstraintText(const std::string& text, size_t line, size_t column, const std::string& sourceFile) {
    std::string cleanText = text;
    
    // 去除首尾空白字符
    cleanText.erase(0, cleanText.find_first_not_of(" \t"));
    cleanText.erase(cleanText.find_last_not_of(" \t") + 1);
    
    if (cleanText.empty()) {
        return ConstraintInfo(ConstraintType::GLOBAL, "", line, column, sourceFile);
    }
    
    // 确定约束类型
    ConstraintType type = getConstraintType(cleanText);
    std::string value = extractConstraintValue(cleanText, type);
    
    return ConstraintInfo(type, value, line, column, sourceFile);
}

bool ConstraintParser::validateConstraint(const ConstraintInfo& constraint) {
    if (constraint.value.empty()) {
        return false;
    }
    
    switch (constraint.type) {
        case ConstraintType::ELEMENT:
            // 验证HTML元素名称
            return isValidHTMLElement(constraint.value);
        case ConstraintType::TYPE:
            // 验证类型名称
            return isValidTypeName(constraint.value);
        case ConstraintType::GLOBAL:
            // 全局约束总是有效的
            return true;
        default:
            return false;
    }
}

std::vector<std::string> ConstraintParser::getValidationErrors(const ConstraintInfo& constraint) {
    std::vector<std::string> errors;
    
    if (constraint.value.empty()) {
        errors.push_back("约束值不能为空");
        return errors;
    }
    
    switch (constraint.type) {
        case ConstraintType::ELEMENT:
            if (!isValidHTMLElement(constraint.value)) {
                errors.push_back("无效的HTML元素名称: " + constraint.value);
            }
            break;
        case ConstraintType::TYPE:
            if (!isValidTypeName(constraint.value)) {
                errors.push_back("无效的类型名称: " + constraint.value);
            }
            break;
        case ConstraintType::GLOBAL:
            // 全局约束不需要特殊验证
            break;
        default:
            errors.push_back("未知的约束类型");
            break;
    }
    
    return errors;
}

ConstraintType ConstraintParser::getConstraintType(const std::string& text) {
    if (text.empty()) {
        return ConstraintType::GLOBAL;
    }
    
    // 检查是否为HTML元素
    if (isValidHTMLElement(text)) {
        return ConstraintType::ELEMENT;
    }
    
    // 检查是否为类型名称
    if (isValidTypeName(text)) {
        return ConstraintType::TYPE;
    }
    
    // 默认为全局约束
    return ConstraintType::GLOBAL;
}

bool ConstraintParser::isConstraintStatement(const std::string& text) {
    return text.find("except") != std::string::npos;
}

bool ConstraintParser::isExceptClause(const std::string& text) {
    std::string cleanText = text;
    cleanText.erase(0, cleanText.find_first_not_of(" \t"));
    return cleanText.substr(0, 6) == "except";
}

std::string ConstraintParser::extractConstraintValue(const std::string& text, ConstraintType type) {
    std::string cleanText = text;
    
    // 去除首尾空白字符
    cleanText.erase(0, cleanText.find_first_not_of(" \t"));
    cleanText.erase(cleanText.find_last_not_of(" \t") + 1);
    
    switch (type) {
        case ConstraintType::ELEMENT:
            // 对于HTML元素，直接返回名称
            return cleanText;
        case ConstraintType::TYPE:
            // 对于类型，检查是否有前缀
            if (cleanText[0] == '@') {
                return cleanText.substr(1);
            }
            return cleanText;
        case ConstraintType::GLOBAL:
            // 对于全局约束，返回原始文本
            return cleanText;
        default:
            return cleanText;
    }
}

std::shared_ptr<ConstraintNode> ConstraintParser::createConstraintNode(const ConstraintGroup& group) {
    auto node = std::make_shared<ConstraintNode>();
    // 注意：ConstraintNode可能没有这些成员，需要根据实际定义调整
    
    for (const auto& constraint : group.constraints) {
        // 这里需要根据实际的ConstraintNode结构来创建子节点
        // 暂时跳过，因为ConstraintInfoNode可能不存在
    }
    
    return node;
}

void ConstraintParser::clearErrors() {
    errors_.clear();
}

void ConstraintParser::clearStatistics() {
    statistics_ = ParseStatistics();
}

std::string ConstraintParser::getDebugInfo() const {
    std::ostringstream oss;
    oss << "ConstraintParser Debug Info:\n";
    oss << "  Total Constraints: " << statistics_.totalConstraints << "\n";
    oss << "  Element Constraints: " << statistics_.elementConstraints << "\n";
    oss << "  Type Constraints: " << statistics_.typeConstraints << "\n";
    oss << "  Global Constraints: " << statistics_.globalConstraints << "\n";
    oss << "  Parsing Errors: " << statistics_.parsingErrors << "\n";
    oss << "  Current Errors: " << errors_.size() << "\n";
    
    if (!errors_.empty()) {
        oss << "  Error Details:\n";
        for (const auto& error : errors_) {
            oss << "    - " << error << "\n";
        }
    }
    
    return oss.str();
}

void ConstraintParser::reset() {
    clearErrors();
    clearStatistics();
}

// 私有辅助方法
bool ConstraintParser::isValidHTMLElement(const std::string& name) {
    // 简单的HTML元素名称验证
    static const std::vector<std::string> validElements = {
        "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "a", "img", "table", "tr", "td", "th", "form", "input", "button",
        "textarea", "select", "option", "label", "fieldset", "legend", "section", "article",
        "header", "footer", "nav", "aside", "main", "figure", "figcaption", "blockquote",
        "code", "pre", "em", "strong", "i", "b", "u", "s", "mark", "small", "del", "ins"
    };
    
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    return std::find(validElements.begin(), validElements.end(), lowerName) != validElements.end();
}

bool ConstraintParser::isValidTypeName(const std::string& name) {
    // 验证类型名称（如 @Html, @Style, @JavaScript 等）
    if (name.empty()) {
        return false;
    }
    
    // 检查是否为有效的类型名称
    static const std::vector<std::string> validTypes = {
        "Html", "Style", "JavaScript", "Chtl", "CJmod", "Config", "Custom", "Template"
    };
    
    std::string cleanName = name;
    if (cleanName[0] == '@') {
        cleanName = cleanName.substr(1);
    }
    
    return std::find(validTypes.begin(), validTypes.end(), cleanName) != validTypes.end();
}

} // namespace CHTL