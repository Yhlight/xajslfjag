
// Generated from src/CSS/CSSSimple.g4 by ANTLR 4.13.2


#include "CSSSimpleVisitor.h"

#include "CSSSimpleParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct CSSSimpleParserStaticData final {
  CSSSimpleParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  CSSSimpleParserStaticData(const CSSSimpleParserStaticData&) = delete;
  CSSSimpleParserStaticData(CSSSimpleParserStaticData&&) = delete;
  CSSSimpleParserStaticData& operator=(const CSSSimpleParserStaticData&) = delete;
  CSSSimpleParserStaticData& operator=(CSSSimpleParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag csssimpleParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<CSSSimpleParserStaticData> csssimpleParserStaticData = nullptr;

void csssimpleParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (csssimpleParserStaticData != nullptr) {
    return;
  }
#else
  assert(csssimpleParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CSSSimpleParserStaticData>(
    std::vector<std::string>{
      "stylesheet", "statement", "ruleset", "selector", "simpleSelector", 
      "declaration", "property", "value", "valueList", "atRule", "any"
    },
    std::vector<std::string>{
      "", "','", "'{'", "'}'", "'*'", "'.'", "'['", "']'", "':'", "';'", 
      "')'", "'@'", "'('"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "Ident", "Hash", 
      "Number", "Percentage", "Dimension", "String_", "Color", "Function", 
      "Comment", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,22,118,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,1,0,5,0,24,8,0,10,0,12,0,27,9,0,1,0,1,
  	0,1,1,1,1,3,1,33,8,1,1,2,1,2,1,2,5,2,38,8,2,10,2,12,2,41,9,2,1,2,1,2,
  	5,2,45,8,2,10,2,12,2,48,9,2,1,2,1,2,1,3,4,3,53,8,3,11,3,12,3,54,1,4,1,
  	4,1,4,1,4,1,4,1,4,1,4,1,4,3,4,65,8,4,1,5,1,5,1,5,1,5,3,5,71,8,5,1,6,1,
  	6,1,7,1,7,1,7,1,7,1,7,1,7,1,7,1,7,1,7,1,7,4,7,85,8,7,11,7,12,7,86,1,8,
  	1,8,1,8,5,8,92,8,8,10,8,12,8,95,9,8,1,9,1,9,1,9,5,9,100,8,9,10,9,12,9,
  	103,9,9,1,9,1,9,1,9,5,9,108,8,9,10,9,12,9,111,9,9,1,9,3,9,114,8,9,1,10,
  	1,10,1,10,0,0,11,0,2,4,6,8,10,12,14,16,18,20,0,1,5,0,1,1,6,10,12,13,15,
  	15,18,19,127,0,25,1,0,0,0,2,32,1,0,0,0,4,34,1,0,0,0,6,52,1,0,0,0,8,64,
  	1,0,0,0,10,66,1,0,0,0,12,72,1,0,0,0,14,84,1,0,0,0,16,88,1,0,0,0,18,96,
  	1,0,0,0,20,115,1,0,0,0,22,24,3,2,1,0,23,22,1,0,0,0,24,27,1,0,0,0,25,23,
  	1,0,0,0,25,26,1,0,0,0,26,28,1,0,0,0,27,25,1,0,0,0,28,29,5,0,0,1,29,1,
  	1,0,0,0,30,33,3,4,2,0,31,33,3,18,9,0,32,30,1,0,0,0,32,31,1,0,0,0,33,3,
  	1,0,0,0,34,39,3,6,3,0,35,36,5,1,0,0,36,38,3,6,3,0,37,35,1,0,0,0,38,41,
  	1,0,0,0,39,37,1,0,0,0,39,40,1,0,0,0,40,42,1,0,0,0,41,39,1,0,0,0,42,46,
  	5,2,0,0,43,45,3,10,5,0,44,43,1,0,0,0,45,48,1,0,0,0,46,44,1,0,0,0,46,47,
  	1,0,0,0,47,49,1,0,0,0,48,46,1,0,0,0,49,50,5,3,0,0,50,5,1,0,0,0,51,53,
  	3,8,4,0,52,51,1,0,0,0,53,54,1,0,0,0,54,52,1,0,0,0,54,55,1,0,0,0,55,7,
  	1,0,0,0,56,65,5,13,0,0,57,65,5,4,0,0,58,65,5,14,0,0,59,60,5,5,0,0,60,
  	65,5,13,0,0,61,62,5,6,0,0,62,63,5,13,0,0,63,65,5,7,0,0,64,56,1,0,0,0,
  	64,57,1,0,0,0,64,58,1,0,0,0,64,59,1,0,0,0,64,61,1,0,0,0,65,9,1,0,0,0,
  	66,67,3,12,6,0,67,68,5,8,0,0,68,70,3,14,7,0,69,71,5,9,0,0,70,69,1,0,0,
  	0,70,71,1,0,0,0,71,11,1,0,0,0,72,73,5,13,0,0,73,13,1,0,0,0,74,85,5,13,
  	0,0,75,85,5,15,0,0,76,85,5,18,0,0,77,85,5,19,0,0,78,79,5,20,0,0,79,80,
  	3,16,8,0,80,81,5,10,0,0,81,85,1,0,0,0,82,85,5,16,0,0,83,85,5,17,0,0,84,
  	74,1,0,0,0,84,75,1,0,0,0,84,76,1,0,0,0,84,77,1,0,0,0,84,78,1,0,0,0,84,
  	82,1,0,0,0,84,83,1,0,0,0,85,86,1,0,0,0,86,84,1,0,0,0,86,87,1,0,0,0,87,
  	15,1,0,0,0,88,93,3,14,7,0,89,90,5,1,0,0,90,92,3,14,7,0,91,89,1,0,0,0,
  	92,95,1,0,0,0,93,91,1,0,0,0,93,94,1,0,0,0,94,17,1,0,0,0,95,93,1,0,0,0,
  	96,97,5,11,0,0,97,101,5,13,0,0,98,100,3,20,10,0,99,98,1,0,0,0,100,103,
  	1,0,0,0,101,99,1,0,0,0,101,102,1,0,0,0,102,113,1,0,0,0,103,101,1,0,0,
  	0,104,114,5,9,0,0,105,109,5,2,0,0,106,108,3,2,1,0,107,106,1,0,0,0,108,
  	111,1,0,0,0,109,107,1,0,0,0,109,110,1,0,0,0,110,112,1,0,0,0,111,109,1,
  	0,0,0,112,114,5,3,0,0,113,104,1,0,0,0,113,105,1,0,0,0,114,19,1,0,0,0,
  	115,116,7,0,0,0,116,21,1,0,0,0,13,25,32,39,46,54,64,70,84,86,93,101,109,
  	113
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  csssimpleParserStaticData = std::move(staticData);
}

}

CSSSimpleParser::CSSSimpleParser(TokenStream *input) : CSSSimpleParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

CSSSimpleParser::CSSSimpleParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  CSSSimpleParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *csssimpleParserStaticData->atn, csssimpleParserStaticData->decisionToDFA, csssimpleParserStaticData->sharedContextCache, options);
}

