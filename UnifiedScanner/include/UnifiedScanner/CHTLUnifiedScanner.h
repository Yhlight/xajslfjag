#pragma once

#include <string>
#include <vector>
#include <memory>
#include "CHTL/Core/Config.h"

namespace CHTL {

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
        size_t StartLine;
        size_t StartColumn;
    };

    // Interface for slice processors
    class ISliceProcessor {
    public:
        virtual ~ISliceProcessor() = default;
        virtual std::vector<CodeSlice> Process(const CodeSlice& slice) = 0;
    };

    class CHTLUnifiedScanner {
    public:
        CHTLUnifiedScanner(const std::string& source, const Config& config);

        std::vector<CodeSlice> Scan();
        
        // Register external processors (like CJMOD)
        void RegisterProcessor(SliceType type, std::shared_ptr<ISliceProcessor> processor);

    private:
        // Core scanning methods
        bool IsAtEnd() const;
        char Advance();
        char Peek() const;
        char PeekNext() const;
        bool Match(const std::string& expected);
        void SkipWhitespace();
        
        // Slice detection methods
        SliceType DetectSliceType();
        CodeSlice ScanCHTLSlice();
        CodeSlice ScanStyleSlice();
        CodeSlice ScanScriptSlice();
        CodeSlice ScanTextSlice();
        
        // Helper methods
        std::string ScanUntil(const std::string& delimiter);
        bool IsInsideString() const;
        void UpdatePosition(char c);
        
    private:
        const std::string& m_Source;
        const Config& m_Config;
        size_t m_Current = 0;
        size_t m_Line = 1;
        size_t m_Column = 1;
        std::map<SliceType, std::shared_ptr<ISliceProcessor>> m_Processors;
    };

}
