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

parser grammar JavaScriptParser_fixed;

options {
    tokenVocab = JavaScriptLexer_fixed;
}

program
    : HashBangLine? sourceElements? EOF
    ;

sourceElement
    : statement
    ;

sourceElements
    : sourceElement+
    ;

statement
    : block
    | variableStatement
    | importStatement
    | exportStatement
    | emptyStatement
    | classDeclaration
    | expressionStatement
    | ifStatement
    | iterationStatement
    | continueStatement
    | breakStatement
    | returnStatement
    | yieldStatement
    | withStatement
    | labelledStatement
    | switchStatement
    | throwStatement
    | tryStatement
    | debuggerStatement
    | functionDeclaration
    | asyncFunctionDeclaration
    ;

block
    : '{' statementList? '}'
    ;

statementList
    : statement+
    ;

importStatement
    : Import importFromBlock
    ;

importFromBlock
    : importDefault (Comma importNamespace)? fromClause eos
    | importNamespace fromClause eos
    | importModuleItems fromClause eos
    | StringLiteral eos
    ;

importModuleItems
    : '{' (importAliasName ',')* importAliasName? '}'
    ;

importAliasName
    : moduleExportName (As importedBinding)?
    ;

moduleExportName
    : identifierName
    | StringLiteral
    ;

importedBinding
    : Identifier
    ;

importDefault
    : aliasName
    ;

importNamespace
    : ('*' As)? aliasName
    ;

fromClause
    : From StringLiteral
    ;

aliasName
    : Identifier
    ;

exportStatement
    : Export exportFromBlock
    | Export declaration
    | Export Default singleExpression eos
    ;

exportFromBlock
    : importNamespace fromClause eos
    | exportModuleItems fromClause? eos
    ;

exportModuleItems
    : '{' (exportAliasName ',')* exportAliasName? '}'
    ;

exportAliasName
    : moduleExportName (As moduleExportName)?
    ;

declaration
    : variableStatement
    | classDeclaration
    | functionDeclaration
    | asyncFunctionDeclaration
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

emptyStatement
    : SemiColon
    ;

expressionStatement
    : {this.notOpenBraceAndNotFunction()}? expressionSequence eos
    ;

ifStatement
    : If '(' expressionSequence ')' statement (Else statement)?
    ;

iterationStatement
    : Do statement While '(' expressionSequence ')' eos
    | While '(' expressionSequence ')' statement
    | For '(' (expressionSequence | variableDeclarationList)? ';' expressionSequence? ';' expressionSequence? ')' statement
    | For '(' (singleExpression | variableDeclarationList) In expressionSequence ')' statement
    | For '(' (singleExpression | variableDeclarationList) identifierName expressionSequence ')' statement
    | For Await? '(' (singleExpression | variableDeclarationList) Of expressionSequence ')' statement
    ;

varModifier
    : Var
    | StrictLet
    | Const
    ;

continueStatement
    : Continue ({this.notLineTerminator()}? Identifier)? eos
    ;

breakStatement
    : Break ({this.notLineTerminator()}? Identifier)? eos
    ;

returnStatement
    : Return ({this.notLineTerminator()}? expressionSequence)? eos
    ;

yieldStatement
    : Yield ({this.notLineTerminator()}? expressionSequence)? eos
    ;

withStatement
    : With '(' expressionSequence ')' statement
    ;

switchStatement
    : Switch '(' expressionSequence ')' caseBlock
    ;

caseBlock
    : '{' caseClauses? (defaultClause caseClauses?)? '}'
    ;

caseClauses
    : caseClause+
    ;

caseClause
    : Case expressionSequence ':' statementList?
    ;

defaultClause
    : Default ':' statementList?
    ;

labelledStatement
    : Identifier ':' statement
    ;

throwStatement
    : Throw {this.notLineTerminator()}? expressionSequence eos
    ;

tryStatement
    : Try block (catchProduction finallyProduction? | finallyProduction)
    ;

catchProduction
    : Catch ('(' assignable? ')')? block
    ;

finallyProduction
    : Finally block
    ;

debuggerStatement
    : Debugger eos
    ;

functionDeclaration
    : Async? Function_ '*'? Identifier '(' formalParameterList? ')' '{' functionBody '}'
    ;

asyncFunctionDeclaration
    : Async Function_ '*'? Identifier '(' formalParameterList? ')' '{' functionBody '}'
    ;

classDeclaration
    : Class Identifier classTail
    ;

classTail
    : (Extends singleExpression)? '{' classElement* '}'
    ;

classElement
    : (Static | {this.n("static")}? Identifier | Async)* methodDefinition
    | emptyStatement
    | '#'? propertyName '=' singleExpression eos
    ;

methodDefinition
    : '*'? '#'? propertyName '(' formalParameterList? ')' '{' functionBody '}'
    | '*'? '#'? getter '(' ')' '{' functionBody '}'
    | '*'? '#'? setter '(' formalParameterList? ')' '{' functionBody '}'
    ;

formalParameterList
    : formalParameterArg (',' formalParameterArg)* (',' lastFormalParameterArg)?
    | lastFormalParameterArg
    ;

formalParameterArg
    : assignable ('=' singleExpression)?
    ;