CSSSimpleParser::~CSSSimpleParser() {
  delete _interpreter;
}

const atn::ATN& CSSSimpleParser::getATN() const {
  return *csssimpleParserStaticData->atn;
}

std::string CSSSimpleParser::getGrammarFileName() const {
  return "CSSSimple.g4";
}

const std::vector<std::string>& CSSSimpleParser::getRuleNames() const {
  return csssimpleParserStaticData->ruleNames;
}

const dfa::Vocabulary& CSSSimpleParser::getVocabulary() const {
  return csssimpleParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView CSSSimpleParser::getSerializedATN() const {
  return csssimpleParserStaticData->serializedATN;
}


//----------------- StylesheetContext ------------------------------------------------------------------

CSSSimpleParser::StylesheetContext::StylesheetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSSimpleParser::StylesheetContext::EOF() {
  return getToken(CSSSimpleParser::EOF, 0);
}

std::vector<CSSSimpleParser::StatementContext *> CSSSimpleParser::StylesheetContext::statement() {
  return getRuleContexts<CSSSimpleParser::StatementContext>();
}

CSSSimpleParser::StatementContext* CSSSimpleParser::StylesheetContext::statement(size_t i) {
  return getRuleContext<CSSSimpleParser::StatementContext>(i);
}


size_t CSSSimpleParser::StylesheetContext::getRuleIndex() const {
  return CSSSimpleParser::RuleStylesheet;
}


std::any CSSSimpleParser::StylesheetContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitStylesheet(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::StylesheetContext* CSSSimpleParser::stylesheet() {
  StylesheetContext *_localctx = _tracker.createInstance<StylesheetContext>(_ctx, getState());
  enterRule(_localctx, 0, CSSSimpleParser::RuleStylesheet);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(25);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 26736) != 0)) {
      setState(22);
      statement();
      setState(27);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(28);
    match(CSSSimpleParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

CSSSimpleParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSSimpleParser::RulesetContext* CSSSimpleParser::StatementContext::ruleset() {
  return getRuleContext<CSSSimpleParser::RulesetContext>(0);
}

CSSSimpleParser::AtRuleContext* CSSSimpleParser::StatementContext::atRule() {
  return getRuleContext<CSSSimpleParser::AtRuleContext>(0);
}


size_t CSSSimpleParser::StatementContext::getRuleIndex() const {
  return CSSSimpleParser::RuleStatement;
}


std::any CSSSimpleParser::StatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitStatement(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::StatementContext* CSSSimpleParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 2, CSSSimpleParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(32);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSSimpleParser::T__3:
      case CSSSimpleParser::T__4:
      case CSSSimpleParser::T__5:
      case CSSSimpleParser::Ident:
      case CSSSimpleParser::Hash: {
        enterOuterAlt(_localctx, 1);
        setState(30);
        ruleset();
        break;
      }

      case CSSSimpleParser::T__10: {
        enterOuterAlt(_localctx, 2);
        setState(31);
        atRule();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RulesetContext ------------------------------------------------------------------

CSSSimpleParser::RulesetContext::RulesetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSSimpleParser::SelectorContext *> CSSSimpleParser::RulesetContext::selector() {
  return getRuleContexts<CSSSimpleParser::SelectorContext>();
}

CSSSimpleParser::SelectorContext* CSSSimpleParser::RulesetContext::selector(size_t i) {
  return getRuleContext<CSSSimpleParser::SelectorContext>(i);
}

std::vector<CSSSimpleParser::DeclarationContext *> CSSSimpleParser::RulesetContext::declaration() {
  return getRuleContexts<CSSSimpleParser::DeclarationContext>();
}

CSSSimpleParser::DeclarationContext* CSSSimpleParser::RulesetContext::declaration(size_t i) {
  return getRuleContext<CSSSimpleParser::DeclarationContext>(i);
}


size_t CSSSimpleParser::RulesetContext::getRuleIndex() const {
  return CSSSimpleParser::RuleRuleset;
}


std::any CSSSimpleParser::RulesetContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitRuleset(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::RulesetContext* CSSSimpleParser::ruleset() {
  RulesetContext *_localctx = _tracker.createInstance<RulesetContext>(_ctx, getState());
  enterRule(_localctx, 4, CSSSimpleParser::RuleRuleset);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(34);
    selector();
    setState(39);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSSSimpleParser::T__0) {
      setState(35);
      match(CSSSimpleParser::T__0);
      setState(36);
      selector();
      setState(41);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(42);
    match(CSSSimpleParser::T__1);
    setState(46);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSSSimpleParser::Ident) {
      setState(43);
      declaration();
      setState(48);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(49);
    match(CSSSimpleParser::T__2);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SelectorContext ------------------------------------------------------------------

CSSSimpleParser::SelectorContext::SelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSSimpleParser::SimpleSelectorContext *> CSSSimpleParser::SelectorContext::simpleSelector() {
  return getRuleContexts<CSSSimpleParser::SimpleSelectorContext>();
}

CSSSimpleParser::SimpleSelectorContext* CSSSimpleParser::SelectorContext::simpleSelector(size_t i) {
  return getRuleContext<CSSSimpleParser::SimpleSelectorContext>(i);
}


size_t CSSSimpleParser::SelectorContext::getRuleIndex() const {
  return CSSSimpleParser::RuleSelector;
}


std::any CSSSimpleParser::SelectorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitSelector(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::SelectorContext* CSSSimpleParser::selector() {
  SelectorContext *_localctx = _tracker.createInstance<SelectorContext>(_ctx, getState());
  enterRule(_localctx, 6, CSSSimpleParser::RuleSelector);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(52); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(51);
      simpleSelector();
      setState(54); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 24688) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SimpleSelectorContext ------------------------------------------------------------------

CSSSimpleParser::SimpleSelectorContext::SimpleSelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSSimpleParser::SimpleSelectorContext::Ident() {
  return getToken(CSSSimpleParser::Ident, 0);
}

tree::TerminalNode* CSSSimpleParser::SimpleSelectorContext::Hash() {
  return getToken(CSSSimpleParser::Hash, 0);
}


size_t CSSSimpleParser::SimpleSelectorContext::getRuleIndex() const {
  return CSSSimpleParser::RuleSimpleSelector;
}


std::any CSSSimpleParser::SimpleSelectorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitSimpleSelector(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::SimpleSelectorContext* CSSSimpleParser::simpleSelector() {
  SimpleSelectorContext *_localctx = _tracker.createInstance<SimpleSelectorContext>(_ctx, getState());
  enterRule(_localctx, 8, CSSSimpleParser::RuleSimpleSelector);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(64);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSSimpleParser::Ident: {
        setState(56);
        match(CSSSimpleParser::Ident);
        break;
      }

      case CSSSimpleParser::T__3: {
        setState(57);
        match(CSSSimpleParser::T__3);
        break;
      }

      case CSSSimpleParser::Hash: {
        setState(58);
        match(CSSSimpleParser::Hash);
        break;
      }

      case CSSSimpleParser::T__4: {
        setState(59);
        match(CSSSimpleParser::T__4);
        setState(60);
        match(CSSSimpleParser::Ident);
        break;
      }

      case CSSSimpleParser::T__5: {
        setState(61);
        match(CSSSimpleParser::T__5);
        setState(62);
        match(CSSSimpleParser::Ident);
        setState(63);
        match(CSSSimpleParser::T__6);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DeclarationContext ------------------------------------------------------------------

CSSSimpleParser::DeclarationContext::DeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSSimpleParser::PropertyContext* CSSSimpleParser::DeclarationContext::property() {
  return getRuleContext<CSSSimpleParser::PropertyContext>(0);
}

CSSSimpleParser::ValueContext* CSSSimpleParser::DeclarationContext::value() {
  return getRuleContext<CSSSimpleParser::ValueContext>(0);
}


size_t CSSSimpleParser::DeclarationContext::getRuleIndex() const {
  return CSSSimpleParser::RuleDeclaration;
}


std::any CSSSimpleParser::DeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitDeclaration(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::DeclarationContext* CSSSimpleParser::declaration() {
  DeclarationContext *_localctx = _tracker.createInstance<DeclarationContext>(_ctx, getState());
  enterRule(_localctx, 10, CSSSimpleParser::RuleDeclaration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(66);
    property();
    setState(67);
    match(CSSSimpleParser::T__7);
    setState(68);
    value();
    setState(70);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSSimpleParser::T__8) {
      setState(69);
      match(CSSSimpleParser::T__8);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyContext ------------------------------------------------------------------

CSSSimpleParser::PropertyContext::PropertyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSSimpleParser::PropertyContext::Ident() {
  return getToken(CSSSimpleParser::Ident, 0);
}


size_t CSSSimpleParser::PropertyContext::getRuleIndex() const {
  return CSSSimpleParser::RuleProperty;
}


std::any CSSSimpleParser::PropertyContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitProperty(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::PropertyContext* CSSSimpleParser::property() {
  PropertyContext *_localctx = _tracker.createInstance<PropertyContext>(_ctx, getState());
  enterRule(_localctx, 12, CSSSimpleParser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(72);
    match(CSSSimpleParser::Ident);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueContext ------------------------------------------------------------------

CSSSimpleParser::ValueContext::ValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> CSSSimpleParser::ValueContext::Ident() {
  return getTokens(CSSSimpleParser::Ident);
}

tree::TerminalNode* CSSSimpleParser::ValueContext::Ident(size_t i) {
  return getToken(CSSSimpleParser::Ident, i);
}

std::vector<tree::TerminalNode *> CSSSimpleParser::ValueContext::Number() {
  return getTokens(CSSSimpleParser::Number);
}

tree::TerminalNode* CSSSimpleParser::ValueContext::Number(size_t i) {
  return getToken(CSSSimpleParser::Number, i);
}

std::vector<tree::TerminalNode *> CSSSimpleParser::ValueContext::String_() {
  return getTokens(CSSSimpleParser::String_);
}

tree::TerminalNode* CSSSimpleParser::ValueContext::String_(size_t i) {
  return getToken(CSSSimpleParser::String_, i);
}

std::vector<tree::TerminalNode *> CSSSimpleParser::ValueContext::Color() {
  return getTokens(CSSSimpleParser::Color);
}

tree::TerminalNode* CSSSimpleParser::ValueContext::Color(size_t i) {
  return getToken(CSSSimpleParser::Color, i);
}

std::vector<tree::TerminalNode *> CSSSimpleParser::ValueContext::Function() {
  return getTokens(CSSSimpleParser::Function);
}

tree::TerminalNode* CSSSimpleParser::ValueContext::Function(size_t i) {
  return getToken(CSSSimpleParser::Function, i);
}

std::vector<CSSSimpleParser::ValueListContext *> CSSSimpleParser::ValueContext::valueList() {
  return getRuleContexts<CSSSimpleParser::ValueListContext>();
}

CSSSimpleParser::ValueListContext* CSSSimpleParser::ValueContext::valueList(size_t i) {
  return getRuleContext<CSSSimpleParser::ValueListContext>(i);
}

std::vector<tree::TerminalNode *> CSSSimpleParser::ValueContext::Percentage() {
  return getTokens(CSSSimpleParser::Percentage);
}

tree::TerminalNode* CSSSimpleParser::ValueContext::Percentage(size_t i) {
  return getToken(CSSSimpleParser::Percentage, i);
}

std::vector<tree::TerminalNode *> CSSSimpleParser::ValueContext::Dimension() {
  return getTokens(CSSSimpleParser::Dimension);
}

tree::TerminalNode* CSSSimpleParser::ValueContext::Dimension(size_t i) {
  return getToken(CSSSimpleParser::Dimension, i);
}


size_t CSSSimpleParser::ValueContext::getRuleIndex() const {
  return CSSSimpleParser::RuleValue;
}


std::any CSSSimpleParser::ValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitValue(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::ValueContext* CSSSimpleParser::value() {
  ValueContext *_localctx = _tracker.createInstance<ValueContext>(_ctx, getState());
  enterRule(_localctx, 14, CSSSimpleParser::RuleValue);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(84); 
    _errHandler->sync(this);
    alt = 1;
    do {
      switch (alt) {
        case 1: {
              setState(84);
              _errHandler->sync(this);
              switch (_input->LA(1)) {
                case CSSSimpleParser::Ident: {
                  setState(74);
                  match(CSSSimpleParser::Ident);
                  break;
                }

                case CSSSimpleParser::Number: {
                  setState(75);
                  match(CSSSimpleParser::Number);
                  break;
                }

                case CSSSimpleParser::String_: {
                  setState(76);
                  match(CSSSimpleParser::String_);
                  break;
                }

                case CSSSimpleParser::Color: {
                  setState(77);
                  match(CSSSimpleParser::Color);
                  break;
                }

                case CSSSimpleParser::Function: {
                  setState(78);
                  match(CSSSimpleParser::Function);
                  setState(79);
                  valueList();
                  setState(80);
                  match(CSSSimpleParser::T__9);
                  break;
                }

                case CSSSimpleParser::Percentage: {
                  setState(82);
                  match(CSSSimpleParser::Percentage);
                  break;
                }

                case CSSSimpleParser::Dimension: {
                  setState(83);
                  match(CSSSimpleParser::Dimension);
                  break;
                }

              default:
                throw NoViableAltException(this);
              }
              break;
            }

      default:
        throw NoViableAltException(this);
      }
      setState(86); 
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx);
    } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueListContext ------------------------------------------------------------------

CSSSimpleParser::ValueListContext::ValueListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSSimpleParser::ValueContext *> CSSSimpleParser::ValueListContext::value() {
  return getRuleContexts<CSSSimpleParser::ValueContext>();
}

CSSSimpleParser::ValueContext* CSSSimpleParser::ValueListContext::value(size_t i) {
  return getRuleContext<CSSSimpleParser::ValueContext>(i);
}


size_t CSSSimpleParser::ValueListContext::getRuleIndex() const {
  return CSSSimpleParser::RuleValueList;
}


std::any CSSSimpleParser::ValueListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitValueList(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::ValueListContext* CSSSimpleParser::valueList() {
  ValueListContext *_localctx = _tracker.createInstance<ValueListContext>(_ctx, getState());
  enterRule(_localctx, 16, CSSSimpleParser::RuleValueList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(88);
    value();
    setState(93);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSSSimpleParser::T__0) {
      setState(89);
      match(CSSSimpleParser::T__0);
      setState(90);
      value();
      setState(95);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AtRuleContext ------------------------------------------------------------------

CSSSimpleParser::AtRuleContext::AtRuleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSSimpleParser::AtRuleContext::Ident() {
  return getToken(CSSSimpleParser::Ident, 0);
}

std::vector<CSSSimpleParser::AnyContext *> CSSSimpleParser::AtRuleContext::any() {
  return getRuleContexts<CSSSimpleParser::AnyContext>();
}

CSSSimpleParser::AnyContext* CSSSimpleParser::AtRuleContext::any(size_t i) {
  return getRuleContext<CSSSimpleParser::AnyContext>(i);
}

std::vector<CSSSimpleParser::StatementContext *> CSSSimpleParser::AtRuleContext::statement() {
  return getRuleContexts<CSSSimpleParser::StatementContext>();
}

CSSSimpleParser::StatementContext* CSSSimpleParser::AtRuleContext::statement(size_t i) {
  return getRuleContext<CSSSimpleParser::StatementContext>(i);
}


size_t CSSSimpleParser::AtRuleContext::getRuleIndex() const {
  return CSSSimpleParser::RuleAtRule;
}


std::any CSSSimpleParser::AtRuleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitAtRule(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::AtRuleContext* CSSSimpleParser::atRule() {
  AtRuleContext *_localctx = _tracker.createInstance<AtRuleContext>(_ctx, getState());
  enterRule(_localctx, 18, CSSSimpleParser::RuleAtRule);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(96);
    match(CSSSimpleParser::T__10);
    setState(97);
    match(CSSSimpleParser::Ident);
    setState(101);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(98);
        any(); 
      }
      setState(103);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx);
    }
    setState(113);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSSimpleParser::T__8: {
        setState(104);
        match(CSSSimpleParser::T__8);
        break;
      }

      case CSSSimpleParser::T__1: {
        setState(105);
        match(CSSSimpleParser::T__1);
        setState(109);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 26736) != 0)) {
          setState(106);
          statement();
          setState(111);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(112);
        match(CSSSimpleParser::T__2);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AnyContext ------------------------------------------------------------------

CSSSimpleParser::AnyContext::AnyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSSimpleParser::AnyContext::Ident() {
  return getToken(CSSSimpleParser::Ident, 0);
}

tree::TerminalNode* CSSSimpleParser::AnyContext::Number() {
  return getToken(CSSSimpleParser::Number, 0);
}

tree::TerminalNode* CSSSimpleParser::AnyContext::String_() {
  return getToken(CSSSimpleParser::String_, 0);
}

tree::TerminalNode* CSSSimpleParser::AnyContext::Color() {
  return getToken(CSSSimpleParser::Color, 0);
}


size_t CSSSimpleParser::AnyContext::getRuleIndex() const {
  return CSSSimpleParser::RuleAny;
}


std::any CSSSimpleParser::AnyContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CSSSimpleVisitor*>(visitor))
    return parserVisitor->visitAny(this);
  else
    return visitor->visitChildren(this);
}

CSSSimpleParser::AnyContext* CSSSimpleParser::any() {
  AnyContext *_localctx = _tracker.createInstance<AnyContext>(_ctx, getState());
  enterRule(_localctx, 20, CSSSimpleParser::RuleAny);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(115);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 833474) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void CSSSimpleParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  csssimpleParserInitialize();
#else
  ::antlr4::internal::call_once(csssimpleParserOnceFlag, csssimpleParserInitialize);
#endif
}
