#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CompilerDispatcher/CompilerDispatcher.h"

namespace CHTL {

// 扫描器状态
enum class ScannerState {
    NORMAL,         // 普通CHTL代码
    IN_STYLE,       // 在style块中
    IN_SCRIPT,      // 在script块中
    IN_COMMENT,     // 在注释中
    IN_STRING,      // 在字符串中
    IN_TEMPLATE,    // 在模板中
    IN_ORIGIN,      // 在原始嵌入中
};

// 上下文栈项
struct ContextItem {
    std::string type;       // 上下文类型（element, style, script等）
    std::string name;       // 名称（如元素名）
    int startLine;
    int startColumn;
    std::string attributes; // 相关属性
};

// 统一扫描器 - 负责精准代码切割
class Scanner {
public:
    Scanner();
    ~Scanner();
    
    // 设置源代码
    void SetSource(const std::string& source);
    
    // 获取下一个代码片段
    CodeFragment NextFragment();
    
    // 检查是否到达文件末尾
    bool IsEOF() const;
    
    // 配置
    void SetDebugMode(bool debug) { debugMode = debug; }
    void SetSliceSize(size_t size) { sliceSize = size; }
    
    // 获取当前位置
    int GetLine() const { return currentLine; }
    int GetColumn() const { return currentColumn; }
    
private:
    std::string source;
    size_t position;
    int currentLine;
    int currentColumn;
    
    // 配置
    bool debugMode;
    size_t sliceSize;      // 切片大小
    
    // 状态管理
    ScannerState state;
    std::vector<ContextItem> contextStack;
    
    // 缓冲区
    std::string buffer;
    FragmentType bufferType;
    int bufferStartLine;
    int bufferStartColumn;
    
    // 内部方法
    char CurrentChar() const;
    char PeekChar(int offset = 1) const;
    void Advance();
    void AdvanceN(int n);
    
    // 扫描方法
    bool ScanCHTL();
    bool ScanStyle();
    bool ScanScript();
    bool ScanComment();
    bool ScanString(char quote);
    bool ScanOrigin();
    
    // 判断方法
    bool IsStartOfStyle() const;
    bool IsStartOfScript() const;
    bool IsStartOfCHTLJS() const;
    bool IsEndOfBlock() const;
    
    // 切片处理
    std::string ReadSlice();
    bool IsSliceBoundaryValid(const std::string& slice) const;
    void AdjustSliceBoundary(std::string& slice);
    
    // CHTL JS特征检测
    bool ContainsCHTLJSFeatures(const std::string& content) const;
    std::vector<std::pair<size_t, size_t>> FindCHTLJSBlocks(const std::string& content) const;
    
    // 上下文管理
    void PushContext(const std::string& type, const std::string& name);
    void PopContext();
    std::string GetCurrentContext() const;
    
    // 创建片段
    CodeFragment CreateFragment(FragmentType type, const std::string& content);
    
    // 错误处理
    void ReportError(const std::string& message);
    std::vector<std::string> errors;
};

} // namespace CHTL