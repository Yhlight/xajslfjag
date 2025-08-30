#include "EnhancedNode.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTLJS {

// ========== CHTLJSEnhancedNode 基类实现 ==========

CHTLJSEnhancedNode::CHTLJSEnhancedNode(CHTLJSNodeType type, const String& value, const CHTL::Position& pos)
    : BaseNode(CHTL::NodeType::CHTLJS_FUNCTION, value, pos), jsType(type) {
}

String CHTLJSEnhancedNode::jsTypeToString() const {
    return nodeTypeToString(jsType);
}

String CHTLJSEnhancedNode::nodeTypeToString(CHTLJSNodeType type) {
    switch (type) {
        case CHTLJSNodeType::LISTEN: return "listen";
        case CHTLJSNodeType::DELEGATE: return "delegate";
        case CHTLJSNodeType::ANIMATE: return "animate";
        case CHTLJSNodeType::VIR: return "vir";
        case CHTLJSNodeType::MODULE: return "module";
        case CHTLJSNodeType::SELECTOR: return "selector";
        case CHTLJSNodeType::EVENT_BIND: return "event_bind";
        case CHTLJSNodeType::TWO_WAY_BIND: return "two_way_bind";
        case CHTLJSNodeType::COMPUTED: return "computed";
        case CHTLJSNodeType::CONDITIONAL: return "conditional";
        case CHTLJSNodeType::LIST_RENDER: return "list_render";
        case CHTLJSNodeType::WATCHER: return "watcher";
        case CHTLJSNodeType::STREAM: return "stream";
        default: return "unknown";
    }
}

// ========== ListenNode 实现 ==========

ListenNode::ListenNode(const CHTL::Position& pos)
    : CHTLJSEnhancedNode(CHTLJSNodeType::LISTEN, "listen", pos), isDeclarative(true) {
}

void ListenNode::addEvent(const String& eventType, const String& handler) {
    if (isValidEventType(eventType) && isValidHandler(handler)) {
        events[eventType] = handler;
    }
}

void ListenNode::setTargetSelector(const String& selector) {
    targetSelector = selector;
}

bool ListenNode::hasEvent(const String& eventType) const {
    return events.find(eventType) != events.end();
}

String ListenNode::generateJavaScript() const {
    std::ostringstream oss;
    
    if (isDeclarative) {
        // 声明式语法: {{selector}}.listen { click: handler, ... }
        oss << "document.querySelector('" << targetSelector << "').addEventListener({\n";
        for (const auto& [eventType, handler] : events) {
            oss << "    '" << eventType << "': " << handler << ",\n";
        }
        oss << "});";
    } else {
        // 传统语法
        for (const auto& [eventType, handler] : events) {
            oss << "document.querySelector('" << targetSelector << "').addEventListener('"
                << eventType << "', " << handler << ");\n";
        }
    }
    
    return oss.str();
}

bool ListenNode::isValidSyntax() const {
    return !targetSelector.empty() && !events.empty();
}

std::unique_ptr<CHTL::BaseNode> ListenNode::clone() const {
    auto cloned = std::make_unique<ListenNode>(position);
    cloned->targetSelector = targetSelector;
    cloned->events = events;
    cloned->isDeclarative = isDeclarative;
    return cloned;
}

std::unique_ptr<ListenNode> ListenNode::createListen(const String& selector, const CHTL::Position& pos) {
    auto node = std::make_unique<ListenNode>(pos);
    node->setTargetSelector(selector);
    return node;
}

std::unordered_map<String, String> ListenNode::parseEventMap(const String& eventMapString) {
    std::unordered_map<String, String> result;
    std::regex eventRegex(R"((\w+):\s*([^,}]+))");
    std::sregex_iterator iter(eventMapString.begin(), eventMapString.end(), eventRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        String eventType = match[1].str();
        String handler = match[2].str();
        
        // 去除前后空白
        eventType.erase(0, eventType.find_first_not_of(" \t"));
        eventType.erase(eventType.find_last_not_of(" \t") + 1);
        handler.erase(0, handler.find_first_not_of(" \t"));
        handler.erase(handler.find_last_not_of(" \t") + 1);
        
        result[eventType] = handler;
    }
    
    return result;
}

