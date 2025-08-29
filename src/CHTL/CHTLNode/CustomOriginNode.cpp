#include "CustomOriginNode.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <fstream>
#include <iostream>

namespace CHTL {

// ========== VueTypeHandler 实现 ==========

bool VueTypeHandler::validateContent(const String& content) const {
    // 检查Vue单文件组件基本结构
    return content.find("<template>") != String::npos || 
           content.find("<script>") != String::npos ||
           content.find("<style>") != String::npos;
}

String VueTypeHandler::transformContent(const String& content, const String& targetFormat) const {
    if (targetFormat == "html") {
        return extractTemplate(content);
    } else if (targetFormat == "js") {
        return extractScript(content);
    } else if (targetFormat == "css") {
        return extractStyle(content);
    }
    return content;
}

StringVector VueTypeHandler::extractDependencies(const String& content) const {
    StringVector deps;
    String script = extractScript(content);
    
    // 提取import语句
    std::regex importRegex(R"(import\s+.*?\s+from\s+['"]([^'"]+)['"])");
    std::sregex_iterator iter(script.begin(), script.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        deps.push_back(iter->str(1));
    }
    
    return deps;
}

String VueTypeHandler::compile(const String& content, const StringUnorderedMap& options) const {
    std::ostringstream oss;
    
    // 简化的Vue组件编译
    oss << "// Vue Component Compiled Output\n";
    oss << "(function() {\n";
    oss << "  const template = `" << extractTemplate(content) << "`;\n";
    oss << "  const script = " << extractScript(content) << ";\n";
    oss << "  const style = `" << extractStyle(content) << "`;\n";
    oss << "  \n";
    oss << "  // Component registration logic\n";
    oss << "  Vue.component('custom-component', {\n";
    oss << "    template: template,\n";
    oss << "    ...script\n";
    oss << "  });\n";
    oss << "})();\n";
    
    return oss.str();
}

String VueTypeHandler::extractTemplate(const String& content) const {
    std::regex templateRegex(R"(<template[^>]*>(.*?)</template>)", std::regex::dotall);
    std::smatch match;
    
    if (std::regex_search(content, match, templateRegex)) {
        return match[1].str();
    }
    
    return "";
}

String VueTypeHandler::extractScript(const String& content) const {
    std::regex scriptRegex(R"(<script[^>]*>(.*?)</script>)", std::regex::dotall);
    std::smatch match;
    
    if (std::regex_search(content, match, scriptRegex)) {
        return match[1].str();
    }
    
    return "{}";
}

String VueTypeHandler::extractStyle(const String& content) const {
    std::regex styleRegex(R"(<style[^>]*>(.*?)</style>)", std::regex::dotall);
    std::smatch match;
    
    if (std::regex_search(content, match, styleRegex)) {
        return match[1].str();
    }
    
    return "";
}

StringUnorderedMap VueTypeHandler::extractProps(const String& script) const {
    StringUnorderedMap props;
    
    // 简化的props提取
    std::regex propsRegex(R"(props:\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(script, match, propsRegex)) {
        String propsContent = match[1].str();
        std::regex propRegex(R"((\w+):\s*([^,}]+))");
        std::sregex_iterator iter(propsContent.begin(), propsContent.end(), propRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            props[iter->str(1)] = iter->str(2);
        }
    }
    
    return props;
}

// ========== ReactTypeHandler 实现 ==========

bool ReactTypeHandler::validateContent(const String& content) const {
    // 检查React组件基本特征
    return content.find("import React") != String::npos ||
           content.find("function ") != String::npos ||
           content.find("class ") != String::npos ||
           content.find("const ") != String::npos ||
           content.find("export default") != String::npos;
}

String ReactTypeHandler::transformContent(const String& content, const String& targetFormat) const {
    if (targetFormat == "html") {
        // 提取JSX并转换为HTML (简化)
        String component = extractComponent(content);
        std::regex jsxRegex(R"(return\s*\((.*?)\);)", std::regex::dotall);
        std::smatch match;
        
        if (std::regex_search(component, match, jsxRegex)) {
            return match[1].str();
        }
    }
    
    return content;
}

StringVector ReactTypeHandler::extractDependencies(const String& content) const {
    return extractImports(content);
}

String ReactTypeHandler::compile(const String& content, const StringUnorderedMap& options) const {
    std::ostringstream oss;
    
    // 简化的React组件编译
    oss << "// React Component Compiled Output\n";
    oss << "import React from 'react';\n\n";
    oss << content << "\n";
    oss << "\nexport default CustomComponent;\n";
    
    return oss.str();
}

StringVector ReactTypeHandler::extractImports(const String& content) const {
    StringVector imports;
    std::regex importRegex(R"(import\s+.*?\s+from\s+['"]([^'"]+)['"])");
    std::sregex_iterator iter(content.begin(), content.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        imports.push_back(iter->str(1));
    }
    
    return imports;
}

String ReactTypeHandler::extractComponent(const String& content) const {
    // 提取函数组件或类组件
    std::regex funcRegex(R"(function\s+\w+\s*\([^)]*\)\s*\{(.*?)\})", std::regex::dotall);
    std::regex classRegex(R"(class\s+\w+.*?\{(.*?)\})", std::regex::dotall);
    std::smatch match;
    
    if (std::regex_search(content, match, funcRegex) || 
        std::regex_search(content, match, classRegex)) {
        return match[1].str();
    }
    
    return content;
}

StringUnorderedMap ReactTypeHandler::extractProps(const String& content) const {
    StringUnorderedMap props;
    
    // 从PropTypes或TypeScript接口提取props
    std::regex propTypesRegex(R"(propTypes\s*=\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, propTypesRegex)) {
        String propsContent = match[1].str();
        std::regex propRegex(R"((\w+):\s*([^,}]+))");
        std::sregex_iterator iter(propsContent.begin(), propsContent.end(), propRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            props[iter->str(1)] = iter->str(2);
        }
    }
    
    return props;
}

// ========== MarkdownTypeHandler 实现 ==========

bool MarkdownTypeHandler::validateContent(const String& content) const {
    // 检查Markdown基本特征
    return content.find("#") != String::npos ||
           content.find("*") != String::npos ||
           content.find("[") != String::npos ||
           content.find("`") != String::npos;
}

String MarkdownTypeHandler::transformContent(const String& content, const String& targetFormat) const {
    if (targetFormat == "html") {
        return parseMarkdownToHTML(content);
    }
    return content;
}

StringVector MarkdownTypeHandler::extractDependencies(const String& content) const {
    StringVector deps;
    
    // 提取图片和链接
    StringVector links = extractLinks(content);
    deps.insert(deps.end(), links.begin(), links.end());
    
    return deps;
}

String MarkdownTypeHandler::compile(const String& content, const StringUnorderedMap& options) const {
    return parseMarkdownToHTML(content);
}

String MarkdownTypeHandler::parseMarkdownToHTML(const String& content) const {
    String html = content;
    
    // 简化的Markdown到HTML转换
    // 标题
    std::regex h1Regex(R"(^# (.+)$)", std::regex::multiline);
    html = std::regex_replace(html, h1Regex, "<h1>$1</h1>");
    
    std::regex h2Regex(R"(^## (.+)$)", std::regex::multiline);
    html = std::regex_replace(html, h2Regex, "<h2>$1</h2>");
    
    std::regex h3Regex(R"(^### (.+)$)", std::regex::multiline);
    html = std::regex_replace(html, h3Regex, "<h3>$1</h3>");
    
    // 粗体
    std::regex boldRegex(R"(\*\*([^*]+)\*\*)");
    html = std::regex_replace(html, boldRegex, "<strong>$1</strong>");
    
    // 斜体
    std::regex italicRegex(R"(\*([^*]+)\*)");
    html = std::regex_replace(html, italicRegex, "<em>$1</em>");
    
    // 链接
    std::regex linkRegex(R"(\[([^\]]+)\]\(([^)]+)\))");
    html = std::regex_replace(html, linkRegex, "<a href=\"$2\">$1</a>");
    
    // 代码块
    std::regex codeRegex(R"(`([^`]+)`)");
    html = std::regex_replace(html, codeRegex, "<code>$1</code>");
    
    // 段落
    std::regex paragraphRegex(R"(^(?!<[hH][1-6]>)(.+)$)", std::regex::multiline);
    html = std::regex_replace(html, paragraphRegex, "<p>$1</p>");
    
    return html;
}

StringVector MarkdownTypeHandler::extractCodeBlocks(const String& content) const {
    StringVector codeBlocks;
    std::regex codeRegex(R"(```([^`]+)```)");
    std::sregex_iterator iter(content.begin(), content.end(), codeRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        codeBlocks.push_back(iter->str(1));
    }
    
    return codeBlocks;
}

StringVector MarkdownTypeHandler::extractLinks(const String& content) const {
    StringVector links;
    std::regex linkRegex(R"(\[([^\]]+)\]\(([^)]+)\))");
    std::sregex_iterator iter(content.begin(), content.end(), linkRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        links.push_back(iter->str(2));
    }
    
    return links;
}

// ========== YamlTypeHandler 实现 ==========

bool YamlTypeHandler::validateContent(const String& content) const {
    return validateYamlSyntax(content);
}

String YamlTypeHandler::transformContent(const String& content, const String& targetFormat) const {
    if (targetFormat == "json") {
        return yamlToJson(content);
    }
    return content;
}

StringVector YamlTypeHandler::extractDependencies(const String& content) const {
    StringVector deps;
    
    // 提取YAML中的文件引用
    std::regex fileRegex(R"(file:\s*['"]?([^'"]+)['"]?)");
    std::sregex_iterator iter(content.begin(), content.end(), fileRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        deps.push_back(iter->str(1));
    }
    
    return deps;
}

String YamlTypeHandler::compile(const String& content, const StringUnorderedMap& options) const {
    // YAML通常不需要编译，直接返回
    return content;
}

String YamlTypeHandler::yamlToJson(const String& yaml) const {
    // 简化的YAML到JSON转换
    std::ostringstream json;
    json << "{\n";
    
    std::istringstream iss(yaml);
    String line;
    bool first = true;
    
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != String::npos) {
            if (!first) json << ",\n";
            first = false;
            
            String key = line.substr(0, colonPos);
            String value = line.substr(colonPos + 1);
            
            // 去除前后空白
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            json << "  \"" << key << "\": \"" << value << "\"";
        }
    }
    
    json << "\n}";
    return json.str();
}

bool YamlTypeHandler::validateYamlSyntax(const String& yaml) const {
    // 简化的YAML语法验证
    std::istringstream iss(yaml);
    String line;
    
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // 检查基本的键值对格式
        if (line.find(':') == String::npos && !line.empty()) {
            return false;
        }
    }
    
    return true;
}

// ========== CustomOriginNode 实现 ==========

CustomOriginNode::CustomOriginNode(CustomOriginType type, const String& identifier, const Position& pos)
    : OriginNode(pos), customType(type), typeIdentifier(identifier), 
      isCompiled(false), targetFormat("html") {
    
    initializeHandler();
}

void CustomOriginNode::setHandler(std::shared_ptr<CustomOriginTypeHandler> typeHandler) {
    handler = typeHandler;
}

void CustomOriginNode::setContent(const String& content) {
    OriginNode::setContent(content);
    isCompiled = false;
    compiledContent.clear();
    
    // 提取依赖
    if (handler) {
        dependencies = handler->extractDependencies(content);
    }
}

String CustomOriginNode::getContent() const {
    return OriginNode::getContent();
}

String CustomOriginNode::getTransformedContent(const String& format) const {
    if (!handler) {
        return getContent();
    }
    
    String targetFmt = format.empty() ? targetFormat : format;
    return handler->transformContent(getContent(), targetFmt);
}

bool CustomOriginNode::compile(const StringUnorderedMap& options) {
    if (!handler || !handler->requiresCompilation()) {
        return true;
    }
    
    String content = getContent();
    if (content.empty()) {
        return false;
    }
    
    try {
        StringUnorderedMap allOptions = compileOptions;
        for (const auto& [key, value] : options) {
            allOptions[key] = value;
        }
        
        compiledContent = handler->compile(content, allOptions);
        isCompiled = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "编译错误: " << e.what() << std::endl;
        return false;
    }
}

bool CustomOriginNode::isCompilationRequired() const {
    return handler && handler->requiresCompilation();
}

void CustomOriginNode::setCompileOptions(const StringUnorderedMap& options) {
    compileOptions = options;
    isCompiled = false; // 重置编译状态
}

StringVector CustomOriginNode::extractDependencies() {
    if (handler) {
        dependencies = handler->extractDependencies(getContent());
    }
    return dependencies;
}

void CustomOriginNode::addDependency(const String& dependency) {
    if (std::find(dependencies.begin(), dependencies.end(), dependency) == dependencies.end()) {
        dependencies.push_back(dependency);
    }
}

void CustomOriginNode::setDependencies(const StringVector& deps) {
    dependencies = deps;
}

bool CustomOriginNode::validateContent() const {
    if (!handler) {
        return true;
    }
    
    return handler->validateContent(getContent());
}

StringVector CustomOriginNode::getValidationErrors() const {
    StringVector errors;
    
    if (!handler) {
        errors.push_back("未设置类型处理器");
        return errors;
    }
    
    if (!validateContent()) {
        errors.push_back("内容格式无效");
    }
    
    if (getContent().empty()) {
        errors.push_back("内容不能为空");
    }
    
    return errors;
}

bool CustomOriginNode::loadFromFile(const String& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    file.close();
    
    setContent(content.str());
    sourceFile = filePath;
    
    return true;
}

bool CustomOriginNode::saveToFile(const String& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    String content = isCompiled ? compiledContent : getContent();
    file << content;
    file.close();
    
    return true;
}

String CustomOriginNode::convertToFormat(const String& format) const {
    if (!handler) {
        return getContent();
    }
    
    return handler->transformContent(getContent(), format);
}

String CustomOriginNode::getValue() const {
    return typeIdentifier + " " + getContent();
}

std::unique_ptr<BaseNode> CustomOriginNode::clone() const {
    auto cloned = std::make_unique<CustomOriginNode>(customType, typeIdentifier, position);
    cloned->setContent(getContent());
    cloned->handler = handler;
    cloned->compileOptions = compileOptions;
    cloned->dependencies = dependencies;
    cloned->isCompiled = isCompiled;
    cloned->compiledContent = compiledContent;
    cloned->sourceFile = sourceFile;
    cloned->targetFormat = targetFormat;
    return cloned;
}

String CustomOriginNode::toString() const {
    std::ostringstream oss;
    oss << "[Origin] " << typeIdentifier << " " << getNodeName() << " {\n";
    oss << getContent() << "\n";
    oss << "}";
    return oss.str();
}

String CustomOriginNode::toHTML() const {
    return getTransformedContent("html");
}

String CustomOriginNode::toJSON() const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"type\": \"" << customTypeToString(customType) << "\",\n";
    json << "  \"identifier\": \"" << typeIdentifier << "\",\n";
    json << "  \"content\": \"" << getContent() << "\",\n";
    json << "  \"dependencies\": [";
    
