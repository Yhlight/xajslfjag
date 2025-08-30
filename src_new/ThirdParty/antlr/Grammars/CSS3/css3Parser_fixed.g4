// $antlr-format alignTrailingComments true, columnLimit 150, maxEmptyLinesToKeep 1, reflowComments false, useTab false
// $antlr-format allowShortRulesOnASingleLine true, allowShortBlocksOnASingleLine true, minEmptyLines 0, alignSemicolons ownLine
// $antlr-format alignColons trailing, singleLineOverrulesHangingColon true, alignLexerCommands true, alignLabels true, alignTrailers true

parser grammar css3Parser_fixed;

options {
    tokenVocab = css3Lexer_fixed;
}

stylesheet
    : (CDO | CDC | ws | statement)*
    ;

statement
    : ruleset
    | at_rule
    ;

at_rule
    : AT_KEYWORD ws* any* (block | SEMICOLON ws*)
    ;

block
    : LBRACE ws* (any | block | ATKEYWORD ws* | SEMICOLON ws*)* RBRACE ws*
    ;

ruleset
    : selector_group LBRACE ws* declaration_list? RBRACE ws*
    ;

selector_group
    : selector (COMMA ws* selector)*
    ;

selector
    : simple_selector_sequence (combinator simple_selector_sequence)*
    ;

combinator
    : PLUS ws*
    | MINUS ws*
    | MULTIPLY ws*
    | ws+
    ;

simple_selector_sequence
    : type_selector (hash | class | attrib | pseudo | negation)*
    | (hash | class | attrib | pseudo | negation)+
    ;

type_selector
    : namespace_prefix? element_name
    | namespace_prefix? MULTIPLY
    ;

namespace_prefix
    : (IDENT | MULTIPLY)? PIPE
    ;

element_name
    : IDENT
    ;

hash
    : HASH
    ;

class
    : DOT IDENT
    ;

attrib
    : LBRACKET ws* IDENT ws* ((PREFIXMATCH | SUFFIXMATCH | SUBSTRINGMATCH | EQUAL | INCLUDES | DASHMATCH) ws* (IDENT | STRING) ws*)? RBRACKET
    ;

pseudo
    : COLON COLON? (IDENT | functional_pseudo)
    ;

functional_pseudo
    : FUNCTION ws* expression RPAREN
    ;

negation
    : NOT ws* negation_arg ws* RPAREN
    ;

negation_arg
    : type_selector
    | hash
    | class
    | attrib
    | pseudo
    ;

declaration_list
    : declaration (SEMICOLON ws* declaration)* SEMICOLON? ws*
    ;

declaration
    : property COLON ws* expression prio?
    ;

prio
    : IMPORTANT_SYM ws*
    ;

property
    : IDENT ws*
    ;

expression
    : term (operator term)*
    ;

operator
    : DIV ws*
    | COMMA ws*
    | ws+
    ;

term
    : unary_operator?
    ( NUMBER ws*
    | PERCENTAGE ws*
    | DIMENSION ws*
    | STRING ws*
    | IDENT ws*
    | URI ws*
    | HASH ws*
    | UnicodeRange ws*
    | function
    )
    ;

unary_operator
    : MINUS
    | PLUS
    ;

function
    : FUNCTION ws* expression RPAREN ws*
    ;

any
    : IDENT ws*
    | NUMBER ws*
    | PERCENTAGE ws*
    | DIMENSION ws*
    | STRING ws*
    | DELIM ws*
    | URI ws*
    | HASH ws*
    | UnicodeRange ws*
    | INCLUDES ws*
    | DASHMATCH ws*
    | COLON ws*
    | function
    | LPAREN ws* any* RPAREN ws*
    | LBRACKET ws* any* RBRACKET ws*
    ;

ws
    : Space
    ;

// Additional tokens for compatibility
DIV: DIVIDE;
DELIM: .;