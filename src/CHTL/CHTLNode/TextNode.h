#pragma once
#include "BaseNode.h"

namespace CHTL {

/**
 * 文本节点
 * 表示CHTL中的text块
 */
class TextNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    TextNode(const std::string& content = "");
    
    /**
     * 析构函数
     */
    virtual ~TextNode() = default;
    
    // ========== 文本内容管理 ==========
    
    /**
     * 获取文本内容
     */
    const std::string& getContent() const;
    
    /**
     * 设置文本内容
     */
    void setContent(const std::string& content);
    
    /**
     * 追加文本内容
     */
    void appendContent(const std::string& content);
    
    /**
     * 获取原始文本（未处理的）
     */
    const std::string& getRawContent() const;
    
    /**
     * 设置原始文本
     */
    void setRawContent(const std::string& rawContent);
    
    // ========== 文本类型 ==========
    
    /**
     * 检查是否为字面量文本（无引号）
     */
    bool isLiteral() const;
    
    /**
     * 设置是否为字面量文本
     */
    void setLiteral(bool literal);
    
    /**
     * 检查是否为双引号字符串
     */
    bool isDoubleQuoted() const;
    
    /**
     * 设置是否为双引号字符串
     */
    void setDoubleQuoted(bool doubleQuoted);
    
    /**
     * 检查是否为单引号字符串
     */
    bool isSingleQuoted() const;
    
    /**
     * 设置是否为单引号字符串
     */
    void setSingleQuoted(bool singleQuoted);
    
    // ========== 文本处理 ==========
    
    /**
     * 获取处理后的文本（转义处理）
     */
    std::string getProcessedContent() const;
    
    /**
     * 检查是否包含变量
     */
    bool hasVariables() const;
    
    /**
     * 获取文本中的变量
     */
    std::vector<std::string> getVariables() const;
    
    /**
     * 替换变量
     */
    void replaceVariable(const std::string& varName, const std::string& value);
    
    /**
     * 获取文本长度
     */
    size_t getLength() const;
    
    /**
     * 检查是否为空白文本
     */
    bool isWhitespace() const;
    
    /**
     * 修剪空白字符
     */
    void trim();
    
    // ========== 重写基类方法 ==========
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 深度克隆节点
     */
    std::shared_ptr<BaseNode> deepClone() const override;
    
    /**
     * 转换为字符串
     */
    std::string toString() const override;
    
    /**
     * 转换为HTML文本
     */
    std::string toHtmlText() const;
    
    /**
     * 验证节点
     */
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

protected:
    /**
     * 内部验证方法
     */
    bool internalValidate(ErrorReporter* errorReporter) const override;

private:
    std::string m_content;      // 处理后的文本内容
    std::string m_rawContent;   // 原始文本内容
    bool m_isLiteral;          // 是否为字面量文本
    bool m_isDoubleQuoted;     // 是否为双引号字符串
    bool m_isSingleQuoted;     // 是否为单引号字符串
    
    // 辅助方法
    std::string processEscapeSequences(const std::string& text) const;
    std::string htmlEscape(const std::string& text) const;
};

} // namespace CHTL