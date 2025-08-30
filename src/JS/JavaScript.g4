grammar JavaScript;

// Parser Rules
program
    : sourceElements? EOF
    ;

sourceElements
    : sourceElement+
    ;

sourceElement
    : statement
    | functionDeclaration
    | classDeclaration
    ;

statement
    : block
    | variableStatement
    | emptyStatement
    | expressionStatement
    | ifStatement
    | iterationStatement
    | continueStatement
    | breakStatement
    | returnStatement
    | withStatement
    | labelledStatement
    | switchStatement
    | throwStatement
    | tryStatement
    | debuggerStatement
    | importStatement
    | exportStatement
    ;

block
    : '{' statementList? '}'
    ;

statementList
    : statement+
    ;

variableStatement
    : variableDeclarationList eos
    ;

variableDeclarationList
    : varModifier variableDeclaration (',' variableDeclaration)*
    ;

variableDeclaration
    : assignable ('=' singleExpression)?
    ;

varModifier
    : 'var' | 'let' | 'const'
    ;

emptyStatement
    : ';'
    ;

expressionStatement
    : expressionSequence eos
    ;

ifStatement
    : 'if' '(' expressionSequence ')' statement ('else' statement)?
    ;

iterationStatement
    : 'do' statement 'while' '(' expressionSequence ')' eos
    | 'while' '(' expressionSequence ')' statement
    | 'for' '(' (forControl | forInControl) ')' statement
    ;

forControl
    : expressionSequence? ';' expressionSequence? ';' expressionSequence?
    ;

forInControl
    : (varModifier variableDeclaration | singleExpression) ('in' | 'of') expressionSequence
    ;

continueStatement
    : 'continue' identifier? eos
    ;

breakStatement
    : 'break' identifier? eos
    ;

returnStatement
    : 'return' expressionSequence? eos
    ;

withStatement
    : 'with' '(' expressionSequence ')' statement
    ;

switchStatement
    : 'switch' '(' expressionSequence ')' caseBlock
    ;

caseBlock
    : '{' caseClauses? '}'
    ;

caseClauses
    : caseClause+ defaultClause? caseClause*
    ;

caseClause
    : 'case' expressionSequence ':' statementList?
    ;

defaultClause
    : 'default' ':' statementList?
    ;

labelledStatement
    : identifier ':' statement
    ;

throwStatement
    : 'throw' expressionSequence eos
    ;

tryStatement
    : 'try' block (catchClause finallyClause? | finallyClause)
    ;

catchClause
    : 'catch' ('(' assignable ')')? block
    ;

finallyClause
    : 'finally' block
    ;

debuggerStatement
    : 'debugger' eos
    ;

functionDeclaration
    : 'async'? 'function' '*'? identifier '(' formalParameterList? ')' functionBody
    ;

classDeclaration
    : 'class' identifier classHeritage? classTail
    ;

classHeritage
    : 'extends' singleExpression
    ;

classTail
    : '{' classElement* '}'
    ;

classElement
    : (Static | identifier | async)* methodDefinition
    | emptyStatement
    | '#'? propertyName '=' singleExpression
    ;

methodDefinition
    : '*'? propertyName '(' formalParameterList? ')' functionBody
    | '*'? getter '(' ')' functionBody
    | '*'? setter '(' formalParameterList ')' functionBody
    ;

formalParameterList
    : formalParameterArg (',' formalParameterArg)* (',' lastFormalParameterArg)?
    | lastFormalParameterArg
    ;

formalParameterArg
    : assignable ('=' singleExpression)?
    ;

lastFormalParameterArg
    : '...' singleExpression
    ;

functionBody
    : '{' sourceElements? '}'
    ;

sourceElements
    : sourceElement+
    ;

arrayLiteral
    : '[' elementList? ']'
    ;

elementList
    : arrayElement (',' arrayElement)*
    ;

arrayElement
    : '...'? singleExpression
    ;

propertyAssignment
    : propertyName ':' singleExpression
    | '[' singleExpression ']' ':' singleExpression
    | methodDefinition
    | identifier
    ;

propertyName
    : identifierName
    | StringLiteral
    | numericLiteral
    | '[' singleExpression ']'
    ;

arguments
    : '(' argumentList? ')'
    ;

argumentList
    : argument (',' argument)*
    ;

argument
    : '...'? singleExpression
    ;

