#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * CommentNode - 注释节点
 * 处理CHTL中的注释语法
 */
class CommentNode : public BaseNode {
private:
    std::string comment_content_;
    bool is_chtl_comment_;          // 是否为CHTL注释（--）
    bool generate_target_comment_;   // 是否生成目标语言注释
    
public:
    /**
     * 构造函数
     * @param content 注释内容
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    CommentNode(const std::string& content, 
                const std::string& location = "", 
                int line = 0, 
                int column = 0);
    
    /**
     * 获取注释内容
     * @return 注释内容
     */
    const std::string& getCommentContent() const;
    
    /**
     * 设置注释内容
     * @param content 注释内容
     */
    void setCommentContent(const std::string& content);
    
    /**
     * 是否为CHTL注释
     * @return 是否为CHTL注释
     */
    bool isCHTLComment() const;
    
    /**
     * 设置是否为CHTL注释
     * @param is_chtl 是否为CHTL注释
     */
    void setCHTLComment(bool is_chtl);
    
    /**
     * 是否生成目标语言注释
     * @return 是否生成
     */
    bool shouldGenerateTargetComment() const;
    
    /**
     * 设置是否生成目标语言注释
     * @param generate 是否生成
     */
    void setGenerateTargetComment(bool generate);
    
    /**
     * 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;
    
    /**
     * 接受访问者模式
     * @param visitor 访问者
     */
    void accept(NodeVisitor& visitor) override;
    
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 生成HTML注释
     * @return HTML注释
     */
    std::string generateHTMLComment() const;
    
    /**
     * 生成CSS注释
     * @return CSS注释
     */
    std::string generateCSSComment() const;
    
    /**
     * 生成JavaScript注释
     * @return JavaScript注释
     */
    std::string generateJSComment() const;
};

} // namespace Node
} // namespace CHTL