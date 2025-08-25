#pragma once

#include "UnifiedScanner/CHTLUnifiedScanner.h"
#include "CJMOD/CJMODPatternRegistry.h"
#include <string>
#include <vector>

namespace CHTL {

    // Implements dual-pointer scanning for CJMOD
    class CJMODProcessor : public ISliceProcessor {
    public:
        CJMODProcessor();
        
        std::vector<CodeSlice> Process(const CodeSlice& slice) override;
        
    private:
        // Dual-pointer scanning mechanism
        struct ScanResult {
            size_t Start;
            size_t End;
            std::string Keyword;
            std::vector<std::string> Arguments;
        };
        
        std::vector<ScanResult> DualPointerScan(const std::string& content);
        
        // Prefix capture mechanism
        std::string CapturePrefix(const std::string& content, size_t keywordPos, const std::string& keyword);
        
        // Helper methods
        bool IsKeywordBoundary(char before, char after) const;
        std::vector<std::string> ParseArguments(const std::string& argString);
        std::string GenerateReplacement(const std::string& keyword, const std::vector<std::string>& args);
    };

}