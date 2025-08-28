#include "UseStatementParser.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <regex>

namespace CHTL {

UseStatementParser::UseStatementParser(std::shared_ptr<ConfigurationManager> configManager)
    : configManager_(configManager) {
}

UseStatementParser::~UseStatementParser() {
}

std::shared_ptr<UseStatement> UseStatementParser::parse(const std::string& source) {
    clearErrors();
    
    if (source.empty()) {
        addError("源代码为空");
        return nullptr;
    }
    
    // 查找第一个非空行
    std::istringstream iss(source);
    std::string line;
    int lineNumber = 1;
    
    while (std::getline(iss, line)) {
        // 跳过空行和注释行
        if (line.empty() || line.find("--") == 0) {
            lineNumber++;
            continue;
        }
        
        // 检查是否以use开头
        if (line.find("use") == 0) {
            // 解析use语句
            if (line.find("html5") != std::string::npos) {
                return parseHTML5Type(line, lineNumber);
            } else if (line.find("[Configuration]") != std::string::npos) {
                return parseFullConfigGroup(line, lineNumber);
            } else if (line.find("@Config") != std::string::npos) {
                return parseConfigGroup(line, lineNumber);
            } else {
                addError("未知的use语句格式: " + line);
                return nullptr;
            }
        }
        
        // 如果遇到其他语句，说明use语句不在文件开头
        if (line.find("[") == 0 || line.find("@") == 0 || 
            (line.find_first_not_of(" \t") != std::string::npos && line.find_first_not_of(" \t") != 0)) {
            addError("use语句必须在文件开头");
            return nullptr;
        }
        
        lineNumber++;
    }
    
    addError("未找到use语句");
    return nullptr;
}

std::shared_ptr<UseStatement> UseStatementParser::parseHTML5Type(const std::string& line, int lineNumber) {
    auto statement = std::make_shared<UseStatement>();
    statement->type = UseStatementType::HTML5_TYPE;
    statement->lineNumber = lineNumber;
    statement->columnNumber = 1;
    
    size_t position = 0;
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 匹配 "use"
    if (!matchKeyword(line, position, "use")) {
        addError("期望关键字 'use'");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 匹配 "html5"
    if (!matchKeyword(line, position, "html5")) {
        addError("期望关键字 'html5'");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 检查分号
    if (position >= line.length() || line[position] != ';') {
        addError("期望分号 ';'");
        return nullptr;
    }
    
    statement->html5Type = "html5";
    return statement;
}

std::shared_ptr<UseStatement> UseStatementParser::parseConfigGroup(const std::string& line, int lineNumber) {
    auto statement = std::make_shared<UseStatement>();
    statement->type = UseStatementType::CONFIG_GROUP;
    statement->lineNumber = lineNumber;
    statement->columnNumber = 1;
    
    size_t position = 0;
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 匹配 "use"
    if (!matchKeyword(line, position, "use")) {
        addError("期望关键字 'use'");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 匹配 "@Config"
    if (!matchKeyword(line, position, "@Config")) {
        addError("期望关键字 '@Config'");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 提取配置组名称
    std::string configGroupName = extractIdentifier(line, position);
    if (configGroupName.empty()) {
        addError("期望配置组名称");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 检查分号
    if (position >= line.length() || line[position] != ';') {
        addError("期望分号 ';'");
        return nullptr;
    }
    
    statement->configGroupName = configGroupName;
    return statement;
}

std::shared_ptr<UseStatement> UseStatementParser::parseFullConfigGroup(const std::string& line, int lineNumber) {
    auto statement = std::make_shared<UseStatement>();
    statement->type = UseStatementType::FULL_CONFIG_GROUP;
    statement->lineNumber = lineNumber;
    statement->columnNumber = 1;
    
    size_t position = 0;
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 匹配 "use"
    if (!matchKeyword(line, position, "use")) {
        addError("期望关键字 'use'");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 匹配 "[Configuration]"
    if (!matchKeyword(line, position, "[Configuration]")) {
        addError("期望关键字 '[Configuration]'");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 匹配 "@Config"
    if (!matchKeyword(line, position, "@Config")) {
        addError("期望关键字 '@Config'");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 提取配置组名称
    std::string configGroupName = extractIdentifier(line, position);
    if (configGroupName.empty()) {
        addError("期望配置组名称");
        return nullptr;
    }
    
    // 跳过空白字符
    skipWhitespace(line, position);
    
    // 检查分号
    if (position >= line.length() || line[position] != ';') {
        addError("期望分号 ';'");
        return nullptr;
    }
    
    statement->configGroupName = configGroupName;
    return statement;
}

bool UseStatementParser::validateUseStatement(const std::shared_ptr<UseStatement>& statement) {
    if (!statement) {
        addError("use语句为空");
        return false;
    }
    
    switch (statement->type) {
        case UseStatementType::HTML5_TYPE:
            if (statement->html5Type != "html5") {
                addError("HTML5类型必须是 'html5'");
                return false;
            }
            break;
            
        case UseStatementType::CONFIG_GROUP:
        case UseStatementType::FULL_CONFIG_GROUP:
            if (statement->configGroupName.empty()) {
                addError("配置组名称不能为空");
                return false;
            }
            
            // 检查配置组是否存在
            if (!configManager_->hasConfigurationGroup(statement->configGroupName)) {
                addError("配置组不存在: " + statement->configGroupName);
                return false;
            }
            break;
    }
    
    return true;
}

bool UseStatementParser::applyUseStatement(const std::shared_ptr<UseStatement>& statement) {
    if (!validateUseStatement(statement)) {
        return false;
    }
    
    switch (statement->type) {
        case UseStatementType::HTML5_TYPE:
            // HTML5类型不需要特殊处理，只是声明
            break;
            
        case UseStatementType::CONFIG_GROUP:
        case UseStatementType::FULL_CONFIG_GROUP:
            // 激活指定的配置组
            if (!configManager_->activateConfigurationGroup(statement->configGroupName)) {
                addError("无法激活配置组: " + statement->configGroupName);
                return false;
            }
            break;
    }
    
    return true;
}

std::vector<std::string> UseStatementParser::getErrors() const {
    return errors_;
}

void UseStatementParser::clearErrors() {
    errors_.clear();
}

void UseStatementParser::addError(const std::string& error) {
    errors_.push_back(error);
}

bool UseStatementParser::isAtFileBeginning(const std::string& source, int lineNumber) {
    std::istringstream iss(source);
    std::string line;
    int currentLine = 1;
    
    while (std::getline(iss, line) && currentLine < lineNumber) {
        // 跳过空行和注释行
        if (!line.empty() && line.find("--") != 0) {
            // 如果遇到非空非注释行，说明use语句不在文件开头
            if (line.find_first_not_of(" \t") != std::string::npos) {
                return false;
            }
        }
        currentLine++;
    }
    
    return true;
}

int UseStatementParser::getLineNumber(const std::string& source, size_t position) {
    int lineNumber = 1;
    for (size_t i = 0; i < position && i < source.length(); ++i) {
        if (source[i] == '\n') {
            lineNumber++;
        }
    }
    return lineNumber;
}

int UseStatementParser::getColumnNumber(const std::string& source, size_t position) {
    int columnNumber = 1;
    for (size_t i = 0; i < position && i < source.length(); ++i) {
        if (source[i] == '\n') {
            columnNumber = 1;
        } else {
            columnNumber++;
        }
    }
    return columnNumber;
}

void UseStatementParser::skipWhitespace(const std::string& line, size_t& position) {
    while (position < line.length() && std::isspace(line[position])) {
        position++;
    }
}

std::string UseStatementParser::extractIdentifier(const std::string& line, size_t& position) {
    std::string identifier;
    
    while (position < line.length() && 
           (std::isalnum(line[position]) || line[position] == '_' || line[position] == '-')) {
        identifier += line[position];
        position++;
    }
    
    return identifier;
}

bool UseStatementParser::matchKeyword(const std::string& line, size_t& position, const std::string& keyword) {
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

} // namespace CHTL