#pragma once

#include "Common/Token.h"
#include <unordered_map>

namespace CHTL {

// CHTL专用Token
class CHTLToken : public Token {
public:
    // CHTL特有的Token类型
    enum class CHTLType : int {
        // === 关键字 ===
        // 块定义关键字
        KEYWORD_TEMPLATE = static_cast<int>(Token::Type::CHTL_TOKEN_START),
        KEYWORD_CUSTOM,
        KEYWORD_CONFIGURATION,
        KEYWORD_ORIGIN,
        KEYWORD_IMPORT,
        KEYWORD_NAMESPACE,
        KEYWORD_INFO,
        KEYWORD_EXPORT,
        
        // 内置块关键字
        KEYWORD_TEXT,
        KEYWORD_STYLE,
        KEYWORD_SCRIPT,
        
        // 修饰符关键字
        KEYWORD_INHERIT,
        KEYWORD_DELETE,
        KEYWORD_INSERT,
        KEYWORD_EXCEPT,
        KEYWORD_USE,
        
        // 位置关键字
        KEYWORD_AFTER,
        KEYWORD_BEFORE,
        KEYWORD_REPLACE,
        KEYWORD_AT_TOP,
        KEYWORD_AT_BOTTOM,
        
        // 其他关键字
        KEYWORD_FROM,
        KEYWORD_AS,
        
        // === 类型标记 ===
        TYPE_STYLE,        // @Style
        TYPE_ELEMENT,      // @Element
        TYPE_VAR,          // @Var
        TYPE_HTML,         // @Html
        TYPE_JAVASCRIPT,   // @JavaScript
        TYPE_CHTL,         // @Chtl
        TYPE_CJMOD,        // @CJmod
        TYPE_CONFIG,       // @Config
        TYPE_VUE,          // @Vue (自定义原始嵌入类型示例)
        
        // === 特殊标记 ===
        SQUARE_BRACKET_OPEN,   // [
        SQUARE_BRACKET_CLOSE,  // ]
        DOUBLE_COLON,          // ::
        ARROW_RIGHT,           // ->
        
        // === CE对等式 ===
        // COLON 和 EQUALS 在基类中已定义，这里只是标记它们的等价性
        
        // === 选择器和引用 ===
        CLASS_SELECTOR,        // .className
        ID_SELECTOR,           // #idName
        REFERENCE_SELECTOR,    // & (上下文推导)
        INDEX_SELECTOR,        // [index]
        
        // === 字面量扩展 ===
        CSS_VALUE,             // CSS属性值
        PATH_LITERAL,          // 文件路径
        MODULE_PREFIX,         // chtl:: (官方模块前缀)
        
        // === 配置相关 ===
        CONFIG_KEY,            // 配置键名
        CONFIG_VALUE,          // 配置值
        
        // === 模板/自定义相关 ===
        TEMPLATE_PARAM,        // 模板参数
        PLACEHOLDER,           // 占位符
        
        // === HTML元素 ===
        HTML_ELEMENT,          // HTML元素名
        
        // === 属性相关 ===
        ATTRIBUTE_NAME,        // 属性名
        ATTRIBUTE_VALUE,       // 属性值
        
        // === 其他 ===
        NAMESPACE_PATH,        // 命名空间路径 (space.room2)
        WILDCARD               // 通配符 *
    };
    
    CHTLToken(CHTLType type, const std::string& lexeme, const Location& location)
        : Token(static_cast<Token::Type>(type), lexeme, location) {}
    
    CHTLType GetCHTLType() const { 
        return static_cast<CHTLType>(type_); 
    }
    
    // 检查是否为CHTL关键字
    bool IsCHTLKeyword() const {
        int typeValue = static_cast<int>(type_);
        return typeValue >= static_cast<int>(CHTLType::KEYWORD_TEMPLATE) &&
               typeValue <= static_cast<int>(CHTLType::KEYWORD_AS);
    }
    
    // 检查是否为类型标记
    bool IsTypeMarker() const {
        int typeValue = static_cast<int>(type_);
        return typeValue >= static_cast<int>(CHTLType::TYPE_STYLE) &&
               typeValue <= static_cast<int>(CHTLType::TYPE_VUE);
    }
    
    // 检查是否为块定义关键字
    bool IsBlockKeyword() const {
        CHTLType chtlType = GetCHTLType();
        return chtlType == CHTLType::KEYWORD_TEMPLATE ||
               chtlType == CHTLType::KEYWORD_CUSTOM ||
               chtlType == CHTLType::KEYWORD_CONFIGURATION ||
               chtlType == CHTLType::KEYWORD_ORIGIN ||
               chtlType == CHTLType::KEYWORD_NAMESPACE ||
               chtlType == CHTLType::KEYWORD_TEXT ||
               chtlType == CHTLType::KEYWORD_STYLE ||
               chtlType == CHTLType::KEYWORD_SCRIPT;
    }
    
    // 获取Token类型名称
    std::string GetTypeName() const;
    
    // 静态方法：根据关键字字符串获取Token类型
    static CHTLType GetKeywordType(const std::string& keyword);
    static CHTLType GetTypeMarkerType(const std::string& marker);
    
    // 判断字符串是否为关键字
    static bool IsKeyword(const std::string& str);
    static bool IsTypeMarker(const std::string& str);
    
private:
    // 关键字映射表
    static const std::unordered_map<std::string, CHTLType> keywordMap_;
    static const std::unordered_map<std::string, CHTLType> typeMarkerMap_;
    
    // 初始化映射表
    static std::unordered_map<std::string, CHTLType> InitKeywordMap();
    static std::unordered_map<std::string, CHTLType> InitTypeMarkerMap();
};

using CHTLTokenPtr = std::shared_ptr<CHTLToken>;

} // namespace CHTL