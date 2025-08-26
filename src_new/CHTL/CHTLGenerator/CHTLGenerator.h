#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace Generator {

/**
 * CHTLGenerator - CHTL代码生成器
 * 将CHTL AST转换为HTML、CSS、JavaScript
 */
class CHTLGenerator {
public:
    /**
     * 生成结果
     */
    struct GenerateResult {
        bool success;
        std::string html;
        std::string css;
        std::string javascript;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        GenerateResult() : success(false) {}
    };

private:
    std::shared_ptr<Context::CHTLContext> context_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
public:
    /**
     * 构造函数
     * @param context CHTL上下文
     */
    explicit CHTLGenerator(std::shared_ptr<Context::CHTLContext> context);
    
    /**
     * 生成代码
     * @param root_node 根节点
     * @return 生成结果
     */
    GenerateResult generate(std::shared_ptr<Node::BaseNode> root_node);
    
    /**
     * 生成HTML
     * @param node 节点
     * @return HTML字符串
     */
    std::string generateHTML(std::shared_ptr<Node::BaseNode> node);
    
    /**
     * 生成CSS
     * @param node 节点
     * @return CSS字符串
     */
    std::string generateCSS(std::shared_ptr<Node::BaseNode> node);
    
    /**
     * 生成JavaScript
     * @param node 节点
     * @return JavaScript字符串
     */
    std::string generateJavaScript(std::shared_ptr<Node::BaseNode> node);
    
    /**
     * 获取错误列表
     * @return 错误列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * 获取警告列表
     * @return 警告列表
     */
    const std::vector<std::string>& getWarnings() const;

private:
    /**
     * 生成元素节点HTML
     * @param element_node 元素节点
     * @return HTML字符串
     */
    std::string generateElementHTML(std::shared_ptr<Node::BaseNode> element_node);
    
    /**
     * 生成文本节点HTML
     * @param text_node 文本节点
     * @return HTML字符串
     */
    std::string generateTextHTML(std::shared_ptr<Node::BaseNode> text_node);
    
    /**
     * 生成样式节点CSS
     * @param style_node 样式节点
     * @return CSS字符串
     */
    std::string generateStyleCSS(std::shared_ptr<Node::BaseNode> style_node);
    
    /**
     * 生成脚本节点JavaScript
     * @param script_node 脚本节点
     * @return JavaScript字符串
     */
    std::string generateScriptJS(std::shared_ptr<Node::BaseNode> script_node);
    
    /**
     * 添加错误
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * 添加警告
     * @param warning 警告信息
     */
    void addWarning(const std::string& warning);
};

} // namespace Generator
} // namespace CHTL