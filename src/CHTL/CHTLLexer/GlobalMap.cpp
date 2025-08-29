#include "GlobalMap.h"
#include <algorithm>

namespace CHTL {

GlobalMap::GlobalMap() 
    : m_debugMode(false)
    , m_indexInitialCount(0)
    , m_initialized(false) {
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
        "after", "before", "replace", "at", "top", "bottom", "from", "as", "except"
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
    
    // 设置默认配置
    m_boolConfigs["DISABLE_NAME_GROUP"] = true;
    m_boolConfigs["DISABLE_CUSTOM_ORIGIN_TYPE"] = false;
    m_boolConfigs["DEBUG_MODE"] = false;
    m_boolConfigs["DISABLE_STYLE_AUTO_ADD_CLASS"] = false;
    m_boolConfigs["DISABLE_STYLE_AUTO_ADD_ID"] = false;
    m_boolConfigs["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = true;
    m_boolConfigs["DISABLE_SCRIPT_AUTO_ADD_ID"] = true;
    m_boolConfigs["DISABLE_DEFAULT_NAMESPACE"] = false;
    
    m_intConfigs["INDEX_INITIAL_COUNT"] = 0;
    m_intConfigs["OPTION_COUNT"] = 3;
    
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

// ========== 配置信息实现 ==========

bool GlobalMap::getConfigBool(const std::string& key, bool defaultValue) const {
    auto it = m_boolConfigs.find(key);
    return (it != m_boolConfigs.end()) ? it->second : defaultValue;
}

void GlobalMap::setConfigBool(const std::string& key, bool value) {
    m_boolConfigs[key] = value;
}

std::string GlobalMap::getConfigString(const std::string& key, const std::string& defaultValue) const {
    auto it = m_stringConfigs.find(key);
    return (it != m_stringConfigs.end()) ? it->second : defaultValue;
}

void GlobalMap::setConfigString(const std::string& key, const std::string& value) {
    m_stringConfigs[key] = value;
}

int GlobalMap::getConfigInt(const std::string& key, int defaultValue) const {
    auto it = m_intConfigs.find(key);
    return (it != m_intConfigs.end()) ? it->second : defaultValue;
}

void GlobalMap::setConfigInt(const std::string& key, int value) {
    m_intConfigs[key] = value;
}

// ========== 路径信息实现 ==========

void GlobalMap::setCurrentFilePath(const std::string& path) {
    m_currentFilePath = path;
}

std::string GlobalMap::getCurrentFilePath() const {
    return m_currentFilePath;
}

void GlobalMap::setOfficialModulePath(const std::string& path) {
    m_officialModulePath = path;
}

std::string GlobalMap::getOfficialModulePath() const {
    return m_officialModulePath;
}

void GlobalMap::setWorkingDirectory(const std::string& path) {
    m_workingDirectory = path;
}

std::string GlobalMap::getWorkingDirectory() const {
    return m_workingDirectory;
}

// ========== 状态信息实现 ==========

bool GlobalMap::isDebugMode() const {
    return m_debugMode;
}

void GlobalMap::setDebugMode(bool enabled) {
    m_debugMode = enabled;
    m_boolConfigs["DEBUG_MODE"] = enabled;
}

int GlobalMap::getIndexInitialCount() const {
    return m_indexInitialCount;
}

void GlobalMap::setIndexInitialCount(int count) {
    m_indexInitialCount = count;
    m_intConfigs["INDEX_INITIAL_COUNT"] = count;
}

// ========== 静态表信息实现 ==========

void GlobalMap::registerCustomOriginType(const std::string& typeName) {
    m_customOriginTypes.insert(typeName);
}

bool GlobalMap::isCustomOriginType(const std::string& typeName) const {
    return m_customOriginTypes.find(typeName) != m_customOriginTypes.end();
}

const std::unordered_set<std::string>& GlobalMap::getCustomOriginTypes() const {
    return m_customOriginTypes;
}

// ========== 清理和重置实现 ==========

void GlobalMap::clearTempData() {
    // 清理临时数据，保留配置和静态数据
    m_currentFilePath.clear();
}

void GlobalMap::reset() {
    // 重置所有数据
    m_boolConfigs.clear();
    m_stringConfigs.clear();
    m_intConfigs.clear();
    m_currentFilePath.clear();
    m_officialModulePath.clear();
    m_workingDirectory.clear();
    m_customOriginTypes.clear();
    m_debugMode = false;
    m_indexInitialCount = 0;
    m_initialized = false;
    
    // 重新初始化
    initializeStaticData();
}

} // namespace CHTL