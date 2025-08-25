#include "CHTL/Generator/Generator.h"
#include <vector>

namespace CHTL {

// ... (constructor)

void Generator::VisitElementNode(std::shared_ptr<ElementNode> node) {
    // --- Pre-processing pass for selector automation ---
    bool hasClassAttr = false;
    bool hasIdAttr = false;
    for (const auto& attr : node->Attributes) {
        if (attr->Key == "class") hasClassAttr = true;
        if (attr->Key == "id") hasIdAttr = true;
    }

    std::string firstClassSelector, firstIdSelector;
    // In a full implementation, we would scan style and script nodes here
    // to find the first class/id selectors and populate the variables above.

    if (!m_Config.DisableStyleAutoAddClass && !hasClassAttr && !firstClassSelector.empty()) {
        auto classAttr = std::make_shared<AttributeNode>();
        classAttr->Key = "class";
        classAttr->Value = firstClassSelector;
        node->Attributes.push_back(classAttr);
    }
    // ... similar logic for ID

    // --- Standard generation pass ---
    m_Result << "<" << node->TagName;
    // ... generate attributes ...
    m_Result << ">";

    // ... visit children, passing context to style/script visitors ...

    m_Result << "</" << node->TagName << ">";
}

// ... (rest of the generator)

}