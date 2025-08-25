#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL_JS {

    struct DelegatedEvent {
        std::string ChildSelector;
        std::string EventType;
        std::string HandlerCode;
    };

    // Singleton registry to manage all event delegations.
    class EventDelegateRegistry {
    public:
        static EventDelegateRegistry& GetInstance() {
            static EventDelegateRegistry instance;
            return instance;
        }

        void Register(const std::string& parentSelector, const DelegatedEvent& delegatedEvent);

        // This will be called by the generator at the end of compilation.
        std::string GenerateFinalCode() const;

    private:
        EventDelegateRegistry() = default;
        std::map<std::string, std::vector<DelegatedEvent>> m_Delegations;
    };

}
