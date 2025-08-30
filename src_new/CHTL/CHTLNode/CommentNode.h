#pragma once
#include "BaseNode.h"

namespace CHTL {

/**
 * 注释类型枚举
 */
enum class CommentType {
    LINE_COMMENT,       // // 行注释
    BLOCK_COMMENT,      // /* */ 块注释  
    GENERATOR_COMMENT   // -- 生成器注释
};

/**
 * 注释节点
 * 表示CHTL中的各种注释
 */
class CommentNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    CommentNode(CommentType commentType, const std::string& content = "");
    
    /**
     * 析构函数
     */
    virtual ~CommentNode() = default;
    
    // ========== 注释属性 ==========
    
    /**
     * 获取注释类型
     */
    CommentType getCommentType() const;
    
    /**
     * 获取注释内容
     */
    const std::string& getCommentContent() const;
    
    /**
     * 设置注释内容
     */
    void setCommentContent(const std::string& content);
    
    /**
     * 检查是否为生成器注释
     */
    bool isGeneratorComment() const;
    
    /**
     * 获取注释类型字符串
     */
    std::string getCommentTypeString() const;
    
    // ========== 重写基类方法 ==========
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 转换为字符串
     */
    std::string toString() const override;
    
    /**
     * 验证节点
     */
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

private:
    CommentType m_commentType;      // 注释类型
    std::string m_commentContent;   // 注释内容
};

} // namespace CHTL