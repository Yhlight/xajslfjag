#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTL {
    namespace Core {
        
        /**
         * HTML标签类型枚举
         */
        enum class HTMLTagType {
            // 结构化标签
            HTML, HEAD, BODY, TITLE, META, LINK, STYLE, SCRIPT,
            
            // 块级元素
            DIV, P, H1, H2, H3, H4, H5, H6, SECTION, ARTICLE, ASIDE, NAV,
            HEADER, FOOTER, MAIN, BLOCKQUOTE, PRE, UL, OL, LI, DL, DT, DD,
            TABLE, THEAD, TBODY, TFOOT, TR, TD, TH, CAPTION, COLGROUP, COL,
            FORM, FIELDSET, LEGEND, TEXTAREA, SELECT, OPTION, OPTGROUP,
            
            // 行内元素
            SPAN, A, STRONG, EM, B, I, U, SMALL, SUB, SUP, CODE, KBD, SAMP,
            VAR, CITE, Q, ABBR, DFN, TIME, MARK, DEL, INS,
            
            // 表单元素
            INPUT, BUTTON, LABEL, DATALIST, OUTPUT, PROGRESS, METER,
            
            // 媒体元素
            IMG, AUDIO, VIDEO, SOURCE, TRACK, CANVAS, SVG,
            
            // 单标签元素
            BR, HR, AREA, BASE, EMBED, PARAM, WBR,
            
            // 其他
            IFRAME, OBJECT, NOSCRIPT, TEMPLATE,
            
            UNKNOWN_TAG
        };
        
        /**
         * CSS属性类型枚举
         */
        enum class CSSPropertyType {
            // 布局属性
            DISPLAY, POSITION, TOP, RIGHT, BOTTOM, LEFT, FLOAT, CLEAR,
            WIDTH, HEIGHT, MIN_WIDTH, MIN_HEIGHT, MAX_WIDTH, MAX_HEIGHT,
            MARGIN, MARGIN_TOP, MARGIN_RIGHT, MARGIN_BOTTOM, MARGIN_LEFT,
            PADDING, PADDING_TOP, PADDING_RIGHT, PADDING_BOTTOM, PADDING_LEFT,
            
            // 视觉属性
            BACKGROUND, BACKGROUND_COLOR, BACKGROUND_IMAGE, BACKGROUND_REPEAT,
            BACKGROUND_POSITION, BACKGROUND_SIZE, BACKGROUND_ATTACHMENT,
            BORDER, BORDER_WIDTH, BORDER_STYLE, BORDER_COLOR,
            BORDER_TOP, BORDER_RIGHT, BORDER_BOTTOM, BORDER_LEFT,
            BORDER_RADIUS, BOX_SHADOW, OPACITY, VISIBILITY,
            
            // 文本属性
            FONT, FONT_FAMILY, FONT_SIZE, FONT_WEIGHT, FONT_STYLE,
            COLOR, TEXT_ALIGN, TEXT_DECORATION, TEXT_TRANSFORM,
            LINE_HEIGHT, LETTER_SPACING, WORD_SPACING, TEXT_INDENT,
            
            // 变换属性
            TRANSFORM, TRANSITION, ANIMATION,
            
            // Flexbox属性
            FLEX, FLEX_DIRECTION, FLEX_WRAP, JUSTIFY_CONTENT, ALIGN_ITEMS,
            ALIGN_CONTENT, FLEX_GROW, FLEX_SHRINK, FLEX_BASIS,
            
            // Grid属性
            GRID, GRID_TEMPLATE_COLUMNS, GRID_TEMPLATE_ROWS, GRID_GAP,
            
            // 其他
            Z_INDEX, OVERFLOW, CURSOR, LIST_STYLE,
            
            UNKNOWN_PROPERTY
        };
        
        /**
         * CHTL全局符号映射表
         * 管理HTML标签、CSS属性、关键字等全局符号的映射关系
         */
        class CHTLGlobalMap {
        private:
            // HTML标签映射
            static std::unordered_map<std::string, HTMLTagType> htmlTagMap_;
            static std::unordered_map<HTMLTagType, std::string> htmlTagNameMap_;
            
            // CSS属性映射
            static std::unordered_map<std::string, CSSPropertyType> cssPropertyMap_;
            static std::unordered_map<CSSPropertyType, std::string> cssPropertyNameMap_;
            
            // HTML单标签集合
            static std::unordered_set<HTMLTagType> voidElementTags_;
            
            // 块级元素集合
            static std::unordered_set<HTMLTagType> blockElementTags_;
            
            // 行内元素集合
            static std::unordered_set<HTMLTagType> inlineElementTags_;
            
            // 初始化标志
            static bool initialized_;
            
            // 初始化方法
            static void InitializeHTMLTags();
            static void InitializeCSSProperties();
            static void InitializeElementTypes();
            
        public:
            // 确保映射表已初始化
            static void EnsureInitialized();
            
            // HTML标签相关方法
            static HTMLTagType GetHTMLTagType(const std::string& tagName);
            static std::string GetHTMLTagName(HTMLTagType type);
            static bool IsValidHTMLTag(const std::string& tagName);
            static bool IsVoidElement(HTMLTagType type);
            static bool IsBlockElement(HTMLTagType type);
            static bool IsInlineElement(HTMLTagType type);
            
            // CSS属性相关方法
            static CSSPropertyType GetCSSPropertyType(const std::string& propertyName);
            static std::string GetCSSPropertyName(CSSPropertyType type);
            static bool IsValidCSSProperty(const std::string& propertyName);
            
            // 配置组关键字验证
            static bool IsValidConfigurationKey(const std::string& key);
            static bool IsValidNameConfigurationKey(const std::string& key);
            
            // 获取所有有效标签和属性列表
            static std::vector<std::string> GetAllHTMLTags();
            static std::vector<std::string> GetAllCSSProperties();
            
            // 工具方法
            static std::string ToLowerCase(const std::string& str);
            static std::string ToCamelCase(const std::string& str);
            static std::string ToKebabCase(const std::string& str);
        };
        
    } // namespace Core
} // namespace CHTL