bool ListenNode::isListenSyntax(const String& input) {
    return input.find(".listen") != String::npos || input.find("->listen") != String::npos;
}

bool ListenNode::isValidEventType(const String& eventType) const {
    static const StringVector validEvents = {
        "click", "mouseenter", "mouseleave", "mousemove", "mousedown", "mouseup",
        "keydown", "keyup", "keypress", "focus", "blur", "change", "input",
        "submit", "load", "unload", "resize", "scroll", "touchstart", "touchend"
    };
    
    return std::find(validEvents.begin(), validEvents.end(), eventType) != validEvents.end();
}

bool ListenNode::isValidHandler(const String& handler) const {
    return !handler.empty() && (handler.find("function") != String::npos || 
                                handler.find("=>") != String::npos ||
                                handler.find("()") != String::npos);
}

// ========== DelegateNode 实现 ==========

DelegateNode::DelegateNode(const CHTL::Position& pos)
    : CHTLJSEnhancedNode(CHTLJSNodeType::DELEGATE, "delegate", pos) {
}

void DelegateNode::setParentSelector(const String& selector) {
    parentSelector = selector;
}

void DelegateNode::addTargetSelector(const String& selector) {
    if (isValidSelector(selector)) {
        targetSelectors.push_back(selector);
    }
}

void DelegateNode::setTargetSelectors(const StringVector& selectors) {
    targetSelectors.clear();
    for (const auto& selector : selectors) {
        addTargetSelector(selector);
    }
}

void DelegateNode::addEvent(const String& eventType, const String& handler) {
    events[eventType] = handler;
}

String DelegateNode::generateJavaScript() const {
    std::ostringstream oss;
    
    // 生成事件委托注册
    oss << generateDelegateRegistration();
    
    return oss.str();
}

bool DelegateNode::isValidSyntax() const {
    return !parentSelector.empty() && !targetSelectors.empty() && !events.empty();
}

std::unique_ptr<CHTL::BaseNode> DelegateNode::clone() const {
    auto cloned = std::make_unique<DelegateNode>(position);
    cloned->parentSelector = parentSelector;
    cloned->targetSelectors = targetSelectors;
    cloned->events = events;
    return cloned;
}

std::unique_ptr<DelegateNode> DelegateNode::createDelegate(const String& parentSelector, const CHTL::Position& pos) {
    auto node = std::make_unique<DelegateNode>(pos);
    node->setParentSelector(parentSelector);
    return node;
}

StringVector DelegateNode::parseTargetList(const String& targetString) {
    StringVector result;
    std::regex targetRegex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator iter(targetString.begin(), targetString.end(), targetRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        result.push_back(match[1].str());
    }
    
    return result;
}

bool DelegateNode::isDelegateSyntax(const String& input) {
    return input.find(".delegate") != String::npos || input.find("->delegate") != String::npos;
}

String DelegateNode::generateDelegateRegistration() const {
    std::ostringstream oss;
    
    oss << "// Event delegation for " << parentSelector << "\n";
    oss << "(function() {\n";
    oss << "  const parent = document.querySelector('" << parentSelector << "');\n";
    oss << "  if (!parent) return;\n";
    oss << "  \n";
    
    for (const auto& [eventType, handler] : events) {
        oss << "  parent.addEventListener('" << eventType << "', function(e) {\n";
        oss << "    const targets = [";
        
        for (size_t i = 0; i < targetSelectors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "'" << targetSelectors[i] << "'";
        }
        
        oss << "];\n";
        oss << "    for (const selector of targets) {\n";
        oss << "      if (e.target.matches(selector)) {\n";
        oss << "        (" << handler << ")(e);\n";
        oss << "        break;\n";
        oss << "      }\n";
        oss << "    }\n";
        oss << "  });\n";
    }
    
    oss << "})();";
    
    return oss.str();
}