expressionSequence
    : singleExpression (',' singleExpression)*
    ;

singleExpression
    : anonymousFunction
    | classExpression
    | singleExpression '[' expressionSequence ']'
    | singleExpression '?.' singleExpression
    | singleExpression '.' identifierName
    | singleExpression arguments
    | singleExpression '++'
    | singleExpression '--'
    | 'new' singleExpression arguments?
    | 'delete' singleExpression
    | 'void' singleExpression
    | 'typeof' singleExpression
    | '++' singleExpression
    | '--' singleExpression
    | '+' singleExpression
    | '-' singleExpression
    | '~' singleExpression
    | '!' singleExpression
    | 'await' singleExpression
    | singleExpression ('*' | '/' | '%') singleExpression
    | singleExpression ('+' | '-') singleExpression
    | singleExpression ('<<' | '>>' | '>>>') singleExpression
    | singleExpression ('<' | '>' | '<=' | '>=') singleExpression
    | singleExpression 'instanceof' singleExpression
    | singleExpression 'in' singleExpression
    | singleExpression ('==' | '!=' | '===' | '!==') singleExpression
    | singleExpression '&' singleExpression
    | singleExpression '^' singleExpression
    | singleExpression '|' singleExpression
    | singleExpression '&&' singleExpression
    | singleExpression '||' singleExpression
    | singleExpression '??' singleExpression
    | singleExpression '?' singleExpression ':' singleExpression
    | singleExpression '=' singleExpression
    | singleExpression assignmentOperator singleExpression
    | importExpression
    | templateStringLiteral
    | yieldExpression
    | 'this'
    | identifier
    | 'super'
    | literal
    | arrayLiteral
    | objectLiteral
    | '(' expressionSequence ')'
    | arrowFunctionDeclaration
    ;

assignable
    : identifier
    | arrayLiteral
    | objectLiteral
    ;

objectLiteral
    : '{' (propertyAssignment (',' propertyAssignment)*)? ','? '}'
    ;

anonymousFunction
    : functionDeclaration
    | 'async'? 'function' '*'? '(' formalParameterList? ')' functionBody
    ;

arrowFunctionDeclaration
    : 'async'? arrowFunctionParameters '=>' arrowFunctionBody
    ;

arrowFunctionParameters
    : identifier
    | '(' formalParameterList? ')'
    ;

arrowFunctionBody
    : singleExpression
    | functionBody
    ;

classExpression
    : 'class' identifier? classHeritage? classTail
    ;

assignmentOperator
    : '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '>>>='
    | '&=' | '^=' | '|=' | '**=' | '||=' | '&&=' | '??='
    ;

literal
    : NullLiteral
    | BooleanLiteral
    | StringLiteral
    | templateStringLiteral
    | RegularExpressionLiteral
    | numericLiteral
    | bigintLiteral
    ;

templateStringLiteral
    : '`' templateStringAtom* '`'
    ;

templateStringAtom
    : TemplateStringAtom
    | '${' singleExpression '}'
    ;

numericLiteral
    : DecimalLiteral
    | HexIntegerLiteral
    | OctalIntegerLiteral
    | OctalIntegerLiteral2
    | BinaryIntegerLiteral
    ;

bigintLiteral
    : BigHexIntegerLiteral
    | BigOctalIntegerLiteral
    | BigBinaryIntegerLiteral
    | BigDecimalIntegerLiteral
    ;

getter
    : 'get' propertyName
    ;

setter
    : 'set' propertyName
    ;

identifierName
    : identifier
    | reservedWord
    ;

identifier
    : Identifier
    | NonStrictLet
    | Async
    ;

reservedWord
    : keyword
    | NullLiteral
    | BooleanLiteral
    | 'let'
    | 'static'
    | 'yield'
    | 'await'
    | 'async'
    ;

keyword
    : 'break' | 'case' | 'catch' | 'class' | 'const' | 'continue'
    | 'debugger' | 'default' | 'delete' | 'do' | 'else' | 'export'
    | 'extends' | 'finally' | 'for' | 'function' | 'if' | 'import'
    | 'in' | 'instanceof' | 'new' | 'return' | 'super' | 'switch'
    | 'this' | 'throw' | 'try' | 'typeof' | 'var' | 'void' | 'while'
    | 'with' | 'yield'
    ;

importStatement
    : 'import' (importFromBlock | StringLiteral) eos
    ;

