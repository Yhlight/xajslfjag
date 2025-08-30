#include "Generator.h"
#include "../../Util/StringUtils.h"
#include "../../CSS/CSSCompiler.h"
#include "../../JS/JSCompiler.h"
#include "../../CMODSystem/CJMODGenerator.h"
#include <chrono>
#include <sstream>
#include <regex>
#include "../CHTLNode/BaseNode.h"
#include <fstream>

namespace CHTL {

Generator::Generator(const GeneratorConfig& config) 
    : config(config) {
}

String Generator::generateHTML(const BaseNode* ast) {
    if (!ast) return "";
    
    std::ostringstream html;
    
    // 生成HTML文档结构
    if (config.includeDoctype) {
        html << "<!DOCTYPE html>" << config.newlineString;
    }
    
    // 递归生成HTML内容
    generateHTMLRecursive(ast, html, 0);
    
    return html.str();
}

void Generator::generateHTMLRecursive(const BaseNode* node, std::ostringstream& output, int depth) {
    if (!node) return;
    
    String indent = getIndent(depth);
    
    switch (node->getType()) {
        case NodeType::DOCUMENT:
            // 文档节点：遍历子节点
            for (const auto& child : node->getChildren()) {
                generateHTMLRecursive(child.get(), output, depth);
            }
            break;
            
        case NodeType::ELEMENT: {
            String tagName = node->getValue();
            if (config.lowercaseTagNames) {
                tagName = Util::StringUtils::toLowerCase(tagName);
            }
            
            // 开始标签
            output << indent << "<" << tagName;
            
            // 生成属性
            auto attributes = node->getAttributeNames();
            for (const auto& attrName : attributes) {
                String attrValue = node->getAttribute(attrName);
                String attrNameFinal = config.lowercaseAttributeNames ? 
                    Util::StringUtils::toLowerCase(attrName) : attrName;
                output << " " << attrNameFinal << "=\"" << escapeHTML(attrValue) << "\"";
            }
            
            // 检查是否为自闭合标签
            bool isVoidElement = isVoidHTMLElement(tagName);
            if (isVoidElement && config.selfCloseVoidElements) {
                output << " />";
            } else {
                output << ">";
                
                // 生成子节点
                bool hasBlockChildren = hasBlockLevelChildren(node);
                if (hasBlockChildren && config.formatHTML) {
                    output << config.newlineString;
                }
                
                for (const auto& child : node->getChildren()) {
                    generateHTMLRecursive(child.get(), output, hasBlockChildren ? depth + 1 : 0);
                }
                
                // 结束标签
                if (hasBlockChildren && config.formatHTML) {
                    output << indent;
                }
                output << "</" << tagName << ">";
            }
            
            if (config.formatHTML) {
                output << config.newlineString;
            }
            break;
        }
        
        case NodeType::TEXT: {
            String textContent = node->getValue();
            if (!textContent.empty()) {
                output << indent << escapeHTML(textContent);
                if (config.formatHTML) {
                    output << config.newlineString;
                }
            }
            break;
        }
        
        case NodeType::COMMENT: {
            if (config.includeComments) {
                String commentContent = node->getValue();
                output << indent << "<!-- " << commentContent << " -->";
                if (config.formatHTML) {
                    output << config.newlineString;
                }
            }
            break;
        }
        
        case NodeType::STYLE: {
            // 样式节点作为<style>标签嵌入
            output << indent << "<style>";
            if (config.formatHTML) {
                output << config.newlineString;
            }
            
            String cssContent = generateCSS(node);
            if (!cssContent.empty()) {
                output << cssContent;
            }
            
            output << indent << "</style>";
            if (config.formatHTML) {
                output << config.newlineString;
            }
            break;
        }
        
        case NodeType::SCRIPT: {
            // 脚本节点作为<script>标签嵌入
            output << indent << "<script>";
            if (config.formatHTML) {
                output << config.newlineString;
            }
            
            String jsContent = generateJavaScript(node);
            if (!jsContent.empty()) {
                output << jsContent;
            }
            
            output << indent << "</script>";
            if (config.formatHTML) {
                output << config.newlineString;
            }
            break;
        }
        
        case NodeType::TEMPLATE:
        case NodeType::CUSTOM: {
            // 模板和自定义节点：实例化后生成
            String instantiated = (node->getType() == NodeType::TEMPLATE) ? 
                instantiateTemplate(node) : instantiateCustom(node);
            
            if (!instantiated.empty()) {
                output << instantiated;
            }
            break;
        }
        
        case NodeType::ORIGIN: {
            // 原始嵌入：直接输出原始内容
            String originContent = generateOrigin(node);
            if (!originContent.empty()) {
                output << originContent;
            }
            break;
        }
        
        default:
            // 其他节点类型：递归处理子节点
            for (const auto& child : node->getChildren()) {
                generateHTMLRecursive(child.get(), output, depth);
            }
            break;
    }
}

String Generator::generateCSS(const BaseNode* ast) {
    if (!ast) return "";
    
    std::ostringstream cssOutput;
    
    // 收集所有CSS内容
    collectCSSContent(ast, cssOutput);
    
    String cssSource = cssOutput.str();
    if (cssSource.empty()) {
        return "";
    }
    
    // 使用CSS编译器处理完整的CSS代码片段
    CSS::CSSCompilerConfig cssConfig;
    cssConfig.minifyOutput = config.optimizeCSS;
    cssConfig.preserveComments = config.includeCSSComments;
    cssConfig.enableAutoprefixer = config.addVendorPrefixes;
    
    CSS::CSSCompiler cssCompiler(cssConfig);
    auto result = cssCompiler.compile(cssSource);
    
    if (result.success) {
        return result.css;
    } else {
        // 如果编译失败，返回原始CSS
        return cssSource;
    }
}

String Generator::generateJavaScript(const BaseNode* ast) {
    if (!ast) return "";
    
    std::ostringstream jsOutput;
    
    if (config.addStrictMode) {
        jsOutput << "'use strict';" << config.newlineString;
    }
    
    // 收集所有JavaScript内容
    collectJavaScriptContent(ast, jsOutput);
    
    String jsSource = jsOutput.str();
    if (jsSource.empty()) {
        return "";
    }
    
    // 使用JS编译器处理完整的JavaScript代码片段
    JS::JSCompilerConfig jsConfig;
    jsConfig.minifyOutput = config.optimizeJS;
    jsConfig.preserveComments = config.includeJSComments;
    jsConfig.enableModules = config.useESModules;
    jsConfig.strictMode = config.addStrictMode;
    
    JS::JSCompiler jsCompiler(jsConfig);
    auto result = jsCompiler.compile(jsSource);
    
    if (result.success) {
        return result.javascript;
    } else {
        // 如果编译失败，返回原始JavaScript
        return jsSource;
    }
}

void Generator::generateJavaScriptRecursive(const BaseNode* node, std::ostringstream& output, int depth) {
    if (!node) return;
    
    switch (node->getType()) {
        case NodeType::SCRIPT: {
            // 脚本节点：输出内容
            String content = node->getValue();
            if (!content.empty()) {
                output << content;
                if (config.formatJS) {
                    output << config.newlineString;
                }
            }
            break;
        }
        
        case NodeType::CHTLJS_FUNCTION:
        case NodeType::CHTLJS_LISTEN:
        case NodeType::CHTLJS_DELEGATE:
        case NodeType::CHTLJS_ANIMATE:
        case NodeType::CHTLJS_VIR_OBJECT:
        case NodeType::CHTLJS_MODULE: {
            // CHTL JS增强语法：转换为标准JavaScript
            String enhancedJS = convertCHTLJSToJS(node);
            if (!enhancedJS.empty()) {
                output << enhancedJS;
                if (config.formatJS) {
                    output << config.newlineString;
                }
            }
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                generateJavaScriptRecursive(child.get(), output, depth);
            }
            break;
    }
}

String Generator::convertCHTLJSToJS(const BaseNode* node) {
    // 使用CJMOD生成器处理CHTL JS语法
    CJMOD::CJMODGenerator cjmodGenerator;
    
    String nodeValue = node->getValue();
    auto result = cjmodGenerator.generateFromSyntax(nodeValue);
    
    if (result.success) {
        return result.generatedCode;
    } else {
        // 如果CJMOD生成失败，返回注释形式
        return "// CHTL JS (generation failed): " + nodeValue;
    }
}

void Generator::collectCSSContent(const BaseNode* node, std::ostringstream& output) {
    if (!node) return;
    
    switch (node->getType()) {
        case NodeType::STYLE: {
            // 样式节点：直接获取完整CSS内容
            String cssContent = node->getValue();
            if (!cssContent.empty()) {
                // 清理CSS内容，移除多余空格和换行
                cssContent = cleanCSSContent(cssContent);
                output << cssContent << config.newlineString;
            }
            break;
        }
        
        case NodeType::TEMPLATE_STYLE:
        case NodeType::CUSTOM_STYLE: {
            // 模板和自定义样式：收集内容
            String content = node->getValue();
            if (!content.empty()) {
                output << "/* Template/Custom style */" << config.newlineString;
                output << content << config.newlineString;
            }
            break;
        }
        
        case NodeType::ORIGIN_STYLE: {
            // @Style原始嵌入：收集CSS内容
            String content = node->getValue();
            if (!content.empty()) {
                output << "/* Origin Style */" << config.newlineString;
                output << content << config.newlineString;
            }
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                collectCSSContent(child.get(), output);
            }
            break;
    }
}

void Generator::collectJavaScriptContent(const BaseNode* node, std::ostringstream& output) {
    if (!node) return;
    
    switch (node->getType()) {
        case NodeType::SCRIPT: {
            // 脚本节点：直接获取完整JavaScript内容
            String jsContent = node->getValue();
            if (!jsContent.empty()) {
                // 清理JavaScript内容
                jsContent = cleanJSContent(jsContent);
                output << jsContent << config.newlineString;
            }
            break;
        }
        
        case NodeType::CHTLJS_FUNCTION:
        case NodeType::CHTLJS_LISTEN:
        case NodeType::CHTLJS_DELEGATE:
        case NodeType::CHTLJS_ANIMATE:
        case NodeType::CHTLJS_VIR_OBJECT:
        case NodeType::CHTLJS_MODULE: {
            // CHTL JS增强语法：转换为JavaScript
            output << "/* CHTL JS converted */" << config.newlineString;
            String convertedJS = convertCHTLJSToJS(node);
            if (!convertedJS.empty()) {
                output << convertedJS << config.newlineString;
            }
            break;
        }
        
        case NodeType::ORIGIN_JAVASCRIPT: {
            // @JavaScript原始嵌入：收集JavaScript内容
            String content = node->getValue();
            if (!content.empty()) {
                output << "/* Origin JavaScript */" << config.newlineString;
                output << content << config.newlineString;
            }
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                collectJavaScriptContent(child.get(), output);
            }
            break;
    }
}

GenerationOutput Generator::generate(const BaseNode* ast) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    GenerationOutput output;
    
