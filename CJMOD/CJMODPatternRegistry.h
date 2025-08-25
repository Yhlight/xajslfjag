#pragma once

#include "include/CJMOD/API_raw.h"
#include <string>
#include <map>

namespace CHTL {

    struct PatternInfo {
        std::string FullPattern;
        CJMODHandlerFn Handler;
    };

    // Singleton registry for raw CJMOD patterns
    class CJMODPatternRegistry {
    public:
        static CJMODPatternRegistry& GetInstance() {
            static CJMODPatternRegistry instance;
            return instance;
        }

        void Register(const std::string& triggerKeyword, const std::string& pattern, CJMODHandlerFn handler);
        const PatternInfo* GetPatternInfo(const std::string& triggerKeyword) const;

    private:
        CJMODPatternRegistry() = default;
        std::map<std::string, PatternInfo> m_Patterns;
    };

}
