#include "ConstraintParser.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <regex>

namespace CHTL {

ConstraintParser::ConstraintParser(std::shared_ptr<ConfigurationManager> configManager)
    : configManager_(configManager) {
}

ConstraintParser::~ConstraintParser() {
}

std::vector<std::shared_ptr<ConstraintStatement>> ConstraintParser::parse(const std::string& source, const std::string& scope) {
    clearErrors();
    std::vector<std::shared_ptr<ConstraintStatement>> statements;
    
    if (source.empty()) {
        return statements;
    }
    
    std::istringstream iss(source);
    std::string line;
    int lineNumber = 1;
    
    while (std::getline(iss, line)) {
        // 查找except语句
        if (line.find("except") != std::string::npos) {
            auto statement = parseExceptStatement(line, lineNumber, scope);
            if (statement) {
                statements.push_back(statement);
            }
        }
        lineNumber++;
    }
    
    return statements;
}

std::shared_ptr<ConstraintStatement> ConstraintParser::parseExceptStatement(const std::string& line, int lineNumber, const std::string& scope) {
    auto statement = std::make_shared<ConstraintStatement>();
    statement->lineNumber = lineNumber;
    statement->columnNumber = 1;
    statement->scope = scope;
    
    size_t position = 0;
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 匹配 "except"
    if (!matchKeyword(line, position, "except")) {
        addError("期望关键字 'except'");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 解析约束目标列表
    while (position < line.length()) {
        // 跳过空白字符
        skipWhitespace(line, position);
        
        if (position >= line.length()) {
            break;
        }
        
        // 检查是否到达语句结束
        if (line[position] == ';' || line[position] == '{') {
            break;
        }
        
        // 提取目标文本
        std::string targetText;
        while (position < line.length() && line[position] != ',' && line[position] != ';' && line[position] != '{') {
            targetText += line[position];
            position++;
        }
        
        // 去除首尾空白字符
        targetText.erase(0, targetText.find_first_not_of(" \t"));
        targetText.erase(targetText.find_last_not_of(" \t") + 1);
        
        if (!targetText.empty()) {
            auto target = parseConstraintTarget(targetText, lineNumber);
            statement->targets.push_back(target);
        }
        
        // 跳过逗号
        if (position < line.length() && line[position] == ',') {
            position++;
        }
    }
    
    // 确定约束类型
    if (scope.empty()) {
        statement->type = ConstraintType::GLOBAL;
    } else if (statement->targets.size() > 0) {
        // 根据第一个目标的类型确定约束类型
        auto& firstTarget = statement->targets[0];
        if (firstTarget.type == ConstraintTargetType::HTML_ELEMENT || 
            firstTarget.type == ConstraintTargetType::CUSTOM_ELEMENT ||
            firstTarget.type == ConstraintTargetType::TEMPLATE_VAR ||
            firstTarget.type == ConstraintTargetType::TEMPLATE_ELEMENT ||
            firstTarget.type == ConstraintTargetType::TEMPLATE_STYLE) {
            statement->type = ConstraintType::EXACT;
        } else {
            statement->type = ConstraintType::TYPE;
        }
    }
    
    return statement;
}

ConstraintTarget ConstraintParser::parseConstraintTarget(const std::string& targetText, int lineNumber) {
    ConstraintTarget target;
    target.lineNumber = lineNumber;
    target.fullPath = targetText;
    
    // 去除首尾空白字符
    std::string trimmedText = targetText;
    trimmedText.erase(0, trimmedText.find_first_not_of(" \t"));
    trimmedText.erase(trimmedText.find_last_not_of(" \t") + 1);
    
    if (trimmedText.empty()) {
        target.type = ConstraintTargetType::HTML_ELEMENT;
        target.name = "";
        return target;
    }
    
    // 检查是否是HTML元素（纯标识符）
    if (trimmedText.find('[') == std::string::npos && trimmedText.find('@') == std::string::npos) {
        return parseHTMLElement(trimmedText, lineNumber);
    }
    
    // 检查是否是自定义元素
    if (trimmedText.find("[Custom]") != std::string::npos) {
        return parseCustomElement(trimmedText, lineNumber);
    }
    
    // 检查是否是模板对象
    if (trimmedText.find("[Template]") != std::string::npos) {
        return parseTemplateObject(trimmedText, lineNumber);
    }
    
    // 检查是否是类型约束
    if (trimmedText.find("@Html") != std::string::npos || 
        trimmedText.find("[Custom]") != std::string::npos ||
        trimmedText.find("[Template]") != std::string::npos) {
        return parseTypeConstraint(trimmedText, lineNumber);
    }
    
    // 默认为HTML元素
    return parseHTMLElement(trimmedText, lineNumber);
}

ConstraintTarget ConstraintParser::parseHTMLElement(const std::string& elementName, int lineNumber) {
    ConstraintTarget target;
    target.type = ConstraintTargetType::HTML_ELEMENT;
    target.name = elementName;
    target.fullPath = elementName;
    target.lineNumber = lineNumber;
    target.columnNumber = 1;
    return target;
}

ConstraintTarget ConstraintParser::parseCustomElement(const std::string& customText, int lineNumber) {
    ConstraintTarget target;
    target.type = ConstraintTargetType::CUSTOM_ELEMENT;
    target.fullPath = customText;
    target.lineNumber = lineNumber;
    target.columnNumber = 1;
    
    // 提取元素名称
    size_t pos = customText.find("@Element");
    if (pos != std::string::npos) {
        pos += 8; // "@Element" 的长度
        while (pos < customText.length() && std::isspace(customText[pos])) {
            pos++;
        }
        size_t endPos = pos;
        while (endPos < customText.length() && 
               (std::isalnum(customText[endPos]) || customText[endPos] == '_' || customText[endPos] == '-')) {
            endPos++;
        }
        target.name = customText.substr(pos, endPos - pos);
    } else {
        target.name = customText;
    }
    
    return target;
}

ConstraintTarget ConstraintParser::parseTemplateObject(const std::string& templateText, int lineNumber) {
    ConstraintTarget target;
    target.fullPath = templateText;
    target.lineNumber = lineNumber;
    target.columnNumber = 1;
    
    // 确定模板对象类型
    if (templateText.find("@Var") != std::string::npos) {
        target.type = ConstraintTargetType::TEMPLATE_VAR;
        target.name = "@Var";
    } else if (templateText.find("@Element") != std::string::npos) {
        target.type = ConstraintTargetType::TEMPLATE_ELEMENT;
        target.name = "@Element";
    } else if (templateText.find("@Style") != std::string::npos) {
        target.type = ConstraintTargetType::TEMPLATE_STYLE;
        target.name = "@Style";
    } else {
        target.type = ConstraintTargetType::TEMPLATE_BLOCK;
        target.name = "[Template]";
    }
    
    return target;
}

ConstraintTarget ConstraintParser::parseTypeConstraint(const std::string& typeText, int lineNumber) {
    ConstraintTarget target;
    target.fullPath = typeText;
    target.lineNumber = lineNumber;
    target.columnNumber = 1;
    
    if (typeText.find("@Html") != std::string::npos) {
        target.type = ConstraintTargetType::ORIGIN_HTML;
        target.name = "@Html";
    } else if (typeText.find("[Custom]") != std::string::npos) {
        target.type = ConstraintTargetType::CUSTOM_BLOCK;
        target.name = "[Custom]";
    } else if (typeText.find("[Template]") != std::string::npos) {
        target.type = ConstraintTargetType::TEMPLATE_BLOCK;
        target.name = "[Template]";
    } else {
        target.type = ConstraintTargetType::HTML_ELEMENT;
        target.name = typeText;
    }
    
    return target;
}

bool ConstraintParser::validateConstraintStatement(const std::shared_ptr<ConstraintStatement>& statement) {
    if (!statement) {
        addError("约束语句为空");
        return false;
    }
    
    if (statement->targets.empty()) {
        addError("约束语句必须包含至少一个目标");
        return false;
    }
    
    // 验证每个约束目标
    for (const auto& target : statement->targets) {
        if (target.name.empty() && target.type != ConstraintTargetType::TEMPLATE_BLOCK) {
            addError("约束目标名称不能为空");
            return false;
        }
    }
    
    return true;
}

bool ConstraintParser::applyConstraintStatement(const std::shared_ptr<ConstraintStatement>& statement) {
    if (!validateConstraintStatement(statement)) {
        return false;
    }
    
    std::string normalizedScope = normalizeScope(statement->scope);
    
    for (const auto& target : statement->targets) {
        switch (target.type) {
            case ConstraintTargetType::HTML_ELEMENT:
            case ConstraintTargetType::CUSTOM_ELEMENT:
            case ConstraintTargetType::TEMPLATE_VAR:
            case ConstraintTargetType::TEMPLATE_ELEMENT:
            case ConstraintTargetType::TEMPLATE_STYLE:
                elementConstraints_[normalizedScope].insert(target.fullPath);
                break;
                
            case ConstraintTargetType::ORIGIN_HTML:
            case ConstraintTargetType::CUSTOM_BLOCK:
            case ConstraintTargetType::TEMPLATE_BLOCK:
                typeConstraints_[normalizedScope].insert(target.fullPath);
                break;
        }
    }
    
    return true;
}

bool ConstraintParser::isElementConstrained(const std::string& elementName, const std::string& scope) const {
    std::string normalizedScope = normalizeScope(scope);
    
    // 检查全局约束
    auto globalIt = elementConstraints_.find("");
    if (globalIt != elementConstraints_.end()) {
        if (globalIt->second.find(elementName) != globalIt->second.end()) {
            return true;
        }
    }
    
    // 检查作用域约束
    auto scopeIt = elementConstraints_.find(normalizedScope);
    if (scopeIt != elementConstraints_.end()) {
        if (scopeIt->second.find(elementName) != scopeIt->second.end()) {
            return true;
        }
    }
    
    return false;
}

bool ConstraintParser::isTypeConstrained(const std::string& typeName, const std::string& scope) const {
    std::string normalizedScope = normalizeScope(scope);
    
    // 检查全局约束
    auto globalIt = typeConstraints_.find("");
    if (globalIt != typeConstraints_.end()) {
        if (globalIt->second.find(typeName) != globalIt->second.end()) {
            return true;
        }
    }
    
    // 检查作用域约束
    auto scopeIt = typeConstraints_.find(normalizedScope);
    if (scopeIt != typeConstraints_.end()) {
        if (scopeIt->second.find(typeName) != scopeIt->second.end()) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> ConstraintParser::getErrors() const {
    return errors_;
}

void ConstraintParser::clearErrors() {
    errors_.clear();
}

void ConstraintParser::addError(const std::string& error) {
    errors_.push_back(error);
}

void ConstraintParser::skipWhitespace(const std::string& line, size_t& position) {
    while (position < line.length() && std::isspace(line[position])) {
        position++;
    }
}

std::string ConstraintParser::extractIdentifier(const std::string& line, size_t& position) {
    std::string identifier;
    
    while (position < line.length() && 
           (std::isalnum(line[position]) || line[position] == '_' || line[position] == '-')) {
        identifier += line[position];
        position++;
    }
    
    return identifier;
}

bool ConstraintParser::matchKeyword(const std::string& line, size_t& position, const std::string& keyword) {
    if (position + keyword.length() > line.length()) {
        return false;
    }
    
    std::string substr = line.substr(position, keyword.length());
    if (substr == keyword) {
        position += keyword.length();
        return true;
    }
    
    return false;
}

std::string ConstraintParser::normalizeScope(const std::string& scope) const {
    if (scope.empty()) {
        return "";
    }
    
    // 去除首尾空白字符
    std::string normalized = scope;
    normalized.erase(0, normalized.find_first_not_of(" \t"));
    normalized.erase(normalized.find_last_not_of(" \t") + 1);
    
    return normalized;
}

} // namespace CHTL