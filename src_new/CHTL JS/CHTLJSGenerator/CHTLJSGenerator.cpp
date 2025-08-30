#include "CHTLJSGenerator.h"
#include <sstream>
#include <regex>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator(const GenerateOptions& options)
    : m_options(options) {
}

std::string CHTLJSGenerator::generateJS(const NodePtr& root) {
    if (!root) return "";
    
    std::ostringstream js;
    
    // 生成根节点的JavaScript代码
    for (size_t i = 0; i < root->getChildCount(); ++i) {
        NodePtr child = root->getChild(i);
        std::string childJS = generateNodeJS(child);
        if (!childJS.empty()) {
            js << childJS << "\n";
        }
    }
    
    std::string result = js.str();
    
    if (m_options.optimizeOutput) {
        result = optimizeGeneratedCode(result);
    }
    
    return result;
}

std::string CHTLJSGenerator::generateModuleCode(const NodePtr& moduleNode) {
    if (!moduleNode || moduleNode->nodeType != CHTLJSNodeType::MODULE_NODE) {
        return "";
    }
    
    std::ostringstream js;
    
    // 生成AMD风格模块加载代码
    js << "// CHTL JS Module Loading\n";
    js << "(function() {\n";
    js << generateIndentation(1) << "var loadedModules = [];\n";
    
    for (size_t i = 0; i < moduleNode->getChildCount(); ++i) {
        NodePtr child = moduleNode->getChild(i);
        if (child && !child->content.empty()) {
            js << generateIndentation(1) << "loadedModules.push('" << child->content << "');\n";
            js << generateIndentation(1) << "var script = document.createElement('script');\n";
            js << generateIndentation(1) << "script.src = '" << child->content << "';\n";
            js << generateIndentation(1) << "document.head.appendChild(script);\n";
        }
    }
    
    js << "})();\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateEnhancedSelector(const NodePtr& selectorNode) {
    if (!selectorNode || selectorNode->nodeType != CHTLJSNodeType::ENHANCED_SELECTOR_NODE) {
        return "";
    }
    
    std::string selector = selectorNode->content;
    return convertEnhancedSelector(selector);
}

std::string CHTLJSGenerator::generateEventListener(const NodePtr& listenerNode) {
    if (!listenerNode || listenerNode->nodeType != CHTLJSNodeType::EVENT_LISTENER_NODE) {
        return "";
    }
    
    std::ostringstream js;
    
    // 生成addEventListener代码
    for (size_t i = 0; i < listenerNode->getChildCount(); ++i) {
        NodePtr child = listenerNode->getChild(i);
        if (child) {
            std::string content = child->content;
            size_t colonPos = content.find(':');
            if (colonPos != std::string::npos) {
                std::string event = content.substr(0, colonPos);
                std::string handler = content.substr(colonPos + 1);
                
                js << "element.addEventListener('" << event << "', " << handler << ");\n";
            }
        }
    }
    
    return js.str();
}

std::string CHTLJSGenerator::generateEventDelegate(const NodePtr& delegateNode) {
    if (!delegateNode || delegateNode->nodeType != CHTLJSNodeType::EVENT_DELEGATE_NODE) {
        return "";
    }
    
    std::ostringstream js;
    
    // 生成事件委托代码
    js << "// Event Delegation\n";
    js << "document.addEventListener('click', function(e) {\n";
    js << generateIndentation(1) << "// Delegate event handling\n";
    js << "}, true);\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateAnimation(const NodePtr& animationNode) {
    if (!animationNode || animationNode->nodeType != CHTLJSNodeType::ANIMATION_NODE) {
        return "";
    }
    
    std::ostringstream js;
    
    // 生成requestAnimationFrame动画代码
    js << "// CHTL JS Animation\n";
    js << "(function() {\n";
    js << generateIndentation(1) << "function animate(element, properties, duration) {\n";
    js << generateIndentation(2) << "var start = performance.now();\n";
    js << generateIndentation(2) << "function frame(time) {\n";
    js << generateIndentation(3) << "var progress = (time - start) / duration;\n";
    js << generateIndentation(3) << "if (progress < 1) {\n";
    js << generateIndentation(4) << "// Apply animation properties\n";
    js << generateIndentation(4) << "requestAnimationFrame(frame);\n";
    js << generateIndentation(3) << "}\n";
    js << generateIndentation(2) << "}\n";
    js << generateIndentation(2) << "requestAnimationFrame(frame);\n";
    js << generateIndentation(1) << "}\n";
    js << "})();\n";
    
    return js.str();
}

