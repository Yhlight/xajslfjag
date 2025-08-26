#include "CHTLGenerator.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// === CHTLStyleScope实现 ===

CHTLStyleScope::CHTLStyleScope(const std::string& id, bool global) 
    : scopeId(id), isGlobal(global) {}

std::string CHTLStyleScope::getScopedSelector(const std::string& selector) {
    if (isGlobal) return selector;
    
    auto it = localSelectors.find(selector);
    if (it != localSelectors.end()) {
        return it->second;
    }
    
    // 生成作用域选择器
    std::string scoped = "[data-chtl-scope=\"" + scopeId + "\"] " + selector;
    localSelectors[selector] = scoped;
    return scoped;
}

void CHTLStyleScope::addLocalSelector(const std::string& original, const std::string& scoped) {
    localSelectors[original] = scoped;
}

// === CHTLTemplateManager实现 ===

void CHTLTemplateManager::registerTemplate(const std::string& name, std::shared_ptr<CHTLTemplateNode> templateNode) {
    templates[name] = templateNode;
}

void CHTLTemplateManager::registerCustom(const std::string& name, std::shared_ptr<CHTLCustomNode> customNode) {
    customs[name] = customNode;
}

void CHTLTemplateManager::registerOrigin(const std::string& name, std::shared_ptr<CHTLNode> originNode) {
    origins[name] = originNode;
}

std::shared_ptr<CHTLTemplateNode> CHTLTemplateManager::getTemplate(const std::string& name) {
    auto it = templates.find(name);
    return it != templates.end() ? it->second : nullptr;
}

std::shared_ptr<CHTLCustomNode> CHTLTemplateManager::getCustom(const std::string& name) {
    auto it = customs.find(name);
    return it != customs.end() ? it->second : nullptr;
}

std::shared_ptr<CHTLNode> CHTLTemplateManager::getOrigin(const std::string& name) {
    auto it = origins.find(name);
    return it != origins.end() ? it->second : nullptr;
}

bool CHTLTemplateManager::hasTemplate(const std::string& name) const {
    return templates.find(name) != templates.end();
}

bool CHTLTemplateManager::hasCustom(const std::string& name) const {
    return customs.find(name) != customs.end();
}

bool CHTLTemplateManager::hasOrigin(const std::string& name) const {
    return origins.find(name) != origins.end();
}

// === CHTLVariableResolver实现 ===

void CHTLVariableResolver::setVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

void CHTLVariableResolver::setGlobalVariable(const std::string& name, const std::string& value) {
    globalVariables[name] = value;
}

std::string CHTLVariableResolver::resolveVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    
    auto globalIt = globalVariables.find(name);
    if (globalIt != globalVariables.end()) {
        return globalIt->second;
    }
    
    return ""; // 变量未找到
}

bool CHTLVariableResolver::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end() || 
           globalVariables.find(name) != globalVariables.end();
}

std::string CHTLVariableResolver::interpolateString(const std::string& input) const {
    std::string result = input;
    size_t pos = 0;
    
    // 简单的变量插值 ${variable}
    while ((pos = result.find("${", pos)) != std::string::npos) {
        size_t end = result.find("}", pos);
        if (end != std::string::npos) {
            std::string varName = result.substr(pos + 2, end - pos - 2);
            std::string varValue = resolveVariable(varName);
            result.replace(pos, end - pos + 1, varValue);
            pos += varValue.length();
        } else {
            break;
        }
    }
    
    return result;
}

// === CHTLGenerator主类实现 ===

CHTLGenerator::CHTLGenerator(const CHTLGeneratorConfig& cfg) 
    : config(cfg), indentLevel(0), nextScopeId(1) {}

