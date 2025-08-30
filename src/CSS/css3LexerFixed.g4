lexer grammar css3LexerFixed;

channels {
    ERROR
}

// Tokens
Comment: '/*' .*? '*/' -> channel(HIDDEN);

Space: [ \t]+ -> channel(HIDDEN);

Cdo: '<!--';

Cdc: '-->';

Includes: '~=';

DashMatch: '|=';

Hash: '#' Name;

Import: '@' I M P O R T;

Page: '@' P A G E;

Media: '@' M E D I A;

Namespace: '@' N A M E S P A C E;

fragment NewlineOrSpace: '\r\n' | [ \t\r\n\f];

fragment Unicode: '\\' Hex Hex? Hex? Hex? Hex? Hex? NewlineOrSpace?;

fragment Escape: Unicode | '\\' ~[\r\n\f0-9a-fA-F];

fragment Nmstart: [_a-zA-Z] | Nonascii | Escape;

fragment Nmchar: [_a-zA-Z0-9-] | Nonascii | Escape;

fragment Nonascii: ~[\u0000-\u007F];

fragment Name: Nmchar+;

fragment Url: ([!#$&-~] | Nonascii | Escape)*;

Charset: '@charset ';

Important: '!' (Comment | Space)* I M P O R T A N T;

fragment Percentage_: Number '%';

Percentage: Percentage_;

fragment FontRelative: Number E [MX];

fragment ViewportRelative: Number V [HW];

fragment AbsLength: Number (P X | C M | M M | I N | P T | P C | Q);

fragment Angle: Number (D E G | R A D | G R A D | T U R N);

fragment Time: Number (M S | S);

fragment Freq: Number (H Z | K H Z);

fragment Resolution: Number (D P I | D P C M | D P P X);

fragment Length_: FontRelative | ViewportRelative | AbsLength | '0';

fragment Dimension_: Number Ident;

Length: Length_;

Dimension: Dimension_;

UnknownDimension: Number Ident;

fragment Whitespace: Space | '\t' | '\r' | '\n' | '\f';

Number: [0-9]+ | [0-9]* '.' [0-9]+;

String_: '"' (~["\n\r\f\\] | '\\' Newline | Escape)* '"' 
       | '\'' (~['\n\r\f\\] | '\\' Newline | Escape)* '\'';

PseudoNot: ':' N O T '(';

Uri: U R L '(' Whitespace* String_ Whitespace* ')'
   | U R L '(' Whitespace* Url Whitespace* ')';

Ident: '-'? Nmstart Nmchar*;

Function_: Ident '(';

// Fragments for case-insensitive matching
fragment A: [aA];
fragment B: [bB];
fragment C: [cC];
fragment D: [dD];
fragment E: [eE];
fragment F: [fF];
fragment G: [gG];
fragment H: [hH];
fragment I: [iI];
fragment J: [jJ];
fragment K: [kK];
fragment L: [lL];
fragment M: [mM];
fragment N: [nN];
fragment O: [oO];
fragment P: [pP];
fragment Q: [qQ];
fragment R: [rR];
fragment S: [sS];
fragment T: [tT];
fragment U: [uU];
fragment V: [vV];
fragment W: [wW];
fragment X: [xX];
fragment Y: [yY];
fragment Z: [zZ];

fragment Hex: [0-9a-fA-F];

fragment Newline: '\n' | '\r\n' | '\r' | '\f';

// Punctuation and operators
Semicolon: ';';
Comma: ',';
LeftBrace: '{';
RightBrace: '}';
LeftParen: '(';
RightParen: ')';
LeftBracket: '[';
RightBracket: ']';
Colon: ':';
Dot: '.';
Plus: '+';
Minus: '-';
Star: '*';
Slash: '/';
Equals: '=';
Pipe: '|';
Tilde: '~';
Greater: '>';
Dollar: '$';
Caret: '^';

// Keywords
Only: 'only';
Not: 'not';
And: 'and';

// Combined operators
TildeEquals: '~=';
PipeEquals: '|=';
CaretEquals: '^=';
DollarEquals: '$=';
StarEquals: '*=';

// Any unmatched character
AnyChar: .;