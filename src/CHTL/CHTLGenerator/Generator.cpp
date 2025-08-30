#include "Generator.h"
#include "../../Util/StringUtils.h"
#include "../../CSS/CSSCompiler.h"
#include "../../JS/JSCompiler.h"
#include "../CMODSystem/CJMODGenerator.h"
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
        return result.javascript;
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
        // 🔄 正确的编译流程：
        // 1. CHTL编译器 → 处理CHTL语法 (模板、自定义、原始嵌入等)
        // 2. CHTL JS编译器 → 处理CHTL JS语法 (listen, animate, vir等)
        // 3. 代码合并阶段 → 将所有CSS和JS代码合并成完整片段
        // 4. CSS编译器 → 接收合并后的完整CSS代码片段
        // 5. JS编译器 → 接收合并后的完整JavaScript代码片段
        
        // 阶段1: CHTL编译器 - 处理CHTL语法
        output.html = generateHTML(documentNode);
        
        // 阶段2: CHTL JS编译器 - 处理CHTL JS语法并转换
        processCHTLJSSyntax(documentNode);
        
        // 阶段3: 代码收集阶段 - 收集所有CSS和JS代码
        std::ostringstream cssCollector;
        std::ostringstream jsCollector;
        collectCSSContent(documentNode, cssCollector);
        collectJavaScriptContent(documentNode, jsCollector);
        String mergedCSSCode = cssCollector.str();
        String mergedJSCode = jsCollector.str();
        
        // 阶段4: CSS编译器 - 接收合并后的完整CSS代码片段
        if (!mergedCSSCode.empty()) {
            CSS::CSSCompilerConfig cssConfig;
            cssConfig.minifyOutput = config.optimizeCSS;
            cssConfig.preserveComments = config.includeCSSComments;
            cssConfig.enableAutoprefixer = config.addVendorPrefixes;
            cssConfig.enableNesting = true;
            cssConfig.enableVariables = true;
            
            CSS::CSSCompiler cssCompiler(cssConfig);
            auto cssResult = cssCompiler.compile(mergedCSSCode);
            
            if (cssResult.success) {
                output.css = cssResult.css;
            } else {
                output.css = mergedCSSCode;
                output.warnings.push_back("CSS编译失败，使用合并后的CSS");
                for (const auto& error : cssResult.errors) {
                    output.warnings.push_back("CSS错误: " + error);
                }
            }
        }
        
        // 阶段5: JS编译器 - 接收合并后的完整JavaScript代码片段
        if (!mergedJSCode.empty()) {
            JS::JSCompilerConfig jsConfig;
            jsConfig.minifyOutput = config.optimizeJS;
            jsConfig.preserveComments = config.includeJSComments;
            jsConfig.enableModules = config.useESModules;
            jsConfig.strictMode = config.addStrictMode;
            jsConfig.enableAsync = true;
            
            JS::JSCompiler jsCompiler(jsConfig);
            auto jsResult = jsCompiler.compile(mergedJSCode);
            
            if (jsResult.success) {
                output.javascript = jsResult.javascript;
            } else {
                output.javascript = mergedJSCode;
                output.warnings.push_back("JS编译失败，使用合并后的JavaScript");
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
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*\{\s*)"), " { ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*\}\s*)"), " } ");
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
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*\{\s*)"), " { ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*\}\s*)"), " } ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*\(\s*)"), "(");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*\)\s*)"), ") ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s*,\s*)"), ", ");
    
    // 移除开头和结尾的空白
    cleaned.erase(0, cleaned.find_first_not_of(" \t\n\r"));
    cleaned.erase(cleaned.find_last_not_of(" \t\n\r") + 1);
    
    return cleaned;
}

// ========== 正确编译流程的新方法实现 ==========

void Generator::processCHTLJSSyntax(const BaseNode* node) {
    if (!node) return;
    
    // 处理CHTL JS增强语法节点
    switch (node->getType()) {
        case NodeType::CHTLJS_FUNCTION:
        case NodeType::CHTLJS_LISTEN:
        case NodeType::CHTLJS_DELEGATE:
        case NodeType::CHTLJS_ANIMATE:
        case NodeType::CHTLJS_VIR_OBJECT:
        case NodeType::CHTLJS_MODULE: {
            // 使用CJMOD生成器转换CHTL JS语法为标准JavaScript
            String convertedJS = convertCHTLJSToJS(node);
            
            // 将转换后的代码存储到节点中，以便后续合并
            // 这里可以将转换结果临时存储或标记
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                processCHTLJSSyntax(child.get());
            }
            break;
    }
}

