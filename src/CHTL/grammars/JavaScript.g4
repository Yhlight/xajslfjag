grammar JavaScript;

// 主程序
program: sourceElements? EOF;

sourceElements: sourceElement+;

sourceElement:
    | statement
    | functionDeclaration
    | variableStatement
    ;

// 语句
statement:
    | block
    | expressionStatement
    | ifStatement
    | iterationStatement
    | returnStatement
    | breakStatement
    | continueStatement
    | tryStatement
    | throwStatement
    | emptyStatement
    ;

// 块语句
block: LBRACE statementList? RBRACE;

statementList: statement+;

// 表达式语句
expressionStatement: expression SEMICOLON?;

// 条件语句
ifStatement: IF LPAREN expression RPAREN statement (ELSE statement)?;

// 循环语句
iterationStatement:
    | FOR LPAREN forInit? SEMICOLON expression? SEMICOLON expression? RPAREN statement
    | FOR LPAREN LEFT_HAND_SIDE_EXPRESSION IN expression RPAREN statement
    | WHILE LPAREN expression RPAREN statement
    | DO statement WHILE LPAREN expression RPAREN SEMICOLON?
    ;

forInit: variableDeclarationList | expression;

// 返回语句
returnStatement: RETURN expression? SEMICOLON?;

// 跳转语句
breakStatement: BREAK IDENTIFIER? SEMICOLON?;
continueStatement: CONTINUE IDENTIFIER? SEMICOLON?;

// 异常处理
tryStatement: TRY block catchClause? finallyClause?;
catchClause: CATCH LPAREN IDENTIFIER RPAREN block;
finallyClause: FINALLY block;
throwStatement: THROW expression SEMICOLON?;

// 空语句
emptyStatement: SEMICOLON;

// 函数声明
functionDeclaration: FUNCTION IDENTIFIER LPAREN formalParameterList? RPAREN LBRACE functionBody RBRACE;

formalParameterList: IDENTIFIER (COMMA IDENTIFIER)*;

functionBody: sourceElements?;

// 变量声明
variableStatement: VAR variableDeclarationList SEMICOLON?;

variableDeclarationList: variableDeclaration (COMMA variableDeclaration)*;

variableDeclaration: IDENTIFIER (ASSIGN assignmentExpression)?;

// 表达式
expression: assignmentExpression (COMMA assignmentExpression)*;

assignmentExpression: 
    | conditionalExpression
    | LEFT_HAND_SIDE_EXPRESSION assignmentOperator assignmentExpression
    ;

assignmentOperator:
    | ASSIGN
    | MULTIPLY_ASSIGN
    | DIVIDE_ASSIGN
    | MODULUS_ASSIGN
    | PLUS_ASSIGN
    | MINUS_ASSIGN
    ;

conditionalExpression: logicalORExpression (QUESTION_MARK assignmentExpression COLON assignmentExpression)?;

logicalORExpression: logicalANDExpression (LOGICAL_OR logicalANDExpression)*;

logicalANDExpression: bitwiseORExpression (LOGICAL_AND bitwiseORExpression)*;

bitwiseORExpression: bitwiseXORExpression (BITWISE_OR bitwiseXORExpression)*;

bitwiseXORExpression: bitwiseANDExpression (BITWISE_XOR bitwiseANDExpression)*;

bitwiseANDExpression: equalityExpression (BITWISE_AND equalityExpression)*;

equalityExpression: relationalExpression ((EQUALS | NOT_EQUALS | STRICT_EQUALS | STRICT_NOT_EQUALS) relationalExpression)*;

relationalExpression: shiftExpression ((LESS_THAN | GREATER_THAN | LESS_EQUALS | GREATER_EQUALS | INSTANCEOF | IN) shiftExpression)*;

shiftExpression: additiveExpression ((LEFT_SHIFT | RIGHT_SHIFT | UNSIGNED_RIGHT_SHIFT) additiveExpression)*;

additiveExpression: multiplicativeExpression ((PLUS | MINUS) multiplicativeExpression)*;

multiplicativeExpression: unaryExpression ((MULTIPLY | DIVIDE | MODULUS) unaryExpression)*;

unaryExpression:
    | postfixExpression
    | DELETE unaryExpression
    | VOID unaryExpression
    | TYPEOF unaryExpression
    | INCREMENT unaryExpression
    | DECREMENT unaryExpression
    | PLUS unaryExpression
    | MINUS unaryExpression
    | BITWISE_NOT unaryExpression
    | LOGICAL_NOT unaryExpression
    ;