bool DelegateNode::isValidSelector(const String& selector) const {
    return !selector.empty() && selector.find_first_not_of(" \t") != String::npos;
}

// ========== AnimateNode 实现 ==========

AnimateNode::AnimateNode(const CHTL::Position& pos)
    : CHTLJSEnhancedNode(CHTLJSNodeType::ANIMATE, "animate", pos),
      duration(1000), easing("ease"), loop(1), direction("normal"), delay(0) {
}

void AnimateNode::setTarget(const String& selector) {
    targetSelector = selector;
}

void AnimateNode::setBeginState(const std::unordered_map<String, String>& state) {
    beginState = state;
}

void AnimateNode::setEndState(const std::unordered_map<String, String>& state) {
    endState = state;
}

void AnimateNode::addKeyframe(double at, const std::unordered_map<String, String>& state) {
    keyframes.push_back({at, state});
}

String AnimateNode::generateJavaScript() const {
    std::ostringstream oss;
    
    oss << "// Animation for " << targetSelector << "\n";
    oss << "(function() {\n";
    oss << "  const target = document.querySelector('" << targetSelector << "');\n";
    oss << "  if (!target) return;\n";
    oss << "  \n";
    
    // 生成关键帧CSS
    oss << "  const keyframes = [\n";
    
    if (!beginState.empty()) {
        oss << "    { ";
        for (const auto& [prop, value] : beginState) {
            oss << prop << ": '" << value << "', ";
        }
        oss << "},\n";
    }
    
    for (const auto& [at, state] : keyframes) {
        oss << "    { offset: " << at << ", ";
        for (const auto& [prop, value] : state) {
            oss << prop << ": '" << value << "', ";
        }
        oss << "},\n";
    }
    
    if (!endState.empty()) {
        oss << "    { ";
        for (const auto& [prop, value] : endState) {
            oss << prop << ": '" << value << "', ";
        }
        oss << "},\n";
    }
    
    oss << "  ];\n";
    oss << "  \n";
    
    // 生成动画选项
    oss << "  const options = {\n";
    oss << "    duration: " << duration << ",\n";
    oss << "    easing: '" << easing << "',\n";
    if (loop != 1) {
        oss << "    iterations: " << (loop == -1 ? "Infinity" : std::to_string(loop)) << ",\n";
    }
    if (!direction.empty() && direction != "normal") {
        oss << "    direction: '" << direction << "',\n";
    }
    if (delay > 0) {
        oss << "    delay: " << delay << ",\n";
    }
    oss << "  };\n";
    oss << "  \n";
    
    // 创建和播放动画
    oss << "  const animation = target.animate(keyframes, options);\n";
    
    if (!callback.empty()) {
        oss << "  animation.addEventListener('finish', function() {\n";
        oss << "    (" << callback << ")();\n";
        oss << "  });\n";
    }
    
    oss << "})();";
    
    return oss.str();
}

bool AnimateNode::isValidSyntax() const {
    return !targetSelector.empty() && (!beginState.empty() || !endState.empty() || !keyframes.empty());
}

std::unique_ptr<CHTL::BaseNode> AnimateNode::clone() const {
    auto cloned = std::make_unique<AnimateNode>(position);
    cloned->targetSelector = targetSelector;
    cloned->duration = duration;
    cloned->easing = easing;
    cloned->beginState = beginState;
    cloned->endState = endState;
    cloned->keyframes = keyframes;
    cloned->loop = loop;
    cloned->direction = direction;
    cloned->delay = delay;
    cloned->callback = callback;
    return cloned;
}

