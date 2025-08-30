/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 by Bart Kiers (original author) and Alexandre Vitorelli (contributor -> ported to CSharp)
 * Copyright (c) 2017-2020 by Ivan Kochurkin (Positive Technologies):
    added ECMAScript 6 support, cleared and transformed to the universal grammar.
 * Copyright (c) 2018 by Juan Alvarez (contributor -> ported to Go)
 * Copyright (c) 2019 by Student Main (contributor -> ES2020)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

lexer grammar JavaScriptLexer_fixed;

channels {
    ERROR
}

// Remove superClass option for C++ target
// options {
//     superClass = JavaScriptLexerBase;
// }

HashBangLine      : '#!' ~[\r\n\u2028\u2029]*;
MultiLineComment  : '/*' .*? '*/'             -> channel(HIDDEN);
SingleLineComment : '//' ~[\r\n\u2028\u2029]* -> channel(HIDDEN);
RegularExpressionLiteral: '/' RegularExpressionFirstChar RegularExpressionChar* '/' [gimsuyx]*;

OpenBracket                : '[';
CloseBracket               : ']';
OpenParen                  : '(';
CloseParen                 : ')';
OpenBrace                  : '{';
CloseBrace                 : '}';
SemiColon                  : ';';
Comma                      : ',';
Assign                     : '=';
QuestionMark               : '?';
Colon                      : ':';
Ellipsis                   : '...';
Dot                        : '.';
PlusPlus                   : '++';
MinusMinus                 : '--';
Plus                       : '+';
Minus                      : '-';
BitNot                     : '~';
Not                        : '!';
Multiply                   : '*';
Divide                     : '/';
Modulus                    : '%';
RightShiftArithmetic       : '>>';
LeftShiftArithmetic        : '<<';
RightShiftLogical          : '>>>';
LessThan                   : '<';
MoreThan                   : '>';
LessThanEquals             : '<=';
GreaterThanEquals          : '>=';
Equals_                    : '==';
NotEquals                  : '!=';
IdentityEquals             : '===';
IdentityNotEquals          : '!==';
BitAnd                     : '&';
BitXOr                     : '^';
BitOr                      : '|';
And                        : '&&';
Or                         : '||';
MultiplyAssign             : '*=';
DivideAssign               : '/=';
ModulusAssign              : '%=';
PlusAssign                 : '+=';
MinusAssign                : '-=';
LeftShiftArithmeticAssign  : '<<=';
RightShiftArithmeticAssign : '>>=';
RightShiftLogicalAssign    : '>>>=';
BitAndAssign               : '&=';
BitXorAssign               : '^=';
BitOrAssign                : '|=';
PowerAssign                : '**=';
ARROW                      : '=>';

/// Null Literals
NullLiteral                : 'null';

/// Boolean Literals
BooleanLiteral             : 'true'
                           | 'false';

/// Numeric Literals
DecimalLiteral             : DecimalIntegerLiteral '.' [0-9]* ExponentPart?
                           | '.' [0-9]+ ExponentPart?
                           | DecimalIntegerLiteral ExponentPart?;

/// Numeric Literals
HexIntegerLiteral          : '0' [xX] [0-9a-fA-F]+;
OctalIntegerLiteral        : '0' [0-7]+;
OctalIntegerLiteral2       : '0' [oO] [0-7]+;
BinaryIntegerLiteral       : '0' [bB] [01]+;
BigHexIntegerLiteral       : '0' [xX] [0-9a-fA-F]+ 'n';
BigOctalIntegerLiteral     : '0' [oO] [0-7]+ 'n';
BigBinaryIntegerLiteral    : '0' [bB] [01]+ 'n';
BigDecimalIntegerLiteral   : DecimalIntegerLiteral 'n';

/// Keywords
Break                      : 'break';
Do                         : 'do';
Instanceof                 : 'instanceof';
Typeof                     : 'typeof';
Case                       : 'case';
Else                       : 'else';
New                        : 'new';
Var                        : 'var';
Catch                      : 'catch';
Finally                    : 'finally';
Return                     : 'return';
Void                       : 'void';
Continue                   : 'continue';
For                        : 'for';
Switch                     : 'switch';
While                      : 'while';
Debugger                   : 'debugger';
Function_                  : 'function';
This                       : 'this';
With                       : 'with';
Default                    : 'default';
If                         : 'if';
Throw                      : 'throw';
Delete                     : 'delete';
In                         : 'in';
Try                        : 'try';
As                         : 'as';
From                       : 'from';
Of                         : 'of';
Class                      : 'class';
Enum                       : 'enum';
Extends                    : 'extends';
Super                      : 'super';
Const                      : 'const';
Export                     : 'export';
Import                     : 'import';
Async                      : 'async';
Await                      : 'await';
Implements                 : 'implements';
StrictLet                  : 'let';
NonStrictLet               : 'let';
Private                    : 'private';
Public                     : 'public';
Interface                  : 'interface';
Package                    : 'package';
Protected                  : 'protected';
Static                     : 'static';
Yield                      : 'yield';

/// String Literals
StringLiteral              : '"' DoubleStringCharacter* '"'
                           | '\'' SingleStringCharacter* '\'';

TemplateStringLiteral      : '`' ('\\`' | ~'`')* '`';

/// Identifiers
Identifier                 : IdentifierStart IdentifierPart*;

/// Whitespace
WhiteSpaces                : [\t\u000B\u000C\u0020\u00A0]+ -> channel(HIDDEN);
LineTerminator             : [\r\n\u2028\u2029] -> channel(HIDDEN);

/// Comments
HtmlComment                : '<!--' .*? '-->' -> channel(HIDDEN);
CDataComment               : '<![CDATA[' .*? ']]>' -> channel(HIDDEN);

UnexpectedCharacter        : . -> channel(ERROR);

// Fragment rules
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

fragment HexEscapeSequence
    : 'x' [0-9a-fA-F] [0-9a-fA-F]
    ;

fragment UnicodeEscapeSequence
    : 'u' [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F] [0-9a-fA-F]
    ;

fragment ExtendedUnicodeEscapeSequence
    : 'u{' [0-9a-fA-F]+ '}'
    ;

fragment SingleEscapeCharacter
    : ['"\\bfnrtv]
    ;

fragment NonEscapeCharacter
    : ~['"\\bfnrtv0-9xu\r\n]
    ;

fragment EscapeCharacter
    : SingleEscapeCharacter
    | [0-9]
    | [xu]
    ;

fragment LineContinuation
    : '\\' [\r\n\u2028\u2029]
    ;

fragment DecimalIntegerLiteral
    : '0'
    | [1-9] [0-9]*
    ;

fragment ExponentPart
    : [eE] [+-]? [0-9]+
    ;

fragment IdentifierStart
    : [a-zA-Z]
    | '$'
    | '_'
    | '\\' UnicodeEscapeSequence
    ;

fragment IdentifierPart
    : IdentifierStart
    | [0-9]
    ;

fragment RegularExpressionFirstChar
    : ~[*\r\n\u2028\u2029\\/[]
    | RegularExpressionBackslashSequence
    | RegularExpressionClass
    ;

fragment RegularExpressionChar
    : ~[\r\n\u2028\u2029\\/[]
    | RegularExpressionBackslashSequence
    | RegularExpressionClass
    ;

fragment RegularExpressionClass
    : '[' RegularExpressionClassChar* ']'
    ;

fragment RegularExpressionClassChar
    : ~[\r\n\u2028\u2029\\\]]
    | RegularExpressionBackslashSequence
    ;

fragment RegularExpressionBackslashSequence
    : '\\' ~[\r\n\u2028\u2029]
    ;