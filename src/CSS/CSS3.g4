grammar CSS3;

// Parser rules
stylesheet
    : ws? nestedStatement* ws? EOF
    ;

nestedStatement
    : ruleset
    | media
    | page
    | fontFaceRule
    | keyframesRule
    | supportsRule
    | viewport
    | atRule
    ;

ruleset
    : selectors ws? '{' ws? declarationList? '}' ws?
    ;

selectors
    : selector (ws? ',' ws? selector)*
    ;

selector
    : simpleSelectorSequence ws? (combinator simpleSelectorSequence ws?)*
    ;

combinator
    : ws? ('>>' | '>' | '+' | '~') ws?
    | ws
    ;

simpleSelectorSequence
    : (typeSelector | universal) (hash | className | attrib | pseudo | negation)*
    | (hash | className | attrib | pseudo | negation)+
    ;

typeSelector
    : typeNamespacePrefix? elementName
    ;

typeNamespacePrefix
    : (Ident | '*')? '|'
    ;

elementName
    : Ident
    ;

universal
    : typeNamespacePrefix? '*'
    ;

className
    : '.' Ident
    ;

attrib
    : '[' ws? typeNamespacePrefix? Ident ws? (attribOperator ws? (Ident | String_) ws?)? ']'
    ;

attribOperator
    : '=' | '~=' | '|=' | '^=' | '$=' | '*='
    ;

pseudo
    : ':' ':'? (Ident | functionalPseudo)
    ;

functionalPseudo
    : Function ws? expression ')'
    ;

expression
    : (ws? '+' ws? | ws? '-' ws? | ws?)? term (ws? operator ws? term)*
    ;

operator
    : '/' | ',' | ws
    ;

term
    : number ws?
    | percentage ws?
    | dimension ws?
    | String_ ws?
    | UnicodeRange ws?
    | Ident ws?
    | var
    | Uri ws?
    | hexcolor
    | calc
    | Function ws? expression ')' ws?
    ;

var
    : Var ws? Variable ws? ')' ws?
    ;

calc
    : Calc ws? calcSum ')' ws?
    ;

calcSum
    : calcProduct ws? (('+' | '-') ws? calcProduct ws?)*
    ;

calcProduct
    : calcValue ws? (('*' | '/') ws? calcValue ws?)*
    ;

calcValue
    : number ws?
    | dimension ws?
    | percentage ws?
    | '(' ws? calcSum ')' ws?
    ;

negation
    : PseudoNot ws? negationArg ws? ')'
    ;

negationArg
    : typeSelector
    | universal
    | hash
    | className
    | attrib
    | pseudo
    ;

declarationList
    : (declaration? ';' ws?)* declaration?
    ;

declaration
    : property ':' ws? expr prio?
    | property ':' ws? value
    ;

property
    : Ident ws?
    | Variable ws?
    ;

prio
    : Important ws?
    ;

value
    : (any | block | atKeyword ws?)+
    ;

expr
    : term (operator term)*
    ;

hexcolor
    : Hash ws?
    ;

hash
    : Hash ws?
    ;

atKeyword
    : AtKeyword ws?
    ;

number
    : (Plus | Minus)? (Number | Percentage | Dimension)
    ;

percentage
    : (Plus | Minus)? Percentage
    ;

dimension
    : (Plus | Minus)? Dimension
    ;

any
    : (Ident ws? | number ws? | percentage ws? | dimension ws? | String_ ws? | Uri ws? | Hash ws? | UnicodeRange ws? | Includes ws? | DashMatch ws? | ':' | Function ws? (any | block)* ')' ws?)
    | '(' ws? (any | block)* ')' ws?
    | '[' ws? (any | block)* ']' ws?
    ;

block
    : '{' ws? (declarationList | nestedStatement | any | block | ';' ws?)* '}' ws?
    ;

atRule
    : AtKeyword ws? any* (block | ';' ws?)
    ;

media
    : Media ws? mediaQueryList groupRuleBody ws?
    ;

mediaQueryList
    : (mediaQuery (Comma ws? mediaQuery)*)?
    ;

mediaQuery
    : (MediaOnly | Not)? ws? mediaType ws? (And ws? mediaExpression)*
    | mediaExpression (And ws? mediaExpression)*
    ;

mediaType
    : Ident
    ;

mediaExpression
    : '(' ws? mediaFeature (':' ws? expr)? ')' ws?
    ;

mediaFeature
    : Ident
    ;

page
    : Page ws? pseudoPage? ws? '{' ws? declaration? (';' ws? declaration?)* '}' ws?
    ;

pseudoPage
    : ':' Ident ws?
    ;

fontFaceRule
    : FontFace ws? '{' ws? fontFaceDeclaration? (';' ws? fontFaceDeclaration?)* '}' ws?
    ;

