#ifndef CJMOD_RUNTIME_H
#define CJMOD_RUNTIME_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "../CJMODPackager.h"

namespace CHTL {

// 前置声明
class CJMODExtension;
class CJMODRuntimeContext;

// 语法定义（JSON解析后的结构）
struct SyntaxPattern {
    std::string name;                          // 语法名称
    std::string regex;                         // 正则表达式
    std::vector<std::string> captureGroups;    // 捕获组名称
    std::string processor;                     // 处理器函数名
    std::map<std::string, std::string> options;// 额外选项
};

// 处理结果
struct ProcessResult {
    bool success = false;
    std::string generatedCode;     // 生成的JavaScript代码
    std::string errorMessage;      // 错误信息
    std::vector<std::string> dependencies;  // 需要的依赖
    std::map<std::string, std::string> metadata;  // 元数据
};

// CJMOD扩展接口（C++扩展需要实现）
class CJMODExtension {
public:
    virtual ~CJMODExtension() = default;
    
    // 获取扩展信息
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // 初始化（加载时调用一次）
    virtual bool initialize(CJMODRuntimeContext* context) = 0;
    
    // 处理匹配的代码片段
    virtual ProcessResult process(
        const std::string& syntaxName,
        const std::string& matchedText,
        const std::map<std::string, std::string>& captures
    ) = 0;
    
    // 清理
    virtual void cleanup() {}
};

// 运行时上下文
class CJMODRuntimeContext {
public:
    // 获取编译器版本
    std::string getCompilerVersion() const;
    
    // 获取当前文件信息
    std::string getCurrentFile() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 日志
    void log(const std::string& message);
    void logError(const std::string& error);
    void logWarning(const std::string& warning);
    
    // 获取配置
    std::string getConfig(const std::string& key) const;
    
    // 缓存管理
    void setCacheValue(const std::string& key, const std::string& value);
    std::string getCacheValue(const std::string& key) const;
    bool hasCacheValue(const std::string& key) const;
};

// CJMOD运行时管理器
class CJMODRuntime {
public:
    static CJMODRuntime& getInstance() {
        static CJMODRuntime instance;
        return instance;
    }
    
    // 加载CJMOD
    bool loadModule(const std::string& path);
    bool loadModule(const CJMODInfo& info, const std::string& extensionPath);
    
    // 获取所有语法模式
    std::vector<SyntaxPattern> getAllSyntaxPatterns() const;
    
    // 处理代码片段
    ProcessResult processFragment(
        const std::string& moduleName,
        const std::string& syntaxName,
        const std::string& fragment,
        const std::map<std::string, std::string>& captures
    );
    
    // 卸载模块
    void unloadModule(const std::string& moduleName);
    void unloadAll();
    
    // 设置运行时上下文
    void setContext(std::shared_ptr<CJMODRuntimeContext> context) {
        context_ = context;
    }
    
private:
    CJMODRuntime() = default;
    ~CJMODRuntime();
    
    CJMODRuntime(const CJMODRuntime&) = delete;
    CJMODRuntime& operator=(const CJMODRuntime&) = delete;
    
    struct LoadedModule {
        CJMODInfo info;
        void* handle = nullptr;  // 动态库句柄
        std::unique_ptr<CJMODExtension> extension;
        std::vector<SyntaxPattern> syntaxPatterns;
    };
    
    std::map<std::string, LoadedModule> modules_;
    std::shared_ptr<CJMODRuntimeContext> context_;
    
    // 加载动态库
    void* loadDynamicLibrary(const std::string& path);
    void unloadDynamicLibrary(void* handle);
    
    // 创建扩展实例
    std::unique_ptr<CJMODExtension> createExtension(void* handle);
    
    // 解析语法定义
    std::vector<SyntaxPattern> parseSyntaxDefinitions(
        const std::map<std::string, std::string>& definitions
    );
};

// 导出宏（用于扩展DLL）
#ifdef _WIN32
    #define CJMOD_EXPORT extern "C" __declspec(dllexport)
#else
    #define CJMOD_EXPORT extern "C" __attribute__((visibility("default")))
#endif

// 扩展必须实现的工厂函数
typedef CJMODExtension* (*CreateExtensionFunc)();
typedef void (*DestroyExtensionFunc)(CJMODExtension*);

// 标准导出函数名
#define CJMOD_CREATE_EXTENSION_FUNC "cjmod_create_extension"
#define CJMOD_DESTROY_EXTENSION_FUNC "cjmod_destroy_extension"

} // namespace CHTL

#endif // CJMOD_RUNTIME_H