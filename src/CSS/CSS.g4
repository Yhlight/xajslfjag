grammar CSS;

// Parser Rules
stylesheet
    : statement*
    ;

statement
    : ruleSet
    | atRule
    | comment
    ;

atRule
    : AT_KEYWORD ws* (any | block)* ';'?
    ;

ruleSet
    : selectors '{' declaration* '}'
    ;

selectors
    : selector (',' ws* selector)*
    ;

selector
    : simpleSelector (combinator simpleSelector)*
    ;

combinator
    : ws* ('+' | '>' | '~' | '||') ws*
    | ws+
    ;

simpleSelector
    : (typeSelector | universal)? (idSelector | classSelector | attributeSelector | pseudoSelector)*
    | (idSelector | classSelector | attributeSelector | pseudoSelector)+
    ;

typeSelector
    : IDENT
    ;

universal
    : '*'
    ;

idSelector
    : '#' IDENT
    ;

classSelector
    : '.' IDENT
    ;

attributeSelector
    : '[' ws* IDENT ws* (attributeOperator ws* (STRING | IDENT) ws*)? ']'
    ;

attributeOperator
    : '=' | '~=' | '|=' | '^=' | '$=' | '*='
    ;

pseudoSelector
    : ':' ':'? (IDENT | function)
    ;

function
    : IDENT '(' ws* expression? ws* ')'
    ;

declaration
    : property ':' ws* values ws* important? ';'?
    ;

property
    : IDENT
    ;

values
    : value (ws+ value)*
    ;

value
    : dimension
    | percentage
    | number
    | string
    | color
    | url
    | function
    | IDENT
    ;

dimension
    : NUMBER UNIT
    ;

percentage
    : NUMBER '%'
    ;

number
    : NUMBER
    ;

string
    : STRING
    ;

color
    : HASH
    ;

url
    : 'url(' ws* (STRING | URL_VALUE) ws* ')'
    ;

important
    : '!' ws* 'important'
    ;

block
    : '{' any* '}'
    ;

any
    : ~('{' | '}' | ';')*
    ;

comment
    : COMMENT
    ;

ws
    : WS
    ;

expression
    : term (ws* operator ws* term)*
    ;

term
    : value
    | '(' ws* expression ws* ')'
    ;

operator
    : '+' | '-' | '*' | '/' | ','
    ;

// Lexer Rules
AT_KEYWORD
    : '@' IDENT
    ;

HASH
    : '#' HEX+
    ;

STRING
    : '"' (~["\\\r\n] | '\\' .)* '"'
    | '\'' (~['\\\r\n] | '\\' .)* '\''
    ;

NUMBER
    : '-'? (DIGIT+ ('.' DIGIT*)? | '.' DIGIT+)
    ;

UNIT
    : 'px' | 'em' | 'rem' | 'vh' | 'vw' | '%'
    | 'pt' | 'cm' | 'mm' | 'in' | 'pc'
    | 'deg' | 'rad' | 'grad' | 'turn'
    | 's' | 'ms'
    | 'Hz' | 'kHz'
    ;

IDENT
    : (LETTER | '_' | '-') (LETTER | DIGIT | '_' | '-')*
    ;

URL_VALUE
    : ~[)'"\s]+
    ;

COMMENT
    : '/*' .*? '*/' -> channel(HIDDEN)
    ;

WS
    : [ \t\r\n]+ -> channel(HIDDEN)
    ;

fragment LETTER
    : [a-zA-Z]
    ;

fragment DIGIT
    : [0-9]
    ;

fragment HEX
    : [0-9a-fA-F]
    ;