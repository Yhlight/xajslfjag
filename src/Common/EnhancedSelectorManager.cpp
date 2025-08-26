#include "EnhancedSelectorManager.h"
#include "GlobalMap.h"
#include "Context.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <queue>

namespace CHTL {

// ===== EnhancedSelectorManager Implementation =====

EnhancedSelectorManager::EnhancedSelectorManager()
    : enable_precise_access(true)
    , enable_complex_selectors(true)
    , enable_dom_tree_cache(true)
    , enable_selector_optimization(true)
    , max_selector_depth(20)
    , cache_size_limit(1000) {
    initializeSelectorPatterns();
    initializeBuiltinSelectors();
}

void EnhancedSelectorManager::initializeSelectorPatterns() {
    // 初始化选择器模式
    
    // 精确访问模式
    precise_access_patterns = {
        std::regex(R"(\{\{([^}]+)\[(\d+)\]\}\})"),           // {{selector[index]}}
        std::regex(R"(\{\{([^}]+)\[(-?\d+)\]\}\})"),         // {{selector[-index]}}
        std::regex(R"(\{\{([^}]+)\[(\d+):(\d+)\]\}\})"),     // {{selector[start:end]}}
        std::regex(R"(\{\{([^}]+)\[([^]]+)\]\}\})"),         // {{selector[attribute]}}
        std::regex(R"(\{\{([^}]+)\[([^=]+)=([^]]+)\]\}\})"), // {{selector[attr=value]}}
    };
    
    // 复杂选择器模式
    complex_selector_patterns = {
        std::regex(R"(\{\{([^}]+)\s+([^}]+)\}\})"),          // {{parent child}}
        std::regex(R"(\{\{([^}]+)\s*>\s*([^}]+)\}\})"),      // {{parent > child}}
        std::regex(R"(\{\{([^}]+)\s*\+\s*([^}]+)\}\})"),     // {{sibling + adjacent}}
        std::regex(R"(\{\{([^}]+)\s*~\s*([^}]+)\}\})"),      // {{sibling ~ general}}
        std::regex(R"(\{\{([^}]+)\s*,\s*([^}]+)\}\})"),      // {{selector1, selector2}}
    };
    
    // 属性选择器模式
    attribute_selector_patterns = {
        std::regex(R"(\[([^=]+)\])"),                        // [attribute]
        std::regex(R"(\[([^=]+)=([^]]+)\])"),               // [attribute=value]
        std::regex(R"(\[([^=]+)\^=([^]]+)\])"),             // [attribute^=value]
        std::regex(R"(\[([^=]+)\$=([^]]+)\])"),             // [attribute$=value]
        std::regex(R"(\[([^=]+)\*=([^]]+)\])"),             // [attribute*=value]
        std::regex(R"(\[([^=]+)~=([^]]+)\])"),              // [attribute~=value]
        std::regex(R"(\[([^=]+)\|=([^]]+)\])"),             // [attribute|=value]
    };
}

void EnhancedSelectorManager::initializeBuiltinSelectors() {
    // 初始化内置选择器验证规则
    
    // HTML元素选择器
    html_element_selectors = {
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "table", "tr", "td", "th", "thead", "tbody",
        "form", "input", "button", "select", "option", "textarea",
        "img", "a", "nav", "header", "footer", "main", "section", "article",
        "aside", "figure", "figcaption", "canvas", "svg", "video", "audio"
    };
    
    // 伪类选择器
    pseudo_class_selectors = {
        ":hover", ":active", ":focus", ":visited", ":link",
        ":first-child", ":last-child", ":nth-child", ":nth-of-type",
        ":first-of-type", ":last-of-type", ":only-child", ":only-of-type",
        ":root", ":empty", ":checked", ":disabled", ":enabled"
    };
    
    // 伪元素选择器
    pseudo_element_selectors = {
        "::before", "::after", "::first-line", "::first-letter",
        "::placeholder", "::selection", "::backdrop"
    };
}

SelectorParseResult EnhancedSelectorManager::parseSelector(const std::string& selector) {
    SelectorParseResult result;
    result.success = false;
    
    if (selector.empty()) {
        result.errors.push_back("选择器不能为空");
        return result;
    }
    
    // 检查是否为增强选择器（以{{开头和}}结尾）
    if (selector.substr(0, 2) == "{{" && selector.substr(selector.length() - 2) == "}}") {
        result = parseEnhancedSelector(selector);
    } else {
        result = parseStandardSelector(selector);
    }
    
    // 验证解析结果
    if (result.success) {
        if (!validateSelectorSyntax(result)) {
            result.success = false;
            result.errors.push_back("选择器语法验证失败");
        }
    }
    
    return result;
}

SelectorParseResult EnhancedSelectorManager::parseEnhancedSelector(const std::string& selector) {
    SelectorParseResult result;
    result.type = SelectorType::ENHANCED;
    result.original_selector = selector;
    
    // 移除 {{ 和 }}
    std::string inner_selector = selector.substr(2, selector.length() - 4);
    
    // 检查精确访问模式
    for (const auto& pattern : precise_access_patterns) {
        std::smatch matches;
        if (std::regex_match(inner_selector, matches, pattern)) {
            result.precise_access_type = determinePreciseAccessType(matches);
            result.base_selector = matches[1].str();
            
            if (matches.size() > 2) {
                result.access_parameter = matches[2].str();
            }
            if (matches.size() > 3) {
                result.secondary_parameter = matches[3].str();
            }
            
            result.success = true;
            return result;
        }
    }
    
    // 检查复杂选择器模式
    for (const auto& pattern : complex_selector_patterns) {
        std::smatch matches;
        if (std::regex_match(inner_selector, matches, pattern)) {
            result.type = SelectorType::COMPLEX;
            result.base_selector = matches[1].str();
            result.secondary_selector = matches[2].str();
            result.relationship = determineRelationship(inner_selector);
            result.success = true;
            return result;
        }
    }
    
    // 检查属性选择器
    for (const auto& pattern : attribute_selector_patterns) {
        std::smatch matches;
        if (std::regex_search(inner_selector, matches, pattern)) {
            result.type = SelectorType::ATTRIBUTE;
            result.attribute_name = matches[1].str();
            if (matches.size() > 2) {
                result.attribute_value = matches[2].str();
            }
            result.success = true;
            return result;
        }
    }
    
    // 默认为简单增强选择器
    result.type = SelectorType::ENHANCED;
    result.base_selector = inner_selector;
    result.success = true;
    
    return result;
}

SelectorParseResult EnhancedSelectorManager::parseStandardSelector(const std::string& selector) {
    SelectorParseResult result;
    result.type = SelectorType::STANDARD;
    result.original_selector = selector;
    result.base_selector = selector;
    
    // 检查是否为类选择器
    if (selector[0] == '.') {
        result.type = SelectorType::CLASS;
        result.class_name = selector.substr(1);
    }
    // 检查是否为ID选择器
    else if (selector[0] == '#') {
        result.type = SelectorType::ID;
        result.id_name = selector.substr(1);
    }
    // 检查是否为HTML元素选择器
    else if (html_element_selectors.find(selector) != html_element_selectors.end()) {
        result.type = SelectorType::ELEMENT;
        result.element_type = selector;
    }
    
    result.success = true;
    return result;
}

PreciseAccessType EnhancedSelectorManager::determinePreciseAccessType(const std::smatch& matches) {
    if (matches.size() < 3) return PreciseAccessType::NONE;
    
    std::string access_param = matches[2].str();
    
    // 检查是否为数字索引
    if (std::regex_match(access_param, std::regex(R"(-?\d+)"))) {
        return PreciseAccessType::INDEX;
    }
    
    // 检查是否为范围访问
    if (matches.size() > 3) {
        return PreciseAccessType::RANGE;
    }
    
    // 检查是否为属性访问
    if (access_param.find('=') != std::string::npos) {
        return PreciseAccessType::ATTRIBUTE_VALUE;
    } else {
        return PreciseAccessType::ATTRIBUTE_NAME;
    }
}

std::string EnhancedSelectorManager::determineRelationship(const std::string& selector) {
    if (selector.find(" > ") != std::string::npos) return "child";
    if (selector.find(" + ") != std::string::npos) return "adjacent";
    if (selector.find(" ~ ") != std::string::npos) return "general";
    if (selector.find(", ") != std::string::npos) return "group";
    if (selector.find(" ") != std::string::npos) return "descendant";
    
    return "none";
}

std::vector<DOMElementInfo> EnhancedSelectorManager::matchElements(const SelectorParseResult& selector_result) {
    std::vector<DOMElementInfo> matched_elements;
    
    if (!selector_result.success) {
        return matched_elements;
    }
    
    // 根据选择器类型进行匹配
    switch (selector_result.type) {
        case SelectorType::ENHANCED:
            matched_elements = matchEnhancedSelector(selector_result);
            break;
            
        case SelectorType::COMPLEX:
            matched_elements = matchComplexSelector(selector_result);
            break;
            
        case SelectorType::ATTRIBUTE:
            matched_elements = matchAttributeSelector(selector_result);
            break;
            
        case SelectorType::CLASS:
        case SelectorType::ID:
        case SelectorType::ELEMENT:
        case SelectorType::STANDARD:
            matched_elements = matchStandardSelector(selector_result);
            break;
            
        default:
            break;
    }
    
    // 应用精确访问
    if (selector_result.precise_access_type != PreciseAccessType::NONE) {
        matched_elements = applyPreciseAccess(matched_elements, selector_result);
    }
    
    return matched_elements;
}

std::vector<DOMElementInfo> EnhancedSelectorManager::matchEnhancedSelector(const SelectorParseResult& selector_result) {
    std::vector<DOMElementInfo> matched_elements;
    
    // 从DOM树中查找匹配的元素
    if (enable_dom_tree_cache && dom_tree_cache.find(selector_result.base_selector) != dom_tree_cache.end()) {
        matched_elements = dom_tree_cache[selector_result.base_selector];
    } else {
        // 遍历DOM树查找匹配元素
        matched_elements = traverseDOMTree(selector_result.base_selector);
        
        // 缓存结果
        if (enable_dom_tree_cache && matched_elements.size() <= cache_size_limit) {
            dom_tree_cache[selector_result.base_selector] = matched_elements;
        }
    }
    
    return matched_elements;
}

std::vector<DOMElementInfo> EnhancedSelectorManager::matchComplexSelector(const SelectorParseResult& selector_result) {
    std::vector<DOMElementInfo> matched_elements;
    
    // 首先匹配基础选择器
    auto base_elements = matchSelector(selector_result.base_selector);
    
    // 然后根据关系类型匹配相关元素
    for (const auto& base_element : base_elements) {
        auto related_elements = findRelatedElements(base_element, selector_result.secondary_selector, selector_result.relationship);
        matched_elements.insert(matched_elements.end(), related_elements.begin(), related_elements.end());
    }
    
    return matched_elements;
}

std::vector<DOMElementInfo> EnhancedSelectorManager::matchAttributeSelector(const SelectorParseResult& selector_result) {
    std::vector<DOMElementInfo> matched_elements;
    
    // 遍历所有元素，查找具有指定属性的元素
    for (const auto& [element_id, element_info] : dom_elements) {
        if (matchesAttributeSelector(element_info, selector_result)) {
            matched_elements.push_back(element_info);
        }
    }
    
    return matched_elements;
}

std::vector<DOMElementInfo> EnhancedSelectorManager::matchStandardSelector(const SelectorParseResult& selector_result) {
    std::vector<DOMElementInfo> matched_elements;
    
    for (const auto& [element_id, element_info] : dom_elements) {
        bool matches = false;
        
        switch (selector_result.type) {
            case SelectorType::CLASS:
                matches = elementHasClass(element_info, selector_result.class_name);
                break;
                
            case SelectorType::ID:
                matches = (element_info.id == selector_result.id_name);
                break;
                
            case SelectorType::ELEMENT:
                matches = (element_info.tag_name == selector_result.element_type);
                break;
                
            default:
                matches = (element_info.tag_name == selector_result.base_selector ||
                          element_info.id == selector_result.base_selector ||
                          elementHasClass(element_info, selector_result.base_selector));
                break;
        }
        
        if (matches) {
            matched_elements.push_back(element_info);
        }
    }
    
    return matched_elements;
}

std::vector<DOMElementInfo> EnhancedSelectorManager::applyPreciseAccess(
    const std::vector<DOMElementInfo>& elements, 
    const SelectorParseResult& selector_result) {
    
    std::vector<DOMElementInfo> result;
    
    switch (selector_result.precise_access_type) {
        case PreciseAccessType::INDEX:
            {
                int index = std::stoi(selector_result.access_parameter);
                if (index < 0) {
                    index = static_cast<int>(elements.size()) + index; // 负索引
                }
                if (index >= 0 && index < static_cast<int>(elements.size())) {
                    result.push_back(elements[index]);
                }
            }
            break;
            
        case PreciseAccessType::RANGE:
            {
                int start = std::stoi(selector_result.access_parameter);
                int end = std::stoi(selector_result.secondary_parameter);
                
                start = std::max(0, std::min(start, static_cast<int>(elements.size())));
                end = std::max(0, std::min(end, static_cast<int>(elements.size())));
                
                for (int i = start; i < end && i < static_cast<int>(elements.size()); ++i) {
                    result.push_back(elements[i]);
                }
            }
            break;
            
        case PreciseAccessType::ATTRIBUTE_NAME:
            for (const auto& element : elements) {
                if (element.attributes.find(selector_result.access_parameter) != element.attributes.end()) {
                    result.push_back(element);
                }
            }
            break;
            
        case PreciseAccessType::ATTRIBUTE_VALUE:
            for (const auto& element : elements) {
                auto attr_it = element.attributes.find(selector_result.attribute_name);
                if (attr_it != element.attributes.end() && attr_it->second == selector_result.attribute_value) {
                    result.push_back(element);
                }
            }
            break;
            
        default:
            result = elements;
            break;
    }
    
    return result;
}

std::vector<DOMElementInfo> EnhancedSelectorManager::findRelatedElements(
    const DOMElementInfo& base_element, 
    const std::string& target_selector, 
    const std::string& relationship) {
    
    std::vector<DOMElementInfo> related_elements;
    
    if (relationship == "child") {
        // 查找直接子元素
        for (const auto& child_id : base_element.children) {
            auto child_it = dom_elements.find(child_id);
            if (child_it != dom_elements.end() && matchesSelector(child_it->second, target_selector)) {
                related_elements.push_back(child_it->second);
            }
        }
    }
    else if (relationship == "descendant") {
        // 查找所有后代元素
        std::queue<std::string> queue;
        for (const auto& child_id : base_element.children) {
            queue.push(child_id);
        }
        
        while (!queue.empty()) {
            std::string current_id = queue.front();
            queue.pop();
            
            auto element_it = dom_elements.find(current_id);
            if (element_it != dom_elements.end()) {
                if (matchesSelector(element_it->second, target_selector)) {
                    related_elements.push_back(element_it->second);
                }
                
                // 添加子元素到队列
                for (const auto& child_id : element_it->second.children) {
                    queue.push(child_id);
                }
            }
        }
    }
    else if (relationship == "adjacent") {
        // 查找相邻兄弟元素
        auto parent_it = dom_elements.find(base_element.parent_id);
        if (parent_it != dom_elements.end()) {
            const auto& siblings = parent_it->second.children;
            auto base_pos = std::find(siblings.begin(), siblings.end(), base_element.element_id);
            if (base_pos != siblings.end() && std::next(base_pos) != siblings.end()) {
                auto adjacent_it = dom_elements.find(*std::next(base_pos));
                if (adjacent_it != dom_elements.end() && matchesSelector(adjacent_it->second, target_selector)) {
                    related_elements.push_back(adjacent_it->second);
                }
            }
        }
    }
    else if (relationship == "general") {
        // 查找后续所有兄弟元素
        auto parent_it = dom_elements.find(base_element.parent_id);
        if (parent_it != dom_elements.end()) {
            const auto& siblings = parent_it->second.children;
            auto base_pos = std::find(siblings.begin(), siblings.end(), base_element.element_id);
            if (base_pos != siblings.end()) {
                for (auto it = std::next(base_pos); it != siblings.end(); ++it) {
                    auto sibling_it = dom_elements.find(*it);
                    if (sibling_it != dom_elements.end() && matchesSelector(sibling_it->second, target_selector)) {
                        related_elements.push_back(sibling_it->second);
                    }
                }
            }
        }
    }
    
    return related_elements;
}

bool EnhancedSelectorManager::matchesSelector(const DOMElementInfo& element, const std::string& selector) {
    auto parse_result = parseSelector(selector);
    if (!parse_result.success) return false;
    
    auto matched_elements = matchElements(parse_result);
    
    return std::any_of(matched_elements.begin(), matched_elements.end(),
                       [&element](const DOMElementInfo& matched) {
                           return matched.element_id == element.element_id;
                       });
}

bool EnhancedSelectorManager::matchesAttributeSelector(const DOMElementInfo& element, const SelectorParseResult& selector_result) {
    const std::string& attr_name = selector_result.attribute_name;
    const std::string& attr_value = selector_result.attribute_value;
    
    auto attr_it = element.attributes.find(attr_name);
    if (attr_it == element.attributes.end()) {
        return false; // 属性不存在
    }
    
    if (attr_value.empty()) {
        return true; // 只检查属性存在
    }
    
    // 根据选择器类型进行匹配
    const std::string& element_value = attr_it->second;
    
    // 这里可以根据属性选择器的具体类型（=, ^=, $=, *=, ~=, |=）进行不同的匹配
    // 当前简化为精确匹配
    return element_value == attr_value;
}

bool EnhancedSelectorManager::elementHasClass(const DOMElementInfo& element, const std::string& class_name) {
    auto class_attr = element.attributes.find("class");
    if (class_attr == element.attributes.end()) {
        return false;
    }
    
    // 分割类名列表
    std::stringstream ss(class_attr->second);
    std::string class_item;
    
    while (ss >> class_item) {
        if (class_item == class_name) {
            return true;
        }
    }
    
    return false;
}

std::vector<DOMElementInfo> EnhancedSelectorManager::traverseDOMTree(const std::string& selector) {
    std::vector<DOMElementInfo> matched_elements;
    
    // 从根元素开始遍历
    for (const auto& [element_id, element_info] : dom_elements) {
        if (element_info.parent_id.empty()) { // 根元素
            traverseDOMTreeHelper(element_info, selector, matched_elements);
        }
    }
    
    return matched_elements;
}

void EnhancedSelectorManager::traverseDOMTreeHelper(const DOMElementInfo& element, const std::string& selector, std::vector<DOMElementInfo>& matched_elements) {
    // 检查当前元素是否匹配
    if (matchesSelector(element, selector)) {
        matched_elements.push_back(element);
    }
    
    // 递归遍历子元素
    for (const auto& child_id : element.children) {
        auto child_it = dom_elements.find(child_id);
        if (child_it != dom_elements.end()) {
            traverseDOMTreeHelper(child_it->second, selector, matched_elements);
        }
    }
}

std::string EnhancedSelectorManager::generateCSS(const SelectorParseResult& selector_result) {
    if (!selector_result.success) {
        return "";
    }
    
    std::stringstream css;
    
    switch (selector_result.type) {
        case SelectorType::ENHANCED:
            css << generateEnhancedCSS(selector_result);
            break;
            
        case SelectorType::COMPLEX:
            css << generateComplexCSS(selector_result);
            break;
            
        case SelectorType::ATTRIBUTE:
            css << generateAttributeCSS(selector_result);
            break;
            
        default:
            css << selector_result.base_selector;
            break;
    }
    
    return css.str();
}

std::string EnhancedSelectorManager::generateEnhancedCSS(const SelectorParseResult& selector_result) {
    std::string css_selector = selector_result.base_selector;
    
    // 转换为标准CSS选择器
    if (css_selector.substr(0, 1) == ".") {
        // 已经是类选择器
    } else if (css_selector.substr(0, 1) == "#") {
        // 已经是ID选择器
    } else {
        // 假设为类选择器，添加点号
        css_selector = "." + css_selector;
    }
    
    // 处理精确访问
    if (selector_result.precise_access_type == PreciseAccessType::INDEX) {
        int index = std::stoi(selector_result.access_parameter);
        if (index >= 0) {
            css_selector += ":nth-child(" + std::to_string(index + 1) + ")";
        } else {
            css_selector += ":nth-last-child(" + std::to_string(-index) + ")";
        }
    }
    
    return css_selector;
}

std::string EnhancedSelectorManager::generateComplexCSS(const SelectorParseResult& selector_result) {
    std::string base_css = generateCSS({SelectorType::ENHANCED, selector_result.base_selector, true});
    std::string secondary_css = generateCSS({SelectorType::ENHANCED, selector_result.secondary_selector, true});
    
    if (selector_result.relationship == "child") {
        return base_css + " > " + secondary_css;
    } else if (selector_result.relationship == "descendant") {
        return base_css + " " + secondary_css;
    } else if (selector_result.relationship == "adjacent") {
        return base_css + " + " + secondary_css;
    } else if (selector_result.relationship == "general") {
        return base_css + " ~ " + secondary_css;
    } else if (selector_result.relationship == "group") {
        return base_css + ", " + secondary_css;
    }
    
    return base_css + " " + secondary_css;
}

std::string EnhancedSelectorManager::generateAttributeCSS(const SelectorParseResult& selector_result) {
    std::string css = selector_result.base_selector;
    
    if (!selector_result.attribute_name.empty()) {
        css += "[" + selector_result.attribute_name;
        
        if (!selector_result.attribute_value.empty()) {
            css += "=\"" + selector_result.attribute_value + "\"";
        }
        
        css += "]";
    }
    
    return css;
}

std::string EnhancedSelectorManager::generateJavaScript(const SelectorParseResult& selector_result) {
    if (!selector_result.success) {
        return "";
    }
    
    std::stringstream js;
    
    // 生成 document.querySelector 或 document.querySelectorAll 调用
    std::string css_selector = generateCSS(selector_result);
    
    if (selector_result.precise_access_type == PreciseAccessType::INDEX) {
        // 对于索引访问，使用 querySelector 获取单个元素
        js << "document.querySelector('" << escapeJavaScriptString(css_selector) << "')";
    } else {
        // 对于其他情况，使用 querySelectorAll 获取所有匹配元素
        js << "document.querySelectorAll('" << escapeJavaScriptString(css_selector) << "')";
    }
    
    return js.str();
}

// DOM管理方法
void EnhancedSelectorManager::addElement(const DOMElementInfo& element) {
    dom_elements[element.element_id] = element;
    
    // 清除相关缓存
    if (enable_dom_tree_cache) {
        clearSelectorsCache({element.tag_name, element.id, "." + joinClasses(element)});
    }
}

void EnhancedSelectorManager::removeElement(const std::string& element_id) {
    auto it = dom_elements.find(element_id);
    if (it != dom_elements.end()) {
        const auto& element = it->second;
        
        // 从父元素的子列表中移除
        if (!element.parent_id.empty()) {
            auto parent_it = dom_elements.find(element.parent_id);
            if (parent_it != dom_elements.end()) {
                auto& children = parent_it->second.children;
                children.erase(std::remove(children.begin(), children.end(), element_id), children.end());
            }
        }
        
        // 递归移除子元素
        for (const auto& child_id : element.children) {
            removeElement(child_id);
        }
        
        // 清除相关缓存
        if (enable_dom_tree_cache) {
            clearSelectorsCache({element.tag_name, element.id, "." + joinClasses(element)});
        }
        
        dom_elements.erase(it);
    }
}

void EnhancedSelectorManager::updateElement(const DOMElementInfo& element) {
    auto it = dom_elements.find(element.element_id);
    if (it != dom_elements.end()) {
        // 清除旧的缓存
        if (enable_dom_tree_cache) {
            clearSelectorsCache({it->second.tag_name, it->second.id, "." + joinClasses(it->second)});
        }
        
        // 更新元素
        it->second = element;
        
        // 清除新的缓存
        clearSelectorsCache({element.tag_name, element.id, "." + joinClasses(element)});
    } else {
        addElement(element);
    }
}

DOMElementInfo EnhancedSelectorManager::getElement(const std::string& element_id) const {
    auto it = dom_elements.find(element_id);
    if (it != dom_elements.end()) {
        return it->second;
    }
    return DOMElementInfo(); // 返回空元素
}

bool EnhancedSelectorManager::hasElement(const std::string& element_id) const {
    return dom_elements.find(element_id) != dom_elements.end();
}

std::vector<DOMElementInfo> EnhancedSelectorManager::getAllElements() const {
    std::vector<DOMElementInfo> elements;
    for (const auto& [id, element] : dom_elements) {
        elements.push_back(element);
    }
    return elements;
}

std::vector<DOMElementInfo> EnhancedSelectorManager::matchSelector(const std::string& selector) {
    auto parse_result = parseSelector(selector);
    if (parse_result.success) {
        return matchElements(parse_result);
    }
    return {};
}

// 配置方法
void EnhancedSelectorManager::enablePreciseAccess(bool enable) {
    enable_precise_access = enable;
}

void EnhancedSelectorManager::enableComplexSelectors(bool enable) {
    enable_complex_selectors = enable;
}

void EnhancedSelectorManager::enableDOMTreeCache(bool enable) {
    enable_dom_tree_cache = enable;
    if (!enable) {
        dom_tree_cache.clear();
    }
}

void EnhancedSelectorManager::enableSelectorOptimization(bool enable) {
    enable_selector_optimization = enable;
}

void EnhancedSelectorManager::setMaxSelectorDepth(size_t depth) {
    max_selector_depth = depth;
}

void EnhancedSelectorManager::setCacheSizeLimit(size_t limit) {
    cache_size_limit = limit;
    
    // 如果当前缓存超出限制，清理缓存
    if (dom_tree_cache.size() > cache_size_limit) {
        dom_tree_cache.clear();
    }
}

void EnhancedSelectorManager::clearCache() {
    dom_tree_cache.clear();
}

void EnhancedSelectorManager::clearSelectorsCache(const std::vector<std::string>& selectors) {
    for (const auto& selector : selectors) {
        dom_tree_cache.erase(selector);
    }
}

// 验证方法
bool EnhancedSelectorManager::validateSelectorSyntax(const SelectorParseResult& result) const {
    if (!result.success) return false;
    
    // 验证基础选择器
    if (result.base_selector.empty()) return false;
    
    // 验证精确访问参数
    if (result.precise_access_type != PreciseAccessType::NONE) {
        if (result.access_parameter.empty()) return false;
        
        if (result.precise_access_type == PreciseAccessType::INDEX || 
            result.precise_access_type == PreciseAccessType::RANGE) {
            try {
                std::stoi(result.access_parameter);
                if (result.precise_access_type == PreciseAccessType::RANGE && !result.secondary_parameter.empty()) {
                    std::stoi(result.secondary_parameter);
                }
            } catch (const std::exception&) {
                return false;
            }
        }
    }
    
    return true;
}

bool EnhancedSelectorManager::isValidSelector(const std::string& selector) const {
    auto result = const_cast<EnhancedSelectorManager*>(this)->parseSelector(selector);
    return result.success && validateSelectorSyntax(result);
}

std::vector<std::string> EnhancedSelectorManager::getSelectorSuggestions(const std::string& partial_selector) const {
    std::vector<std::string> suggestions;
    
    // 基于现有元素生成建议
    for (const auto& [id, element] : dom_elements) {
        // 标签名建议
        if (element.tag_name.find(partial_selector) == 0) {
            suggestions.push_back(element.tag_name);
        }
        
        // ID建议
        if (!element.id.empty() && element.id.find(partial_selector) == 0) {
            suggestions.push_back("#" + element.id);
        }
        
        // 类名建议
        auto class_attr = element.attributes.find("class");
        if (class_attr != element.attributes.end()) {
            std::stringstream ss(class_attr->second);
            std::string class_name;
            while (ss >> class_name) {
                if (class_name.find(partial_selector) == 0) {
                    suggestions.push_back("." + class_name);
                }
            }
        }
    }
    
    // 去重
    std::sort(suggestions.begin(), suggestions.end());
    suggestions.erase(std::unique(suggestions.begin(), suggestions.end()), suggestions.end());
    
    return suggestions;
}

// 错误处理
std::vector<std::string> EnhancedSelectorManager::getLastErrors() const {
    return errors;
}

std::vector<std::string> EnhancedSelectorManager::getLastWarnings() const {
    return warnings;
}

void EnhancedSelectorManager::clearErrors() {
    errors.clear();
}

void EnhancedSelectorManager::clearWarnings() {
    warnings.clear();
}

// 私有辅助方法
void EnhancedSelectorManager::addError(const std::string& message) {
    errors.push_back(message);
}

void EnhancedSelectorManager::addWarning(const std::string& message) {
    warnings.push_back(message);
}

std::string EnhancedSelectorManager::escapeJavaScriptString(const std::string& str) const {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '\'': escaped += "\\'"; break;
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

std::string EnhancedSelectorManager::joinClasses(const DOMElementInfo& element) const {
    auto class_attr = element.attributes.find("class");
    if (class_attr != element.attributes.end()) {
        return class_attr->second;
    }
    return "";
}

// ===== SelectorUtils Implementation =====

SelectorType SelectorUtils::parseType(const std::string& selector) {
    if (selector.empty()) return SelectorType::STANDARD;
    
    if (selector.substr(0, 2) == "{{" && selector.length() > 4 && selector.substr(selector.length() - 2) == "}}") {
        return SelectorType::ENHANCED;
    }
    
    if (selector[0] == '.') return SelectorType::CLASS;
    if (selector[0] == '#') return SelectorType::ID;
    if (selector.find('[') != std::string::npos) return SelectorType::ATTRIBUTE;
    if (selector.find(' ') != std::string::npos || 
        selector.find('>') != std::string::npos || 
        selector.find('+') != std::string::npos || 
        selector.find('~') != std::string::npos) {
        return SelectorType::COMPLEX;
    }
    
    return SelectorType::ELEMENT;
}

bool SelectorUtils::isEnhancedSelector(const std::string& selector) {
    return selector.length() > 4 && 
           selector.substr(0, 2) == "{{" && 
           selector.substr(selector.length() - 2) == "}}";
}

bool SelectorUtils::hasPreciseAccess(const std::string& selector) {
    return selector.find('[') != std::string::npos && selector.find(']') != std::string::npos;
}

std::string SelectorUtils::extractBaseSelector(const std::string& enhanced_selector) {
    if (!isEnhancedSelector(enhanced_selector)) {
        return enhanced_selector;
    }
    
    std::string inner = enhanced_selector.substr(2, enhanced_selector.length() - 4);
    
    // 移除精确访问部分
    size_t bracket_pos = inner.find('[');
    if (bracket_pos != std::string::npos) {
        return inner.substr(0, bracket_pos);
    }
    
    return inner;
}

std::string SelectorUtils::normalizSelector(const std::string& selector) {
    std::string normalized = selector;
    
    // 移除多余空格
    std::regex extra_spaces(R"(\s+)");
    normalized = std::regex_replace(normalized, extra_spaces, " ");
    
    // 修正操作符周围的空格
    std::regex operators(R"(\s*([>+~])\s*)");
    normalized = std::regex_replace(normalized, operators, " $1 ");
    
    // 去除首尾空格
    normalized.erase(0, normalized.find_first_not_of(" \t\n\r"));
    normalized.erase(normalized.find_last_not_of(" \t\n\r") + 1);
    
    return normalized;
}

bool SelectorUtils::isValidSelectorChar(char c) {
    return std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '#' || 
           c == '[' || c == ']' || c == '=' || c == '"' || c == '\'' ||
           c == '{' || c == '}' || c == ' ' || c == '>' || c == '+' || c == '~';
}

std::vector<std::string> SelectorUtils::splitComplexSelector(const std::string& selector) {
    std::vector<std::string> parts;
    std::string current;
    
    for (char c : selector) {
        if (c == ',' || c == ' ' || c == '>' || c == '+' || c == '~') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
            if (c != ' ') {
                parts.push_back(std::string(1, c));
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    return parts;
}

// ===== 预定义选择器模式 =====

namespace SelectorPatterns {
    const std::regex ENHANCED_SELECTOR(R"(\{\{([^}]+)\}\})");
    const std::regex PRECISE_ACCESS(R"(\{\{([^}]+)\[([^]]+)\]\}\})");
    const std::regex INDEX_ACCESS(R"(\{\{([^}]+)\[(\d+)\]\}\})");
    const std::regex RANGE_ACCESS(R"(\{\{([^}]+)\[(\d+):(\d+)\]\}\})");
    const std::regex ATTRIBUTE_SELECTOR(R"(\[([^=]+)(?:=([^]]+))?\])");
    const std::regex CLASS_SELECTOR(R"(\.([A-Za-z_][A-Za-z0-9_-]*))");
    const std::regex ID_SELECTOR(R"(#([A-Za-z_][A-Za-z0-9_-]*))");
    const std::regex PSEUDO_CLASS(R"(:([A-Za-z-]+)(?:\(([^)]+)\))?)");
    const std::regex PSEUDO_ELEMENT(R"(::([A-Za-z-]+))");
}

} // namespace CHTL