CHTLGenerationResult CHTLGenerator::generate(std::shared_ptr<CHTLDocumentNode> document) {
    // 重置状态
    htmlStream.str("");
    cssStream.str("");
    jsStream.str("");
    errors.clear();
    warnings.clear();
    indentLevel = 0;
    styleScopes.clear();
    nextScopeId = 1;
    
    try {
        // 第一遍：收集模板、自定义和原始嵌入定义
        collectDefinitions(document);
        
        // 第二遍：生成代码
        generateDocument(document);
        
        // 创建结果
        CHTLGenerationResult result;
        result.html = htmlStream.str();
        result.css = cssStream.str();
        result.javascript = jsStream.str();
        result.errors = errors;
        result.warnings = warnings;
        result.success = errors.empty();
        
        return result;
        
    } catch (const std::exception& e) {
        CHTLGenerationResult result;
        result.success = false;
        result.errors.push_back("代码生成异常: " + std::string(e.what()));
        return result;
    }
}

void CHTLGenerator::collectDefinitions(std::shared_ptr<CHTLDocumentNode> document) {
    for (auto& child : document->children) {
        if (child->type == CHTLNodeType::TEMPLATE_DEFINITION) {
            auto templateNode = std::dynamic_pointer_cast<CHTLTemplateNode>(child);
            if (templateNode) {
                templateManager.registerTemplate(templateNode->name, templateNode);
            }
        } else if (child->type == CHTLNodeType::CUSTOM_DEFINITION) {
            auto customNode = std::dynamic_pointer_cast<CHTLCustomNode>(child);
            if (customNode) {
                templateManager.registerCustom(customNode->name, customNode);
            }
        } else if (child->type == CHTLNodeType::ORIGIN_DEFINITION) {
            templateManager.registerOrigin(child->name, child);
        }
    }
}

void CHTLGenerator::generateDocument(std::shared_ptr<CHTLDocumentNode> document) {
    // HTML5文档结构
    htmlStream << "<!DOCTYPE html>\n";
    htmlStream << "<html lang=\"zh-CN\">\n";
    htmlStream << "<head>\n";
    htmlStream << "  <meta charset=\"utf-8\">\n";
    htmlStream << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
    htmlStream << "  <title>CHTL Generated Page</title>\n";
    
    // 生成样式
    if (!cssStream.str().empty()) {
        htmlStream << "  <style>\n";
        htmlStream << addIndent(cssStream.str(), 2);
        htmlStream << "  </style>\n";
    }
    
    htmlStream << "</head>\n";
    htmlStream << "<body>\n";
    
    // 生成主体内容
    indentLevel = 1;
    for (auto& child : document->children) {
        generateNode(child);
    }
    
    htmlStream << "</body>\n";
    
    // 生成脚本
    if (!jsStream.str().empty()) {
        htmlStream << "<script>\n";
        htmlStream << addIndent(jsStream.str(), 1);
        htmlStream << "</script>\n";
    }
    
    htmlStream << "</html>\n";
}

void CHTLGenerator::generateNode(std::shared_ptr<CHTLNode> node) {
    if (!node) return;
    
    switch (node->type) {
        case CHTLNodeType::HTML_ELEMENT:
            generateElement(std::dynamic_pointer_cast<CHTLElementNode>(node));
            break;
            
        case CHTLNodeType::TEXT_NODE:
            generateTextNode(node);
            break;
            
        case CHTLNodeType::TEMPLATE_USAGE:
            generateTemplateUsage(node);
            break;
            
        case CHTLNodeType::CUSTOM_USAGE:
            generateCustomUsage(node);
            break;
            
        case CHTLNodeType::ORIGIN_DEFINITION:
            generateOriginDefinition(node);
            break;
            
        case CHTLNodeType::STYLE_BLOCK:
            generateStyleBlock(std::dynamic_pointer_cast<CHTLStyleNode>(node));
            break;
            
        case CHTLNodeType::SCRIPT_BLOCK:
            generateScriptBlock(std::dynamic_pointer_cast<CHTLScriptNode>(node));
            break;
            
        // 声明不在HTML输出中生成
        case CHTLNodeType::USE_DECLARATION:
        case CHTLNodeType::NAMESPACE_DECLARATION:
        case CHTLNodeType::TEMPLATE_DEFINITION:
        case CHTLNodeType::CUSTOM_DEFINITION:
        case CHTLNodeType::CONFIGURATION_DEFINITION:
        case CHTLNodeType::IMPORT_STATEMENT:
            break;
            
        default:
            addWarning("未处理的节点类型: " + node->getTypeName());
            break;
    }
}

