#include "CHTLToken.h"

namespace CHTL {

std::unordered_map<std::string, TokenType> keywordMap;

void initializeKeywordMap() {
    // CHTL 关键字
    keywordMap["text"] = TokenType::TEXT;
    keywordMap["style"] = TokenType::STYLE;
    keywordMap["script"] = TokenType::SCRIPT;
    keywordMap["namespace"] = TokenType::NAMESPACE;
    keywordMap["from"] = TokenType::FROM;
    keywordMap["as"] = TokenType::AS;
    keywordMap["delete"] = TokenType::DELETE;
    
    // HTML 标签
    keywordMap["html"] = TokenType::HTML;
    keywordMap["head"] = TokenType::HEAD;
    keywordMap["body"] = TokenType::BODY;
    keywordMap["div"] = TokenType::DIV;
    keywordMap["span"] = TokenType::SPAN;
    keywordMap["p"] = TokenType::P;
    keywordMap["a"] = TokenType::A;
    keywordMap["img"] = TokenType::IMG;
    keywordMap["ul"] = TokenType::UL;
    keywordMap["ol"] = TokenType::OL;
    keywordMap["li"] = TokenType::LI;
    keywordMap["table"] = TokenType::TABLE;
    keywordMap["tr"] = TokenType::TR;
    keywordMap["td"] = TokenType::TD;
    keywordMap["th"] = TokenType::TH;
    keywordMap["form"] = TokenType::FORM;
    keywordMap["input"] = TokenType::INPUT;
    keywordMap["button"] = TokenType::BUTTON;
    keywordMap["textarea"] = TokenType::TEXTAREA;
    keywordMap["select"] = TokenType::SELECT;
    keywordMap["option"] = TokenType::OPTION;
    keywordMap["h1"] = TokenType::H1;
    keywordMap["h2"] = TokenType::H2;
    keywordMap["h3"] = TokenType::H3;
    keywordMap["h4"] = TokenType::H4;
    keywordMap["h5"] = TokenType::H5;
    keywordMap["h6"] = TokenType::H6;
    keywordMap["br"] = TokenType::BR;
    keywordMap["hr"] = TokenType::HR;
    keywordMap["meta"] = TokenType::META;
    keywordMap["link"] = TokenType::LINK;
    keywordMap["title"] = TokenType::TITLE;
    keywordMap["nav"] = TokenType::NAV;
    keywordMap["header"] = TokenType::HEADER;
    keywordMap["footer"] = TokenType::FOOTER;
    keywordMap["main"] = TokenType::MAIN;
    keywordMap["section"] = TokenType::SECTION;
    keywordMap["article"] = TokenType::ARTICLE;
    keywordMap["aside"] = TokenType::ASIDE;
    
    // 导入类型
    keywordMap["@Chtl"] = TokenType::AT_CHTL;
    keywordMap["@CJmod"] = TokenType::AT_CJMOD;
    keywordMap["@Html"] = TokenType::AT_HTML;
    keywordMap["@Style"] = TokenType::AT_STYLE;
    keywordMap["@JavaScript"] = TokenType::AT_JAVASCRIPT;
}

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::DELETE: return "DELETE";
        case TokenType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case TokenType::CUSTOM_VAR: return "CUSTOM_VAR";
        case TokenType::SELECTOR_REF: return "SELECTOR_REF";
        case TokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case TokenType::ID_SELECTOR: return "ID_SELECTOR";
        case TokenType::PSEUDO_CLASS: return "PSEUDO_CLASS";
        case TokenType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
        case TokenType::DOUBLE_BRACE_OPEN: return "{{";
        case TokenType::DOUBLE_BRACE_CLOSE: return "}}";
        case TokenType::ARROW: return "->";
        case TokenType::FAT_ARROW: return "=>";
        case TokenType::COLON: return ":";
        case TokenType::SEMICOLON: return ";";
        case TokenType::COMMA: return ",";
        case TokenType::DOT: return ".";
        case TokenType::EQUALS: return "=";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::ASTERISK: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::PERCENT: return "%";
        case TokenType::EXCLAMATION: return "!";
        case TokenType::QUESTION: return "?";
        case TokenType::AT: return "@";
        case TokenType::HASH: return "#";
        case TokenType::DOLLAR: return "$";
        case TokenType::AMPERSAND: return "&";
        case TokenType::PIPE: return "|";
        case TokenType::TILDE: return "~";
        case TokenType::CARET: return "^";
        case TokenType::LEFT_BRACE: return "{";
        case TokenType::RIGHT_BRACE: return "}";
        case TokenType::LEFT_PAREN: return "(";
        case TokenType::RIGHT_PAREN: return ")";
        case TokenType::LEFT_BRACKET: return "[";
        case TokenType::RIGHT_BRACKET: return "]";
        case TokenType::LESS_THAN: return "<";
        case TokenType::GREATER_THAN: return ">";
        case TokenType::LESS_EQUAL: return "<=";
        case TokenType::GREATER_EQUAL: return ">=";
        case TokenType::EQUAL_EQUAL: return "==";
        case TokenType::NOT_EQUAL: return "!=";
        case TokenType::AND_AND: return "&&";
        case TokenType::OR_OR: return "||";
        case TokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case TokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WILDCARD: return "WILDCARD";
        case TokenType::DOUBLE_COLON: return "::";
        case TokenType::AT_CHTL: return "@Chtl";
        case TokenType::AT_CJMOD: return "@CJmod";
        case TokenType::AT_HTML: return "@Html";
        case TokenType::AT_STYLE: return "@Style";
        case TokenType::AT_JAVASCRIPT: return "@JavaScript";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: {
            // HTML标签
            if (type >= TokenType::HTML && type <= TokenType::ASIDE) {
                switch (type) {
                    case TokenType::HTML: return "HTML";
                    case TokenType::HEAD: return "HEAD";
                    case TokenType::BODY: return "BODY";
                    case TokenType::DIV: return "DIV";
                    case TokenType::SPAN: return "SPAN";
                    case TokenType::P: return "P";
                    case TokenType::A: return "A";
                    case TokenType::IMG: return "IMG";
                    case TokenType::UL: return "UL";
                    case TokenType::OL: return "OL";
                    case TokenType::LI: return "LI";
                    case TokenType::TABLE: return "TABLE";
                    case TokenType::TR: return "TR";
                    case TokenType::TD: return "TD";
                    case TokenType::TH: return "TH";
                    case TokenType::FORM: return "FORM";
                    case TokenType::INPUT: return "INPUT";
                    case TokenType::BUTTON: return "BUTTON";
                    case TokenType::TEXTAREA: return "TEXTAREA";
                    case TokenType::SELECT: return "SELECT";
                    case TokenType::OPTION: return "OPTION";
                    case TokenType::H1: return "H1";
                    case TokenType::H2: return "H2";
                    case TokenType::H3: return "H3";
                    case TokenType::H4: return "H4";
                    case TokenType::H5: return "H5";
                    case TokenType::H6: return "H6";
                    case TokenType::BR: return "BR";
                    case TokenType::HR: return "HR";
                    case TokenType::META: return "META";
                    case TokenType::LINK: return "LINK";
                    case TokenType::TITLE: return "TITLE";
                    case TokenType::NAV: return "NAV";
                    case TokenType::HEADER: return "HEADER";
                    case TokenType::FOOTER: return "FOOTER";
                    case TokenType::MAIN: return "MAIN";
                    case TokenType::SECTION: return "SECTION";
                    case TokenType::ARTICLE: return "ARTICLE";
                    case TokenType::ASIDE: return "ASIDE";
                    default: break;
                }
            }
            return "UNKNOWN";
        }
    }
}

} // namespace CHTL