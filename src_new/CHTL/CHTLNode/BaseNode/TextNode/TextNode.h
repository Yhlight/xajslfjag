#pragma once

#include "../BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * TextNode - 文本节点
 * 新目录结构下的文本节点实现
 */
class TextNode : public BaseNode {
private:
    std::string content_;
    bool is_unquoted_literal_;  // 是否为无修饰字面量
    bool is_interpolated_;      // 是否包含插值表达式
    
public:
    /**
     * 构造函数
     * @param content 文本内容
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    TextNode(const std::string& content, 
             const std::string& location = "", 
             int line = 0, 
             int column = 0);
    
    /**
     * 获取文本内容
     * @return 文本内容
     */
    const std::string& getContent() const;
    
    /**
     * 设置文本内容
     * @param content 文本内容
     */
    void setContent(const std::string& content);
    
    /**
     * 是否为无修饰字面量
     * @return 是否为无修饰字面量
     */
    bool isUnquotedLiteral() const;
    
    /**
     * 设置是否为无修饰字面量
     * @param is_unquoted 是否为无修饰字面量
     */
    void setUnquotedLiteral(bool is_unquoted);
    
    /**
     * 是否包含插值表达式
     * @return 是否包含插值
     */
    bool isInterpolated() const;
    
    /**
     * 设置是否包含插值表达式
     * @param is_interpolated 是否包含插值
     */
    void setInterpolated(bool is_interpolated);
    
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
     * 处理无修饰字面量
     * @return 处理后的内容
     */
    std::string processUnquotedLiteral() const;
    
    /**
     * 处理插值表达式
     * @return 处理后的内容
     */
    std::string processInterpolation() const;
    
    /**
     * 生成最终HTML文本
     * @return HTML文本
     */
    std::string generateHTML() const;
};

} // namespace Node
} // namespace CHTL