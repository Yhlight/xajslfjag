parser grammar css3ParserFixed;

options {
    tokenVocab = css3LexerFixed;
}

stylesheet
    : ws* (charset ws*)? (imports ws*)* namespace_* (nestedStatement | ruleset)* ws* EOF
    ;

charset
    : Charset String_ Semicolon ws*
    ;

imports
    : Import ws* (String_ | Uri) ws* mediaQueryList? Semicolon ws*
    ;

namespace_
    : Namespace ws* (namespacePrefix ws*)? (String_ | Uri) ws* Semicolon ws*
    ;

namespacePrefix
    : Ident
    ;

media
    : Media ws* mediaQueryList ws* LeftBrace ws* ruleset* RightBrace ws*
    ;

mediaQueryList
    : (mediaQuery ws* (Comma ws* mediaQuery)*)?
    ;

mediaQuery
    : (PipeonlyPipe | PipenotPipe)? ws* mediaType ws* (PipeandPipe ws* mediaExpression)*
    | mediaExpression (ws* PipeandPipe ws* mediaExpression)*
    ;

mediaType
    : Ident
    ;

mediaExpression
    : LeftParen ws* mediaFeature ws* (Colon ws* expr)? RightParen ws*
    ;

mediaFeature
    : Ident
    ;

page
    : Page ws* pseudoPage? LeftBrace ws* declaration? (Semicolon ws* declaration)* RightBrace ws*
    ;

pseudoPage
    : Colon Ident ws*
    ;

selectorGroup
    : selector (ws* Comma ws* selector)*
    ;

selector
    : simpleSelectorSequence ws* (combinator simpleSelectorSequence ws*)*
    ;

combinator
    : Pipe+Pipe ws*
    | Pipe>Pipe ws*
    | Pipe~Pipe ws*
    | Space ws*
    ;

simpleSelectorSequence
    : (typeSelector | universal) (Hash | className | attrib | pseudo | negation)*
    | (Hash | className | attrib | pseudo | negation)+
    ;

typeSelector
    : typeNamespacePrefix? elementName
    ;

typeNamespacePrefix
    : (Ident | Star)? Pipe|Pipe
    ;

elementName
    : Ident
    ;

universal
    : typeNamespacePrefix? Star
    ;

className
    : Dot Ident
    ;

attrib
    : LeftBracket ws* typeNamespacePrefix? Ident ws* ((Includes | DashMatch | Equals | Pipe~=Pipe | Pipe|=Pipe | Pipe^=Pipe | Pipe$=Pipe | Pipe*=Pipe) ws* (Ident | String_) ws*)? RightBracket
    ;

pseudo
    : Colon Colon? (Ident | functionalPseudo)
    ;

functionalPseudo
    : Function_ ws* expression RightParen
    ;

expression
    : (Pipe+Pipe ws* | Minus ws*)? term (ws* (Pipe+Pipe | Minus | Dimension | Number | String_ | Ident) ws* term)*
    ;

negation
    : PseudoNot ws* negationArg ws* RightParen
    ;

negationArg
    : typeSelector
    | universal
    | Hash
    | className
    | attrib
    | pseudo
    ;

operator_
    : Slash ws*
    | Comma ws*
    | Space ws*
    | Equals ws*
    ;

property_
    : Ident ws*
    ;

ruleset
    : selectorGroup ws* LeftBrace ws* declarationList? RightBrace ws*
    ;

declarationList
    : (Semicolon ws*)* declaration? (Semicolon ws* declaration?)* (Semicolon ws*)*
    ;

declaration
    : property_ Colon ws* expr prio?
    ;

prio
    : Important ws*
    ;

expr
    : term (operator_? term)*
    ;

term
    : Number ws*
    | Percentage ws*
    | Length ws*
    | Ident ws*
    | Uri ws*
    | Hash ws*
    | Function_ ws* (expr | operator_)* RightParen ws*
    | String_ ws*
    | Dimension ws*
    | UnknownDimension ws*
    ;

hexcolor
    : Hash ws*
    ;

nestedStatement
    : ruleset
    | media
    | page
    ;

ws
    : (Comment | Space)*
    ;