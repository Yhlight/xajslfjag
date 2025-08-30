
// Generated from third-party/antlr/Grammars/CSS3/CSSParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSSParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by CSSParser.
 */
class  CSSParserListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterStylesheet(CSSParser::StylesheetContext *ctx) = 0;
  virtual void exitStylesheet(CSSParser::StylesheetContext *ctx) = 0;

  virtual void enterCharset(CSSParser::CharsetContext *ctx) = 0;
  virtual void exitCharset(CSSParser::CharsetContext *ctx) = 0;

  virtual void enterImport_rule(CSSParser::Import_ruleContext *ctx) = 0;
  virtual void exitImport_rule(CSSParser::Import_ruleContext *ctx) = 0;

  virtual void enterNamespace_rule(CSSParser::Namespace_ruleContext *ctx) = 0;
  virtual void exitNamespace_rule(CSSParser::Namespace_ruleContext *ctx) = 0;

  virtual void enterNested_statement(CSSParser::Nested_statementContext *ctx) = 0;
  virtual void exitNested_statement(CSSParser::Nested_statementContext *ctx) = 0;

  virtual void enterUnknown_at_rule(CSSParser::Unknown_at_ruleContext *ctx) = 0;
  virtual void exitUnknown_at_rule(CSSParser::Unknown_at_ruleContext *ctx) = 0;

  virtual void enterMedia_rule(CSSParser::Media_ruleContext *ctx) = 0;
  virtual void exitMedia_rule(CSSParser::Media_ruleContext *ctx) = 0;

  virtual void enterMedia_query_list(CSSParser::Media_query_listContext *ctx) = 0;
  virtual void exitMedia_query_list(CSSParser::Media_query_listContext *ctx) = 0;

  virtual void enterMedia_query(CSSParser::Media_queryContext *ctx) = 0;
  virtual void exitMedia_query(CSSParser::Media_queryContext *ctx) = 0;

  virtual void enterMedia_type(CSSParser::Media_typeContext *ctx) = 0;
  virtual void exitMedia_type(CSSParser::Media_typeContext *ctx) = 0;

  virtual void enterMedia_expression(CSSParser::Media_expressionContext *ctx) = 0;
  virtual void exitMedia_expression(CSSParser::Media_expressionContext *ctx) = 0;

  virtual void enterMedia_feature(CSSParser::Media_featureContext *ctx) = 0;
  virtual void exitMedia_feature(CSSParser::Media_featureContext *ctx) = 0;

  virtual void enterPage_rule(CSSParser::Page_ruleContext *ctx) = 0;
  virtual void exitPage_rule(CSSParser::Page_ruleContext *ctx) = 0;

  virtual void enterPseudo_page(CSSParser::Pseudo_pageContext *ctx) = 0;
  virtual void exitPseudo_page(CSSParser::Pseudo_pageContext *ctx) = 0;

  virtual void enterFont_face_rule(CSSParser::Font_face_ruleContext *ctx) = 0;
  virtual void exitFont_face_rule(CSSParser::Font_face_ruleContext *ctx) = 0;

  virtual void enterKeyframes_rule(CSSParser::Keyframes_ruleContext *ctx) = 0;
  virtual void exitKeyframes_rule(CSSParser::Keyframes_ruleContext *ctx) = 0;

  virtual void enterKeyframe_block(CSSParser::Keyframe_blockContext *ctx) = 0;
  virtual void exitKeyframe_block(CSSParser::Keyframe_blockContext *ctx) = 0;

  virtual void enterKeyframe_selector(CSSParser::Keyframe_selectorContext *ctx) = 0;
  virtual void exitKeyframe_selector(CSSParser::Keyframe_selectorContext *ctx) = 0;

  virtual void enterSupports_rule(CSSParser::Supports_ruleContext *ctx) = 0;
  virtual void exitSupports_rule(CSSParser::Supports_ruleContext *ctx) = 0;

  virtual void enterSupports_condition(CSSParser::Supports_conditionContext *ctx) = 0;
  virtual void exitSupports_condition(CSSParser::Supports_conditionContext *ctx) = 0;

  virtual void enterSupports_condition_in_parens(CSSParser::Supports_condition_in_parensContext *ctx) = 0;
  virtual void exitSupports_condition_in_parens(CSSParser::Supports_condition_in_parensContext *ctx) = 0;

  virtual void enterSupports_negation(CSSParser::Supports_negationContext *ctx) = 0;
  virtual void exitSupports_negation(CSSParser::Supports_negationContext *ctx) = 0;

  virtual void enterSupports_conjunction(CSSParser::Supports_conjunctionContext *ctx) = 0;
  virtual void exitSupports_conjunction(CSSParser::Supports_conjunctionContext *ctx) = 0;

  virtual void enterSupports_disjunction(CSSParser::Supports_disjunctionContext *ctx) = 0;
  virtual void exitSupports_disjunction(CSSParser::Supports_disjunctionContext *ctx) = 0;

  virtual void enterSupports_declaration_condition(CSSParser::Supports_declaration_conditionContext *ctx) = 0;
  virtual void exitSupports_declaration_condition(CSSParser::Supports_declaration_conditionContext *ctx) = 0;

  virtual void enterGeneral_enclosed(CSSParser::General_enclosedContext *ctx) = 0;
  virtual void exitGeneral_enclosed(CSSParser::General_enclosedContext *ctx) = 0;

  virtual void enterRuleset(CSSParser::RulesetContext *ctx) = 0;
  virtual void exitRuleset(CSSParser::RulesetContext *ctx) = 0;

  virtual void enterSelector_group(CSSParser::Selector_groupContext *ctx) = 0;
  virtual void exitSelector_group(CSSParser::Selector_groupContext *ctx) = 0;

  virtual void enterSelector(CSSParser::SelectorContext *ctx) = 0;
  virtual void exitSelector(CSSParser::SelectorContext *ctx) = 0;

  virtual void enterCombinator(CSSParser::CombinatorContext *ctx) = 0;
  virtual void exitCombinator(CSSParser::CombinatorContext *ctx) = 0;

  virtual void enterSimple_selector_sequence(CSSParser::Simple_selector_sequenceContext *ctx) = 0;
  virtual void exitSimple_selector_sequence(CSSParser::Simple_selector_sequenceContext *ctx) = 0;

  virtual void enterType_selector(CSSParser::Type_selectorContext *ctx) = 0;
  virtual void exitType_selector(CSSParser::Type_selectorContext *ctx) = 0;

  virtual void enterNamespace_prefix(CSSParser::Namespace_prefixContext *ctx) = 0;
  virtual void exitNamespace_prefix(CSSParser::Namespace_prefixContext *ctx) = 0;

  virtual void enterElement_name(CSSParser::Element_nameContext *ctx) = 0;
  virtual void exitElement_name(CSSParser::Element_nameContext *ctx) = 0;

  virtual void enterHash(CSSParser::HashContext *ctx) = 0;
  virtual void exitHash(CSSParser::HashContext *ctx) = 0;

  virtual void enterClass(CSSParser::ClassContext *ctx) = 0;
  virtual void exitClass(CSSParser::ClassContext *ctx) = 0;

  virtual void enterAttrib(CSSParser::AttribContext *ctx) = 0;
  virtual void exitAttrib(CSSParser::AttribContext *ctx) = 0;

  virtual void enterPseudo(CSSParser::PseudoContext *ctx) = 0;
  virtual void exitPseudo(CSSParser::PseudoContext *ctx) = 0;

  virtual void enterFunctional_pseudo(CSSParser::Functional_pseudoContext *ctx) = 0;
  virtual void exitFunctional_pseudo(CSSParser::Functional_pseudoContext *ctx) = 0;

  virtual void enterNegation(CSSParser::NegationContext *ctx) = 0;
  virtual void exitNegation(CSSParser::NegationContext *ctx) = 0;

  virtual void enterNegation_arg(CSSParser::Negation_argContext *ctx) = 0;
  virtual void exitNegation_arg(CSSParser::Negation_argContext *ctx) = 0;

  virtual void enterDeclaration_list(CSSParser::Declaration_listContext *ctx) = 0;
  virtual void exitDeclaration_list(CSSParser::Declaration_listContext *ctx) = 0;

  virtual void enterDeclaration(CSSParser::DeclarationContext *ctx) = 0;
  virtual void exitDeclaration(CSSParser::DeclarationContext *ctx) = 0;

  virtual void enterPrio(CSSParser::PrioContext *ctx) = 0;
  virtual void exitPrio(CSSParser::PrioContext *ctx) = 0;

  virtual void enterProperty(CSSParser::PropertyContext *ctx) = 0;
  virtual void exitProperty(CSSParser::PropertyContext *ctx) = 0;

  virtual void enterExpr(CSSParser::ExprContext *ctx) = 0;
  virtual void exitExpr(CSSParser::ExprContext *ctx) = 0;

  virtual void enterOperator(CSSParser::OperatorContext *ctx) = 0;
  virtual void exitOperator(CSSParser::OperatorContext *ctx) = 0;

  virtual void enterTerm(CSSParser::TermContext *ctx) = 0;
  virtual void exitTerm(CSSParser::TermContext *ctx) = 0;

  virtual void enterUnary_operator(CSSParser::Unary_operatorContext *ctx) = 0;
  virtual void exitUnary_operator(CSSParser::Unary_operatorContext *ctx) = 0;

  virtual void enterFunction(CSSParser::FunctionContext *ctx) = 0;
  virtual void exitFunction(CSSParser::FunctionContext *ctx) = 0;

  virtual void enterAny(CSSParser::AnyContext *ctx) = 0;
  virtual void exitAny(CSSParser::AnyContext *ctx) = 0;


};