importFromBlock
    : (importDefault (',' importNamespace)? | importNamespace) 'from' StringLiteral
    | importDefault (',' importModuleItems)? 'from' StringLiteral
    | importModuleItems 'from' StringLiteral
    ;

importModuleItems
    : '{' (importAliasName (',' importAliasName)*)? '}'
    ;

importAliasName
    : identifierName ('as' identifierName)?
    ;

importDefault
    : aliasName
    ;

importNamespace
    : '*' ('as' identifierName)?
    ;

importExpression
    : 'import' '(' singleExpression ')'
    ;

exportStatement
    : 'export' ('default' | exportFromBlock | declaration) eos
    ;

exportFromBlock
    : importNamespace 'from' StringLiteral
    | exportModuleItems ('from' StringLiteral)?
    ;

exportModuleItems
    : '{' (exportAliasName (',' exportAliasName)*)? '}'
    ;

exportAliasName
    : identifierName ('as' identifierName)?
    ;

declaration
    : variableStatement
    | classDeclaration
    | functionDeclaration
    ;

yieldExpression
    : 'yield' ('*'? expressionSequence)?
    ;

aliasName
    : identifierName
    ;

async
    : 'async'
    ;

eos
    : ';'
    | EOF
    | {_input.LA(1) == '}'}?
    | {lineTerminatorAhead()}?
    ;

// Lexer Rules
HashBangLine
    : '#!' ~[\r\n]* -> channel(HIDDEN)
    ;

MultiLineComment
    : '/*' .*? '*/' -> channel(HIDDEN)
    ;

SingleLineComment
    : '//' ~[\r\n]* -> channel(HIDDEN)
    ;

RegularExpressionLiteral
    : '/' RegularExpressionChar+ '/' IdentifierPart*
    ;

OpenBracket: '[';
CloseBracket: ']';
OpenParen: '(';
CloseParen: ')';
OpenBrace: '{' {processOpenBrace();};
TemplateCloseBrace: {isTemplateString()}? '}' -> popMode;
CloseBrace: '}' {processCloseBrace();};
SemiColon: ';';
Comma: ',';
Assign: '=';
QuestionMark: '?';
Colon: ':';
Ellipsis: '...';
Dot: '.';
PlusPlus: '++';
MinusMinus: '--';
Plus: '+';
Minus: '-';
BitNot: '~';
Not: '!';
Multiply: '*';
Divide: '/';
Modulus: '%';
Power: '**';
NullCoalesce: '??';
Hashtag: '#';
RightShiftArithmetic: '>>';
LeftShiftArithmetic: '<<';
RightShiftLogical: '>>>';
LessThan: '<';
MoreThan: '>';
LessThanEquals: '<=';
GreaterThanEquals: '>=';
Equals_: '==';
NotEquals: '!=';
IdentityEquals: '===';
IdentityNotEquals: '!==';
BitAnd: '&';
BitXOr: '^';
BitOr: '|';
And: '&&';
Or: '||';
MultiplyAssign: '*=';
DivideAssign: '/=';
ModulusAssign: '%=';
PlusAssign: '+=';
MinusAssign: '-=';
LeftShiftArithmeticAssign: '<<=';
RightShiftArithmeticAssign: '>>=';
RightShiftLogicalAssign: '>>>=';
BitAndAssign: '&=';
BitXorAssign: '^=';
BitOrAssign: '|=';
PowerAssign: '**=';
Arrow: '=>';
OptionalChain: '?.';

NullLiteral: 'null';
BooleanLiteral: 'true' | 'false';
DecimalLiteral: DecimalIntegerLiteral '.' DecimalDigit* ExponentPart?
              | '.' DecimalDigit+ ExponentPart?
              | DecimalIntegerLiteral ExponentPart?
              ;

HexIntegerLiteral: '0' [xX] HexDigit+;
OctalIntegerLiteral: '0' [0-7]+;
OctalIntegerLiteral2: '0' [oO] [0-7]+;
BinaryIntegerLiteral: '0' [bB] [01]+;

BigHexIntegerLiteral: '0' [xX] HexDigit+ 'n';
BigOctalIntegerLiteral: '0' [oO] [0-7]+ 'n';
BigBinaryIntegerLiteral: '0' [bB] [01]+ 'n';
BigDecimalIntegerLiteral: DecimalIntegerLiteral 'n';

