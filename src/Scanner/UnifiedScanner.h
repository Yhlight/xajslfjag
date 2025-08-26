#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "CodeSlice.h"
#include "ScannerState.h"
#include "Common/SourceFile.h"
#include "Common/Error.h"

namespace CHTL {

// 统一扫描器 - CHTL项目的核心架构
class UnifiedScanner {
public:
    // 扫描配置
    struct ScanConfig {
        size_t initialSliceSize = 1024;     // 初始切片大小
        size_t maxSliceSize = 8192;         // 最大切片大小
        size_t minSliceSize = 64;           // 最小切片大小
        bool mergeAdjacentSlices = true;    // 是否合并相邻的相同类型切片
        bool debugMode = false;             // 调试模式
    };
    
    UnifiedScanner(const ScanConfig& config = ScanConfig());
    
    // 扫描源文件并返回代码切片
    std::vector<CodeSlicePtr> Scan(const SourceFilePtr& sourceFile, ErrorCollector& errorCollector);
    
    // 获取扫描状态
    const ScannerState& GetState() const { return state_; }
    
private:
    // CHTL语法标记检测
    struct CHTLMarker {
        std::string pattern;
        std::function<bool(const std::string&, size_t)> validator;
        ScannerState::BlockType blockType;
        SliceType sliceType;
    };
    
    // CHTL JS语法标记检测
    struct CHTLJSMarker {
        std::string pattern;
        SliceType sliceType;
        bool requiresContext;  // 是否需要特定上下文
    };
    
    // 内部扫描方法
    void InitializeMarkers();
    
    // 切片处理
    CodeSlicePtr ProcessSlice(const std::string& content, size_t startOffset, size_t endOffset);
    
    // 检测CHTL语法
    bool DetectCHTLSyntax(const std::string& content, size_t offset, size_t& syntaxEnd);
    
    // 检测CHTL JS语法
    bool DetectCHTLJSSyntax(const std::string& content, size_t offset, size_t& syntaxEnd);
    
    // 检测块的开始和结束
    bool DetectBlockStart(const std::string& content, size_t offset, std::string& blockName, ScannerState::BlockType& blockType);
    bool DetectBlockEnd(const std::string& content, size_t offset);
    
    // 细分切片为最小单元
    std::vector<CodeSlicePtr> SplitToMinimalUnits(const CodeSlicePtr& slice);
    
    // 判断切片类型
    SliceType DetermineSliceType(const std::string& content, size_t offset);
    
    // 处理特殊语法
    bool HandleComment(const std::string& content, size_t& offset, std::vector<CodeSlicePtr>& slices);
    bool HandleString(const std::string& content, size_t& offset);
    bool HandleCHTLKeyword(const std::string& content, size_t& offset, std::vector<CodeSlicePtr>& slices);
    bool HandleCHTLJSEnhancedSelector(const std::string& content, size_t& offset, std::vector<CodeSlicePtr>& slices);
    
    // 辅助方法
    bool IsWhitespace(char ch) const { return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'; }
    bool IsIdentifierChar(char ch) const { return std::isalnum(ch) || ch == '_' || ch == '-'; }
    bool IsIdentifierStart(char ch) const { return std::isalpha(ch) || ch == '_'; }
    
    // 向前查看
    bool LookAhead(const std::string& content, size_t offset, const std::string& pattern) const;
    
    // 跳过空白
    size_t SkipWhitespace(const std::string& content, size_t offset) const;
    
    // 读取标识符
    std::string ReadIdentifier(const std::string& content, size_t& offset) const;
    
    // 读取到特定字符
    std::string ReadUntil(const std::string& content, size_t& offset, char delimiter) const;
    std::string ReadUntil(const std::string& content, size_t& offset, const std::string& delimiter) const;
    
    // 创建位置信息
    Location CreateLocation(size_t offset) const;
    
    // 调试输出
    void DebugLog(const std::string& message) const;
    
private:
    ScanConfig config_;
    ScannerState state_;
    SourceFilePtr currentFile_;
    ErrorCollector* errorCollector_;
    
    // 预定义的CHTL标记
    std::vector<CHTLMarker> chtlMarkers_;
    
    // 预定义的CHTL JS标记
    std::vector<CHTLJSMarker> chtljsMarkers_;
};

} // namespace CHTL