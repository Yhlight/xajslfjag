grammar CSS;

// 主规则
stylesheet: (importRule | rule)* EOF;

// 导入规则
importRule: IMPORT STRING_LITERAL SEMICOLON;

// CSS规则
rule: selector LBRACE declaration* RBRACE;

// 选择器
selector: simpleSelector (combinator simpleSelector)*;

simpleSelector: 
    | elementSelector
    | classSelector
    | idSelector
    | attributeSelector
    | pseudoClassSelector
    | pseudoElementSelector
    | universalSelector
    ;

elementSelector: IDENTIFIER;
classSelector: DOT IDENTIFIER;
idSelector: HASH IDENTIFIER;
universalSelector: ASTERISK;

// 组合器
combinator: 
    | WHITESPACE                    // 后代选择器
    | GREATER_THAN                  // 子选择器
    | PLUS                          // 相邻兄弟选择器
    | TILDE                         // 一般兄弟选择器
    ;

// 属性选择器
attributeSelector: 
    LBRACKET IDENTIFIER (
        EQUALS STRING_LITERAL |
        CONTAINS STRING_LITERAL |
        STARTS_WITH STRING_LITERAL |
        ENDS_WITH STRING_LITERAL |
        WORD_MATCH STRING_LITERAL |
        LANG_MATCH STRING_LITERAL
    )? RBRACKET;

// 伪类选择器
pseudoClassSelector: 
    COLON IDENTIFIER (LPAREN (IDENTIFIER | NUMBER | STRING_LITERAL) RPAREN)?;

// 伪元素选择器
pseudoElementSelector: 
    DOUBLE_COLON IDENTIFIER;

// 声明
declaration: property COLON value SEMICOLON?;

property: IDENTIFIER;

value: valueItem (COMMA valueItem)*;

valueItem: 
    | NUMBER UNIT?
    | STRING_LITERAL
    | IDENTIFIER
    | functionCall
    | hexColor
    ;

functionCall: IDENTIFIER LPAREN valueList? RPAREN;

valueList: value (COMMA value)*;

hexColor: HASH HEXDIGIT+;

// 词法规则
IMPORT: '@import';
LBRACE: '{';
RBRACE: '}';
LBRACKET: '[';
RBRACKET: ']';
LPAREN: '(';
RPAREN: ')';
SEMICOLON: ';';
COLON: ':';
DOUBLE_COLON: '::';
DOT: '.';
HASH: '#';
COMMA: ',';
ASTERISK: '*';
GREATER_THAN: '>';
PLUS: '+';
TILDE: '~';

// 属性选择器操作符
EQUALS: '=';
CONTAINS: '*=';
STARTS_WITH: '^=';
ENDS_WITH: '$=';
WORD_MATCH: '~=';
LANG_MATCH: '|=';

// 标识符和字面量
IDENTIFIER: [a-zA-Z_-][a-zA-Z0-9_-]*;
NUMBER: [0-9]+ ('.' [0-9]+)?;
UNIT: 'px' | 'em' | 'rem' | '%' | 'vh' | 'vw' | 'pt' | 'pc' | 'in' | 'cm' | 'mm';
STRING_LITERAL: '"' (~["\r\n])* '"' | '\'' (~['\r\n])* '\'';
HEXDIGIT: [0-9a-fA-F];

// 空白字符
WHITESPACE: [ \t\r\n]+ -> skip;

// 注释
COMMENT: '/*' .*? '*/' -> skip;
LINE_COMMENT: '//' ~[\r\n]* -> skip;