fontFaceDeclaration
    : property ':' ws? expr
    ;

keyframesRule
    : Keyframes ws? Space* Ident ws? '{' ws? keyframeRuleList '}' ws?
    ;

keyframeRuleList
    : keyframeRule*
    ;

keyframeRule
    : keyframeSelector '{' ws? declarationList? '}' ws?
    ;

keyframeSelector
    : (From | To | Percentage) ws? (',' ws? (From | To | Percentage) ws?)*
    ;

viewport
    : Viewport ws? '{' ws? declarationList? '}' ws?
    ;

supportsRule
    : Supports ws? supportsCondition ws? groupRuleBody ws?
    ;

supportsCondition
    : supportsConditionInParens (ws? supportsConjunction ws? supportsConditionInParens)*
    ;

supportsConditionInParens
    : '(' ws? supportsCondition ws? ')'
    | supportsDeclarationCondition
    | generalEnclosed
    ;

supportsConjunction
    : And | Or
    ;

supportsDeclarationCondition
    : '(' ws? declaration ')'
    ;

generalEnclosed
    : Function any* ')'
    | '(' (any | ')')* ')'
    ;

groupRuleBody
    : '{' ws? nestedStatement* '}' ws?
    ;

// Lexer rules
fragment Hex
    : [0-9a-fA-F]
    ;

fragment NewlineOrSpace
    : '\r\n' | [ \t\r\n\f]
    ;

fragment Unicode
    : '\\' Hex Hex? Hex? Hex? Hex? Hex? NewlineOrSpace
    ;

fragment Escape
    : Unicode
    | '\\' ~[\r\n\f0-9a-fA-F]
    ;

fragment Nonascii
    : ~[\u0000-\u007F]
    ;

fragment Nmstart
    : [_a-zA-Z]
    | Nonascii
    | Escape
    ;

fragment Nmchar
    : [_a-zA-Z0-9\-]
    | Nonascii
    | Escape
    ;

fragment Name
    : Nmchar+
    ;

fragment Url
    : ([\u0021\u0023-\u0026\u002A-\u007E] | Nonascii | Escape)*
    ;

fragment Whitespace
    : Space
    | '\t'
    | Newline
    ;

fragment Newline
    : '\n'
    | '\r\n'
    | '\r'
    | '\f'
    ;

fragment ZeroToFourZeros
    : '0'? '0'? '0'? '0'?
    ;

fragment DashChar
    : '-'
    | '\\' ZeroToFourZeros '2d' NewlineOrSpace?
    ;

Ident
    : DashChar? Nmstart Nmchar*
    ;

fragment Dimension_
    : [0-9]+ ('.' [0-9]+)? Ident
    ;

// Lexer tokens
Comment
    : '/*' .*? '*/' -> channel(HIDDEN)
    ;

Space
    : [ \t]+ -> channel(HIDDEN)
    ;

Cdo
    : '<!--'
    ;

Cdc
    : '-->'
    ;

Includes
    : '~='
    ;

DashMatch
    : '|='
    ;

Hash
    : '#' Name
    ;

Important
    : '!' (Space | Comment)* 'important'
    ;

Plus
    : '+'
    ;

Minus
    : '-'
    ;

Number
    : [0-9]+ ('.' [0-9]+)?
    | '.' [0-9]+
    ;

Percentage
    : Number '%'
    ;

Dimension
    : Number Ident
    ;

String_
    : '"' (~["\\\r\n\f] | '\\' Newline | Escape)* '"'
    | '\'' (~['\\\r\n\f] | '\\' Newline | Escape)* '\''
    ;

Uri
    : 'url(' (Space | Comment)* (String_ | Url) (Space | Comment)* ')'
    ;

UnicodeRange
    : 'u+' (Hex | '?')+ ('-' Hex+)?
    ;

MediaOnly
    : 'only'
    ;

Not
    : 'not'
    ;

And
    : 'and'
    ;

Or
    : 'or'
    ;

From
    : 'from'
    ;

To
    : 'to'
    ;

// @rules
AtKeyword
    : '@' Ident
    ;

// Specific @rules
Media      : '@media' ;
Page       : '@page' ;
FontFace   : '@font-face' ;
Keyframes  : '@keyframes' | '@-webkit-keyframes' | '@-moz-keyframes' ;
Viewport   : '@viewport' ;
Supports   : '@supports' ;

// Functions
Calc : 'calc(' ;
Var  : 'var(' ;
Function : Ident '(' ;

// Pseudo
PseudoNot : ':not(' ;

// Variables
Variable
    : '--' Nmchar+
    ;

// Special
Comma : ',' ;

// Skip whitespace
ws
    : (Space | Comment | NewlineToken)*
    ;

NewlineToken
    : Newline -> channel(HIDDEN)
    ;

// Catchall
UnknownAtRule
    : '@' Ident
    ;