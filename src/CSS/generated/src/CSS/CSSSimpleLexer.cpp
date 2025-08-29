
// Generated from src/CSS/CSSSimple.g4 by ANTLR 4.13.2


#include "CSSSimpleLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct CSSSimpleLexerStaticData final {
  CSSSimpleLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  CSSSimpleLexerStaticData(const CSSSimpleLexerStaticData&) = delete;
  CSSSimpleLexerStaticData(CSSSimpleLexerStaticData&&) = delete;
  CSSSimpleLexerStaticData& operator=(const CSSSimpleLexerStaticData&) = delete;
  CSSSimpleLexerStaticData& operator=(CSSSimpleLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag csssimplelexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<CSSSimpleLexerStaticData> csssimplelexerLexerStaticData = nullptr;

void csssimplelexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (csssimplelexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(csssimplelexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CSSSimpleLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
      "T__9", "T__10", "T__11", "Ident", "Hash", "Number", "Percentage", 
      "Dimension", "String_", "Color", "Function", "Comment", "WS"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
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
  	4,0,22,155,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,1,0,1,0,1,1,1,1,1,2,1,2,1,3,1,3,1,4,1,4,1,5,1,5,1,6,1,6,1,7,1,7,
  	1,8,1,8,1,9,1,9,1,10,1,10,1,11,1,11,1,12,1,12,5,12,72,8,12,10,12,12,12,
  	75,9,12,1,13,1,13,4,13,79,8,13,11,13,12,13,80,1,14,4,14,84,8,14,11,14,
  	12,14,85,1,14,1,14,4,14,90,8,14,11,14,12,14,91,3,14,94,8,14,1,15,1,15,
  	1,15,1,16,1,16,4,16,101,8,16,11,16,12,16,102,1,17,1,17,5,17,107,8,17,
  	10,17,12,17,110,9,17,1,17,1,17,1,17,5,17,115,8,17,10,17,12,17,118,9,17,
  	1,17,3,17,121,8,17,1,18,1,18,1,18,1,18,1,18,1,18,1,18,3,18,130,8,18,1,
  	19,1,19,1,19,1,20,1,20,1,20,1,20,5,20,139,8,20,10,20,12,20,142,9,20,1,
  	20,1,20,1,20,1,20,1,20,1,21,4,21,150,8,21,11,21,12,21,151,1,21,1,21,1,
  	140,0,22,1,1,3,2,5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,10,21,11,23,12,25,
  	13,27,14,29,15,31,16,33,17,35,18,37,19,39,20,41,21,43,22,1,0,9,3,0,65,
  	90,95,95,97,122,5,0,45,45,48,57,65,90,95,95,97,122,3,0,48,57,65,90,97,
  	122,1,0,48,57,2,0,65,90,97,122,3,0,10,10,13,13,34,34,3,0,10,10,13,13,
  	39,39,3,0,48,57,65,70,97,102,3,0,9,10,13,13,32,32,166,0,1,1,0,0,0,0,3,
  	1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,
  	0,15,1,0,0,0,0,17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,0,25,
  	1,0,0,0,0,27,1,0,0,0,0,29,1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,0,35,1,0,
  	0,0,0,37,1,0,0,0,0,39,1,0,0,0,0,41,1,0,0,0,0,43,1,0,0,0,1,45,1,0,0,0,
  	3,47,1,0,0,0,5,49,1,0,0,0,7,51,1,0,0,0,9,53,1,0,0,0,11,55,1,0,0,0,13,
  	57,1,0,0,0,15,59,1,0,0,0,17,61,1,0,0,0,19,63,1,0,0,0,21,65,1,0,0,0,23,
  	67,1,0,0,0,25,69,1,0,0,0,27,76,1,0,0,0,29,83,1,0,0,0,31,95,1,0,0,0,33,
  	98,1,0,0,0,35,120,1,0,0,0,37,122,1,0,0,0,39,131,1,0,0,0,41,134,1,0,0,
  	0,43,149,1,0,0,0,45,46,5,44,0,0,46,2,1,0,0,0,47,48,5,123,0,0,48,4,1,0,
  	0,0,49,50,5,125,0,0,50,6,1,0,0,0,51,52,5,42,0,0,52,8,1,0,0,0,53,54,5,
  	46,0,0,54,10,1,0,0,0,55,56,5,91,0,0,56,12,1,0,0,0,57,58,5,93,0,0,58,14,
  	1,0,0,0,59,60,5,58,0,0,60,16,1,0,0,0,61,62,5,59,0,0,62,18,1,0,0,0,63,
  	64,5,41,0,0,64,20,1,0,0,0,65,66,5,64,0,0,66,22,1,0,0,0,67,68,5,40,0,0,
  	68,24,1,0,0,0,69,73,7,0,0,0,70,72,7,1,0,0,71,70,1,0,0,0,72,75,1,0,0,0,
  	73,71,1,0,0,0,73,74,1,0,0,0,74,26,1,0,0,0,75,73,1,0,0,0,76,78,5,35,0,
  	0,77,79,7,2,0,0,78,77,1,0,0,0,79,80,1,0,0,0,80,78,1,0,0,0,80,81,1,0,0,
  	0,81,28,1,0,0,0,82,84,7,3,0,0,83,82,1,0,0,0,84,85,1,0,0,0,85,83,1,0,0,
  	0,85,86,1,0,0,0,86,93,1,0,0,0,87,89,5,46,0,0,88,90,7,3,0,0,89,88,1,0,
  	0,0,90,91,1,0,0,0,91,89,1,0,0,0,91,92,1,0,0,0,92,94,1,0,0,0,93,87,1,0,
  	0,0,93,94,1,0,0,0,94,30,1,0,0,0,95,96,3,29,14,0,96,97,5,37,0,0,97,32,
  	1,0,0,0,98,100,3,29,14,0,99,101,7,4,0,0,100,99,1,0,0,0,101,102,1,0,0,
  	0,102,100,1,0,0,0,102,103,1,0,0,0,103,34,1,0,0,0,104,108,5,34,0,0,105,
  	107,8,5,0,0,106,105,1,0,0,0,107,110,1,0,0,0,108,106,1,0,0,0,108,109,1,
  	0,0,0,109,111,1,0,0,0,110,108,1,0,0,0,111,121,5,34,0,0,112,116,5,39,0,
  	0,113,115,8,6,0,0,114,113,1,0,0,0,115,118,1,0,0,0,116,114,1,0,0,0,116,
  	117,1,0,0,0,117,119,1,0,0,0,118,116,1,0,0,0,119,121,5,39,0,0,120,104,
  	1,0,0,0,120,112,1,0,0,0,121,36,1,0,0,0,122,123,5,35,0,0,123,124,7,7,0,
  	0,124,125,7,7,0,0,125,129,7,7,0,0,126,127,7,7,0,0,127,128,7,7,0,0,128,
  	130,7,7,0,0,129,126,1,0,0,0,129,130,1,0,0,0,130,38,1,0,0,0,131,132,3,
  	25,12,0,132,133,5,40,0,0,133,40,1,0,0,0,134,135,5,47,0,0,135,136,5,42,
  	0,0,136,140,1,0,0,0,137,139,9,0,0,0,138,137,1,0,0,0,139,142,1,0,0,0,140,
  	141,1,0,0,0,140,138,1,0,0,0,141,143,1,0,0,0,142,140,1,0,0,0,143,144,5,
  	42,0,0,144,145,5,47,0,0,145,146,1,0,0,0,146,147,6,20,0,0,147,42,1,0,0,
  	0,148,150,7,8,0,0,149,148,1,0,0,0,150,151,1,0,0,0,151,149,1,0,0,0,151,
  	152,1,0,0,0,152,153,1,0,0,0,153,154,6,21,0,0,154,44,1,0,0,0,13,0,73,80,
  	85,91,93,102,108,116,120,129,140,151,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  csssimplelexerLexerStaticData = std::move(staticData);
}

}

CSSSimpleLexer::CSSSimpleLexer(CharStream *input) : Lexer(input) {
  CSSSimpleLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *csssimplelexerLexerStaticData->atn, csssimplelexerLexerStaticData->decisionToDFA, csssimplelexerLexerStaticData->sharedContextCache);
}

CSSSimpleLexer::~CSSSimpleLexer() {
  delete _interpreter;
}

std::string CSSSimpleLexer::getGrammarFileName() const {
  return "CSSSimple.g4";
}

const std::vector<std::string>& CSSSimpleLexer::getRuleNames() const {
  return csssimplelexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& CSSSimpleLexer::getChannelNames() const {
  return csssimplelexerLexerStaticData->channelNames;
}

const std::vector<std::string>& CSSSimpleLexer::getModeNames() const {
  return csssimplelexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& CSSSimpleLexer::getVocabulary() const {
  return csssimplelexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView CSSSimpleLexer::getSerializedATN() const {
  return csssimplelexerLexerStaticData->serializedATN;
}

const atn::ATN& CSSSimpleLexer::getATN() const {
  return *csssimplelexerLexerStaticData->atn;
}




void CSSSimpleLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  csssimplelexerLexerInitialize();
#else
  ::antlr4::internal::call_once(csssimplelexerLexerOnceFlag, csssimplelexerLexerInitialize);
#endif
}
