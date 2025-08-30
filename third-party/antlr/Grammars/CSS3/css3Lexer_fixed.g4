// $antlr-format alignTrailingComments true, columnLimit 150, maxEmptyLinesToKeep 1, reflowComments false, useTab false
// $antlr-format allowShortRulesOnASingleLine true, allowShortBlocksOnASingleLine true, minEmptyLines 0, alignSemicolons ownLine
// $antlr-format alignColons trailing, singleLineOverrulesHangingColon true, alignLexerCommands true, alignLabels true, alignTrailers true

lexer grammar css3Lexer_fixed;

channels {
    ERROR
}

OpenBracket  : '[';
CloseBracket : ']';
OpenParen    : '(';
CloseParen   : ')';
OpenBrace    : '{';
CloseBrace   : '}';
SemiColon    : ';';
Equal        : '=';
Colon        : ':';
Dot          : '.';
Multiply     : '*';
Divide       : '/';
Pipe         : '|';
Underscore   : '_';

At: '@';

fragment Hex: [0-9a-fA-F];

// Fixed newline and space definition
fragment NewlineOrSpace: '\r\n' | [ \t\r\n\f];

fragment Unicode: '\\' Hex Hex? Hex? Hex? Hex? Hex? NewlineOrSpace?;

fragment Escape: Unicode | '\\' ~[\r\n\f0-9a-fA-F];

fragment Nonascii: ~[\u0000-\u007F];

fragment Nmstart: [_a-zA-Z] | Nonascii | Escape;

fragment Nmchar: [_a-zA-Z0-9\-] | Nonascii | Escape;

// CSS2.2 Grammar defines the following, but I'm not sure how to add them to parser for error handling
// BadString :
// BadUri :
// BadComment :
// BadUri :

Comment: '/*' .*? '*/';

fragment Name: Nmchar+;

fragment Url: ([!#$%&*-\[\]-~] | Nonascii | Escape)*;

Space: [ \t\r\n\f]+ -> channel(HIDDEN);

fragment D: [0-9];
fragment E: [Ee] [+\-]? D+;

NUMBER: D+ ('.' D+)? E?;
NAME: Nmstart Nmchar*;
DIMENSION: NUMBER NAME;

INCLUDES: '~=';
DASHMATCH: '|=';
PREFIXMATCH: '^=';
SUFFIXMATCH: '$=';
SUBSTRINGMATCH: '*=';
NOT: ':not(';

HASH: '#' NAME;

PERCENTAGE: NUMBER '%';

URI: 'url(' Space? (STRING | Url) Space? ')';

UnicodeRange: 'U+' (Hex | '?') (Hex | '?') (Hex | '?') (Hex | '?') (Hex | '?')? (Hex | '?')?;

CDO: '<!--';
CDC: '-->';

SEMICOLON: ';';
COLON: ':';
DOT: '.';
COMMA: ',';
LBRACE: '{';
RBRACE: '}';
LPAREN: '(';
RPAREN: ')';
LBRACKET: '[';
RBRACKET: ']';

PLUS: '+';
MINUS: '-';
DIVIDE: '/';
MULTIPLY: '*';

IMPORTANT_SYM: '!important';

STRING: '"' (~["\\\r\n\f] | '\\' .)* '"'
      | '\'' (~['\\\r\n\f] | '\\' .)* '\'';

IDENT: Nmstart Nmchar*;

FUNCTION: IDENT '(';

AT_KEYWORD: '@' IDENT;

// Media query tokens
ONLY: 'only';
NOT_KEYWORD: 'not';
AND: 'and';

// Error handling
ERROR_TOKEN: . -> channel(ERROR);