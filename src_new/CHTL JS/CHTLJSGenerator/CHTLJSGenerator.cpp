#include "CHTLJSGenerator.h"
#include "../../Error/ErrorReporter.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace CHTL {

CHTLJSGenerator::CHTLJSGenerator() : m_debugMode(false) {
    m_context = std::make_shared<CHTLJSContext>();
}

CHTLJSGenerator::~CHTLJSGenerator() = default;

std::string CHTLJSGenerator::generateJavaScript(std::shared_ptr<BaseNode> rootNode) {
    if (!rootNode) {
        reportError("根节点为空");
        return "";
    }
    
    try {
        if (m_debugMode) {
            std::cout << "🚀 开始CHTL JS代码生成" << std::endl;
        }
        
        clearErrors();
        
        std::ostringstream js;
        
        // 生成CHTL JS代码
        js << "// CHTL JS生成的代码" << std::endl;
        js << "(function() {" << std::endl;
        js << "  'use strict';" << std::endl;
        js << std::endl;
        
        // 处理根节点的所有子节点
        for (const auto& child : rootNode->getChildren()) {
            js << generateJSNode(child, 1);
        }
        
        js << "})();" << std::endl;
        
        std::string result = js.str();
        
        if (m_debugMode) {
            std::cout << "  ✓ CHTL JS生成完成: " << result.length() << " 字符" << std::endl;
        }
        
        return result;
        
    } catch (const std::exception& e) {
        reportError("CHTL JS生成过程中发生异常: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLJSGenerator::generateJSNode(std::shared_ptr<BaseNode> node, int depth) {
    if (!node) {
        return "";
    }
    
    std::ostringstream js;
    
    switch (node->getType()) {
        case NodeType::MODULE_IMPORT:
            js << generateModuleImport(node, depth);
            break;
            
        case NodeType::ENHANCED_SELECTOR:
            js << generateEnhancedSelector(node);
            break;
            
        case NodeType::EVENT_BINDING:
            js << generateEventBinding(node, depth);
            break;
            
        case NodeType::ANIMATE:
            js << generateAnimateBlock(node, depth);
            break;
            
        case NodeType::LISTEN:
            js << generateListenBlock(node, depth);
            break;
            
        case NodeType::VIRTUAL_OBJECT:
            js << generateVirtualObject(node, depth);
            break;
            
        case NodeType::DELEGATE:
            js << generateDelegateBlock(node, depth);
            break;
            
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                js << generateJSNode(child, depth);
            }
            break;
    }
    
    return js.str();
}

std::string CHTLJSGenerator::generateModuleImport(std::shared_ptr<BaseNode> node, int depth) {
    std::ostringstream js;
    
    js << getIndentation(depth) << "// AMD风格模块加载" << std::endl;
    
    std::string countStr = node->getAttribute("count");
    int moduleCount = countStr.empty() ? 0 : std::stoi(countStr);
    
    for (int i = 0; i < moduleCount; ++i) {
        std::string moduleKey = "module" + std::to_string(i);
        std::string modulePath = node->getAttribute(moduleKey);
        
        if (!modulePath.empty()) {
            js << getIndentation(depth) << "loadModule('" << modulePath << "');" << std::endl;
        }
    }
    
    js << std::endl;
    
    return js.str();
}

std::string CHTLJSGenerator::generateEnhancedSelector(std::shared_ptr<BaseNode> node) {
    std::string selector = node->getAttribute("selector");
    std::string type = node->getAttribute("type");
    
    if (type == "class") {
        return "document.querySelector('" + selector + "')";
    } else if (type == "id") {
        return "document.getElementById('" + selector.substr(1) + "')";  // 移除#
    } else if (type == "indexed") {
        // 处理索引选择器 button[0]
        std::regex indexRegex(R"((\w+)\[(\d+)\])");
        std::smatch match;
        
        if (std::regex_search(selector, match, indexRegex)) {
            std::string tag = match[1].str();
            std::string index = match[2].str();
            return "document.querySelectorAll('" + tag + "')[" + index + "]";
        }
    } else if (type == "tag") {
        return "document.querySelectorAll('" + selector + "')";
    }
    
    return "document.querySelector('" + selector + "')";
}

std::string CHTLJSGenerator::generateEventBinding(std::shared_ptr<BaseNode> node, int depth) {
    std::ostringstream js;
    
    std::string eventName = node->getAttribute("event");
    std::string handlerContent = node->getValue();
    
    // 查找选择器子节点
    std::string selectorJS;
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::ENHANCED_SELECTOR) {
            selectorJS = generateEnhancedSelector(child);
            break;
        }
    }
    
    if (!selectorJS.empty()) {
        js << getIndentation(depth) << selectorJS << ".addEventListener('" << eventName << "', function(e) {" << std::endl;
        js << getIndentation(depth + 1) << handlerContent << std::endl;
        js << getIndentation(depth) << "});" << std::endl;
    }
    
    return js.str();
}

