#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {

// AtomArg占位符类型
enum class AtomArgType {
    PLACEHOLDER,        // $
    OPTIONAL,          // $?
    REQUIRED,          // $!
    UNORDERED,         // $_
    VARIADIC,          // ...
    OPTIONAL_UNORDERED, // $?_
    REQUIRED_UNORDERED  // $!_
};

class AtomArg {
public:
    AtomArgType type;
    std::string value;
    std::function<std::string(const std::string&)> bindFunction;
    
    AtomArg(AtomArgType t = AtomArgType::PLACEHOLDER) : type(t) {}
    
    // 绑定获取值的函数
    void bind(std::function<std::string(const std::string&)> func);
    
    // 填充参数值
    void fillValue(const std::string& val);
    void fillValue(int val);
    void fillValue(double val);
    
    // 获取处理后的值
    std::string getProcessedValue() const;
    
    // 类型检查
    bool isOptional() const;
    bool isRequired() const;
    bool isUnordered() const;
    bool isVariadic() const;
};

class Arg {
public:
    std::vector<AtomArg> args;
    std::string transformedResult;
    
    Arg() = default;
    Arg(const std::vector<std::string>& values);
    
    // 索引访问
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    
    // 绑定函数到特定占位符
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    
    // 填充所有参数值
    void fillValue(const Arg& result);
    
    // 转换为最终JS代码
    void transform(const std::string& jsCode);
    
    // 调试输出
    void print() const;
    
    // 获取结果
    std::string getResult() const { return transformedResult; }
    size_t size() const { return args.size(); }
    bool empty() const { return args.empty(); }
};

class CJMODValue {
public:
    std::string content;
    std::string type;
    
    CJMODValue() = default;
    CJMODValue(const std::string& content, const std::string& type = "string");
    
    bool isObject() const;
    bool isFunction() const;
    bool isArray() const;
    bool isCHTLJSFunction() const;
    
    std::string toString() const { return content; }
};

// CJMOD扫描器 - 实现双指针扫描和前置截取
class CJMODScanner {
public:
    CJMODScanner();
    ~CJMODScanner();
    
    // 主扫描接口 - 阻塞式扫描填充过程
    static Arg scan(const Arg& args, const std::string& keyword = "");
    static Arg scan(const std::string& code, const std::string& keyword);
    
    // 双指针扫描机制
    bool dualPointerScan(const std::string& code);
    void initializePointers();
    std::string getCurrentWindow(const std::string& code) const;
    bool advancePointers(const std::string& code);
    
    // 前置截取机制
    bool prefixCutScan(const std::string& code, const std::string& keyword);
    std::string extractPrefixFragment(const std::string& code, size_t keywordPos) const;
    bool isValidCJMODFragment(const std::string& fragment) const;
    
    // 关键字检测
    bool hasKeywordInWindow(const std::string& window, const std::string& keyword) const;
    std::vector<std::string> getCJMODKeywords() const;
    
    // 滑动窗口算法
    bool slidingWindowScan(const std::string& code, const std::string& keyword);
    
    // 获取扫描结果
    std::vector<std::string> getFragmentBuffer() const { return m_fragmentBuffer; }
    std::vector<std::string> getKeywordBuffer() const { return m_keywordBuffer; }
    
private:
    size_t m_frontPointer;
    size_t m_backPointer;
    size_t m_windowSize;
    std::vector<std::string> m_fragmentBuffer;
    std::vector<std::string> m_keywordBuffer;
    
    void reportError(const std::string& message) const;
};

class CJMODGenerator {
public:
    CJMODGenerator();
    ~CJMODGenerator();
    
    // 导出最终JS代码
    static void exportResult(const Arg& args);
    static std::string generateJSCode(const Arg& args);
    
    // 代码优化
    static std::string optimizeJSCode(const std::string& jsCode);
    static std::string minifyJSCode(const std::string& jsCode);
    
private:
    static void reportError(const std::string& message);
};

// 语法分析器
class Syntax {
public:
    // 语法分析
    static Arg analyze(const std::string& syntaxPattern);
    
    // 类型判断
    static bool isObject(const std::string& code);
    static bool isFunction(const std::string& code);
    static bool isArray(const std::string& code);
    static bool isCHTLJSFunction(const std::string& code);
    
