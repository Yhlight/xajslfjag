#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../CHTLLexer/Token.h"

namespace CHTL {

// 前向声明
class BaseNode;
class ElementNode;
class TextNode;

/**
 * @brief 节点类型枚举
 */
enum class NodeType {
    BASE,           // 基础节点
    ELEMENT,        // 元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    TEMPLATE,       // 模板节点
    CUSTOM,         // 自定义节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    ORIGIN,         // 原始节点
    IMPORT,         // 导入节点
    CONFIG,         // 配置节点
    NAMESPACE,      // 命名空间节点
    OPERATOR        // 操作符节点
};

/**
 * @brief 节点智能指针类型定义
 */
using NodePtr = std::shared_ptr<BaseNode>;
using NodeList = std::vector<NodePtr>;

/**
 * @brief 基础节点类
 * 所有CHTL节点的基类
 */
class BaseNode : public std::enable_shared_from_this<BaseNode> {
protected:
    NodeType type_;                           // 节点类型
    std::string name_;                        // 节点名称
    NodePtr parent_;                         // 父节点
    NodeList children_;                      // 子节点列表
    std::unordered_map<std::string, std::string> attributes_; // 属性映射
    Position start_pos_;                     // 起始位置
    Position end_pos_;                       // 结束位置
    std::string raw_content_;                // 原始内容

public:
    /**
     * @brief 构造函数
     * @param type 节点类型
     * @param name 节点名称
     */
    BaseNode(NodeType type, const std::string& name = "");

    /**
     * @brief 虚析构函数
     */
    virtual ~BaseNode() = default;

    // Getter方法
    NodeType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    NodePtr getParent() const { return parent_; }
    const NodeList& getChildren() const { return children_; }
    const std::unordered_map<std::string, std::string>& getAttributes() const { return attributes_; }
    const Position& getStartPos() const { return start_pos_; }
    const Position& getEndPos() const { return end_pos_; }
    const std::string& getRawContent() const { return raw_content_; }

    // Setter方法
    void setName(const std::string& name) { name_ = name; }
    void setParent(NodePtr parent) { parent_ = parent; }
    void setStartPos(const Position& pos) { start_pos_ = pos; }
    void setEndPos(const Position& pos) { end_pos_ = pos; }
    void setRawContent(const std::string& content) { raw_content_ = content; }

    /**
     * @brief 添加子节点
     * @param child 子节点
     */
    virtual void addChild(NodePtr child);

    /**
     * @brief 插入子节点到指定位置
     * @param index 位置索引
     * @param child 子节点
     */
    virtual void insertChild(size_t index, NodePtr child);

    /**
     * @brief 移除子节点
     * @param child 要移除的子节点
     * @return 是否成功移除
     */
    virtual bool removeChild(NodePtr child);

    /**
     * @brief 移除指定索引的子节点
     * @param index 索引
     * @return 被移除的节点，如果索引无效返回nullptr
     */
    virtual NodePtr removeChild(size_t index);

    /**
     * @brief 清空所有子节点
     */
    virtual void clearChildren();

    /**
     * @brief 获取子节点数量
     * @return 子节点数量
     */
    size_t getChildCount() const { return children_.size(); }

    /**
     * @brief 检查是否有子节点
     * @return 有子节点返回true
     */
    bool hasChildren() const { return !children_.empty(); }

    /**
     * @brief 根据索引获取子节点
     * @param index 索引
     * @return 子节点，如果索引无效返回nullptr
     */
    NodePtr getChild(size_t index) const;

    /**
     * @brief 根据名称查找子节点
     * @param name 节点名称
     * @return 找到的节点列表
     */
    NodeList findChildrenByName(const std::string& name) const;

    /**
     * @brief 根据类型查找子节点
     * @param type 节点类型
     * @return 找到的节点列表
     */
    NodeList findChildrenByType(NodeType type) const;

    /**
     * @brief 设置属性
     * @param key 属性名
     * @param value 属性值
     */
    void setAttribute(const std::string& key, const std::string& value);

    /**
     * @brief 获取属性
     * @param key 属性名
     * @param default_value 默认值
     * @return 属性值
     */
    std::string getAttribute(const std::string& key, const std::string& default_value = "") const;

    /**
     * @brief 检查是否有指定属性
     * @param key 属性名
     * @return 有该属性返回true
     */
    bool hasAttribute(const std::string& key) const;

    /**
     * @brief 移除属性
     * @param key 属性名
     * @return 是否成功移除
     */
    bool removeAttribute(const std::string& key);

    /**
     * @brief 清空所有属性
     */
    void clearAttributes();