    try {
        if (!ast) {
            output.errors.push_back("AST根节点为空");
            return output;
        }
        
        // 初始化生成器状态
        resetState();
        
        // 主生成流程
        switch (ast->getType()) {
            case NodeType::DOCUMENT:
                output = generateDocument(ast);
                break;
            case NodeType::ELEMENT:
                output.html = generateElement(ast);
                output.success = true;
                break;
            case NodeType::TEXT:
                output.html = generateText(ast);
                output.success = true;
                break;
            case NodeType::STYLE:
                output.css = generateStyle(ast);
                output.success = true;
                break;
            case NodeType::SCRIPT:
                output.javascript = generateScript(ast);
                output.success = true;
                break;
            case NodeType::TEMPLATE:
                output.html = generateTemplate(ast);
                output.success = true;
                break;
            case NodeType::CUSTOM:
                output.html = generateCustom(ast);
                output.success = true;
                break;
            case NodeType::ORIGIN:
                output.html = generateOrigin(ast);
                output.success = true;
                break;
            default:
                output.html = generateHTML(ast);
                output.css = generateCSS(ast);
                output.javascript = generateJavaScript(ast);
                output.success = true;
                break;
        }
        
        // 后处理
        if (output.success) {
            postProcessOutput(output);
        }
        
    } catch (const std::exception& e) {
        output.errors.push_back("生成过程中发生异常: " + String(e.what()));
        output.success = false;
    }
    
