#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include "Core/Common.h"
#include "CHTL/Token.h"
#include "CHTL/GlobalMap.h"
#include "CHTL/State.h"
#include <stack>

namespace CHTL {

// 前向声明
class CHTLCompiler;

// 作用域类型
enum class ScopeType {
    Global,         // 全局作用域
    Element,        // 元素作用域
    Style,          // 样式作用域
    Script,         // 脚本作用域
    Template,       // 模板作用域
    Custom,         // 自定义作用域
    Origin,         // 原始嵌入作用域
    Configuration,  // 配置作用域
    Namespace,      // 命名空间作用域
    Block           // 块作用域
};

// 作用域信息
struct ScopeInfo {
    ScopeType type;
    String name;
    SourceLocation location;
    HashMap<String, String> metadata;
    
    ScopeInfo(ScopeType t, const String& n = "")
        : type(t), name(n) {}
};

// 编译上下文
class CompileContext {
private:
    // 编译器引用
    CHTLCompiler* compiler_ = nullptr;
    
    // 文件信息
    String currentFile_;
    String outputFile_;
    
    // 作用域栈
    std::stack<ScopeInfo> scopeStack_;
    
    // 局部符号表（作用域内的临时符号）
    HashMap<String, SymbolInfo> localSymbols_;
    
    // 当前元素信息
    struct {
        String tagName;
        HashMap<String, String> attributes;
        Vector<String> classes;
        String id;
        bool hasLocalStyle = false;
        bool hasLocalScript = false;
    } currentElement_;
    
    // 当前样式信息
    struct {
        Vector<String> selectors;
        HashMap<String, String> properties;
        bool isInlineStyle = false;
        String currentSelector;
    } currentStyle_;
    
    // 当前配置信息
    struct {
        String configName;
        HashMap<String, String> values;
        bool isNamedConfig = false;
    } currentConfig_;
    
    // 错误和警告
    Vector<String> errors_;
    Vector<String> warnings_;
    
    // 生成的代码片段
    struct {
        String html;
        String css;
        String javascript;
        HashMap<String, String> additionalFiles;
    } generatedCode_;
    
    // 导入栈（用于检测循环依赖）
    Vector<String> importStack_;
    
    // 自动生成的标识符计数器
    HashMap<String, size_t> autoIdCounters_;
    
public:
    CompileContext() = default;
    explicit CompileContext(CHTLCompiler* compiler) : compiler_(compiler) {}
    ~CompileContext() = default;
    
    // 编译器访问
    void SetCompiler(CHTLCompiler* compiler) { compiler_ = compiler; }
    CHTLCompiler* GetCompiler() const { return compiler_; }
    
    // 文件信息
    void SetCurrentFile(const String& file) { currentFile_ = file; }
    String GetCurrentFile() const { return currentFile_; }
    void SetOutputFile(const String& file) { outputFile_ = file; }
    String GetOutputFile() const { return outputFile_; }
    
    // 作用域管理
    void PushScope(const ScopeInfo& scope);
    void PopScope();
    ScopeInfo GetCurrentScope() const;
    ScopeType GetCurrentScopeType() const;
    bool IsInScope(ScopeType type) const;
    size_t GetScopeDepth() const { return scopeStack_.size(); }
    
    // 局部符号管理
    Result<void> AddLocalSymbol(const SymbolInfo& symbol);
    Optional<SymbolInfo> FindLocalSymbol(const String& name) const;
    void ClearLocalSymbols();
    
    // 元素上下文
    void BeginElement(const String& tagName);
    void EndElement();
    void AddAttribute(const String& name, const String& value);
    void AddClass(const String& className);
    void SetId(const String& id);
    String GetCurrentElementTag() const { return currentElement_.tagName; }
    bool HasLocalStyle() const { return currentElement_.hasLocalStyle; }
    bool HasLocalScript() const { return currentElement_.hasLocalScript; }
    void SetHasLocalStyle(bool value) { currentElement_.hasLocalStyle = value; }
    void SetHasLocalScript(bool value) { currentElement_.hasLocalScript = value; }
    
    // 样式上下文
    void BeginStyle(bool isInline = false);
    void EndStyle();
    void AddSelector(const String& selector);
    void SetCurrentSelector(const String& selector);
    void AddStyleProperty(const String& property, const String& value);
    Vector<String> GetCurrentSelectors() const { return currentStyle_.selectors; }
    bool IsInlineStyle() const { return currentStyle_.isInlineStyle; }
    
    // 配置上下文
    void BeginConfiguration(const String& name = "");
    void EndConfiguration();
    void AddConfigValue(const String& key, const String& value);
    String GetConfigValue(const String& key) const;
    
    // 错误和警告管理
    void AddError(const String& error);
    void AddWarning(const String& warning);
    Vector<String> GetErrors() const { return errors_; }
    Vector<String> GetWarnings() const { return warnings_; }
    bool HasErrors() const { return !errors_.empty(); }
    void ClearErrors() { errors_.clear(); }
    void ClearWarnings() { warnings_.clear(); }
    
    // 代码生成
    void AppendHTML(const String& html);
    void AppendCSS(const String& css);
    void AppendJavaScript(const String& js);
    void AddAdditionalFile(const String& filename, const String& content);
    String GetGeneratedHTML() const { return generatedCode_.html; }
    String GetGeneratedCSS() const { return generatedCode_.css; }
    String GetGeneratedJavaScript() const { return generatedCode_.javascript; }
    HashMap<String, String> GetAdditionalFiles() const { return generatedCode_.additionalFiles; }
    
    // 导入管理
    void PushImport(const String& file);
    void PopImport();
    bool IsInImportStack(const String& file) const;
    Vector<String> GetImportStack() const { return importStack_; }
    
    // 自动标识符生成
    String GenerateAutoId(const String& prefix);
    String GenerateAutoClass(const String& prefix);
    
    // 选择器自动化相关
    bool ShouldAutoAddClass() const;
    bool ShouldAutoAddId() const;
    String GetFirstClassSelector() const;
    String GetFirstIdSelector() const;
    
    // 清理上下文
    void Clear();
    
    // 调试输出
    String DumpContext() const;
};

// 上下文管理器（全局单例）
class ContextManager {
private:
    static UniquePtr<CompileContext> instance_;
    
public:
    static CompileContext& GetInstance();
    static void Reset();
    static void SetInstance(UniquePtr<CompileContext> context);
};

// RAII作用域守卫
class ScopeGuard {
private:
    CompileContext& context_;
    
public:
    ScopeGuard(CompileContext& context, const ScopeInfo& scope)
        : context_(context) {
        context_.PushScope(scope);
    }
    
    ~ScopeGuard() {
        context_.PopScope();
    }
};

// RAII元素守卫
class ElementGuard {
private:
    CompileContext& context_;
    
public:
    ElementGuard(CompileContext& context, const String& tagName)
        : context_(context) {
        context_.BeginElement(tagName);
    }
    
    ~ElementGuard() {
        context_.EndElement();
    }
};

// RAII样式守卫
class StyleGuard {
private:
    CompileContext& context_;
    
public:
    StyleGuard(CompileContext& context, bool isInline = false)
        : context_(context) {
        context_.BeginStyle(isInline);
    }
    
    ~StyleGuard() {
        context_.EndStyle();
    }
};

// RAII导入守卫
class ImportGuard {
private:
    CompileContext& context_;
    
public:
    ImportGuard(CompileContext& context, const String& file)
        : context_(context) {
        context_.PushImport(file);
    }
    
    ~ImportGuard() {
        context_.PopImport();
    }
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H