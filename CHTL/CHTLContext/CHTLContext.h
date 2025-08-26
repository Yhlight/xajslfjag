#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stack>
#include "../CHTLState/CHTLState.h"
#include "../CHTLManage/CHTLManager.h"

namespace CHTL {

// 选择器类型
enum class SelectorType {
    NONE,
    CLASS_SELECTOR,      // .class
    ID_SELECTOR,         // #id
    ELEMENT_SELECTOR,    // element
    REFERENCE_SELECTOR,  // &
    PSEUDO_CLASS,        // :hover
    PSEUDO_ELEMENT      // ::before
};

// 选择器信息
struct SelectorInfo {
    SelectorType type;
    std::string value;
    bool isAutoAdded;    // 是否自动添加
    
    SelectorInfo() : type(SelectorType::NONE), isAutoAdded(false) {}
};

// 变量引用信息
struct VariableReference {
    std::string name;
    std::string namespacePath;
    bool isTemplate;
    bool isCustom;
    bool hasSpecialization;
    std::unordered_map<std::string, std::string> specializations;
};

// 继承信息
struct InheritanceInfo {
    std::string baseType;        // 继承的基类型
    std::string baseName;        // 继承的基名称
    std::vector<std::string> deletedProperties;  // 删除的属性
    std::vector<std::string> deletedInheritances; // 删除的继承
};

// 元素上下文
struct ElementContext {
    std::string elementName;
    std::string elementId;
    std::vector<std::string> elementClasses;
    std::unordered_map<std::string, std::string> attributes;
    bool hasLocalStyle;
    bool hasLocalScript;
    std::vector<SelectorInfo> styleSelectors;
    std::vector<SelectorInfo> scriptSelectors;
};

// CHTL上下文管理器
class CHTLContext {
private:
    std::shared_ptr<CHTLState> state;
    std::shared_ptr<CHTLManager> manager;
    
    // 当前文件信息
    std::string currentFilePath;
    std::string currentFileName;
    std::string currentNamespace;
    
    // 元素栈
    std::stack<ElementContext> elementStack;
    
    // 当前配置缓存
    bool configLoaded;
    
    // 选择器自动化规则
    bool disableStyleAutoAddClass;
    bool disableStyleAutoAddId;
    bool disableScriptAutoAddClass;
    bool disableScriptAutoAddId;
    bool disableDefaultNamespace;
    
    // 继承链
    std::vector<InheritanceInfo> inheritanceChain;
    
    // 当前作用域的符号
    std::unordered_map<std::string, std::shared_ptr<SymbolInfo>> localSymbols;
    
    // 辅助方法
    void loadConfiguration();
    void applyAutoSelector(ElementContext& element);
    std::string resolveReference(const std::string& ref) const;
    
public:
    CHTLContext(std::shared_ptr<CHTLState> state, std::shared_ptr<CHTLManager> manager);
    ~CHTLContext() = default;
    
    // 文件管理
    void setCurrentFile(const std::string& filePath);
    std::string getCurrentFile() const { return currentFilePath; }
    std::string getCurrentFileName() const { return currentFileName; }
    
    // 命名空间管理
    void enterNamespace(const std::string& name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    std::string getFullQualifiedName(const std::string& name) const;
    
    // 元素管理
    void pushElement(const std::string& elementName);
    void popElement();
    ElementContext& getCurrentElement();
    const ElementContext& getCurrentElement() const;
    bool hasCurrentElement() const;
    
    // 属性管理
    void addAttribute(const std::string& name, const std::string& value);
    void addClass(const std::string& className);
    void setId(const std::string& id);
    
    // 选择器管理
    void addStyleSelector(const SelectorInfo& selector);
    void addScriptSelector(const SelectorInfo& selector);
    std::vector<SelectorInfo> getStyleSelectors() const;
    std::vector<SelectorInfo> getScriptSelectors() const;
    
    // 自动选择器
    void applyAutoSelectors();
    bool shouldAutoAddClass() const;
    bool shouldAutoAddId() const;
    std::string getFirstClassSelector() const;
    std::string getFirstIdSelector() const;
    
    // 引用解析
    std::string resolveAmpersandReference() const;
    bool hasClassAttribute() const;
    bool hasIdAttribute() const;
    
    // 变量管理
    void addVariableReference(const VariableReference& varRef);
    bool resolveVariable(const std::string& name, std::string& value) const;
    
    // 继承管理
    void addInheritance(const InheritanceInfo& info);
    void removeInheritance(const std::string& baseName);
    std::vector<InheritanceInfo> getInheritanceChain() const;
    bool hasInheritance(const std::string& baseName) const;
    
    // 符号管理
    void addLocalSymbol(std::shared_ptr<SymbolInfo> symbol);
    std::shared_ptr<SymbolInfo> findLocalSymbol(const std::string& name) const;
    void clearLocalSymbols();
    
    // 配置访问
    bool isConfigurationLoaded() const { return configLoaded; }
    std::string getConfigOption(const std::string& key) const;
    std::vector<std::string> getNameGroupOption(const std::string& key) const;
    
    // 约束检查（委托给state）
    bool isAllowedInCurrentContext(const std::string& element) const;
    bool canUseTemplateVariable() const;
    bool canUseCustomVariable() const;
    bool canUseStyleGroup() const;
    bool canUseFrom() const;
    
    // 状态查询（委托给state）
    bool isInGlobalStyle() const;
    bool isInLocalStyle() const;
    bool isInGlobalScript() const;
    bool isInLocalScript() const;
    
    // 调试
    void dumpContext() const;
    void dumpElementStack() const;
    void dumpInheritanceChain() const;
};

// RAII上下文管理器
class ContextGuard {
private:
    CHTLContext* context;
    std::string savedNamespace;
    bool namespaceChanged;
    bool elementPushed;
    
public:
    ContextGuard(CHTLContext* ctx) : context(ctx), namespaceChanged(false), elementPushed(false) {}
    
    ~ContextGuard() {
        if (elementPushed && context) {
            context->popElement();
        }
        if (namespaceChanged && context) {
            context->exitNamespace();
        }
    }
    
    void pushElement(const std::string& name) {
        if (context) {
            context->pushElement(name);
            elementPushed = true;
        }
    }
    
    void enterNamespace(const std::string& name) {
        if (context) {
            savedNamespace = context->getCurrentNamespace();
            context->enterNamespace(name);
            namespaceChanged = true;
        }
    }
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H