    // 占位符解析
    static AtomArgType parseAtomArgType(const std::string& placeholder);
    static std::vector<std::string> tokenizeSyntax(const std::string& syntaxPattern);
    
private:
    static void reportError(const std::string& message);
};

// CHTL JS函数支持
class CHTLJSFunction {
public:
    CHTLJSFunction();
    ~CHTLJSFunction();
    
    // 快速创建CHTL JS函数
    static bool CreateCHTLJSFunction(const std::string& functionSignature);
    
    // 虚对象绑定
    static bool bindVirtualObject(const std::string& functionName);
    
    // 函数注册和管理
    static bool registerFunction(const std::string& name, const std::string& signature);
    static bool unregisterFunction(const std::string& name);
    static std::vector<std::string> getRegisteredFunctions();
    
    // 虚对象支持检查
    static bool hasVirtualObjectSupport(const std::string& functionName);
    
private:
    static std::unordered_map<std::string, std::string> s_registeredFunctions;
    static std::unordered_map<std::string, bool> s_virtualObjectSupport;
    
    static void reportError(const std::string& message);
    static bool validateFunctionSignature(const std::string& signature);
};

// CJMOD信息结构
struct CJMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
    std::vector<std::string> dependencies;
    std::vector<std::string> exportedFunctions;
};

class CJMODModule {
public:
    CJMODModule() = default;
    ~CJMODModule() = default;
    
    // 基本信息
    std::string getName() const { return m_info.name; }
    std::string getVersion() const { return m_info.version; }
    CJMODInfo getInfo() const { return m_info; }
    
    // C++源码管理
    std::vector<std::string> getCppFiles() const { return m_cppFiles; }
    std::vector<std::string> getHeaderFiles() const { return m_headerFiles; }
    std::string getCppContent(const std::string& fileName) const;
    std::string getHeaderContent(const std::string& fileName) const;
    
    // 子模块支持
    std::vector<std::string> getSubModules() const { return m_subModules; }
    bool hasSubModule(const std::string& subModuleName) const;
    std::shared_ptr<CJMODModule> getSubModule(const std::string& subModuleName) const;
    
    // 导出函数管理
    std::vector<std::string> getExportedFunctions() const { return m_info.exportedFunctions; }
    bool hasExportedFunction(const std::string& functionName) const;
    
    // 模块加载
    bool loadFromPath(const std::string& modulePath);
    bool validateStructure() const;
    bool compileCppSources();
    
private:
    CJMODInfo m_info;
    std::string m_modulePath;
    std::vector<std::string> m_cppFiles;
    std::vector<std::string> m_headerFiles;
    std::vector<std::string> m_subModules;
    std::unordered_map<std::string, std::shared_ptr<CJMODModule>> m_subModuleMap;
    
    bool parseInfoFile(const std::string& infoPath);
    bool loadCppSources();
    bool loadSubModules();
    void reportError(const std::string& message) const;
};

class CJMODApi {
public:
    CJMODApi();
    ~CJMODApi();
    
    // 模块管理
    bool loadCJMOD(const std::string& modulePath);
    bool unloadCJMOD(const std::string& moduleName);
    std::shared_ptr<CJMODModule> getCJMOD(const std::string& moduleName);
    std::vector<std::string> getLoadedCJMODs() const;
    
    // 模块查询
    bool hasCJMOD(const std::string& moduleName) const;
    CJMODInfo getCJMODInfo(const std::string& moduleName) const;
    
    // 子模块访问
    std::shared_ptr<CJMODModule> getSubModule(const std::string& parentModule, const std::string& subModule);
    bool hasSubModule(const std::string& parentModule, const std::string& subModule) const;
    
    // 编译集成
    bool compileCJMOD(const std::string& moduleName);
    bool compileAllCJMODs();
    
    // 打包和解包
    bool packCJMOD(const std::string& sourceDir, const std::string& outputPath);
    bool unpackCJMOD(const std::string& cjmodPath, const std::string& outputDir);
    
    // 验证
    bool validateCJMODStructure(const std::string& modulePath) const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<CJMODModule>> m_loadedModules;
    
    void reportError(const std::string& message) const;
    bool fileExists(const std::string& path) const;
    bool directoryExists(const std::string& path) const;
};

}