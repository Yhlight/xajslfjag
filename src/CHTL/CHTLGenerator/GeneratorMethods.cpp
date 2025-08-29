#include "Generator.h"
#include "../../Util/StringUtils.h"

namespace CHTL {

String Generator::generateElement(const BaseNode* node) {
    // 基础元素生成实现
    if (!node) return "";
    return "<" + node->getValue() + ">" + node->getValue() + "</" + node->getValue() + ">";
}

String Generator::generateText(const BaseNode* node) {
    // 文本生成实现
    if (!node) return "";
    return node->getValue();
}

String Generator::generateStyle(const BaseNode* node) {
    // 样式生成实现
    if (!node) return "";
    return "<style>" + node->getValue() + "</style>";
}

String Generator::generateScript(const BaseNode* node) {
    // 脚本生成实现
    if (!node) return "";
    return "<script>" + node->getValue() + "</script>";
}

String Generator::generateTemplate(const BaseNode* node) {
    // 模板生成实现
    if (!node) return "";
    return "<!-- Template: " + node->getValue() + " -->";
}

String Generator::generateCustom(const BaseNode* node) {
    // 自定义生成实现
    if (!node) return "";
    return "<!-- Custom: " + node->getValue() + " -->";
}

String Generator::generateOrigin(const BaseNode* node) {
    // 原始内容生成实现
    if (!node) return "";
    return node->getValue();
}

String Generator::instantiateTemplate(const BaseNode* templateNode, const StringUnorderedMap& parameters) {
    // 模板实例化（占位实现）
    if (!templateNode) return "";
    return templateNode->getValue();
}

String Generator::instantiateCustom(const BaseNode* customNode, const StringUnorderedMap& parameters) {
    // 自定义实例化（占位实现）
    if (!customNode) return "";
    return customNode->getValue();
}

void Generator::optimizeHTML(String& html) {
    // HTML优化（修改为void，直接修改参数）
    html = Util::StringUtils::trim(html);
}

void Generator::optimizeCSS(String& css) {
    // CSS优化（修改为void，直接修改参数）
    css = Util::StringUtils::trim(css);
}

} // namespace CHTL