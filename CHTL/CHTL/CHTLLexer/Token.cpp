#include "Token.h"

namespace CHTL {

    // 静态成员初始化
    std::unordered_map<std::string, TokenType> KeywordMap::keywords;
    std::unordered_map<std::string, TokenType> KeywordMap::htmlTags;
    bool KeywordMap::initialized = false;

    // Token方法实现
    bool Token::isKeyword() const {
        return type >= TokenType::TEXT && type <= TokenType::USE;
    }

    bool Token::isHtmlTag() const {
        return type >= TokenType::HTML && type <= TokenType::H6;
    }

    bool Token::isTemplateType() const {
        return type >= TokenType::STYLE_TYPE && type <= TokenType::CJMOD_TYPE;
    }

    bool Token::isLiteral() const {
        return type == TokenType::STRING || 
               type == TokenType::UNQUOTED_STRING || 
               type == TokenType::NUMBER;
    }

    bool Token::isComment() const {
        return type == TokenType::LINE_COMMENT || 
               type == TokenType::BLOCK_COMMENT || 
               type == TokenType::GENERATOR_COMMENT;
    }

    std::string Token::toString() const {
        return "Token{type: " + tokenTypeToString(type) + 
               ", value: \"" + value + 
               "\", line: " + std::to_string(line) + 
               ", column: " + std::to_string(column) + "}";
    }

    // KeywordMap方法实现
    void KeywordMap::initialize() {
        if (initialized) return;

        // 核心语法关键字
        keywords["text"] = TokenType::TEXT;
        keywords["style"] = TokenType::STYLE;
        keywords["html5"] = TokenType::HTML5;

        // 模板系统关键字
        keywords["Template"] = TokenType::TEMPLATE;
        keywords["Custom"] = TokenType::CUSTOM;
        keywords["Origin"] = TokenType::ORIGIN;
        keywords["Import"] = TokenType::IMPORT;
        keywords["Configuration"] = TokenType::CONFIGURATION;
        keywords["Namespace"] = TokenType::NAMESPACE;
        keywords["Info"] = TokenType::INFO;
        keywords["Export"] = TokenType::EXPORT;
        keywords["Name"] = TokenType::NAME;
        keywords["OriginType"] = TokenType::ORIGIN_TYPE;

        // 模板类型
        keywords["@Style"] = TokenType::STYLE_TYPE;
        keywords["@Element"] = TokenType::ELEMENT_TYPE;
        keywords["@Var"] = TokenType::VAR_TYPE;
        keywords["@Html"] = TokenType::HTML_TYPE;
        keywords["@JavaScript"] = TokenType::JAVASCRIPT_TYPE;
        keywords["@Config"] = TokenType::CONFIG_TYPE;
        keywords["@Chtl"] = TokenType::CHTL_TYPE;
        keywords["@CJmod"] = TokenType::CJMOD_TYPE;

        // 操作符关键字
        keywords["inherit"] = TokenType::INHERIT;
        keywords["delete"] = TokenType::DELETE;
        keywords["insert"] = TokenType::INSERT;
        keywords["after"] = TokenType::AFTER;
        keywords["before"] = TokenType::BEFORE;
        keywords["replace"] = TokenType::REPLACE;
        keywords["at"] = TokenType::AT_TOP; // 需要特殊处理 "at top" 和 "at bottom"
        keywords["from"] = TokenType::FROM;
        keywords["as"] = TokenType::AS;
        keywords["except"] = TokenType::EXCEPT;
        keywords["use"] = TokenType::USE;

        // HTML标签
        htmlTags["html"] = TokenType::HTML;
        htmlTags["head"] = TokenType::HEAD;
        htmlTags["body"] = TokenType::BODY;
        htmlTags["div"] = TokenType::DIV;
        htmlTags["span"] = TokenType::SPAN;
        htmlTags["p"] = TokenType::P;
        htmlTags["a"] = TokenType::A;
        htmlTags["img"] = TokenType::IMG;
        htmlTags["input"] = TokenType::INPUT;
        htmlTags["button"] = TokenType::BUTTON;
        htmlTags["form"] = TokenType::FORM;
        htmlTags["table"] = TokenType::TABLE;
        htmlTags["tr"] = TokenType::TR;
        htmlTags["td"] = TokenType::TD;
        htmlTags["th"] = TokenType::TH;
        htmlTags["ul"] = TokenType::UL;
        htmlTags["ol"] = TokenType::OL;
        htmlTags["li"] = TokenType::LI;
        htmlTags["h1"] = TokenType::H1;
        htmlTags["h2"] = TokenType::H2;
        htmlTags["h3"] = TokenType::H3;
        htmlTags["h4"] = TokenType::H4;
        htmlTags["h5"] = TokenType::H5;
        htmlTags["h6"] = TokenType::H6;

        initialized = true;
    }

    TokenType KeywordMap::getKeywordType(const std::string& word) {
        initialize();
        auto it = keywords.find(word);
        return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    }

    TokenType KeywordMap::getHtmlTagType(const std::string& tag) {
        initialize();
        auto it = htmlTags.find(tag);
        return (it != htmlTags.end()) ? it->second : TokenType::IDENTIFIER;
    }