void CHTLGenerator::generateElement(std::shared_ptr<CHTLElementNode> element) {
    if (!element) return;
    
    std::string tagName = element->name;
    
    // 开始标签
    htmlStream << getIndent() << "<" << tagName;
    
    // 生成属性
    for (const auto& attr : element->attributes) {
        htmlStream << " " << attr.first << "=\"" << escapeHTML(attr.second) << "\"";
    }
    
    // 检查是否为自闭合标签
    bool isSelfClosing = isSelfClosingTag(tagName);
    
    if (isSelfClosing) {
        htmlStream << " />\n";
        return;
    }
    
    htmlStream << ">";
    
    // 生成子内容
    bool hasBlockChildren = hasBlockChildElements(element);
    
    if (hasBlockChildren) {
        htmlStream << "\n";
        indentLevel++;
        
        for (auto& child : element->children) {
            generateNode(child);
        }
        
        indentLevel--;
        htmlStream << getIndent();
    } else {
        // 内联内容
        for (auto& child : element->children) {
            if (child->type == CHTLNodeType::TEXT_NODE) {
                htmlStream << escapeHTML(child->value);
            } else {
                generateNode(child);
            }
        }
    }
    
    // 结束标签
    htmlStream << "</" << tagName << ">\n";
}

void CHTLGenerator::generateTextNode(std::shared_ptr<CHTLNode> textNode) {
    if (!textNode) return;
    
    std::string text = variableResolver.interpolateString(textNode->value);
    htmlStream << getIndent() << escapeHTML(text) << "\n";
}

void CHTLGenerator::generateTemplateUsage(std::shared_ptr<CHTLNode> templateUsage) {
    if (!templateUsage) return;
    
    std::string templateName = templateUsage->name;
    std::string templateType = templateUsage->value;
    
    auto templateNode = templateManager.getTemplate(templateName);
    if (!templateNode) {
        addError("未找到模板: " + templateName);
        return;
    }
    
    if (templateType == "@Style") {
        // 样式模板：生成CSS
        for (auto& child : templateNode->children) {
            if (child->type == CHTLNodeType::STYLE_PROPERTY) {
                cssStream << "  " << child->name << ": " << child->value << ";\n";
            }
        }
    } else if (templateType == "@Element") {
        // 元素模板：直接生成HTML
        for (auto& child : templateNode->children) {
            generateNode(child);
        }
    } else if (templateType == "@Var") {
        // 变量模板：设置变量值
        for (auto& child : templateNode->children) {
            if (child->type == CHTLNodeType::STYLE_PROPERTY) {
                variableResolver.setVariable(child->name, child->value);
            }
        }
    }
}

void CHTLGenerator::generateCustomUsage(std::shared_ptr<CHTLNode> customUsage) {
    // 自定义使用与模板使用类似
    generateTemplateUsage(customUsage);
}

void CHTLGenerator::generateOriginDefinition(std::shared_ptr<CHTLNode> originNode) {
    if (!originNode) return;
    
    // 原始嵌入：直接输出内容
    std::string content = originNode->value;
    
    // 提取实际内容（去掉[Origin] @Type Name { } 包装）
    size_t startPos = content.find("{");
    size_t endPos = content.rfind("}");
    
    if (startPos != std::string::npos && endPos != std::string::npos && endPos > startPos) {
        std::string rawContent = content.substr(startPos + 1, endPos - startPos - 1);
        
        // 去掉前后空白
        rawContent = trim(rawContent);
        
        // 检查类型并生成到相应流
        if (content.find("@Html") != std::string::npos) {
            htmlStream << getIndent() << rawContent << "\n";
        } else if (content.find("@Style") != std::string::npos || content.find("@CSS") != std::string::npos) {
            cssStream << rawContent << "\n";
        } else if (content.find("@JavaScript") != std::string::npos || content.find("@JS") != std::string::npos) {
            jsStream << rawContent << "\n";
        } else {
            // 默认为HTML
            htmlStream << getIndent() << rawContent << "\n";
        }
    }
}

