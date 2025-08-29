#pragma once

#include "../Util/Common.h"
#include <memory>

namespace CHTL {
namespace CSS {

// CSS编译器配置
struct CSSCompilerConfig {
    bool minifyOutput = false;
    bool preserveComments = false;
    bool enableAutoprefixer = true;
    bool enableVariables = true;
    bool enableNesting = true;
    bool enableImports = true;
    bool strictMode = false;
    String indentString = "  ";
    String newlineString = "\n";
};

// CSS编译结果
struct CSSCompileResult {
    String css;
    StringVector errors;
    StringVector warnings;
    bool success;
    
    CSSCompileResult() : success(false) {}
};

// CSS编译器主类
class CSSCompiler {
public:
    explicit CSSCompiler(const CSSCompilerConfig& config = CSSCompilerConfig{});
    
    // 主要编译接口
    CSSCompileResult compile(const String& source);
    CSSCompileResult compileFile(const String& filepath);
    String compileToString(const String& source);
    
    // 配置管理
    void setConfig(const CSSCompilerConfig& config);
    const CSSCompilerConfig& getConfig() const;
    
    // 错误处理
    const StringVector& getErrors() const;
    const StringVector& getWarnings() const;
    bool hasErrors() const;
    String getErrorSummary() const;
    void clearErrors();
    
private:
    CSSCompilerConfig config;
    StringVector errors;
    StringVector warnings;
    
    // 编译步骤
    String preprocessCSS(const String& source);
    String processImports(const String& source);
    String processVariables(const String& source);
    String processNesting(const String& source);
    String processAutoprefixer(const String& source);
    String postprocessCSS(const String& source);
    String minifyCSS(const String& source);
    
    // 错误处理
    void addError(const String& message);
    void addWarning(const String& message);
};

// CSS编译器工厂
class CSSCompilerFactory {
public:
    static std::unique_ptr<CSSCompiler> createCompiler(const CSSCompilerConfig& config = CSSCompilerConfig{});
    static std::unique_ptr<CSSCompiler> createMinifyingCompiler();
    static std::unique_ptr<CSSCompiler> createDevelopmentCompiler();
    static std::unique_ptr<CSSCompiler> createProductionCompiler();
    
    // 配置预设
    static CSSCompilerConfig getDefaultConfig();
    static CSSCompilerConfig getMinifyConfig();
    static CSSCompilerConfig getDevelopmentConfig();
    static CSSCompilerConfig getProductionConfig();
};

} // namespace CSS
} // namespace CHTL