#include "Type.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

    // CHTLType实现
    CHTLType::CHTLType(BaseType type, const std::string& name) 
        : baseType(type) {
        attributes.name = name;
    }

    void CHTLType::addChild(std::shared_ptr<CHTLType> child) {
        if (child && canHaveChild(*child)) {
            children.push_back(child);
            child->setParent(shared_from_this());
        }
    }

    void CHTLType::removeChild(std::shared_ptr<CHTLType> child) {
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            children.erase(it);
            child->setParent(nullptr);
        }
    }

    void CHTLType::setProperty(const std::string& key, const std::string& value) {
        attributes.properties[key] = value;
    }

    std::string CHTLType::getProperty(const std::string& key) const {
        auto it = attributes.properties.find(key);
        return (it != attributes.properties.end()) ? it->second : "";
    }

    bool CHTLType::hasProperty(const std::string& key) const {
        return attributes.properties.find(key) != attributes.properties.end();
    }

    bool CHTLType::isCompatibleWith(const CHTLType& other) const {
        // 基础兼容性检查
        return baseType == other.baseType;
    }

    bool CHTLType::canHaveChild(const CHTLType& child) const {
        // 默认实现：检查是否在允许的子类型列表中
        if (attributes.allowedChildren.empty()) {
            return true; // 如果没有限制，则允许所有子类型
        }
        
        std::string childTypeName = child.getName();
        return std::find(attributes.allowedChildren.begin(), 
                        attributes.allowedChildren.end(), 
                        childTypeName) != attributes.allowedChildren.end();
    }

    bool CHTLType::isValid() const {
        // 检查必需属性
        for (const auto& requiredProp : attributes.requiredProperties) {
            if (!hasProperty(requiredProp)) {
                return false;
            }
        }
        return true;
    }

    std::string CHTLType::toString() const {
        std::stringstream ss;
        ss << "CHTLType{" << attributes.name << "}";
        return ss.str();
    }

    std::string CHTLType::toHTML() const {
        return ""; // 基础实现返回空字符串
    }

    // TextType实现
    TextType::TextType(const std::string& content) 
        : CHTLType(BaseType::TEXT, "text") {
        setProperty("content", content);
    }

    std::string TextType::toHTML() const {
        return getProperty("content");
    }

    // HTMLElementType实现
    HTMLElementType::HTMLElementType(const std::string& tag) 
        : CHTLType(BaseType::HTML_ELEMENT, tag), tagName(tag) {
        // 设置允许的子类型
        attributes.allowedChildren = {"text", "html_element", "style_block"};
    }

    void HTMLElementType::setAttribute(const std::string& name, const std::string& value) {
        htmlAttributes[name] = value;
        setProperty("attr_" + name, value);
    }

    std::string HTMLElementType::getAttribute(const std::string& name) const {
        auto it = htmlAttributes.find(name);
        return (it != htmlAttributes.end()) ? it->second : "";
    }

    bool HTMLElementType::canHaveChild(const CHTLType& child) const {
        // HTML元素可以包含文本、其他HTML元素和样式块
        BaseType childType = child.getBaseType();
        return childType == BaseType::TEXT || 
               childType == BaseType::HTML_ELEMENT || 
               childType == BaseType::STYLE_BLOCK;
    }

    std::string HTMLElementType::toHTML() const {
        std::stringstream ss;
        ss << "<" << tagName;
        
        // 添加属性
        for (const auto& attr : htmlAttributes) {
            ss << " " << attr.first << "=\"" << attr.second << "\"";
        }
        
        if (children.empty()) {
            ss << " />";
        } else {
            ss << ">";
            for (const auto& child : children) {
                ss << child->toHTML();
            }
            ss << "</" << tagName << ">";
        }
        
        return ss.str();
    }

    // StyleBlockType实现
    StyleBlockType::StyleBlockType(const std::string& selector) 
        : CHTLType(BaseType::STYLE_BLOCK, "style") {
        if (!selector.empty()) {
            if (selector.front() == '.') {
                selectorType = "class";
                selectorValue = selector.substr(1);
            } else if (selector.front() == '#') {
                selectorType = "id";
                selectorValue = selector.substr(1);
            } else {
                selectorType = "element";
                selectorValue = selector;
            }
        } else {
            selectorType = "inline";
        }
    }

    void StyleBlockType::addCSSProperty(const std::string& property, const std::string& value) {
        cssProperties[property] = value;
        setProperty("css_" + property, value);
    }

    void StyleBlockType::setSelectorType(const std::string& type, const std::string& value) {
        selectorType = type;
        selectorValue = value;
    }

    std::string StyleBlockType::toHTML() const {
        if (selectorType == "inline") {
            // 内联样式
            std::stringstream ss;
            for (const auto& prop : cssProperties) {
                if (ss.tellp() > 0) ss << "; ";
                ss << prop.first << ": " << prop.second;
            }
            return ss.str();
        }
        return toCSS();
    }

    std::string StyleBlockType::toCSS() const {
        std::stringstream ss;
        
        // 生成选择器
        if (selectorType == "class") {
            ss << "." << selectorValue;
        } else if (selectorType == "id") {
            ss << "#" << selectorValue;
        } else if (selectorType == "element") {
            ss << selectorValue;
        }
        
        ss << " {\n";
        for (const auto& prop : cssProperties) {
            ss << "    " << prop.first << ": " << prop.second << ";\n";
        }
        ss << "}\n";
        
        return ss.str();
    }

    // TemplateType实现
    TemplateType::TemplateType(const std::string& type, const std::string& name) 
        : CHTLType(BaseType::TEMPLATE_STYLE, name), templateType(type) {
        attributes.isTemplate = true;
        
        // 根据模板类型设置基础类型
        if (type == "Style") {
            baseType = BaseType::TEMPLATE_STYLE;
        } else if (type == "Element") {
            baseType = BaseType::TEMPLATE_ELEMENT;
        } else if (type == "Var") {
            baseType = BaseType::TEMPLATE_VAR;
        }
    }

    void TemplateType::setTemplateData(const std::string& key, const std::string& value) {
        templateData[key] = value;
        setProperty("template_" + key, value);
    }

    std::string TemplateType::getTemplateData(const std::string& key) const {
        auto it = templateData.find(key);
        return (it != templateData.end()) ? it->second : "";
    }

    bool TemplateType::inheritFrom(const TemplateType& baseTemplate) {
        if (templateType != baseTemplate.templateType) {
            return false; // 只能继承同类型模板
        }
        
        // 继承模板数据
        for (const auto& data : baseTemplate.templateData) {
            if (templateData.find(data.first) == templateData.end()) {
                templateData[data.first] = data.second;
            }
        }
        
        return true;
    }

    std::string TemplateType::toString() const {
        std::stringstream ss;
        ss << "[Template] @" << templateType << " " << attributes.name;
        return ss.str();
    }

    // CustomType实现
    CustomType::CustomType(const std::string& type, const std::string& name) 
        : TemplateType(type, name) {
        attributes.isCustom = true;
        
        // 根据自定义类型设置基础类型
        if (type == "Style") {
            baseType = BaseType::CUSTOM_STYLE;
        } else if (type == "Element") {
            baseType = BaseType::CUSTOM_ELEMENT;
        } else if (type == "Var") {
            baseType = BaseType::CUSTOM_VAR;
        }
    }

    void CustomType::deleteProperty(const std::string& property) {
        deletedProperties.push_back(property);
        // 从模板数据中移除
        templateData.erase(property);
    }

    void CustomType::overrideProperty(const std::string& property, const std::string& value) {
        overriddenProperties[property] = value;
        templateData[property] = value;
    }

    bool CustomType::inheritFrom(const TemplateType& baseTemplate) {
        if (!TemplateType::inheritFrom(baseTemplate)) {
            return false;
        }
        
        // 应用删除和重写操作
        for (const auto& deletedProp : deletedProperties) {
            templateData.erase(deletedProp);
        }
        
        for (const auto& overriddenProp : overriddenProperties) {
            templateData[overriddenProp.first] = overriddenProp.second;
        }
        
        return true;
    }

    std::string CustomType::toString() const {
        std::stringstream ss;
        ss << "[Custom] @" << templateType << " " << attributes.name;
        return ss.str();
    }

    // OriginType实现
    OriginType::OriginType(const std::string& language, const std::string& content) 
        : CHTLType(BaseType::ORIGIN_HTML, "origin"), originLanguage(language), rawContent(content) {
        
        // 根据语言类型设置基础类型
        if (language == "Html") {
            baseType = BaseType::ORIGIN_HTML;
        } else if (language == "Style") {
            baseType = BaseType::ORIGIN_STYLE;
        } else if (language == "JavaScript") {
            baseType = BaseType::ORIGIN_JAVASCRIPT;
        } else {
            baseType = BaseType::ORIGIN_CUSTOM;
        }
    }

    std::string OriginType::toHTML() const {
        return rawContent; // 原始嵌入直接返回内容
    }

    // ConfigurationType实现
    ConfigurationType::ConfigurationType(const std::string& name) 
        : CHTLType(BaseType::CONFIGURATION, name.empty() ? "Configuration" : name) {
    }

    void ConfigurationType::setSetting(const std::string& key, const std::string& value) {
        configSettings[key] = value;
        setProperty("config_" + key, value);
    }

    std::string ConfigurationType::getSetting(const std::string& key) const {
        auto it = configSettings.find(key);
        return (it != configSettings.end()) ? it->second : "";
    }

    void ConfigurationType::setNameGroup(const std::string& key, const std::vector<std::string>& values) {
        nameGroups[key] = values;
    }

    std::vector<std::string> ConfigurationType::getNameGroup(const std::string& key) const {
        auto it = nameGroups.find(key);
        return (it != nameGroups.end()) ? it->second : std::vector<std::string>();
    }

    void ConfigurationType::setOriginType(const std::string& typeName, const std::string& typeValue) {
        originTypes[typeName] = typeValue;
    }

    std::string ConfigurationType::getOriginType(const std::string& typeName) const {
        auto it = originTypes.find(typeName);
        return (it != originTypes.end()) ? it->second : "";
    }

    std::string ConfigurationType::toString() const {
        std::stringstream ss;
        ss << "[Configuration]";
        if (!attributes.name.empty() && attributes.name != "Configuration") {
            ss << " @Config " << attributes.name;
        }
        return ss.str();
    }

    // ImportType实现
    ImportType::ImportType(const std::string& type, const std::string& path) 
        : CHTLType(BaseType::IMPORT_STATEMENT, "import"), importType(type), sourcePath(path) {
    }

    std::string ImportType::toString() const {
        std::stringstream ss;
        ss << "[Import] " << importType << " from " << sourcePath;
        if (!aliasName.empty()) {
            ss << " as " << aliasName;
        }
        return ss.str();
    }

    // NamespaceType实现
    NamespaceType::NamespaceType(const std::string& name) 
        : CHTLType(BaseType::NAMESPACE, name) {
        namespacePath.push_back(name);
    }

    NamespaceType::NamespaceType(const std::vector<std::string>& path) 
        : CHTLType(BaseType::NAMESPACE, ""), namespacePath(path) {
        if (!path.empty()) {
            attributes.name = path.back();
        }
    }

    void NamespaceType::addNamespace(const std::string& name) {
        namespacePath.push_back(name);
        attributes.name = name;
    }

    std::string NamespaceType::getFullPath() const {
        std::stringstream ss;
        for (size_t i = 0; i < namespacePath.size(); ++i) {
            if (i > 0) ss << ".";
            ss << namespacePath[i];
        }
        return ss.str();
    }

    std::string NamespaceType::toString() const {
        return "[Namespace] " + getFullPath();
    }

    // ConstraintType实现
    ConstraintType::ConstraintType() 
        : CHTLType(BaseType::CONSTRAINT, "constraint"), isGlobal(false) {
    }

    void ConstraintType::addProhibitedElement(const std::string& element) {
        prohibitedElements.push_back(element);
    }

    void ConstraintType::addProhibitedType(const std::string& type) {
        prohibitedTypes.push_back(type);
    }

    bool ConstraintType::isElementProhibited(const std::string& element) const {
        return std::find(prohibitedElements.begin(), prohibitedElements.end(), element) 
               != prohibitedElements.end();
    }

    bool ConstraintType::isTypeProhibited(const std::string& type) const {
        return std::find(prohibitedTypes.begin(), prohibitedTypes.end(), type) 
               != prohibitedTypes.end();
    }

    std::string ConstraintType::toString() const {
        std::stringstream ss;
        ss << "except ";
        for (size_t i = 0; i < prohibitedElements.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << prohibitedElements[i];
        }
        for (size_t i = 0; i < prohibitedTypes.size(); ++i) {
            if (!prohibitedElements.empty() || i > 0) ss << ", ";
            ss << prohibitedTypes[i];
        }
        return ss.str();
    }

    // TypeSystem实现
    TypeSystem::TypeSystem() {
        // 初始化基础类型
    }

    void TypeSystem::registerType(const std::string& name, std::shared_ptr<CHTLType> type) {
        registeredTypes[name] = type;
    }

    std::shared_ptr<CHTLType> TypeSystem::getType(const std::string& name) const {
        auto it = registeredTypes.find(name);
        return (it != registeredTypes.end()) ? it->second : nullptr;
    }

    void TypeSystem::registerTemplate(std::shared_ptr<TemplateType> templateType) {
        templates.push_back(templateType);
        registerType(templateType->getName(), templateType);
    }

    std::shared_ptr<TemplateType> TypeSystem::getTemplate(const std::string& name) const {
        for (const auto& tmpl : templates) {
            if (tmpl->getName() == name) {
                return tmpl;
            }
        }
        return nullptr;
    }

    void TypeSystem::registerCustom(std::shared_ptr<CustomType> customType) {
        customs.push_back(customType);
        registerType(customType->getName(), customType);
    }

    std::shared_ptr<CustomType> TypeSystem::getCustom(const std::string& name) const {
        for (const auto& custom : customs) {
            if (custom->getName() == name) {
                return custom;
            }
        }
        return nullptr;
    }

    void TypeSystem::registerConfiguration(std::shared_ptr<ConfigurationType> config) {
        configurations.push_back(config);
        registerType(config->getName(), config);
    }

    std::shared_ptr<ConfigurationType> TypeSystem::getConfiguration(const std::string& name) const {
        if (name.empty()) {
            // 返回第一个无名配置或第一个配置
            for (const auto& config : configurations) {
                if (config->getName() == "Configuration") {
                    return config;
                }
            }
            return configurations.empty() ? nullptr : configurations[0];
        }
        
        for (const auto& config : configurations) {
            if (config->getName() == name) {
                return config;
            }
        }
        return nullptr;
    }

    bool TypeSystem::isTypeCompatible(const std::string& parentType, const std::string& childType) const {
        auto parent = getType(parentType);
        auto child = getType(childType);
        
        if (!parent || !child) {
            return false;
        }
        
        return parent->canHaveChild(*child);
    }

    std::shared_ptr<CHTLType> TypeSystem::createType(BaseType baseType, const std::string& name) const {
        switch (baseType) {
            case BaseType::TEXT:
                return std::make_shared<TextType>(name);
            case BaseType::HTML_ELEMENT:
                return std::make_shared<HTMLElementType>(name);
            case BaseType::STYLE_BLOCK:
                return std::make_shared<StyleBlockType>(name);
            case BaseType::TEMPLATE_STYLE:
            case BaseType::TEMPLATE_ELEMENT:
            case BaseType::TEMPLATE_VAR: {
                std::string templateType = (baseType == BaseType::TEMPLATE_STYLE) ? "Style" :
                                         (baseType == BaseType::TEMPLATE_ELEMENT) ? "Element" : "Var";
                return std::make_shared<TemplateType>(templateType, name);
            }
            case BaseType::CUSTOM_STYLE:
            case BaseType::CUSTOM_ELEMENT:
            case BaseType::CUSTOM_VAR: {
                std::string customType = (baseType == BaseType::CUSTOM_STYLE) ? "Style" :
                                       (baseType == BaseType::CUSTOM_ELEMENT) ? "Element" : "Var";
                return std::make_shared<CustomType>(customType, name);
            }
            case BaseType::ORIGIN_HTML:
            case BaseType::ORIGIN_STYLE:
            case BaseType::ORIGIN_JAVASCRIPT:
            case BaseType::ORIGIN_CUSTOM: {
                std::string language = (baseType == BaseType::ORIGIN_HTML) ? "Html" :
                                     (baseType == BaseType::ORIGIN_STYLE) ? "Style" :
                                     (baseType == BaseType::ORIGIN_JAVASCRIPT) ? "JavaScript" : name;
                return std::make_shared<OriginType>(language);
            }
            case BaseType::CONFIGURATION:
                return std::make_shared<ConfigurationType>(name);
            case BaseType::IMPORT_STATEMENT:
                return std::make_shared<ImportType>("", "");
            case BaseType::NAMESPACE:
                return std::make_shared<NamespaceType>(name);
            case BaseType::CONSTRAINT:
                return std::make_shared<ConstraintType>();
            default:
                return std::make_shared<CHTLType>(baseType, name);
        }
    }
}