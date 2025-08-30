#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

enum class FragmentType {
    CHTL,
    CHTL_JS,
    CSS,
    JAVASCRIPT,
    HTML,
    COMMENT_GENERATOR,  // --注释
    COMMENT_NORMAL,     // //和/**/注释
    UNKNOWN
};

struct CodeFragment {
    std::string content;
    FragmentType type;
    size_t startPos;
    size_t endPos;
    int lineNumber;
    std::string context;
    bool isMinimalUnit;
    bool needsForwardExpansion;
};

struct ScanContext {
    std::string currentNamespace;
    std::vector<std::string> openBlocks;
    bool inStyleBlock;
    bool inScriptBlock;
    bool inOriginBlock;
    bool inTemplateBlock;
    bool inCustomBlock;
    std::string originType;
    int braceDepth;
    int parenDepth;
};

class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner();
    
    // 主扫描接口
    std::vector<CodeFragment> scanCode(const std::string& code);
    
    // 可变长度切片扫描
    std::vector<CodeFragment> variableLengthSlicing(const std::string& code);
    bool needsForwardExpansion(const CodeFragment& fragment, const std::string& fullCode);
    CodeFragment expandFragment(const CodeFragment& fragment, const std::string& fullCode);
    
    // 最小单元切割
    std::vector<CodeFragment> minimalUnitCutting(const CodeFragment& fragment);
    bool isMinimalUnit(const std::string& code, FragmentType type);
    std::vector<std::string> splitToMinimalUnits(const std::string& code, FragmentType type);
    
    // 上下文连续性保持
    bool shouldMaintainContinuity(const std::vector<CodeFragment>& fragments, size_t index);
    std::vector<CodeFragment> mergeContinuousFragments(const std::vector<CodeFragment>& fragments);
    
    // 片段类型检测
    FragmentType detectFragmentType(const std::string& code, const ScanContext& context);
    bool isCHTLCode(const std::string& code, const ScanContext& context);
    bool isCHTLJSCode(const std::string& code, const ScanContext& context);
    bool isCSSCode(const std::string& code, const ScanContext& context);
    bool isJavaScriptCode(const std::string& code, const ScanContext& context);
    bool isGeneratorComment(const std::string& code);
    
    // 上下文管理
    ScanContext createInitialContext();
    void updateContext(ScanContext& context, const std::string& code);
    void enterBlock(ScanContext& context, const std::string& blockType);
    void exitBlock(ScanContext& context);
    
    // CHTL语法检测
    bool hasCHTLKeywords(const std::string& code);
    bool hasCHTLJSKeywords(const std::string& code);
    bool hasTemplateUsage(const std::string& code);
    bool hasCustomUsage(const std::string& code);
    bool hasOriginBlock(const std::string& code);
    bool hasImportStatement(const std::string& code);
    bool hasNamespaceDeclaration(const std::string& code);
    bool hasConfigurationBlock(const std::string& code);
    
    // CHTL JS语法检测
    bool hasEnhancedSelectors(const std::string& code);
    bool hasEventBinding(const std::string& code);
    bool hasAnimateBlock(const std::string& code);
    bool hasListenBlock(const std::string& code);
    bool hasVirtualObject(const std::string& code);
    bool hasModuleImport(const std::string& code);
    bool hasDelegateBlock(const std::string& code);
    
    // 注释处理
    std::vector<CodeFragment> extractComments(const std::string& code);
    std::string processGeneratorComments(const std::string& comment, const ScanContext& context);
    
    // 辅助方法
    std::vector<std::string> getCHTLKeywords();
    std::vector<std::string> getCHTLJSKeywords();
    std::vector<std::string> getBlockKeywords();
    
    // 错误处理
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    
    // 调试和统计
    void enableDebugMode(bool enable) { m_debugMode = enable; }
    void printScanStatistics();
    
private:
    bool m_debugMode;
    std::unordered_map<FragmentType, size_t> m_fragmentCounts;
    std::vector<std::string> m_scanErrors;
    std::vector<std::string> m_scanWarnings;
    
    // 内部扫描方法
    size_t findNextSignificantPosition(const std::string& code, size_t startPos);
    size_t findBlockEnd(const std::string& code, size_t startPos, char openChar, char closeChar);
    bool isInsideString(const std::string& code, size_t pos);
    bool isInsideComment(const std::string& code, size_t pos);
    
    // 语法模式匹配
    bool matchesCHTLPattern(const std::string& code);
    bool matchesCHTLJSPattern(const std::string& code);
    bool matchesCSSPattern(const std::string& code);
    bool matchesJSPattern(const std::string& code);
    
    // 上下文推断
    std::string inferContextFromSurrounding(const std::string& code, size_t pos);
    FragmentType inferTypeFromContext(const std::string& code, const ScanContext& context);
    
    // 片段验证
    bool validateFragment(const CodeFragment& fragment);
    bool isCompleteFragment(const CodeFragment& fragment);
    
    // 优化
    void optimizeFragments(std::vector<CodeFragment>& fragments);
    void removeEmptyFragments(std::vector<CodeFragment>& fragments);
    void mergeCompatibleFragments(std::vector<CodeFragment>& fragments);
};

}