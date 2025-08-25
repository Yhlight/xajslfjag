#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>
#include "../CJMOD/include/CJMOD/CJMOD_API.h"

namespace CHTL {

    class CHTLUnifiedScanner; // Forward declaration

    // Singleton registry for all CJMOD functions
    class CJMODRegistry {
    public:
        static CJMODRegistry& GetInstance() {
            static CJMODRegistry instance;
            return instance;
        }

        using Factory = std::function<std::shared_ptr<CHTLJSFunction>()>;

        void Register(const std::string& moduleName, const std::string& functionName, Factory factory);
        void Activate(const std::string& moduleName, CHTLUnifiedScanner* scanner);

    private:
        CJMODRegistry() = default;
        std::map<std::string, std::map<std::string, Factory>> m_Modules;
    };

}
