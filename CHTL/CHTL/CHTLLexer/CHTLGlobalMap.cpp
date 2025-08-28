#include "CHTLGlobalMap.h"
#include <algorithm>
#include <cctype>

namespace CHTL {
    namespace Core {
        
        // 静态成员初始化
        std::unordered_map<std::string, HTMLTagType> CHTLGlobalMap::htmlTagMap_;
        std::unordered_map<HTMLTagType, std::string> CHTLGlobalMap::htmlTagNameMap_;
        std::unordered_map<std::string, CSSPropertyType> CHTLGlobalMap::cssPropertyMap_;
        std::unordered_map<CSSPropertyType, std::string> CHTLGlobalMap::cssPropertyNameMap_;
        std::unordered_set<HTMLTagType> CHTLGlobalMap::voidElementTags_;
        std::unordered_set<HTMLTagType> CHTLGlobalMap::blockElementTags_;
        std::unordered_set<HTMLTagType> CHTLGlobalMap::inlineElementTags_;
        bool CHTLGlobalMap::initialized_ = false;
        
        void CHTLGlobalMap::EnsureInitialized() {
            if (!initialized_) {
                InitializeHTMLTags();
                InitializeCSSProperties();
                InitializeElementTypes();
                initialized_ = true;
            }
        }
        
