
// Generated from third-party/antlr/Grammars/CSS3/CSSParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSSParser : public antlr4::Parser {
public:
  enum {
    LBRACE = 1, RBRACE = 2, LPAREN = 3, RPAREN = 4, LBRACKET = 5, RBRACKET = 6, 
    SEMICOLON = 7, COLON = 8, COMMA = 9, DOT = 10, HASH = 11, AT = 12, PLUS = 13, 
    MINUS = 14, MULTIPLY = 15, DIVIDE = 16, EQUALS = 17, INCLUDES = 18, 
    DASHMATCH = 19, PREFIXMATCH = 20, SUFFIXMATCH = 21, SUBSTRINGMATCH = 22, 
    STRING = 23, NUMBER = 24, PERCENTAGE = 25, IDENT = 26, DIMENSION = 27, 
    FUNCTION = 28, URI = 29, COMMENT = 30, WS = 31, IMPORTANT = 32, CDO = 33, 
    CDC = 34, ONLY_KEYWORD = 35, NOT_KEYWORD = 36, AND_KEYWORD = 37, AT_CHARSET = 38, 
    AT_IMPORT = 39, AT_NAMESPACE = 40, AT_MEDIA = 41, AT_PAGE = 42, AT_FONT_FACE = 43, 
    AT_KEYFRAMES = 44, AT_SUPPORTS = 45, FROM_KEYWORD = 46, TO_KEYWORD = 47, 
    OR_KEYWORD = 48, TILDE = 49, PIPE = 50, NOT_FUNCTION = 51, ANYCHAR = 52
  };

  enum {
    RuleStylesheet = 0, RuleCharset = 1, RuleImport_rule = 2, RuleNamespace_rule = 3, 
    RuleNested_statement = 4, RuleUnknown_at_rule = 5, RuleMedia_rule = 6, 
    RuleMedia_query_list = 7, RuleMedia_query = 8, RuleMedia_type = 9, RuleMedia_expression = 10, 
    RuleMedia_feature = 11, RulePage_rule = 12, RulePseudo_page = 13, RuleFont_face_rule = 14, 
    RuleKeyframes_rule = 15, RuleKeyframe_block = 16, RuleKeyframe_selector = 17, 
    RuleSupports_rule = 18, RuleSupports_condition = 19, RuleSupports_condition_in_parens = 20, 
    RuleSupports_negation = 21, RuleSupports_conjunction = 22, RuleSupports_disjunction = 23, 
    RuleSupports_declaration_condition = 24, RuleGeneral_enclosed = 25, 
    RuleRuleset = 26, RuleSelector_group = 27, RuleSelector = 28, RuleCombinator = 29, 
    RuleSimple_selector_sequence = 30, RuleType_selector = 31, RuleNamespace_prefix = 32, 
    RuleElement_name = 33, RuleHash = 34, RuleClass = 35, RuleAttrib = 36, 
    RulePseudo = 37, RuleFunctional_pseudo = 38, RuleNegation = 39, RuleNegation_arg = 40, 
    RuleDeclaration_list = 41, RuleDeclaration = 42, RulePrio = 43, RuleProperty = 44, 
    RuleExpr = 45, RuleOperator = 46, RuleTerm = 47, RuleUnary_operator = 48, 
    RuleFunction = 49, RuleAny = 50
  };

  explicit CSSParser(antlr4::TokenStream *input);

  CSSParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~CSSParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class StylesheetContext;
  class CharsetContext;
  class Import_ruleContext;
  class Namespace_ruleContext;
  class Nested_statementContext;
  class Unknown_at_ruleContext;
  class Media_ruleContext;
  class Media_query_listContext;
  class Media_queryContext;
  class Media_typeContext;
  class Media_expressionContext;
  class Media_featureContext;
  class Page_ruleContext;
  class Pseudo_pageContext;
  class Font_face_ruleContext;
  class Keyframes_ruleContext;
  class Keyframe_blockContext;
  class Keyframe_selectorContext;
  class Supports_ruleContext;
  class Supports_conditionContext;
  class Supports_condition_in_parensContext;
  class Supports_negationContext;
  class Supports_conjunctionContext;
  class Supports_disjunctionContext;
  class Supports_declaration_conditionContext;
  class General_enclosedContext;
  class RulesetContext;
  class Selector_groupContext;
  class SelectorContext;
  class CombinatorContext;
  class Simple_selector_sequenceContext;
  class Type_selectorContext;
  class Namespace_prefixContext;
  class Element_nameContext;
  class HashContext;
  class ClassContext;
  class AttribContext;
  class PseudoContext;
  class Functional_pseudoContext;
  class NegationContext;
  class Negation_argContext;
  class Declaration_listContext;
  class DeclarationContext;
  class PrioContext;
  class PropertyContext;
  class ExprContext;
  class OperatorContext;
  class TermContext;
  class Unary_operatorContext;
  class FunctionContext;
  class AnyContext; 

  class  StylesheetContext : public antlr4::ParserRuleContext {
  public:
    StylesheetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<CharsetContext *> charset();
    CharsetContext* charset(size_t i);
    std::vector<Import_ruleContext *> import_rule();
    Import_ruleContext* import_rule(size_t i);
    std::vector<Namespace_ruleContext *> namespace_rule();
    Namespace_ruleContext* namespace_rule(size_t i);
    std::vector<Nested_statementContext *> nested_statement();
    Nested_statementContext* nested_statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StylesheetContext* stylesheet();

  class  CharsetContext : public antlr4::ParserRuleContext {
  public:
    CharsetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_CHARSET();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *SEMICOLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CharsetContext* charset();

  class  Import_ruleContext : public antlr4::ParserRuleContext {
  public:
    Import_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_IMPORT();
    antlr4::tree::TerminalNode *SEMICOLON();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *URI();
    Media_query_listContext *media_query_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Import_ruleContext* import_rule();

  class  Namespace_ruleContext : public antlr4::ParserRuleContext {
  public:
    Namespace_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_NAMESPACE();
    antlr4::tree::TerminalNode *SEMICOLON();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *URI();
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Namespace_ruleContext* namespace_rule();

  class  Nested_statementContext : public antlr4::ParserRuleContext {
  public:
    Nested_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    RulesetContext *ruleset();
    Media_ruleContext *media_rule();
    Page_ruleContext *page_rule();
    Font_face_ruleContext *font_face_rule();
    Keyframes_ruleContext *keyframes_rule();
    Supports_ruleContext *supports_rule();
    Unknown_at_ruleContext *unknown_at_rule();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Nested_statementContext* nested_statement();

  class  Unknown_at_ruleContext : public antlr4::ParserRuleContext {
  public:
    Unknown_at_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *SEMICOLON();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Unknown_at_ruleContext* unknown_at_rule();

  class  Media_ruleContext : public antlr4::ParserRuleContext {
  public:
    Media_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_MEDIA();
    Media_query_listContext *media_query_list();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<Nested_statementContext *> nested_statement();
    Nested_statementContext* nested_statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Media_ruleContext* media_rule();

  class  Media_query_listContext : public antlr4::ParserRuleContext {
  public:
    Media_query_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Media_queryContext *> media_query();
    Media_queryContext* media_query(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Media_query_listContext* media_query_list();

  class  Media_queryContext : public antlr4::ParserRuleContext {
  public:
    Media_queryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Media_typeContext *media_type();
    std::vector<antlr4::tree::TerminalNode *> AND_KEYWORD();
    antlr4::tree::TerminalNode* AND_KEYWORD(size_t i);
    std::vector<Media_expressionContext *> media_expression();
    Media_expressionContext* media_expression(size_t i);
    antlr4::tree::TerminalNode *ONLY_KEYWORD();
    antlr4::tree::TerminalNode *NOT_KEYWORD();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Media_queryContext* media_query();

  class  Media_typeContext : public antlr4::ParserRuleContext {
  public:
    Media_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Media_typeContext* media_type();

  class  Media_expressionContext : public antlr4::ParserRuleContext {
  public:
    Media_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Media_featureContext *media_feature();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *COLON();
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Media_expressionContext* media_expression();

  class  Media_featureContext : public antlr4::ParserRuleContext {
  public:
    Media_featureContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Media_featureContext* media_feature();

  class  Page_ruleContext : public antlr4::ParserRuleContext {
  public:
    Page_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_PAGE();
    antlr4::tree::TerminalNode *LBRACE();
    Declaration_listContext *declaration_list();
    antlr4::tree::TerminalNode *RBRACE();
    Pseudo_pageContext *pseudo_page();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Page_ruleContext* page_rule();

  class  Pseudo_pageContext : public antlr4::ParserRuleContext {
  public:
    Pseudo_pageContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Pseudo_pageContext* pseudo_page();

  class  Font_face_ruleContext : public antlr4::ParserRuleContext {
  public:
    Font_face_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_FONT_FACE();
    antlr4::tree::TerminalNode *LBRACE();
    Declaration_listContext *declaration_list();
    antlr4::tree::TerminalNode *RBRACE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Font_face_ruleContext* font_face_rule();

  class  Keyframes_ruleContext : public antlr4::ParserRuleContext {
  public:
    Keyframes_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_KEYFRAMES();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<Keyframe_blockContext *> keyframe_block();
    Keyframe_blockContext* keyframe_block(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Keyframes_ruleContext* keyframes_rule();

  class  Keyframe_blockContext : public antlr4::ParserRuleContext {
  public:
    Keyframe_blockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Keyframe_selectorContext *keyframe_selector();
    antlr4::tree::TerminalNode *LBRACE();
    Declaration_listContext *declaration_list();
    antlr4::tree::TerminalNode *RBRACE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Keyframe_blockContext* keyframe_block();

  class  Keyframe_selectorContext : public antlr4::ParserRuleContext {
  public:
    Keyframe_selectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> PERCENTAGE();
    antlr4::tree::TerminalNode* PERCENTAGE(size_t i);
    std::vector<antlr4::tree::TerminalNode *> FROM_KEYWORD();
    antlr4::tree::TerminalNode* FROM_KEYWORD(size_t i);
    std::vector<antlr4::tree::TerminalNode *> TO_KEYWORD();
    antlr4::tree::TerminalNode* TO_KEYWORD(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Keyframe_selectorContext* keyframe_selector();

  class  Supports_ruleContext : public antlr4::ParserRuleContext {
  public:
    Supports_ruleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_SUPPORTS();
    Supports_conditionContext *supports_condition();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<Nested_statementContext *> nested_statement();
    Nested_statementContext* nested_statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Supports_ruleContext* supports_rule();

  class  Supports_conditionContext : public antlr4::ParserRuleContext {
  public:
    Supports_conditionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Supports_negationContext *supports_negation();
    Supports_conjunctionContext *supports_conjunction();
    Supports_disjunctionContext *supports_disjunction();
    Supports_condition_in_parensContext *supports_condition_in_parens();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Supports_conditionContext* supports_condition();

  class  Supports_condition_in_parensContext : public antlr4::ParserRuleContext {
  public:
    Supports_condition_in_parensContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Supports_conditionContext *supports_condition();
    antlr4::tree::TerminalNode *RPAREN();
    Supports_declaration_conditionContext *supports_declaration_condition();
    General_enclosedContext *general_enclosed();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Supports_condition_in_parensContext* supports_condition_in_parens();

  class  Supports_negationContext : public antlr4::ParserRuleContext {
  public:
    Supports_negationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NOT_KEYWORD();
    Supports_condition_in_parensContext *supports_condition_in_parens();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Supports_negationContext* supports_negation();

  class  Supports_conjunctionContext : public antlr4::ParserRuleContext {
  public:
    Supports_conjunctionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Supports_condition_in_parensContext *> supports_condition_in_parens();
    Supports_condition_in_parensContext* supports_condition_in_parens(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AND_KEYWORD();
    antlr4::tree::TerminalNode* AND_KEYWORD(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Supports_conjunctionContext* supports_conjunction();

  class  Supports_disjunctionContext : public antlr4::ParserRuleContext {
  public:
    Supports_disjunctionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Supports_condition_in_parensContext *> supports_condition_in_parens();
    Supports_condition_in_parensContext* supports_condition_in_parens(size_t i);
    std::vector<antlr4::tree::TerminalNode *> OR_KEYWORD();
    antlr4::tree::TerminalNode* OR_KEYWORD(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Supports_disjunctionContext* supports_disjunction();

  class  Supports_declaration_conditionContext : public antlr4::ParserRuleContext {
  public:
    Supports_declaration_conditionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    DeclarationContext *declaration();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Supports_declaration_conditionContext* supports_declaration_condition();

  class  General_enclosedContext : public antlr4::ParserRuleContext {
  public:
    General_enclosedContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *FUNCTION();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  General_enclosedContext* general_enclosed();

  class  RulesetContext : public antlr4::ParserRuleContext {
  public:
    RulesetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Selector_groupContext *selector_group();
    antlr4::tree::TerminalNode *LBRACE();
    Declaration_listContext *declaration_list();
    antlr4::tree::TerminalNode *RBRACE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RulesetContext* ruleset();

  class  Selector_groupContext : public antlr4::ParserRuleContext {
  public:
    Selector_groupContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SelectorContext *> selector();
    SelectorContext* selector(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Selector_groupContext* selector_group();

  class  SelectorContext : public antlr4::ParserRuleContext {
  public:
    SelectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Simple_selector_sequenceContext *> simple_selector_sequence();
    Simple_selector_sequenceContext* simple_selector_sequence(size_t i);
    std::vector<CombinatorContext *> combinator();
    CombinatorContext* combinator(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SelectorContext* selector();

  class  CombinatorContext : public antlr4::ParserRuleContext {
  public:
    CombinatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *MULTIPLY();
    antlr4::tree::TerminalNode *TILDE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CombinatorContext* combinator();

  class  Simple_selector_sequenceContext : public antlr4::ParserRuleContext {
  public:
    Simple_selector_sequenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_selectorContext *type_selector();
    std::vector<HashContext *> hash();
    HashContext* hash(size_t i);
    std::vector<ClassContext *> class_();
    ClassContext* class_(size_t i);
    std::vector<AttribContext *> attrib();
    AttribContext* attrib(size_t i);
    std::vector<PseudoContext *> pseudo();
    PseudoContext* pseudo(size_t i);
    std::vector<NegationContext *> negation();
    NegationContext* negation(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Simple_selector_sequenceContext* simple_selector_sequence();

  class  Type_selectorContext : public antlr4::ParserRuleContext {
  public:
    Type_selectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Element_nameContext *element_name();
    Namespace_prefixContext *namespace_prefix();
    antlr4::tree::TerminalNode *MULTIPLY();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Type_selectorContext* type_selector();

  class  Namespace_prefixContext : public antlr4::ParserRuleContext {
  public:
    Namespace_prefixContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PIPE();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *MULTIPLY();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Namespace_prefixContext* namespace_prefix();

  class  Element_nameContext : public antlr4::ParserRuleContext {
  public:
    Element_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Element_nameContext* element_name();

  class  HashContext : public antlr4::ParserRuleContext {
  public:
    HashContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  HashContext* hash();

  class  ClassContext : public antlr4::ParserRuleContext {
  public:
    ClassContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ClassContext* class_();

  class  AttribContext : public antlr4::ParserRuleContext {
  public:
    AttribContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    antlr4::tree::TerminalNode *PREFIXMATCH();
    antlr4::tree::TerminalNode *SUFFIXMATCH();
    antlr4::tree::TerminalNode *SUBSTRINGMATCH();
    antlr4::tree::TerminalNode *EQUALS();
    antlr4::tree::TerminalNode *INCLUDES();
    antlr4::tree::TerminalNode *DASHMATCH();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AttribContext* attrib();

  class  PseudoContext : public antlr4::ParserRuleContext {
  public:
    PseudoContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    antlr4::tree::TerminalNode *IDENT();
    Functional_pseudoContext *functional_pseudo();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PseudoContext* pseudo();

  class  Functional_pseudoContext : public antlr4::ParserRuleContext {
  public:
    Functional_pseudoContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FUNCTION();
    ExprContext *expr();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Functional_pseudoContext* functional_pseudo();

  class  NegationContext : public antlr4::ParserRuleContext {
  public:
    NegationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NOT_FUNCTION();
    Negation_argContext *negation_arg();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  NegationContext* negation();

  class  Negation_argContext : public antlr4::ParserRuleContext {
  public:
    Negation_argContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_selectorContext *type_selector();
    HashContext *hash();
    ClassContext *class_();
    AttribContext *attrib();
    PseudoContext *pseudo();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Negation_argContext* negation_arg();

  class  Declaration_listContext : public antlr4::ParserRuleContext {
  public:
    Declaration_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DeclarationContext *> declaration();
    DeclarationContext* declaration(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMICOLON();
    antlr4::tree::TerminalNode* SEMICOLON(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Declaration_listContext* declaration_list();

  class  DeclarationContext : public antlr4::ParserRuleContext {
  public:
    DeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyContext *property();
    antlr4::tree::TerminalNode *COLON();
    ExprContext *expr();
    PrioContext *prio();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DeclarationContext* declaration();

  class  PrioContext : public antlr4::ParserRuleContext {
  public:
    PrioContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IMPORTANT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PrioContext* prio();

  class  PropertyContext : public antlr4::ParserRuleContext {
  public:
    PropertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PropertyContext* property();

  class  ExprContext : public antlr4::ParserRuleContext {
  public:
    ExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TermContext *> term();
    TermContext* term(size_t i);
    std::vector<OperatorContext *> operator_();
    OperatorContext* operator_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExprContext* expr();

  class  OperatorContext : public antlr4::ParserRuleContext {
  public:
    OperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DIVIDE();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OperatorContext* operator_();

  class  TermContext : public antlr4::ParserRuleContext {
  public:
    TermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *DIMENSION();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *URI();
    antlr4::tree::TerminalNode *HASH();
    FunctionContext *function();
    Unary_operatorContext *unary_operator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TermContext* term();

  class  Unary_operatorContext : public antlr4::ParserRuleContext {
  public:
    Unary_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *PLUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Unary_operatorContext* unary_operator();

  class  FunctionContext : public antlr4::ParserRuleContext {
  public:
    FunctionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FUNCTION();
    ExprContext *expr();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FunctionContext* function();

  class  AnyContext : public antlr4::ParserRuleContext {
  public:
    AnyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *NUMBER();
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *DIMENSION();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *URI();
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *MULTIPLY();
    antlr4::tree::TerminalNode *DIVIDE();
    antlr4::tree::TerminalNode *EQUALS();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *SEMICOLON();
    antlr4::tree::TerminalNode *COMMA();
    antlr4::tree::TerminalNode *DOT();
    FunctionContext *function();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *RBRACKET();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *ANYCHAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AnyContext* any();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