    bool KeywordMap::isKeyword(const std::string& word) {
        initialize();
        return keywords.find(word) != keywords.end();
    }

    bool KeywordMap::isHtmlTag(const std::string& tag) {
        initialize();
        return htmlTags.find(tag) != htmlTags.end();
    }

    // Token类型转字符串函数
    std::string tokenTypeToString(TokenType type) {
        switch (type) {
            // 基础符号
            case TokenType::LEFT_BRACE: return "LEFT_BRACE";
            case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
            case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
            case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
            case TokenType::LEFT_PAREN: return "LEFT_PAREN";
            case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
            case TokenType::SEMICOLON: return "SEMICOLON";
            case TokenType::COLON: return "COLON";
            case TokenType::EQUAL: return "EQUAL";
            case TokenType::COMMA: return "COMMA";
            case TokenType::DOT: return "DOT";
            case TokenType::AMPERSAND: return "AMPERSAND";
            case TokenType::HASH: return "HASH";
            case TokenType::AT: return "AT";

            // 字面量
            case TokenType::STRING: return "STRING";
            case TokenType::UNQUOTED_STRING: return "UNQUOTED_STRING";
            case TokenType::NUMBER: return "NUMBER";

            // 注释
            case TokenType::LINE_COMMENT: return "LINE_COMMENT";
            case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
            case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";

            // 核心语法关键字
            case TokenType::TEXT: return "TEXT";
            case TokenType::STYLE: return "STYLE";
            case TokenType::HTML5: return "HTML5";

            // 模板系统关键字
            case TokenType::TEMPLATE: return "TEMPLATE";
            case TokenType::CUSTOM: return "CUSTOM";
            case TokenType::ORIGIN: return "ORIGIN";
            case TokenType::IMPORT: return "IMPORT";
            case TokenType::CONFIGURATION: return "CONFIGURATION";
            case TokenType::NAMESPACE: return "NAMESPACE";
            case TokenType::INFO: return "INFO";
            case TokenType::EXPORT: return "EXPORT";
            case TokenType::NAME: return "NAME";
            case TokenType::ORIGIN_TYPE: return "ORIGIN_TYPE";

            // 模板类型
            case TokenType::STYLE_TYPE: return "STYLE_TYPE";
            case TokenType::ELEMENT_TYPE: return "ELEMENT_TYPE";
            case TokenType::VAR_TYPE: return "VAR_TYPE";
            case TokenType::HTML_TYPE: return "HTML_TYPE";
            case TokenType::JAVASCRIPT_TYPE: return "JAVASCRIPT_TYPE";
            case TokenType::CONFIG_TYPE: return "CONFIG_TYPE";
            case TokenType::CHTL_TYPE: return "CHTL_TYPE";
            case TokenType::CJMOD_TYPE: return "CJMOD_TYPE";

            // 操作符关键字
            case TokenType::INHERIT: return "INHERIT";
            case TokenType::DELETE: return "DELETE";
            case TokenType::INSERT: return "INSERT";
            case TokenType::AFTER: return "AFTER";
            case TokenType::BEFORE: return "BEFORE";
            case TokenType::REPLACE: return "REPLACE";
            case TokenType::AT_TOP: return "AT_TOP";
            case TokenType::AT_BOTTOM: return "AT_BOTTOM";
            case TokenType::FROM: return "FROM";
            case TokenType::AS: return "AS";
            case TokenType::EXCEPT: return "EXCEPT";
            case TokenType::USE: return "USE";

            // HTML标签
            case TokenType::HTML: return "HTML";
            case TokenType::HEAD: return "HEAD";
            case TokenType::BODY: return "BODY";
            case TokenType::DIV: return "DIV";
            case TokenType::SPAN: return "SPAN";
            case TokenType::P: return "P";
            case TokenType::A: return "A";
            case TokenType::IMG: return "IMG";
            case TokenType::INPUT: return "INPUT";
            case TokenType::BUTTON: return "BUTTON";
            case TokenType::FORM: return "FORM";
            case TokenType::TABLE: return "TABLE";
            case TokenType::TR: return "TR";
            case TokenType::TD: return "TD";
            case TokenType::TH: return "TH";
            case TokenType::UL: return "UL";
            case TokenType::OL: return "OL";
            case TokenType::LI: return "LI";
            case TokenType::H1: return "H1";
            case TokenType::H2: return "H2";
            case TokenType::H3: return "H3";
            case TokenType::H4: return "H4";
            case TokenType::H5: return "H5";
            case TokenType::H6: return "H6";

            // 特殊标识符
            case TokenType::IDENTIFIER: return "IDENTIFIER";

            // 结束符
            case TokenType::EOF_TOKEN: return "EOF_TOKEN";
            case TokenType::NEWLINE: return "NEWLINE";
            case TokenType::WHITESPACE: return "WHITESPACE";

            // 错误
            case TokenType::UNKNOWN: return "UNKNOWN";

            default: return "UNKNOWN";
        }
    }
}