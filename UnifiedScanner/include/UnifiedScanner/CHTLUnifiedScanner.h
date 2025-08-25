#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <string_view>

namespace CHTL {
    // Forward declarations
    class Config;
    class CHTLJSFunction;

    enum class SliceType {
        CHTL,
        CHTL_JS,
        CSS,
        JS,
        UNKNOWN
    };

    struct CodeSlice {
        std::string Content;
        SliceType Type;
        size_t StartPosition;  // 在原始源码中的起始位置
        size_t EndPosition;    // 在原始源码中的结束位置
        int LineNumber;        // 起始行号
    };

    class CHTLUnifiedScanner {
    public:
        CHTLUnifiedScanner(const std::string& source, const Config& config);
        
        // 主扫描方法
        std::vector<CodeSlice> Scan();
        
        // CJMOD函数注册
        void RegisterFunction(std::shared_ptr<CHTLJSFunction> func);

    private:
        // 扫描状态
        enum class ScanState {
            CHTL,           // 正在扫描CHTL代码
            LocalStyle,     // 在局部样式块中
            LocalScript,    // 在局部脚本块中
            GlobalStyle,    // 在全局样式块中
            GlobalScript    // 在全局脚本块中
        };

        // 辅助方法
        bool IsAtEnd() const;
        char Peek(size_t offset = 0) const;
        char Advance();
        bool Match(const std::string& expected);
        void SkipWhitespace();
        
        // 切片处理
        void AddSlice(SliceType type, size_t start, size_t end);
        bool IsContextualBreakpoint(size_t position);
        void PerformSecondarySlicing(CodeSlice& slice);
        
        // 模式识别
        SliceType IdentifySliceType(const std::string& content);
        bool IsCHTLPattern(std::string_view content);
        bool IsCHTLJSPattern(std::string_view content);
        
        // 特殊块处理
        void ScanLocalStyleBlock();
        void ScanLocalScriptBlock();
        void ScanGlobalBlock();
        
        // Script块内的CHTL JS处理
        std::vector<CodeSlice> ProcessScriptBlock(const std::string& content);
        
        // 成员变量
        const std::string& m_Source;
        const Config& m_Config;
        size_t m_Current = 0;
        size_t m_Start = 0;
        int m_Line = 1;
        ScanState m_State = ScanState::CHTL;
        
        std::vector<CodeSlice> m_Slices;
        std::vector<std::shared_ptr<CHTLJSFunction>> m_RegisteredFunctions;
        std::map<std::string, std::function<void()>> m_KeywordCallbacks;
    };

}