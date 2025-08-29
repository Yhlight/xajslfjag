#pragma once

#include "../Util/Common.h"
#include <memory>

namespace CHTL {
namespace JS {

// JavaScript编译器配置
struct JSCompilerConfig {
    bool minifyOutput = false;
    bool preserveComments = false;
    bool enableSourceMaps = false;
    bool enableOptimization = true;
    bool strictMode = true;
    bool enableModules = true;
    bool enableAsync = true;
    String targetVersion = "ES2015";  // ES5, ES2015, ES2017, etc.
    String moduleFormat = "AMD";      // AMD, CommonJS, ES6, UMD
    String indentString = "  ";
    String newlineString = "\n";
};

// JavaScript编译结果
struct JSCompileResult {
    String javascript;
    String sourceMap;
    StringVector errors;
    StringVector warnings;
    bool success;
    
    JSCompileResult() : success(false) {}
};

// JavaScript编译器主类
class JSCompiler {
public:
    explicit JSCompiler(const JSCompilerConfig& config = JSCompilerConfig{});
    
    // 主要编译接口
    JSCompileResult compile(const String& source);
    JSCompileResult compileFile(const String& filepath);
    String compileToString(const String& source);
    
    // 验证接口
    bool validateSyntax(const String& source);
    StringVector checkSyntax(const String& source);
    
    // 配置管理
    void setConfig(const JSCompilerConfig& config);
    const JSCompilerConfig& getConfig() const;
    
    // 错误处理
    const StringVector& getErrors() const;
    const StringVector& getWarnings() const;
    bool hasErrors() const;
    String getErrorSummary() const;
    void clearErrors();
    
private:
    JSCompilerConfig config;
    StringVector errors;
    StringVector warnings;
    
    // 编译步骤
    String preprocessJS(const String& source);
    String transpileES6(const String& source);
    String processModules(const String& source);
    String optimizeCode(const String& source);
    String postprocessJS(const String& source);
    String minifyJS(const String& source);
    String generateSourceMap(const String& original, const String& compiled);
    
    // 语法检查
    bool isValidJavaScript(const String& source);
    StringVector findSyntaxErrors(const String& source);
    
    // 转换方法
    String convertArrowFunctions(const String& source);
    String convertLetConst(const String& source);
    String convertTemplateStrings(const String& source);
    String convertClasses(const String& source);
    String convertModuleImports(const String& source);
    String convertAsyncAwait(const String& source);
    
    // 优化方法
    String deadCodeElimination(const String& source);
    String constantFolding(const String& source);
    String variableRenaming(const String& source);
    
    // 错误处理
    void addError(const String& message);
    void addWarning(const String& message);
};

// JavaScript编译器工厂
class JSCompilerFactory {
public:
    static std::unique_ptr<JSCompiler> createCompiler(const JSCompilerConfig& config = JSCompilerConfig{});
    static std::unique_ptr<JSCompiler> createES5Compiler();
    static std::unique_ptr<JSCompiler> createES6Compiler();
    static std::unique_ptr<JSCompiler> createMinifyingCompiler();
    static std::unique_ptr<JSCompiler> createDevelopmentCompiler();
    static std::unique_ptr<JSCompiler> createProductionCompiler();
    
    // 配置预设
    static JSCompilerConfig getDefaultConfig();
    static JSCompilerConfig getES5Config();
    static JSCompilerConfig getES6Config();
    static JSCompilerConfig getMinifyConfig();
    static JSCompilerConfig getDevelopmentConfig();
    static JSCompilerConfig getProductionConfig();
};

} // namespace JS
} // namespace CHTL