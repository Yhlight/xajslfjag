
// Generated from third-party/antlr/Grammars/CSS3/CSSParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSSParserListener.h"


/**
 * This class provides an empty implementation of CSSParserListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  CSSParserBaseListener : public CSSParserListener {
public:

  virtual void enterStylesheet(CSSParser::StylesheetContext * /*ctx*/) override { }
  virtual void exitStylesheet(CSSParser::StylesheetContext * /*ctx*/) override { }

  virtual void enterCharset(CSSParser::CharsetContext * /*ctx*/) override { }
  virtual void exitCharset(CSSParser::CharsetContext * /*ctx*/) override { }

  virtual void enterImport_rule(CSSParser::Import_ruleContext * /*ctx*/) override { }
  virtual void exitImport_rule(CSSParser::Import_ruleContext * /*ctx*/) override { }

  virtual void enterNamespace_rule(CSSParser::Namespace_ruleContext * /*ctx*/) override { }
  virtual void exitNamespace_rule(CSSParser::Namespace_ruleContext * /*ctx*/) override { }

  virtual void enterNested_statement(CSSParser::Nested_statementContext * /*ctx*/) override { }
  virtual void exitNested_statement(CSSParser::Nested_statementContext * /*ctx*/) override { }

  virtual void enterUnknown_at_rule(CSSParser::Unknown_at_ruleContext * /*ctx*/) override { }
  virtual void exitUnknown_at_rule(CSSParser::Unknown_at_ruleContext * /*ctx*/) override { }

  virtual void enterMedia_rule(CSSParser::Media_ruleContext * /*ctx*/) override { }
  virtual void exitMedia_rule(CSSParser::Media_ruleContext * /*ctx*/) override { }

  virtual void enterMedia_query_list(CSSParser::Media_query_listContext * /*ctx*/) override { }
  virtual void exitMedia_query_list(CSSParser::Media_query_listContext * /*ctx*/) override { }

  virtual void enterMedia_query(CSSParser::Media_queryContext * /*ctx*/) override { }
  virtual void exitMedia_query(CSSParser::Media_queryContext * /*ctx*/) override { }

  virtual void enterMedia_type(CSSParser::Media_typeContext * /*ctx*/) override { }
  virtual void exitMedia_type(CSSParser::Media_typeContext * /*ctx*/) override { }

  virtual void enterMedia_expression(CSSParser::Media_expressionContext * /*ctx*/) override { }
  virtual void exitMedia_expression(CSSParser::Media_expressionContext * /*ctx*/) override { }

  virtual void enterMedia_feature(CSSParser::Media_featureContext * /*ctx*/) override { }
  virtual void exitMedia_feature(CSSParser::Media_featureContext * /*ctx*/) override { }

  virtual void enterPage_rule(CSSParser::Page_ruleContext * /*ctx*/) override { }
  virtual void exitPage_rule(CSSParser::Page_ruleContext * /*ctx*/) override { }

  virtual void enterPseudo_page(CSSParser::Pseudo_pageContext * /*ctx*/) override { }
  virtual void exitPseudo_page(CSSParser::Pseudo_pageContext * /*ctx*/) override { }

  virtual void enterFont_face_rule(CSSParser::Font_face_ruleContext * /*ctx*/) override { }
  virtual void exitFont_face_rule(CSSParser::Font_face_ruleContext * /*ctx*/) override { }

  virtual void enterKeyframes_rule(CSSParser::Keyframes_ruleContext * /*ctx*/) override { }
  virtual void exitKeyframes_rule(CSSParser::Keyframes_ruleContext * /*ctx*/) override { }

  virtual void enterKeyframe_block(CSSParser::Keyframe_blockContext * /*ctx*/) override { }
  virtual void exitKeyframe_block(CSSParser::Keyframe_blockContext * /*ctx*/) override { }

  virtual void enterKeyframe_selector(CSSParser::Keyframe_selectorContext * /*ctx*/) override { }
  virtual void exitKeyframe_selector(CSSParser::Keyframe_selectorContext * /*ctx*/) override { }

  virtual void enterSupports_rule(CSSParser::Supports_ruleContext * /*ctx*/) override { }
  virtual void exitSupports_rule(CSSParser::Supports_ruleContext * /*ctx*/) override { }

  virtual void enterSupports_condition(CSSParser::Supports_conditionContext * /*ctx*/) override { }
  virtual void exitSupports_condition(CSSParser::Supports_conditionContext * /*ctx*/) override { }

  virtual void enterSupports_condition_in_parens(CSSParser::Supports_condition_in_parensContext * /*ctx*/) override { }
  virtual void exitSupports_condition_in_parens(CSSParser::Supports_condition_in_parensContext * /*ctx*/) override { }

  virtual void enterSupports_negation(CSSParser::Supports_negationContext * /*ctx*/) override { }
  virtual void exitSupports_negation(CSSParser::Supports_negationContext * /*ctx*/) override { }

  virtual void enterSupports_conjunction(CSSParser::Supports_conjunctionContext * /*ctx*/) override { }
  virtual void exitSupports_conjunction(CSSParser::Supports_conjunctionContext * /*ctx*/) override { }

  virtual void enterSupports_disjunction(CSSParser::Supports_disjunctionContext * /*ctx*/) override { }
  virtual void exitSupports_disjunction(CSSParser::Supports_disjunctionContext * /*ctx*/) override { }

  virtual void enterSupports_declaration_condition(CSSParser::Supports_declaration_conditionContext * /*ctx*/) override { }
  virtual void exitSupports_declaration_condition(CSSParser::Supports_declaration_conditionContext * /*ctx*/) override { }

  virtual void enterGeneral_enclosed(CSSParser::General_enclosedContext * /*ctx*/) override { }
  virtual void exitGeneral_enclosed(CSSParser::General_enclosedContext * /*ctx*/) override { }

  virtual void enterRuleset(CSSParser::RulesetContext * /*ctx*/) override { }
  virtual void exitRuleset(CSSParser::RulesetContext * /*ctx*/) override { }

  virtual void enterSelector_group(CSSParser::Selector_groupContext * /*ctx*/) override { }
  virtual void exitSelector_group(CSSParser::Selector_groupContext * /*ctx*/) override { }

  virtual void enterSelector(CSSParser::SelectorContext * /*ctx*/) override { }
  virtual void exitSelector(CSSParser::SelectorContext * /*ctx*/) override { }

  virtual void enterCombinator(CSSParser::CombinatorContext * /*ctx*/) override { }
  virtual void exitCombinator(CSSParser::CombinatorContext * /*ctx*/) override { }

  virtual void enterSimple_selector_sequence(CSSParser::Simple_selector_sequenceContext * /*ctx*/) override { }
  virtual void exitSimple_selector_sequence(CSSParser::Simple_selector_sequenceContext * /*ctx*/) override { }

  virtual void enterType_selector(CSSParser::Type_selectorContext * /*ctx*/) override { }
  virtual void exitType_selector(CSSParser::Type_selectorContext * /*ctx*/) override { }

  virtual void enterNamespace_prefix(CSSParser::Namespace_prefixContext * /*ctx*/) override { }
  virtual void exitNamespace_prefix(CSSParser::Namespace_prefixContext * /*ctx*/) override { }

  virtual void enterElement_name(CSSParser::Element_nameContext * /*ctx*/) override { }
  virtual void exitElement_name(CSSParser::Element_nameContext * /*ctx*/) override { }

  virtual void enterHash(CSSParser::HashContext * /*ctx*/) override { }
  virtual void exitHash(CSSParser::HashContext * /*ctx*/) override { }

  virtual void enterClass(CSSParser::ClassContext * /*ctx*/) override { }
  virtual void exitClass(CSSParser::ClassContext * /*ctx*/) override { }

  virtual void enterAttrib(CSSParser::AttribContext * /*ctx*/) override { }
  virtual void exitAttrib(CSSParser::AttribContext * /*ctx*/) override { }

  virtual void enterPseudo(CSSParser::PseudoContext * /*ctx*/) override { }
  virtual void exitPseudo(CSSParser::PseudoContext * /*ctx*/) override { }

  virtual void enterFunctional_pseudo(CSSParser::Functional_pseudoContext * /*ctx*/) override { }
  virtual void exitFunctional_pseudo(CSSParser::Functional_pseudoContext * /*ctx*/) override { }

  virtual void enterNegation(CSSParser::NegationContext * /*ctx*/) override { }
  virtual void exitNegation(CSSParser::NegationContext * /*ctx*/) override { }

  virtual void enterNegation_arg(CSSParser::Negation_argContext * /*ctx*/) override { }
  virtual void exitNegation_arg(CSSParser::Negation_argContext * /*ctx*/) override { }

  virtual void enterDeclaration_list(CSSParser::Declaration_listContext * /*ctx*/) override { }
  virtual void exitDeclaration_list(CSSParser::Declaration_listContext * /*ctx*/) override { }

  virtual void enterDeclaration(CSSParser::DeclarationContext * /*ctx*/) override { }
  virtual void exitDeclaration(CSSParser::DeclarationContext * /*ctx*/) override { }

  virtual void enterPrio(CSSParser::PrioContext * /*ctx*/) override { }
  virtual void exitPrio(CSSParser::PrioContext * /*ctx*/) override { }

  virtual void enterProperty(CSSParser::PropertyContext * /*ctx*/) override { }
  virtual void exitProperty(CSSParser::PropertyContext * /*ctx*/) override { }

  virtual void enterExpr(CSSParser::ExprContext * /*ctx*/) override { }
  virtual void exitExpr(CSSParser::ExprContext * /*ctx*/) override { }

  virtual void enterOperator(CSSParser::OperatorContext * /*ctx*/) override { }
  virtual void exitOperator(CSSParser::OperatorContext * /*ctx*/) override { }

  virtual void enterTerm(CSSParser::TermContext * /*ctx*/) override { }
  virtual void exitTerm(CSSParser::TermContext * /*ctx*/) override { }

  virtual void enterUnary_operator(CSSParser::Unary_operatorContext * /*ctx*/) override { }
  virtual void exitUnary_operator(CSSParser::Unary_operatorContext * /*ctx*/) override { }

  virtual void enterFunction(CSSParser::FunctionContext * /*ctx*/) override { }
  virtual void exitFunction(CSSParser::FunctionContext * /*ctx*/) override { }

  virtual void enterAny(CSSParser::AnyContext * /*ctx*/) override { }
  virtual void exitAny(CSSParser::AnyContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

