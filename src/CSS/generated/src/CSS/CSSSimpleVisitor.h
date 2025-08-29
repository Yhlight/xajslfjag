
// Generated from src/CSS/CSSSimple.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSSSimpleParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by CSSSimpleParser.
 */
class  CSSSimpleVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by CSSSimpleParser.
   */
    virtual std::any visitStylesheet(CSSSimpleParser::StylesheetContext *context) = 0;

    virtual std::any visitStatement(CSSSimpleParser::StatementContext *context) = 0;

    virtual std::any visitRuleset(CSSSimpleParser::RulesetContext *context) = 0;

    virtual std::any visitSelector(CSSSimpleParser::SelectorContext *context) = 0;

    virtual std::any visitSimpleSelector(CSSSimpleParser::SimpleSelectorContext *context) = 0;

    virtual std::any visitDeclaration(CSSSimpleParser::DeclarationContext *context) = 0;

    virtual std::any visitProperty(CSSSimpleParser::PropertyContext *context) = 0;

    virtual std::any visitValue(CSSSimpleParser::ValueContext *context) = 0;

    virtual std::any visitValueList(CSSSimpleParser::ValueListContext *context) = 0;

    virtual std::any visitAtRule(CSSSimpleParser::AtRuleContext *context) = 0;

    virtual std::any visitAny(CSSSimpleParser::AnyContext *context) = 0;


};

