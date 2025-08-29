#include "CJMODGenerator.h"
#include "../Util/StringUtils.h"
#include <iostream>
#include <algorithm>
#include <regex>

namespace CJMOD {

// CJMODGenerator实现
CJMODGenerator::CJMODGenerator(const GeneratorConfig& cfg) : config(cfg) {
    // 初始化模板映射
    templateMap["listen"] = R"(
function ${functionName}(${params}) {
    ${body}
}
)";
    
    templateMap["delegate"] = R"(
function ${functionName}(${params}) {
    ${parentElement}.addEventListener('${eventType}', function(event) {
        if (event.target.matches('${childSelector}')) {
            ${handler}(event);
        }
    });
}
)";
    
    templateMap["animate"] = R"(
function ${functionName}(${params}) {
    return new Promise((resolve) => {
        ${target}.animate(${keyframes}, {
            duration: ${duration},
            easing: '${easing}',
            fill: 'forwards'
        }).addEventListener('finish', resolve);
    });
}
)";
}

GenerationResult CJMODGenerator::generate(const Arg& args, const String& keyword) {
    GenerationResult result;
    
    try {
        String functionName = generateUniqueFunctionName(keyword);
        String paramList = generateParameterList(args);
        String functionBody = generateFunctionBody(args, keyword);
        
        // 应用模板
        StringUnorderedMap variables = {
            {"functionName", functionName},
            {"params", paramList},
            {"body", functionBody},
            {"keyword", keyword}
        };
        
        String template_ = templateMap.count(keyword) ? templateMap[keyword] : templateMap["listen"];
        String generatedCode = applyTemplate(keyword, variables);
        
        if (config.enableOptimization) {
            generatedCode = optimizeCode(generatedCode);
        }
        
        result.generatedCode = generatedCode;
        result.success = true;
        result.metadata["functionName"] = functionName;
        result.metadata["keyword"] = keyword;
        
        generatedFunctions.push_back(functionName);
        
    } catch (const std::exception& e) {
        result.addError("代码生成失败: " + String(e.what()));
    }
    
    return result;
}

GenerationResult CJMODGenerator::generateFromSyntax(const String& syntaxString) {
    GenerationResult result;
    
    try {
        // 分析语法字符串
        Arg args = Syntax::analyze(syntaxString);
        
        // 提取关键字
        String keyword = "custom";
        for (const auto& atom : args.atoms) {
            if (!atom.placeholder.empty() && atom.placeholder != "$" && atom.placeholder != "$?" && atom.placeholder != "$!") {
                keyword = atom.placeholder;
                break;
            }
        }
        
        result = generate(args, keyword);
        
    } catch (const std::exception& e) {
        result.addError("语法解析失败: " + String(e.what()));
    }
    
    return result;
}

String CJMODGenerator::generateFunction(const Arg& args, const String& keyword) {
    String functionName = generateUniqueFunctionName(keyword);
    String params = generateParameterList(args);
    String body = generateFunctionBody(args, keyword);
    
    String function = "function " + functionName + "(" + params + ") {\n";
    function += body + "\n";
    function += "}";
    
    return function;
}

String CJMODGenerator::generateParameterList(const Arg& args) {
    StringVector params;
    
    for (const auto& atom : args.atoms) {
        if (atom.placeholder.find("$") != String::npos) {
            String paramName = "param" + std::to_string(params.size());
            if (atom.placeholder == "$!") {
                // 必需参数
                params.push_back(paramName);
            } else if (atom.placeholder == "$?") {
                // 可选参数，设置默认值
                params.push_back(paramName + " = null");
            } else if (atom.placeholder == "$_") {
                // 无序参数
                params.push_back("..." + paramName);
            } else {
                // 普通参数
                params.push_back(paramName);
            }
        }
    }
    
    return CHTL::Util::StringUtils::join(params, ", ");
}

