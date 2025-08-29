#include "CJMODScanner.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"
#include <algorithm>
#include <regex>
#include <set>

namespace CJMOD {

// ScannerState实现
ScannerState::ScannerState() 
    : position(0), line(1), column(1), isValid(true) {
}

void ScannerState::advance(char c) {
    position++;
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
}

CHTL::Position ScannerState::getCurrentPosition() const {
    return {line, column, position};
}

bool ScannerState::isAtEnd(const CHTL::String& source) const {
    return position >= source.length();
}

// DualPointerScanResult实现
bool DualPointerScanResult::hasErrors() const {
    return !errors.empty();
}

bool DualPointerScanResult::hasWarnings() const {
    return !warnings.empty();
}

CHTL::String DualPointerScanResult::getErrorSummary() const {
    if (errors.empty()) return "No errors";
    
    CHTL::String summary = "Errors (" + std::to_string(errors.size()) + "):\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        summary += std::to_string(i + 1) + ". " + errors[i] + "\n";
    }
    return summary;
}

CHTL::String DualPointerScanResult::getWarningSummary() const {
    if (warnings.empty()) return "No warnings";
    
    CHTL::String summary = "Warnings (" + std::to_string(warnings.size()) + "):\n";
    for (size_t i = 0; i < warnings.size(); ++i) {
        summary += std::to_string(i + 1) + ". " + warnings[i] + "\n";
    }
    return summary;
}

// CJMODScanner实现
CJMODScanner::CJMODScanner(const CHTL::String& source, const CJMODScannerConfig& config)
    : sourceCode(source), config(config), primaryState(), auxiliaryState() {
    errors.clear();
    warnings.clear();
    initializePattern();
}

DualPointerScanResult CJMODScanner::scan() {
    DualPointerScanResult result;
    
    try {
        // 重置扫描器状态
        resetScanners();
        
        // 执行双指针扫描
        performDualPointerScan(result);
        
        // 验证扫描结果
        validateScanResults(result);
        
        // 后处理
        if (config.enablePostProcessing) {
            postProcessResults(result);
        }
        
        result.success = result.errors.empty();
        
    } catch (const std::exception& e) {
        result.errors.push_back("CJMOD扫描异常: " + CHTL::String(e.what()));
        result.success = false;
    }
    
    return result;
}

void CJMODScanner::performDualPointerScan(DualPointerScanResult& result) {
    while (!primaryState.isAtEnd(sourceCode) || !auxiliaryState.isAtEnd(sourceCode)) {
        // 主指针扫描
        if (!primaryState.isAtEnd(sourceCode)) {
            scanWithPrimaryPointer(result);
        }
        
        // 辅助指针扫描
        if (!auxiliaryState.isAtEnd(sourceCode)) {
            scanWithAuxiliaryPointer(result);
        }
        
        // 同步指针位置
        synchronizePointers();
        
        // 检查死循环
        if (hasDeadlock()) {
            result.errors.push_back("扫描器死锁检测");
            break;
        }
    }
}

void CJMODScanner::scanWithPrimaryPointer(DualPointerScanResult& result) {
    // 主指针负责前置截取
    if (detectCJMODPrefix(primaryState.position)) {
        performPrefixCapture(result);
    } else {
        // 正常字符扫描
        advancePrimaryPointer();
    }
}

void CJMODScanner::scanWithAuxiliaryPointer(DualPointerScanResult& result) {
    // 辅助指针负责辅助扫描和验证
    if (auxiliaryState.position < primaryState.position) {
        // 验证已扫描区域
        validateScannedRegion(auxiliaryState.position, primaryState.position, result);
        auxiliaryState.position = primaryState.position;
    } else {
        // 前瞻扫描
        performLookaheadScan(result);
    }
}

