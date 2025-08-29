
// Generated from src/CSS/CSSSimple.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSSSimpleVisitor.h"


/**
 * This class provides an empty implementation of CSSSimpleVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  CSSSimpleBaseVisitor : public CSSSimpleVisitor {
public:

  virtual std::any visitStylesheet(CSSSimpleParser::StylesheetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(CSSSimpleParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRuleset(CSSSimpleParser::RulesetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelector(CSSSimpleParser::SelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSimpleSelector(CSSSimpleParser::SimpleSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration(CSSSimpleParser::DeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty(CSSSimpleParser::PropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValue(CSSSimpleParser::ValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValueList(CSSSimpleParser::ValueListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtRule(CSSSimpleParser::AtRuleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAny(CSSSimpleParser::AnyContext *ctx) override {
    return visitChildren(ctx);
  }


};