String CJMODGenerator::generateFunctionBody(const Arg& args, const String& keyword) {
    String body;
    
    if (keyword == "listen") {
        body = "    // 事件监听逻辑\n";
        body += "    for (const [event, handler] of Object.entries(arguments[0] || {})) {\n";
        body += "        this.addEventListener(event, handler);\n";
        body += "    }";
    } else if (keyword == "delegate") {
        body = "    // 事件委托逻辑\n";
        body += "    const target = arguments[0];\n";
        body += "    const events = arguments[1] || {};\n";
        body += "    for (const [event, handler] of Object.entries(events)) {\n";
        body += "        this.addEventListener(event, (e) => {\n";
        body += "            if (e.target.matches(target)) handler(e);\n";
        body += "        });\n";
        body += "    }";
    } else if (keyword == "animate") {
        body = "    // 动画逻辑\n";
        body += "    const options = arguments[0] || {};\n";
        body += "    return this.animate(options.keyframes, {\n";
        body += "        duration: options.duration || 1000,\n";
        body += "        easing: options.easing || 'ease'\n";
        body += "    });";
    } else {
        body = "    // 自定义逻辑\n";
        body += "    console.log('CJMOD function:', '" + keyword + "', arguments);";
    }
    
    return body;
}

String CJMODGenerator::generateUniqueFunctionName(const String& keyword) {
    String baseName = sanitizeIdentifier(keyword);
    String functionName = "cjmod_" + baseName + "_" + std::to_string(functionCounter++);
    return functionName;
}

String CJMODGenerator::sanitizeIdentifier(const String& identifier) {
    String sanitized = identifier;
    
    // 替换非法字符
    std::replace_if(sanitized.begin(), sanitized.end(), 
        [](char c) { return !std::isalnum(c) && c != '_'; }, '_');
    
    // 确保以字母或下划线开头
    if (!sanitized.empty() && std::isdigit(sanitized[0])) {
        sanitized = "_" + sanitized;
    }
    
    return sanitized;
}

String CJMODGenerator::applyTemplate(const String& templateName, const StringUnorderedMap& variables) {
    String template_ = templateMap.count(templateName) ? templateMap[templateName] : "";
    
    if (template_.empty()) {
        return "";
    }
    
    String result = template_;
    
    // 替换变量
    for (const auto& var : variables) {
        String placeholder = "${" + var.first + "}";
        result = CHTL::Util::StringUtils::replace_all(result, placeholder, var.second);
    }
    
    return result;
}

String CJMODGenerator::optimizeCode(const String& code) {
    String optimized = code;
    
    if (config.enableOptimization) {
        optimized = removeRedundantCode(optimized);
        optimized = combineVariables(optimized);
    }
    
    if (config.enableMinification) {
        // 简单的压缩：移除多余空白
        optimized = CHTL::Util::StringUtils::replace_all(optimized, "  ", " ");
        optimized = CHTL::Util::StringUtils::replace_all(optimized, "\n\n", "\n");
    }
    
    return CHTL::Util::StringUtils::trim(optimized);
}

String CJMODGenerator::removeRedundantCode(const String& code) {
    // 移除重复的函数声明等
    return code;
}

String CJMODGenerator::combineVariables(const String& code) {
    // 合并变量声明等优化
    return code;
}

// 静态方法实现
GenerationResult CJMODGenerator::exportResult(const Arg& args) {
    CJMODGenerator generator;
    return generator.generate(args, "export");
}

String CJMODGenerator::exportJavaScript(const Arg& args, const String& keyword) {
    CJMODGenerator generator;
    auto result = generator.generate(args, keyword);
    return result.success ? result.generatedCode : "";
}

// CJMODScanner实现
CJMODScanner::CJMODScanner(const String& source) : sourceCode(source) {
    initializePointers();
}

Arg CJMODScanner::scan(const Arg& args, const String& keyword) {
    Arg result = args;
    
    // 扫描逻辑：查找关键字并填充参数
    for (auto& atom : result.atoms) {
        if (atom.placeholder.find("$") != String::npos) {
            // 这里应该从源代码中提取实际值
            // 简化实现：使用示例值
            if (atom.placeholder == "$") {
                atom.fillValue("defaultValue");
            } else if (atom.placeholder == "$?") {
                atom.fillValue("optionalValue");
            } else if (atom.placeholder == "$!") {
                atom.fillValue("requiredValue");
            }
        }
    }
    
    return result;
}

void CJMODScanner::initializePointers() {
    frontPointer = 0;
    backPointer = 0;
}

bool CJMODScanner::findKeyword(const String& keyword, size_t& position) {
    position = sourceCode.find(keyword, frontPointer);
    return position != String::npos;
}

Arg CJMODScanner::scanFragment(const String& keyword) {
    Arg result;
    
    size_t keywordPos;
    if (findKeyword(keyword, keywordPos)) {
        // 提取片段
        String fragment = extractFragment(backPointer, keywordPos + keyword.length());
        
        // 分析片段
        result = Syntax::analyze(fragment);
    }
    
    return result;
}

