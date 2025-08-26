#ifndef CHTLJS_CONTEXT_H
#define CHTLJS_CONTEXT_H

#include "Core/Common.h"
#include "CHTLJS/Token.h"
#include "CHTLJS/GlobalMap.h"
#include "CHTLJS/State.h"
#include <stack>

namespace CHTLJS {

// 前向声明
class CHTLJSCompiler;

// 作用域类型
enum class ScopeType {
    Global,                 // 全局作用域
    Function,               // 函数作用域
    Block,                  // 块作用域
    Object,                 // 对象作用域
    Array,                  // 数组作用域
    EnhancedSelector,       // 增强选择器作用域
    EventHandler,           // 事件处理函数作用域
    Animation,              // 动画定义作用域
    VirtualObject,          // 虚对象作用域
    KeyValuePairs           // 键值对作用域
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

// CHTL JS编译上下文
class CompileContext {
private:
    // 编译器引用
    CHTLJSCompiler* compiler_ = nullptr;
    
    // 文件信息
    String currentFile_;
    String outputFile_;
    
    // 作用域栈
    std::stack<ScopeInfo> scopeStack_;
    
    // 局部符号表
    HashMap<String, SymbolInfo> localSymbols_;
    
    // 当前选择器信息
    struct {
        String selector;            // 当前的选择器
        bool isEnhanced = false;    // 是否是增强选择器
        String compiledSelector;    // 编译后的选择器
        int nestingLevel = 0;       // 嵌套级别
    } currentSelector_;
    
    // 当前事件信息
    struct {
        String eventName;           // 事件名
        String targetSelector;      // 目标选择器
        String handlerName;         // 处理函数名
        bool isDelegate = false;    // 是否是事件委托
        HashMap<String, String> options;  // 事件选项（支持无序键值对）
    } currentEvent_;
    
    // 当前动画信息
    struct {
        String name;                // 动画名
        String targetSelector;      // 目标选择器
        AnimationInfo info;         // 动画详细信息
        bool inFromBlock = false;   // 是否在from块
        bool inToBlock = false;     // 是否在to块
        bool inByBlock = false;     // 是否在by块
    } currentAnimation_;
    
    // 当前虚对象信息
    struct {
        String name;                // 虚对象名
        HashMap<String, String> properties;  // 属性
        HashMap<String, String> methods;     // 方法
        bool isProcessing = false;  // 是否正在处理
    } currentVir_;
    
    // 键值对处理（支持无序和可选）
    struct {
        Vector<String> keys;        // 已定义的键
        HashMap<String, bool> optional;  // 键是否可选
        HashMap<String, String> values;  // 键值对
        bool allowUnordered = true; // 是否允许无序
    } currentKeyValuePairs_;
    
    // 错误和警告
    Vector<String> errors_;
    Vector<String> warnings_;
    
    // 生成的代码
    struct {
        String javascript;          // 生成的JavaScript代码
        String helpers;             // 辅助函数代码
        String initialization;      // 初始化代码
    } generatedCode_;
    
    // 箭头函数转换栈
    Vector<String> arrowFunctionStack_;
    
    // 无修饰字面量处理
    HashSet<String> unquotedLiterals_;
    
public:
    CompileContext() = default;
    explicit CompileContext(CHTLJSCompiler* compiler) : compiler_(compiler) {}
    ~CompileContext() = default;
    
    // 编译器访问
    void SetCompiler(CHTLJSCompiler* compiler) { compiler_ = compiler; }
    CHTLJSCompiler* GetCompiler() const { return compiler_; }
    
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
    
    // 选择器上下文
    void BeginEnhancedSelector(const String& selector);
    void EndEnhancedSelector();
    void SetCompiledSelector(const String& compiled);
    String GetCurrentSelector() const { return currentSelector_.selector; }
    String GetCompiledSelector() const { return currentSelector_.compiledSelector; }
    bool IsInEnhancedSelector() const { return currentSelector_.isEnhanced; }
    
    // 事件上下文
    void BeginEventListener(const String& eventName, const String& selector);
    void EndEventListener();
    void SetEventHandler(const String& handler);
    void SetEventDelegate(bool delegate) { currentEvent_.isDelegate = delegate; }
    void AddEventOption(const String& key, const String& value);
    