        void CHTLGlobalMap::InitializeHTMLTags() {
            // 结构化标签
            htmlTagMap_["html"] = HTMLTagType::HTML;
            htmlTagMap_["head"] = HTMLTagType::HEAD;
            htmlTagMap_["body"] = HTMLTagType::BODY;
            htmlTagMap_["title"] = HTMLTagType::TITLE;
            htmlTagMap_["meta"] = HTMLTagType::META;
            htmlTagMap_["link"] = HTMLTagType::LINK;
            htmlTagMap_["style"] = HTMLTagType::STYLE;
            htmlTagMap_["script"] = HTMLTagType::SCRIPT;
            
            // 块级元素
            htmlTagMap_["div"] = HTMLTagType::DIV;
            htmlTagMap_["p"] = HTMLTagType::P;
            htmlTagMap_["h1"] = HTMLTagType::H1;
            htmlTagMap_["h2"] = HTMLTagType::H2;
            htmlTagMap_["h3"] = HTMLTagType::H3;
            htmlTagMap_["h4"] = HTMLTagType::H4;
            htmlTagMap_["h5"] = HTMLTagType::H5;
            htmlTagMap_["h6"] = HTMLTagType::H6;
            htmlTagMap_["section"] = HTMLTagType::SECTION;
            htmlTagMap_["article"] = HTMLTagType::ARTICLE;
            htmlTagMap_["aside"] = HTMLTagType::ASIDE;
            htmlTagMap_["nav"] = HTMLTagType::NAV;
            htmlTagMap_["header"] = HTMLTagType::HEADER;
            htmlTagMap_["footer"] = HTMLTagType::FOOTER;
            htmlTagMap_["main"] = HTMLTagType::MAIN;
            htmlTagMap_["blockquote"] = HTMLTagType::BLOCKQUOTE;
            htmlTagMap_["pre"] = HTMLTagType::PRE;
            htmlTagMap_["ul"] = HTMLTagType::UL;
            htmlTagMap_["ol"] = HTMLTagType::OL;
            htmlTagMap_["li"] = HTMLTagType::LI;
            htmlTagMap_["dl"] = HTMLTagType::DL;
            htmlTagMap_["dt"] = HTMLTagType::DT;
            htmlTagMap_["dd"] = HTMLTagType::DD;
            htmlTagMap_["table"] = HTMLTagType::TABLE;
            htmlTagMap_["thead"] = HTMLTagType::THEAD;
            htmlTagMap_["tbody"] = HTMLTagType::TBODY;
            htmlTagMap_["tfoot"] = HTMLTagType::TFOOT;
            htmlTagMap_["tr"] = HTMLTagType::TR;
            htmlTagMap_["td"] = HTMLTagType::TD;
            htmlTagMap_["th"] = HTMLTagType::TH;
            htmlTagMap_["caption"] = HTMLTagType::CAPTION;
            htmlTagMap_["colgroup"] = HTMLTagType::COLGROUP;
            htmlTagMap_["col"] = HTMLTagType::COL;
            htmlTagMap_["form"] = HTMLTagType::FORM;
            htmlTagMap_["fieldset"] = HTMLTagType::FIELDSET;
            htmlTagMap_["legend"] = HTMLTagType::LEGEND;
            htmlTagMap_["textarea"] = HTMLTagType::TEXTAREA;
            htmlTagMap_["select"] = HTMLTagType::SELECT;
            htmlTagMap_["option"] = HTMLTagType::OPTION;
            htmlTagMap_["optgroup"] = HTMLTagType::OPTGROUP;
            
            // 行内元素
            htmlTagMap_["span"] = HTMLTagType::SPAN;
            htmlTagMap_["a"] = HTMLTagType::A;
            htmlTagMap_["strong"] = HTMLTagType::STRONG;
            htmlTagMap_["em"] = HTMLTagType::EM;
            htmlTagMap_["b"] = HTMLTagType::B;
            htmlTagMap_["i"] = HTMLTagType::I;
            htmlTagMap_["u"] = HTMLTagType::U;
            htmlTagMap_["small"] = HTMLTagType::SMALL;
            htmlTagMap_["sub"] = HTMLTagType::SUB;
            htmlTagMap_["sup"] = HTMLTagType::SUP;
            htmlTagMap_["code"] = HTMLTagType::CODE;
            htmlTagMap_["kbd"] = HTMLTagType::KBD;
            htmlTagMap_["samp"] = HTMLTagType::SAMP;
            htmlTagMap_["var"] = HTMLTagType::VAR;
            htmlTagMap_["cite"] = HTMLTagType::CITE;
            htmlTagMap_["q"] = HTMLTagType::Q;
            htmlTagMap_["abbr"] = HTMLTagType::ABBR;
            htmlTagMap_["dfn"] = HTMLTagType::DFN;
            htmlTagMap_["time"] = HTMLTagType::TIME;
            htmlTagMap_["mark"] = HTMLTagType::MARK;
            htmlTagMap_["del"] = HTMLTagType::DEL;
            htmlTagMap_["ins"] = HTMLTagType::INS;
            
            // 表单元素
            htmlTagMap_["input"] = HTMLTagType::INPUT;
            htmlTagMap_["button"] = HTMLTagType::BUTTON;
            htmlTagMap_["label"] = HTMLTagType::LABEL;
            htmlTagMap_["datalist"] = HTMLTagType::DATALIST;
            htmlTagMap_["output"] = HTMLTagType::OUTPUT;
            htmlTagMap_["progress"] = HTMLTagType::PROGRESS;
            htmlTagMap_["meter"] = HTMLTagType::METER;
            
            // 媒体元素
            htmlTagMap_["img"] = HTMLTagType::IMG;
            htmlTagMap_["audio"] = HTMLTagType::AUDIO;
            htmlTagMap_["video"] = HTMLTagType::VIDEO;
            htmlTagMap_["source"] = HTMLTagType::SOURCE;
            htmlTagMap_["track"] = HTMLTagType::TRACK;
            htmlTagMap_["canvas"] = HTMLTagType::CANVAS;
            htmlTagMap_["svg"] = HTMLTagType::SVG;
            
            // 单标签元素
            htmlTagMap_["br"] = HTMLTagType::BR;
            htmlTagMap_["hr"] = HTMLTagType::HR;
            htmlTagMap_["area"] = HTMLTagType::AREA;
            htmlTagMap_["base"] = HTMLTagType::BASE;
            htmlTagMap_["embed"] = HTMLTagType::EMBED;
            htmlTagMap_["param"] = HTMLTagType::PARAM;
            htmlTagMap_["wbr"] = HTMLTagType::WBR;
            
            // 其他
            htmlTagMap_["iframe"] = HTMLTagType::IFRAME;
            htmlTagMap_["object"] = HTMLTagType::OBJECT;
            htmlTagMap_["noscript"] = HTMLTagType::NOSCRIPT;
            htmlTagMap_["template"] = HTMLTagType::TEMPLATE;
            
            // 创建反向映射
            for (const auto& pair : htmlTagMap_) {
                htmlTagNameMap_[pair.second] = pair.first;
            }
        }
        
