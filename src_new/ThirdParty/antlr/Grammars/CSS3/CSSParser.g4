parser grammar CSSParser;

options {
    tokenVocab = CSSLexer;
}

stylesheet
    : (charset | import_rule | namespace_rule | nested_statement)* EOF
    ;

charset
    : AT_CHARSET STRING SEMICOLON
    ;

import_rule
    : AT_IMPORT (STRING | URI) media_query_list? SEMICOLON
    ;

namespace_rule
    : AT_NAMESPACE IDENT? (STRING | URI) SEMICOLON
    ;

nested_statement
    : ruleset
    | media_rule
    | page_rule
    | font_face_rule
    | keyframes_rule
    | supports_rule
    | unknown_at_rule
    ;

unknown_at_rule
    : AT IDENT any* (LBRACE any* RBRACE | SEMICOLON)
    ;

media_rule
    : AT_MEDIA media_query_list LBRACE nested_statement* RBRACE
    ;

media_query_list
    : media_query (COMMA media_query)*
    ;

media_query
    : (ONLY_KEYWORD | NOT_KEYWORD)? media_type (AND_KEYWORD media_expression)*
    | media_expression (AND_KEYWORD media_expression)*
    ;

media_type
    : IDENT
    ;

media_expression
    : LPAREN media_feature (COLON expr)? RPAREN
    ;

media_feature
    : IDENT
    ;

page_rule
    : AT_PAGE pseudo_page? LBRACE declaration_list RBRACE
    ;

pseudo_page
    : COLON IDENT
    ;

font_face_rule
    : AT_FONT_FACE LBRACE declaration_list RBRACE
    ;

keyframes_rule
    : AT_KEYFRAMES IDENT LBRACE keyframe_block* RBRACE
    ;

keyframe_block
    : keyframe_selector LBRACE declaration_list RBRACE
    ;

keyframe_selector
    : (PERCENTAGE | FROM_KEYWORD | TO_KEYWORD) (COMMA (PERCENTAGE | FROM_KEYWORD | TO_KEYWORD))*
    ;

supports_rule
    : AT_SUPPORTS supports_condition LBRACE nested_statement* RBRACE
    ;

supports_condition
    : supports_negation
    | supports_conjunction
    | supports_disjunction
    | supports_condition_in_parens
    ;

supports_condition_in_parens
    : LPAREN supports_condition RPAREN
    | supports_declaration_condition
    | general_enclosed
    ;

supports_negation
    : NOT_KEYWORD supports_condition_in_parens
    ;

supports_conjunction
    : supports_condition_in_parens (AND_KEYWORD supports_condition_in_parens)+
    ;

supports_disjunction
    : supports_condition_in_parens (OR_KEYWORD supports_condition_in_parens)+
    ;

supports_declaration_condition
    : LPAREN declaration RPAREN
    ;

general_enclosed
    : (FUNCTION | LPAREN) any* RPAREN
    ;

ruleset
    : selector_group LBRACE declaration_list RBRACE
    ;

selector_group
    : selector (COMMA selector)*
    ;

selector
    : simple_selector_sequence (combinator simple_selector_sequence)*
    ;

combinator
    : PLUS
    | MINUS
    | MULTIPLY
    | TILDE
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
    : HASH IDENT
    ;

class
    : DOT IDENT
    ;

attrib
    : LBRACKET IDENT ((PREFIXMATCH | SUFFIXMATCH | SUBSTRINGMATCH | EQUALS | INCLUDES | DASHMATCH) (IDENT | STRING))? RBRACKET
    ;

pseudo
    : COLON COLON? (IDENT | functional_pseudo)
    ;

functional_pseudo
    : FUNCTION expr RPAREN
    ;

negation
    : NOT_FUNCTION negation_arg RPAREN
    ;

negation_arg
    : type_selector
    | hash
    | class
    | attrib
    | pseudo
    ;

declaration_list
    : declaration? (SEMICOLON declaration?)*
    ;

declaration
    : property COLON expr prio?
    ;

prio
    : IMPORTANT
    ;

property
    : IDENT
    ;

expr
    : term (operator term)*
    ;

operator
    : DIVIDE
    | COMMA
    | /* empty for space */
    ;

term
    : unary_operator?
    ( NUMBER
    | PERCENTAGE
    | DIMENSION
    | STRING
    | IDENT
    | URI
    | HASH IDENT?
    | function
    )
    ;

unary_operator
    : MINUS
    | PLUS
    ;

function
    : FUNCTION expr RPAREN
    ;

any
    : IDENT
    | NUMBER
    | PERCENTAGE
    | DIMENSION
    | STRING
    | URI
    | HASH
    | PLUS
    | MINUS
    | MULTIPLY
    | DIVIDE
    | EQUALS
    | COLON
    | SEMICOLON
    | COMMA
    | DOT
    | function
    | LPAREN any* RPAREN
    | LBRACKET any* RBRACKET
    | LBRACE any* RBRACE
    | ANYCHAR
    ;