bool CJMODScanner::detectCJMODPrefix(size_t position) {
    if (position >= sourceCode.length()) return false;
    
    // 检测CJMOD特有的前缀模式
    CHTL::StringVector cjmodPrefixes = {
        "@CJmod", 
        "extern", 
        "native", 
        "export",
        "__cjmod__"
    };
    
    for (const auto& prefix : cjmodPrefixes) {
        if (matchPatternAtPosition(prefix, position)) {
            return true;
        }
    }
    
    // 检测C++风格的函数声明
    if (detectCppFunctionSignature(position)) {
        return true;
    }
    
    // 检测导出宏
    if (detectExportMacro(position)) {
        return true;
    }
    
    return false;
}

void CJMODScanner::performPrefixCapture(DualPointerScanResult& result) {
    size_t captureStart = primaryState.position;
    
    // 确定捕获类型
    CJMODElementType elementType = determineCJMODElementType(captureStart);
    
    // 执行前置截取
    size_t captureEnd = findCaptureEnd(captureStart, elementType);
    
    if (captureEnd > captureStart) {
        // 创建CJMOD元素
        CJMODElement element;
        element.type = elementType;
        element.content = sourceCode.substr(captureStart, captureEnd - captureStart);
        element.startPosition = captureStart;
        element.endPosition = captureEnd;
        element.sourceLocation = primaryState.getCurrentPosition();
        
        // 解析元素属性
        parseElementAttributes(element);
        
        // 验证元素有效性
        if (validateCJMODElement(element)) {
            result.capturedElements.push_back(element);
        } else {
            result.warnings.push_back("无效的CJMOD元素: " + element.content.substr(0, 50));
        }
        
        // 更新主指针位置
        primaryState.position = captureEnd;
        updateScannerState(primaryState, captureEnd);
    } else {
        result.errors.push_back("CJMOD前置截取失败 at position " + std::to_string(captureStart));
        advancePrimaryPointer();
    }
}

CJMODElementType CJMODScanner::determineCJMODElementType(size_t position) {
    CHTL::String segment = getSegmentAtPosition(position, 100);
    
    // 检测函数定义
    if (std::regex_search(segment, std::regex(R"(\w+\s*\([^)]*\)\s*\{)"))) {
        return CJMODElementType::FUNCTION;
    }
    
    // 检测类定义
    if (std::regex_search(segment, std::regex(R"(class\s+\w+)"))) {
        return CJMODElementType::CLASS;
    }
    
    // 检测导出声明
    if (std::regex_search(segment, std::regex(R"(export\s+)"))) {
        return CJMODElementType::EXPORT;
    }
    
    // 检测导入声明
    if (std::regex_search(segment, std::regex(R"(@CJmod\s+from)"))) {
        return CJMODElementType::IMPORT;
    }
    
    // 检测变量定义
    if (std::regex_search(segment, std::regex(R"(\w+\s+\w+\s*[=;])"))) {
        return CJMODElementType::VARIABLE;
    }
    
    return CJMODElementType::UNKNOWN;
}

size_t CJMODScanner::findCaptureEnd(size_t start, CJMODElementType type) {
    size_t pos = start;
    int braceLevel = 0;
    int parenLevel = 0;
    bool inString = false;
    char stringChar = 0;
    
    while (pos < sourceCode.length()) {
        char c = sourceCode[pos];
        
        // 处理字符串
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
        } else if (inString && c == stringChar && (pos == 0 || sourceCode[pos-1] != '\\')) {
            inString = false;
        }
        
        if (!inString) {
            // 处理嵌套结构
            if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
                if (braceLevel == 0 && type == CJMODElementType::FUNCTION) {
                    return pos + 1;
                }
            } else if (c == '(') {
                parenLevel++;
            } else if (c == ')') {
                parenLevel--;
            } else if (c == ';' && braceLevel == 0 && parenLevel == 0) {
                // 语句结束
                if (type == CJMODElementType::VARIABLE || 
                    type == CJMODElementType::EXPORT ||
                    type == CJMODElementType::IMPORT) {
                    return pos + 1;
                }
            }
        }
        
        pos++;
        
        // 防止无限循环
        if (pos - start > config.maxCaptureLength) {
            break;
        }
    }
    
    return pos;
}