String CJMODScanner::extractFragment(size_t start, size_t end) {
    if (start >= sourceCode.length() || end > sourceCode.length() || start >= end) {
        return "";
    }
    
    return sourceCode.substr(start, end - start);
}

// VirtualObjectManager实现
void VirtualObjectManager::registerVirtualObject(const String& name, const String& definition) {
    virtualObjects[name] = definition;
}

String VirtualObjectManager::generateVirtualObjectCode(const String& name) const {
    auto it = virtualObjects.find(name);
    if (it != virtualObjects.end()) {
        return "const " + name + " = " + it->second + ";";
    }
    return "";
}

void VirtualObjectManager::bindVirtualObject(const String& functionName) {
    // 静态绑定实现
    static StringVector boundObjects;
    boundObjects.push_back(functionName);
}

// CJMODUtils命名空间实现
namespace CJMODUtils {

bool validateSyntax(const String& syntax) {
    // 基本语法验证
    if (syntax.empty()) {
        return false;
    }
    
    // 检查括号匹配
    int braceCount = 0;
    for (char c : syntax) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
    }
    
    return braceCount == 0;
}

bool validateKeyword(const String& keyword) {
    if (keyword.empty()) {
        return false;
    }
    
    // 关键字应该是有效的标识符
    return std::isalpha(keyword[0]) || keyword[0] == '_';
}

bool validatePlaceholder(const String& placeholder) {
    StringVector validPlaceholders = {"$", "$?", "$!", "$_", "..."};
    return std::find(validPlaceholders.begin(), validPlaceholders.end(), placeholder) != validPlaceholders.end();
}

String convertToJavaScript(const String& cjmodCode) {
    // 简单的转换逻辑
    String jsCode = cjmodCode;
    
    // 转换增强选择器
    size_t pos = 0;
    while ((pos = jsCode.find("{{", pos)) != String::npos) {
        size_t endPos = jsCode.find("}}", pos);
        if (endPos != String::npos) {
            String selector = jsCode.substr(pos + 2, endPos - pos - 2);
            String replacement = "document.querySelector('" + selector + "')";
            jsCode.replace(pos, endPos - pos + 2, replacement);
            pos += replacement.length();
        } else {
            break;
        }
    }
    
    // 转换操作符
    jsCode = CHTL::Util::StringUtils::replace_all(jsCode, "->", ".");
    
    return jsCode;
}

StringVector extractPlaceholders(const String& syntax) {
    StringVector placeholders;
    std::regex placeholderPattern(R"(\$[?!_]?)");
    
    std::sregex_iterator iter(syntax.begin(), syntax.end(), placeholderPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        placeholders.push_back(iter->str());
    }
    
    return placeholders;
}

String replacePlaceholders(const String& template_, const StringUnorderedMap& values) {
    String result = template_;
    
    for (const auto& value : values) {
        String placeholder = "${" + value.first + "}";
        result = CHTL::Util::StringUtils::replace_all(result, placeholder, value.second);
    }
    
    return result;
}

StringVector findKeywords(const String& code) {
    StringVector keywords;
    
    // 查找常见的CJMOD关键字
    StringVector commonKeywords = {"listen", "delegate", "animate", "module", "vir"};
    
    for (const auto& keyword : commonKeywords) {
        if (code.find(keyword) != String::npos) {
            keywords.push_back(keyword);
        }
    }
    
    return keywords;
}

String formatCJMODCode(const String& code) {
    String formatted = code;
    
    // 简单的格式化
    formatted = CHTL::Util::StringUtils::replace_all(formatted, "{", " {\n");
    formatted = CHTL::Util::StringUtils::replace_all(formatted, "}", "\n}");
    formatted = CHTL::Util::StringUtils::replace_all(formatted, ";", ";\n");
    
    return formatted;
}

void printCJMODInfo(const Arg& args) {
    std::cout << "CJMOD Arg 信息:" << std::endl;
    std::cout << "原子数量: " << args.atoms.size() << std::endl;
    
    for (size_t i = 0; i < args.atoms.size(); ++i) {
        const auto& atom = args.atoms[i];
        std::cout << "原子 " << i << ": ";
        std::cout << "占位符=" << atom.placeholder;
        std::cout << ", 值=" << atom.getValue();
        std::cout << ", 已填充=" << (atom.isFilled ? "是" : "否");
        std::cout << std::endl;
    }
}

} // namespace CJMODUtils

} // namespace CJMOD