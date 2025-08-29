#include "Type.h"
#include <sstream>

namespace CHTL {

// TypeInfo 方法实现
bool TypeInfo::isTemplateType() const {
    return nodeType == NodeType::TEMPLATE_STYLE ||
           nodeType == NodeType::TEMPLATE_ELEMENT ||
           nodeType == NodeType::TEMPLATE_VAR;
}

bool TypeInfo::isCustomType() const {
    return nodeType == NodeType::CUSTOM_STYLE ||
           nodeType == NodeType::CUSTOM_ELEMENT ||
           nodeType == NodeType::CUSTOM_VAR;
}

bool TypeInfo::isOriginType() const {
    return nodeType == NodeType::ORIGIN_HTML ||
           nodeType == NodeType::ORIGIN_STYLE ||
           nodeType == NodeType::ORIGIN_JAVASCRIPT ||
           nodeType == NodeType::ORIGIN_CUSTOM;
}

bool TypeInfo::isImportType() const {
    return nodeType == NodeType::IMPORT_HTML ||
           nodeType == NodeType::IMPORT_STYLE ||
           nodeType == NodeType::IMPORT_JAVASCRIPT ||
           nodeType == NodeType::IMPORT_CHTL ||
           nodeType == NodeType::IMPORT_CJMOD ||
           nodeType == NodeType::IMPORT_CONFIG;
}

bool TypeInfo::isStyleType() const {
    return nodeType == NodeType::STYLE_BLOCK ||
           nodeType == NodeType::INLINE_STYLE ||
           nodeType == NodeType::CLASS_SELECTOR_STYLE ||
           nodeType == NodeType::ID_SELECTOR_STYLE ||
           nodeType == NodeType::PSEUDO_SELECTOR_STYLE;
}

bool TypeInfo::isStructuralType() const {
    return nodeType == NodeType::NAMESPACE ||
           nodeType == NodeType::CONFIGURATION ||
           nodeType == NodeType::INFO ||
           nodeType == NodeType::EXPORT ||
           nodeType == NodeType::ROOT;
}

std::string TypeInfo::toString() const {
    std::stringstream ss;
    ss << "TypeInfo{node:" << static_cast<int>(nodeType) 
       << ", data:" << static_cast<int>(dataType)
       << ", scope:" << static_cast<int>(scopeType)
       << ", name:'" << name << "'}";
    return ss.str();
}

// HtmlElementTypes 实现
const std::unordered_set<std::string>& HtmlElementTypes::getValidElements() {
    static const std::unordered_set<std::string> validElements = {
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
    return validElements;
}

bool HtmlElementTypes::isValidElement(const std::string& elementName) {
    const auto& elements = getValidElements();
    return elements.find(elementName) != elements.end();
}

bool HtmlElementTypes::isSelfClosingElement(const std::string& elementName) {
    static const std::unordered_set<std::string> selfClosing = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    return selfClosing.find(elementName) != selfClosing.end();
}

bool HtmlElementTypes::isBlockElement(const std::string& elementName) {
    static const std::unordered_set<std::string> blockElements = {
        "div", "p", "h1", "h2", "h3", "h4", "h5", "h6", "header", "footer", "main",
        "section", "article", "aside", "nav", "blockquote", "pre", "ol", "ul", "li",
        "dl", "dt", "dd", "figure", "figcaption", "table", "form", "fieldset", "address"
    };
    return blockElements.find(elementName) != blockElements.end();
}

bool HtmlElementTypes::isInlineElement(const std::string& elementName) {
    static const std::unordered_set<std::string> inlineElements = {
        "a", "abbr", "b", "bdi", "bdo", "br", "button", "cite", "code", "data", "dfn",
        "em", "i", "input", "kbd", "label", "mark", "q", "s", "samp", "small", "span",
        "strong", "sub", "sup", "time", "u", "var", "wbr", "img"
    };
    return inlineElements.find(elementName) != inlineElements.end();
}

// CssPropertyTypes 实现
const std::unordered_set<std::string>& CssPropertyTypes::getValidProperties() {
    static const std::unordered_set<std::string> validProperties = {
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
    return validProperties;
}

bool CssPropertyTypes::isValidProperty(const std::string& propertyName) {
    const auto& properties = getValidProperties();
    return properties.find(propertyName) != properties.end();
}

DataType CssPropertyTypes::getPropertyValueType(const std::string& propertyName) {
    // 简化实现，大部分CSS属性值都是字符串类型
    return DataType::CSS_VALUE;
}

// ChtlKeywordTypes 实现
const std::unordered_set<std::string>& ChtlKeywordTypes::getKeywords() {
    static const std::unordered_set<std::string> keywords = {
        "text", "style", "html5", "use", "inherit", "delete", "insert",
        "after", "before", "replace", "at", "top", "bottom", "from", "as", "except",
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]",
        "[Configuration]", "[Info]", "[Export]", "[Name]", "[OriginType]"
    };
    return keywords;
}

bool ChtlKeywordTypes::isKeyword(const std::string& word) {
    const auto& keywords = getKeywords();
    return keywords.find(word) != keywords.end();
}

NodeType ChtlKeywordTypes::getKeywordNodeType(const std::string& keyword) {
    static const std::unordered_map<std::string, NodeType> keywordMap = {
        {"text", NodeType::TEXT_NODE},
        {"style", NodeType::STYLE_BLOCK},
        {"inherit", NodeType::INHERIT_OP},
        {"delete", NodeType::DELETE_OP},
        {"insert", NodeType::INSERT_OP},
        {"except", NodeType::EXCEPT_CONSTRAINT},
        {"[Template]", NodeType::TEMPLATE_STYLE},
        {"[Custom]", NodeType::CUSTOM_STYLE},
        {"[Origin]", NodeType::ORIGIN_HTML},
        {"[Import]", NodeType::IMPORT_HTML},
        {"[Namespace]", NodeType::NAMESPACE},
        {"[Configuration]", NodeType::CONFIGURATION},
        {"[Info]", NodeType::INFO},
        {"[Export]", NodeType::EXPORT}
    };
    
    auto it = keywordMap.find(keyword);
    return (it != keywordMap.end()) ? it->second : NodeType::UNKNOWN_NODE;
}

// TypeFactory 实现
TypeInfo TypeFactory::createTypeInfo(const std::string& typeName) {
    if (HtmlElementTypes::isValidElement(typeName)) {
        return TypeInfo(NodeType::ELEMENT_NODE, DataType::HTML_TAG, ScopeType::ELEMENT_SCOPE, typeName);
    }
    
    if (ChtlKeywordTypes::isKeyword(typeName)) {
        NodeType nodeType = ChtlKeywordTypes::getKeywordNodeType(typeName);
        return TypeInfo(nodeType, DataType::IDENTIFIER, ScopeType::LOCAL_SCOPE, typeName);
    }
    
    // 检查是否为选择器
    if (!typeName.empty() && typeName[0] == '.') {
        return TypeInfo(NodeType::CLASS_SELECTOR_STYLE, DataType::SELECTOR, ScopeType::STYLE_SCOPE, typeName);
    }
    
    if (!typeName.empty() && typeName[0] == '#') {
        return TypeInfo(NodeType::ID_SELECTOR_STYLE, DataType::SELECTOR, ScopeType::STYLE_SCOPE, typeName);
    }
    
    // 默认为标识符
    return TypeInfo(NodeType::UNKNOWN_NODE, DataType::IDENTIFIER, ScopeType::LOCAL_SCOPE, typeName);
}

TypeInfo TypeFactory::createFromToken(const std::string& tokenValue, const std::string& context) {
    return createTypeInfo(tokenValue);
}

bool TypeFactory::isCompatible(const TypeInfo& source, const TypeInfo& target) {
    // 简化的兼容性检查
    return source.dataType == target.dataType || 
           source.dataType == DataType::UNKNOWN_TYPE ||
           target.dataType == DataType::UNKNOWN_TYPE;
}

TypeInfo TypeFactory::getDefaultType() {
    return TypeInfo();
}

} // namespace CHTL