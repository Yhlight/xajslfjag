#ifndef CHTL_SCANNER_UNIFIED_SCANNER_H
#define CHTL_SCANNER_UNIFIED_SCANNER_H

#include "Core/Common.h"
#include <queue>
#include <stack>

namespace CHTL {

// 前向声明
class SliceManager;
class TokenIdentifier;

// 扫描器模式
enum class ScanMode {
    Normal,         // 常规模式
    CHTL,          // CHTL代码块
    CHTLJS,        // CHTL JS代码块
    CSS,           // CSS代码块
    JavaScript,    // JavaScript代码块
    String,        // 字符串内部
    Comment,       // 注释内部
    Origin         // 原始嵌入内部
};

// 切片类型（更细化的分类）
enum class SliceCategory {
    // CHTL相关
    CHTLElement,           // CHTL元素
    CHTLAttribute,         // CHTL属性
    CHTLText,              // text { }
    CHTLStyle,             // style { } 局部样式块
    CHTLScript,            // script { } 局部脚本块
    CHTLTemplate,          // [Template]
    CHTLCustom,            // [Custom]
    CHTLOrigin,            // [Origin]
    CHTLImport,            // [Import]
    CHTLNamespace,         // [Namespace]
    CHTLConfiguration,     // [Configuration]
    
    // CHTL JS相关
    CHTLJSSelector,        // {{选择器}}
    CHTLJSArrow,           // ->
    CHTLJSFunction,        // listen, delegate, animate等
    CHTLJSVirtual,         // vir虚对象
    
    // 通用
    PlainCSS,              // 纯CSS代码
    PlainJavaScript,       // 纯JavaScript代码
    PlainHTML,             // 纯HTML代码
    
    // 特殊
    Comment,               // 注释
    GeneratorComment,      // --生成器注释
    Whitespace,            // 空白
    Unknown                // 未知类型
};

// 代码切片
struct ScannerSlice {
    SliceCategory category;
    String content;
    Position start;
    Position end;
    ScanMode mode;
    HashMap<String, String> metadata;  // 额外信息
    
    ScannerSlice() = default;
    ScannerSlice(SliceCategory cat, const String& cont, 
                const Position& s, const Position& e, ScanMode m)
        : category(cat), content(cont), start(s), end(e), mode(m) {}
};

// 扫描器配置
struct ScannerConfig {
    size_t initialSliceSize = 1024;      // 初始切片大小
    size_t maxSliceSize = 65536;         // 最大切片大小
    size_t minSliceSize = 64;            // 最小切片大小
    bool enableDoublePointerScan = true; // 启用双指针扫描
    bool enablePrefixCapture = true;     // 启用前置截取
    bool preserveWhitespace = false;     // 保留空白
    bool mergeAdjacentSlices = true;     // 合并相邻切片
};

// 统一扫描器
class UnifiedScanner {
private:
    // 源代码管理
    String sourceCode_;
    String filename_;
    size_t currentPos_ = 0;
    Position currentPosition_;
    
    // 扫描状态
    ScanMode currentMode_ = ScanMode::Normal;
    std::stack<ScanMode> modeStack_;
    
    // 切片管理
    UniquePtr<SliceManager> sliceManager_;
    Vector<ScannerSlice> slices_;
    
    // 配置
    ScannerConfig config_;
    
    // 标识符识别器
    UniquePtr<TokenIdentifier> tokenIdentifier_;
    
    // 双指针扫描支持
    struct DoublePointer {
        size_t front = 0;
        size_t back = 0;
        bool collecting = false;
    } doublePointer_;
    
public:
    UnifiedScanner();
    explicit UnifiedScanner(const ScannerConfig& config);
    ~UnifiedScanner();
    
    // 主要接口
    Result<Vector<ScannerSlice>> ScanCode(const String& code, const String& filename = "<input>");
    
    // 配置管理
    void SetConfig(const ScannerConfig& config) { config_ = config; }
    const ScannerConfig& GetConfig() const { return config_; }
    
    // 获取扫描结果
    const Vector<ScannerSlice>& GetSlices() const { return slices_; }
    
    // 清空状态
    void Reset();
    
private:
    // 核心扫描逻辑
    Result<void> PerformScan();
    
    // 可变长度切片读取
    Result<String> ReadVariableSlice();
    