std::string CHTLJSGenerator::generateVirtualObject(const NodePtr& virtualNode) {
    if (!virtualNode || virtualNode->nodeType != CHTLJSNodeType::VIRTUAL_OBJECT_NODE) {
        return "";
    }
    
    std::string objectName = virtualNode->content;
    
    // vir是编译期语法糖，不生成实际代码
    // 但需要缓存虚拟对象信息供后续引用
    m_virtualObjectCache[objectName] = "cached";
    
    // 为子节点生成代码
    std::ostringstream js;
    for (size_t i = 0; i < virtualNode->getChildCount(); ++i) {
        NodePtr child = virtualNode->getChild(i);
        js << generateNodeJS(child);
    }
    
    return js.str();
}

std::string CHTLJSGenerator::generateEventBinding(const NodePtr& bindingNode) {
    if (!bindingNode || bindingNode->nodeType != CHTLJSNodeType::EVENT_BINDING_NODE) {
        return "";
    }
    
    std::string content = bindingNode->content;
    size_t colonPos = content.find(':');
    if (colonPos != std::string::npos) {
        std::string event = content.substr(0, colonPos);
        std::string handler = content.substr(colonPos + 1);
        
        return "element.addEventListener('" + event + "', function() " + handler + ");";
    }
    
    return "";
}

std::string CHTLJSGenerator::generateNodeJS(const NodePtr& node) {
    if (!node) return "";
    
    switch (node->nodeType) {
        case CHTLJSNodeType::MODULE_NODE:
            return generateModuleCode(node);
        case CHTLJSNodeType::ENHANCED_SELECTOR_NODE:
            return generateEnhancedSelector(node);
        case CHTLJSNodeType::EVENT_LISTENER_NODE:
            return generateEventListener(node);
        case CHTLJSNodeType::EVENT_DELEGATE_NODE:
            return generateEventDelegate(node);
        case CHTLJSNodeType::ANIMATION_NODE:
            return generateAnimation(node);
        case CHTLJSNodeType::VIRTUAL_OBJECT_NODE:
            return generateVirtualObject(node);
        case CHTLJSNodeType::EVENT_BINDING_NODE:
            return generateEventBinding(node);
        default:
            return node->content;
    }
}

std::string CHTLJSGenerator::convertEnhancedSelector(const std::string& selector) {
    std::string jsSelector = selector;
    
    // 转换增强选择器为标准JavaScript
    if (jsSelector.find('.') == 0) {
        // 类选择器 {{.box}} -> document.querySelector('.box')
        return "document.querySelector('" + jsSelector + "')";
    } else if (jsSelector.find('#') == 0) {
        // ID选择器 {{#box}} -> document.getElementById('box')
        std::string id = jsSelector.substr(1);
        return "document.getElementById('" + id + "')";
    } else if (jsSelector.find('[') != std::string::npos) {
        // 索引访问 {{button[0]}} -> document.querySelectorAll('button')[0]
        size_t bracketPos = jsSelector.find('[');
        std::string element = jsSelector.substr(0, bracketPos);
        std::string index = jsSelector.substr(bracketPos);
        return "document.querySelectorAll('" + element + "')" + index;
    } else {
        // 标签选择器 {{button}} -> document.querySelectorAll('button')
        return "document.querySelectorAll('" + jsSelector + "')";
    }
}

std::string CHTLJSGenerator::convertArrowOperator(const std::string& code) {
    // 将 -> 转换为 .
    std::regex arrowPattern(R"(->)");
    return std::regex_replace(code, arrowPattern, ".");
}

std::string CHTLJSGenerator::generateIndentation(size_t level) const {
    std::string result = "";
    for (size_t i = 0; i < level; ++i) {
        result += m_options.indentation;
    }
    return result;
}

std::string CHTLJSGenerator::optimizeGeneratedCode(const std::string& code) {
    std::string result = code;
    
    if (m_options.minifyOutput) {
        // 简单的代码压缩：移除多余空白
        result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
        result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";");
        result = std::regex_replace(result, std::regex(R"(\s*\{\s*)"), "{");
        result = std::regex_replace(result, std::regex(R"(\s*\}\s*)"), "}");
    }
    
    return result;
}

} // namespace CHTLJS