        void CHTLGlobalMap::InitializeCSSProperties() {
            // 布局属性
            cssPropertyMap_["display"] = CSSPropertyType::DISPLAY;
            cssPropertyMap_["position"] = CSSPropertyType::POSITION;
            cssPropertyMap_["top"] = CSSPropertyType::TOP;
            cssPropertyMap_["right"] = CSSPropertyType::RIGHT;
            cssPropertyMap_["bottom"] = CSSPropertyType::BOTTOM;
            cssPropertyMap_["left"] = CSSPropertyType::LEFT;
            cssPropertyMap_["float"] = CSSPropertyType::FLOAT;
            cssPropertyMap_["clear"] = CSSPropertyType::CLEAR;
            cssPropertyMap_["width"] = CSSPropertyType::WIDTH;
            cssPropertyMap_["height"] = CSSPropertyType::HEIGHT;
            cssPropertyMap_["min-width"] = CSSPropertyType::MIN_WIDTH;
            cssPropertyMap_["min-height"] = CSSPropertyType::MIN_HEIGHT;
            cssPropertyMap_["max-width"] = CSSPropertyType::MAX_WIDTH;
            cssPropertyMap_["max-height"] = CSSPropertyType::MAX_HEIGHT;
            cssPropertyMap_["margin"] = CSSPropertyType::MARGIN;
            cssPropertyMap_["margin-top"] = CSSPropertyType::MARGIN_TOP;
            cssPropertyMap_["margin-right"] = CSSPropertyType::MARGIN_RIGHT;
            cssPropertyMap_["margin-bottom"] = CSSPropertyType::MARGIN_BOTTOM;
            cssPropertyMap_["margin-left"] = CSSPropertyType::MARGIN_LEFT;
            cssPropertyMap_["padding"] = CSSPropertyType::PADDING;
            cssPropertyMap_["padding-top"] = CSSPropertyType::PADDING_TOP;
            cssPropertyMap_["padding-right"] = CSSPropertyType::PADDING_RIGHT;
            cssPropertyMap_["padding-bottom"] = CSSPropertyType::PADDING_BOTTOM;
            cssPropertyMap_["padding-left"] = CSSPropertyType::PADDING_LEFT;
            
            // 视觉属性
            cssPropertyMap_["background"] = CSSPropertyType::BACKGROUND;
            cssPropertyMap_["background-color"] = CSSPropertyType::BACKGROUND_COLOR;
            cssPropertyMap_["background-image"] = CSSPropertyType::BACKGROUND_IMAGE;
            cssPropertyMap_["background-repeat"] = CSSPropertyType::BACKGROUND_REPEAT;
            cssPropertyMap_["background-position"] = CSSPropertyType::BACKGROUND_POSITION;
            cssPropertyMap_["background-size"] = CSSPropertyType::BACKGROUND_SIZE;
            cssPropertyMap_["background-attachment"] = CSSPropertyType::BACKGROUND_ATTACHMENT;
            cssPropertyMap_["border"] = CSSPropertyType::BORDER;
            cssPropertyMap_["border-width"] = CSSPropertyType::BORDER_WIDTH;
            cssPropertyMap_["border-style"] = CSSPropertyType::BORDER_STYLE;
            cssPropertyMap_["border-color"] = CSSPropertyType::BORDER_COLOR;
            cssPropertyMap_["border-top"] = CSSPropertyType::BORDER_TOP;
            cssPropertyMap_["border-right"] = CSSPropertyType::BORDER_RIGHT;
            cssPropertyMap_["border-bottom"] = CSSPropertyType::BORDER_BOTTOM;
            cssPropertyMap_["border-left"] = CSSPropertyType::BORDER_LEFT;
            cssPropertyMap_["border-radius"] = CSSPropertyType::BORDER_RADIUS;
            cssPropertyMap_["box-shadow"] = CSSPropertyType::BOX_SHADOW;
            cssPropertyMap_["opacity"] = CSSPropertyType::OPACITY;
            cssPropertyMap_["visibility"] = CSSPropertyType::VISIBILITY;
            
            // 文本属性
            cssPropertyMap_["font"] = CSSPropertyType::FONT;
            cssPropertyMap_["font-family"] = CSSPropertyType::FONT_FAMILY;
            cssPropertyMap_["font-size"] = CSSPropertyType::FONT_SIZE;
            cssPropertyMap_["font-weight"] = CSSPropertyType::FONT_WEIGHT;
            cssPropertyMap_["font-style"] = CSSPropertyType::FONT_STYLE;
            cssPropertyMap_["color"] = CSSPropertyType::COLOR;
            cssPropertyMap_["text-align"] = CSSPropertyType::TEXT_ALIGN;
            cssPropertyMap_["text-decoration"] = CSSPropertyType::TEXT_DECORATION;
            cssPropertyMap_["text-transform"] = CSSPropertyType::TEXT_TRANSFORM;
            cssPropertyMap_["line-height"] = CSSPropertyType::LINE_HEIGHT;
            cssPropertyMap_["letter-spacing"] = CSSPropertyType::LETTER_SPACING;
            cssPropertyMap_["word-spacing"] = CSSPropertyType::WORD_SPACING;
            cssPropertyMap_["text-indent"] = CSSPropertyType::TEXT_INDENT;
            
            // 变换属性
            cssPropertyMap_["transform"] = CSSPropertyType::TRANSFORM;
            cssPropertyMap_["transition"] = CSSPropertyType::TRANSITION;
            cssPropertyMap_["animation"] = CSSPropertyType::ANIMATION;
            
            // Flexbox属性
            cssPropertyMap_["flex"] = CSSPropertyType::FLEX;
            cssPropertyMap_["flex-direction"] = CSSPropertyType::FLEX_DIRECTION;
            cssPropertyMap_["flex-wrap"] = CSSPropertyType::FLEX_WRAP;
            cssPropertyMap_["justify-content"] = CSSPropertyType::JUSTIFY_CONTENT;
            cssPropertyMap_["align-items"] = CSSPropertyType::ALIGN_ITEMS;
            cssPropertyMap_["align-content"] = CSSPropertyType::ALIGN_CONTENT;
            cssPropertyMap_["flex-grow"] = CSSPropertyType::FLEX_GROW;
            cssPropertyMap_["flex-shrink"] = CSSPropertyType::FLEX_SHRINK;
            cssPropertyMap_["flex-basis"] = CSSPropertyType::FLEX_BASIS;
            
            // Grid属性
            cssPropertyMap_["grid"] = CSSPropertyType::GRID;
            cssPropertyMap_["grid-template-columns"] = CSSPropertyType::GRID_TEMPLATE_COLUMNS;
            cssPropertyMap_["grid-template-rows"] = CSSPropertyType::GRID_TEMPLATE_ROWS;
            cssPropertyMap_["grid-gap"] = CSSPropertyType::GRID_GAP;
            
            // 其他
            cssPropertyMap_["z-index"] = CSSPropertyType::Z_INDEX;
            cssPropertyMap_["overflow"] = CSSPropertyType::OVERFLOW;
            cssPropertyMap_["cursor"] = CSSPropertyType::CURSOR;
            cssPropertyMap_["list-style"] = CSSPropertyType::LIST_STYLE;
            
            // 创建反向映射
            for (const auto& pair : cssPropertyMap_) {
                cssPropertyNameMap_[pair.second] = pair.first;
            }
        }
        
