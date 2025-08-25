#pragma once

#include <string>
#include <vector>
#include <memory>
#include "CHTL/Core/Ast.h"

namespace CHTL {

    struct SelectorInfo {
        std::string Selector;
        enum Type { Class, Id, Tag, Compound } Type;
        bool IsFirstClass = false;
        bool IsFirstId = false;
    };

    class SelectorScanner {
    public:
        // Scan style blocks for selectors
        static std::vector<SelectorInfo> ScanStyleBlock(const std::shared_ptr<StyleNode>& styleNode);
        
        // Scan script blocks for CHTL JS selectors ({{.class}}, {{#id}}, etc.)
        static std::vector<SelectorInfo> ScanScriptBlock(const std::shared_ptr<ScriptNode>& scriptNode);
        
        // Extract first class/id selectors from a list
        static std::pair<std::string, std::string> GetFirstSelectors(const std::vector<SelectorInfo>& selectors);
        
    private:
        static SelectorInfo ParseSelector(const std::string& selector);
        static std::vector<std::string> ExtractCHTLJSSelectors(const std::string& scriptContent);
    };

}