    // 动画上下文
    void BeginAnimation(const String& name, const String& selector);
    void EndAnimation();
    void EnterFromBlock() { currentAnimation_.inFromBlock = true; }
    void ExitFromBlock() { currentAnimation_.inFromBlock = false; }
    void EnterToBlock() { currentAnimation_.inToBlock = true; }
    void ExitToBlock() { currentAnimation_.inToBlock = false; }
    void EnterByBlock() { currentAnimation_.inByBlock = true; }
    void ExitByBlock() { currentAnimation_.inByBlock = false; }
    void AddAnimationProperty(const String& property, const String& value);
    void SetAnimationOption(const String& option, const String& value);
    
    // 虚对象上下文
    void BeginVirtualObject(const String& name);
    void EndVirtualObject();
    void AddVirtualProperty(const String& property, const String& value);
    void AddVirtualMethod(const String& method, const String& body);
    
    // 键值对上下文（支持无序和可选）
    void BeginKeyValuePairs(bool allowUnordered = true);
    void EndKeyValuePairs();
    void AddKey(const String& key, bool optional = false);
    void SetKeyValue(const String& key, const String& value);
    bool IsKeyDefined(const String& key) const;
    bool IsKeyOptional(const String& key) const;
    HashMap<String, String> GetKeyValuePairs() const { return currentKeyValuePairs_.values; }
    
    // 箭头函数处理
    void PushArrowFunction(const String& params);
    String PopArrowFunction();
    bool IsInArrowFunction() const { return !arrowFunctionStack_.empty(); }
    
    // 无修饰字面量
    void RegisterUnquotedLiteral(const String& literal);
    bool IsUnquotedLiteral(const String& str) const;
    
    // 错误和警告管理
    void AddError(const String& error);
    void AddWarning(const String& warning);
    Vector<String> GetErrors() const { return errors_; }
    Vector<String> GetWarnings() const { return warnings_; }
    bool HasErrors() const { return !errors_.empty(); }
    
    // 代码生成
    void AppendJavaScript(const String& js);
    void AppendHelper(const String& helper);
    void AppendInitialization(const String& init);
    String GetGeneratedJavaScript() const { return generatedCode_.javascript; }
    String GetHelpers() const { return generatedCode_.helpers; }
    String GetInitialization() const { return generatedCode_.initialization; }
    
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

// 增强选择器守卫
class EnhancedSelectorContextGuard {
private:
    CompileContext& context_;
    
public:
    EnhancedSelectorContextGuard(CompileContext& context, const String& selector)
        : context_(context) {
        context_.BeginEnhancedSelector(selector);
    }
    
    ~EnhancedSelectorContextGuard() {
        context_.EndEnhancedSelector();
    }
};

// 事件监听器守卫
class EventListenerGuard {
private:
    CompileContext& context_;
    
public:
    EventListenerGuard(CompileContext& context, const String& event, const String& selector)
        : context_(context) {
        context_.BeginEventListener(event, selector);
    }
    
    ~EventListenerGuard() {
        context_.EndEventListener();
    }
};

// 动画守卫
class AnimationGuard {
private:
    CompileContext& context_;
    
public:
    AnimationGuard(CompileContext& context, const String& name, const String& selector)
        : context_(context) {
        context_.BeginAnimation(name, selector);
    }
    
    ~AnimationGuard() {
        context_.EndAnimation();
    }
};

// 虚对象守卫
class VirtualObjectGuard {
private:
    CompileContext& context_;
    
public:
    VirtualObjectGuard(CompileContext& context, const String& name)
        : context_(context) {
        context_.BeginVirtualObject(name);
    }
    
    ~VirtualObjectGuard() {
        context_.EndVirtualObject();
    }
};

// 键值对守卫
class KeyValuePairsGuard {
private:
    CompileContext& context_;
    
public:
    KeyValuePairsGuard(CompileContext& context, bool allowUnordered = true)
        : context_(context) {
        context_.BeginKeyValuePairs(allowUnordered);
    }
    
    ~KeyValuePairsGuard() {
        context_.EndKeyValuePairs();
    }
};

} // namespace CHTLJS

#endif // CHTLJS_CONTEXT_H