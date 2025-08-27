#include "CHTLGenerator.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/TemplateNode.h"
#include "CHTLNode/CustomNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/OriginNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/ConfigNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/OperatorNode.h"
#include <algorithm>

namespace CHTL {

CHTLGenerator::CHTLGenerator(std::shared_ptr<GlobalMap> globalMap,
                           std::shared_ptr<CHTLContext> context)
    : globalMap(globalMap ? globalMap : std::make_shared<GlobalMap>()),
      context(context ? context : std::make_shared<CHTLContext>()),
      indentLevel(0), inHead(false), useHtml5(false) {
}

CHTLGenerator::~CHTLGenerator() = default;

std::string CHTLGenerator::Generate(NodePtr root) {
    ClearErrors();
    output.str("");
    globalStyles.str("");
    globalScripts.str("");
    indentLevel = 0;
    
    // 访问根节点
    root->Accept(this);
    
    return output.str();
}

void CHTLGenerator::VisitElementNode(ElementNode* node) {
    std::string tagName = node->GetName();
    
    // 处理特殊的HTML结构
    if (tagName == "html") {
        if (useHtml5) {
            WriteLine("<!DOCTYPE html>");
        }
    }
    
    // 开始标签
    WriteIndent();
    Write("<" + tagName);
    
    // 生成属性
    Write(GenerateAttributes(node->GetAttributes()));
    
    // 检查是否是自闭合标签
    if (node->IsVoidElement() && node->GetChildCount() == 0 && 
        !node->GetStyleNode() && !node->GetScriptNode()) {
        WriteLine(" />");
        return;
    }
    
    WriteLine(">");
    
    Indent();
    
    // 处理局部样式
    if (node->GetStyleNode()) {
        ProcessLocalStyle(
            std::dynamic_pointer_cast<StyleNode>(node->GetStyleNode()).get(),
            node
        );
    }
    
    // 访问子节点
    for (const auto& child : node->GetChildren()) {
        child->Accept(this);
    }
    
    // 处理局部脚本
    if (node->GetScriptNode()) {
        ProcessLocalScript(
            std::dynamic_pointer_cast<ScriptNode>(node->GetScriptNode()).get(),
            node
        );
    }
    
    Dedent();
    
    // 结束标签
    WriteIndent();
    WriteLine("</" + tagName + ">");
}

void CHTLGenerator::VisitTextNode(TextNode* node) {
    WriteIndent();
    WriteLine(node->GetContent());
}

void CHTLGenerator::VisitCommentNode(CommentNode* node) {
    if (node->GetCommentType() == CommentType::GENERATOR) {
        WriteIndent();
        WriteLine(node->GenerateForContext(context->GetCurrentScope()));
    }
    // 单行和多行注释不生成输出
}

void CHTLGenerator::VisitTemplateNode(TemplateNode* node) {
    // 模板定义不生成输出
}

void CHTLGenerator::VisitCustomNode(CustomNode* node) {
    // 自定义定义不生成输出
}

void CHTLGenerator::VisitStyleNode(StyleNode* node) {
    if (!node->IsLocalStyle()) {
        // 全局样式
        WriteIndent();
        WriteLine("<style>");
        Indent();
        
        // 获取样式内容
        auto separated = node->SeparateStyles();
        Write(separated.second);
        
        Dedent();
        WriteIndent();
        WriteLine("</style>");
    }
    // 局部样式在ProcessLocalStyle中处理
}

void CHTLGenerator::VisitScriptNode(ScriptNode* node) {
    if (!node->IsLocalScript()) {
        // 全局脚本
        WriteIndent();
        WriteLine("<script>");
        Indent();
        
        WriteIndent();
        WriteLine(node->GetContent());
        
        Dedent();
        WriteIndent();
        WriteLine("</script>");
    }
    // 局部脚本在ProcessLocalScript中处理
}

void CHTLGenerator::VisitOriginNode(OriginNode* node) {
    // 原始嵌入直接输出
    Write(node->Generate());
}

void CHTLGenerator::VisitImportNode(ImportNode* node) {
    // 导入节点不生成输出
    // 导入的内容应该已经被解析器处理
}

void CHTLGenerator::VisitConfigNode(ConfigNode* node) {
    // 配置节点不生成输出
}

void CHTLGenerator::VisitNamespaceNode(NamespaceNode* node) {
    // 命名空间节点不生成输出，只处理其子节点
    for (const auto& child : node->GetChildren()) {
        child->Accept(this);
    }
}

void CHTLGenerator::VisitOperatorNode(OperatorNode* node) {
    switch (node->GetOperatorType()) {
        case OperatorType::USE:
            ProcessUseOperator(node);
            break;
        case OperatorType::DELETE:
            ProcessDeleteOperator(node);
            break;
        case OperatorType::INSERT:
            ProcessInsertOperator(node);
            break;
        default:
            // 其他操作符在解析阶段处理
            break;
    }
}

void CHTLGenerator::ProcessLocalStyle(StyleNode* node, ElementNode* element) {
    // 分离内联样式和全局样式
    auto [inlineStyle, globalStyle] = node->SeparateStyles();
    
    // 添加内联样式到元素
    if (!inlineStyle.empty()) {
        std::string existingStyle = element->GetAttribute("style");
        if (!existingStyle.empty()) {
            existingStyle += " ";
        }
        element->SetAttribute("style", existingStyle + inlineStyle);
    }
    
    // 添加全局样式到全局样式表
    if (!globalStyle.empty()) {
        globalStyles << globalStyle << "\n";
    }
}

void CHTLGenerator::ProcessLocalScript(ScriptNode* node, ElementNode* element) {
    // 获取自动化选择器配置
    auto config = context->GetActiveConfig();
    if (!config) {
        config = globalMap->GetDefaultConfig();
    }
    
    bool autoAddClass = true;
    bool autoAddId = true;
    
    if (config) {
        autoAddClass = config->GetConfigValue("DISABLE_SCRIPT_AUTO_ADD_CLASS") != "true";
        autoAddId = config->GetConfigValue("DISABLE_SCRIPT_AUTO_ADD_ID") != "true";
    }
    
    // 自动添加类或ID
    if (autoAddClass && !element->HasAttribute("class") && !context->IsAutoClassAdded()) {
        std::string className = node->GetFirstClassSelector();
        if (!className.empty()) {
            element->SetAttribute("class", className);
            context->SetAutoClassAdded(true);
        }
    }
    
    if (autoAddId && !element->HasAttribute("id") && !context->IsAutoIdAdded()) {
        std::string idName = node->GetFirstIdSelector();
        if (!idName.empty()) {
            element->SetAttribute("id", idName);
            context->SetAutoIdAdded(true);
        }
    }
    
    // 生成包装的脚本
    globalScripts << node->WrapLocalScript() << "\n";
}

void CHTLGenerator::ProcessUseOperator(OperatorNode* node) {
    std::string target = node->GetUseTarget();
    if (target == "html5") {
        useHtml5 = true;
    }
}

std::string CHTLGenerator::GenerateAttributes(const std::unordered_map<std::string, std::string>& attributes) {
    std::string result;
    
    for (const auto& [name, value] : attributes) {
        // 跳过特殊属性
        if (name == "index" || name == "self-closing") {
            continue;
        }
        
        result += " " + name + "=\"" + value + "\"";
    }
    
    return result;
}

void CHTLGenerator::Indent() {
    indentLevel++;
    UpdateIndent();
}

void CHTLGenerator::Dedent() {
    if (indentLevel > 0) {
        indentLevel--;
        UpdateIndent();
    }
}

void CHTLGenerator::UpdateIndent() {
    currentIndent = std::string(indentLevel * 4, ' ');
}

void CHTLGenerator::Write(const std::string& text) {
    output << text;
}

void CHTLGenerator::WriteLine(const std::string& text) {
    output << text << "\n";
}

void CHTLGenerator::WriteIndent() {
    output << currentIndent;
}

void CHTLGenerator::ReportError(const std::string& message) {
    errors.push_back(message);
}

void CHTLGenerator::ProcessDeleteOperator(OperatorNode* node) {
    // TODO: 实现删除操作符处理
    // 这应该修改当前上下文中的元素或属性
}

void CHTLGenerator::ProcessInsertOperator(OperatorNode* node) {
    // TODO: 实现插入操作符处理
    // 这应该在当前上下文中插入新的元素或属性
}

void CHTLGenerator::ProcessTemplateUsage(const std::string& type, const std::string& name,
                                        const std::unordered_map<std::string, std::string>& params) {
    // TODO: 实现模板使用处理
}

void CHTLGenerator::ProcessCustomUsage(const std::string& type, const std::string& name,
                                      const std::unordered_map<std::string, std::string>& params,
                                      const std::set<std::string>& deletions) {
    // TODO: 实现自定义使用处理
}



} // namespace CHTL