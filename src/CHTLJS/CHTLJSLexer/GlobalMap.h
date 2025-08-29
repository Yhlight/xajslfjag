#pragma once

#include "../../Util/Common.h"
#include "Token.h"
#include <unordered_map>
#include <vector>
#include <functional>

namespace CHTL {
namespace JS {

// CHTL JS关键字映射类
class CHTLJSKeywordMap {
public:
    static const std::unordered_map<String, String>& getKeywordMap();
    static bool isKeyword(const String& word);
    static CHTLJSTokenType getKeywordType(const String& word);
    static StringVector getAllKeywords();
    
    // CHTL JS特有功能
    static bool isCHTLJSKeyword(const String& word);
    static bool isJavaScriptKeyword(const String& word);
    static bool isCJMODKeyword(const String& word);
    
private:
    static std::unordered_map<String, String> keywordMap;
    static void initializeKeywordMap();
};

// CHTL JS操作符映射类
class CHTLJSOperatorMap {
public:
    static const std::unordered_map<String, String>& getOperatorMap();
    static bool isOperator(const String& op);
    static CHTLJSTokenType getOperatorType(const String& op);
    static StringVector getAllOperators();
    
    // 操作符优先级
    static int getOperatorPrecedence(const String& op);
    static bool isLeftAssociative(const String& op);
    static bool isRightAssociative(const String& op);
    
    // CHTL JS特殊操作符
    static bool isCHTLJSOperator(const String& op);
    static bool isEventBindingOperator(const String& op);
    static bool isSelectorOperator(const String& op);
    
private:
    static std::unordered_map<String, String> operatorMap;
    static std::unordered_map<String, int> precedenceMap;
    static std::unordered_map<String, bool> associativityMap;
    static void initializeOperatorMap();
    static void initializePrecedenceMap();
};

// CHTL JS选择器映射类
class CHTLJSSelectorMap {
public:
    struct SelectorInfo {
        String pattern;
        String jsEquivalent;
        bool requiresContext;
        StringVector dependencies;
    };
    
    static const std::unordered_map<String, SelectorInfo>& getSelectorMap();
    static bool isValidSelector(const String& selector);
    static String translateToJS(const String& selector, const String& context = "");
    static StringVector extractSelectors(const String& source);
    
    // CSS选择器验证
    static bool isValidCSSSelector(const String& selector);
    static String optimizeSelector(const String& selector);
    
private:
    static std::unordered_map<String, SelectorInfo> selectorMap;
    static void initializeSelectorMap();
};

// CHTL JS函数映射类
class CHTLJSFunctionMap {
public:
    struct FunctionInfo {
        String name;
        StringVector parameters;
        String returnType;
        String jsImplementation;
        bool isAsync;
        bool requiresDOM;
    };
    
    static const std::unordered_map<String, FunctionInfo>& getFunctionMap();
    static bool isBuiltinFunction(const String& name);
    static FunctionInfo getFunctionInfo(const String& name);
    static String generateJSCode(const String& name, const StringVector& args);
    
    // 函数类型检查
    static bool isListenFunction(const String& name);
    static bool isDelegateFunction(const String& name);
    static bool isAnimateFunction(const String& name);
    static bool isVirFunction(const String& name);
    
private:
    static std::unordered_map<String, FunctionInfo> functionMap;
    static void initializeFunctionMap();
};

// CHTL JS事件映射类
class CHTLJSEventMap {
public:
    struct EventInfo {
        String name;
        StringVector parameters;
        bool isBubbling;
        bool isCancelable;
        String defaultAction;
    };
    
    static const std::unordered_map<String, EventInfo>& getEventMap();
    static bool isValidEvent(const String& eventName);
    static EventInfo getEventInfo(const String& eventName);
    static String generateEventListener(const String& event, const String& handler);
    
    // 事件委托支持
    static String generateDelegateListener(const String& parent, const String& target, 
                                         const String& event, const String& handler);
    
private:
    static std::unordered_map<String, EventInfo> eventMap;
    static void initializeEventMap();
};

// CHTL JS模块映射类
class CHTLJSModuleMap {
public:
    struct ModuleInfo {
        String name;
        String path;
        StringVector exports;
        StringVector dependencies;
        bool isAsync;
        String loadStrategy;
    };
    
    static const std::unordered_map<String, ModuleInfo>& getModuleMap();
    static bool isRegisteredModule(const String& name);
    static ModuleInfo getModuleInfo(const String& name);
    static void registerModule(const String& name, const ModuleInfo& info);
    
    // AMD风格模块加载
    static String generateAMDLoader(const String& moduleName, const StringVector& dependencies);
    static String generateModuleWrapper(const String& content, const StringVector& exports);
    
private:
    static std::unordered_map<String, ModuleInfo> moduleMap;
    static void initializeModuleMap();
};

// CHTL JS全局映射管理器
class CHTLJSGlobalMap {
public:
    // 初始化所有映射表
    static void initialize();
    static bool isInitialized();
    
    // 统一查询接口
    static bool isReservedWord(const String& word);
    static bool isBuiltinFunction(const String& name);
    static bool isValidOperator(const String& op);
    static bool isValidSelector(const String& selector);
    
    // 代码生成辅助
    static String translateCHTLJSToJS(const String& source);
    static StringVector extractDependencies(const String& source);
    static String optimizeGeneratedCode(const String& jsCode);
    
    // 错误检查
    static StringVector validateCHTLJSSyntax(const String& source);
    static bool hasConflicts(const String& source);
    
private:
    static bool initialized;
    static void initializeAll();
};

} // namespace JS
} // namespace CHTL