    // 计算生成时间
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    output.generationTime = duration.count() / 1000.0; // 转换为毫秒
    
    // 计算输出大小
    output.outputSize = output.html.size() + output.css.size() + output.javascript.size();
    
    return output;
}

GenerationOutput Generator::generateDocument(const BaseNode* documentNode) {
    GenerationOutput output;
    
    try {
        // 🔄 正确的生成流程：
        // 1. 首先处理CHTL语法（模板、自定义等）并生成HTML
        // 2. 然后收集所有CSS和JS内容
        // 3. 最后调用CSS和JS编译器处理完整代码片段
        
        // 第一阶段：生成HTML结构
        output.html = generateHTML(documentNode);
        
        // 第二阶段：收集完整的CSS代码片段
        std::ostringstream cssCollector;
        collectCSSContent(documentNode, cssCollector);
        String completeCSSCode = cssCollector.str();
        
        // 第三阶段：收集完整的JavaScript代码片段
        std::ostringstream jsCollector;
        collectJavaScriptContent(documentNode, jsCollector);
        String completeJSCode = jsCollector.str();
        
        // 第四阶段：如果有CSS内容，使用ANTLR CSS编译器处理完整片段
        if (!completeCSSCode.empty()) {
            CSS::CSSCompilerConfig cssConfig;
            cssConfig.minifyOutput = config.optimizeCSS;
            cssConfig.preserveComments = config.includeCSSComments;
            cssConfig.enableAutoprefixer = config.addVendorPrefixes;
            cssConfig.enableNesting = true;
            cssConfig.enableVariables = true;
            
            CSS::CSSCompiler cssCompiler(cssConfig);
            auto cssResult = cssCompiler.compile(completeCSSCode);
            
            if (cssResult.success) {
                output.css = cssResult.css;
            } else {
                output.css = completeCSSCode;
                output.warnings.push_back("CSS编译失败，使用原始CSS");
                for (const auto& error : cssResult.errors) {
                    output.warnings.push_back("CSS错误: " + error);
                }
            }
        }
        
        // 第五阶段：如果有JavaScript内容，使用ANTLR JS编译器处理完整片段
        if (!completeJSCode.empty()) {
            JS::JSCompilerConfig jsConfig;
            jsConfig.minifyOutput = config.optimizeJS;
            jsConfig.preserveComments = config.includeJSComments;
            jsConfig.enableModules = config.useESModules;
            jsConfig.strictMode = config.addStrictMode;
            jsConfig.enableAsync = true;
            
            JS::JSCompiler jsCompiler(jsConfig);
            auto jsResult = jsCompiler.compile(completeJSCode);
            
            if (jsResult.success) {
                output.javascript = jsResult.javascript;
            } else {
                output.javascript = completeJSCode;
                output.warnings.push_back("JS编译失败，使用原始JavaScript");
                for (const auto& error : jsResult.errors) {
                    output.warnings.push_back("JS错误: " + error);
                }
            }
        }
        
        output.success = true;
        
    } catch (const std::exception& e) {
        output.errors.push_back("文档生成异常: " + String(e.what()));
        output.success = false;
    }
    
    return output;
}

