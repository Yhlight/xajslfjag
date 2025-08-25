#include "CHTL/Generator/SelectorScanner.h"
#include <regex>
#include <algorithm>

namespace CHTL {

    std::vector<SelectorInfo> SelectorScanner::ScanStyleBlock(const std::shared_ptr<StyleNode>& styleNode) {
        std::vector<SelectorInfo> selectors;
        
        for (const auto& rule : styleNode->Rules) {
            if (auto selectorBlock = std::dynamic_pointer_cast<SelectorBlockNode>(rule)) {
                SelectorInfo info = ParseSelector(selectorBlock->Selector);
                
                // Mark first class/id selectors
                if (info.Type == SelectorInfo::Class) {
                    bool hasClass = std::any_of(selectors.begin(), selectors.end(),
                        [](const SelectorInfo& s) { return s.Type == SelectorInfo::Class; });
                    if (!hasClass) info.IsFirstClass = true;
                }
                if (info.Type == SelectorInfo::Id) {
                    bool hasId = std::any_of(selectors.begin(), selectors.end(),
                        [](const SelectorInfo& s) { return s.Type == SelectorInfo::Id; });
                    if (!hasId) info.IsFirstId = true;
                }
                
                selectors.push_back(info);
            }
        }
        
        return selectors;
    }
    
    std::vector<SelectorInfo> SelectorScanner::ScanScriptBlock(const std::shared_ptr<ScriptNode>& scriptNode) {
        std::vector<SelectorInfo> selectors;
        
        // Extract CHTL JS selectors from script content
        std::vector<std::string> chtljsSelectors = ExtractCHTLJSSelectors(scriptNode->Content);
        
        for (const auto& sel : chtljsSelectors) {
            SelectorInfo info;
            
            // Parse CHTL JS selector format
            if (sel.substr(0, 2) == "{{" && sel.substr(sel.length() - 2) == "}}") {
                std::string innerSel = sel.substr(2, sel.length() - 4);
                
                if (innerSel[0] == '.') {
                    info.Selector = innerSel.substr(1);
                    info.Type = SelectorInfo::Class;
                } else if (innerSel[0] == '#') {
                    info.Selector = innerSel.substr(1);
                    info.Type = SelectorInfo::Id;
                } else {
                    // Plain selector like {{box}} - could be tag, class, or id
                    info.Selector = innerSel;
                    info.Type = SelectorInfo::Tag; // Default to tag
                }
                
                // Mark first occurrences
                if (info.Type == SelectorInfo::Class) {
                    bool hasClass = std::any_of(selectors.begin(), selectors.end(),
                        [](const SelectorInfo& s) { return s.Type == SelectorInfo::Class; });
                    if (!hasClass) info.IsFirstClass = true;
                }
                if (info.Type == SelectorInfo::Id) {
                    bool hasId = std::any_of(selectors.begin(), selectors.end(),
                        [](const SelectorInfo& s) { return s.Type == SelectorInfo::Id; });
                    if (!hasId) info.IsFirstId = true;
                }
                
                selectors.push_back(info);
            }
        }
        
        return selectors;
    }
    
    std::pair<std::string, std::string> SelectorScanner::GetFirstSelectors(const std::vector<SelectorInfo>& selectors) {
        std::string firstClass;
        std::string firstId;
        
        for (const auto& sel : selectors) {
            if (sel.IsFirstClass && firstClass.empty()) {
                firstClass = sel.Selector;
            }
            if (sel.IsFirstId && firstId.empty()) {
                firstId = sel.Selector;
            }
        }
        
        return {firstClass, firstId};
    }
    
    SelectorInfo SelectorScanner::ParseSelector(const std::string& selector) {
        SelectorInfo info;
        info.Selector = selector;
        
        if (selector[0] == '.') {
            info.Type = SelectorInfo::Class;
            info.Selector = selector.substr(1);
        } else if (selector[0] == '#') {
            info.Type = SelectorInfo::Id;
            info.Selector = selector.substr(1);
        } else if (selector.find('.') != std::string::npos || 
                   selector.find('#') != std::string::npos ||
                   selector.find(':') != std::string::npos ||
                   selector.find(' ') != std::string::npos) {
            info.Type = SelectorInfo::Compound;
        } else {
            info.Type = SelectorInfo::Tag;
        }
        
        return info;
    }
    
    std::vector<std::string> SelectorScanner::ExtractCHTLJSSelectors(const std::string& scriptContent) {
        std::vector<std::string> selectors;
        std::regex selectorRegex(R"(\{\{[^}]+\}\})");
        
        auto begin = std::sregex_iterator(scriptContent.begin(), scriptContent.end(), selectorRegex);
        auto end = std::sregex_iterator();
        
        for (std::sregex_iterator i = begin; i != end; ++i) {
            selectors.push_back(i->str());
        }
        
        return selectors;
    }

}