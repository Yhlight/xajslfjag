#include "GlobalMap.h"
#include <algorithm>

namespace CHTL {

GlobalMap::GlobalMap() 
    : m_initialized(false) {
    initializeStaticData();
}

GlobalMap& GlobalMap::getInstance() {
    static GlobalMap instance;
    return instance;
}

void GlobalMap::initializeStaticData() {
    if (m_initialized) {
        return;
    }
    
    // 初始化CHTL关键字
    m_keywords = {
        "text", "style", "html5", "use", "inherit", "delete", "insert",
        "after", "before", "replace", "at", "top", "bottom", "from", "as", "except",
        // CHTL JS 关键字
        "script", "module", "load", "listen", "delegate", "animate", "vir",
        "click", "mouseenter", "mouseleave", "mousemove", "target", "duration",
        "easing", "begin", "when", "end", "loop", "direction", "delay", "callback"
    };
    
    // 初始化HTML元素
    m_htmlElements = {
        // 文档结构
        "html", "head", "body", "title", "meta", "link", "script", "style", "base",
        
        // 文本内容
        "h1", "h2", "h3", "h4", "h5", "h6", "p", "br", "hr", "pre", "blockquote",
        
        // 行内文本
        "a", "abbr", "b", "bdi", "bdo", "cite", "code", "data", "dfn", "em", "i",
        "kbd", "mark", "q", "rp", "rt", "ruby", "s", "samp", "small", "span", "strong",
        "sub", "sup", "time", "u", "var", "wbr",
        
        // 图像和媒体
        "img", "area", "map", "audio", "video", "source", "track",
        
        // 内嵌内容
        "iframe", "embed", "object", "param", "picture",
        
        // 脚本
        "canvas", "noscript",
        
        // 编辑
        "del", "ins",
        
        // 表格
        "table", "caption", "colgroup", "col", "tbody", "thead", "tfoot", "tr", "td", "th",
        
        // 表单
        "form", "fieldset", "legend", "label", "input", "button", "select", "datalist",
        "optgroup", "option", "textarea", "output", "progress", "meter",
        
        // 交互元素
        "details", "summary", "dialog",
        
        // 节段
        "main", "section", "nav", "article", "aside", "header", "footer", "address",
        
        // 分组内容
        "div", "figure", "figcaption", "ol", "ul", "li", "dl", "dt", "dd",
        
        // Web组件
        "slot", "template"
    };
    
    // 初始化CSS属性
    m_cssProperties = {
        // 布局
        "display", "position", "top", "right", "bottom", "left", "z-index",
        "float", "clear", "overflow", "overflow-x", "overflow-y", "visibility",
        
        // 盒模型
        "width", "height", "min-width", "min-height", "max-width", "max-height",
        "margin", "margin-top", "margin-right", "margin-bottom", "margin-left",
        "padding", "padding-top", "padding-right", "padding-bottom", "padding-left",
        "border", "border-width", "border-style", "border-color",
        "border-top", "border-right", "border-bottom", "border-left",
        "border-radius", "box-sizing",
        
        // 文本
        "color", "font", "font-family", "font-size", "font-weight", "font-style",
        "font-variant", "line-height", "text-align", "text-decoration", "text-indent",
        "text-transform", "text-shadow", "letter-spacing", "word-spacing", "white-space",
        
        // 背景
        "background", "background-color", "background-image", "background-repeat",
        "background-position", "background-size", "background-attachment",
        
        // 变换和动画
        "transform", "transform-origin", "transition", "animation",
        
        // Flexbox
        "flex", "flex-direction", "flex-wrap", "flex-flow", "justify-content",
        "align-items", "align-content", "align-self", "flex-grow", "flex-shrink", "flex-basis",
        
        // Grid
        "grid", "grid-template", "grid-template-columns", "grid-template-rows",
        "grid-column", "grid-row", "grid-area", "gap", "column-gap", "row-gap",
        
        // 其他
        "opacity", "cursor", "outline", "resize", "user-select", "pointer-events"
    };
    

    
    m_initialized = true;
}

// ========== 关键字映射实现 ==========

bool GlobalMap::isKeyword(const std::string& word) const {
    return m_keywords.find(word) != m_keywords.end();
}

bool GlobalMap::isHtmlElement(const std::string& element) const {
    return m_htmlElements.find(element) != m_htmlElements.end();
}

bool GlobalMap::isCssProperty(const std::string& property) const {
    return m_cssProperties.find(property) != m_cssProperties.end();
}



} // namespace CHTL