void CHTLGenerator::generateStyleBlock(std::shared_ptr<CHTLStyleNode> styleNode) {
    if (!styleNode) return;
    
    // 创建样式作用域
    std::string scopeId = "chtl-" + std::to_string(nextScopeId++);
    auto scope = std::make_unique<CHTLStyleScope>(scopeId, !styleNode->isLocal);
    
    // 生成样式规则
    for (auto& child : styleNode->children) {
        if (child->type == CHTLNodeType::STYLE_RULE) {
            generateStyleRule(child, scopeId);
        } else if (child->type == CHTLNodeType::STYLE_PROPERTY) {
            generateStyleProperty(child);
        }
    }
    
    styleScopes.push_back(std::move(scope));
}

void CHTLGenerator::generateStyleRule(std::shared_ptr<CHTLNode> styleRule, const std::string& scopeId) {
    if (!styleRule) return;
    
    std::string selector = styleRule->name;
    
    // 应用作用域
    if (!scopeId.empty()) {
        selector = "[data-chtl-scope=\"" + scopeId + "\"] " + selector;
    }
    
    cssStream << selector << " {\n";
    
    for (auto& child : styleRule->children) {
        if (child->type == CHTLNodeType::STYLE_PROPERTY) {
            cssStream << "  " << child->name << ": " << child->value << ";\n";
        }
    }
    
    cssStream << "}\n";
}

void CHTLGenerator::generateStyleProperty(std::shared_ptr<CHTLNode> styleProperty) {
    if (!styleProperty) return;
    
    std::string property = styleProperty->name;
    std::string value = variableResolver.interpolateString(styleProperty->value);
    
    cssStream << "  " << property << ": " << value << ";\n";
}

void CHTLGenerator::generateScriptBlock(std::shared_ptr<CHTLScriptNode> scriptNode) {
    if (!scriptNode) return;
    
    std::string scriptContent = scriptNode->getScriptContent();
    
    // 脚本内容交给CHTL JS编译器处理（这里简化为直接输出）
    jsStream << "// CHTL Local Script\n";
    jsStream << scriptContent << "\n";
}

// === 辅助方法 ===

std::string CHTLGenerator::getIndent() const {
    return std::string(indentLevel * config.indentString.length(), ' ');
}

std::string CHTLGenerator::addIndent(const std::string& text, int level) const {
    std::string indent(level * config.indentString.length(), ' ');
    std::stringstream ss(text);
    std::string line;
    std::stringstream result;
    
    while (std::getline(ss, line)) {
        if (!line.empty()) {
            result << indent << line << "\n";
        } else {
            result << "\n";
        }
    }
    
    return result.str();
}

std::string CHTLGenerator::escapeHTML(const std::string& text) const {
    std::string result = text;
    
    // 替换HTML特殊字符
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    return result;
}

bool CHTLGenerator::isSelfClosingTag(const std::string& tagName) const {
    static const std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return std::find(selfClosingTags.begin(), selfClosingTags.end(), tagName) != selfClosingTags.end();
}

bool CHTLGenerator::hasBlockChildElements(std::shared_ptr<CHTLElementNode> element) const {
    for (auto& child : element->children) {
        if (child->type == CHTLNodeType::HTML_ELEMENT || 
            child->type == CHTLNodeType::STYLE_BLOCK || 
            child->type == CHTLNodeType::SCRIPT_BLOCK) {
            return true;
        }
    }
    return false;
}

std::string CHTLGenerator::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

void CHTLGenerator::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLGenerator::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

} // namespace CHTL