lastFormalParameterArg
    : Ellipsis singleExpression
    ;

functionBody
    : sourceElements?
    ;

arrayLiteral
    : ('[' elementList ']')
    ;

elementList
    : ','* arrayElement? (','+ arrayElement)* ','*
    ;

arrayElement
    : Ellipsis? singleExpression
    ;

propertyAssignment
    : propertyName ':' singleExpression
    | '[' singleExpression ']' ':' singleExpression
    | Async? '*'? propertyName '(' formalParameterList? ')' '{' functionBody '}'
    | getter '(' ')' '{' functionBody '}'
    | setter '(' formalParameterArg ')' '{' functionBody '}'
    | Ellipsis? singleExpression
    ;

propertyName
    : identifierName
    | StringLiteral
    | numericLiteral
    | '[' singleExpression ']'
    ;

arguments
    : '(' (argument (',' argument)* ','?)? ')'
    ;

argument
    : Ellipsis? (singleExpression | Identifier)
    ;

expressionSequence
    : singleExpression (',' singleExpression)*
    ;

singleExpression
    : anonymousFunction
    | arrowFunction
    | asyncArrowFunction
    | classExpression
    | primaryExpression
    | singleExpression '[' expressionSequence ']'
    | singleExpression '?.' '[' expressionSequence ']'
    | singleExpression '.' '#'? identifierName
    | singleExpression '?.' '#'? identifierName
    | singleExpression arguments
    | singleExpression '?.' arguments
    | New singleExpression arguments?
    | New '.' Identifier
    | singleExpression {this.notLineTerminator()}? '++'
    | singleExpression {this.notLineTerminator()}? '--'
    | Delete singleExpression
    | Void singleExpression
    | Typeof singleExpression
    | '++' singleExpression
    | '--' singleExpression
    | '+' singleExpression
    | '-' singleExpression
    | '~' singleExpression
    | '!' singleExpression
    | Await singleExpression
    | '<' singleExpression '>' singleExpression
    | singleExpression ('*' | '/' | '%') singleExpression
    | singleExpression ('+' | '-') singleExpression
    | singleExpression ('<<' | '>>' | '>>>') singleExpression
    | singleExpression ('<' | '>' | '<=' | '>=') singleExpression
    | singleExpression Instanceof singleExpression
    | singleExpression In singleExpression
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
    | Import '(' singleExpression ')'
    | singleExpression TemplateStringLiteral
    ;

assignmentOperator
    : '*='
    | '/='
    | '%='
    | '+='
    | '-='
    | '<<='
    | '>>='
    | '>>>='
    | '&='
    | '^='
    | '|='
    | '**='
    ;

anonymousFunction
    : functionDeclaration
    ;

arrowFunction
    : arrowFunctionParameters '=>' arrowFunctionBody
    ;

asyncArrowFunction
    : Async arrowFunctionParameters '=>' arrowFunctionBody
    ;

arrowFunctionParameters
    : Identifier
    | '(' formalParameterList? ')'
    ;

arrowFunctionBody
    : singleExpression
    | '{' functionBody '}'
    ;

assignable
    : Identifier
    | arrayLiteral
    | objectLiteral
    ;

objectLiteral
    : '{' (propertyAssignment (',' propertyAssignment)* ','?)? '}'
    ;

primaryExpression
    : This
    | Super
    | Identifier
    | literal
    | arrayLiteral
    | objectLiteral
    | '(' expressionSequence ')'
    ;

literal
    : NullLiteral
    | BooleanLiteral
    | StringLiteral
    | TemplateStringLiteral
    | RegularExpressionLiteral
    | numericLiteral
    ;

numericLiteral
    : DecimalLiteral
    | HexIntegerLiteral
    | OctalIntegerLiteral
    | OctalIntegerLiteral2
    | BinaryIntegerLiteral
    | BigHexIntegerLiteral
    | BigOctalIntegerLiteral
    | BigBinaryIntegerLiteral
    | BigDecimalIntegerLiteral
    ;

identifierName
    : Identifier
    | reservedWord
    ;

reservedWord
    : keyword
    | futureReservedWord
    | NullLiteral
    | BooleanLiteral
    ;

keyword
    : Break
    | Do
    | Instanceof
    | Typeof
    | Case
    | Else
    | New
    | Var
    | Catch
    | Finally
    | Return
    | Void
    | Continue
    | For
    | Switch
    | While
    | Debugger
    | Function_
    | This
    | With
    | Default
    | If
    | Throw
    | Delete
    | In
    | Try
    | Class
    | Enum
    | Extends
    | Super
    | Const
    | Export
    | Import
    | Async
    | Await
    | From
    | As
    | Of
    ;

futureReservedWord
    : Implements
    | StrictLet
    | NonStrictLet
    | Private
    | Public
    | Interface
    | Package
    | Protected
    | Static
    | Yield
    ;

getter
    : {this.p("get")}? Identifier
    ;

setter
    : {this.p("set")}? Identifier
    ;

classExpression
    : Class Identifier? classTail
    ;

eos
    : SemiColon
    | EOF
    | {this.lineTerminatorAhead()}?
    | {this.closeBrace()}?
    ;