    for (size_t i = 0; i < dependencies.size(); ++i) {
        if (i > 0) json << ", ";
        json << "\"" << dependencies[i] << "\"";
    }
    
    json << "],\n";
    json << "  \"compiled\": " << (isCompiled ? "true" : "false") << "\n";
    json << "}";
    
    return json.str();
}

String CustomOriginNode::toXML() const {
    std::ostringstream xml;
    xml << "<customOrigin type=\"" << customTypeToString(customType) << "\" identifier=\"" << typeIdentifier << "\">\n";
    xml << "  <content><![CDATA[" << getContent() << "]]></content>\n";
    xml << "  <dependencies>\n";
    
    for (const String& dep : dependencies) {
        xml << "    <dependency>" << dep << "</dependency>\n";
    }
    
    xml << "  </dependencies>\n";
    xml << "  <compiled>" << (isCompiled ? "true" : "false") << "</compiled>\n";
    xml << "</customOrigin>";
    
    return xml.str();
}

// 静态工厂方法
std::unique_ptr<CustomOriginNode> CustomOriginNode::createVueComponent(const String& content, const Position& pos) {
    auto node = std::make_unique<CustomOriginNode>(CustomOriginType::VUE, "@Vue", pos);
    node->setContent(content);
    return node;
}

std::unique_ptr<CustomOriginNode> CustomOriginNode::createReactComponent(const String& content, const Position& pos) {
    auto node = std::make_unique<CustomOriginNode>(CustomOriginType::REACT, "@React", pos);
    node->setContent(content);
    return node;
}

