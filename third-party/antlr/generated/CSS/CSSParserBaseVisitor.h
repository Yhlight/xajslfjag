
// Generated from CSSParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSSParserVisitor.h"


/**
 * This class provides an empty implementation of CSSParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  CSSParserBaseVisitor : public CSSParserVisitor {
public:

  virtual std::any visitStylesheet(CSSParser::StylesheetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCharset(CSSParser::CharsetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImport_rule(CSSParser::Import_ruleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespace_rule(CSSParser::Namespace_ruleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNested_statement(CSSParser::Nested_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnknown_at_rule(CSSParser::Unknown_at_ruleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMedia_rule(CSSParser::Media_ruleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMedia_query_list(CSSParser::Media_query_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMedia_query(CSSParser::Media_queryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMedia_type(CSSParser::Media_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMedia_expression(CSSParser::Media_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMedia_feature(CSSParser::Media_featureContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPage_rule(CSSParser::Page_ruleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPseudo_page(CSSParser::Pseudo_pageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFont_face_rule(CSSParser::Font_face_ruleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitKeyframes_rule(CSSParser::Keyframes_ruleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitKeyframe_block(CSSParser::Keyframe_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitKeyframe_selector(CSSParser::Keyframe_selectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupports_rule(CSSParser::Supports_ruleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupports_condition(CSSParser::Supports_conditionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupports_condition_in_parens(CSSParser::Supports_condition_in_parensContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupports_negation(CSSParser::Supports_negationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupports_conjunction(CSSParser::Supports_conjunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupports_disjunction(CSSParser::Supports_disjunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupports_declaration_condition(CSSParser::Supports_declaration_conditionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGeneral_enclosed(CSSParser::General_enclosedContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRuleset(CSSParser::RulesetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelector_group(CSSParser::Selector_groupContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelector(CSSParser::SelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCombinator(CSSParser::CombinatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSimple_selector_sequence(CSSParser::Simple_selector_sequenceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_selector(CSSParser::Type_selectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespace_prefix(CSSParser::Namespace_prefixContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElement_name(CSSParser::Element_nameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHash(CSSParser::HashContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass(CSSParser::ClassContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttrib(CSSParser::AttribContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPseudo(CSSParser::PseudoContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctional_pseudo(CSSParser::Functional_pseudoContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNegation(CSSParser::NegationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNegation_arg(CSSParser::Negation_argContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration_list(CSSParser::Declaration_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration(CSSParser::DeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrio(CSSParser::PrioContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty(CSSParser::PropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpr(CSSParser::ExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator(CSSParser::OperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTerm(CSSParser::TermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnary_operator(CSSParser::Unary_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction(CSSParser::FunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAny(CSSParser::AnyContext *ctx) override {
    return visitChildren(ctx);
  }


};

