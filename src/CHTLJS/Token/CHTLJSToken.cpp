#include "CHTLJSToken.h"

namespace CHTL {

// 初始化静态成员
const std::unordered_map<std::string, CHTLJSToken::CHTLJSType> CHTLJSToken::keywordMap_ = 
    CHTLJSToken::InitKeywordMap();
const std::unordered_map<std::string, CHTLJSToken::CHTLJSType> CHTLJSToken::functionMap_ = 
    CHTLJSToken::InitFunctionMap();
const std::unordered_map<std::string, CHTLJSToken::CHTLJSType> CHTLJSToken::animParamMap_ = 
    CHTLJSToken::InitAnimParamMap();

std::unordered_map<std::string, CHTLJSToken::CHTLJSType> CHTLJSToken::InitKeywordMap() {
    return {
        {"vir", CHTLJSType::KEYWORD_VIR},
        {"listen", CHTLJSType::KEYWORD_LISTEN},
        {"delegate", CHTLJSType::KEYWORD_DELEGATE},
        {"animate", CHTLJSType::KEYWORD_ANIMATE}
    };
}

std::unordered_map<std::string, CHTLJSToken::CHTLJSType> CHTLJSToken::InitFunctionMap() {
    return {
        // 珂朵莉模块函数
        {"printMylove", CHTLJSType::FUNCTION_PRINTMYLOVE},
        {"iNeverAway", CHTLJSType::FUNCTION_INEVERAWAY}
    };
}

std::unordered_map<std::string, CHTLJSToken::CHTLJSType> CHTLJSToken::InitAnimParamMap() {
    return {
        {"duration", CHTLJSType::ANIM_DURATION},
        {"easing", CHTLJSType::ANIM_EASING},
        {"begin", CHTLJSType::ANIM_BEGIN},
        {"end", CHTLJSType::ANIM_END},
        {"when", CHTLJSType::ANIM_WHEN},
        {"at", CHTLJSType::ANIM_AT},
        {"loop", CHTLJSType::ANIM_LOOP},
        {"direction", CHTLJSType::ANIM_DIRECTION},
        {"delay", CHTLJSType::ANIM_DELAY},
        {"callback", CHTLJSType::ANIM_CALLBACK},
        
        // 通用参数
        {"target", CHTLJSType::EVENT_TARGET},
        {"url", CHTLJSType::PARAM_URL},
        {"mode", CHTLJSType::PARAM_MODE},
        {"width", CHTLJSType::PARAM_WIDTH},
        {"height", CHTLJSType::PARAM_HEIGHT},
        {"scale", CHTLJSType::PARAM_SCALE}
    };
}

std::string CHTLJSToken::GetTypeName() const {
    switch (GetCHTLJSType()) {
        // 增强选择器
        case CHTLJSType::ENHANCED_SELECTOR_START: return "ENHANCED_SELECTOR_START";
        case CHTLJSType::ENHANCED_SELECTOR_END: return "ENHANCED_SELECTOR_END";
        case CHTLJSType::SELECTOR_CLASS: return "SELECTOR_CLASS";
        case CHTLJSType::SELECTOR_ID: return "SELECTOR_ID";
        case CHTLJSType::SELECTOR_TAG: return "SELECTOR_TAG";
        case CHTLJSType::SELECTOR_INDEX: return "SELECTOR_INDEX";
        case CHTLJSType::SELECTOR_DESCENDANT: return "SELECTOR_DESCENDANT";
        case CHTLJSType::SELECTOR_AMPERSAND: return "SELECTOR_AMPERSAND";
        
        // 虚对象
        case CHTLJSType::KEYWORD_VIR: return "KEYWORD_VIR";
        case CHTLJSType::VIR_IDENTIFIER: return "VIR_IDENTIFIER";
        
        // 事件系统
        case CHTLJSType::KEYWORD_LISTEN: return "KEYWORD_LISTEN";
        case CHTLJSType::KEYWORD_DELEGATE: return "KEYWORD_DELEGATE";
        case CHTLJSType::EVENT_NAME: return "EVENT_NAME";
        case CHTLJSType::EVENT_TARGET: return "EVENT_TARGET";
        
        // 动画系统
        case CHTLJSType::KEYWORD_ANIMATE: return "KEYWORD_ANIMATE";
        case CHTLJSType::ANIM_DURATION: return "ANIM_DURATION";
        case CHTLJSType::ANIM_EASING: return "ANIM_EASING";
        case CHTLJSType::ANIM_BEGIN: return "ANIM_BEGIN";
        case CHTLJSType::ANIM_END: return "ANIM_END";
        case CHTLJSType::ANIM_WHEN: return "ANIM_WHEN";
        case CHTLJSType::ANIM_AT: return "ANIM_AT";
        case CHTLJSType::ANIM_LOOP: return "ANIM_LOOP";
        case CHTLJSType::ANIM_DIRECTION: return "ANIM_DIRECTION";
        case CHTLJSType::ANIM_DELAY: return "ANIM_DELAY";
        case CHTLJSType::ANIM_CALLBACK: return "ANIM_CALLBACK";
        
        // 方法调用
        case CHTLJSType::METHOD_ARROW: return "METHOD_ARROW";
        case CHTLJSType::METHOD_NAME: return "METHOD_NAME";
        
        // 特殊函数
        case CHTLJSType::FUNCTION_PRINTMYLOVE: return "FUNCTION_PRINTMYLOVE";
        case CHTLJSType::FUNCTION_INEVERAWAY: return "FUNCTION_INEVERAWAY";
        
        // 参数
        case CHTLJSType::PARAM_URL: return "PARAM_URL";
        case CHTLJSType::PARAM_MODE: return "PARAM_MODE";
        case CHTLJSType::PARAM_WIDTH: return "PARAM_WIDTH";
        case CHTLJSType::PARAM_HEIGHT: return "PARAM_HEIGHT";
        case CHTLJSType::PARAM_SCALE: return "PARAM_SCALE";
        
        // 特殊类型
        case CHTLJSType::VOID_TYPE: return "VOID_TYPE";
        case CHTLJSType::STATE_MARKER: return "STATE_MARKER";
        
        // 其他
        case CHTLJSType::KEY_VALUE_PAIR: return "KEY_VALUE_PAIR";
        case CHTLJSType::ARROW_FUNCTION: return "ARROW_FUNCTION";
        case CHTLJSType::OBJECT_LITERAL: return "OBJECT_LITERAL";
        case CHTLJSType::FUNCTION_DECLARATION: return "FUNCTION_DECLARATION";
        case CHTLJSType::CHTLJS_IDENTIFIER: return "CHTLJS_IDENTIFIER";
        case CHTLJSType::CHTLJS_STRING: return "CHTLJS_STRING";
        case CHTLJSType::CHTLJS_NUMBER: return "CHTLJS_NUMBER";
        case CHTLJSType::CHTLJS_COMMENT: return "CHTLJS_COMMENT";
        
        default:
            // 检查是否是基类Token类型
            if (static_cast<int>(type_) < static_cast<int>(Token::Type::CHTLJS_TOKEN_START)) {
                return Token::ToString();
            }
            return "UNKNOWN_CHTLJS_TOKEN";
    }
}

CHTLJSToken::CHTLJSType CHTLJSToken::GetKeywordType(const std::string& keyword) {
    auto it = keywordMap_.find(keyword);
    if (it != keywordMap_.end()) {
        return it->second;
    }
    return static_cast<CHTLJSType>(-1);
}

CHTLJSToken::CHTLJSType CHTLJSToken::GetFunctionType(const std::string& funcName) {
    auto it = functionMap_.find(funcName);
    if (it != functionMap_.end()) {
        return it->second;
    }
    return static_cast<CHTLJSType>(-1);
}

CHTLJSToken::CHTLJSType CHTLJSToken::GetAnimParamType(const std::string& param) {
    // 去除冒号（如果有的话）
    std::string cleanParam = param;
    if (!cleanParam.empty() && cleanParam.back() == ':') {
        cleanParam.pop_back();
    }
    
    auto it = animParamMap_.find(cleanParam);
    if (it != animParamMap_.end()) {
        return it->second;
    }
    return static_cast<CHTLJSType>(-1);
}

bool CHTLJSToken::IsKeyword(const std::string& str) {
    return keywordMap_.find(str) != keywordMap_.end();
}

bool CHTLJSToken::IsCJMODFunction(const std::string& str) {
    return functionMap_.find(str) != functionMap_.end();
}

} // namespace CHTL