std::unique_ptr<AnimateNode> AnimateNode::createAnimate(const String& selector, const CHTL::Position& pos) {
    auto node = std::make_unique<AnimateNode>(pos);
    node->setTarget(selector);
    return node;
}

std::unordered_map<String, String> AnimateNode::parseStyleState(const String& stateString) {
    std::unordered_map<String, String> result;
    std::regex styleRegex(R"((\w+(?:-\w+)*):\s*([^;,}]+))");
    std::sregex_iterator iter(stateString.begin(), stateString.end(), styleRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        String property = match[1].str();
        String value = match[2].str();
        
        // 去除前后空白
        property.erase(0, property.find_first_not_of(" \t"));
        property.erase(property.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        result[property] = value;
    }
    
    return result;
}

bool AnimateNode::isAnimateSyntax(const String& input) {
    return input.find("animate") != String::npos;
}

bool AnimateNode::isValidCSSProperty(const String& property) const {
    // 简化的CSS属性验证
    static const StringVector validProps = {
        "opacity", "transform", "width", "height", "left", "top", "right", "bottom",
        "margin", "padding", "background-color", "color", "font-size", "border-radius"
    };
    
    return std::find(validProps.begin(), validProps.end(), property) != validProps.end();
}

// ========== VirNode 实现 ==========

VirNode::VirNode(const String& name, const CHTL::Position& pos)
    : CHTLJSEnhancedNode(CHTLJSNodeType::VIR, "vir " + name, pos),
      objectName(name), sourceType(CHTLJSNodeType::LISTEN), isCompileTime(true) {
}

void VirNode::addProperty(const String& name, const String& value) {
    properties[name] = value;
}

void VirNode::addMethod(const String& name, const String& signature) {
    methods[name] = signature;
}

bool VirNode::hasProperty(const String& name) const {
    return properties.find(name) != properties.end();
}

bool VirNode::hasMethod(const String& name) const {
    return methods.find(name) != methods.end();
}

String VirNode::getProperty(const String& name) const {
    auto it = properties.find(name);
    return it != properties.end() ? it->second : "";
}

String VirNode::getMethod(const String& name) const {
    auto it = methods.find(name);
    return it != methods.end() ? it->second : "";
}

String VirNode::generateJavaScript() const {
    if (isCompileTime) {
        // 编译时处理，生成访问代码
        std::ostringstream oss;
        oss << "// Virtual object " << objectName << " (compile-time)\n";
        oss << "// Properties: ";
        for (const auto& [name, value] : properties) {
            oss << name << " ";
        }
        oss << "\n// Methods: ";
        for (const auto& [name, sig] : methods) {
            oss << name << " ";
        }
        return oss.str();
    } else {
        // 运行时处理，不生成代码
        return "// Virtual object " + objectName + " (runtime processing)";
    }
}

bool VirNode::isValidSyntax() const {
    return isValidObjectName(objectName) && (!properties.empty() || !methods.empty());
}

std::unique_ptr<CHTL::BaseNode> VirNode::clone() const {
    auto cloned = std::make_unique<VirNode>(objectName, position);
    cloned->sourceType = sourceType;
    cloned->properties = properties;
    cloned->methods = methods;
    cloned->isCompileTime = isCompileTime;
    return cloned;
}

std::unique_ptr<VirNode> VirNode::createVir(const String& name, CHTLJSNodeType sourceType, const CHTL::Position& pos) {
    auto node = std::make_unique<VirNode>(name, pos);
    node->setSourceType(sourceType);
    return node;
}

std::pair<String, CHTLJSNodeType> VirNode::parseVirDeclaration(const String& declaration) {
    std::regex virRegex(R"(vir\s+(\w+)\s*=\s*(\w+))");
    std::smatch match;
    
    if (std::regex_match(declaration, match, virRegex)) {
        String name = match[1].str();
        String sourceTypeName = match[2].str();
        
        CHTLJSNodeType sourceType = CHTLJSNodeType::LISTEN;
        if (sourceTypeName == "listen") sourceType = CHTLJSNodeType::LISTEN;
        else if (sourceTypeName == "animate") sourceType = CHTLJSNodeType::ANIMATE;
        else if (sourceTypeName == "delegate") sourceType = CHTLJSNodeType::DELEGATE;
        
        return {name, sourceType};
    }
    
    return {"", CHTLJSNodeType::LISTEN};
}

bool VirNode::isVirSyntax(const String& input) {
    return input.find("vir ") != String::npos;
}

bool VirNode::isValidObjectName(const String& name) const {
    return !name.empty() && name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") == String::npos;
}

// ========== ModuleNode 实现 ==========

ModuleNode::ModuleNode(const CHTL::Position& pos)
    : CHTLJSEnhancedNode(CHTLJSNodeType::MODULE, "module", pos),
      isChainSyntax(false), loadMode("parallel") {
}

void ModuleNode::addLoadPath(const String& path) {
    if (isValidPath(path)) {
        loadPaths.push_back(path);
    }
}

void ModuleNode::setLoadPaths(const StringVector& paths) {
    loadPaths.clear();
    for (const auto& path : paths) {
        addLoadPath(path);
    }
}

String ModuleNode::generateJavaScript() const {
    return generateAMDLoader();
}

bool ModuleNode::isValidSyntax() const {
    return !loadPaths.empty();
}

std::unique_ptr<CHTL::BaseNode> ModuleNode::clone() const {
    auto cloned = std::make_unique<ModuleNode>(position);
    cloned->loadPaths = loadPaths;
    cloned->isChainSyntax = isChainSyntax;
    cloned->loadMode = loadMode;
    return cloned;
}

std::unique_ptr<ModuleNode> ModuleNode::createModule(const CHTL::Position& pos) {
    return std::make_unique<ModuleNode>(pos);
}

StringVector ModuleNode::parseLoadPaths(const String& pathString) {
    StringVector result;
    std::regex pathRegex(R"(load:\s*([^,\n]+))");
    std::sregex_iterator iter(pathString.begin(), pathString.end(), pathRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        String path = match[1].str();
        
        // 去除前后空白和引号
        path.erase(0, path.find_first_not_of(" \t\"'"));
        path.erase(path.find_last_not_of(" \t\"'") + 1);
        
        result.push_back(path);
    }
    
    return result;
}

bool ModuleNode::isModuleSyntax(const String& input) {
    return input.find("module") != String::npos && input.find("load:") != String::npos;
}

String ModuleNode::generateAMDLoader() const {
    std::ostringstream oss;
    
    oss << "// AMD-style module loader\n";
    oss << "(function() {\n";
    oss << "  const loadedModules = new Set();\n";
    oss << "  const loadQueue = [];\n";
    oss << "  \n";
    
    // 添加加载路径
    for (const auto& path : loadPaths) {
        oss << "  loadQueue.push('" << path << "');\n";
    }
    
    oss << "  \n";
    oss << "  function loadModule(path) {\n";
    oss << "    if (loadedModules.has(path)) return Promise.resolve();\n";
    oss << "    \n";
    oss << "    return new Promise((resolve, reject) => {\n";
    oss << "      const script = document.createElement('script');\n";
    oss << "      script.src = path;\n";
    oss << "      script.onload = () => {\n";
    oss << "        loadedModules.add(path);\n";
    oss << "        resolve();\n";
    oss << "      };\n";
    oss << "      script.onerror = reject;\n";
    oss << "      document.head.appendChild(script);\n";
    oss << "    });\n";
    oss << "  }\n";
    oss << "  \n";
    
    if (loadMode == "sequential") {
        oss << "  // Sequential loading\n";
        oss << "  loadQueue.reduce((promise, path) => {\n";
        oss << "    return promise.then(() => loadModule(path));\n";
        oss << "  }, Promise.resolve());\n";
    } else {
        oss << "  // Parallel loading\n";
        oss << "  Promise.all(loadQueue.map(loadModule));\n";
    }
    
    oss << "})();";
    
    return oss.str();
}

bool ModuleNode::isValidPath(const String& path) const {
    return !path.empty() && path.find("..") == String::npos; // 基本安全检查
}

// ========== SelectorNode 实现 ==========

SelectorNode::SelectorNode(const String& sel, const CHTL::Position& pos)
    : CHTLJSEnhancedNode(CHTLJSNodeType::SELECTOR, sel, pos),
      selector(sel), isIndexed(false), index(0), isPreciseAccess(false) {
}

void SelectorNode::setAccessMethod(const String& method) {
    accessMethod = method;
}

void SelectorNode::setIndexedAccess(size_t idx) {
    isIndexed = true;
    index = idx;
}

String SelectorNode::generateJavaScript() const {
    return generateDOMQuery();
}

bool SelectorNode::isValidSyntax() const {
    return isValidCSSSelector(selector);
}

std::unique_ptr<CHTL::BaseNode> SelectorNode::clone() const {
    auto cloned = std::make_unique<SelectorNode>(selector, position);
    cloned->accessMethod = accessMethod;
    cloned->isIndexed = isIndexed;
    cloned->index = index;
    cloned->isPreciseAccess = isPreciseAccess;
    return cloned;
}

std::unique_ptr<SelectorNode> SelectorNode::createSelector(const String& selector, const CHTL::Position& pos) {
    return std::make_unique<SelectorNode>(selector, pos);
}

std::pair<String, size_t> SelectorNode::parseIndexedSelector(const String& selectorString) {
    std::regex indexRegex(R"(^(.+)\[(\d+)\]$)");
    std::smatch match;
    
    if (std::regex_match(selectorString, match, indexRegex)) {
        String sel = match[1].str();
        size_t idx = std::stoul(match[2].str());
        return {sel, idx};
    }
    
    return {selectorString, 0};
}

bool SelectorNode::isSelectorSyntax(const String& input) {
    return input.find("{{") != String::npos && input.find("}}") != String::npos;
}

bool SelectorNode::isValidCSSSelector(const String& selector) {
    // 简化的CSS选择器验证
    return !selector.empty() && 
           selector.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.#[]():+ ~>") == String::npos;
}

String SelectorNode::generateDOMQuery() const {
    std::ostringstream oss;
    
    String optimizedSelector = optimizeSelector();
    
    if (isIndexed) {
        if (isPreciseAccess) {
            oss << "document.querySelectorAll('" << optimizedSelector << "')[" << index << "]";
        } else {
            oss << "document.querySelectorAll('" << optimizedSelector << "')[" << index << "]";
        }
    } else {
        if (optimizedSelector.find("#") == 0) {
            // ID选择器优化
            oss << "document.getElementById('" << optimizedSelector.substr(1) << "')";
        } else if (optimizedSelector.find(".") == 0) {
            // 类选择器优化
            oss << "document.getElementsByClassName('" << optimizedSelector.substr(1) << "')[0]";
        } else if (optimizedSelector.find_first_of(".#[]():+~>") == String::npos) {
            // 标签选择器优化
            oss << "document.getElementsByTagName('" << optimizedSelector << "')[0]";
        } else {
            // 通用选择器
            oss << "document.querySelector('" << optimizedSelector << "')";
        }
    }
    
    return oss.str();
}

String SelectorNode::optimizeSelector() const {
    // 基本的选择器优化
    String optimized = selector;
    
    // 去除多余的空格
    std::regex multiSpace(R"(\s+)");
    optimized = std::regex_replace(optimized, multiSpace, " ");
    
    // 去除前后空格
    optimized.erase(0, optimized.find_first_not_of(" \t"));
    optimized.erase(optimized.find_last_not_of(" \t") + 1);
    
    return optimized;
}

} // namespace CHTLJS