std::unique_ptr<CustomOriginNode> CustomOriginNode::createMarkdownDocument(const String& content, const Position& pos) {
    auto node = std::make_unique<CustomOriginNode>(CustomOriginType::MARKDOWN, "@Markdown", pos);
    node->setContent(content);
    return node;
}

std::unique_ptr<CustomOriginNode> CustomOriginNode::createYamlConfig(const String& content, const Position& pos) {
    auto node = std::make_unique<CustomOriginNode>(CustomOriginType::YAML, "@Yaml", pos);
    node->setContent(content);
    return node;
}

std::unique_ptr<CustomOriginNode> CustomOriginNode::createCustomType(CustomOriginType type, const String& identifier, 
                                                                   const String& content, const Position& pos) {
    auto node = std::make_unique<CustomOriginNode>(type, identifier, pos);
    node->setContent(content);
    return node;
}

CustomOriginType CustomOriginNode::parseCustomType(const String& typeIdentifier) {
    static const std::unordered_map<String, CustomOriginType> typeMap = {
        {"@Vue", CustomOriginType::VUE},
        {"@React", CustomOriginType::REACT},
        {"@Angular", CustomOriginType::ANGULAR},
        {"@Svelte", CustomOriginType::SVELTE},
        {"@Markdown", CustomOriginType::MARKDOWN},
        {"@Yaml", CustomOriginType::YAML},
        {"@Json", CustomOriginType::JSON},
        {"@Xml", CustomOriginType::XML},
        {"@Sql", CustomOriginType::SQL},
        {"@Python", CustomOriginType::PYTHON},
        {"@TypeScript", CustomOriginType::TYPESCRIPT},
        {"@Scss", CustomOriginType::SCSS},
        {"@Less", CustomOriginType::LESS},
        {"@GraphQL", CustomOriginType::GRAPHQL},
        {"@Wasm", CustomOriginType::WASM},
        {"@Glsl", CustomOriginType::GLSL},
        {"@Hlsl", CustomOriginType::HLSL}
    };
    
    auto it = typeMap.find(typeIdentifier);
    return it != typeMap.end() ? it->second : CustomOriginType::CUSTOM;
}

