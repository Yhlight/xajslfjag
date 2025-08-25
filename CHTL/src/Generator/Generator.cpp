#include "CHTL/Generator/Generator.h"
#include "CHTL/Generator/SelectorScanner.h"
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
    
    // Scan style and script nodes in children
    std::vector<SelectorInfo> allSelectors;
    
    for (const auto& child : node->Children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            auto styleSelectors = SelectorScanner::ScanStyleBlock(styleNode);
            allSelectors.insert(allSelectors.end(), styleSelectors.begin(), styleSelectors.end());
        } else if (auto scriptNode = std::dynamic_pointer_cast<ScriptNode>(child)) {
            auto scriptSelectors = SelectorScanner::ScanScriptBlock(scriptNode);
            allSelectors.insert(allSelectors.end(), scriptSelectors.begin(), scriptSelectors.end());
        }
    }
    
    auto [firstClass, firstId] = SelectorScanner::GetFirstSelectors(allSelectors);
    
    // Auto-add class attribute if needed
    if (!m_Config.DisableStyleAutoAddClass && !hasClassAttr && !firstClass.empty()) {
        auto classAttr = std::make_shared<AttributeNode>();
        classAttr->Key = "class";
        classAttr->Value = firstClass;
        node->Attributes.push_back(classAttr);
    }
    
    // Auto-add id attribute if needed
    if (!m_Config.DisableStyleAutoAddId && !hasIdAttr && !firstId.empty()) {
        auto idAttr = std::make_shared<AttributeNode>();
        idAttr->Key = "id";
        idAttr->Value = firstId;
        node->Attributes.push_back(idAttr);
    }

    // --- Standard generation pass ---
    m_Result << "<" << node->TagName;
    
    // Generate attributes
    for (const auto& attr : node->Attributes) {
        m_Result << " " << attr->Key << "=\"";
        if (std::holds_alternative<std::string>(attr->Value)) {
            m_Result << std::get<std::string>(attr->Value);
        }
        m_Result << "\"";
    }
    
    m_Result << ">";

    // Visit children
    for (const auto& child : node->Children) {
        // Visit child nodes
    }

    m_Result << "</" << node->TagName << ">";
}

// ... (rest of the generator)

}