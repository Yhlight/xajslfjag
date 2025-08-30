#pragma once
#include "../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

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
    bool inStringLiteral;               // 是否在字符串字面量中
    bool inComment;                     // 是否在注释中
    
    ScanContext() : position(0), lineNumber(1), columnNumber(1), 
                   inCHTLBlock(false), inCHTLJSBlock(false), 
                   inStringLiteral(false), inComment(false) {}
};

/**
 * CHTL统一扫描器
 * 负责精准代码切割，将源代码分解为不同类型的片段
 */
class CHTLUnifiedScanner {
public:
    /**
     * 构造函数
     */
    CHTLUnifiedScanner();
    
    /**
     * 析构函数
     */
    ~CHTLUnifiedScanner() = default;
    
    // ========== 主扫描功能 ==========
    
    /**
     * 扫描源代码，返回代码片段列表
     */
    std::vector<CodeFragment> scan(const std::string& source);
    
    /**
     * 扫描单个片段（用于可变长度切片）
     */
    CodeFragment scanFragment(const std::string& source, size_t& position);
    
    // ========== CHTL/CHTL JS最小单元切割 ==========
    
    /**
     * 将CHTL片段切割为最小单元
     */
    std::vector<CodeFragment> splitCHTLToMinimalUnits(const CodeFragment& fragment);
    
    /**
     * 将CHTL JS片段切割为最小单元
     */
    std::vector<CodeFragment> splitCHTLJSToMinimalUnits(const CodeFragment& fragment);
    
    // ========== 可变长度切片 ==========
    
    /**
     * 智能切片：根据上下文调整切片长度
     */
    std::vector<CodeFragment> smartSlice(const std::string& source, size_t sliceSize = 1024);
    
    /**
     * 检查切片边界是否合理
     */
    bool isValidSliceBoundary(const std::string& source, size_t position);
    
    /**
     * 扩展切片以包含完整语法单元
     */
    size_t extendSliceForCompleteness(const std::string& source, size_t start, size_t end);
    
    // ========== 语法识别 ==========
    
    /**
     * 识别片段类型
     */
    FragmentType identifyFragmentType(const std::string& content, const ScanContext& context);
    
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
    
    /**
     * 检查是否为CHTL JS函数
     */
    bool isCHTLJSFunction(const std::string& content);
    
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
     * 检查是否为完整的语法单元
     */
    bool isCompleteSyntaxUnit(const std::string& content, FragmentType type);
    
    // ========== 双指针扫描（CJMOD支持）==========
    
    /**
     * 双指针扫描：用于CJMOD片段检测
     */
    std::vector<CodeFragment> doublePointerScan(const std::string& source, 
                                               const std::string& keyword);
    
    /**
     * 前置截取：从其他片段中截取回CJMOD相关内容
     */
    CodeFragment frontTruncate(const std::vector<CodeFragment>& fragments, 
                              const std::string& keyword);
    
    // ========== 工具方法 ==========
    
    /**
     * 生成扫描报告
     */
    std::string generateScanReport(const std::vector<CodeFragment>& fragments);
    
    /**
     * 验证片段完整性
     */
    bool validateFragmentIntegrity(const std::vector<CodeFragment>& fragments, 
                                  const std::string& originalSource);
    
    /**
     * 设置调试模式
     */
    void setDebugMode(bool enabled);

private:
    bool m_debugMode;                                   // 调试模式
    std::unordered_map<std::string, FragmentType> m_syntaxPatterns; // 语法模式映射
    
    // ========== 私有扫描方法 ==========
    
    /**
     * 扫描字符串字面量
     */
    size_t scanStringLiteral(const std::string& source, size_t position, char quote);
    
    /**
     * 扫描注释
     */
    size_t scanComment(const std::string& source, size_t position);
    
    /**
     * 扫描CHTL块
     */
    CodeFragment scanCHTLBlock(const std::string& source, size_t& position);
    
    /**
     * 扫描CHTL JS块
     */
    CodeFragment scanCHTLJSBlock(const std::string& source, size_t& position);
    
    /**
     * 扫描大括号块
     */
    size_t scanBraceBlock(const std::string& source, size_t position);
    
    /**
     * 更新扫描位置信息
     */
    void updatePosition(ScanContext& context, char ch);
    
    /**
     * 跳过空白字符
     */
    size_t skipWhitespace(const std::string& source, size_t position);
    
    /**
     * 初始化语法模式
     */
    void initializeSyntaxPatterns();
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message, size_t line = 0, size_t column = 0);
};

} // namespace CHTL