void CJMODScanner::parseElementAttributes(CJMODElement& element) {
    const CHTL::String& content = element.content;
    
    // 解析函数签名
    if (element.type == CJMODElementType::FUNCTION) {
        parseFunctionSignature(element, content);
    }
    
    // 解析类声明
    else if (element.type == CJMODElementType::CLASS) {
        parseClassDeclaration(element, content);
    }
    
    // 解析导出声明
    else if (element.type == CJMODElementType::EXPORT) {
        parseExportDeclaration(element, content);
    }
    
    // 解析导入声明
    else if (element.type == CJMODElementType::IMPORT) {
        parseImportDeclaration(element, content);
    }
    
    // 解析变量声明
    else if (element.type == CJMODElementType::VARIABLE) {
        parseVariableDeclaration(element, content);
    }
}

void CJMODScanner::parseFunctionSignature(CJMODElement& element, const CHTL::String& content) {
    // 解析函数名
    std::regex functionRegex(R"((\w+)\s*\([^)]*\))");
    std::smatch match;
    
    if (std::regex_search(content, match, functionRegex)) {
        element.name = match[1].str();
    }
    
    // 解析参数列表
    std::regex paramRegex(R"(\(([^)]*)\))");
    if (std::regex_search(content, match, paramRegex)) {
        CHTL::String paramStr = match[1].str();
        element.parameters = parseParameterList(paramStr);
    }
    
    // 解析返回类型
    std::regex returnTypeRegex(R"((\w+)\s+\w+\s*\()");
    if (std::regex_search(content, match, returnTypeRegex)) {
        element.attributes["return_type"] = match[1].str();
    }
    
    // 检测extern声明
    if (content.find("extern") != CHTL::String::npos) {
        element.attributes["extern"] = "true";
    }
    
    // 检测native声明
    if (content.find("native") != CHTL::String::npos) {
        element.attributes["native"] = "true";
    }
}

void CJMODScanner::parseClassDeclaration(CJMODElement& element, const CHTL::String& content) {
    // 解析类名
    std::regex classRegex(R"(class\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(content, match, classRegex)) {
        element.name = match[1].str();
    }
    
    // 解析继承关系
    std::regex inheritanceRegex(R"(:\s*public\s+(\w+))");
    if (std::regex_search(content, match, inheritanceRegex)) {
        element.attributes["base_class"] = match[1].str();
    }
    
    // 检测抽象类
    if (content.find("virtual") != CHTL::String::npos) {
        element.attributes["abstract"] = "true";
    }
}

void CJMODScanner::parseExportDeclaration(CJMODElement& element, const CHTL::String& content) {
    // 解析导出名称
    std::regex exportRegex(R"(export\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(content, match, exportRegex)) {
        element.name = match[1].str();
    }
    
    // 解析导出类型
    if (content.find("function") != CHTL::String::npos) {
        element.attributes["export_type"] = "function";
    } else if (content.find("class") != CHTL::String::npos) {
        element.attributes["export_type"] = "class";
    } else {
        element.attributes["export_type"] = "variable";
    }
}