GenerationOutput Generator::generateFragment(const BaseNode* fragmentNode) {
    return generate(fragmentNode);
}

// 批量生成方法（暂不实现）
// 文件写入方法（暂不实现）
// 统计方法（暂不实现）

// 辅助方法实现
void Generator::resetState() {
    // 重置生成器状态
    errors.clear();
    warnings.clear();
}

void Generator::postProcessOutput(GenerationOutput& output) {
    // 后处理生成的输出
    if (config.minifyOutput) {
        output.html = minifyHTML(output.html);
        output.css = minifyCSS(output.css);
        output.javascript = minifyJS(output.javascript);
    }
    
    if (config.enableOptimizations) {
        optimizeHTML(output.html);
        optimizeCSS(output.css);
        optimizeJS(output.javascript);
    }
}

String Generator::getIndent(int depth) const {
    String indent;
    for (int i = 0; i < depth; ++i) {
        indent += config.indentString;
    }
    return indent;
}

String Generator::escapeHTML(const String& text) const {
    String escaped = text;
    escaped = Util::StringUtils::replace_all(escaped, "&", "&amp;");
    escaped = Util::StringUtils::replace_all(escaped, "<", "&lt;");
    escaped = Util::StringUtils::replace_all(escaped, ">", "&gt;");
    escaped = Util::StringUtils::replace_all(escaped, "\"", "&quot;");
    escaped = Util::StringUtils::replace_all(escaped, "'", "&#39;");
    return escaped;
}

