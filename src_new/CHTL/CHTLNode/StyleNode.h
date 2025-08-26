#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * StyleNode - 样式节点
 * 处理style块的CSS代码
 */
class StyleNode : public BaseNode {
private:
    std::string css_content_;
    bool is_local_;                     // 是否为局部样式
    std::string parent_selector_;       // 父选择器（局部样式时）
    std::vector<std::string> selectors_; // 解析出的选择器
    bool auto_add_class_;              // 是否自动添加class
    bool auto_add_id_;                 // 是否自动添加id
    
public:
    /**
     * 构造函数
     * @param css_content CSS内容
     * @param is_local 是否为局部样式
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    StyleNode(const std::string& css_content,
              bool is_local = false,
              const std::string& location = "", 
              int line = 0, 
              int column = 0);
    
    /**
     * 获取CSS内容
     * @return CSS内容
     */
    const std::string& getCSSContent() const;
    
    /**
     * 设置CSS内容
     * @param content CSS内容
     */
    void setCSSContent(const std::string& content);
    
    /**
     * 是否为局部样式
     * @return 是否为局部样式
     */
    bool isLocal() const;
    
    /**
     * 设置是否为局部样式
     * @param is_local 是否为局部样式
     */
    void setLocal(bool is_local);
    
    /**
     * 获取父选择器
     * @return 父选择器
     */
    const std::string& getParentSelector() const;
    
    /**
     * 设置父选择器
     * @param selector 父选择器
     */
    void setParentSelector(const std::string& selector);
    
    /**
     * 获取选择器列表
     * @return 选择器列表
     */
    const std::vector<std::string>& getSelectors() const;
    
    /**
     * 添加选择器
     * @param selector 选择器
     */
    void addSelector(const std::string& selector);
    
    /**
     * 是否自动添加class
     * @return 是否自动添加
     */
    bool shouldAutoAddClass() const;
    
    /**
     * 设置是否自动添加class
     * @param auto_add 是否自动添加
     */
    void setAutoAddClass(bool auto_add);
    
    /**
     * 是否自动添加id
     * @return 是否自动添加
     */
    bool shouldAutoAddId() const;
    
    /**
     * 设置是否自动添加id
     * @param auto_add 是否自动添加
     */
    void setAutoAddId(bool auto_add);
    
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
     * 生成最终CSS
     * @return 生成的CSS
     */
    std::string generateCSS() const;
    
    /**
     * 解析CSS选择器
     */
    void parseSelectors();
    
    /**
     * 处理&引用
     * @param css CSS内容
     * @return 处理后的CSS
     */
    std::string processAmpersandReferences(const std::string& css) const;
};

} // namespace Node
} // namespace CHTL