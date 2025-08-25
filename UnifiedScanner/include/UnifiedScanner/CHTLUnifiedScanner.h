#pragma once

#include <string>
#include <vector>

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
    };

    class CHTLUnifiedScanner {
    public:
        CHTLUnifiedScanner(const std::string& source);

        std::vector<CodeSlice> Scan();
        #include "CJMOD/CJMOD_API.h"
#include <functional>

// ...

    class CHTLUnifiedScanner {
    public:
        // ...
        void RegisterFunction(std::shared_ptr<CHTLJSFunction> func);

    private:
        std::vector<std::shared_ptr<CHTLJSFunction>> m_RegisteredFunctions;
        // ...
    };

    private:
        std::vector<CodeSlice> ProcessScriptBlock(const std::string& content);
        std::map<std::string, std::function<void()>> m_KeywordCallbacks;
        // ... private helper methods for scanning
        const std::string& m_Source;
        size_t m_Current = 0;
    };

}
