#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

// 代码片段类型
enum class FragmentType {
    CHTL,       // CHTL代码片段
    CHTLJS,     // CHTL JS代码片段  
    CSS,        // CSS代码片段
    JS,         // JavaScript代码片段
    UNKNOWN     // 未知类型
};

// 代码片段结构
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    
    CodeFragment(FragmentType t, const std::string& c, 
                 size_t sl, size_t sc, size_t el, size_t ec)
        : type(t), content(c), startLine(sl), startColumn(sc), 
          endLine(el), endColumn(ec) {}
};

// 扫描器配置
struct ScannerConfig {
    size_t initialSliceSize = 1024;     // 初始切片大小
    size_t maxSliceSize = 8192;         // 最大切片大小
    bool enableVariableLengthSlicing = true;  // 启用可变长度切片
    bool enableMinimalUnitSlicing = true;     // 启用最小单元切片
};

// CHTLUnifiedScanner - 精准代码切割器
class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const ScannerConfig& config = ScannerConfig());
    ~CHTLUnifiedScanner();
    
    // 扫描整个源代码，返回切割后的代码片段
    std::vector<CodeFragment> scan(const std::string& sourceCode);
    
    // 设置自定义的片段识别器
    void setFragmentRecognizer(FragmentType type, 
                               std::function<bool(const std::string&, size_t)> recognizer);
    
    // 重置扫描器状态
    void reset();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // 内部辅助方法
    bool isValidCutPoint(const std::string& content, size_t position);
    std::vector<CodeFragment> performSecondarySlicing(const CodeFragment& fragment);
    FragmentType detectFragmentType(const std::string& content, size_t position);
    
    // CHTL和CHTL JS最小单元检测
    bool isCHTLMinimalUnit(const std::string& content, size_t start, size_t end);
    bool isCHTLJSMinimalUnit(const std::string& content, size_t start, size_t end);
    
    // 扩展切片以确保完整性
    size_t expandSliceForward(const std::string& content, size_t position);
    size_t expandSliceBackward(const std::string& content, size_t position);
};

// 扫描器异常类
class ScannerException : public std::exception {
public:
    explicit ScannerException(const std::string& message) : msg(message) {}
    const char* what() const noexcept override { return msg.c_str(); }
    
private:
    std::string msg;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H