std::string CHTLJSGenerator::generateAnimateBlock(std::shared_ptr<BaseNode> node, int depth) {
    std::ostringstream js;
    
    js << getIndentation(depth) << "// CHTL JS动画" << std::endl;
    js << getIndentation(depth) << "requestAnimationFrame(function() {" << std::endl;
    
    // 生成动画参数
    std::string target = node->getAttribute("target");
    std::string duration = node->getAttribute("duration");
    std::string easing = node->getAttribute("easing");
    
    if (!target.empty()) {
        js << getIndentation(depth + 1) << "const target = " << target << ";" << std::endl;
    }
    
    if (!duration.empty()) {
        js << getIndentation(depth + 1) << "const duration = " << duration << ";" << std::endl;
    }
    
    if (!easing.empty()) {
        js << getIndentation(depth + 1) << "const easing = '" << easing << "';" << std::endl;
    }
    
    js << getIndentation(depth + 1) << "// 动画实现逻辑" << std::endl;
    js << getIndentation(depth) << "});" << std::endl;
    
    return js.str();
}

std::string CHTLJSGenerator::generateListenBlock(std::shared_ptr<BaseNode> node, int depth) {
    std::ostringstream js;
    
    js << getIndentation(depth) << "{" << std::endl;
    
    // 生成事件监听器
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::EVENT_LISTENER) {
            std::string eventName = child->getAttribute("event");
            std::string handlerContent = child->getValue();
            
            js << getIndentation(depth + 1) << eventName << ": function(e) {" << std::endl;
            js << getIndentation(depth + 2) << handlerContent << std::endl;
            js << getIndentation(depth + 1) << "}," << std::endl;
        }
    }
    
    js << getIndentation(depth) << "}";
    
    return js.str();
}

std::string CHTLJSGenerator::generateVirtualObject(std::shared_ptr<BaseNode> node, int depth) {
    std::ostringstream js;
    
    std::string virName = node->getAttribute("name");
    
    js << getIndentation(depth) << "// 虚拟对象: " << virName << std::endl;
    js << getIndentation(depth) << "var " << virName << " = ";
    
    // 处理虚拟对象内容
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::LISTEN) {
            js << generateListenBlock(child, 0);
        }
    }
    
    js << ";" << std::endl;
    
    return js.str();
}

std::string CHTLJSGenerator::generateDelegateBlock(std::shared_ptr<BaseNode> node, int depth) {
    std::ostringstream js;
    
    std::string target = node->getAttribute("target");
    
    js << getIndentation(depth) << "// 事件委托" << std::endl;
    js << getIndentation(depth) << target << ".addEventListener('click', function(e) {" << std::endl;
    
    // 生成委托逻辑
    for (const auto& attr : node->getAttributes()) {
        if (attr.first != "target") {
            js << getIndentation(depth + 1) << "if (e.target.matches('某选择器')) {" << std::endl;
            js << getIndentation(depth + 2) << "// " << attr.first << ": " << attr.second << std::endl;
            js << getIndentation(depth + 1) << "}" << std::endl;
        }
    }
    
    js << getIndentation(depth) << "});" << std::endl;
    
    return js.str();
}

std::string CHTLJSGenerator::getIndentation(int depth) const {
    std::string indentation;
    for (int i = 0; i < depth; ++i) {
        indentation += "  ";
    }
    return indentation;
}

void CHTLJSGenerator::setContext(std::shared_ptr<CHTLJSContext> context) {
    m_context = context;
}

void CHTLJSGenerator::clearErrors() {
    m_errors.clear();
    m_warnings.clear();
}

void CHTLJSGenerator::reportError(const std::string& message) {
    m_errors.push_back(message);
    
    if (m_debugMode) {
        std::cout << "  ❌ CHTL JS生成错误: " << message << std::endl;
    }
    
    ErrorReporter reporter;
    reporter.error("CHTLJSGenerator", message);
}

void CHTLJSGenerator::reportWarning(const std::string& message) {
    m_warnings.push_back(message);
    
    if (m_debugMode) {
        std::cout << "  ⚠️ CHTL JS生成警告: " << message << std::endl;
    }
    
    ErrorReporter reporter;
    reporter.warning("CHTLJSGenerator", message);
}

}