        void CHTLGlobalMap::InitializeElementTypes() {
            // 单标签元素
            voidElementTags_.insert({
                HTMLTagType::BR, HTMLTagType::HR, HTMLTagType::AREA, HTMLTagType::BASE,
                HTMLTagType::EMBED, HTMLTagType::IMG, HTMLTagType::INPUT, HTMLTagType::LINK,
                HTMLTagType::META, HTMLTagType::PARAM, HTMLTagType::SOURCE, HTMLTagType::TRACK,
                HTMLTagType::WBR, HTMLTagType::COL
            });
            
            // 块级元素
            blockElementTags_.insert({
                HTMLTagType::DIV, HTMLTagType::P, HTMLTagType::H1, HTMLTagType::H2,
                HTMLTagType::H3, HTMLTagType::H4, HTMLTagType::H5, HTMLTagType::H6,
                HTMLTagType::SECTION, HTMLTagType::ARTICLE, HTMLTagType::ASIDE, HTMLTagType::NAV,
                HTMLTagType::HEADER, HTMLTagType::FOOTER, HTMLTagType::MAIN, HTMLTagType::BLOCKQUOTE,
                HTMLTagType::PRE, HTMLTagType::UL, HTMLTagType::OL, HTMLTagType::LI,
                HTMLTagType::DL, HTMLTagType::DT, HTMLTagType::DD, HTMLTagType::TABLE,
                HTMLTagType::THEAD, HTMLTagType::TBODY, HTMLTagType::TFOOT, HTMLTagType::TR,
                HTMLTagType::FORM, HTMLTagType::FIELDSET, HTMLTagType::TEXTAREA, HTMLTagType::SELECT
            });
            
            // 行内元素
            inlineElementTags_.insert({
                HTMLTagType::SPAN, HTMLTagType::A, HTMLTagType::STRONG, HTMLTagType::EM,
                HTMLTagType::B, HTMLTagType::I, HTMLTagType::U, HTMLTagType::SMALL,
                HTMLTagType::SUB, HTMLTagType::SUP, HTMLTagType::CODE, HTMLTagType::KBD,
                HTMLTagType::SAMP, HTMLTagType::VAR, HTMLTagType::CITE, HTMLTagType::Q,
                HTMLTagType::ABBR, HTMLTagType::DFN, HTMLTagType::TIME, HTMLTagType::MARK,
                HTMLTagType::DEL, HTMLTagType::INS, HTMLTagType::BUTTON, HTMLTagType::LABEL,
                HTMLTagType::OUTPUT, HTMLTagType::PROGRESS, HTMLTagType::METER
            });
        }
        
