grammar CSSSimple;

// Parser rules
stylesheet
    : statement* EOF
    ;

statement
    : ruleset
    | atRule
    ;

ruleset
    : selector (',' selector)* '{' declaration* '}'
    ;

selector
    : simpleSelector+
    ;

simpleSelector
    : (Ident | '*' | Hash | '.' Ident | '[' Ident ']')
    ;

declaration
    : property ':' value ';'?
    ;

property
    : Ident
    ;

value
    : (Ident | Number | String_ | Color | Function valueList ')' | Percentage | Dimension)+
    ;

valueList
    : value (',' value)*
    ;

atRule
    : '@' Ident any* (';' | '{' statement* '}')
    ;

any
    : (Ident | Number | String_ | Color | ':' | ';' | ',' | '(' | ')' | '[' | ']')
    ;

// Lexer rules
Ident
    : [a-zA-Z_] [a-zA-Z0-9_-]*
    ;

Hash
    : '#' [a-zA-Z0-9]+
    ;

Number
    : [0-9]+ ('.' [0-9]+)?
    ;

Percentage
    : Number '%'
    ;

Dimension
    : Number [a-zA-Z]+
    ;

String_
    : '"' (~["\r\n])* '"'
    | '\'' (~['\r\n])* '\''
    ;

Color
    : '#' [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F] ([0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F])?
    ;

Function
    : Ident '('
    ;

Comment
    : '/*' .*? '*/' -> skip
    ;

WS
    : [ \t\r\n]+ -> skip
    ;