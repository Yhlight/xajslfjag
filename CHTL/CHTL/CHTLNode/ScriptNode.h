#pragma once

#include "BaseNode.h"
#include <vector>
#include <regex>

namespace CHTL {

// 脚本节点 - 处理局部脚本块
class ScriptNode : public BaseNode {
public:
    ScriptNode(bool isLocal = false);
    
    bool IsLocalScript() const { return isLocal; }
    void SetLocal(bool local) { isLocal = local; }
    
    std::string GetContent() const { return content; }
    void SetContent(const std::string& c) { content = c; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 选择器自动化
    std::vector<std::string> ExtractClassSelectors() const;
    std::vector<std::string> ExtractIdSelectors() const;
    std::string GetFirstClassSelector() const;
    std::string GetFirstIdSelector() const;
    
    // 检查是否包含CHTL JS特征
    bool ContainsCHTLJSFeatures() const;
    
    // 处理局部脚本（包装成不污染全局的形式）
    std::string WrapLocalScript() const;
    
private:
    bool isLocal;
    std::string content;
    
    // 从{{.class}}或{{#id}}中提取选择器
    std::vector<std::string> ExtractSelectorsWithPattern(const std::string& pattern) const;
};

} // namespace CHTL