void CJMODScanner::parseImportDeclaration(CJMODElement& element, const CHTL::String& content) {
    // 解析导入路径
    std::regex importRegex(R"(@CJmod\s+from\s+['"]([^'"]+)['"])");
    std::smatch match;
    
    if (std::regex_search(content, match, importRegex)) {
        element.attributes["import_path"] = match[1].str();
    }
    
    // 解析别名
    std::regex aliasRegex(R"(as\s+(\w+))");
    if (std::regex_search(content, match, aliasRegex)) {
        element.attributes["alias"] = match[1].str();
    }
}

void CJMODScanner::parseVariableDeclaration(CJMODElement& element, const CHTL::String& content) {
    // 解析变量类型和名称
    std::regex varRegex(R"((\w+)\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(content, match, varRegex)) {
        element.attributes["variable_type"] = match[1].str();
        element.name = match[2].str();
    }
    
    // 检测常量
    if (content.find("const") != CHTL::String::npos) {
        element.attributes["const"] = "true";
    }
    
    // 检测静态变量
    if (content.find("static") != CHTL::String::npos) {
        element.attributes["static"] = "true";
    }
}

CHTL::StringVector CJMODScanner::parseParameterList(const CHTL::String& paramStr) {
    CHTL::StringVector parameters;
    
    if (paramStr.empty()) return parameters;
    
    // 按逗号分割参数
    std::regex commaRegex(R"(\s*,\s*)");
    std::sregex_token_iterator iter(paramStr.begin(), paramStr.end(), commaRegex, -1);
    std::sregex_token_iterator end;
    
    for (; iter != end; ++iter) {
        CHTL::String param = iter->str();
        if (!param.empty()) {
            // 移除多余空白
            param = std::regex_replace(param, std::regex(R"(^\s+|\s+$)"), "");
            parameters.push_back(param);
        }
    }
    
    return parameters;
}

bool CJMODScanner::validateCJMODElement(const CJMODElement& element) {
    // 检查基本有效性
    if (element.content.empty() || element.name.empty()) {
        return false;
    }
    
    // 检查类型特定的有效性
    switch (element.type) {
        case CJMODElementType::FUNCTION:
            return validateFunctionElement(element);
        case CJMODElementType::CLASS:
            return validateClassElement(element);
        case CJMODElementType::EXPORT:
            return validateExportElement(element);
        case CJMODElementType::IMPORT:
            return validateImportElement(element);
        case CJMODElementType::VARIABLE:
            return validateVariableElement(element);
        default:
            return false;
    }
}

bool CJMODScanner::validateFunctionElement(const CJMODElement& element) {
    // 检查函数名有效性
    if (!isValidIdentifier(element.name)) {
        return false;
    }
    
    // 检查参数类型
    for (const CHTL::String& param : element.parameters) {
        if (!isValidParameterDeclaration(param)) {
            return false;
        }
    }
    
    // 检查返回类型
    auto it = element.attributes.find("return_type");
    if (it != element.attributes.end()) {
        if (!isValidTypeName(it->second)) {
            return false;
        }
    }
    
    return true;
}

bool CJMODScanner::validateClassElement(const CJMODElement& element) {
    // 检查类名有效性
    if (!isValidIdentifier(element.name)) {
        return false;
    }
    
    // 检查基类
    auto it = element.attributes.find("base_class");
    if (it != element.attributes.end()) {
        if (!isValidIdentifier(it->second)) {
            return false;
        }
    }
    
    return true;
}

bool CJMODScanner::validateExportElement(const CJMODElement& element) {
    // 检查导出名称
    if (!isValidIdentifier(element.name)) {
        return false;
    }
    
    // 检查导出类型
    auto it = element.attributes.find("export_type");
    if (it != element.attributes.end()) {
        CHTL::StringVector validTypes = {"function", "class", "variable"};
        if (std::find(validTypes.begin(), validTypes.end(), it->second) == validTypes.end()) {
            return false;
        }
    }
    
    return true;
}

bool CJMODScanner::validateImportElement(const CJMODElement& element) {
    // 检查导入路径
    auto it = element.attributes.find("import_path");
    if (it == element.attributes.end() || it->second.empty()) {
        return false;
    }
    
    // 检查别名（如果存在）
    auto aliasIt = element.attributes.find("alias");
    if (aliasIt != element.attributes.end()) {
        if (!isValidIdentifier(aliasIt->second)) {
            return false;
        }
    }
    
    return true;
}

bool CJMODScanner::validateVariableElement(const CJMODElement& element) {
    // 检查变量名
    if (!isValidIdentifier(element.name)) {
        return false;
    }
    
    // 检查变量类型
    auto it = element.attributes.find("variable_type");
    if (it != element.attributes.end()) {
        if (!isValidTypeName(it->second)) {
            return false;
        }
    }
    
    return true;
}

void CJMODScanner::validateScannedRegion(size_t start, size_t end, DualPointerScanResult& result) {
    if (start >= end || end > sourceCode.length()) return;
    
    CHTL::String region = sourceCode.substr(start, end - start);
    
    // 检查语法错误
    if (hasSyntaxErrors(region)) {
        result.warnings.push_back("语法错误检测在区域 [" + 
                                 std::to_string(start) + ", " + std::to_string(end) + "]");
    }
    
    // 检查不匹配的括号
    if (hasUnmatchedBrackets(region)) {
        result.warnings.push_back("不匹配的括号在区域 [" + 
                                 std::to_string(start) + ", " + std::to_string(end) + "]");
    }
}

void CJMODScanner::performLookaheadScan(DualPointerScanResult& result) {
    size_t lookaheadStart = auxiliaryState.position;
    size_t lookaheadEnd = std::min(lookaheadStart + config.maxLookahead, sourceCode.length());
    
    // 前瞻扫描CJMOD模式
    for (size_t pos = lookaheadStart; pos < lookaheadEnd; ++pos) {
        if (detectCJMODPrefix(pos)) {
            // 记录预期的CJMOD元素
            LookaheadInfo info;
            info.position = pos;
            info.expectedType = determineCJMODElementType(pos);
            info.confidence = calculateConfidence(pos);
            
            result.lookaheadInfo.push_back(info);
        }
    }
    
    // 更新辅助指针
    auxiliaryState.position = lookaheadEnd;
    updateScannerState(auxiliaryState, lookaheadEnd);
}

double CJMODScanner::calculateConfidence(size_t position) {
    CHTL::String segment = getSegmentAtPosition(position, 50);
    double confidence = 0.0;
    
    // 基于关键字的置信度
    CHTL::StringVector highConfidenceKeywords = {"@CJmod", "extern", "export"};
    CHTL::StringVector mediumConfidenceKeywords = {"native", "class", "function"};
    
    for (const auto& keyword : highConfidenceKeywords) {
        if (segment.find(keyword) != CHTL::String::npos) {
            confidence += 0.4;
        }
    }
    
    for (const auto& keyword : mediumConfidenceKeywords) {
        if (segment.find(keyword) != CHTL::String::npos) {
            confidence += 0.2;
        }
    }
    
    // 基于语法结构的置信度
    if (std::regex_search(segment, std::regex(R"(\w+\s*\([^)]*\)\s*\{)"))) {
        confidence += 0.3; // 函数定义模式
    }
    
    if (std::regex_search(segment, std::regex(R"(class\s+\w+)"))) {
        confidence += 0.25; // 类定义模式
    }
    
    return std::min(confidence, 1.0);
}

void CJMODScanner::synchronizePointers() {
    // 确保辅助指针不落后主指针太多
    if (auxiliaryState.position < primaryState.position - config.maxPointerGap) {
        auxiliaryState.position = primaryState.position - config.maxPointerGap;
        updateScannerState(auxiliaryState, auxiliaryState.position);
    }
    
    // 确保主指针不领先辅助指针太多
    if (primaryState.position > auxiliaryState.position + config.maxPointerGap) {
        primaryState.position = auxiliaryState.position + config.maxPointerGap;
        updateScannerState(primaryState, primaryState.position);
    }
}

bool CJMODScanner::hasDeadlock() {
    static size_t lastPrimaryPos = 0;
    static size_t lastAuxiliaryPos = 0;
    static int stuckCount = 0;
    
    if (primaryState.position == lastPrimaryPos && 
        auxiliaryState.position == lastAuxiliaryPos) {
        stuckCount++;
        return stuckCount > config.deadlockThreshold;
    }
    
    lastPrimaryPos = primaryState.position;
    lastAuxiliaryPos = auxiliaryState.position;
    stuckCount = 0;
    return false;
}

void CJMODScanner::validateScanResults(DualPointerScanResult& result) {
    // 检查捕获元素的完整性
    for (const auto& element : result.capturedElements) {
        if (!validateElementCompleteness(element)) {
            result.warnings.push_back("不完整的CJMOD元素: " + element.name);
        }
    }
    
    // 检查重复元素
    checkDuplicateElements(result);
    
    // 检查依赖关系
    validateDependencies(result);
}

bool CJMODScanner::validateElementCompleteness(const CJMODElement& element) {
    // 检查基本字段
    if (element.name.empty() || element.content.empty()) {
        return false;
    }
    
    // 检查位置信息
    if (element.startPosition >= element.endPosition) {
        return false;
    }
    
    // 检查类型特定的完整性
    switch (element.type) {
        case CJMODElementType::FUNCTION:
            return element.parameters.size() >= 0; // 函数可以无参数
        case CJMODElementType::IMPORT:
            return element.attributes.find("import_path") != element.attributes.end();
        default:
            return true;
    }
}

void CJMODScanner::checkDuplicateElements(DualPointerScanResult& result) {
    std::set<CHTL::String> elementNames;
    
    for (const auto& element : result.capturedElements) {
        if (elementNames.find(element.name) != elementNames.end()) {
            result.warnings.push_back("重复的CJMOD元素: " + element.name);
        } else {
            elementNames.insert(element.name);
        }
    }
}

void CJMODScanner::validateDependencies(DualPointerScanResult& result) {
    // 收集所有导入和导出
    std::set<CHTL::String> exports;
    std::set<CHTL::String> imports;
    
    for (const auto& element : result.capturedElements) {
        if (element.type == CJMODElementType::EXPORT) {
            exports.insert(element.name);
        } else if (element.type == CJMODElementType::IMPORT) {
            auto it = element.attributes.find("import_path");
            if (it != element.attributes.end()) {
                imports.insert(it->second);
            }
        }
    }
    
    // 检查未解析的导入
    for (const CHTL::String& import : imports) {
        if (exports.find(import) == exports.end()) {
            result.warnings.push_back("未解析的导入: " + import);
        }
    }
}

void CJMODScanner::postProcessResults(DualPointerScanResult& result) {
    // 排序捕获的元素
    if (config.sortElementsByPosition) {
        std::sort(result.capturedElements.begin(), result.capturedElements.end(),
                 [](const CJMODElement& a, const CJMODElement& b) {
                     return a.startPosition < b.startPosition;
                 });
    }
    
    // 优化元素信息
    if (config.optimizeElementInfo) {
        optimizeElementInformation(result);
    }
    
    // 生成统计信息
    generateStatistics(result);
}

void CJMODScanner::optimizeElementInformation(DualPointerScanResult& result) {
    for (auto& element : result.capturedElements) {
        // 移除多余的空白
        element.content = std::regex_replace(element.content, std::regex(R"(\s+)"), " ");
        
        // 优化属性存储
        optimizeElementAttributes(element);
    }
}

void CJMODScanner::optimizeElementAttributes(CJMODElement& element) {
    // 移除空属性
    auto it = element.attributes.begin();
    while (it != element.attributes.end()) {
        if (it->second.empty()) {
            it = element.attributes.erase(it);
        } else {
            ++it;
        }
    }
}

void CJMODScanner::generateStatistics(DualPointerScanResult& result) {
    // 统计元素类型
    CHTL::StringUnorderedMap typeCounts;
    
    for (const auto& element : result.capturedElements) {
        CHTL::String typeStr = cjmodElementTypeToString(element.type);
        typeCounts[typeStr]++;
    }
    
    // 生成统计字符串
    CHTL::String stats = "CJMOD扫描统计:\n";
    for (const auto& pair : typeCounts) {
        stats += "  " + pair.first + ": " + std::to_string(pair.second) + "\n";
    }
    
    result.statistics = stats;
}

// 辅助方法实现
void CJMODScanner::resetScanners() {
    primaryState = ScannerState();
    auxiliaryState = ScannerState();
    errors.clear();
    warnings.clear();
}

void CJMODScanner::advancePrimaryPointer() {
    if (primaryState.position < sourceCode.length()) {
        primaryState.advance(sourceCode[primaryState.position]);
    }
}

void CJMODScanner::updateScannerState(ScannerState& state, size_t position) {
    if (position >= sourceCode.length()) {
        state.position = sourceCode.length();
        return;
    }
    
    // 重新计算行列信息
    state.line = 1;
    state.column = 1;
    
    for (size_t i = 0; i < position && i < sourceCode.length(); ++i) {
        if (sourceCode[i] == '\n') {
            state.line++;
            state.column = 1;
        } else {
            state.column++;
        }
    }
    
    state.position = position;
}

bool CJMODScanner::matchPatternAtPosition(const CHTL::String& pattern, size_t position) {
    if (position + pattern.length() > sourceCode.length()) {
        return false;
    }
    
    return sourceCode.substr(position, pattern.length()) == pattern;
}

CHTL::String CJMODScanner::getSegmentAtPosition(size_t position, size_t length) {
    if (position >= sourceCode.length()) {
        return "";
    }
    
    size_t actualLength = std::min(length, sourceCode.length() - position);
    return sourceCode.substr(position, actualLength);
}

bool CJMODScanner::detectCppFunctionSignature(size_t position) {
    CHTL::String segment = getSegmentAtPosition(position, 200);
    
    // 检测C++函数签名模式
    std::regex cppFunctionRegex(R"((\w+\s+)*\w+\s+\w+\s*\([^)]*\)\s*(\{|;))");
    return std::regex_search(segment, cppFunctionRegex);
}

bool CJMODScanner::detectExportMacro(size_t position) {
    CHTL::String segment = getSegmentAtPosition(position, 50);
    
    // 检测导出宏模式
    CHTL::StringVector exportMacros = {
        "CHTL_EXPORT",
        "CJMOD_EXPORT", 
        "__declspec(dllexport)",
        "extern \"C\""
    };
    
    for (const auto& macro : exportMacros) {
        if (segment.find(macro) != CHTL::String::npos) {
            return true;
        }
    }
    
    return false;
}

bool CJMODScanner::isValidIdentifier(const CHTL::String& identifier) {
    if (identifier.empty()) return false;
    
    // 检查首字符
    if (!std::isalpha(identifier[0]) && identifier[0] != '_') {
        return false;
    }
    
    // 检查其他字符
    for (size_t i = 1; i < identifier.length(); ++i) {
        char c = identifier[i];
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool CJMODScanner::isValidParameterDeclaration(const CHTL::String& param) {
    // 简化的参数声明验证
    std::regex paramRegex(R"(\w+\s+\w+)"); // type name
    return std::regex_match(param, paramRegex);
}

bool CJMODScanner::isValidTypeName(const CHTL::String& typeName) {
    if (typeName.empty()) return false;
    
    // 基本类型检查
    CHTL::StringVector validTypes = {
        "void", "int", "float", "double", "char", "bool",
        "string", "String", "size_t"
    };
    
    for (const auto& validType : validTypes) {
        if (typeName == validType) {
            return true;
        }
    }
    
    // 自定义类型（以大写字母开头）
    return !typeName.empty() && std::isupper(typeName[0]);
}

bool CJMODScanner::hasSyntaxErrors(const CHTL::String& content) {
    // 简化的语法错误检查
    return content.find(";;") != CHTL::String::npos || // 双分号
           content.find("{{") != CHTL::String::npos;   // 双开括号
}

bool CJMODScanner::hasUnmatchedBrackets(const CHTL::String& content) {
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    
    for (char c : content) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
    }
    
    return braceCount != 0 || parenCount != 0 || bracketCount != 0;
}

void CJMODScanner::initializePattern() {
    // 初始化CJMOD模式识别
    // 这里可以预编译正则表达式以提高性能
}

CHTL::String CJMODScanner::cjmodElementTypeToString(CJMODElementType type) {
    switch (type) {
        case CJMODElementType::FUNCTION: return "Function";
        case CJMODElementType::CLASS: return "Class";
        case CJMODElementType::VARIABLE: return "Variable";
        case CJMODElementType::EXPORT: return "Export";
        case CJMODElementType::IMPORT: return "Import";
        default: return "Unknown";
    }
}

void CJMODScanner::setConfig(const CJMODScannerConfig& newConfig) {
    config = newConfig;
}

CJMODScannerConfig CJMODScanner::getConfig() const {
    return config;
}

size_t CJMODScanner::getCurrentPosition() const {
    return primaryState.position;
}

size_t CJMODScanner::getAuxiliaryPosition() const {
    return auxiliaryState.position;
}

} // namespace CJMOD