postfixExpression: 
    LEFT_HAND_SIDE_EXPRESSION (INCREMENT | DECREMENT)?;

LEFT_HAND_SIDE_EXPRESSION: 
    | callExpression
    | memberExpression
    ;

callExpression: 
    memberExpression arguments (arguments | LBRACKET expression RBRACKET | DOT IDENTIFIER)*;

memberExpression: 
    primaryExpression (LBRACKET expression RBRACKET | DOT IDENTIFIER)*;

arguments: LPAREN argumentList? RPAREN;

argumentList: assignmentExpression (COMMA assignmentExpression)*;

primaryExpression:
    | THIS
    | IDENTIFIER
    | literal
    | arrayLiteral
    | objectLiteral
    | LPAREN expression RPAREN
    ;

arrayLiteral: LBRACKET elementList? RBRACKET;

elementList: assignmentExpression (COMMA assignmentExpression)*;

objectLiteral: LBRACE propertyNameAndValueList? RBRACE;

propertyNameAndValueList: propertyAssignment (COMMA propertyAssignment)*;

propertyAssignment: 
    propertyName COLON assignmentExpression;

propertyName:
    | IDENTIFIER
    | STRING_LITERAL
    | NUMBER_LITERAL
    ;

literal:
    | NULL_LITERAL
    | BOOLEAN_LITERAL
    | NUMBER_LITERAL
    | STRING_LITERAL
    | REGULAR_EXPRESSION_LITERAL
    ;

// 词法规则
// 关键字
BREAK: 'break';
CASE: 'case';
CATCH: 'catch';
CONTINUE: 'continue';
DEFAULT: 'default';
DELETE: 'delete';
DO: 'do';
ELSE: 'else';
FINALLY: 'finally';
FOR: 'for';
FUNCTION: 'function';
IF: 'if';
IN: 'in';
INSTANCEOF: 'instanceof';
NEW: 'new';
RETURN: 'return';
SWITCH: 'switch';
THIS: 'this';
THROW: 'throw';
TRY: 'try';
TYPEOF: 'typeof';
VAR: 'var';
VOID: 'void';
WHILE: 'while';

// 字面量
NULL_LITERAL: 'null';
BOOLEAN_LITERAL: 'true' | 'false';
NUMBER_LITERAL: [0-9]+ ('.' [0-9]+)?;
STRING_LITERAL: '"' (~["\r\n\\] | '\\' .)* '"' | '\'' (~['\r\n\\] | '\\' .)* '\'';
REGULAR_EXPRESSION_LITERAL: '/' (~[/\r\n\\] | '\\' .)+ '/' [gimuy]*;

// 标识符
IDENTIFIER: [a-zA-Z_$][a-zA-Z0-9_$]*;

// 操作符
ASSIGN: '=';
MULTIPLY_ASSIGN: '*=';
DIVIDE_ASSIGN: '/=';
MODULUS_ASSIGN: '%=';
PLUS_ASSIGN: '+=';
MINUS_ASSIGN: '-=';

LOGICAL_OR: '||';
LOGICAL_AND: '&&';
BITWISE_OR: '|';
BITWISE_XOR: '^';
BITWISE_AND: '&';

EQUALS: '==';
NOT_EQUALS: '!=';
STRICT_EQUALS: '===';
STRICT_NOT_EQUALS: '!==';

LESS_THAN: '<';
GREATER_THAN: '>';
LESS_EQUALS: '<=';
GREATER_EQUALS: '>=';

LEFT_SHIFT: '<<';
RIGHT_SHIFT: '>>';
UNSIGNED_RIGHT_SHIFT: '>>>';

PLUS: '+';
MINUS: '-';
MULTIPLY: '*';
DIVIDE: '/';
MODULUS: '%';

INCREMENT: '++';
DECREMENT: '--';
BITWISE_NOT: '~';
LOGICAL_NOT: '!';

QUESTION_MARK: '?';
COLON: ':';
SEMICOLON: ';';
COMMA: ',';
DOT: '.';

LBRACE: '{';
RBRACE: '}';
LPAREN: '(';
RPAREN: ')';
LBRACKET: '[';
RBRACKET: ']';

// 空白字符和注释
WHITESPACE: [ \t\r\n]+ -> skip;
LINE_COMMENT: '//' ~[\r\n]* -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;