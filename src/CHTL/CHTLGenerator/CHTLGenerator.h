#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/Context.h"

namespace CHTL {

struct GenerationOptions {
    bool enableMinification = false;
    bool enableSourceMap = false;
    bool enableDebugComments = false;
    bool preserveComments = true;
    std::string indentation = "  ";
    std::string lineEnding = "\n";
};

class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator();
    
    // 主生成接口
    std::string generateHTML(std::shared_ptr<BaseNode> rootNode);
    std::string generateCSS(std::shared_ptr<BaseNode> rootNode);
    std::string generateJavaScript(std::shared_ptr<BaseNode> rootNode);
    
    // 完整生成
    struct GenerationResult {
        std::string html;
        std::string css;
        std::string javascript;
        bool success = false;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    GenerationResult generateAll(std::shared_ptr<BaseNode> rootNode);
    
    // 设置和配置
    void setContext(std::shared_ptr<CHTLContext> context);
    void setOptions(const GenerationOptions& options);
    GenerationOptions getOptions() const { return m_options; }
    
    // 调试和错误处理
    void enableDebugMode(bool enable) { m_debugMode = enable; }
    std::vector<std::string> getErrors() const { return m_errors; }
    std::vector<std::string> getWarnings() const { return m_warnings; }
    void clearErrors();
    
private:
    std::shared_ptr<CHTLContext> m_context;
    GenerationOptions m_options;
    bool m_debugMode;
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
    
    // HTML生成
    std::string generateHTMLNode(std::shared_ptr<BaseNode> node, int depth = 0);
    std::string generateHTMLElement(std::shared_ptr<BaseNode> node, int depth);
    std::string generateHTMLText(std::shared_ptr<BaseNode> node);
    std::string generateHTMLAttribute(std::shared_ptr<BaseNode> node);
    std::string generateHTMLComment(std::shared_ptr<BaseNode> node);
    
    // CSS生成
    std::string generateCSSNode(std::shared_ptr<BaseNode> node, int depth = 0);
    std::string generateCSSRule(std::shared_ptr<BaseNode> node, int depth);
    std::string generateCSSProperty(std::shared_ptr<BaseNode> node);
    std::string generateCSSSelector(std::shared_ptr<BaseNode> node);
    
    // JavaScript生成
    std::string generateJSNode(std::shared_ptr<BaseNode> node, int depth = 0);
    std::string generateJSFunction(std::shared_ptr<BaseNode> node);
    std::string generateJSStatement(std::shared_ptr<BaseNode> node);
    
    // 模板和自定义处理
    std::string expandTemplate(std::shared_ptr<BaseNode> templateNode);
    std::string expandCustom(std::shared_ptr<BaseNode> customNode);
    std::string processTemplateInheritance(std::shared_ptr<BaseNode> node);
    std::string processCustomSpecialization(std::shared_ptr<BaseNode> node);
    
    // 变量替换
    std::string replaceVariables(const std::string& content);
    std::string resolveVariableReference(const std::string& varRef);
    
    // 原始嵌入处理
    std::string processOriginEmbedding(std::shared_ptr<BaseNode> originNode);
    std::string generateOriginContent(const std::string& type, const std::string& content);
    
    // 生成器注释处理
    std::string processGeneratorComment(const std::string& comment, const std::string& context);
    
    // 格式化和优化
    std::string formatHTML(const std::string& html);
    std::string formatCSS(const std::string& css);
    std::string formatJavaScript(const std::string& js);
    std::string minifyCode(const std::string& code, const std::string& type);
    
    // 辅助方法
    std::string getIndentation(int depth) const;
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJavaScript(const std::string& text);
    
    // 节点收集
    void collectStyleNodes(std::shared_ptr<BaseNode> node, std::vector<std::shared_ptr<BaseNode>>& styleNodes);
    void collectScriptNodes(std::shared_ptr<BaseNode> node, std::vector<std::shared_ptr<BaseNode>>& scriptNodes);
    
    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    
    // 上下文管理
    void pushGenerationContext(const std::string& context);
    void popGenerationContext();
    std::string getCurrentGenerationContext() const;
    
private:
    std::vector<std::string> m_generationContextStack;
};

}