        HTMLTagType CHTLGlobalMap::GetHTMLTagType(const std::string& tagName) {
            EnsureInitialized();
            std::string lowerTag = ToLowerCase(tagName);
            auto it = htmlTagMap_.find(lowerTag);
            return it != htmlTagMap_.end() ? it->second : HTMLTagType::UNKNOWN_TAG;
        }
        
        std::string CHTLGlobalMap::GetHTMLTagName(HTMLTagType type) {
            EnsureInitialized();
            auto it = htmlTagNameMap_.find(type);
            return it != htmlTagNameMap_.end() ? it->second : "unknown";
        }
        
        bool CHTLGlobalMap::IsValidHTMLTag(const std::string& tagName) {
            return GetHTMLTagType(tagName) != HTMLTagType::UNKNOWN_TAG;
        }
        
        bool CHTLGlobalMap::IsVoidElement(HTMLTagType type) {
            EnsureInitialized();
            return voidElementTags_.find(type) != voidElementTags_.end();
        }
        
        bool CHTLGlobalMap::IsBlockElement(HTMLTagType type) {
            EnsureInitialized();
            return blockElementTags_.find(type) != blockElementTags_.end();
        }
        
        bool CHTLGlobalMap::IsInlineElement(HTMLTagType type) {
            EnsureInitialized();
            return inlineElementTags_.find(type) != inlineElementTags_.end();
        }
        
        CSSPropertyType CHTLGlobalMap::GetCSSPropertyType(const std::string& propertyName) {
            EnsureInitialized();
            std::string lowerProperty = ToLowerCase(propertyName);
            auto it = cssPropertyMap_.find(lowerProperty);
            return it != cssPropertyMap_.end() ? it->second : CSSPropertyType::UNKNOWN_PROPERTY;
        }
        
        std::string CHTLGlobalMap::GetCSSPropertyName(CSSPropertyType type) {
            EnsureInitialized();
            auto it = cssPropertyNameMap_.find(type);
            return it != cssPropertyNameMap_.end() ? it->second : "unknown";
        }
        