// Keywords
Break: 'break';
Do: 'do';
Instanceof: 'instanceof';
Typeof: 'typeof';
Case: 'case';
Else: 'else';
New: 'new';
Var: 'var';
Catch: 'catch';
Finally: 'finally';
Return: 'return';
Void: 'void';
Continue: 'continue';
For: 'for';
Switch: 'switch';
While: 'while';
Debugger: 'debugger';
Function_: 'function';
This: 'this';
With: 'with';
Default: 'default';
If: 'if';
Throw: 'throw';
Delete: 'delete';
In: 'in';
Try: 'try';
As: 'as';
From: 'from';
Class: 'class';
Enum: 'enum';
Extends: 'extends';
Super: 'super';
Const: 'const';
Export: 'export';
Import: 'import';
Async: 'async';
Await: 'await';
Yield: 'yield';
Static: 'static' {!isStrictMode()}? | {isStrictMode()}? 'static' -> type(StrictLet);
Let: 'let' {!isStrictMode()}? | {isStrictMode()}? 'let' -> type(StrictLet);
NonStrictLet: 'let' {!isStrictMode()}?;
StrictLet: 'let' {isStrictMode()}?;

// Identifiers
Identifier: IdentifierStart IdentifierPart*;

// String Literals
StringLiteral
    : '"' DoubleStringCharacter* '"'
    | '\'' SingleStringCharacter* '\''
    ;

BackTick: '`' {processTemplateLiteral();} -> pushMode(TEMPLATE);

WhiteSpaces: [\t\u000B\u000C\u0020\u00A0]+ -> channel(HIDDEN);
LineTerminator: [\r\n\u2028\u2029] -> channel(HIDDEN);
HtmlComment: '<!--' .*? '-->' -> channel(HIDDEN);
CDataComment: '<![CDATA[' .*? ']]>' -> channel(HIDDEN);
UnexpectedCharacter: .;

mode TEMPLATE;
TemplateStringStartExpression: '${' -> pushMode(DEFAULT_MODE);
TemplateStringAtom: ~[`$\\]+ | '\\' . | '$' ~'{';
BackTickInside: '`' {processTemplateLiteral();} -> type(BackTick), popMode;

// Fragments
fragment DoubleStringCharacter
    : ~["\\\r\n]
    | '\\' EscapeSequence
    | LineContinuation
    ;

fragment SingleStringCharacter
    : ~['\\\r\n]
    | '\\' EscapeSequence
    | LineContinuation
    ;

fragment EscapeSequence
    : CharacterEscapeSequence
    | '0'
    | HexEscapeSequence
    | UnicodeEscapeSequence
    | ExtendedUnicodeEscapeSequence
    ;

fragment CharacterEscapeSequence
    : SingleEscapeCharacter
    | NonEscapeCharacter
    ;

fragment SingleEscapeCharacter
    : ['"\\bfnrtv]
    ;

fragment NonEscapeCharacter
    : ~['"\\bfnrtv0-9xu\r\n]
    ;

fragment EscapeCharacter
    : SingleEscapeCharacter
    | DecimalDigit
    | [xu]
    ;

fragment HexEscapeSequence
    : 'x' HexDigit HexDigit
    ;

fragment UnicodeEscapeSequence
    : 'u' HexDigit HexDigit HexDigit HexDigit
    ;

fragment ExtendedUnicodeEscapeSequence
    : 'u{' HexDigit+ '}'
    ;

fragment DecimalIntegerLiteral
    : '0'
    | [1-9] DecimalDigit*
    ;

fragment ExponentPart
    : [eE] [+-]? DecimalDigit+
    ;

fragment IdentifierStart
    : [\p{L}]
    | [$_]
    | '\\' UnicodeEscapeSequence
    ;

fragment IdentifierPart
    : IdentifierStart
    | [\p{Mn}]
    | [\p{Nd}]
    | [\p{Pc}]
    ;

fragment DecimalDigit
    : [0-9]
    ;

fragment HexDigit
    : [0-9a-fA-F]
    ;

fragment LineContinuation
    : '\\' [\r\n\u2028\u2029]
    ;

fragment RegularExpressionChar
    : ~[\r\n/\\[]
    | '\\' ~[\r\n]
    | '[' RegularExpressionClassChar* ']'
    ;

fragment RegularExpressionClassChar
    : ~[\r\n\]\\]
    | '\\' ~[\r\n]
    ;