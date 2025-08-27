#pragma once
#include "BaseNode.hpp"

namespace CHTL {

/**
 * 注释节点类型枚举
 */
enum class CommentType {
    SINGLE_LINE,          // 单行注释 //
    MULTI_LINE,           // 多行注释 /**/
    GENERATOR             // 生成器注释 --
};

/**
 * CHTL注释节点类
 * 处理三种类型的注释：单行、多行、生成器注释
 */
class CommentNode : public BaseNode {
private:
    CommentType commentType;
    std::string commentContent;
    bool shouldGenerate;      // 是否应该生成到输出中
    std::string targetLanguage; // 目标语言（用于--注释）

public:
    CommentNode(CommentType type = CommentType::SINGLE_LINE, const std::string& content = "");
    
    // 注释特定方法
    CommentType getCommentType() const;
    void setCommentType(CommentType type);
    
    const std::string& getCommentContent() const;
    void setCommentContent(const std::string& content);
    
    bool getShouldGenerate() const;
    void setShouldGenerate(bool generate);
    
    const std::string& getTargetLanguage() const;
    void setTargetLanguage(const std::string& language);
    
    // 生成不同语言的注释
    std::string generateComment(const std::string& language) const;
    
    // 重写基类方法
    std::string toString() const override;
    std::string toHTML() const override;
    std::string toCSS() const override;
    std::string toJavaScript() const override;
    void validate(CHTLState& state) override;
    std::shared_ptr<BaseNode> clone() const override;
};

}