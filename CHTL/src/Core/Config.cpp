#include "CHTL/Core/Config.h"

namespace CHTL {

Config::Config() {
    // Initialize all default names as per the documentation

    // Keywords
    Names[ConfigurableKeyword::KEYWORD_INHERIT] = {"inherit"};
    Names[ConfigurableKeyword::KEYWORD_DELETE] = {"delete"};
    Names[ConfigurableKeyword::KEYWORD_INSERT] = {"insert"};
    Names[ConfigurableKeyword::KEYWORD_AFTER] = {"after"};
    Names[ConfigurableKeyword::KEYWORD_BEFORE] = {"before"};
    Names[ConfigurableKeyword::KEYWORD_REPLACE] = {"replace"};
    Names[ConfigurableKeyword::KEYWORD_ATTOP] = {"at top"};
    Names[ConfigurableKeyword::KEYWORD_ATBOTTOM] = {"at bottom"};
    Names[ConfigurableKeyword::KEYWORD_FROM] = {"from"};
    Names[ConfigurableKeyword::KEYWORD_AS] = {"as"};
    Names[ConfigurableKeyword::KEYWORD_EXCEPT] = {"except"};
    Names[ConfigurableKeyword::KEYWORD_TEXT] = {"text"};
    Names[ConfigurableKeyword::KEYWORD_STYLE] = {"style"};
    Names[ConfigurableKeyword::KEYWORD_SCRIPT] = {"script"};
    Names[ConfigurableKeyword::KEYWORD_USE] = {"use"};
    Names[ConfigurableKeyword::KEYWORD_HTML5] = {"html5"};

    // Block Keywords
    Names[ConfigurableKeyword::KEYWORD_CUSTOM] = {"[Custom]"};
    Names[ConfigurableKeyword::KEYWORD_TEMPLATE] = {"[Template]"};
    Names[ConfigurableKeyword::KEYWORD_ORIGIN] = {"[Origin]"};
    Names[ConfigurableKeyword::KEYWORD_IMPORT] = {"[Import]"};
    Names[ConfigurableKeyword::KEYWORD_NAMESPACE] = {"[Namespace]"};
    Names[ConfigurableKeyword::KEYWORD_CONFIGURATION] = {"[Configuration]"};

    // Type Identifiers
    // Apply OPTION_COUNT limit
    Names[ConfigurableKeyword::TYPE_CUSTOM_STYLE] = {"@Style", "@style", "@CSS", "@Css", "@css"};
    if (Names[ConfigurableKeyword::TYPE_CUSTOM_STYLE].size() > OptionCountLimit) {
        Names[ConfigurableKeyword::TYPE_CUSTOM_STYLE].resize(OptionCountLimit);
    }
    Names[ConfigurableKeyword::TYPE_CUSTOM_ELEMENT] = {"@Element"};
    Names[ConfigurableKeyword::TYPE_CUSTOM_VAR] = {"@Var"};
    Names[ConfigurableKeyword::TYPE_TEMPLATE_STYLE] = {"@Style"};
    Names[ConfigurableKeyword::TYPE_TEMPLATE_ELEMENT] = {"@Element"};
    Names[ConfigurableKeyword::TYPE_TEMPLATE_VAR] = {"@Var"};
    Names[ConfigurableKeyword::TYPE_ORIGIN_HTML] = {"@Html"};
    Names[ConfigurableKeyword::TYPE_ORIGIN_STYLE] = {"@Style"};
    Names[ConfigurableKeyword::TYPE_ORIGIN_JAVASCRIPT] = {"@JavaScript"};
    Names[ConfigurableKeyword::TYPE_IMPORT_HTML] = {"@Html"};
    Names[ConfigurableKeyword::TYPE_IMPORT_STYLE] = {"@Style"};
    Names[ConfigurableKeyword::TYPE_IMPORT_JAVASCRIPT] = {"@JavaScript"};
    Names[ConfigurableKeyword::TYPE_IMPORT_CHTL] = {"@Chtl"};
    Names[ConfigurableKeyword::TYPE_IMPORT_CJMOD] = {"@CJmod"};
    Names[ConfigurableKeyword::TYPE_IMPORT_CONFIG] = {"@Config"};
    Names[ConfigurableKeyword::TYPE_CONFIGURATION_CONFIG] = {"@Config"};
}

}
