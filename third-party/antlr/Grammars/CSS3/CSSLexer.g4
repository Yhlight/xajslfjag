lexer grammar CSSLexer;

channels {
    ERROR
}

// Basic symbols
LBRACE    : '{';
RBRACE    : '}';
LPAREN    : '(';
RPAREN    : ')';
LBRACKET  : '[';
RBRACKET  : ']';
SEMICOLON : ';';
COLON     : ':';
COMMA     : ',';
DOT       : '.';
HASH      : '#';
AT        : '@';
PLUS      : '+';
MINUS     : '-';
MULTIPLY  : '*';
DIVIDE    : '/';
EQUALS    : '=';

// CSS-specific operators
INCLUDES       : '~=';
DASHMATCH      : '|=';
PREFIXMATCH    : '^=';
SUFFIXMATCH    : '$=';
SUBSTRINGMATCH : '*=';

// String literals
STRING : '"' (~["\\\r\n] | '\\' .)* '"'
       | '\'' (~['\\\r\n] | '\\' .)* '\'';

// Numbers
fragment DIGIT : [0-9];
fragment EXPONENT : [eE] [+-]? DIGIT+;
NUMBER : DIGIT+ ('.' DIGIT+)? EXPONENT?;
PERCENTAGE : NUMBER '%';

// Identifiers
fragment NMSTART : [a-zA-Z_] | NONASCII | ESCAPE;
fragment NMCHAR  : [a-zA-Z0-9_-] | NONASCII | ESCAPE;
fragment NONASCII : ~[\u0000-\u007F];
fragment ESCAPE   : '\\' [0-9a-fA-F] [0-9a-fA-F]? [0-9a-fA-F]? [0-9a-fA-F]? [0-9a-fA-F]? [0-9a-fA-F]?
                  | '\\' ~[\r\n\f0-9a-fA-F];

IDENT : NMSTART NMCHAR*;
DIMENSION : NUMBER IDENT;

// Functions
FUNCTION : IDENT '(';

// URLs
URI : 'url(' WS* (STRING | URL_CHARS) WS* ')';
fragment URL_CHARS : (~[ \t\r\n\f()"'] | '\\' .)*;

// Comments
COMMENT : '/*' .*? '*/' -> channel(HIDDEN);

// Whitespace
WS : [ \t\r\n\f]+ -> channel(HIDDEN);

// Important
IMPORTANT : '!' WS* 'important';

// CDO/CDC for HTML comments
CDO : '<!--';
CDC : '-->';

// Media query keywords
ONLY_KEYWORD : 'only';
NOT_KEYWORD  : 'not';
AND_KEYWORD  : 'and';

// At-rules
AT_CHARSET   : '@charset';
AT_IMPORT    : '@import';
AT_NAMESPACE : '@namespace';
AT_MEDIA     : '@media';
AT_PAGE      : '@page';
AT_FONT_FACE : '@font-face';
AT_KEYFRAMES : '@keyframes';
AT_SUPPORTS  : '@supports';

// Special keywords
FROM_KEYWORD : 'from';
TO_KEYWORD   : 'to';
OR_KEYWORD   : 'or';

// Special selectors
TILDE        : '~';
PIPE         : '|';
NOT_FUNCTION : ':not(';

// Any other character
ANYCHAR : .;