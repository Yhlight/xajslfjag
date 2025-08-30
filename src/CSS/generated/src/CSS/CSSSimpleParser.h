
// Generated from src/CSS/CSSSimple.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSSSimpleParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, Ident = 13, Hash = 14, 
    Number = 15, Percentage = 16, Dimension = 17, String_ = 18, Color = 19, 
    Function = 20, Comment = 21, WS = 22
  };

  enum {
    RuleStylesheet = 0, RuleStatement = 1, RuleRuleset = 2, RuleSelector = 3, 
    RuleSimpleSelector = 4, RuleDeclaration = 5, RuleProperty = 6, RuleValue = 7, 
    RuleValueList = 8, RuleAtRule = 9, RuleAny = 10
  };

  explicit CSSSimpleParser(antlr4::TokenStream *input);

  CSSSimpleParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~CSSSimpleParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class StylesheetContext;
  class StatementContext;
  class RulesetContext;
  class SelectorContext;
  class SimpleSelectorContext;
  class DeclarationContext;
  class PropertyContext;
  class ValueContext;
  class ValueListContext;
  class AtRuleContext;
  class AnyContext; 

  class  StylesheetContext : public antlr4::ParserRuleContext {
  public:
    StylesheetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StylesheetContext* stylesheet();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    RulesetContext *ruleset();
    AtRuleContext *atRule();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StatementContext* statement();

  class  RulesetContext : public antlr4::ParserRuleContext {
  public:
    RulesetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SelectorContext *> selector();
    SelectorContext* selector(size_t i);
    std::vector<DeclarationContext *> declaration();
    DeclarationContext* declaration(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RulesetContext* ruleset();

  class  SelectorContext : public antlr4::ParserRuleContext {
  public:
    SelectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SimpleSelectorContext *> simpleSelector();
    SimpleSelectorContext* simpleSelector(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SelectorContext* selector();

  class  SimpleSelectorContext : public antlr4::ParserRuleContext {
  public:
    SimpleSelectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Ident();
    antlr4::tree::TerminalNode *Hash();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SimpleSelectorContext* simpleSelector();

  class  DeclarationContext : public antlr4::ParserRuleContext {
  public:
    DeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyContext *property();
    ValueContext *value();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DeclarationContext* declaration();

  class  PropertyContext : public antlr4::ParserRuleContext {
  public:
    PropertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Ident();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyContext* property();

  class  ValueContext : public antlr4::ParserRuleContext {
  public:
    ValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> Ident();
    antlr4::tree::TerminalNode* Ident(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Number();
    antlr4::tree::TerminalNode* Number(size_t i);
    std::vector<antlr4::tree::TerminalNode *> String_();
    antlr4::tree::TerminalNode* String_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Color();
    antlr4::tree::TerminalNode* Color(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Function();
    antlr4::tree::TerminalNode* Function(size_t i);
    std::vector<ValueListContext *> valueList();
    ValueListContext* valueList(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Percentage();
    antlr4::tree::TerminalNode* Percentage(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Dimension();
    antlr4::tree::TerminalNode* Dimension(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueContext* value();

  class  ValueListContext : public antlr4::ParserRuleContext {
  public:
    ValueListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ValueContext *> value();
    ValueContext* value(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueListContext* valueList();

  class  AtRuleContext : public antlr4::ParserRuleContext {
  public:
    AtRuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Ident();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AtRuleContext* atRule();

  class  AnyContext : public antlr4::ParserRuleContext {
  public:
    AnyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Ident();
    antlr4::tree::TerminalNode *Number();
    antlr4::tree::TerminalNode *String_();
    antlr4::tree::TerminalNode *Color();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AnyContext* any();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

