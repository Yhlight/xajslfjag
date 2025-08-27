#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <unordered_set>

namespace CHTL {

// 脚本选择器类型
enum class ScriptSelectorType {
    ELEMENT,        // 元素选择器 {{button}}
    CLASS,          // 类选择器 {{.box}}
    ID,             // ID选择器 {{#box}}
    COMPLEX         // 复合选择器 {{.box button}}
};

// 脚本选择器结构
struct ScriptSelector {
    ScriptSelectorType type;
    std::string selector;
    int index;  // 索引访问，-1表示不使用索引
    
    ScriptSelector(ScriptSelectorType t, const std::string& s, int idx = -1) 
        : type(t), selector(s), index(idx) {}
};

// 脚本节点类
class ScriptNode : public BaseNode {
private:
    std::string content;
    bool isGlobal;  // 是否为全局脚本块
    std::vector<ScriptSelector> selectors;  // 使用的选择器列表
    std::unordered_set<std::string> autoAddClasses;  // 需要自动添加的类名
    std::unordered_set<std::string> autoAddIds;      // 需要自动添加的ID
    
public:
    ScriptNode(bool global = false, int line = 0, int col = 0)
        : BaseNode(NodeType::SCRIPT, line, col), isGlobal(global) {}
    
    // 内容管理
    const std::string& getContent() const { return content; }
    void setContent(const std::string& c) { content = c; }
    void appendContent(const std::string& c) { content += c; }
    
    // 全局/局部判断
    bool getIsGlobal() const { return isGlobal; }
    void setIsGlobal(bool global) { isGlobal = global; }
    
    // 选择器管理
    void addSelector(const ScriptSelector& sel);
    const std::vector<ScriptSelector>& getSelectors() const { return selectors; }
    
    // 自动添加管理
    void addAutoAddClass(const std::string& className);
    void addAutoAddId(const std::string& idName);
    const std::unordered_set<std::string>& getAutoAddClasses() const { return autoAddClasses; }
    const std::unordered_set<std::string>& getAutoAddIds() const { return autoAddIds; }
    
    // 获取第一个需要自动添加的类名/ID
    std::string getFirstAutoAddClass() const;
    std::string getFirstAutoAddId() const;
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class ScriptNodeVisitor : public NodeVisitor {
public:
    virtual void visitScriptNode(ScriptNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H