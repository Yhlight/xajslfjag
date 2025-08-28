#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTL {
    namespace 核心 {
        
        /**
         * HTML标签类型枚举
         * 严格按照语法文档支持的HTML标签
         */
        enum class HTML标签类型 {
            // 结构化标签
            HTML, HEAD, BODY, TITLE, META, LINK, STYLE, SCRIPT,
            
            // 块级元素（语法文档明确支持）
            DIV, P, H1, H2, H3, H4, H5, H6, SECTION, ARTICLE, ASIDE, NAV,
            HEADER, FOOTER, MAIN, BLOCKQUOTE, PRE, UL, OL, LI, DL, DT, DD,
            TABLE, THEAD, TBODY, TFOOT, TR, TD, TH, CAPTION, COLGROUP, COL,
            FORM, FIELDSET, LEGEND, TEXTAREA, SELECT, OPTION, OPTGROUP,
            
            // 行内元素（语法文档明确支持）
            SPAN, A, STRONG, EM, B, I, U, SMALL, SUB, SUP, CODE, KBD, SAMP,
            VAR, CITE, Q, ABBR, DFN, TIME, MARK, DEL, INS,
            
            // 表单元素（语法文档明确支持）
            INPUT, BUTTON, LABEL, DATALIST, OUTPUT, PROGRESS, METER,
            
            // 媒体元素（语法文档明确支持）
            IMG, AUDIO, VIDEO, SOURCE, TRACK, CANVAS, SVG,
            
            // 单标签元素（语法文档明确支持）
            BR, HR, AREA, BASE, EMBED, PARAM, WBR,
            
            // 其他（语法文档明确支持）
            IFRAME, OBJECT, NOSCRIPT, TEMPLATE,
            
            未知标签
        };
        
        /**
         * CSS属性类型枚举
         * 严格按照语法文档在局部样式块中支持的CSS属性
         */
        enum class CSS属性类型 {
            // 布局属性（语法文档局部样式块支持）
            DISPLAY, POSITION, TOP, RIGHT, BOTTOM, LEFT, FLOAT, CLEAR,
            WIDTH, HEIGHT, MIN_WIDTH, MIN_HEIGHT, MAX_WIDTH, MAX_HEIGHT,
            MARGIN, MARGIN_TOP, MARGIN_RIGHT, MARGIN_BOTTOM, MARGIN_LEFT,
            PADDING, PADDING_TOP, PADDING_RIGHT, PADDING_BOTTOM, PADDING_LEFT,
            
            // 视觉属性（语法文档局部样式块支持）
            BACKGROUND, BACKGROUND_COLOR, BACKGROUND_IMAGE, BACKGROUND_REPEAT,
            BACKGROUND_POSITION, BACKGROUND_SIZE, BACKGROUND_ATTACHMENT,
            BORDER, BORDER_WIDTH, BORDER_STYLE, BORDER_COLOR,
            BORDER_TOP, BORDER_RIGHT, BORDER_BOTTOM, BORDER_LEFT,
            BORDER_RADIUS, BOX_SHADOW, OPACITY, VISIBILITY,
            
            // 文本属性（语法文档局部样式块支持）
            FONT, FONT_FAMILY, FONT_SIZE, FONT_WEIGHT, FONT_STYLE,
            COLOR, TEXT_ALIGN, TEXT_DECORATION, TEXT_TRANSFORM,
            LINE_HEIGHT, LETTER_SPACING, WORD_SPACING, TEXT_INDENT,
            
            // 变换属性（语法文档局部样式块支持）
            TRANSFORM, TRANSITION, ANIMATION,
            
            // Flexbox属性（语法文档局部样式块支持）
            FLEX, FLEX_DIRECTION, FLEX_WRAP, JUSTIFY_CONTENT, ALIGN_ITEMS,
            ALIGN_CONTENT, FLEX_GROW, FLEX_SHRINK, FLEX_BASIS,
            
            // Grid属性（语法文档局部样式块支持）
            GRID, GRID_TEMPLATE_COLUMNS, GRID_TEMPLATE_ROWS, GRID_GAP,
            
            // 其他（语法文档局部样式块支持）
            Z_INDEX, OVERFLOW, CURSOR, LIST_STYLE,
            
            未知属性
        };
        
        /**
         * CHTL全局符号映射表
         * 管理HTML标签、CSS属性、关键字等全局符号的映射关系
         * 严格按照CHTL语法文档的规范，不私自扩展
         */
        class CHTL全局映射表 {
        private:
            // HTML标签映射表
            static std::unordered_map<std::string, HTML标签类型> HTML标签映射表_;
            static std::unordered_map<HTML标签类型, std::string> HTML标签名称映射表_;
            
            // CSS属性映射表
            static std::unordered_map<std::string, CSS属性类型> CSS属性映射表_;
            static std::unordered_map<CSS属性类型, std::string> CSS属性名称映射表_;
            
            // HTML单标签集合（语法文档定义的自闭合标签）
            static std::unordered_set<HTML标签类型> 单标签集合_;
            
            // 块级元素集合（语法文档定义的块级元素）
            static std::unordered_set<HTML标签类型> 块级元素集合_;
            
            // 行内元素集合（语法文档定义的行内元素）
            static std::unordered_set<HTML标签类型> 行内元素集合_;
            
            // 初始化标志
            static bool 已初始化_;
            
            // 初始化方法
            static void 初始化HTML标签();
            static void 初始化CSS属性();
            static void 初始化元素类型();
            
        public:
            // 确保映射表已初始化
            static void 确保已初始化();
            
            // HTML标签相关方法
            static HTML标签类型 获取HTML标签类型(const std::string& 标签名);
            static std::string 获取HTML标签名称(HTML标签类型 类型);
            static bool 是有效HTML标签(const std::string& 标签名);
            static bool 是单标签元素(HTML标签类型 类型);
            static bool 是块级元素(HTML标签类型 类型);
            static bool 是行内元素(HTML标签类型 类型);
            
            // CSS属性相关方法
            static CSS属性类型 获取CSS属性类型(const std::string& 属性名);
            static std::string 获取CSS属性名称(CSS属性类型 类型);
            static bool 是有效CSS属性(const std::string& 属性名);
            
            // 配置组关键字验证（严格按照语法文档的配置组规范）
            static bool 是有效配置关键字(const std::string& 关键字);
            static bool 是有效名称配置关键字(const std::string& 关键字);
            static bool 是有效原始类型配置关键字(const std::string& 关键字);
            
            // 获取所有有效标签和属性列表
            static std::vector<std::string> 获取所有HTML标签();
            static std::vector<std::string> 获取所有CSS属性();
            
            // 工具方法
            static std::string 转小写(const std::string& 字符串);
            static std::string 转驼峰格式(const std::string& 字符串);
            static std::string 转短横线格式(const std::string& 字符串);
        };
        
    } // namespace 核心
} // namespace CHTL