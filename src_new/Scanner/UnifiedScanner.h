#pragma once
#include "../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 代码片段类型
 */
enum class FragmentType {
    CHTL,           // CHTL语法片段
    CHTL_JS,        // CHTL JS语法片段
    CSS,            // CSS片段
    JAVASCRIPT,     // JavaScript片段
    HTML,           // HTML片段
    UNKNOWN
};

/**
 * 代码片段信息
 */
struct CodeFragment {
    FragmentType type;          // 片段类型
    std::string content;        // 片段内容
    size_t startPosition;       // 开始位置
    size_t endPosition;         // 结束位置
    size_t lineNumber;          // 行号
    size_t columnNumber;        // 列号
    
    CodeFragment(FragmentType t = FragmentType::UNKNOWN, 
                const std::string& c = "", 
                size_t start = 0, 
                size_t end = 0,
                size_t line = 1,
                size_t col = 1)
        : type(t), content(c), startPosition(start), endPosition(end), 
          lineNumber(line), columnNumber(col) {}
};

/**
 * 扫描上下文
 */
struct ScanContext {
    std::string source;                 // 源代码
    size_t position;                    // 当前位置
    size_t lineNumber;                  // 当前行号
    size_t columnNumber;                // 当前列号
    bool inCHTLBlock;                   // 是否在CHTL块中
    bool inCHTLJSBlock;                 // 是否在CHTL JS块中
    
    ScanContext(const std::string& src = "") 
        : source(src), position(0), lineNumber(1), columnNumber(1),
          inCHTLBlock(false), inCHTLJSBlock(false) {}
};

/**
 * 统一扫描器
 * 精准代码切割器，基于可变长度切片工作
 * 严格按照目标规划.ini的架构设计
 */
class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner() = default;
    
    // ========== 核心扫描功能 ==========
    
    /**
     * 智能切片：根据上下文调整切片长度
     * 基于可变长度切片进行工作
     */
    std::vector<CodeFragment> smartSlice(const std::string& source, size_t sliceSize = 1024);
    
    /**
     * 检查切片边界是否合理
     * 检查下一个片段的开头是否可能组成完整的CHTL或CHTL JS代码片段
     */
    bool isValidSliceBoundary(const std::string& source, size_t position);
    
    /**
     * 扩展切片以包含完整语法单元
     * 如果截断位置不合理，向前扩增一定长度
     */
    size_t extendSliceForCompleteness(const std::string& source, size_t start, size_t end);
    
    // ========== 语法识别 ==========
    
    /**
     * 识别片段类型
     */
    FragmentType identifyFragmentType(const std::string& content);
    
    /**
     * 检查是否为CHTL语法
     */
    bool isCHTLSyntax(const std::string& content);
    
    /**
     * 检查是否为CHTL JS语法
     */
    bool isCHTLJSSyntax(const std::string& content);
    
    /**
     * 检查是否为增强选择器 {{...}}
     */
    bool isEnhancedSelector(const std::string& content);
    
    // ========== 最小单元识别 ==========
    
    /**
     * 获取CHTL最小语法单元
     */
    std::vector<std::string> getCHTLMinimalUnits();
    
    /**
     * 获取CHTL JS最小语法单元
     */
    std::vector<std::string> getCHTLJSMinimalUnits();
    
    /**
     * 基于最小单元进行二次切割
     * 确保结果绝对精确，例如{{box}}->click将被切割为{{box}}->和click
     */
    std::vector<CodeFragment> preciseSlice(const std::string& content, FragmentType type);

private:
    /**
     * 报告扫描错误
     */
    void reportError(const std::string& message, size_t line = 0, size_t column = 0);
};

} // namespace CHTL