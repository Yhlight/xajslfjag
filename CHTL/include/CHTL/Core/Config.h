#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace CHTL {

    // Enum to represent all configurable keywords and types
    enum class ConfigurableKeyword {
        // Keywords
        KEYWORD_INHERIT,
        KEYWORD_DELETE,
        KEYWORD_INSERT,
        KEYWORD_AFTER,
        KEYWORD_BEFORE,
        KEYWORD_REPLACE,
        KEYWORD_ATTOP,
        KEYWORD_ATBOTTOM,
        KEYWORD_FROM,
        KEYWORD_AS,
        KEYWORD_EXCEPT,
        KEYWORD_TEXT,
        KEYWORD_STYLE,       // Local style block
        KEYWORD_SCRIPT,      // Local script block
        KEYWORD_USE,         // use
        KEYWORD_HTML5,       // html5

        // Block Keywords
        KEYWORD_CUSTOM,
        KEYWORD_TEMPLATE,
        KEYWORD_ORIGIN,
        KEYWORD_IMPORT,
        KEYWORD_NAMESPACE,
        KEYWORD_CONFIGURATION,

        // Type Identifiers (used with @)
        TYPE_CUSTOM_STYLE,
        TYPE_CUSTOM_ELEMENT,
        TYPE_CUSTOM_VAR,
        TYPE_TEMPLATE_STYLE,
        TYPE_TEMPLATE_ELEMENT,
        TYPE_TEMPLATE_VAR,
        TYPE_ORIGIN_HTML,
        TYPE_ORIGIN_STYLE,
        TYPE_ORIGIN_JAVASCRIPT,
        TYPE_IMPORT_HTML,
        TYPE_IMPORT_STYLE,
        TYPE_IMPORT_JAVASCRIPT,
        TYPE_IMPORT_CHTL,
        TYPE_IMPORT_CJMOD,
        TYPE_IMPORT_CONFIG,
        TYPE_CONFIGURATION_CONFIG
    };

    struct Config {
        // General settings
        int IndexInitialCount = 0;
        bool DisableNameGroup = true;
        bool DisableCustomOriginType = false;
        bool DebugMode = false;
        bool DisableDefaultNamespace = false;

        // Selector and auto-add rules
        bool DisableStyleAutoAddClass = false;
        bool DisableStyleAutoAddId = false;
        bool DisableScriptAutoAddClass = true;
        bool DisableScriptAutoAddId = true;

        // Name group settings
        std::map<ConfigurableKeyword, std::vector<std::string>> Names;
        int OptionCountLimit = 3;

        // OriginType settings
        std::map<std::string, std::string> OriginTypes;

        Config(); // Constructor to set default values

        // Helper to get the primary name for a keyword
        const std::string& GetName(ConfigurableKeyword keyword) const {
            return Names.at(keyword).front();
        }
    };

}