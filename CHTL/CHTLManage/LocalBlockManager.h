#ifndef CHTL_LOCAL_BLOCK_MANAGER_H
#define CHTL_LOCAL_BLOCK_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../CHTLContext/CHTLContext.h"

namespace CHTL {

// 局部块类型
enum class LocalBlockType {
    STYLE_BLOCK,        // style { }
    SCRIPT_BLOCK,       // script { }
    INLINE_STYLE        // 内联样式
};

// 局部块信息
struct LocalBlockInfo {
    LocalBlockType type;
    std::string content;                    // 块内容
    std::vector<SelectorInfo> selectors;    // 关联的选择器
    std::string elementName;                // 所属元素
    std::string elementId;                  // 元素ID
    std::vector<std::string> elementClasses;// 元素类列表
    size_t startLine;                       // 开始行号
    size_t endLine;                         // 结束行号
    bool hasAutoSelector;                   // 是否有自动选择器
    
    LocalBlockInfo() : type(LocalBlockType::STYLE_BLOCK), 
                      startLine(0), endLine(0), 
                      hasAutoSelector(false) {}
};

// CSS规则信息
struct CSSRuleInfo {
    std::string selector;
    std::unordered_map<std::string, std::string> properties;
    std::string rawContent;
    
    CSSRuleInfo() = default;
};

// 脚本处理器信息
struct ScriptHandlerInfo {
    std::string eventType;      // 事件类型
    std::string selector;       // 选择器
    std::string handlerCode;    // 处理器代码
    bool isDelegated;          // 是否使用事件委托
    
    ScriptHandlerInfo() : isDelegated(false) {}
};

// 局部块管理器 - 管理局部样式和脚本块
class LocalBlockManager {
private:
    // 局部块存储：元素路径 -> 局部块列表
    std::unordered_map<std::string, std::vector<LocalBlockInfo>> localBlocks;
    
    // CSS规则缓存：选择器 -> 规则信息
    std::unordered_map<std::string, std::vector<CSSRuleInfo>> cssRulesCache;
    
    // 脚本处理器缓存：选择器 -> 处理器列表
    std::unordered_map<std::string, std::vector<ScriptHandlerInfo>> scriptHandlersCache;
    
    // 当前处理的元素栈
    std::vector<std::string> elementPathStack;
    
    // 辅助方法
    std::string getCurrentElementPath() const;
    void parseCSSContent(const std::string& content, std::vector<CSSRuleInfo>& rules);
    void parseScriptContent(const std::string& content, std::vector<ScriptHandlerInfo>& handlers);
    std::string generateUniqueClass(const std::string& elementName, size_t blockIndex);
    
public:
    LocalBlockManager();
    ~LocalBlockManager() = default;
    
    // 元素管理
    void enterElement(const std::string& elementName);
    void exitElement();
    std::string getCurrentElement() const;
    
    // 局部块管理
    void addStyleBlock(const LocalBlockInfo& blockInfo);
    void addScriptBlock(const LocalBlockInfo& blockInfo);
    void addInlineStyle(const std::string& elementPath, const std::string& styleContent);
    
    // 获取局部块
    std::vector<LocalBlockInfo> getLocalBlocks(const std::string& elementPath) const;
    std::vector<LocalBlockInfo> getStyleBlocks(const std::string& elementPath) const;
    std::vector<LocalBlockInfo> getScriptBlocks(const std::string& elementPath) const;
    
    // CSS规则管理
    void extractCSSRules(const LocalBlockInfo& styleBlock);
    std::vector<CSSRuleInfo> getCSSRules(const std::string& selector) const;
    std::vector<CSSRuleInfo> getAllCSSRules() const;
    bool hasCSSRule(const std::string& selector) const;
    
    // 脚本处理器管理
    void extractScriptHandlers(const LocalBlockInfo& scriptBlock);
    std::vector<ScriptHandlerInfo> getScriptHandlers(const std::string& selector) const;
    std::vector<ScriptHandlerInfo> getAllScriptHandlers() const;
    bool hasScriptHandler(const std::string& selector, const std::string& eventType) const;
    
    // 选择器作用域处理
    std::string scopeSelector(const std::string& selector, const std::string& scope) const;
    std::vector<std::string> getScopedSelectors(const LocalBlockInfo& block) const;
    
    // 自动生成类名
    std::string generateAutoClass(const std::string& elementPath, LocalBlockType type);
    std::string generateAutoId(const std::string& elementPath, LocalBlockType type);
    
    // 合并处理
    std::string mergeStyleBlocks(const std::vector<LocalBlockInfo>& blocks) const;
    std::string mergeScriptBlocks(const std::vector<LocalBlockInfo>& blocks) const;
    
    // 验证
    bool validateStyleBlock(const LocalBlockInfo& block) const;
    bool validateScriptBlock(const LocalBlockInfo& block) const;
    std::vector<std::string> getBlockErrors(const LocalBlockInfo& block) const;
    
    // 清理
    void clearElement(const std::string& elementPath);
    void clearAll();
    
    // 统计
    size_t getStyleBlockCount() const;
    size_t getScriptBlockCount() const;
    size_t getTotalBlockCount() const;
    
    // 调试
    void dumpLocalBlocks() const;
    void dumpCSSRules() const;
    void dumpScriptHandlers() const;
};

} // namespace CHTL

#endif // CHTL_LOCAL_BLOCK_MANAGER_H