bool Generator::isVoidHTMLElement(const String& tagName) const {
    static const StringVector voidElements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    String lowerTag = Util::StringUtils::toLowerCase(tagName);
    return std::find(voidElements.begin(), voidElements.end(), lowerTag) != voidElements.end();
}

bool Generator::hasBlockLevelChildren(const BaseNode* node) const {
    if (!node) return false;
    
    for (const auto& child : node->getChildren()) {
        NodeType type = child->getType();
        if (type == NodeType::ELEMENT || type == NodeType::STYLE || 
            type == NodeType::SCRIPT || type == NodeType::TEMPLATE ||
            type == NodeType::CUSTOM) {
            return true;
        }
    }
    return false;
}

String Generator::minifyHTML(const String& html) const {
    // 简单的HTML压缩
    String minified = html;
    minified = Util::StringUtils::replace_all(minified, "\n", "");
    minified = Util::StringUtils::replace_all(minified, "\t", "");
    minified = Util::StringUtils::replace_all(minified, "  ", " ");
    return Util::StringUtils::trim(minified);
}

String Generator::minifyCSS(const String& css) const {
    // 简单的CSS压缩
    String minified = css;
    minified = Util::StringUtils::replace_all(minified, "\n", "");
    minified = Util::StringUtils::replace_all(minified, "\t", "");
    minified = Util::StringUtils::replace_all(minified, "  ", " ");
    minified = Util::StringUtils::replace_all(minified, "; ", ";");
    minified = Util::StringUtils::replace_all(minified, "{ ", "{");
    minified = Util::StringUtils::replace_all(minified, " }", "}");
    return Util::StringUtils::trim(minified);
}

String Generator::minifyJS(const String& js) const {
    // 简单的JS压缩
    String minified = js;
    minified = Util::StringUtils::replace_all(minified, "\n", "");
    minified = Util::StringUtils::replace_all(minified, "\t", "");
    minified = Util::StringUtils::replace_all(minified, "  ", " ");
    return Util::StringUtils::trim(minified);
}

void Generator::optimizeHTML(String& html) const {
    // HTML优化（移除无用属性等）
    html = Util::StringUtils::trim(html);
}

void Generator::optimizeCSS(String& css) const {
    // CSS优化（合并规则等）
    css = Util::StringUtils::trim(css);
}

void Generator::optimizeJS(const String& js) const {
    // JS优化（变量名压缩等）
    // 当前为占位实现
}

String Generator::cleanCSSContent(const String& css) {
    String cleaned = css;
    
    // 移除多余的空格（但保留必要的空格）
    std::regex multipleSpaces(R"(\s+)");
    cleaned = std::regex_replace(cleaned, multipleSpaces, " ");
    
    // 清理分号和大括号周围的空格
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*;\s*)"), "; ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*{\s*)"), " { ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*}\s*)"), " } ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*:\s*)"), ": ");
    
    // 移除开头和结尾的空白
    cleaned.erase(0, cleaned.find_first_not_of(" \t\n\r"));
    cleaned.erase(cleaned.find_last_not_of(" \t\n\r") + 1);
    
    return cleaned;
}

String Generator::cleanJSContent(const String& js) {
    String cleaned = js;
    
    // 移除多余的空格（但保留必要的空格）
    std::regex multipleSpaces(R"(\s+)");
    cleaned = std::regex_replace(cleaned, multipleSpaces, " ");
    
    // 清理常见的JavaScript语法周围的空格
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*;\s*)"), "; ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*{\s*)"), " { ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*}\s*)"), " } ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*\(\s*)"), "(");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*\)\s*)"), ") ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*,\s*)"), ", ");
    
    // 移除开头和结尾的空白
    cleaned.erase(0, cleaned.find_first_not_of(" \t\n\r"));
    cleaned.erase(cleaned.find_last_not_of(" \t\n\r") + 1);
    
    return cleaned;
}

} // namespace CHTL