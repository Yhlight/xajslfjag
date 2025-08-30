
// Generated from CSSParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSSParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by CSSParser.
 */
class  CSSParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by CSSParser.
   */
    virtual std::any visitStylesheet(CSSParser::StylesheetContext *context) = 0;

    virtual std::any visitCharset(CSSParser::CharsetContext *context) = 0;

    virtual std::any visitImport_rule(CSSParser::Import_ruleContext *context) = 0;

    virtual std::any visitNamespace_rule(CSSParser::Namespace_ruleContext *context) = 0;

    virtual std::any visitNested_statement(CSSParser::Nested_statementContext *context) = 0;

    virtual std::any visitUnknown_at_rule(CSSParser::Unknown_at_ruleContext *context) = 0;

    virtual std::any visitMedia_rule(CSSParser::Media_ruleContext *context) = 0;

    virtual std::any visitMedia_query_list(CSSParser::Media_query_listContext *context) = 0;

    virtual std::any visitMedia_query(CSSParser::Media_queryContext *context) = 0;

    virtual std::any visitMedia_type(CSSParser::Media_typeContext *context) = 0;

    virtual std::any visitMedia_expression(CSSParser::Media_expressionContext *context) = 0;

    virtual std::any visitMedia_feature(CSSParser::Media_featureContext *context) = 0;

    virtual std::any visitPage_rule(CSSParser::Page_ruleContext *context) = 0;

    virtual std::any visitPseudo_page(CSSParser::Pseudo_pageContext *context) = 0;

    virtual std::any visitFont_face_rule(CSSParser::Font_face_ruleContext *context) = 0;

    virtual std::any visitKeyframes_rule(CSSParser::Keyframes_ruleContext *context) = 0;

    virtual std::any visitKeyframe_block(CSSParser::Keyframe_blockContext *context) = 0;

    virtual std::any visitKeyframe_selector(CSSParser::Keyframe_selectorContext *context) = 0;

    virtual std::any visitSupports_rule(CSSParser::Supports_ruleContext *context) = 0;

    virtual std::any visitSupports_condition(CSSParser::Supports_conditionContext *context) = 0;

    virtual std::any visitSupports_condition_in_parens(CSSParser::Supports_condition_in_parensContext *context) = 0;

    virtual std::any visitSupports_negation(CSSParser::Supports_negationContext *context) = 0;

    virtual std::any visitSupports_conjunction(CSSParser::Supports_conjunctionContext *context) = 0;

    virtual std::any visitSupports_disjunction(CSSParser::Supports_disjunctionContext *context) = 0;

    virtual std::any visitSupports_declaration_condition(CSSParser::Supports_declaration_conditionContext *context) = 0;

    virtual std::any visitGeneral_enclosed(CSSParser::General_enclosedContext *context) = 0;

    virtual std::any visitRuleset(CSSParser::RulesetContext *context) = 0;

    virtual std::any visitSelector_group(CSSParser::Selector_groupContext *context) = 0;

    virtual std::any visitSelector(CSSParser::SelectorContext *context) = 0;

    virtual std::any visitCombinator(CSSParser::CombinatorContext *context) = 0;

    virtual std::any visitSimple_selector_sequence(CSSParser::Simple_selector_sequenceContext *context) = 0;

    virtual std::any visitType_selector(CSSParser::Type_selectorContext *context) = 0;

    virtual std::any visitNamespace_prefix(CSSParser::Namespace_prefixContext *context) = 0;

    virtual std::any visitElement_name(CSSParser::Element_nameContext *context) = 0;

    virtual std::any visitHash(CSSParser::HashContext *context) = 0;

    virtual std::any visitClass(CSSParser::ClassContext *context) = 0;

    virtual std::any visitAttrib(CSSParser::AttribContext *context) = 0;

    virtual std::any visitPseudo(CSSParser::PseudoContext *context) = 0;

    virtual std::any visitFunctional_pseudo(CSSParser::Functional_pseudoContext *context) = 0;

    virtual std::any visitNegation(CSSParser::NegationContext *context) = 0;

    virtual std::any visitNegation_arg(CSSParser::Negation_argContext *context) = 0;

    virtual std::any visitDeclaration_list(CSSParser::Declaration_listContext *context) = 0;

    virtual std::any visitDeclaration(CSSParser::DeclarationContext *context) = 0;

    virtual std::any visitPrio(CSSParser::PrioContext *context) = 0;

    virtual std::any visitProperty(CSSParser::PropertyContext *context) = 0;

    virtual std::any visitExpr(CSSParser::ExprContext *context) = 0;

    virtual std::any visitOperator(CSSParser::OperatorContext *context) = 0;

    virtual std::any visitTerm(CSSParser::TermContext *context) = 0;

    virtual std::any visitUnary_operator(CSSParser::Unary_operatorContext *context) = 0;

    virtual std::any visitFunction(CSSParser::FunctionContext *context) = 0;

    virtual std::any visitAny(CSSParser::AnyContext *context) = 0;


};

