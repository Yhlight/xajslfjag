#pragma once

#include "CJMOD/CJMODPatternRegistry.h"
#include <string_view>
#include <optional>

namespace CHTL {

    class PatternMatcher {
    public:
        PatternMatcher(std::string_view source, const PatternInfo& patternInfo);

        // Attempts to match the pattern from the current source position.
        // Returns the captured args and the end position of the match if successful.
        std::optional<std::pair<CapturedArgs, size_t>> Match();

    private:
        std::string_view m_Source;
        const PatternInfo& m_PatternInfo;
    };

}
