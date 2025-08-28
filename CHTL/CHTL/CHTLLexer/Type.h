#ifndef CHTL_TYPE_H
#define CHTL_TYPE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

    // 前向声明
    class TypeSystem;

    // CHTL基础类型枚举
    enum class BaseType {
        // 文本类型
        TEXT,               // 文本节点
        UNQUOTED_TEXT,      // 无引号文本
        STRING,             // 字符串字面量
        
        // 元素类型
        HTML_ELEMENT,       // HTML元素节点
        
        // 属性类型
        ATTRIBUTE,          // 元素属性
        
        // 样式类型
        STYLE_BLOCK,        // 样式块
        INLINE_STYLE,       // 内联样式
        CLASS_SELECTOR,     // 类选择器
        ID_SELECTOR,        // ID选择器
        
        // 模板类型
        TEMPLATE_STYLE,     // 样式组模板
        TEMPLATE_ELEMENT,   // 元素模板
        TEMPLATE_VAR,       // 变量组模板
        
        // 自定义类型
        CUSTOM_STYLE,       // 自定义样式组
        CUSTOM_ELEMENT,     // 自定义元素
        CUSTOM_VAR,         // 自定义变量组
        
        // 原始嵌入类型
        ORIGIN_HTML,        // 原始HTML
        ORIGIN_STYLE,       // 原始CSS
        ORIGIN_JAVASCRIPT,  // 原始JavaScript
        ORIGIN_CUSTOM,      // 自定义原始类型
        
        // 配置类型
        CONFIGURATION,      // 配置组
        NAME_CONFIG,        // 名称配置
        ORIGIN_TYPE_CONFIG, // 原始类型配置
        
        // 导入类型
        IMPORT_STATEMENT,   // 导入语句
        
        // 命名空间类型
        NAMESPACE,          // 命名空间
        
        // 约束类型
        CONSTRAINT,         // 约束定义
        
        // 其他
        COMMENT,            // 注释
        UNKNOWN             // 未知类型
    };

    // 类型属性结构
    struct TypeAttributes {
        std::string name;                           // 类型名称
        std::unordered_map<std::string, std::string> properties; // 属性映射
        std::vector<std::string> allowedChildren;   // 允许的子类型
        std::vector<std::string> requiredProperties; // 必需属性
        bool isNullable;                           // 是否可为空
        bool isTemplate;                           // 是否为模板
        bool isCustom;                             // 是否为自定义
        
        TypeAttributes() : isNullable(false), isTemplate(false), isCustom(false) {}
    };

    // CHTL类型基类
    class CHTLType {
    protected:
        BaseType baseType;
        TypeAttributes attributes;
        std::vector<std::shared_ptr<CHTLType>> children;
        std::weak_ptr<CHTLType> parent;
        
    public:
        CHTLType(BaseType type, const std::string& name = "");
        virtual ~CHTLType() = default;
        
        // 基础方法
        BaseType getBaseType() const { return baseType; }
        const std::string& getName() const { return attributes.name; }
        const TypeAttributes& getAttributes() const { return attributes; }
        
        // 层次结构方法
        void addChild(std::shared_ptr<CHTLType> child);
        void removeChild(std::shared_ptr<CHTLType> child);
        const std::vector<std::shared_ptr<CHTLType>>& getChildren() const { return children; }
        std::shared_ptr<CHTLType> getParent() const { return parent.lock(); }
        void setParent(std::shared_ptr<CHTLType> parentType) { parent = parentType; }
        
        // 属性操作
        void setProperty(const std::string& key, const std::string& value);
        std::string getProperty(const std::string& key) const;
        bool hasProperty(const std::string& key) const;
        
        // 类型检查
        virtual bool isCompatibleWith(const CHTLType& other) const;
        virtual bool canHaveChild(const CHTLType& child) const;
        virtual bool isValid() const;
        
        // 序列化
        virtual std::string toString() const;
        virtual std::string toHTML() const;
    };

    // 具体类型实现

    // 文本类型
    class TextType : public CHTLType {
    public:
        TextType(const std::string& content = "");
        std::string toHTML() const override;
    };

    // HTML元素类型
    class HTMLElementType : public CHTLType {
    private:
        std::string tagName;
        std::unordered_map<std::string, std::string> htmlAttributes;
        
    public:
        HTMLElementType(const std::string& tag);
        
        void setAttribute(const std::string& name, const std::string& value);
        std::string getAttribute(const std::string& name) const;
        const std::string& getTagName() const { return tagName; }
        
        bool canHaveChild(const CHTLType& child) const override;
        std::string toHTML() const override;
    };

    // 样式块类型
    class StyleBlockType : public CHTLType {
    private:
        std::unordered_map<std::string, std::string> cssProperties;
        std::string selectorType; // "inline", "class", "id", "element"
        std::string selectorValue;
        
    public:
        StyleBlockType(const std::string& selector = "");
        
        void addCSSProperty(const std::string& property, const std::string& value);
        void setSelectorType(const std::string& type, const std::string& value = "");
        
        std::string toHTML() const override;
        std::string toCSS() const;
    };

    // 模板类型
    class TemplateType : public CHTLType {
    private:
        std::string templateType; // "Style", "Element", "Var"
        std::unordered_map<std::string, std::string> templateData;
        
    public:
        TemplateType(const std::string& type, const std::string& name);
        
        void setTemplateData(const std::string& key, const std::string& value);
        std::string getTemplateData(const std::string& key) const;
        
        // 模板继承
        virtual bool inheritFrom(const TemplateType& baseTemplate);
        
        std::string toString() const override;
    };

    // 自定义类型
    class CustomType : public TemplateType {
    private:
        std::vector<std::string> deletedProperties;
        std::unordered_map<std::string, std::string> overriddenProperties;
        
    public:
        CustomType(const std::string& type, const std::string& name);
        
        // 特例化操作
        void deleteProperty(const std::string& property);
        void overrideProperty(const std::string& property, const std::string& value);
        
        bool inheritFrom(const TemplateType& baseTemplate) override;
        std::string toString() const override;
    };

    // 原始嵌入类型
    class OriginType : public CHTLType {
    private:
        std::string originLanguage; // "Html", "Style", "JavaScript", 或自定义
        std::string rawContent;
        
    public:
        OriginType(const std::string& language, const std::string& content = "");
        
        void setRawContent(const std::string& content) { rawContent = content; }
        const std::string& getRawContent() const { return rawContent; }
        const std::string& getLanguage() const { return originLanguage; }
        
        std::string toHTML() const override;
    };

    // 配置类型
    class ConfigurationType : public CHTLType {
    private:
        std::unordered_map<std::string, std::string> configSettings;
        std::unordered_map<std::string, std::vector<std::string>> nameGroups;
        std::unordered_map<std::string, std::string> originTypes;
        
    public:
        ConfigurationType(const std::string& name = "");
        
        // 配置设置
        void setSetting(const std::string& key, const std::string& value);
        std::string getSetting(const std::string& key) const;
        
        // 名称组配置
        void setNameGroup(const std::string& key, const std::vector<std::string>& values);
        std::vector<std::string> getNameGroup(const std::string& key) const;
        
        // 原始类型配置
        void setOriginType(const std::string& typeName, const std::string& typeValue);
        std::string getOriginType(const std::string& typeName) const;
        
        std::string toString() const override;
    };

    // 导入类型
    class ImportType : public CHTLType {
    private:
        std::string importType;     // "Html", "Style", "JavaScript", "Chtl", "Custom", "Template", etc.
        std::string sourcePath;
        std::string aliasName;
        std::string targetName;
        
    public:
        ImportType(const std::string& type, const std::string& path);
        
        void setAlias(const std::string& alias) { aliasName = alias; }
        void setTarget(const std::string& target) { targetName = target; }
        
        const std::string& getImportType() const { return importType; }
        const std::string& getSourcePath() const { return sourcePath; }
        const std::string& getAlias() const { return aliasName; }
        const std::string& getTarget() const { return targetName; }
        
        std::string toString() const override;
    };

    // 命名空间类型
    class NamespaceType : public CHTLType {
    private:
        std::vector<std::string> namespacePath;
        
    public:
        NamespaceType(const std::string& name);
        NamespaceType(const std::vector<std::string>& path);
        
        void addNamespace(const std::string& name);
        std::string getFullPath() const;
        const std::vector<std::string>& getPath() const { return namespacePath; }
        
        std::string toString() const override;
    };

    // 约束类型
    class ConstraintType : public CHTLType {
    private:
        std::vector<std::string> prohibitedElements;
        std::vector<std::string> prohibitedTypes;
        bool isGlobal;
        
    public:
        ConstraintType();
        
        void addProhibitedElement(const std::string& element);
        void addProhibitedType(const std::string& type);
        void setGlobal(bool global) { isGlobal = global; }
        
        bool isElementProhibited(const std::string& element) const;
        bool isTypeProhibited(const std::string& type) const;
        bool getIsGlobal() const { return isGlobal; }
        
        std::string toString() const override;
    };

    // 类型系统管理器
    class TypeSystem {
    private:
        std::unordered_map<std::string, std::shared_ptr<CHTLType>> registeredTypes;
        std::vector<std::shared_ptr<TemplateType>> templates;
        std::vector<std::shared_ptr<CustomType>> customs;
        std::vector<std::shared_ptr<ConfigurationType>> configurations;
        
    public:
        TypeSystem();
        
        // 类型注册
        void registerType(const std::string& name, std::shared_ptr<CHTLType> type);
        std::shared_ptr<CHTLType> getType(const std::string& name) const;
        
        // 模板管理
        void registerTemplate(std::shared_ptr<TemplateType> templateType);
        std::shared_ptr<TemplateType> getTemplate(const std::string& name) const;
        
        // 自定义类型管理
        void registerCustom(std::shared_ptr<CustomType> customType);
        std::shared_ptr<CustomType> getCustom(const std::string& name) const;
        
        // 配置管理
        void registerConfiguration(std::shared_ptr<ConfigurationType> config);
        std::shared_ptr<ConfigurationType> getConfiguration(const std::string& name = "") const;
        
        // 类型检查
        bool isTypeCompatible(const std::string& parentType, const std::string& childType) const;
        
        // 类型创建工厂方法
        std::shared_ptr<CHTLType> createType(BaseType baseType, const std::string& name = "") const;
    };
}

#endif // CHTL_TYPE_H