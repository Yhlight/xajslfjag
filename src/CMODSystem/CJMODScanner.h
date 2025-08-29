#pragma once

#include "../Util/Common.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"
#include "Syntax.h"
#include <vector>
#include <unordered_map>

namespace CJMOD {

// 前置声明
class Arg;

// 扫描器状态
struct ScannerState {
    size_t position;
    size_t line;
    size_t column;
    bool isValid;
    
    ScannerState();
    void advance(char c);
    CHTL::Position getCurrentPosition() const;
    bool isAtEnd(const CHTL::String& source) const;
    void reset() { position = 0; line = 1; column = 1; isValid = true; }
};

// CJMOD元素类型
enum class CJMODElementType {
    FUNCTION,
    CLASS,
    VARIABLE,
    EXPORT,
    IMPORT,
    UNKNOWN
};

// CJMOD元素
struct CJMODElement {
    CJMODElementType type;
    CHTL::String name;
    CHTL::String content;
    CHTL::String signature;
    size_t startPosition;
    size_t endPosition;
    std::unordered_map<CHTL::String, CHTL::String> attributes;
    
    CJMODElement() : type(CJMODElementType::UNKNOWN), startPosition(0), endPosition(0) {}
};

// 双指针扫描结果
struct DualPointerScanResult {
    CHTL::StringVector fragments;
    CHTL::StringVector keywords;
    CHTL::StringVector errors;
    CHTL::StringVector warnings;
    std::vector<CJMODElement> capturedElements;
    std::unordered_map<CHTL::String, CHTL::String> metadata;
    bool success = false;
    
    bool hasErrors() const;
    bool hasWarnings() const;
    CHTL::String getErrorSummary() const;
    CHTL::String getWarningSummary() const;
};

// CJMOD扫描器配置
struct CJMODScannerConfig {
    bool enableDualPointer = true;
    bool enablePreemptiveCapture = true;
    bool enableKeywordValidation = true;
    bool enableFragmentValidation = true;
    bool enablePostProcessing = true;
    bool strictMode = false;
    bool sortElementsByPosition = true;
    bool optimizeElementInfo = true;
    size_t maxScanDepth = 1000;
    CHTL::StringVector customKeywords;
    CHTL::StringVector ignorePatterns;
};

// CJMOD扫描器主类
class CJMODScanner {
public:
    // 构造函数
    CJMODScanner(const CHTL::String& source, const CJMODScannerConfig& config = CJMODScannerConfig{});
    
    // 实例扫描方法
    DualPointerScanResult scan();
    
    // 关键的静态扫描方法 - 动态获取值的核心
    static Arg scan(const Arg& args, const CHTL::String& keyword);
    
    // 前置截取机制
    static CHTL::String preemptiveCapture(const CHTL::String& source, const CHTL::String& pattern);
    
    // 语法片段验证
    static bool validateSyntaxFragment(const CHTL::String& fragment);
    
    // 工具方法
    void resetScanners();
    const CHTL::StringVector& getErrors() const { return errors; }
    const CHTL::StringVector& getWarnings() const { return warnings; }
    
    // 配置方法
    void setConfig(const CJMODScannerConfig& newConfig);
    CJMODScannerConfig getConfig() const;
    size_t getCurrentPosition() const;
    size_t getAuxiliaryPosition() const;
    
private:
    // 成员变量
    CHTL::String sourceCode;
    CJMODScannerConfig config;
    ScannerState primaryState;
    ScannerState auxiliaryState;
    CHTL::StringVector errors;
    CHTL::StringVector warnings;
    
    // 核心算法实现
    void performDualPointerScan(DualPointerScanResult& result);
    void scanWithPrimaryPointer(DualPointerScanResult& result);
    void scanWithAuxiliaryPointer(DualPointerScanResult& result);
    
    // 同步和状态管理
    void synchronizePointers();
    bool hasDeadlock();
    void advancePrimaryPointer();
    void updateScannerState(ScannerState& state, size_t position);
    
    // CJMOD检测和处理
    bool detectCJMODPrefix(size_t position);
    void performPrefixCapture(DualPointerScanResult& result);
    void validateScannedRegion(size_t start, size_t end, DualPointerScanResult& result);
    void performLookaheadScan(DualPointerScanResult& result);
    double calculateConfidence(size_t position);
    
    // 后处理和优化
    void postProcessResults(DualPointerScanResult& result);
    void validateScanResults(DualPointerScanResult& result);
    void optimizeElementInformation(DualPointerScanResult& result);
    void generateStatistics(DualPointerScanResult& result);
    void checkDuplicateElements(DualPointerScanResult& result);
    void validateDependencies(DualPointerScanResult& result);
    
    // 元素处理方法
    CJMODElementType determineCJMODElementType(size_t position);
    size_t findCaptureEnd(size_t start, CJMODElementType type);
    void parseElementAttributes(CJMODElement& element);
    void parseFunctionSignature(CJMODElement& element, const CHTL::String& content);
    void parseClassDeclaration(CJMODElement& element, const CHTL::String& content);
    void parseExportDeclaration(CJMODElement& element, const CHTL::String& content);
    void parseImportDeclaration(CJMODElement& element, const CHTL::String& content);
    void parseVariableDeclaration(CJMODElement& element, const CHTL::String& content);
    CHTL::StringVector parseParameterList(const CHTL::String& paramStr);
    
    // 元素验证方法
    bool validateCJMODElement(const CJMODElement& element);
    bool validateFunctionElement(const CJMODElement& element);
    bool validateClassElement(const CJMODElement& element);
    bool validateExportElement(const CJMODElement& element);
    bool validateImportElement(const CJMODElement& element);
    bool validateVariableElement(const CJMODElement& element);
    bool validateElementCompleteness(const CJMODElement& element);
    void optimizeElementAttributes(CJMODElement& element);
    
    // 工具方法
    bool matchPatternAtPosition(const CHTL::String& pattern, size_t position);
    CHTL::String getSegmentAtPosition(size_t position, size_t length);
    bool detectCppFunctionSignature(size_t position);
    bool detectExportMacro(size_t position);
    bool isValidIdentifier(const CHTL::String& identifier);
    bool isValidParameterDeclaration(const CHTL::String& param);
    bool isValidTypeName(const CHTL::String& typeName);
    bool hasSyntaxErrors(const CHTL::String& content);
    bool hasUnmatchedBrackets(const CHTL::String& content);
    
    // 字符串转换
    static CHTL::String cjmodElementTypeToString(CJMODElementType type);
    
    // 初始化
    void initializePattern();
    
    // 静态辅助方法
    static CHTL::StringVector extractArgumentsFromSource(const CHTL::String& source, const CHTL::String& keyword);
    static bool isValidCJMODKeyword(const CHTL::String& keyword);
    static CHTL::String cleanupFragment(const CHTL::String& fragment);
};

} // namespace CJMOD