String Generator::mergeAllCSSCode(const BaseNode* node) {
    std::ostringstream mergedCSS;
    
    // 合并所有CSS代码源：
    // 1. 直接的style节点
    // 2. 模板样式 (已处理)
    // 3. 自定义样式 (已处理)
    // 4. 原始嵌入的@Style
    
    std::function<void(const BaseNode*)> mergeCSSRecursive = [&](const BaseNode* current) {
        if (!current) return;
        
        switch (current->getType()) {
            case NodeType::STYLE: {
                String cssContent = current->getValue();
                if (!cssContent.empty()) {
                    // 修复CSS语法问题
                    cssContent = cleanCSSContent(cssContent);
                    
                    // 确保CSS规则完整
                    size_t openBraces = std::count(cssContent.begin(), cssContent.end(), '{');
                    size_t closeBraces = std::count(cssContent.begin(), cssContent.end(), '}');
                    while (closeBraces < openBraces) {
                        cssContent += " }";
                        closeBraces++;
                    }
                    
                    mergedCSS << cssContent << config.newlineString;
                }
                break;
            }
            
            case NodeType::TEMPLATE_STYLE:
            case NodeType::CUSTOM_STYLE: {
                // 已处理的模板和自定义样式
                String styleContent = current->getValue();
                if (!styleContent.empty()) {
                    mergedCSS << "/* Template/Custom Style */" << config.newlineString;
                    mergedCSS << styleContent << config.newlineString;
                }
                break;
            }
            
            case NodeType::ORIGIN_STYLE: {
                // @Style原始嵌入
                String originCSS = current->getValue();
                if (!originCSS.empty()) {
                    mergedCSS << "/* Origin Style */" << config.newlineString;
                    mergedCSS << originCSS << config.newlineString;
                }
                break;
            }
            
            default:
                // 递归处理子节点
                for (const auto& child : current->getChildren()) {
                    mergeCSSRecursive(child.get());
                }
                break;
        }
    };
    
    mergeCSSRecursive(node);
    
    return mergedCSS.str();
}

String Generator::mergeAllJavaScriptCode(const BaseNode* node) {
    std::ostringstream mergedJS;
    
    // 合并所有JavaScript代码源：
    // 1. 直接的script节点
    // 2. CHTL JS转换后的代码
    // 3. 原始嵌入的@JavaScript
    
    std::function<void(const BaseNode*)> mergeJSRecursive = [&](const BaseNode* current) {
        if (!current) return;
        
        switch (current->getType()) {
            case NodeType::SCRIPT: {
                String jsContent = current->getValue();
                if (!jsContent.empty()) {
                    // 修复JavaScript语法问题
                    jsContent = cleanJSContent(jsContent);
                    
                    mergedJS << jsContent << config.newlineString;
                }
                break;
            }
            
            case NodeType::CHTLJS_FUNCTION:
            case NodeType::CHTLJS_LISTEN:
            case NodeType::CHTLJS_DELEGATE:
            case NodeType::CHTLJS_ANIMATE:
            case NodeType::CHTLJS_VIR_OBJECT:
            case NodeType::CHTLJS_MODULE: {
                // CHTL JS转换后的代码
                String convertedJS = convertCHTLJSToJS(current);
                if (!convertedJS.empty()) {
                    mergedJS << "/* CHTL JS Converted */" << config.newlineString;
                    mergedJS << convertedJS << config.newlineString;
                }
                break;
            }
            
            case NodeType::ORIGIN_JAVASCRIPT: {
                // @JavaScript原始嵌入
                String originJS = current->getValue();
                if (!originJS.empty()) {
                    mergedJS << "/* Origin JavaScript */" << config.newlineString;
                    mergedJS << originJS << config.newlineString;
                }
                break;
            }
            
            default:
                // 递归处理子节点
                for (const auto& child : current->getChildren()) {
                    mergeJSRecursive(child.get());
                }
                break;
        }
    };
    
    mergeJSRecursive(node);
    
    return mergedJS.str();
}

} // namespace CHTL