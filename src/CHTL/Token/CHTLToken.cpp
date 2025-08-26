#include "CHTLToken.h"

namespace CHTL {

// 初始化静态成员
const std::unordered_map<std::string, CHTLToken::CHTLType> CHTLToken::keywordMap_ = 
    CHTLToken::InitKeywordMap();
const std::unordered_map<std::string, CHTLToken::CHTLType> CHTLToken::typeMarkerMap_ = 
    CHTLToken::InitTypeMarkerMap();

std::unordered_map<std::string, CHTLToken::CHTLType> CHTLToken::InitKeywordMap() {
    return {
        // 块定义关键字
        {"[Template]", CHTLType::KEYWORD_TEMPLATE},
        {"[Custom]", CHTLType::KEYWORD_CUSTOM},
        {"[Configuration]", CHTLType::KEYWORD_CONFIGURATION},
        {"[Origin]", CHTLType::KEYWORD_ORIGIN},
        {"[Import]", CHTLType::KEYWORD_IMPORT},
        {"[Namespace]", CHTLType::KEYWORD_NAMESPACE},
        {"[Info]", CHTLType::KEYWORD_INFO},
        {"[Export]", CHTLType::KEYWORD_EXPORT},
        
        // 内置块关键字
        {"text", CHTLType::KEYWORD_TEXT},
        {"style", CHTLType::KEYWORD_STYLE},
        {"script", CHTLType::KEYWORD_SCRIPT},
        
        // 修饰符关键字
        {"inherit", CHTLType::KEYWORD_INHERIT},
        {"delete", CHTLType::KEYWORD_DELETE},
        {"insert", CHTLType::KEYWORD_INSERT},
        {"except", CHTLType::KEYWORD_EXCEPT},
        {"use", CHTLType::KEYWORD_USE},
        
        // 位置关键字
        {"after", CHTLType::KEYWORD_AFTER},
        {"before", CHTLType::KEYWORD_BEFORE},
        {"replace", CHTLType::KEYWORD_REPLACE},
        {"at top", CHTLType::KEYWORD_AT_TOP},
        {"at bottom", CHTLType::KEYWORD_AT_BOTTOM},
        
        // 其他关键字
        {"from", CHTLType::KEYWORD_FROM},
        {"as", CHTLType::KEYWORD_AS}
    };
}

std::unordered_map<std::string, CHTLToken::CHTLType> CHTLToken::InitTypeMarkerMap() {
    return {
        {"@Style", CHTLType::TYPE_STYLE},
        {"@Element", CHTLType::TYPE_ELEMENT},
        {"@Var", CHTLType::TYPE_VAR},
        {"@Html", CHTLType::TYPE_HTML},
        {"@JavaScript", CHTLType::TYPE_JAVASCRIPT},
        {"@Chtl", CHTLType::TYPE_CHTL},
        {"@CJmod", CHTLType::TYPE_CJMOD},
        {"@Config", CHTLType::TYPE_CONFIG}
        // 自定义类型如 @Vue 会在运行时动态添加
    };
}

std::string CHTLToken::GetTypeName() const {
    switch (GetCHTLType()) {
        // 关键字
        case CHTLType::KEYWORD_TEMPLATE: return "KEYWORD_TEMPLATE";
        case CHTLType::KEYWORD_CUSTOM: return "KEYWORD_CUSTOM";
        case CHTLType::KEYWORD_CONFIGURATION: return "KEYWORD_CONFIGURATION";
        case CHTLType::KEYWORD_ORIGIN: return "KEYWORD_ORIGIN";
        case CHTLType::KEYWORD_IMPORT: return "KEYWORD_IMPORT";
        case CHTLType::KEYWORD_NAMESPACE: return "KEYWORD_NAMESPACE";
        case CHTLType::KEYWORD_INFO: return "KEYWORD_INFO";
        case CHTLType::KEYWORD_EXPORT: return "KEYWORD_EXPORT";
        case CHTLType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case CHTLType::KEYWORD_STYLE: return "KEYWORD_STYLE";
        case CHTLType::KEYWORD_SCRIPT: return "KEYWORD_SCRIPT";
        case CHTLType::KEYWORD_INHERIT: return "KEYWORD_INHERIT";
        case CHTLType::KEYWORD_DELETE: return "KEYWORD_DELETE";
        case CHTLType::KEYWORD_INSERT: return "KEYWORD_INSERT";
        case CHTLType::KEYWORD_EXCEPT: return "KEYWORD_EXCEPT";
        case CHTLType::KEYWORD_USE: return "KEYWORD_USE";
        case CHTLType::KEYWORD_AFTER: return "KEYWORD_AFTER";
        case CHTLType::KEYWORD_BEFORE: return "KEYWORD_BEFORE";
        case CHTLType::KEYWORD_REPLACE: return "KEYWORD_REPLACE";
        case CHTLType::KEYWORD_AT_TOP: return "KEYWORD_AT_TOP";
        case CHTLType::KEYWORD_AT_BOTTOM: return "KEYWORD_AT_BOTTOM";
        case CHTLType::KEYWORD_FROM: return "KEYWORD_FROM";
        case CHTLType::KEYWORD_AS: return "KEYWORD_AS";
        
        // 类型标记
        case CHTLType::TYPE_STYLE: return "TYPE_STYLE";
        case CHTLType::TYPE_ELEMENT: return "TYPE_ELEMENT";
        case CHTLType::TYPE_VAR: return "TYPE_VAR";
        case CHTLType::TYPE_HTML: return "TYPE_HTML";
        case CHTLType::TYPE_JAVASCRIPT: return "TYPE_JAVASCRIPT";
        case CHTLType::TYPE_CHTL: return "TYPE_CHTL";
        case CHTLType::TYPE_CJMOD: return "TYPE_CJMOD";
        case CHTLType::TYPE_CONFIG: return "TYPE_CONFIG";
        case CHTLType::TYPE_VUE: return "TYPE_VUE";
        
        // 特殊标记
        case CHTLType::SQUARE_BRACKET_OPEN: return "SQUARE_BRACKET_OPEN";
        case CHTLType::SQUARE_BRACKET_CLOSE: return "SQUARE_BRACKET_CLOSE";
        case CHTLType::DOUBLE_COLON: return "DOUBLE_COLON";
        case CHTLType::ARROW_RIGHT: return "ARROW_RIGHT";
        
        // 选择器和引用
        case CHTLType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case CHTLType::ID_SELECTOR: return "ID_SELECTOR";
        case CHTLType::REFERENCE_SELECTOR: return "REFERENCE_SELECTOR";
        case CHTLType::INDEX_SELECTOR: return "INDEX_SELECTOR";
        
        // 其他
        case CHTLType::CSS_VALUE: return "CSS_VALUE";
        case CHTLType::PATH_LITERAL: return "PATH_LITERAL";
        case CHTLType::MODULE_PREFIX: return "MODULE_PREFIX";
        case CHTLType::CONFIG_KEY: return "CONFIG_KEY";
        case CHTLType::CONFIG_VALUE: return "CONFIG_VALUE";
        case CHTLType::TEMPLATE_PARAM: return "TEMPLATE_PARAM";
        case CHTLType::PLACEHOLDER: return "PLACEHOLDER";
        case CHTLType::HTML_ELEMENT: return "HTML_ELEMENT";
        case CHTLType::ATTRIBUTE_NAME: return "ATTRIBUTE_NAME";
        case CHTLType::ATTRIBUTE_VALUE: return "ATTRIBUTE_VALUE";
        case CHTLType::NAMESPACE_PATH: return "NAMESPACE_PATH";
        case CHTLType::WILDCARD: return "WILDCARD";
        
        default:
            // 检查是否是基类Token类型
            if (static_cast<int>(type_) < static_cast<int>(Token::Type::CHTL_TOKEN_START)) {
                return Token::ToString();
            }
            return "UNKNOWN_CHTL_TOKEN";
    }
}

CHTLToken::CHTLType CHTLToken::GetKeywordType(const std::string& keyword) {
    auto it = keywordMap_.find(keyword);
    if (it != keywordMap_.end()) {
        return it->second;
    }
    
    // 检查是否是HTML元素（简单判断：以字母开头）
    if (!keyword.empty() && std::isalpha(keyword[0])) {
        // 可能是HTML元素，但需要进一步验证
        return CHTLType::HTML_ELEMENT;
    }
    
    return static_cast<CHTLType>(-1); // 无效类型
}

CHTLToken::CHTLType CHTLToken::GetTypeMarkerType(const std::string& marker) {
    auto it = typeMarkerMap_.find(marker);
    if (it != typeMarkerMap_.end()) {
        return it->second;
    }
    
    // 检查是否是自定义类型（以@开头）
    if (!marker.empty() && marker[0] == '@' && marker.length() > 1) {
        // 可能是自定义原始嵌入类型
        return CHTLType::TYPE_VUE; // 使用TYPE_VUE作为自定义类型的示例
    }
    
    return static_cast<CHTLType>(-1); // 无效类型
}

bool CHTLToken::IsKeyword(const std::string& str) {
    return keywordMap_.find(str) != keywordMap_.end();
}

bool CHTLToken::IsTypeMarker(const std::string& str) {
    if (typeMarkerMap_.find(str) != typeMarkerMap_.end()) {
        return true;
    }
    
    // 检查是否是自定义类型标记
    return !str.empty() && str[0] == '@' && str.length() > 1;
}

} // namespace CHTL