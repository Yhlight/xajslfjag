#include "Core/EventDelegateRegistry.h"
#include <sstream>

namespace CHTL_JS {

// ... (Register method)

std::string EventDelegateRegistry::GenerateFinalCode() const {
    std::stringstream finalCode;
    for (const auto& parentPair : m_Delegations) {
        const std::string& parentSelector = parentPair.first;
        const auto& allEvents = parentPair.second;

        // Group events by event type (e.g., all 'click' events together)
        std::map<std::string, std::vector<DelegatedEvent>> eventsByType;
        for (const auto& event : allEvents) {
            eventsByType[event.EventType].push_back(event);
        }

        // Create one listener for each event type
        for (const auto& typePair : eventsByType) {
            const std::string& eventType = typePair.first;
            const auto& events = typePair.second;

            finalCode << "document.querySelector('" << parentSelector << "').addEventListener('" << eventType << "', (e) => {\n";
            
            for (const auto& event : events) {
                finalCode << "  if (e.target.matches('" << event.ChildSelector << "')) {\n";
                finalCode << "    (" << event.HandlerCode << ")(e);\n";
                finalCode << "  }\n";
            }

            finalCode << "});\n";
        }
    }
    return finalCode.str();
}

}