        bool CHTLGlobalMap::IsValidCSSProperty(const std::string& propertyName) {
            return GetCSSPropertyType(propertyName) != CSSPropertyType::UNKNOWN_PROPERTY;
        }
        
        bool CHTLGlobalMap::IsValidConfigurationKey(const std::string& key) {
            static const std::unordered_set<std::string> configKeys = {
                "INDEX_INITIAL_COUNT", "DISABLE_NAME_GROUP", "DISABLE_CUSTOM_ORIGIN_TYPE",
                "DEBUG_MODE", "DISABLE_STYLE_AUTO_ADD_CLASS", "DISABLE_STYLE_AUTO_ADD_ID",
                "DISABLE_SCRIPT_AUTO_ADD_CLASS", "DISABLE_SCRIPT_AUTO_ADD_ID",
                "DISABLE_DEFAULT_NAMESPACE"
            };
            
            return configKeys.find(key) != configKeys.end();
        }
        
        bool CHTLGlobalMap::IsValidNameConfigurationKey(const std::string& key) {
            static const std::unordered_set<std::string> nameConfigKeys = {
                "CUSTOM_STYLE", "CUSTOM_ELEMENT", "CUSTOM_VAR",
                "TEMPLATE_STYLE", "TEMPLATE_ELEMENT", "TEMPLATE_VAR",
                "ORIGIN_HTML", "ORIGIN_STYLE", "ORIGIN_JAVASCRIPT",
                "IMPORT_HTML", "IMPORT_STYLE", "IMPORT_JAVASCRIPT",
                "IMPORT_CHTL", "IMPORT_CRMOD", "IMPORT_CONFIG",
                "CONFIGURATION_CONFIG", "KEYWORD_INHERIT", "KEYWORD_DELETE",
                "KEYWORD_INSERT", "KEYWORD_AFTER", "KEYWORD_BEFORE",
                "KEYWORD_REPLACE", "KEYWORD_ATTOP", "KEYWORD_ATBOTTOM",
                "KEYWORD_FROM", "KEYWORD_AS", "KEYWORD_EXCEPT",
                "KEYWORD_TEXT", "KEYWORD_STYLE", "KEYWORD_SCRIPT",
                "KEYWORD_CUSTOM", "KEYWORD_TEMPLATE", "KEYWORD_ORIGIN",
                "KEYWORD_IMPORT", "KEYWORD_NAMESPACE", "OPTION_COUNT"
            };
            
            return nameConfigKeys.find(key) != nameConfigKeys.end();
        }
        
        std::vector<std::string> CHTLGlobalMap::GetAllHTMLTags() {
            EnsureInitialized();
            std::vector<std::string> tags;
            for (const auto& pair : htmlTagMap_) {
                tags.push_back(pair.first);
            }
            return tags;
        }
        
        std::vector<std::string> CHTLGlobalMap::GetAllCSSProperties() {
            EnsureInitialized();
            std::vector<std::string> properties;
            for (const auto& pair : cssPropertyMap_) {
                properties.push_back(pair.first);
            }
            return properties;
        }
        
        std::string CHTLGlobalMap::ToLowerCase(const std::string& str) {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }
        
        std::string CHTLGlobalMap::ToCamelCase(const std::string& str) {
            std::string result;
            bool nextUpper = false;
            
            for (char c : str) {
                if (c == '-' || c == '_') {
                    nextUpper = true;
                } else if (nextUpper) {
                    result += std::toupper(c);
                    nextUpper = false;
                } else {
                    result += std::tolower(c);
                }
            }
            
            return result;
        }
        
        std::string CHTLGlobalMap::ToKebabCase(const std::string& str) {
            std::string result;
            
            for (size_t i = 0; i < str.length(); ++i) {
                char c = str[i];
                if (std::isupper(c)) {
                    if (i > 0) {
                        result += '-';
                    }
                    result += std::tolower(c);
                } else {
                    result += c;
                }
            }
            
            return result;
        }
        
    } // namespace Core
} // namespace CHTL