    /**
     * @brief 获取节点的深度（从根节点算起）
     * @return 深度值
     */
    int getDepth() const;

    /**
     * @brief 检查是否为根节点
     * @return 是根节点返回true
     */
    bool isRoot() const { return parent_ == nullptr; }

    /**
     * @brief 检查是否为叶子节点
     * @return 是叶子节点返回true
     */
    bool isLeaf() const { return children_.empty(); }

    /**
     * @brief 获取根节点
     * @return 根节点
     */
    NodePtr getRoot();

    /**
     * @brief 获取所有祖先节点
     * @return 祖先节点列表（从父节点到根节点）
     */
    NodeList getAncestors() const;

    /**
     * @brief 获取所有后代节点
     * @return 后代节点列表
     */
    NodeList getDescendants() const;

    /**
     * @brief 克隆节点
     * @param deep 是否深度克隆（包括子节点）
     * @return 克隆的节点
     */
    virtual NodePtr clone(bool deep = true) const;

    /**
     * @brief 接受访问者模式
     * @param visitor 访问者对象
     */
    virtual void accept(class NodeVisitor& visitor);

    /**
     * @brief 转换为字符串表示
     * @param indent 缩进级别
     * @return 字符串表示
     */
    virtual std::string toString(int indent = 0) const;

    /**
     * @brief 转换为XML格式
     * @param indent 缩进级别
     * @return XML字符串
     */
    virtual std::string toXml(int indent = 0) const;

    /**
     * @brief 转换为JSON格式
     * @param indent 缩进级别
     * @return JSON字符串
     */
    virtual std::string toJson(int indent = 0) const;

    /**
     * @brief 验证节点的有效性
     * @return 验证结果消息，空字符串表示有效
     */
    virtual std::string validate() const;

protected:
    /**
     * @brief 生成缩进字符串
     * @param level 缩进级别
     * @return 缩进字符串
     */
    std::string getIndent(int level) const;

    /**
     * @brief 转义字符串中的特殊字符
     * @param str 原始字符串
     * @return 转义后的字符串
     */
    std::string escapeString(const std::string& str) const;
};

/**
 * @brief 元素节点类
 * 表示HTML元素或CHTL元素
 */
class ElementNode : public BaseNode {
private:
    std::string tag_;                        // 标签名
    bool is_self_closing_;                  // 是否为自闭合标签
    bool is_html_element_;                  // 是否为HTML元素

public:
    /**
     * @brief 构造函数
     * @param tag 标签名
     * @param is_html_element 是否为HTML元素
     */
    ElementNode(const std::string& tag, bool is_html_element = false);

    /**
     * @brief 析构函数
     */
    virtual ~ElementNode() = default;

    // Getter方法
    const std::string& getTag() const { return tag_; }
    bool isSelfClosing() const { return is_self_closing_; }
    bool isHtmlElement() const { return is_html_element_; }

    // Setter方法
    void setTag(const std::string& tag) { tag_ = tag; }
    void setSelfClosing(bool self_closing) { is_self_closing_ = self_closing; }
    void setHtmlElement(bool is_html) { is_html_element_ = is_html; }

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
     * @brief 转换为HTML格式
     * @param indent 缩进级别
     * @return HTML字符串
     */
    std::string toHtml(int indent = 0) const;

    /**
     * @brief 验证元素节点的有效性
     * @return 验证结果消息
     */
    virtual std::string validate() const override;
};

/**
 * @brief 文本节点类
 * 表示文本内容
 */
class TextNode : public BaseNode {
private:
    std::string text_;                       // 文本内容
    bool is_literal_;                       // 是否为字面量
    bool preserve_whitespace_;              // 是否保留空白字符

public:
    /**
     * @brief 构造函数
     * @param text 文本内容
     * @param is_literal 是否为字面量
     */
    TextNode(const std::string& text = "", bool is_literal = false);

    /**
     * @brief 析构函数
     */
    virtual ~TextNode() = default;

    // Getter方法
    const std::string& getText() const { return text_; }
    bool isLiteral() const { return is_literal_; }
    bool preserveWhitespace() const { return preserve_whitespace_; }

    // Setter方法
    void setText(const std::string& text) { text_ = text; }
    void setLiteral(bool literal) { is_literal_ = literal; }
    void setPreserveWhitespace(bool preserve) { preserve_whitespace_ = preserve; }

    /**
     * @brief 追加文本
     * @param text 要追加的文本
     */
    void appendText(const std::string& text);

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
     * @brief 验证文本节点的有效性
     * @return 验证结果消息
     */
    virtual std::string validate() const override;
};

} // namespace CHTL