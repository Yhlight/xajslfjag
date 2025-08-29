parser grammar SimpleJSParser;

options {
    tokenVocab = SimpleJSLexer;
}

program
    : statement* EOF
    ;

statement
    : block
    | varDeclaration
    | functionDeclaration
    | classDeclaration
    | expressionStatement
    | ifStatement
    | whileStatement
    | forStatement
    | returnStatement
    | throwStatement
    | tryStatement
    | switchStatement
    | breakStatement
    | continueStatement
    | emptyStatement
    ;

block
    : LBRACE statement* RBRACE
    ;

varDeclaration
    : (VAR | LET | CONST) varDeclarator (COMMA varDeclarator)* SEMICOLON?
    ;

varDeclarator
    : IDENTIFIER (ASSIGN expression)?
    ;

functionDeclaration
    : ASYNC? FUNCTION IDENTIFIER LPAREN parameterList? RPAREN block
    ;

classDeclaration
    : CLASS IDENTIFIER (EXTENDS IDENTIFIER)? LBRACE classElement* RBRACE
    ;

classElement
    : methodDefinition
    | propertyDefinition
    ;

methodDefinition
    : IDENTIFIER LPAREN parameterList? RPAREN block
    ;

propertyDefinition
    : IDENTIFIER (ASSIGN expression)? SEMICOLON?
    ;

parameterList
    : IDENTIFIER (COMMA IDENTIFIER)*
    ;

expressionStatement
    : expression SEMICOLON?
    ;

ifStatement
    : IF LPAREN expression RPAREN statement (ELSE statement)?
    ;

whileStatement
    : WHILE LPAREN expression RPAREN statement
    ;

forStatement
    : FOR LPAREN (varDeclaration | expression)? SEMICOLON expression? SEMICOLON expression? RPAREN statement
    | FOR LPAREN (varDeclaration | expression) (IN | OF) expression RPAREN statement
    ;

returnStatement
    : RETURN expression? SEMICOLON?
    ;

throwStatement
    : THROW expression SEMICOLON?
    ;

tryStatement
    : TRY block (CATCH LPAREN IDENTIFIER? RPAREN block)? (FINALLY block)?
    ;

switchStatement
    : SWITCH LPAREN expression RPAREN LBRACE caseClause* defaultClause? caseClause* RBRACE
    ;

caseClause
    : CASE expression COLON statement*
    ;

defaultClause
    : DEFAULT COLON statement*
    ;

breakStatement
    : BREAK SEMICOLON?
    ;

continueStatement
    : CONTINUE SEMICOLON?
    ;

emptyStatement
    : SEMICOLON
    ;

expression
    : assignmentExpression
    ;

assignmentExpression
    : conditionalExpression
    | leftHandSideExpression (ASSIGN | PLUS_ASSIGN | MINUS_ASSIGN | MULT_ASSIGN | DIV_ASSIGN | MOD_ASSIGN) assignmentExpression
    ;

conditionalExpression
    : logicalOrExpression (QUESTION expression COLON expression)?
    ;

logicalOrExpression
    : logicalAndExpression (OR logicalAndExpression)*
    ;

logicalAndExpression
    : equalityExpression (AND equalityExpression)*
    ;

equalityExpression
    : relationalExpression ((EQ | NE | EQ_STRICT | NE_STRICT) relationalExpression)*
    ;

relationalExpression
    : additiveExpression ((LT | LE | GT | GE | INSTANCEOF | IN) additiveExpression)*
    ;

additiveExpression
    : multiplicativeExpression ((PLUS | MINUS) multiplicativeExpression)*
    ;

multiplicativeExpression
    : unaryExpression ((MULTIPLY | DIVIDE | MODULUS) unaryExpression)*
    ;

unaryExpression
    : postfixExpression
    | (PLUS_PLUS | MINUS_MINUS) unaryExpression
    | (PLUS | MINUS | NOT | BIT_NOT | TYPEOF | VOID | DELETE) unaryExpression
    | AWAIT unaryExpression
    ;

postfixExpression
    : leftHandSideExpression (PLUS_PLUS | MINUS_MINUS)?
    ;

leftHandSideExpression
    : callExpression
    | memberExpression
    ;

callExpression
    : memberExpression arguments
    | callExpression arguments
    | callExpression LBRACKET expression RBRACKET
    | callExpression DOT IDENTIFIER
    ;

memberExpression
    : primaryExpression
    | memberExpression LBRACKET expression RBRACKET
    | memberExpression DOT IDENTIFIER
    | NEW memberExpression arguments?
    ;

primaryExpression
    : THIS
    | SUPER
    | IDENTIFIER
    | literal
    | arrayLiteral
    | objectLiteral
    | LPAREN expression RPAREN
    | functionExpression
    | arrowFunction
    ;

functionExpression
    : ASYNC? FUNCTION IDENTIFIER? LPAREN parameterList? RPAREN block
    ;

arrowFunction
    : (IDENTIFIER | LPAREN parameterList? RPAREN) ARROW (expression | block)
    ;

arguments
    : LPAREN (expression (COMMA expression)*)? RPAREN
    ;

arrayLiteral
    : LBRACKET (expression (COMMA expression)*)? RBRACKET
    ;

objectLiteral
    : LBRACE (propertyAssignment (COMMA propertyAssignment)*)? RBRACE
    ;

propertyAssignment
    : IDENTIFIER COLON expression
    | STRING COLON expression
    | NUMBER COLON expression
    | IDENTIFIER LPAREN parameterList? RPAREN block
    ;

literal
    : NULL
    | TRUE
    | FALSE
    | UNDEFINED
    | NUMBER
    | STRING
    | TEMPLATE
    | REGEX
    ;