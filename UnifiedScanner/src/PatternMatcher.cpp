#include "UnifiedScanner/PatternMatcher.h"
#include <sstream>

namespace CHTL {

// ... (constructor)

std::optional<std::pair<CapturedArgs, size_t>> PatternMatcher::Match() {
    std::stringstream patternStream(m_PatternInfo.FullPattern);
    std::string patternToken;
    size_t sourcePos = 0;
    CapturedArgs capturedArgs;

    std::vector<std::string> patternTokens;
    while (patternStream >> patternToken) {
        patternTokens.push_back(patternToken);
    }

    for (size_t i = 0; i < patternTokens.size(); ++i) {
        patternToken = patternTokens[i];
        // ... (skip whitespace)

        bool isOptional = patternToken.length() > 2 && patternToken[0] == '$' && patternToken.back() == '?';
        bool isVariadic = patternToken == "...";

        if (isVariadic) {
            // Greedily capture everything until the end (or a future delimiter)
            // This is a simplified implementation.
            capturedArgs["$variadic"] = m_Source.substr(sourcePos);
            sourcePos = m_Source.length();
            break; // Assume variadic is always last
        }

        if (patternToken[0] == '$') {
            // Capture argument
            std::string nextKeyword = (i + 1 < patternTokens.size()) ? patternTokens[i+1] : "";
            size_t argStart = sourcePos;
            size_t argEnd = nextKeyword.empty() ? m_Source.length() : m_Source.find(nextKeyword, sourcePos);

            if (argEnd == std::string::npos) {
                if (isOptional) {
                    capturedArgs[patternToken] = ""; // Optional arg not found
                    continue; // Skip to the next pattern token
                } else {
                    return std::nullopt; // Required arg delimiter not found
                }
            }
            // ... (capture logic as before)
        } else {
            // Match keyword
            // ... (as before)
        }
    }

    return std::make_pair(capturedArgs, sourcePos);
}

}