String CustomOriginNode::customTypeToString(CustomOriginType type) {
    switch (type) {
        case CustomOriginType::VUE: return "Vue";
        case CustomOriginType::REACT: return "React";
        case CustomOriginType::ANGULAR: return "Angular";
        case CustomOriginType::SVELTE: return "Svelte";
        case CustomOriginType::MARKDOWN: return "Markdown";
        case CustomOriginType::YAML: return "Yaml";
        case CustomOriginType::JSON: return "Json";
        case CustomOriginType::XML: return "Xml";
        case CustomOriginType::SQL: return "Sql";
        case CustomOriginType::PYTHON: return "Python";
        case CustomOriginType::TYPESCRIPT: return "TypeScript";
        case CustomOriginType::SCSS: return "Scss";
        case CustomOriginType::LESS: return "Less";
        case CustomOriginType::GRAPHQL: return "GraphQL";
        case CustomOriginType::WASM: return "Wasm";
        case CustomOriginType::GLSL: return "Glsl";
        case CustomOriginType::HLSL: return "Hlsl";
        case CustomOriginType::CUSTOM: return "Custom";
        default: return "Unknown";
    }
}

String CustomOriginNode::customTypeToIdentifier(CustomOriginType type) {
    return "@" + customTypeToString(type);
}

void CustomOriginNode::initializeHandler() {
    auto& registry = CustomOriginTypeRegistry::getInstance();
    handler = registry.getHandler(customType);
}

bool CustomOriginNode::isValidTypeIdentifier(const String& identifier) const {
    return !identifier.empty() && identifier[0] == '@';
}

String CustomOriginNode::generateDefaultContent() const {
    switch (customType) {
        case CustomOriginType::VUE:
            return "<template>\n  <div>Vue Component</div>\n</template>\n\n<script>\nexport default {\n  name: 'CustomComponent'\n}\n</script>";
        case CustomOriginType::REACT:
            return "import React from 'react';\n\nfunction CustomComponent() {\n  return <div>React Component</div>;\n}\n\nexport default CustomComponent;";
        case CustomOriginType::MARKDOWN:
            return "# Custom Document\n\nThis is a custom markdown document.";
        case CustomOriginType::YAML:
            return "name: custom-config\nversion: 1.0.0\ndescription: Custom YAML configuration";
        default:
            return "// Custom content";
    }
}

// ========== CustomOriginTypeRegistry 实现 ==========

CustomOriginTypeRegistry& CustomOriginTypeRegistry::getInstance() {
    static CustomOriginTypeRegistry instance;
    return instance;
}

void CustomOriginTypeRegistry::registerType(const String& identifier, CustomOriginType type, 
                                           std::function<std::shared_ptr<CustomOriginTypeHandler>()> handlerFactory) {
    TypeRegistration registration;
    registration.identifier = identifier;
    registration.type = type;
    registration.handlerFactory = handlerFactory;
    
    registrations[identifier] = registration;
    typeToIdentifier[type] = identifier;
}

void CustomOriginTypeRegistry::unregisterType(const String& identifier) {
    auto it = registrations.find(identifier);
    if (it != registrations.end()) {
        typeToIdentifier.erase(it->second.type);
        registrations.erase(it);
    }
}

bool CustomOriginTypeRegistry::isTypeRegistered(const String& identifier) const {
    return registrations.find(identifier) != registrations.end();
}

std::shared_ptr<CustomOriginTypeHandler> CustomOriginTypeRegistry::getHandler(const String& identifier) const {
    auto it = registrations.find(identifier);
    if (it != registrations.end()) {
        return it->second.handlerFactory();
    }
    return nullptr;
}

std::shared_ptr<CustomOriginTypeHandler> CustomOriginTypeRegistry::getHandler(CustomOriginType type) const {
    auto it = typeToIdentifier.find(type);
    if (it != typeToIdentifier.end()) {
        return getHandler(it->second);
    }
    return nullptr;
}

CustomOriginType CustomOriginTypeRegistry::getType(const String& identifier) const {
    auto it = registrations.find(identifier);
    return it != registrations.end() ? it->second.type : CustomOriginType::CUSTOM;
}

String CustomOriginTypeRegistry::getIdentifier(CustomOriginType type) const {
    auto it = typeToIdentifier.find(type);
    return it != typeToIdentifier.end() ? it->second : "";
}

StringVector CustomOriginTypeRegistry::getAllIdentifiers() const {
    StringVector identifiers;
    for (const auto& [identifier, registration] : registrations) {
        identifiers.push_back(identifier);
    }
    return identifiers;
}

StringVector CustomOriginTypeRegistry::getIdentifiersByType(CustomOriginType type) const {
    StringVector identifiers;
    for (const auto& [identifier, registration] : registrations) {
        if (registration.type == type) {
            identifiers.push_back(identifier);
        }
    }
    return identifiers;
}

void CustomOriginTypeRegistry::registerExtension(const String& extension, const String& identifier) {
    extensionMapping[extension] = identifier;
}

String CustomOriginTypeRegistry::getIdentifierByExtension(const String& extension) const {
    auto it = extensionMapping.find(extension);
    return it != extensionMapping.end() ? it->second : "";
}

StringVector CustomOriginTypeRegistry::getSupportedExtensions() const {
    StringVector extensions;
    for (const auto& [extension, identifier] : extensionMapping) {
        extensions.push_back(extension);
    }
    return extensions;
}

void CustomOriginTypeRegistry::registerBuiltinTypes() {
    registerWebFrameworkTypes();
    registerMarkupTypes();
    registerDataTypes();
    registerShaderTypes();
}

void CustomOriginTypeRegistry::registerWebFrameworkTypes() {
    registerBuiltinType("@Vue", CustomOriginType::VUE, 
                       []() { return std::make_shared<VueTypeHandler>(); }, 
                       {".vue"});
    
    registerBuiltinType("@React", CustomOriginType::REACT, 
                       []() { return std::make_shared<ReactTypeHandler>(); }, 
                       {".jsx", ".tsx"});
}

void CustomOriginTypeRegistry::registerMarkupTypes() {
    registerBuiltinType("@Markdown", CustomOriginType::MARKDOWN, 
                       []() { return std::make_shared<MarkdownTypeHandler>(); }, 
                       {".md", ".markdown"});
}

