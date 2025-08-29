#pragma once

#include "../Util/Common.h"
#include "Syntax.h"

using namespace CHTL;

namespace CJMOD {

// 生成器配置
struct GeneratorConfig {
    bool enableOptimization = true;        // 启用优化
    bool generateComments = false;         // 生成注释
    bool enableMinification = false;       // 启用压缩
    String indentString = "  ";            // 缩进字符串
    String newlineString = "\n";           // 换行字符串
    bool preserveWhitespace = false;       // 保留空白字符
    bool generateSourceMap = false;        // 生成源映射
};

// 生成结果
struct GenerationResult {
    bool success = false;                  // 是否成功
    String generatedCode;                  // 生成的代码
    StringVector errors;                   // 错误列表
    StringVector warnings;                 // 警告列表
    StringUnorderedMap metadata;           // 元数据
    
    void addError(const String& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const String& warning) {
        warnings.push_back(warning);
    }
    
    void clear() {
        success = false;
        generatedCode.clear();
        errors.clear();
        warnings.clear();
        metadata.clear();
    }
};

// CJMOD代码生成器
class CJMODGenerator {
private:
    GeneratorConfig config;
    StringUnorderedMap templateMap;        // 模板映射
    StringVector generatedFunctions;       // 已生成的函数
    int functionCounter = 0;               // 函数计数器
    
    // 生成方法
    String generateFunction(const Arg& args, const String& keyword);
    String generateParameterList(const Arg& args);
    String generateFunctionBody(const Arg& args, const String& keyword);
    String generateAtomArg(const AtomArg& atom);
    
    // 优化方法
    String optimizeCode(const String& code);
    String removeRedundantCode(const String& code);
    String combineVariables(const String& code);
    
    // 辅助方法
    String generateUniqueFunctionName(const String& keyword);
    String sanitizeIdentifier(const String& identifier);
    bool isValidIdentifier(const String& identifier);
    
public:
    // 构造函数
    explicit CJMODGenerator(const GeneratorConfig& cfg = GeneratorConfig());
    
    // 主要生成方法
    GenerationResult generate(const Arg& args, const String& keyword);
    GenerationResult generateFromSyntax(const String& syntaxString);
    GenerationResult generateMultiple(const std::vector<Arg>& argsList, const StringVector& keywords);
    
    // 导出方法
    static GenerationResult exportResult(const Arg& args);
    static String exportJavaScript(const Arg& args, const String& keyword);
    static String exportCppFunction(const Arg& args, const String& keyword);
    
    // 模板系统
    void addTemplate(const String& name, const String& template_);
    void removeTemplate(const String& name);
    String applyTemplate(const String& templateName, const StringUnorderedMap& variables);
    
    // 配置管理
    void setConfig(const GeneratorConfig& cfg) { config = cfg; }
    GeneratorConfig getConfig() const { return config; }
    
    // 统计信息
    String getGenerationStats() const;
    void resetStats();
    
    // 调试方法
    void printGenerationInfo(const GenerationResult& result) const;
};

// CJMOD双指针扫描器
class CJMODScanner {
private:
    String sourceCode;                     // 源代码
    size_t frontPointer = 0;              // 前指针
    size_t backPointer = 0;               // 后指针
    StringVector keywords;                 // 关键字列表
    bool enablePreTruncation = true;       // 启用前置截取
    
    // 扫描方法
    bool findKeyword(const String& keyword, size_t& position);
    String extractFragment(size_t start, size_t end);
    bool needsExpansion(const String& fragment, const String& keyword);
    String expandFragment(const String& fragment, const String& keyword);
    
    // 双指针算法
    void initializePointers();
    void movePointers();
    bool synchronizePointers();
    
    // 前置截取
    String preTruncate(const String& fragment, const String& keyword);
    bool shouldTruncate(const String& fragment, const String& keyword);
    
public:
    // 构造函数
    explicit CJMODScanner(const String& source);
    
    // 主要扫描方法
    static Arg scan(const Arg& args, const String& keyword);
    static Arg scanWithKeywords(const String& source, const StringVector& keywords);
    static StringVector findAllKeywords(const String& source);
    
    // 配置方法
    void setEnablePreTruncation(bool enable) { enablePreTruncation = enable; }
    void addKeyword(const String& keyword);
    void setKeywords(const StringVector& kws) { keywords = kws; }
    
    // 扫描接口
    Arg scanFragment(const String& keyword);
    StringVector scanMultiple(const StringVector& keywords);
    
    // 调试方法
    void printScanProgress() const;
    String getScannerStatus() const;
};

// CJMOD函数创建器
class CJMODFunctionCreator {
private:
    StringUnorderedMap functionTemplates;   // 函数模板
    StringVector supportedKeywords;         // 支持的关键字
    
public:
    // 创建方法
    String createFunction(const String& keyword, const Arg& args);
    String createAdvancedFunction(const String& keyword, const Arg& args, const StringUnorderedMap& options);
    
    // 模板管理
    void addFunctionTemplate(const String& keyword, const String& template_);
    String getFunctionTemplate(const String& keyword) const;
    
    // 支持的功能
    StringVector getSupportedKeywords() const { return supportedKeywords; }
    bool supportsKeyword(const String& keyword) const;
    
    // 预定义函数
    void setupDefaultFunctions();
    void addListenFunction();
    void addDelegateFunction();
    void addAnimateFunction();
    void addModuleFunction();
};

// CJMOD虚对象管理器
class VirtualObjectManager {
private:
    StringUnorderedMap virtualObjects;     // 虚对象映射
    StringUnorderedMap objectProperties;   // 对象属性
    
public:
    // 虚对象管理
    void registerVirtualObject(const String& name, const String& definition);
    void removeVirtualObject(const String& name);
    bool hasVirtualObject(const String& name) const;
    
    // 属性管理
    void setObjectProperty(const String& objectName, const String& propertyName, const String& value);
    String getObjectProperty(const String& objectName, const String& propertyName) const;
    
    // 代码生成
    String generateVirtualObjectCode(const String& name) const;
    String generateAccessCode(const String& objectName, const String& propertyName) const;
    
    // 绑定管理
    static void bindVirtualObject(const String& functionName);
    static StringVector getBoundObjects();
};

// CJMOD工具函数
namespace CJMODUtils {
    // 语法验证
    bool validateSyntax(const String& syntax);
    bool validateKeyword(const String& keyword);
    bool validatePlaceholder(const String& placeholder);
    
    // 代码转换
    String convertToJavaScript(const String& cjmodCode);
    String convertToCpp(const String& cjmodCode);
    String optimizeCJMODCode(const String& code);
    
    // 占位符处理
    StringVector extractPlaceholders(const String& syntax);
    String replacePlaceholders(const String& template_, const StringUnorderedMap& values);
    bool isValidPlaceholderType(const String& type);
    
    // 关键字分析
    StringVector findKeywords(const String& code);
    String getKeywordType(const String& keyword);
    bool isReservedKeyword(const String& keyword);
    
    // 调试工具
    String formatCJMODCode(const String& code);
    String analyzeCJMODSyntax(const String& syntax);
    void printCJMODInfo(const Arg& args);
}

} // namespace CJMOD