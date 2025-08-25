#include "UnifiedScanner/CHTLUnifiedScanner.h"
#include "UnifiedScanner/PatternMatcher.h"
#include "CJMOD/CJMODPatternRegistry.h"
#include <iostream>

namespace CHTL {

// ... (constructor, other methods)

std::vector<CodeSlice> CHTLUnifiedScanner::Scan() {
    std::vector<CodeSlice> slices;
    size_t lastCut = 0;

    while (m_Current < m_Source.length()) {
        // ... (logic to find script blocks)

        // Let's assume we found a script block and its content is in a string variable `scriptContent`
        std::string scriptContent = "...";
        std::string originalContent = scriptContent;

        for (size_t i = 0; i < scriptContent.length(); ++i) {
            if (isalpha(scriptContent[i])) { // Potential trigger keyword
                size_t start = i;
                while (i < scriptContent.length() && isalnum(scriptContent[i])) {
                    i++;
                }
                std::string keyword = scriptContent.substr(start, i - start);
                
                const PatternInfo* info = CJMODPatternRegistry::GetInstance().GetPatternInfo(keyword);
                if (info) {
                    std::string_view sourceView(scriptContent.data() + start, scriptContent.length() - start);
                    PatternMatcher matcher(sourceView, *info);
                    auto result = matcher.Match();

                    if (result.has_value()) {
                        auto [args, endPos] = result.value();
                        std::string generatedJs = info->Handler(args);

                        // Replace the original source with the generated code
                        scriptContent.replace(start, endPos, generatedJs);
                        
                        // Adjust scanner position
                        i = start + generatedJs.length() - 1;
                    }
                }
            }
        }
        slices.push_back({scriptContent, SliceType::JS}); // Add the processed script
    }
    return slices;
}

}