void CustomOriginTypeRegistry::registerDataTypes() {
    registerBuiltinType("@Yaml", CustomOriginType::YAML, 
                       []() { return std::make_shared<YamlTypeHandler>(); }, 
                       {".yml", ".yaml"});
}

void CustomOriginTypeRegistry::registerShaderTypes() {
    // 着色器类型注册可以在这里添加
}

std::unique_ptr<CustomOriginNode> CustomOriginTypeRegistry::createNode(const String& identifier, const String& content, 
                                                                      const Position& pos) const {
    CustomOriginType type = getType(identifier);
    auto node = std::make_unique<CustomOriginNode>(type, identifier, pos);
    node->setContent(content);
    return node;
}

void CustomOriginTypeRegistry::dumpRegisteredTypes() const {
    std::cout << "=== 已注册的自定义原始嵌入类型 ===\n";
    std::cout << "总数: " << registrations.size() << "\n\n";
    
    for (const auto& [identifier, registration] : registrations) {
        std::cout << "标识符: " << identifier << "\n";
        std::cout << "类型: " << CustomOriginNode::customTypeToString(registration.type) << "\n";
        std::cout << "扩展名: ";
        for (const String& ext : registration.supportedExtensions) {
            std::cout << ext << " ";
        }
        std::cout << "\n";
        std::cout << "描述: " << registration.description << "\n\n";
    }
}

size_t CustomOriginTypeRegistry::getRegisteredTypeCount() const {
    return registrations.size();
}

bool CustomOriginTypeRegistry::validateRegistry() const {
    // 检查是否有重复的类型标识符
    for (const auto& [identifier1, registration1] : registrations) {
        for (const auto& [identifier2, registration2] : registrations) {
            if (identifier1 != identifier2 && registration1.type == registration2.type) {
                return false; // 同一类型有多个标识符
            }
        }
    }
    
    return true;
}

void CustomOriginTypeRegistry::registerBuiltinType(const String& identifier, CustomOriginType type,
                                                  std::function<std::shared_ptr<CustomOriginTypeHandler>()> factory,
                                                  const StringVector& extensions) {
    registerType(identifier, type, factory);
    
    for (const String& ext : extensions) {
        registerExtension(ext, identifier);
    }
    
    // 更新注册信息
    auto it = registrations.find(identifier);
    if (it != registrations.end()) {
        it->second.supportedExtensions = extensions;
    }
}

// ========== CustomOriginTypeBuilder 实现 ==========

