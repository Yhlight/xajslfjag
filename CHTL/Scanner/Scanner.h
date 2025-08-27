#ifndef CHTL_SCANNER_H
#define CHTL_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// 代码片段类型
enum class FragmentType {
    CHTL,       // CHTL代码片段
    CHTL_JS,    // CHTL JS代码片段
    CSS,        // CSS代码片段
    JS,         // JavaScript代码片段
    MIXED       // 混合片段（需要进一步分析）
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    std::string content;
    int startLine;
    int startColumn;
    int endLine;
    int endColumn;
    
    // 元数据
    bool isInLocalStyle;    // 是否在局部样式块中
    bool isInLocalScript;   // 是否在局部脚本块中
    bool isInGlobalStyle;   // 是否在全局样式块中
    bool isInGlobalScript;  // 是否在全局脚本块中
    
    CodeFragment(FragmentType t, const std::string& c, int sl, int sc, int el, int ec)
        : type(t), content(c), startLine(sl), startColumn(sc), endLine(el), endColumn(ec),
          isInLocalStyle(false), isInLocalScript(false), 
          isInGlobalStyle(false), isInGlobalScript(false) {}
};

// 扫描器上下文
struct ScannerContext {
    bool inCHTLBlock;
    bool inStyleBlock;
    bool inScriptBlock;
    bool inTextBlock;
    bool inOriginBlock;
    int braceDepth;
    int bracketDepth;
    std::string currentElement;
    std::vector<std::string> elementStack;
    
    ScannerContext() 
        : inCHTLBlock(false), inStyleBlock(false), inScriptBlock(false),
          inTextBlock(false), inOriginBlock(false), braceDepth(0), bracketDepth(0) {}
};

// 统一扫描器类
class Scanner {
private:
    std::string source;
    size_t position;
    ScannerContext context;
    std::vector<CodeFragment> fragments;
    
    // 基于可变长度切片的扫描
    static const size_t MIN_SLICE_SIZE = 128;
    static const size_t MAX_SLICE_SIZE = 4096;
    size_t currentSliceSize;
    
    // 辅助方法
    char peekChar(int offset = 0) const;
    std::string peekString(size_t length) const;
    void advance(size_t count = 1);
    bool isAtEnd() const;
    
    // 片段识别
    FragmentType identifyFragment(const std::string& content);
    bool isCHTLKeyword(const std::string& word);
    bool isCHTLJSFeature(const std::string& content);
    bool isValidCutPoint(size_t pos);
    
    // 二次切割
    std::vector<CodeFragment> splitFragment(const CodeFragment& fragment);
    std::vector<CodeFragment> splitCHTLFragment(const CodeFragment& fragment);
    std::vector<CodeFragment> splitCHTLJSFragment(const CodeFragment& fragment);
    
    // 上下文更新
    void updateContext(const std::string& content);
    void enterBlock(const std::string& blockType);
    void exitBlock();
    
    // 扫描方法
    CodeFragment scanNextFragment();
    void adjustSliceSize(bool needExpand);
    
    // CJMOD支持的双指针扫描
    struct DoublePointer {
        size_t front;
        size_t back;
        bool collecting;
        std::string keyword;
    };
    
    std::vector<CodeFragment> doublePointerScan(const std::string& content, const std::string& keyword);
    std::string prefixCapture(const std::string& content, const std::string& keyword);

public:
    Scanner(const std::string& src);
    ~Scanner() = default;
    
    // 执行扫描
    std::vector<CodeFragment> scan();
    
    // 获取指定类型的片段
    std::vector<CodeFragment> getFragmentsByType(FragmentType type);
    
    // 合并连续的同类型片段
    std::vector<CodeFragment> mergeFragments(const std::vector<CodeFragment>& fragments);
    
    // 验证片段完整性
    bool validateFragments();
    
    // 调试输出
    void dumpFragments() const;
    
    // CJMOD扫描接口
    std::vector<CodeFragment> scanForCJMOD(const std::string& keyword);
};

} // namespace CHTL

#endif // CHTL_SCANNER_H