#pragma once

#include "../Util/Common.h"
#include "../CHTL/CHTLLexer/Token.h"
#include "../CHTLJS/CHTLJSLexer/Token.h"

namespace CHTL {

// 代码片段类型
enum class FragmentType {
    CHTL,           // CHTL语法片段
    CHTL_JS,        // CHTL JS语法片段  
    CSS,            // CSS片段
    JAVASCRIPT,     // JavaScript片段
    HTML,           // 原始HTML片段
    UNKNOWN         // 未知类型
};

// 代码片段
struct CodeFragment {
    String content;         // 片段内容
    FragmentType type;      // 片段类型
    size_t startPosition;   // 起始位置
    size_t endPosition;     // 结束位置
    int nestLevel;          // 嵌套层级
    String context;         // 上下文信息
};

// 扫描器配置
struct ScannerConfig {
    bool enableCHTLJSDetection = true;      // 启用CHTL JS检测
    bool enableUnquotedLiterals = true;     // 支持无引号字面量
    bool enableContextAnalysis = true;      // 启用上下文分析
    size_t maxFragmentSize = 10000;         // 最大片段大小
    size_t minFragmentSize = 10;            // 最小片段大小
    bool preserveWhitespace = false;        // 保留空白字符
};

// CHTL统一扫描器 - 精准代码切割器
class CHTLUnifiedScanner {
private:
    String sourceCode;
    size_t currentPosition;
    size_t sourceLength;
    ScannerConfig config;
    StringVector fragmentCache;
    
    // 检测方法
    bool isCHTLSyntax(const String& fragment, size_t position) const;
    bool isCHTLJSSyntax(const String& fragment, size_t position) const;
    bool isCSSFragment(const String& fragment, size_t position) const;
    bool isJavaScriptFragment(const String& fragment, size_t position) const;
    bool isHTMLFragment(const String& fragment, size_t position) const;
    
    // CHTL语法检测
    bool hasCHTLKeywords(const String& fragment) const;
    bool hasCHTLBrackets(const String& fragment) const;
    bool hasCHTLTemplates(const String& fragment) const;
    bool hasCHTLImports(const String& fragment) const;
    bool hasCHTLOrigin(const String& fragment) const;
    
    // CHTL JS语法检测
    bool hasCHTLJSSelectors(const String& fragment) const;
    bool hasCHTLJSOperators(const String& fragment) const;
    bool hasCHTLJSFunctions(const String& fragment) const;
    bool hasCHTLJSModules(const String& fragment) const;
    bool hasCHTLJSVirtualObjects(const String& fragment) const;
    
    // 切割方法
    std::vector<CodeFragment> performPreciseCutting(const String& content) const;
    CodeFragment cutCHTLFragment(const String& content, size_t start, size_t& end) const;
    CodeFragment cutCHTLJSFragment(const String& content, size_t start, size_t& end) const;
    CodeFragment cutCSSFragment(const String& content, size_t start, size_t& end) const;
    CodeFragment cutJSFragment(const String& content, size_t start, size_t& end) const;
    
    // 优化方法
    bool shouldMergeFragments(const CodeFragment& prev, const CodeFragment& current) const;
    CodeFragment mergeFragments(const CodeFragment& first, const CodeFragment& second) const;
    std::vector<CodeFragment> optimizeFragments(const std::vector<CodeFragment>& fragments) const;
    
    // 验证方法
    bool validateFragmentIntegrity(const CodeFragment& fragment) const;
    bool needsExpansion(const String& fragment, size_t position) const;
    size_t calculateExpansionSize(const String& fragment, size_t position) const;
    
    // 上下文分析
    String analyzeContext(const String& content, size_t position) const;
    int calculateNestLevel(const String& content, size_t position) const;
    
public:
    // 构造函数
    explicit CHTLUnifiedScanner(const String& source, const ScannerConfig& cfg = ScannerConfig());
    
    // 主要扫描方法
    std::vector<CodeFragment> scanAndCut();
    std::vector<CodeFragment> scanWithContext();
    
    // 单步扫描
    CodeFragment scanNextFragment();
    bool hasMoreFragments() const;
    void reset();
    
    // 配置管理
    void setConfig(const ScannerConfig& cfg);
    ScannerConfig getConfig() const;
    
    // 工具方法
    FragmentType detectFragmentType(const String& fragment, size_t position = 0) const;
    bool isValidCHTLSyntax(const String& fragment) const;
    bool isValidCHTLJSSyntax(const String& fragment) const;
    
    // 调试方法
    void printFragments(const std::vector<CodeFragment>& fragments) const;
    String fragmentTypeToString(FragmentType type) const;
    
    // 状态查询
    size_t getCurrentPosition() const { return currentPosition; }
    size_t getSourceLength() const { return sourceLength; }
    double getProgress() const { return static_cast<double>(currentPosition) / sourceLength; }
};

// 辅助函数
namespace ScannerUtils {
    // 字符串工具
    bool startsWithCHTLKeyword(const String& str);
    bool containsCHTLJSSelector(const String& str);
    bool isCHTLBracket(char c);
    bool isValidIdentifier(const String& str);
    
    // 位置计算
    std::pair<size_t, size_t> findMatchingBrace(const String& content, size_t start);
    std::pair<size_t, size_t> findStringLiteral(const String& content, size_t start);
    size_t findLineEnd(const String& content, size_t start);
    
    // 语法验证
    bool validateCHTLSyntax(const String& fragment);
    bool validateCHTLJSSyntax(const String& fragment);
    bool validateCSSProperty(const String& fragment);
    bool validateJSStatement(const String& fragment);
}

} // namespace CHTL