CustomOriginTypeBuilder::CustomOriginTypeBuilder(const String& identifier) {
    config.identifier = identifier;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::displayName(const String& name) {
    config.displayName = name;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::description(const String& desc) {
    config.description = desc;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::fileExtensions(const StringVector& extensions) {
    config.fileExtensions = extensions;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::mimeType(const String& mime) {
    config.mimeType = mime;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::requiresCompilation(bool requires) {
    config.requiresCompilation = requires;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::defaultCompileOptions(const StringUnorderedMap& options) {
    config.defaultCompileOptions = options;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::dependencies(const StringVector& deps) {
    config.dependencies = deps;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::syntaxHighlighting(const String& syntax) {
    config.syntaxHighlighting = syntax;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::icon(const String& iconPath) {
    config.iconPath = iconPath;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::documentation(const String& docUrl) {
    config.documentationUrl = docUrl;
    return *this;
}

CustomOriginTypeBuilder& CustomOriginTypeBuilder::handler(std::function<std::shared_ptr<CustomOriginTypeHandler>()> factory) {
    handlerFactory = factory;
    return *this;
}

CustomOriginTypeConfig CustomOriginTypeBuilder::build() {
    if (validateConfig()) {
        return config;
    }
    return CustomOriginTypeConfig{};
}

bool CustomOriginTypeBuilder::registerToGlobal() {
    if (!validateConfig() || !handlerFactory) {
        return false;
    }
    
    auto& registry = CustomOriginTypeRegistry::getInstance();
    CustomOriginType type = CustomOriginNode::parseCustomType(config.identifier);
    
    registry.registerType(config.identifier, type, handlerFactory);
    
    for (const String& ext : config.fileExtensions) {
        registry.registerExtension(ext, config.identifier);
    }
    
    return true;
}

bool CustomOriginTypeBuilder::validateConfig() const {
    return buildErrors.empty() && !config.identifier.empty();
}

void CustomOriginTypeBuilder::addBuildError(const String& error) {
    buildErrors.push_back(error);
}

// ========== CustomOriginUtils 实现 ==========

namespace CustomOriginUtils {

CustomOriginType detectContentType(const String& content) {
    // Vue组件检测
    if (content.find("<template>") != String::npos && content.find("<script>") != String::npos) {
        return CustomOriginType::VUE;
    }
    
    // React组件检测
    if (content.find("import React") != String::npos || content.find("export default") != String::npos) {
        return CustomOriginType::REACT;
    }
    
    // Markdown检测
    if (content.find("#") != String::npos || content.find("*") != String::npos || content.find("[") != String::npos) {
        return CustomOriginType::MARKDOWN;
    }
    
    // YAML检测
    if (content.find(":") != String::npos && content.find("---") != String::npos) {
        return CustomOriginType::YAML;
    }
    
    // JSON检测
    if ((content.find("{") != String::npos && content.find("}") != String::npos) ||
        (content.find("[") != String::npos && content.find("]") != String::npos)) {
        return CustomOriginType::JSON;
    }
    
    return CustomOriginType::CUSTOM;
}

String detectFileTypeByExtension(const String& filePath) {
    auto& registry = CustomOriginTypeRegistry::getInstance();
    
    size_t dotPos = filePath.rfind('.');
    if (dotPos != String::npos) {
        String extension = filePath.substr(dotPos);
        return registry.getIdentifierByExtension(extension);
    }
    
    return "";
}

String detectMimeType(const String& content) {
    CustomOriginType type = detectContentType(content);
    auto& registry = CustomOriginTypeRegistry::getInstance();
    auto handler = registry.getHandler(type);
    
    return handler ? handler->getMimeType() : "text/plain";
}

String convertContent(const String& content, CustomOriginType fromType, CustomOriginType toType) {
    auto& registry = CustomOriginTypeRegistry::getInstance();
    auto fromHandler = registry.getHandler(fromType);
    auto toHandler = registry.getHandler(toType);
    
    if (!fromHandler || !toHandler) {
        return content;
    }
    
    // 简化的转换逻辑
    String targetFormat = CustomOriginNode::customTypeToString(toType);
    std::transform(targetFormat.begin(), targetFormat.end(), targetFormat.begin(), ::tolower);
    
    return fromHandler->transformContent(content, targetFormat);
}

String normalizeContent(const String& content, CustomOriginType type) {
    // 规范化内容格式
    String normalized = content;
    
    // 统一换行符
    std::regex lineEndingRegex(R"(\r\n|\r)");
    normalized = std::regex_replace(normalized, lineEndingRegex, "\n");
    
    // 去除尾随空白
    std::regex trailingWhitespaceRegex(R"([ \t]+$)", std::regex::multiline);
    normalized = std::regex_replace(normalized, trailingWhitespaceRegex, "");
    
    return normalized;
}

StringVector analyzeContentDependencies(const String& content, CustomOriginType type) {
    auto& registry = CustomOriginTypeRegistry::getInstance();
    auto handler = registry.getHandler(type);
    
    return handler ? handler->extractDependencies(content) : StringVector{};
}

StringVector extractImportStatements(const String& content, CustomOriginType type) {
    StringVector imports;
    
    // 通用的import语句提取
    std::regex importRegex(R"(import\s+.*?\s+from\s+['"]([^'"]+)['"])");
    std::sregex_iterator iter(content.begin(), content.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        imports.push_back(iter->str(1));
    }
    
    return imports;
}

bool validateContentSyntax(const String& content, CustomOriginType type) {
    auto& registry = CustomOriginTypeRegistry::getInstance();
    auto handler = registry.getHandler(type);
    
    return handler ? handler->validateContent(content) : true;
}

StringVector getContentValidationErrors(const String& content, CustomOriginType type) {
    StringVector errors;
    
    if (!validateContentSyntax(content, type)) {
        errors.push_back("内容语法验证失败");
    }
    
    if (content.empty()) {
        errors.push_back("内容不能为空");
    }
    
    return errors;
}

StringUnorderedMap extractContentMetadata(const String& content, CustomOriginType type) {
    StringUnorderedMap metadata;
    
    metadata["type"] = CustomOriginNode::customTypeToString(type);
    metadata["size"] = std::to_string(content.size());
    metadata["lines"] = std::to_string(std::count(content.begin(), content.end(), '\n') + 1);
    
    // 提取标题
    String title = extractTitle(content, type);
    if (!title.empty()) {
        metadata["title"] = title;
    }
    
    return metadata;
}

String extractTitle(const String& content, CustomOriginType type) {
    switch (type) {
        case CustomOriginType::MARKDOWN: {
            std::regex titleRegex(R"(^# (.+)$)", std::regex::multiline);
            std::smatch match;
            if (std::regex_search(content, match, titleRegex)) {
                return match[1].str();
            }
            break;
        }
        case CustomOriginType::VUE: {
            std::regex nameRegex(R"(name:\s*['"]([^'"]+)['"])");
            std::smatch match;
            if (std::regex_search(content, match, nameRegex)) {
                return match[1].str();
            }
            break;
        }
        default:
            break;
    }
    
    return "";
}

String extractDescription(const String& content, CustomOriginType type) {
    // 提取内容描述的简化实现
    std::istringstream iss(content);
    String line;
    
    while (std::getline(iss, line)) {
        if (!line.empty() && line[0] != '#' && line.find("//") != 0) {
            return line.substr(0, 100); // 返回前100个字符作为描述
        }
    }
    
    return "";
}

String optimizeContent(const String& content, CustomOriginType type) {
    return normalizeContent(content, type);
}

String minifyContent(const String& content, CustomOriginType type) {
    String minified = content;
    
    // 移除多余的空白
    std::regex whitespaceRegex(R"(\s+)");
    minified = std::regex_replace(minified, whitespaceRegex, " ");
    
    // 移除注释 (简化实现)
    std::regex commentRegex(R"(//.*?$)", std::regex::multiline);
    minified = std::regex_replace(minified, commentRegex, "");
    
    return minified;
}

String beautifyContent(const String& content, CustomOriginType type) {
    // 美化内容的简化实现
    String beautified = normalizeContent(content, type);
    
    // 可以根据类型添加特定的美化逻辑
    switch (type) {
        case CustomOriginType::JSON: {
            // JSON美化逻辑
            break;
        }
        case CustomOriginType::XML: {
            // XML美化逻辑
            break;
        }
        default:
            break;
    }
    
    return beautified;
}

} // namespace CustomOriginUtils

// ========== TypeScriptAdvancedHandler 实现 ==========

bool TypeScriptAdvancedHandler::validateContent(const String& content) const {
    // 基本 TypeScript/JavaScript 语法验证
    return !content.empty() && (
        content.find("function") != String::npos ||
        content.find("class") != String::npos ||
        content.find("interface") != String::npos ||
        content.find("const") != String::npos ||
        content.find("let") != String::npos ||
        content.find("var") != String::npos
    );
}

String TypeScriptAdvancedHandler::transformContent(const String& content, const String& targetFormat) const {
    if (targetFormat == "js") {
        return transpileToJavaScript(content);
    } else if (targetFormat == "d.ts") {
        return generateTypeDeclarations(content);
    }
    return content;
}

StringVector TypeScriptAdvancedHandler::extractDependencies(const String& content) const {
    StringVector deps;
    
    // 提取 import 语句
    std::regex importRegex(R"(import\s+.*?\s+from\s+['"]([^'"]+)['"])");
    std::sregex_iterator iter(content.begin(), content.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        deps.push_back(iter->str(1));
    }
    
    // 提取 require 语句
    std::regex requireRegex(R"(require\s*\(\s*['"]([^'"]+)['"]\s*\))");
    iter = std::sregex_iterator(content.begin(), content.end(), requireRegex);
    
    for (; iter != end; ++iter) {
        deps.push_back(iter->str(1));
    }
    
    return deps;
}

String TypeScriptAdvancedHandler::compile(const String& content, const StringUnorderedMap& options) const {
    return transpileToJavaScript(content, options);
}

bool TypeScriptAdvancedHandler::validateSemantics(const String& content) const {
    return !hasTypeErrors(content);
}

bool TypeScriptAdvancedHandler::validateSecurity(const String& content) const {
    // 安全检查
    StringVector dangerousPatterns = {"eval(", "Function(", "document.write", "innerHTML ="};
    for (const String& pattern : dangerousPatterns) {
        if (content.find(pattern) != String::npos) {
            return false;
        }
    }
    return true;
}

StringVector TypeScriptAdvancedHandler::getSemanticErrors(const String& content) const {
    StringVector errors;
    if (hasTypeErrors(content)) {
        errors.push_back("TypeScript type errors detected");
    }
    return errors;
}

StringVector TypeScriptAdvancedHandler::getSecurityWarnings(const String& content) const {
    StringVector warnings;
    if (content.find("eval(") != String::npos) {
        warnings.push_back("Use of eval() function detected - potential security risk");
    }
    if (content.find("innerHTML") != String::npos) {
        warnings.push_back("Use of innerHTML - potential XSS vulnerability");
    }
    return warnings;
}

String TypeScriptAdvancedHandler::compileIncremental(const String& content, const String& previousContent, 
                                                    const StringUnorderedMap& options) const {
    auto changedSections = getChangedSections(content, previousContent);
    if (changedSections.empty()) {
        return previousContent; // 无变化
    }
    
    // 增量编译逻辑
    return compile(content, options);
}

StringVector TypeScriptAdvancedHandler::getChangedSections(const String& content, const String& previousContent) const {
    StringVector changes;
    
    auto currentFunctions = extractFunctions(content);
    auto previousFunctions = extractFunctions(previousContent);
    
    for (const String& func : currentFunctions) {
        if (std::find(previousFunctions.begin(), previousFunctions.end(), func) == previousFunctions.end()) {
            changes.push_back("Function added: " + func);
        }
    }
    
    return changes;
}

String TypeScriptAdvancedHandler::optimize(const String& content, const StringUnorderedMap& options) const {
    String optimized = content;
    
    // 移除注释
    std::regex commentRegex(R"(//.*?$|/\*.*?\*/)");
    optimized = std::regex_replace(optimized, commentRegex, "");
    
    // 移除多余空格
    std::regex spaceRegex(R"(\s+)");
    optimized = std::regex_replace(optimized, spaceRegex, " ");
    
    return optimized;
}

String TypeScriptAdvancedHandler::minify(const String& content) const {
    String minified = optimize(content);
    
    // 移除换行
    std::regex newlineRegex(R"(\n)");
    minified = std::regex_replace(minified, newlineRegex, "");
    
    return minified;
}

String TypeScriptAdvancedHandler::beautify(const String& content) const {
    // 简化的美化实现
    String beautified = content;
    
    // 添加适当的缩进和换行
    std::regex braceRegex(R"(\{)");
    beautified = std::regex_replace(beautified, braceRegex, "{\n    ");
    
    return beautified;
}

size_t TypeScriptAdvancedHandler::estimateOutputSize(const String& content) const {
    // 估算编译后的大小
    return content.size() * 1.2; // TypeScript 编译后通常会稍微增大
}

StringVector TypeScriptAdvancedHandler::analyzeDependencyGraph(const String& content) const {
    return extractDependencies(content);
}

StringVector TypeScriptAdvancedHandler::getCircularDependencies(const String& content) const {
    // 简化的循环依赖检测
    StringVector circular;
    auto deps = extractDependencies(content);
    
    // 检查是否有依赖指向自己
    for (const String& dep : deps) {
        if (dep == "." || dep == "./" || dep.find("../") == 0) {
            circular.push_back(dep);
        }
    }
    
    return circular;
}

StringUnorderedMap TypeScriptAdvancedHandler::getDependencyVersions(const String& content) const {
    StringUnorderedMap versions;
    // 从 package.json 或其他配置中提取版本信息
    return versions;
}

bool TypeScriptAdvancedHandler::hasConflictingDependencies(const String& content) const {
    return false; // 简化实现
}

String TypeScriptAdvancedHandler::generateWrapper(const String& content, const String& wrapperType) const {
    std::ostringstream oss;
    
    if (wrapperType == "module") {
        oss << "(function(module, exports) {\n";
        oss << content << "\n";
        oss << "})(this.module || {}, this.exports || {});\n";
    } else if (wrapperType == "iife") {
        oss << "(function() {\n";
        oss << content << "\n";
        oss << "})();\n";
    } else {
        return content;
    }
    
    return oss.str();
}

String TypeScriptAdvancedHandler::generateBindings(const String& content, const String& targetLanguage) const {
    if (targetLanguage == "c++") {
        return "// C++ bindings for TypeScript code\n" + content;
    } else if (targetLanguage == "python") {
        return "# Python bindings for TypeScript code\n" + content;
    }
    return content;
}

String TypeScriptAdvancedHandler::extractInterface(const String& content) const {
    std::ostringstream oss;
    auto interfaces = extractInterfaces(content);
    
    for (const String& interface : interfaces) {
        oss << interface << "\n";
    }
    
    return oss.str();
}

String TypeScriptAdvancedHandler::generateDocumentation(const String& content) const {
    std::ostringstream oss;
    
    oss << "# TypeScript Documentation\n\n";
    
    auto functions = extractFunctions(content);
    if (!functions.empty()) {
        oss << "## Functions\n";
        for (const String& func : functions) {
            oss << "- " << func << "\n";
        }
        oss << "\n";
    }
    
    auto classes = extractClasses(content);
    if (!classes.empty()) {
        oss << "## Classes\n";
        for (const String& cls : classes) {
            oss << "- " << cls << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

String TypeScriptAdvancedHandler::generateHotReloadCode(const String& content) const {
    std::ostringstream oss;
    
    oss << "// Hot reload wrapper\n";
    oss << "if (module.hot) {\n";
    oss << "  module.hot.accept(() => {\n";
    oss << "    console.log('Module reloaded');\n";
    oss << "  });\n";
    oss << "}\n\n";
    oss << content;
    
    return oss.str();
}

StringVector TypeScriptAdvancedHandler::getWatchedFiles(const String& content) const {
    return extractDependencies(content);
}

String TypeScriptAdvancedHandler::generateSourceMap(const String& content) const {
    return "// Source map placeholder for: " + content.substr(0, 50) + "...";
}

StringUnorderedMap TypeScriptAdvancedHandler::extractMetrics(const String& content) const {
    StringUnorderedMap metrics;
    
    metrics["lines"] = std::to_string(std::count(content.begin(), content.end(), '\n'));
    metrics["functions"] = std::to_string(extractFunctions(content).size());
    metrics["classes"] = std::to_string(extractClasses(content).size());
    metrics["interfaces"] = std::to_string(extractInterfaces(content).size());
    
    return metrics;
}

size_t TypeScriptAdvancedHandler::getComplexityScore(const String& content) const {
    size_t complexity = 0;
    
    // 简化的复杂度计算
    complexity += std::count_if(content.begin(), content.end(), [](char c) { return c == '{'; });
    complexity += std::count_if(content.begin(), content.end(), [](char c) { return c == '('; });
    
    return complexity;
}

StringVector TypeScriptAdvancedHandler::getUsedFeatures(const String& content) const {
    StringVector features;
    
    if (content.find("class") != String::npos) features.push_back("classes");
    if (content.find("interface") != String::npos) features.push_back("interfaces");
    if (content.find("async") != String::npos) features.push_back("async/await");
    if (content.find("=>") != String::npos) features.push_back("arrow functions");
    
    return features;
}

StringVector TypeScriptAdvancedHandler::getSuggestions(const String& content) const {
    StringVector suggestions;
    
    if (content.find("var ") != String::npos) {
        suggestions.push_back("Consider using 'const' or 'let' instead of 'var'");
    }
    
    if (content.find("any") != String::npos) {
        suggestions.push_back("Consider using more specific types instead of 'any'");
    }
    
    return suggestions;
}

StringVector TypeScriptAdvancedHandler::getSecurityRisks(const String& content) const {
    return getSecurityWarnings(content);
}

String TypeScriptAdvancedHandler::sanitizeContent(const String& content) const {
    String sanitized = content;
    
    // 移除危险的函数调用
    sanitized = std::regex_replace(sanitized, std::regex(R"(eval\s*\([^)]*\))"), "/* eval removed */");
    
    return sanitized;
}

bool TypeScriptAdvancedHandler::isContentSafe(const String& content) const {
    return validateSecurity(content);
}

StringUnorderedMap TypeScriptAdvancedHandler::getPermissions(const String& content) const {
    StringUnorderedMap permissions;
    
    if (content.find("fetch") != String::npos) {
        permissions["network"] = "required";
    }
    
    if (content.find("localStorage") != String::npos) {
        permissions["storage"] = "required";
    }
    
    return permissions;
}

String TypeScriptAdvancedHandler::getCacheKey(const String& content, const StringUnorderedMap& options) const {
    std::hash<String> hasher;
    String optionsStr;
    for (const auto& [key, value] : options) {
        optionsStr += key + "=" + value + ";";
    }
    
    return std::to_string(hasher(content + optionsStr));
}

size_t TypeScriptAdvancedHandler::estimateCompilationTime(const String& content) const {
    // 基于内容大小估算编译时间（毫秒）
    return content.size() / 1000 + 100; // 每1000字符约1毫秒，基础100毫秒
}

StringVector TypeScriptAdvancedHandler::getOptimizationHints(const String& content) const {
    StringVector hints;
    
    if (content.size() > 10000) {
        hints.push_back("Consider splitting large files into smaller modules");
    }
    
    if (getComplexityScore(content) > 100) {
        hints.push_back("High complexity detected - consider refactoring");
    }
    
    return hints;
}

// TypeScript 特定方法实现
StringVector TypeScriptAdvancedHandler::extractTypeDefinitions(const String& content) const {
    StringVector types;
    
    std::regex typeRegex(R"(type\s+(\w+)\s*=)");
    std::sregex_iterator iter(content.begin(), content.end(), typeRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        types.push_back(iter->str(1));
    }
    
    return types;
}

StringVector TypeScriptAdvancedHandler::extractInterfaces(const String& content) const {
    StringVector interfaces;
    
    std::regex interfaceRegex(R"(interface\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), interfaceRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        interfaces.push_back(iter->str(1));
    }
    
    return interfaces;
}

StringVector TypeScriptAdvancedHandler::extractClasses(const String& content) const {
    StringVector classes;
    
    std::regex classRegex(R"(class\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), classRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        classes.push_back(iter->str(1));
    }
    
    return classes;
}

StringVector TypeScriptAdvancedHandler::extractFunctions(const String& content) const {
    StringVector functions;
    
    std::regex functionRegex(R"(function\s+(\w+)\s*\(|(\w+)\s*:\s*\([^)]*\)\s*=>|(\w+)\s*\([^)]*\)\s*\{)");
    std::sregex_iterator iter(content.begin(), content.end(), functionRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        if (!iter->str(1).empty()) {
            functions.push_back(iter->str(1));
        } else if (!iter->str(2).empty()) {
            functions.push_back(iter->str(2));
        } else if (!iter->str(3).empty()) {
            functions.push_back(iter->str(3));
        }
    }
    
    return functions;
}

StringUnorderedMap TypeScriptAdvancedHandler::extractModuleExports(const String& content) const {
    StringUnorderedMap exports;
    
    std::regex exportRegex(R"(export\s+.*?(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), exportRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        exports[iter->str(1)] = "exported";
    }
    
    return exports;
}

bool TypeScriptAdvancedHandler::hasTypeErrors(const String& content) const {
    // 简化的类型错误检测
    return content.find("// @ts-expect-error") != String::npos ||
           content.find("// @ts-ignore") != String::npos;
}

String TypeScriptAdvancedHandler::transpileToJavaScript(const String& content, const StringUnorderedMap& options) const {
    String js = content;
    
    // 移除 TypeScript 类型注解
    js = std::regex_replace(js, std::regex(R"(:\s*\w+(\[\])?(\s*\|\s*\w+)*\s*(?=[,;=)])"), "");
    js = std::regex_replace(js, std::regex(R"(interface\s+\w+\s*\{[^}]*\})"), "");
    js = std::regex_replace(js, std::regex(R"(type\s+\w+\s*=\s*[^;]+;)"), "");
    
    return js;
}

String TypeScriptAdvancedHandler::generateTypeDeclarations(const String& content) const {
    std::ostringstream oss;
    
    oss << "// Generated type declarations\n\n";
    
    auto interfaces = extractInterfaces(content);
    for (const String& interface : interfaces) {
        oss << "export interface " << interface << ";\n";
    }
    
    auto types = extractTypeDefinitions(content);
    for (const String& type : types) {
        oss << "export type " << type << ";\n";
    }
    
    return oss.str();
}

} // namespace CHTL