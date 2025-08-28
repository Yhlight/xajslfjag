#include "CHTLGlobalMap.h"

namespace CHTL {

CHTLGlobalMap& CHTLGlobalMap::Instance() {
    static CHTLGlobalMap instance;
    return instance;
}

void CHTLGlobalMap::Initialize() {
    InitializeHTMLElements();
    InitializeKeywords();
    InitializeDefaultConfiguration();
}

void CHTLGlobalMap::InitializeHTMLElements() {
    // 所有标准HTML元素
    htmlElements_ = {
        // 根元素
        "html",
        
        // 文档元数据
        "head", "title", "base", "link", "meta", "style",
        
        // 分区根元素
        "body",
        
        // 内容分区
        "address", "article", "aside", "footer", "header", "h1", "h2", "h3", "h4", "h5", "h6",
        "hgroup", "main", "nav", "section",
        
        // 文本内容
        "blockquote", "dd", "div", "dl", "dt", "figcaption", "figure", "hr", "li", "ol",
        "p", "pre", "ul",
        
        // 内联文本语义
        "a", "abbr", "b", "bdi", "bdo", "br", "cite", "code", "data", "dfn", "em",
        "i", "kbd", "mark", "q", "rp", "rt", "ruby", "s", "samp", "small", "span",
        "strong", "sub", "sup", "time", "u", "var", "wbr",
        
        // 图片和多媒体
        "area", "audio", "img", "map", "track", "video",
        
        // 嵌入内容
        "embed", "iframe", "object", "param", "picture", "portal", "source",
        
        // SVG和MathML
        "svg", "math",
        
        // 脚本
        "canvas", "noscript", "script",
        
        // 编辑标识
        "del", "ins",
        
        // 表格内容
        "caption", "col", "colgroup", "table", "tbody", "td", "tfoot", "th", "thead", "tr",
        
        // 表单
        "button", "datalist", "fieldset", "form", "input", "label", "legend", "meter",
        "optgroup", "option", "output", "progress", "select", "textarea",
        
        // 交互元素
        "details", "dialog", "menu", "summary",
        
        // Web组件
        "slot", "template"
    };
}

void CHTLGlobalMap::InitializeKeywords() {
    keywords_ = {
        // 基础关键字
        {"text", CHTLTokenType::TEXT},
        {"style", CHTLTokenType::STYLE},
        {"script", CHTLTokenType::SCRIPT},
        
        // 声明关键字
        {"[Template]", CHTLTokenType::TEMPLATE},
        {"[Custom]", CHTLTokenType::CUSTOM},
        {"[Origin]", CHTLTokenType::ORIGIN},
        {"[Configuration]", CHTLTokenType::CONFIGURATION},
        {"[Import]", CHTLTokenType::IMPORT},
        {"[Namespace]", CHTLTokenType::NAMESPACE},
        
        // 类型关键字
        {"@Style", CHTLTokenType::STYLE_TEMPLATE},
        {"@Element", CHTLTokenType::ELEMENT_TEMPLATE},
        {"@Var", CHTLTokenType::VAR_TEMPLATE},
        {"@Html", CHTLTokenType::ORIGIN_HTML},
        {"@JavaScript", CHTLTokenType::ORIGIN_JAVASCRIPT},
        {"@Chtl", CHTLTokenType::IDENTIFIER}, // 特殊处理
        {"@CJmod", CHTLTokenType::IDENTIFIER}, // 特殊处理
        {"@Config", CHTLTokenType::IDENTIFIER}, // 特殊处理
        
        // 配置节
        {"[Name]", CHTLTokenType::NAME_SECTION},
        {"[OriginType]", CHTLTokenType::ORIGIN_TYPE_SECTION},
        
        // 操作关键字
        {"inherit", CHTLTokenType::INHERIT},
        {"delete", CHTLTokenType::DELETE},
        {"insert", CHTLTokenType::INSERT},
        {"after", CHTLTokenType::AFTER},
        {"before", CHTLTokenType::BEFORE},
        {"replace", CHTLTokenType::REPLACE},
        {"at top", CHTLTokenType::AT_TOP},
        {"at bottom", CHTLTokenType::AT_BOTTOM},
        
        // 导入关键字
        {"from", CHTLTokenType::FROM},
        {"as", CHTLTokenType::AS},
        {"except", CHTLTokenType::EXCEPT}
    };
    
    templateTypes_ = {"@Style", "@Element", "@Var"};
    customTypes_ = {"@Style", "@Element", "@Var"};
    originTypes_ = {"@Html", "@Style", "@JavaScript"};
}

void CHTLGlobalMap::InitializeDefaultConfiguration() {
    configuration_ = {
        {"INDEX_INITIAL_COUNT", "0"},
        {"DISABLE_NAME_GROUP", "true"},
        {"DISABLE_CUSTOM_ORIGIN_TYPE", "false"},
        {"DEBUG_MODE", "false"},
        {"DISABLE_DEFAULT_NAMESPACE", "false"},
        {"DISABLE_STYLE_AUTO_ADD_CLASS", "false"},
        {"DISABLE_STYLE_AUTO_ADD_ID", "false"},
        {"DISABLE_SCRIPT_AUTO_ADD_CLASS", "true"},
        {"DISABLE_SCRIPT_AUTO_ADD_ID", "true"}
    };
}

bool CHTLGlobalMap::IsHTMLElement(const std::string& name) const {
    return htmlElements_.find(name) != htmlElements_.end();
}

bool CHTLGlobalMap::IsKeyword(const std::string& keyword) const {
    return keywords_.find(keyword) != keywords_.end();
}

CHTLTokenType CHTLGlobalMap::GetKeywordType(const std::string& keyword) const {
    auto it = keywords_.find(keyword);
    return it != keywords_.end() ? it->second : CHTLTokenType::IDENTIFIER;
}

bool CHTLGlobalMap::IsTemplateType(const std::string& type) const {
    return templateTypes_.find(type) != templateTypes_.end();
}

bool CHTLGlobalMap::IsCustomType(const std::string& type) const {
    return customTypes_.find(type) != customTypes_.end();
}

bool CHTLGlobalMap::IsOriginType(const std::string& type) const {
    return originTypes_.find(type) != originTypes_.end() || 
           IsRegisteredOriginType(type);
}

void CHTLGlobalMap::UpdateConfiguration(const std::unordered_map<std::string, std::string>& config) {
    for (const auto& pair : config) {
        configuration_[pair.first] = pair.second;
    }
}

std::string CHTLGlobalMap::GetConfigValue(const std::string& key) const {
    auto it = configuration_.find(key);
    return it != configuration_.end() ? it->second : "";
}

void CHTLGlobalMap::RegisterOriginType(const std::string& typeName) {
    registeredOriginTypes_.insert(typeName);
}

bool CHTLGlobalMap::IsRegisteredOriginType(const std::string& typeName) const {
    return registeredOriginTypes_.find(typeName) != registeredOriginTypes_.end();
}

void CHTLGlobalMap::RegisterNamespace(const std::string& namespaceName) {
    namespaces_.insert(namespaceName);
}

bool CHTLGlobalMap::IsNamespaceRegistered(const std::string& namespaceName) const {
    return namespaces_.find(namespaceName) != namespaces_.end();
}

void CHTLGlobalMap::EnterNamespace(const std::string& namespaceName) {
    if (!currentNamespace_.empty()) {
        currentNamespace_ += ".";
    }
    currentNamespace_ += namespaceName;
}

void CHTLGlobalMap::ExitNamespace() {
    size_t lastDot = currentNamespace_.find_last_of('.');
    if (lastDot != std::string::npos) {
        currentNamespace_ = currentNamespace_.substr(0, lastDot);
    } else {
        currentNamespace_.clear();
    }
}

std::string CHTLGlobalMap::GetCurrentNamespace() const {
    return currentNamespace_;
}

void CHTLGlobalMap::RegisterTemplate(const std::string& type, const std::string& name) {
    registeredTemplates_[type].insert(name);
}

void CHTLGlobalMap::RegisterCustom(const std::string& type, const std::string& name) {
    registeredCustoms_[type].insert(name);
}

bool CHTLGlobalMap::IsTemplateRegistered(const std::string& type, const std::string& name) const {
    auto it = registeredTemplates_.find(type);
    return it != registeredTemplates_.end() && it->second.find(name) != it->second.end();
}

bool CHTLGlobalMap::IsCustomRegistered(const std::string& type, const std::string& name) const {
    auto it = registeredCustoms_.find(type);
    return it != registeredCustoms_.end() && it->second.find(name) != it->second.end();
}

void CHTLGlobalMap::Clear() {
    namespaces_.clear();
    currentNamespace_.clear();
    registeredOriginTypes_.clear();
    registeredTemplates_.clear();
    registeredCustoms_.clear();
}

} // namespace CHTL