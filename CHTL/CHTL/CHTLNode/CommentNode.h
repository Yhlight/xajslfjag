#pragma once
#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 注释类型枚举
 */
enum class CommentType {
    SINGLE_LINE,    // 单行注释 //
    MULTI_LINE,     // 多行注释 /* */
    GENERATOR       // 生成器注释 --
};

/**
 * @brief 注释节点类
 * 表示CHTL中的注释
 */
class CommentNode : public BaseNode {
private:
    CommentType comment_type_;  // 注释类型
    std::string content_;       // 注释内容
    bool preserve_in_output_;   // 是否在输出中保留

public:
    /**
     * @brief 构造函数
     * @param type 注释类型
     * @param content 注释内容
     */
    CommentNode(CommentType type, const std::string& content = "");

    /**
     * @brief 析构函数
     */
    virtual ~CommentNode() = default;

    // Getter方法
    CommentType getCommentType() const { return comment_type_; }
    const std::string& getContent() const { return content_; }
    bool preserveInOutput() const { return preserve_in_output_; }

    // Setter方法
    void setCommentType(CommentType type) { comment_type_ = type; }
    void setContent(const std::string& content) { content_ = content; }
    void setPreserveInOutput(bool preserve) { preserve_in_output_ = preserve; }

    /**
     * @brief 追加注释内容
     * @param content 要追加的内容
     */
    void appendContent(const std::string& content);

    /**
     * @brief 克隆节点
     * @param deep 是否深度克隆
     * @return 克隆的节点
     */
    virtual NodePtr clone(bool deep = true) const override;

    /**
     * @brief 接受访问者
     * @param visitor 访问者对象
     */
    virtual void accept(NodeVisitor& visitor) override;

    /**
     * @brief 转换为字符串表示
     * @param indent 缩进级别
     * @return 字符串表示
     */
    virtual std::string toString(int indent = 0) const override;

    /**
     * @brief 转换为目标语言的注释格式
     * @param target_language 目标语言（html, css, js等）
     * @return 格式化的注释字符串
     */
    std::string toComment(const std::string& target_language = "html") const;

    /**
     * @brief 验证注释节点的有效性
     * @return 验证结果消息
     */
    virtual std::string validate() const override;

    /**
     * @brief 获取注释类型的字符串表示
     * @return 类型字符串
     */
    std::string getCommentTypeString() const;

private:
    /**
     * @brief 清理注释内容（移除注释标记）
     * @param raw_content 原始内容
     * @return 清理后的内容
     */
    std::string cleanContent(const std::string& raw_content) const;
};

} // namespace CHTL