    // 检查切片边界是否合理
    bool CheckSliceBoundary(const String& slice, size_t nextCharPos);
    
    // 扩展切片
    Result<String> ExtendSlice(const String& currentSlice, size_t extendSize);
    
    // 识别切片类型
    SliceCategory IdentifySliceCategory(const String& slice);
    
    // 处理特定类型的切片
    Result<void> ProcessCHTLSlice(const String& slice, const Position& startPos);
    Result<void> ProcessCHTLJSSlice(const String& slice, const Position& startPos);
    Result<void> ProcessCSSSlice(const String& slice, const Position& startPos);
    Result<void> ProcessJavaScriptSlice(const String& slice, const Position& startPos);
    
    // 二次切割（精准切割）
    Vector<ScannerSlice> SecondarySlice(const String& content, SliceCategory category, 
                                       const Position& startPos);
    
    // CHTL特殊语法识别
    bool IsCHTLKeyword(const String& token);
    bool IsCHTLJSKeyword(const String& token);
    bool IsCHTLBlockStart(const String& slice, size_t pos);
    bool IsCHTLBlockEnd(const String& slice, size_t pos);
    
    // CHTL JS特殊语法识别
    bool IsCHTLJSSelector(const String& slice, size_t pos);  // {{}}
    bool IsCHTLJSArrow(const String& slice, size_t pos);     // ->
    bool IsCHTLJSFunction(const String& token);              // listen, animate等
    
    // 双指针扫描机制
    Result<Vector<ScannerSlice>> DoublePointerScan(const String& content, 
                                                   const Position& startPos);
    
    // 前置截取机制
    Result<String> PrefixCapture(const String& content, size_t keywordPos, 
                                const String& keyword);
    
    // 模式管理
    void PushMode(ScanMode mode);
    void PopMode();
    ScanMode GetCurrentMode() const { return currentMode_; }
    
    // 位置追踪
    void UpdatePosition(char ch);
    Position GetCurrentPosition() const { return currentPosition_; }
    
    // 字符判断辅助函数
    bool IsWhitespace(char ch) const;
    bool IsNewline(char ch) const;
    bool IsIdentifierStart(char ch) const;
    bool IsIdentifierPart(char ch) const;
    
    // 读取辅助函数
    char PeekChar(size_t offset = 0) const;
    char NextChar();
    void SkipWhitespace();
    String ReadUntil(const String& delimiter);
    String ReadUntilAny(const Vector<String>& delimiters);
    
    // 注释处理
    Result<void> ProcessComment();
    bool IsCommentStart(size_t pos) const;
    
    // 字符串处理
    Result<void> ProcessString(char quote);
    
    // 合并相邻切片
    void MergeAdjacentSlices();
    
    // 添加切片
    void AddSlice(const ScannerSlice& slice);
    void AddSlice(SliceCategory category, const String& content, 
                 const Position& start, const Position& end);
};

// 切片管理器
class SliceManager {
private:
    Vector<ScannerSlice> slices_;
    HashMap<SliceCategory, Vector<size_t>> categoryIndex_;  // 分类索引
    
public:
    // 添加切片
    void AddSlice(const ScannerSlice& slice);
    
    // 获取切片
    const Vector<ScannerSlice>& GetAllSlices() const { return slices_; }
    Vector<ScannerSlice> GetSlicesByCategory(SliceCategory category) const;
    
    // 合并切片
    void MergeSlices(const std::function<bool(const ScannerSlice&, const ScannerSlice&)>& canMerge);
    
    // 优化切片
    void OptimizeSlices();
    
    // 清空
    void Clear();
};

// Token标识符识别器
class TokenIdentifier {
private:
    // CHTL关键字
    HashSet<String> chtlKeywords_;
    
    // CHTL JS关键字
    HashSet<String> chtljsKeywords_;
    
    // CHTL块标记
    HashSet<String> chtlBlockMarkers_;
    
public:
    TokenIdentifier();
    
    // 初始化关键字表
    void InitializeKeywords();
    
    // 识别函数
    bool IsCHTLKeyword(const String& token) const;
    bool IsCHTLJSKeyword(const String& token) const;
    bool IsCHTLBlockMarker(const String& token) const;
    
    // 获取Token类型
    Optional<String> GetTokenType(const String& token) const;
};

} // namespace CHTL

#endif // CHTL_SCANNER_UNIFIED_SCANNER_H