lexer grammar SimpleJSLexer;

channels {
    COMMENTS
}

// Keywords
BREAK         : 'break';
CASE          : 'case';
CATCH         : 'catch';
CLASS         : 'class';
CONST         : 'const';
CONTINUE      : 'continue';
DEFAULT       : 'default';
DELETE        : 'delete';
DO            : 'do';
ELSE          : 'else';
EXPORT        : 'export';
EXTENDS       : 'extends';
FINALLY       : 'finally';
FOR           : 'for';
FUNCTION      : 'function';
IF            : 'if';
IMPORT        : 'import';
IN            : 'in';
INSTANCEOF    : 'instanceof';
LET           : 'let';
NEW           : 'new';
RETURN        : 'return';
SUPER         : 'super';
SWITCH        : 'switch';
THIS          : 'this';
THROW         : 'throw';
TRY           : 'try';
TYPEOF        : 'typeof';
VAR           : 'var';
VOID          : 'void';
WHILE         : 'while';
WITH          : 'with';
ASYNC         : 'async';
AWAIT         : 'await';
YIELD         : 'yield';
OF            : 'of';
FROM          : 'from';
AS            : 'as';

// Literals
NULL          : 'null';
TRUE          : 'true';
FALSE         : 'false';
UNDEFINED     : 'undefined';

// Operators
ASSIGN        : '=';
PLUS_ASSIGN   : '+=';
MINUS_ASSIGN  : '-=';
MULT_ASSIGN   : '*=';
DIV_ASSIGN    : '/=';
MOD_ASSIGN    : '%=';
PLUS_PLUS     : '++';
MINUS_MINUS   : '--';
PLUS          : '+';
MINUS         : '-';
MULTIPLY      : '*';
DIVIDE        : '/';
MODULUS       : '%';
EQ            : '==';
NE            : '!=';
EQ_STRICT     : '===';
NE_STRICT     : '!==';
LT            : '<';
LE            : '<=';
GT            : '>';
GE            : '>=';
AND           : '&&';
OR            : '||';
NOT           : '!';
BIT_AND       : '&';
BIT_OR        : '|';
BIT_XOR       : '^';
BIT_NOT       : '~';
LSHIFT        : '<<';
RSHIFT        : '>>';
URSHIFT       : '>>>';
QUESTION      : '?';
COLON         : ':';
ARROW         : '=>';

// Punctuation
SEMICOLON     : ';';
COMMA         : ',';
DOT           : '.';
LPAREN        : '(';
RPAREN        : ')';
LBRACE        : '{';
RBRACE        : '}';
LBRACKET      : '[';
RBRACKET      : ']';

// Numbers
NUMBER        : DECIMAL | HEX | OCTAL | BINARY;

fragment DECIMAL : [0-9]+ ('.' [0-9]+)? ([eE] [+-]? [0-9]+)?;
fragment HEX     : '0' [xX] [0-9a-fA-F]+;
fragment OCTAL   : '0' [0-7]+;
fragment BINARY  : '0' [bB] [01]+;

// Strings
STRING        : '"' (~["\\\r\n] | '\\' .)* '"'
              | '\'' (~['\\\r\n] | '\\' .)* '\'';

// Template literals
TEMPLATE      : '`' (~[`\\] | '\\' .)* '`';

// Regular expressions
REGEX         : '/' (~[/\\\r\n] | '\\' .)+ '/' [gimsuyx]*;

// Identifiers
IDENTIFIER    : [a-zA-Z_$] [a-zA-Z0-9_$]*;

// Comments
LINE_COMMENT  : '//' ~[\r\n]* -> channel(COMMENTS);
BLOCK_COMMENT : '/*' .*? '*/' -> channel(COMMENTS);

// Whitespace
WS            : [ \t\r\n\f]+ -> skip;

// Any character
ANY           : .;