#pragma once

#include "Common/Token.h"
#include <unordered_map>

namespace CHTL {

// CHTL JS专用Token
class CHTLJSToken : public Token {
public:
    // CHTL JS特有的Token类型
    enum class CHTLJSType : int {
        // === 增强选择器相关 ===
        ENHANCED_SELECTOR_START = static_cast<int>(Token::Type::CHTLJS_TOKEN_START),  // {{
        ENHANCED_SELECTOR_END,      // }}
        SELECTOR_CLASS,             // .className (在{{}}内)
        SELECTOR_ID,                // #idName (在{{}}内)
        SELECTOR_TAG,               // tagName (在{{}}内)
        SELECTOR_INDEX,             // [index] (在{{}}内)
        SELECTOR_DESCENDANT,        // 空格 (在{{}}内的后代选择器)
        SELECTOR_AMPERSAND,         // & (引用选择器)
        
        // === 虚对象相关 ===
        KEYWORD_VIR,                // vir
        VIR_IDENTIFIER,             // 虚对象名称
        
        // === 事件系统相关 ===
        KEYWORD_LISTEN,             // listen
        KEYWORD_DELEGATE,           // delegate
        EVENT_NAME,                 // 事件名称 (click, mouseenter等)
        EVENT_TARGET,               // target: (在delegate中)
        
        // === 动画系统相关 ===
        KEYWORD_ANIMATE,            // animate
        ANIM_DURATION,              // duration:
        ANIM_EASING,                // easing:
        ANIM_BEGIN,                 // begin:
        ANIM_END,                   // end:
        ANIM_WHEN,                  // when:
        ANIM_AT,                    // at:
        ANIM_LOOP,                  // loop:
        ANIM_DIRECTION,             // direction:
        ANIM_DELAY,                 // delay:
        ANIM_CALLBACK,              // callback:
        
        // === CHTL JS方法调用 ===
        METHOD_ARROW,               // -> (CHTL JS方法调用)
        METHOD_NAME,                // 方法名
        
        // === 特殊函数 ===
        // 珂朵莉模块函数
        FUNCTION_PRINTMYLOVE,       // printMylove
        FUNCTION_INEVERAWAY,        // iNeverAway
        
        // 模式和参数
        PARAM_URL,                  // url:
        PARAM_MODE,                 // mode:
        PARAM_WIDTH,                // width:
        PARAM_HEIGHT,               // height:
        PARAM_SCALE,                // scale:
        
        // === 特殊类型 ===
        VOID_TYPE,                  // Void<状态>
        STATE_MARKER,               // <状态> (在Void<A>中的A)
        
        // === 键值对 ===
        KEY_VALUE_PAIR,             // 键值对 (在对象中)
        
        // === 函数和对象 ===
        ARROW_FUNCTION,             // () => {}
        OBJECT_LITERAL,             // {}
        FUNCTION_DECLARATION,       // function() {}
        
        // === 其他 ===
        CHTLJS_IDENTIFIER,          // CHTL JS标识符
        CHTLJS_STRING,              // CHTL JS字符串
        CHTLJS_NUMBER,              // CHTL JS数字
        CHTLJS_COMMENT              // CHTL JS注释
    };
    
    CHTLJSToken(CHTLJSType type, const std::string& lexeme, const Location& location)
        : Token(static_cast<Token::Type>(type), lexeme, location) {}
    
    CHTLJSType GetCHTLJSType() const { 
        return static_cast<CHTLJSType>(type_); 
    }
    
    // 检查是否为CHTL JS关键字
    bool IsCHTLJSKeyword() const {
        CHTLJSType jsType = GetCHTLJSType();
        return jsType == CHTLJSType::KEYWORD_VIR ||
               jsType == CHTLJSType::KEYWORD_LISTEN ||
               jsType == CHTLJSType::KEYWORD_DELEGATE ||
               jsType == CHTLJSType::KEYWORD_ANIMATE;
    }
    
    // 检查是否为增强选择器相关Token
    bool IsEnhancedSelectorToken() const {
        CHTLJSType jsType = GetCHTLJSType();
        return jsType >= CHTLJSType::ENHANCED_SELECTOR_START &&
               jsType <= CHTLJSType::SELECTOR_AMPERSAND;
    }
    
    // 检查是否为动画参数Token
    bool IsAnimationParam() const {
        CHTLJSType jsType = GetCHTLJSType();
        return jsType >= CHTLJSType::ANIM_DURATION &&
               jsType <= CHTLJSType::ANIM_CALLBACK;
    }
    
    // 检查是否为CJMOD函数
    bool IsCJMODFunction() const {
        CHTLJSType jsType = GetCHTLJSType();
        return jsType == CHTLJSType::FUNCTION_PRINTMYLOVE ||
               jsType == CHTLJSType::FUNCTION_INEVERAWAY;
    }
    
    // 获取Token类型名称
    std::string GetTypeName() const;
    
    // 静态方法：根据关键字字符串获取Token类型
    static CHTLJSType GetKeywordType(const std::string& keyword);
    static CHTLJSType GetFunctionType(const std::string& funcName);
    static CHTLJSType GetAnimParamType(const std::string& param);
    
    // 判断字符串是否为CHTL JS关键字
    static bool IsKeyword(const std::string& str);
    static bool IsCJMODFunction(const std::string& str);
    
private:
    // 关键字映射表
    static const std::unordered_map<std::string, CHTLJSType> keywordMap_;
    static const std::unordered_map<std::string, CHTLJSType> functionMap_;
    static const std::unordered_map<std::string, CHTLJSType> animParamMap_;
    
    // 初始化映射表
    static std::unordered_map<std::string, CHTLJSType> InitKeywordMap();
    static std::unordered_map<std::string, CHTLJSType> InitFunctionMap();
    static std::unordered_map<std::string, CHTLJSType> InitAnimParamMap();
};

using CHTLJSTokenPtr = std::shared_ptr<CHTLJSToken>;

} // namespace CHTL