#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL_JS {

    // Holds information for a single delegated event
    struct DelegateInfo {
        std::string TargetSelector;
        std::map<std::string, std::string> Events; // e.g., {"click": "handlerFunction"}
    };

    // Singleton to manage and merge all delegate() calls.
    class DelegateRegistry {
    public:
        static DelegateRegistry& GetInstance();

        void Register(const std::string& parentSelector, const DelegateInfo& info);

        // Generates the final, consolidated JavaScript code for all delegations.
        std::string GenerateFinalCode();

    private:
        DelegateRegistry() = default;
        std::map<std::string, std::vector<DelegateInfo>> m_Delegations;
    };

}
