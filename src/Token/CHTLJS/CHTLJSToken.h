#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {
namespace Token {

// CHTL JS Token类型定义，严格按照CHTL语法文档中的CHTL JS部分
enum class CHTLJSTokenType {
    // 基础Token (继承自JavaScript)
    IDENTIFIER,          // 标识符
    STRING_LITERAL,      // 字符串字面量
    NUMBER_LITERAL,      // 数字字面量
    UNQUOTED_LITERAL,    // 无修饰字面量
    
    // 符号
    LEFT_BRACE,          // {
    RIGHT_BRACE,         // }
    LEFT_BRACKET,        // [
    RIGHT_BRACKET,       // ]
    LEFT_PAREN,          // (
    RIGHT_PAREN,         // )
    SEMICOLON,           // ;
    COLON,               // :
    EQUALS,              // =
    COMMA,               // ,
    DOT,                 // .
    
    // CHTL JS特有符号
    ARROW,               // -> (明确使用CHTL语法)
    AMPERSAND,           // & (引用选择器)
    DOUBLE_LEFT_BRACE,   // {{
    DOUBLE_RIGHT_BRACE,  // }}
    
    // CHTL JS关键字
    VIR,                 // vir (虚对象)
    LISTEN,              // listen (增强监听器)
    DELEGATE,            // delegate (事件委托)
    ANIMATE,             // animate (动画)
    
    // 注意：iNeverAway和printMylove是珂朵莉模块的CJMOD功能，不是基础语法
    // 它们应该通过模块导入系统实现，而不是内置token
    
    // 扩展的CHTL JS特性
    EVENT_BIND_OPERATOR, // &-> (事件绑定操作符)
    
    // 动画关键字
    AT_KEYWORD,          // at (动画时间点)
    BEGIN_KEYWORD,       // begin (动画起始状态)
    WHEN_KEYWORD,        // when (动画关键帧)
    END_KEYWORD,         // end (动画结束状态)
    LOOP_KEYWORD,        // loop (动画循环)
    DIRECTION_KEYWORD,   // direction (动画方向)
    DELAY_KEYWORD,       // delay (动画延迟)
    CALLBACK_KEYWORD,    // callback (动画回调)
    TARGET_KEYWORD,      // target (动画目标)
    DURATION_KEYWORD,    // duration (动画持续时间)
    EASING_KEYWORD,      // easing (缓动函数)
    
    // 选择器相关
    CSS_SELECTOR,        // CSS选择器内容 (在{{}}内部)
    CLASS_SELECTOR,      // 类选择器 (.xxx)
    ID_SELECTOR,         // ID选择器 (#xxx)
    TAG_SELECTOR,        // 标签选择器
    INDEXED_SELECTOR,    // 索引选择器 ([0], [1]等)
    
    // 事件类型 (用于listen和delegate)
    CLICK_EVENT,         // click
    MOUSEENTER_EVENT,    // mouseenter
    MOUSELEAVE_EVENT,    // mouseleave
    MOUSEMOVE_EVENT,     // mousemove
    KEYDOWN_EVENT,       // keydown
    KEYUP_EVENT,         // keyup
    FOCUS_EVENT,         // focus
    BLUR_EVENT,          // blur
    CHANGE_EVENT,        // change
    SUBMIT_EVENT,        // submit
    LOAD_EVENT,          // load
    RESIZE_EVENT,        // resize
    SCROLL_EVENT,        // scroll
    
    // 动画相关
    EASING_FUNCTION,     // 缓动函数
    ANIMATION_PROPERTY,  // 动画属性
    
    // 注释
    SINGLE_COMMENT,      // //
    MULTI_COMMENT,       // /* */
    GENERATOR_COMMENT,   // --
    
    // 结束符
    EOF_TOKEN,           // 文件结束
    NEWLINE,             // 换行
    WHITESPACE,          // 空白字符
    
    // 错误类型
    UNKNOWN,             // 未知Token
    ERROR                // 错误Token
};

// CHTL JS Token结构体
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLJSToken() : type(CHTLJSTokenType::UNKNOWN), line(0), column(0), position(0) {}
    
    CHTLJSToken(CHTLJSTokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    // 便于调试的字符串表示
    std::string toString() const;
    
    // 判断是否是选择器相关
    bool isSelector() const;
    
    // 判断是否是事件类型
    bool isEventType() const;
    
    // 判断是否是CHTL JS特有语法
    bool isCHTLJSSpecific() const;
};

// CHTL JS Token工具类
class CHTLJSTokenUtils {
public:
    // 获取所有支持的事件类型
    static const std::vector<std::string>& getEventTypes();
    
    // 判断是否是事件类型
    static bool isEventType(const std::string& event);
    
    // 获取所有支持的缓动函数
    static const std::vector<std::string>& getEasingFunctions();
    
    // 判断是否是缓动函数
    static bool isEasingFunction(const std::string& easing);
    
    // 获取Token类型名称
    static std::string getTokenTypeName(CHTLJSTokenType type);
    
    // 判断字符是否是选择器字符
    static bool isSelectorChar(char c);
    
    // 判断是否是有效的CSS选择器
    static bool isValidCSSSelector(const std::string& selector);
    
    // 解析选择器类型
    static CHTLJSTokenType parseSelectorType(const std::string& selector);
    
private:
    static std::vector<std::string> eventTypes;
    static std::vector<std::string> easingFunctions;
    static std::unordered_map<CHTLJSTokenType, std::string> tokenTypeNames;
    static void initializeEventTypes();
    static void initializeEasingFunctions();
    static void initializeTokenTypeNames();
};

} // namespace Token
} // namespace CHTL