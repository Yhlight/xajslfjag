#include "Generator.h"
#include "../../Util/StringUtils.h"
#include <chrono>
#include <sstream>
#include "../CHTLNode/BaseNode.h"
#include <sstream>
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
    return "/* Generated CSS */\nbody { font-family: Arial; }\n";
}

String Generator::generateJavaScript(const BaseNode* ast) {
    if (!ast) return "";
    
    std::ostringstream js;
    
    if (config.addStrictMode) {
        js << "'use strict';" << config.newlineString;
    }
    
    // 递归生成JavaScript内容
    generateJavaScriptRecursive(ast, js, 0);
    
    return js.str();
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
    // TODO: 实现CHTL JS到JS的转换
    // 这里需要调用CJMOD生成器
    return "// CHTL JS converted: " + node->getValue();
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
    return generate(documentNode);
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

} // namespace CHTL