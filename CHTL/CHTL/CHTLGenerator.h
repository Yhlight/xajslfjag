#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include "CHTLNode/BaseNode.h"
#include "CHTLContext.h"
#include "CHTLLexer/GlobalMap.h"

namespace CHTL {

// CHTL代码生成器
class CHTLGenerator : public NodeVisitor {
public:
    CHTLGenerator(std::shared_ptr<GlobalMap> globalMap = nullptr,
                  std::shared_ptr<CHTLContext> context = nullptr);
    ~CHTLGenerator();
    
    // 生成HTML代码
    std::string Generate(NodePtr root);
    
    // 获取分离的样式和脚本
    std::string GetGlobalStyles() const { return globalStyles.str(); }
    std::string GetGlobalScripts() const { return globalScripts.str(); }
    
    // 错误处理
    bool HasErrors() const { return !errors.empty(); }
    std::vector<std::string> GetErrors() const { return errors; }
    void ClearErrors() { errors.clear(); }
    
    // 访问者模式实现
    void VisitElementNode(ElementNode* node) override;
    void VisitTextNode(TextNode* node) override;
    void VisitCommentNode(CommentNode* node) override;
    void VisitTemplateNode(TemplateNode* node) override;
    void VisitCustomNode(CustomNode* node) override;
    void VisitStyleNode(StyleNode* node) override;
    void VisitScriptNode(ScriptNode* node) override;
    void VisitOriginNode(OriginNode* node) override;
    void VisitImportNode(ImportNode* node) override;
    void VisitConfigNode(ConfigNode* node) override;
    void VisitNamespaceNode(NamespaceNode* node) override;
    void VisitOperatorNode(OperatorNode* node) override;
    
private:
    std::shared_ptr<GlobalMap> globalMap;
    std::shared_ptr<CHTLContext> context;
    
    // 输出流
    std::stringstream output;
    std::stringstream globalStyles;
    std::stringstream globalScripts;
    
    // 缩进管理
    int indentLevel;
    std::string currentIndent;
    
    // 错误列表
    std::vector<std::string> errors;
    
    // 当前处理状态
    bool inHead;
    bool useHtml5;
    
    // 辅助方法
    void Indent();
    void Dedent();
    void UpdateIndent();
    void Write(const std::string& text);
    void WriteLine(const std::string& text);
    void WriteIndent();
    
    // 错误报告
    void ReportError(const std::string& message);
    
    // 处理模板/自定义实例化
    void ProcessTemplateUsage(const std::string& type, const std::string& name,
                             const std::unordered_map<std::string, std::string>& params = {});
    void ProcessCustomUsage(const std::string& type, const std::string& name,
                           const std::unordered_map<std::string, std::string>& params = {},
                           const std::set<std::string>& deletions = {});
    
    // 处理局部样式
    void ProcessLocalStyle(StyleNode* node, ElementNode* element);
    
    // 处理局部脚本
    void ProcessLocalScript(ScriptNode* node, ElementNode* element);
    
    // 处理操作符
    void ProcessUseOperator(OperatorNode* node);
    void ProcessDeleteOperator(OperatorNode* node);
    void ProcessInsertOperator(OperatorNode* node);
    
    // 生成属性字符串
    std::string GenerateAttributes(const std::unordered_map<std::string, std::string>& attributes);
    
    // 处理变量组使用
    std::string ProcessVariableUsage(const std::string& varGroup, const std::string& varName);
};

} // namespace CHTL