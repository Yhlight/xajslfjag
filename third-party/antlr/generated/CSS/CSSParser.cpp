
// Generated from third-party/antlr/Grammars/CSS3/CSSParser.g4 by ANTLR 4.13.2


#include "CSSParserListener.h"

#include "CSSParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct CSSParserStaticData final {
  CSSParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  CSSParserStaticData(const CSSParserStaticData&) = delete;
  CSSParserStaticData(CSSParserStaticData&&) = delete;
  CSSParserStaticData& operator=(const CSSParserStaticData&) = delete;
  CSSParserStaticData& operator=(CSSParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag cssparserParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<CSSParserStaticData> cssparserParserStaticData = nullptr;

void cssparserParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (cssparserParserStaticData != nullptr) {
    return;
  }
#else
  assert(cssparserParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CSSParserStaticData>(
    std::vector<std::string>{
      "stylesheet", "charset", "import_rule", "namespace_rule", "nested_statement", 
      "unknown_at_rule", "media_rule", "media_query_list", "media_query", 
      "media_type", "media_expression", "media_feature", "page_rule", "pseudo_page", 
      "font_face_rule", "keyframes_rule", "keyframe_block", "keyframe_selector", 
      "supports_rule", "supports_condition", "supports_condition_in_parens", 
      "supports_negation", "supports_conjunction", "supports_disjunction", 
      "supports_declaration_condition", "general_enclosed", "ruleset", "selector_group", 
      "selector", "combinator", "simple_selector_sequence", "type_selector", 
      "namespace_prefix", "element_name", "hash", "class", "attrib", "pseudo", 
      "functional_pseudo", "negation", "negation_arg", "declaration_list", 
      "declaration", "prio", "property", "expr", "operator", "term", "unary_operator", 
      "function", "any"
    },
    std::vector<std::string>{
      "", "'{'", "'}'", "'('", "')'", "'['", "']'", "';'", "':'", "','", 
      "'.'", "'#'", "'@'", "'+'", "'-'", "'*'", "'/'", "'='", "'~='", "'|='", 
      "'^='", "'$='", "'*='", "", "", "", "", "", "", "", "", "", "", "'<!--'", 
      "'-->'", "'only'", "'not'", "'and'", "'@charset'", "'@import'", "'@namespace'", 
      "'@media'", "'@page'", "'@font-face'", "'@keyframes'", "'@supports'", 
      "'from'", "'to'", "'or'", "'~'", "'|'", "':not('"
    },
    std::vector<std::string>{
      "", "LBRACE", "RBRACE", "LPAREN", "RPAREN", "LBRACKET", "RBRACKET", 
      "SEMICOLON", "COLON", "COMMA", "DOT", "HASH", "AT", "PLUS", "MINUS", 
      "MULTIPLY", "DIVIDE", "EQUALS", "INCLUDES", "DASHMATCH", "PREFIXMATCH", 
      "SUFFIXMATCH", "SUBSTRINGMATCH", "STRING", "NUMBER", "PERCENTAGE", 
      "IDENT", "DIMENSION", "FUNCTION", "URI", "COMMENT", "WS", "IMPORTANT", 
      "CDO", "CDC", "ONLY_KEYWORD", "NOT_KEYWORD", "AND_KEYWORD", "AT_CHARSET", 
      "AT_IMPORT", "AT_NAMESPACE", "AT_MEDIA", "AT_PAGE", "AT_FONT_FACE", 
      "AT_KEYFRAMES", "AT_SUPPORTS", "FROM_KEYWORD", "TO_KEYWORD", "OR_KEYWORD", 
      "TILDE", "PIPE", "NOT_FUNCTION", "ANYCHAR"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,52,509,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,1,0,1,0,1,0,1,0,5,0,107,8,0,10,0,12,0,110,9,0,1,0,1,0,1,
  	1,1,1,1,1,1,1,1,2,1,2,1,2,3,2,121,8,2,1,2,1,2,1,3,1,3,3,3,127,8,3,1,3,
  	1,3,1,3,1,4,1,4,1,4,1,4,1,4,1,4,1,4,3,4,139,8,4,1,5,1,5,1,5,5,5,144,8,
  	5,10,5,12,5,147,9,5,1,5,1,5,5,5,151,8,5,10,5,12,5,154,9,5,1,5,1,5,3,5,
  	158,8,5,1,6,1,6,1,6,1,6,5,6,164,8,6,10,6,12,6,167,9,6,1,6,1,6,1,7,1,7,
  	1,7,5,7,174,8,7,10,7,12,7,177,9,7,1,8,3,8,180,8,8,1,8,1,8,1,8,5,8,185,
  	8,8,10,8,12,8,188,9,8,1,8,1,8,1,8,5,8,193,8,8,10,8,12,8,196,9,8,3,8,198,
  	8,8,1,9,1,9,1,10,1,10,1,10,1,10,3,10,206,8,10,1,10,1,10,1,11,1,11,1,12,
  	1,12,3,12,214,8,12,1,12,1,12,1,12,1,12,1,13,1,13,1,13,1,14,1,14,1,14,
  	1,14,1,14,1,15,1,15,1,15,1,15,5,15,232,8,15,10,15,12,15,235,9,15,1,15,
  	1,15,1,16,1,16,1,16,1,16,1,16,1,17,1,17,1,17,5,17,247,8,17,10,17,12,17,
  	250,9,17,1,18,1,18,1,18,1,18,5,18,256,8,18,10,18,12,18,259,9,18,1,18,
  	1,18,1,19,1,19,1,19,1,19,3,19,267,8,19,1,20,1,20,1,20,1,20,1,20,1,20,
  	3,20,275,8,20,1,21,1,21,1,21,1,22,1,22,1,22,4,22,283,8,22,11,22,12,22,
  	284,1,23,1,23,1,23,4,23,290,8,23,11,23,12,23,291,1,24,1,24,1,24,1,24,
  	1,25,1,25,5,25,300,8,25,10,25,12,25,303,9,25,1,25,1,25,1,26,1,26,1,26,
  	1,26,1,26,1,27,1,27,1,27,5,27,315,8,27,10,27,12,27,318,9,27,1,28,1,28,
  	1,28,1,28,5,28,324,8,28,10,28,12,28,327,9,28,1,29,1,29,1,30,1,30,1,30,
  	1,30,1,30,1,30,5,30,337,8,30,10,30,12,30,340,9,30,1,30,1,30,1,30,1,30,
  	1,30,4,30,347,8,30,11,30,12,30,348,3,30,351,8,30,1,31,3,31,354,8,31,1,
  	31,1,31,3,31,358,8,31,1,31,3,31,361,8,31,1,32,3,32,364,8,32,1,32,1,32,
  	1,33,1,33,1,34,1,34,1,34,1,35,1,35,1,35,1,36,1,36,1,36,1,36,3,36,380,
  	8,36,1,36,1,36,1,37,1,37,3,37,386,8,37,1,37,1,37,3,37,390,8,37,1,38,1,
  	38,1,38,1,38,1,39,1,39,1,39,1,39,1,40,1,40,1,40,1,40,1,40,3,40,405,8,
  	40,1,41,3,41,408,8,41,1,41,1,41,3,41,412,8,41,5,41,414,8,41,10,41,12,
  	41,417,9,41,1,42,1,42,1,42,1,42,3,42,423,8,42,1,43,1,43,1,44,1,44,1,45,
  	1,45,1,45,1,45,5,45,433,8,45,10,45,12,45,436,9,45,1,46,1,46,1,46,3,46,
  	441,8,46,1,47,3,47,444,8,47,1,47,1,47,1,47,1,47,1,47,1,47,1,47,1,47,3,
  	47,454,8,47,1,47,3,47,457,8,47,1,48,1,48,1,49,1,49,1,49,1,49,1,50,1,50,
  	1,50,1,50,1,50,1,50,1,50,1,50,1,50,1,50,1,50,1,50,1,50,1,50,1,50,1,50,
  	1,50,1,50,1,50,5,50,484,8,50,10,50,12,50,487,9,50,1,50,1,50,1,50,5,50,
  	492,8,50,10,50,12,50,495,9,50,1,50,1,50,1,50,5,50,500,8,50,10,50,12,50,
  	503,9,50,1,50,1,50,3,50,507,8,50,1,50,0,0,51,0,2,4,6,8,10,12,14,16,18,
  	20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,
  	66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,0,9,2,0,23,23,
  	29,29,1,0,35,36,2,0,25,25,46,47,2,0,3,3,28,28,2,0,13,15,49,49,2,0,15,
  	15,26,26,1,0,17,22,2,0,23,23,26,26,1,0,13,14,554,0,108,1,0,0,0,2,113,
  	1,0,0,0,4,117,1,0,0,0,6,124,1,0,0,0,8,138,1,0,0,0,10,140,1,0,0,0,12,159,
  	1,0,0,0,14,170,1,0,0,0,16,197,1,0,0,0,18,199,1,0,0,0,20,201,1,0,0,0,22,
  	209,1,0,0,0,24,211,1,0,0,0,26,219,1,0,0,0,28,222,1,0,0,0,30,227,1,0,0,
  	0,32,238,1,0,0,0,34,243,1,0,0,0,36,251,1,0,0,0,38,266,1,0,0,0,40,274,
  	1,0,0,0,42,276,1,0,0,0,44,279,1,0,0,0,46,286,1,0,0,0,48,293,1,0,0,0,50,
  	297,1,0,0,0,52,306,1,0,0,0,54,311,1,0,0,0,56,319,1,0,0,0,58,328,1,0,0,
  	0,60,350,1,0,0,0,62,360,1,0,0,0,64,363,1,0,0,0,66,367,1,0,0,0,68,369,
  	1,0,0,0,70,372,1,0,0,0,72,375,1,0,0,0,74,383,1,0,0,0,76,391,1,0,0,0,78,
  	395,1,0,0,0,80,404,1,0,0,0,82,407,1,0,0,0,84,418,1,0,0,0,86,424,1,0,0,
  	0,88,426,1,0,0,0,90,428,1,0,0,0,92,440,1,0,0,0,94,443,1,0,0,0,96,458,
  	1,0,0,0,98,460,1,0,0,0,100,506,1,0,0,0,102,107,3,2,1,0,103,107,3,4,2,
  	0,104,107,3,6,3,0,105,107,3,8,4,0,106,102,1,0,0,0,106,103,1,0,0,0,106,
  	104,1,0,0,0,106,105,1,0,0,0,107,110,1,0,0,0,108,106,1,0,0,0,108,109,1,
  	0,0,0,109,111,1,0,0,0,110,108,1,0,0,0,111,112,5,0,0,1,112,1,1,0,0,0,113,
  	114,5,38,0,0,114,115,5,23,0,0,115,116,5,7,0,0,116,3,1,0,0,0,117,118,5,
  	39,0,0,118,120,7,0,0,0,119,121,3,14,7,0,120,119,1,0,0,0,120,121,1,0,0,
  	0,121,122,1,0,0,0,122,123,5,7,0,0,123,5,1,0,0,0,124,126,5,40,0,0,125,
  	127,5,26,0,0,126,125,1,0,0,0,126,127,1,0,0,0,127,128,1,0,0,0,128,129,
  	7,0,0,0,129,130,5,7,0,0,130,7,1,0,0,0,131,139,3,52,26,0,132,139,3,12,
  	6,0,133,139,3,24,12,0,134,139,3,28,14,0,135,139,3,30,15,0,136,139,3,36,
  	18,0,137,139,3,10,5,0,138,131,1,0,0,0,138,132,1,0,0,0,138,133,1,0,0,0,
  	138,134,1,0,0,0,138,135,1,0,0,0,138,136,1,0,0,0,138,137,1,0,0,0,139,9,
  	1,0,0,0,140,141,5,12,0,0,141,145,5,26,0,0,142,144,3,100,50,0,143,142,
  	1,0,0,0,144,147,1,0,0,0,145,143,1,0,0,0,145,146,1,0,0,0,146,157,1,0,0,
  	0,147,145,1,0,0,0,148,152,5,1,0,0,149,151,3,100,50,0,150,149,1,0,0,0,
  	151,154,1,0,0,0,152,150,1,0,0,0,152,153,1,0,0,0,153,155,1,0,0,0,154,152,
  	1,0,0,0,155,158,5,2,0,0,156,158,5,7,0,0,157,148,1,0,0,0,157,156,1,0,0,
  	0,158,11,1,0,0,0,159,160,5,41,0,0,160,161,3,14,7,0,161,165,5,1,0,0,162,
  	164,3,8,4,0,163,162,1,0,0,0,164,167,1,0,0,0,165,163,1,0,0,0,165,166,1,
  	0,0,0,166,168,1,0,0,0,167,165,1,0,0,0,168,169,5,2,0,0,169,13,1,0,0,0,
  	170,175,3,16,8,0,171,172,5,9,0,0,172,174,3,16,8,0,173,171,1,0,0,0,174,
  	177,1,0,0,0,175,173,1,0,0,0,175,176,1,0,0,0,176,15,1,0,0,0,177,175,1,
  	0,0,0,178,180,7,1,0,0,179,178,1,0,0,0,179,180,1,0,0,0,180,181,1,0,0,0,
  	181,186,3,18,9,0,182,183,5,37,0,0,183,185,3,20,10,0,184,182,1,0,0,0,185,
  	188,1,0,0,0,186,184,1,0,0,0,186,187,1,0,0,0,187,198,1,0,0,0,188,186,1,
  	0,0,0,189,194,3,20,10,0,190,191,5,37,0,0,191,193,3,20,10,0,192,190,1,
  	0,0,0,193,196,1,0,0,0,194,192,1,0,0,0,194,195,1,0,0,0,195,198,1,0,0,0,
  	196,194,1,0,0,0,197,179,1,0,0,0,197,189,1,0,0,0,198,17,1,0,0,0,199,200,
  	5,26,0,0,200,19,1,0,0,0,201,202,5,3,0,0,202,205,3,22,11,0,203,204,5,8,
  	0,0,204,206,3,90,45,0,205,203,1,0,0,0,205,206,1,0,0,0,206,207,1,0,0,0,
  	207,208,5,4,0,0,208,21,1,0,0,0,209,210,5,26,0,0,210,23,1,0,0,0,211,213,
  	5,42,0,0,212,214,3,26,13,0,213,212,1,0,0,0,213,214,1,0,0,0,214,215,1,
  	0,0,0,215,216,5,1,0,0,216,217,3,82,41,0,217,218,5,2,0,0,218,25,1,0,0,
  	0,219,220,5,8,0,0,220,221,5,26,0,0,221,27,1,0,0,0,222,223,5,43,0,0,223,
  	224,5,1,0,0,224,225,3,82,41,0,225,226,5,2,0,0,226,29,1,0,0,0,227,228,
  	5,44,0,0,228,229,5,26,0,0,229,233,5,1,0,0,230,232,3,32,16,0,231,230,1,
  	0,0,0,232,235,1,0,0,0,233,231,1,0,0,0,233,234,1,0,0,0,234,236,1,0,0,0,
  	235,233,1,0,0,0,236,237,5,2,0,0,237,31,1,0,0,0,238,239,3,34,17,0,239,
  	240,5,1,0,0,240,241,3,82,41,0,241,242,5,2,0,0,242,33,1,0,0,0,243,248,
  	7,2,0,0,244,245,5,9,0,0,245,247,7,2,0,0,246,244,1,0,0,0,247,250,1,0,0,
  	0,248,246,1,0,0,0,248,249,1,0,0,0,249,35,1,0,0,0,250,248,1,0,0,0,251,
  	252,5,45,0,0,252,253,3,38,19,0,253,257,5,1,0,0,254,256,3,8,4,0,255,254,
  	1,0,0,0,256,259,1,0,0,0,257,255,1,0,0,0,257,258,1,0,0,0,258,260,1,0,0,
  	0,259,257,1,0,0,0,260,261,5,2,0,0,261,37,1,0,0,0,262,267,3,42,21,0,263,
  	267,3,44,22,0,264,267,3,46,23,0,265,267,3,40,20,0,266,262,1,0,0,0,266,
  	263,1,0,0,0,266,264,1,0,0,0,266,265,1,0,0,0,267,39,1,0,0,0,268,269,5,
  	3,0,0,269,270,3,38,19,0,270,271,5,4,0,0,271,275,1,0,0,0,272,275,3,48,
  	24,0,273,275,3,50,25,0,274,268,1,0,0,0,274,272,1,0,0,0,274,273,1,0,0,
  	0,275,41,1,0,0,0,276,277,5,36,0,0,277,278,3,40,20,0,278,43,1,0,0,0,279,
  	282,3,40,20,0,280,281,5,37,0,0,281,283,3,40,20,0,282,280,1,0,0,0,283,
  	284,1,0,0,0,284,282,1,0,0,0,284,285,1,0,0,0,285,45,1,0,0,0,286,289,3,
  	40,20,0,287,288,5,48,0,0,288,290,3,40,20,0,289,287,1,0,0,0,290,291,1,
  	0,0,0,291,289,1,0,0,0,291,292,1,0,0,0,292,47,1,0,0,0,293,294,5,3,0,0,
  	294,295,3,84,42,0,295,296,5,4,0,0,296,49,1,0,0,0,297,301,7,3,0,0,298,
  	300,3,100,50,0,299,298,1,0,0,0,300,303,1,0,0,0,301,299,1,0,0,0,301,302,
  	1,0,0,0,302,304,1,0,0,0,303,301,1,0,0,0,304,305,5,4,0,0,305,51,1,0,0,
  	0,306,307,3,54,27,0,307,308,5,1,0,0,308,309,3,82,41,0,309,310,5,2,0,0,
  	310,53,1,0,0,0,311,316,3,56,28,0,312,313,5,9,0,0,313,315,3,56,28,0,314,
  	312,1,0,0,0,315,318,1,0,0,0,316,314,1,0,0,0,316,317,1,0,0,0,317,55,1,
  	0,0,0,318,316,1,0,0,0,319,325,3,60,30,0,320,321,3,58,29,0,321,322,3,60,
  	30,0,322,324,1,0,0,0,323,320,1,0,0,0,324,327,1,0,0,0,325,323,1,0,0,0,
  	325,326,1,0,0,0,326,57,1,0,0,0,327,325,1,0,0,0,328,329,7,4,0,0,329,59,
  	1,0,0,0,330,338,3,62,31,0,331,337,3,68,34,0,332,337,3,70,35,0,333,337,
  	3,72,36,0,334,337,3,74,37,0,335,337,3,78,39,0,336,331,1,0,0,0,336,332,
  	1,0,0,0,336,333,1,0,0,0,336,334,1,0,0,0,336,335,1,0,0,0,337,340,1,0,0,
  	0,338,336,1,0,0,0,338,339,1,0,0,0,339,351,1,0,0,0,340,338,1,0,0,0,341,
  	347,3,68,34,0,342,347,3,70,35,0,343,347,3,72,36,0,344,347,3,74,37,0,345,
  	347,3,78,39,0,346,341,1,0,0,0,346,342,1,0,0,0,346,343,1,0,0,0,346,344,
  	1,0,0,0,346,345,1,0,0,0,347,348,1,0,0,0,348,346,1,0,0,0,348,349,1,0,0,
  	0,349,351,1,0,0,0,350,330,1,0,0,0,350,346,1,0,0,0,351,61,1,0,0,0,352,
  	354,3,64,32,0,353,352,1,0,0,0,353,354,1,0,0,0,354,355,1,0,0,0,355,361,
  	3,66,33,0,356,358,3,64,32,0,357,356,1,0,0,0,357,358,1,0,0,0,358,359,1,
  	0,0,0,359,361,5,15,0,0,360,353,1,0,0,0,360,357,1,0,0,0,361,63,1,0,0,0,
  	362,364,7,5,0,0,363,362,1,0,0,0,363,364,1,0,0,0,364,365,1,0,0,0,365,366,
  	5,50,0,0,366,65,1,0,0,0,367,368,5,26,0,0,368,67,1,0,0,0,369,370,5,11,
  	0,0,370,371,5,26,0,0,371,69,1,0,0,0,372,373,5,10,0,0,373,374,5,26,0,0,
  	374,71,1,0,0,0,375,376,5,5,0,0,376,379,5,26,0,0,377,378,7,6,0,0,378,380,
  	7,7,0,0,379,377,1,0,0,0,379,380,1,0,0,0,380,381,1,0,0,0,381,382,5,6,0,
  	0,382,73,1,0,0,0,383,385,5,8,0,0,384,386,5,8,0,0,385,384,1,0,0,0,385,
  	386,1,0,0,0,386,389,1,0,0,0,387,390,5,26,0,0,388,390,3,76,38,0,389,387,
  	1,0,0,0,389,388,1,0,0,0,390,75,1,0,0,0,391,392,5,28,0,0,392,393,3,90,
  	45,0,393,394,5,4,0,0,394,77,1,0,0,0,395,396,5,51,0,0,396,397,3,80,40,
  	0,397,398,5,4,0,0,398,79,1,0,0,0,399,405,3,62,31,0,400,405,3,68,34,0,
  	401,405,3,70,35,0,402,405,3,72,36,0,403,405,3,74,37,0,404,399,1,0,0,0,
  	404,400,1,0,0,0,404,401,1,0,0,0,404,402,1,0,0,0,404,403,1,0,0,0,405,81,
  	1,0,0,0,406,408,3,84,42,0,407,406,1,0,0,0,407,408,1,0,0,0,408,415,1,0,
  	0,0,409,411,5,7,0,0,410,412,3,84,42,0,411,410,1,0,0,0,411,412,1,0,0,0,
  	412,414,1,0,0,0,413,409,1,0,0,0,414,417,1,0,0,0,415,413,1,0,0,0,415,416,
  	1,0,0,0,416,83,1,0,0,0,417,415,1,0,0,0,418,419,3,88,44,0,419,420,5,8,
  	0,0,420,422,3,90,45,0,421,423,3,86,43,0,422,421,1,0,0,0,422,423,1,0,0,
  	0,423,85,1,0,0,0,424,425,5,32,0,0,425,87,1,0,0,0,426,427,5,26,0,0,427,
  	89,1,0,0,0,428,434,3,94,47,0,429,430,3,92,46,0,430,431,3,94,47,0,431,
  	433,1,0,0,0,432,429,1,0,0,0,433,436,1,0,0,0,434,432,1,0,0,0,434,435,1,
  	0,0,0,435,91,1,0,0,0,436,434,1,0,0,0,437,441,5,16,0,0,438,441,5,9,0,0,
  	439,441,1,0,0,0,440,437,1,0,0,0,440,438,1,0,0,0,440,439,1,0,0,0,441,93,
  	1,0,0,0,442,444,3,96,48,0,443,442,1,0,0,0,443,444,1,0,0,0,444,456,1,0,
  	0,0,445,457,5,24,0,0,446,457,5,25,0,0,447,457,5,27,0,0,448,457,5,23,0,
  	0,449,457,5,26,0,0,450,457,5,29,0,0,451,453,5,11,0,0,452,454,5,26,0,0,
  	453,452,1,0,0,0,453,454,1,0,0,0,454,457,1,0,0,0,455,457,3,98,49,0,456,
  	445,1,0,0,0,456,446,1,0,0,0,456,447,1,0,0,0,456,448,1,0,0,0,456,449,1,
  	0,0,0,456,450,1,0,0,0,456,451,1,0,0,0,456,455,1,0,0,0,457,95,1,0,0,0,
  	458,459,7,8,0,0,459,97,1,0,0,0,460,461,5,28,0,0,461,462,3,90,45,0,462,
  	463,5,4,0,0,463,99,1,0,0,0,464,507,5,26,0,0,465,507,5,24,0,0,466,507,
  	5,25,0,0,467,507,5,27,0,0,468,507,5,23,0,0,469,507,5,29,0,0,470,507,5,
  	11,0,0,471,507,5,13,0,0,472,507,5,14,0,0,473,507,5,15,0,0,474,507,5,16,
  	0,0,475,507,5,17,0,0,476,507,5,8,0,0,477,507,5,7,0,0,478,507,5,9,0,0,
  	479,507,5,10,0,0,480,507,3,98,49,0,481,485,5,3,0,0,482,484,3,100,50,0,
  	483,482,1,0,0,0,484,487,1,0,0,0,485,483,1,0,0,0,485,486,1,0,0,0,486,488,
  	1,0,0,0,487,485,1,0,0,0,488,507,5,4,0,0,489,493,5,5,0,0,490,492,3,100,
  	50,0,491,490,1,0,0,0,492,495,1,0,0,0,493,491,1,0,0,0,493,494,1,0,0,0,
  	494,496,1,0,0,0,495,493,1,0,0,0,496,507,5,6,0,0,497,501,5,1,0,0,498,500,
  	3,100,50,0,499,498,1,0,0,0,500,503,1,0,0,0,501,499,1,0,0,0,501,502,1,
  	0,0,0,502,504,1,0,0,0,503,501,1,0,0,0,504,507,5,2,0,0,505,507,5,52,0,
  	0,506,464,1,0,0,0,506,465,1,0,0,0,506,466,1,0,0,0,506,467,1,0,0,0,506,
  	468,1,0,0,0,506,469,1,0,0,0,506,470,1,0,0,0,506,471,1,0,0,0,506,472,1,
  	0,0,0,506,473,1,0,0,0,506,474,1,0,0,0,506,475,1,0,0,0,506,476,1,0,0,0,
  	506,477,1,0,0,0,506,478,1,0,0,0,506,479,1,0,0,0,506,480,1,0,0,0,506,481,
  	1,0,0,0,506,489,1,0,0,0,506,497,1,0,0,0,506,505,1,0,0,0,507,101,1,0,0,
  	0,52,106,108,120,126,138,145,152,157,165,175,179,186,194,197,205,213,
  	233,248,257,266,274,284,291,301,316,325,336,338,346,348,350,353,357,360,
  	363,379,385,389,404,407,411,415,422,434,440,443,453,456,485,493,501,506
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  cssparserParserStaticData = std::move(staticData);
}

}

CSSParser::CSSParser(TokenStream *input) : CSSParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

CSSParser::CSSParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  CSSParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *cssparserParserStaticData->atn, cssparserParserStaticData->decisionToDFA, cssparserParserStaticData->sharedContextCache, options);
}

CSSParser::~CSSParser() {
  delete _interpreter;
}

const atn::ATN& CSSParser::getATN() const {
  return *cssparserParserStaticData->atn;
}

std::string CSSParser::getGrammarFileName() const {
  return "CSSParser.g4";
}

const std::vector<std::string>& CSSParser::getRuleNames() const {
  return cssparserParserStaticData->ruleNames;
}

const dfa::Vocabulary& CSSParser::getVocabulary() const {
  return cssparserParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView CSSParser::getSerializedATN() const {
  return cssparserParserStaticData->serializedATN;
}


//----------------- StylesheetContext ------------------------------------------------------------------

CSSParser::StylesheetContext::StylesheetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::StylesheetContext::EOF() {
  return getToken(CSSParser::EOF, 0);
}

std::vector<CSSParser::CharsetContext *> CSSParser::StylesheetContext::charset() {
  return getRuleContexts<CSSParser::CharsetContext>();
}

CSSParser::CharsetContext* CSSParser::StylesheetContext::charset(size_t i) {
  return getRuleContext<CSSParser::CharsetContext>(i);
}

std::vector<CSSParser::Import_ruleContext *> CSSParser::StylesheetContext::import_rule() {
  return getRuleContexts<CSSParser::Import_ruleContext>();
}

CSSParser::Import_ruleContext* CSSParser::StylesheetContext::import_rule(size_t i) {
  return getRuleContext<CSSParser::Import_ruleContext>(i);
}

std::vector<CSSParser::Namespace_ruleContext *> CSSParser::StylesheetContext::namespace_rule() {
  return getRuleContexts<CSSParser::Namespace_ruleContext>();
}

CSSParser::Namespace_ruleContext* CSSParser::StylesheetContext::namespace_rule(size_t i) {
  return getRuleContext<CSSParser::Namespace_ruleContext>(i);
}

std::vector<CSSParser::Nested_statementContext *> CSSParser::StylesheetContext::nested_statement() {
  return getRuleContexts<CSSParser::Nested_statementContext>();
}

CSSParser::Nested_statementContext* CSSParser::StylesheetContext::nested_statement(size_t i) {
  return getRuleContext<CSSParser::Nested_statementContext>(i);
}


size_t CSSParser::StylesheetContext::getRuleIndex() const {
  return CSSParser::RuleStylesheet;
}

void CSSParser::StylesheetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStylesheet(this);
}

void CSSParser::StylesheetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStylesheet(this);
}

CSSParser::StylesheetContext* CSSParser::stylesheet() {
  StylesheetContext *_localctx = _tracker.createInstance<StylesheetContext>(_ctx, getState());
  enterRule(_localctx, 0, CSSParser::RuleStylesheet);
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
    setState(108);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3447793653947680) != 0)) {
      setState(106);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case CSSParser::AT_CHARSET: {
          setState(102);
          charset();
          break;
        }

        case CSSParser::AT_IMPORT: {
          setState(103);
          import_rule();
          break;
        }

        case CSSParser::AT_NAMESPACE: {
          setState(104);
          namespace_rule();
          break;
        }

        case CSSParser::LBRACKET:
        case CSSParser::COLON:
        case CSSParser::DOT:
        case CSSParser::HASH:
        case CSSParser::AT:
        case CSSParser::MULTIPLY:
        case CSSParser::IDENT:
        case CSSParser::AT_MEDIA:
        case CSSParser::AT_PAGE:
        case CSSParser::AT_FONT_FACE:
        case CSSParser::AT_KEYFRAMES:
        case CSSParser::AT_SUPPORTS:
        case CSSParser::PIPE:
        case CSSParser::NOT_FUNCTION: {
          setState(105);
          nested_statement();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(110);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(111);
    match(CSSParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CharsetContext ------------------------------------------------------------------

CSSParser::CharsetContext::CharsetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::CharsetContext::AT_CHARSET() {
  return getToken(CSSParser::AT_CHARSET, 0);
}

tree::TerminalNode* CSSParser::CharsetContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}

tree::TerminalNode* CSSParser::CharsetContext::SEMICOLON() {
  return getToken(CSSParser::SEMICOLON, 0);
}


size_t CSSParser::CharsetContext::getRuleIndex() const {
  return CSSParser::RuleCharset;
}

void CSSParser::CharsetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCharset(this);
}

void CSSParser::CharsetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCharset(this);
}

CSSParser::CharsetContext* CSSParser::charset() {
  CharsetContext *_localctx = _tracker.createInstance<CharsetContext>(_ctx, getState());
  enterRule(_localctx, 2, CSSParser::RuleCharset);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(113);
    match(CSSParser::AT_CHARSET);
    setState(114);
    match(CSSParser::STRING);
    setState(115);
    match(CSSParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Import_ruleContext ------------------------------------------------------------------

CSSParser::Import_ruleContext::Import_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Import_ruleContext::AT_IMPORT() {
  return getToken(CSSParser::AT_IMPORT, 0);
}

tree::TerminalNode* CSSParser::Import_ruleContext::SEMICOLON() {
  return getToken(CSSParser::SEMICOLON, 0);
}

tree::TerminalNode* CSSParser::Import_ruleContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}

tree::TerminalNode* CSSParser::Import_ruleContext::URI() {
  return getToken(CSSParser::URI, 0);
}

CSSParser::Media_query_listContext* CSSParser::Import_ruleContext::media_query_list() {
  return getRuleContext<CSSParser::Media_query_listContext>(0);
}


size_t CSSParser::Import_ruleContext::getRuleIndex() const {
  return CSSParser::RuleImport_rule;
}

void CSSParser::Import_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterImport_rule(this);
}

void CSSParser::Import_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitImport_rule(this);
}

CSSParser::Import_ruleContext* CSSParser::import_rule() {
  Import_ruleContext *_localctx = _tracker.createInstance<Import_ruleContext>(_ctx, getState());
  enterRule(_localctx, 4, CSSParser::RuleImport_rule);
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
    setState(117);
    match(CSSParser::AT_IMPORT);
    setState(118);
    _la = _input->LA(1);
    if (!(_la == CSSParser::STRING

    || _la == CSSParser::URI)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(120);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 103146323976) != 0)) {
      setState(119);
      media_query_list();
    }
    setState(122);
    match(CSSParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Namespace_ruleContext ------------------------------------------------------------------

CSSParser::Namespace_ruleContext::Namespace_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Namespace_ruleContext::AT_NAMESPACE() {
  return getToken(CSSParser::AT_NAMESPACE, 0);
}

tree::TerminalNode* CSSParser::Namespace_ruleContext::SEMICOLON() {
  return getToken(CSSParser::SEMICOLON, 0);
}

tree::TerminalNode* CSSParser::Namespace_ruleContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}

tree::TerminalNode* CSSParser::Namespace_ruleContext::URI() {
  return getToken(CSSParser::URI, 0);
}

tree::TerminalNode* CSSParser::Namespace_ruleContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::Namespace_ruleContext::getRuleIndex() const {
  return CSSParser::RuleNamespace_rule;
}

void CSSParser::Namespace_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNamespace_rule(this);
}

void CSSParser::Namespace_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNamespace_rule(this);
}

CSSParser::Namespace_ruleContext* CSSParser::namespace_rule() {
  Namespace_ruleContext *_localctx = _tracker.createInstance<Namespace_ruleContext>(_ctx, getState());
  enterRule(_localctx, 6, CSSParser::RuleNamespace_rule);
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
    setState(124);
    match(CSSParser::AT_NAMESPACE);
    setState(126);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::IDENT) {
      setState(125);
      match(CSSParser::IDENT);
    }
    setState(128);
    _la = _input->LA(1);
    if (!(_la == CSSParser::STRING

    || _la == CSSParser::URI)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(129);
    match(CSSParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Nested_statementContext ------------------------------------------------------------------

CSSParser::Nested_statementContext::Nested_statementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::RulesetContext* CSSParser::Nested_statementContext::ruleset() {
  return getRuleContext<CSSParser::RulesetContext>(0);
}

CSSParser::Media_ruleContext* CSSParser::Nested_statementContext::media_rule() {
  return getRuleContext<CSSParser::Media_ruleContext>(0);
}

CSSParser::Page_ruleContext* CSSParser::Nested_statementContext::page_rule() {
  return getRuleContext<CSSParser::Page_ruleContext>(0);
}

CSSParser::Font_face_ruleContext* CSSParser::Nested_statementContext::font_face_rule() {
  return getRuleContext<CSSParser::Font_face_ruleContext>(0);
}

CSSParser::Keyframes_ruleContext* CSSParser::Nested_statementContext::keyframes_rule() {
  return getRuleContext<CSSParser::Keyframes_ruleContext>(0);
}

CSSParser::Supports_ruleContext* CSSParser::Nested_statementContext::supports_rule() {
  return getRuleContext<CSSParser::Supports_ruleContext>(0);
}

CSSParser::Unknown_at_ruleContext* CSSParser::Nested_statementContext::unknown_at_rule() {
  return getRuleContext<CSSParser::Unknown_at_ruleContext>(0);
}


size_t CSSParser::Nested_statementContext::getRuleIndex() const {
  return CSSParser::RuleNested_statement;
}

void CSSParser::Nested_statementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNested_statement(this);
}

void CSSParser::Nested_statementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNested_statement(this);
}

CSSParser::Nested_statementContext* CSSParser::nested_statement() {
  Nested_statementContext *_localctx = _tracker.createInstance<Nested_statementContext>(_ctx, getState());
  enterRule(_localctx, 8, CSSParser::RuleNested_statement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(138);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::LBRACKET:
      case CSSParser::COLON:
      case CSSParser::DOT:
      case CSSParser::HASH:
      case CSSParser::MULTIPLY:
      case CSSParser::IDENT:
      case CSSParser::PIPE:
      case CSSParser::NOT_FUNCTION: {
        enterOuterAlt(_localctx, 1);
        setState(131);
        ruleset();
        break;
      }

      case CSSParser::AT_MEDIA: {
        enterOuterAlt(_localctx, 2);
        setState(132);
        media_rule();
        break;
      }

      case CSSParser::AT_PAGE: {
        enterOuterAlt(_localctx, 3);
        setState(133);
        page_rule();
        break;
      }

      case CSSParser::AT_FONT_FACE: {
        enterOuterAlt(_localctx, 4);
        setState(134);
        font_face_rule();
        break;
      }

      case CSSParser::AT_KEYFRAMES: {
        enterOuterAlt(_localctx, 5);
        setState(135);
        keyframes_rule();
        break;
      }

      case CSSParser::AT_SUPPORTS: {
        enterOuterAlt(_localctx, 6);
        setState(136);
        supports_rule();
        break;
      }

      case CSSParser::AT: {
        enterOuterAlt(_localctx, 7);
        setState(137);
        unknown_at_rule();
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

//----------------- Unknown_at_ruleContext ------------------------------------------------------------------

CSSParser::Unknown_at_ruleContext::Unknown_at_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Unknown_at_ruleContext::AT() {
  return getToken(CSSParser::AT, 0);
}

tree::TerminalNode* CSSParser::Unknown_at_ruleContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

tree::TerminalNode* CSSParser::Unknown_at_ruleContext::LBRACE() {
  return getToken(CSSParser::LBRACE, 0);
}

tree::TerminalNode* CSSParser::Unknown_at_ruleContext::RBRACE() {
  return getToken(CSSParser::RBRACE, 0);
}

tree::TerminalNode* CSSParser::Unknown_at_ruleContext::SEMICOLON() {
  return getToken(CSSParser::SEMICOLON, 0);
}

std::vector<CSSParser::AnyContext *> CSSParser::Unknown_at_ruleContext::any() {
  return getRuleContexts<CSSParser::AnyContext>();
}

CSSParser::AnyContext* CSSParser::Unknown_at_ruleContext::any(size_t i) {
  return getRuleContext<CSSParser::AnyContext>(i);
}


size_t CSSParser::Unknown_at_ruleContext::getRuleIndex() const {
  return CSSParser::RuleUnknown_at_rule;
}

void CSSParser::Unknown_at_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnknown_at_rule(this);
}

void CSSParser::Unknown_at_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnknown_at_rule(this);
}

CSSParser::Unknown_at_ruleContext* CSSParser::unknown_at_rule() {
  Unknown_at_ruleContext *_localctx = _tracker.createInstance<Unknown_at_ruleContext>(_ctx, getState());
  enterRule(_localctx, 10, CSSParser::RuleUnknown_at_rule);
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
    setState(140);
    match(CSSParser::AT);
    setState(141);
    match(CSSParser::IDENT);
    setState(145);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(142);
        any(); 
      }
      setState(147);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
    }
    setState(157);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::LBRACE: {
        setState(148);
        match(CSSParser::LBRACE);
        setState(152);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 4503600692981674) != 0)) {
          setState(149);
          any();
          setState(154);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(155);
        match(CSSParser::RBRACE);
        break;
      }

      case CSSParser::SEMICOLON: {
        setState(156);
        match(CSSParser::SEMICOLON);
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

//----------------- Media_ruleContext ------------------------------------------------------------------

CSSParser::Media_ruleContext::Media_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Media_ruleContext::AT_MEDIA() {
  return getToken(CSSParser::AT_MEDIA, 0);
}

CSSParser::Media_query_listContext* CSSParser::Media_ruleContext::media_query_list() {
  return getRuleContext<CSSParser::Media_query_listContext>(0);
}

tree::TerminalNode* CSSParser::Media_ruleContext::LBRACE() {
  return getToken(CSSParser::LBRACE, 0);
}

tree::TerminalNode* CSSParser::Media_ruleContext::RBRACE() {
  return getToken(CSSParser::RBRACE, 0);
}

std::vector<CSSParser::Nested_statementContext *> CSSParser::Media_ruleContext::nested_statement() {
  return getRuleContexts<CSSParser::Nested_statementContext>();
}

CSSParser::Nested_statementContext* CSSParser::Media_ruleContext::nested_statement(size_t i) {
  return getRuleContext<CSSParser::Nested_statementContext>(i);
}


size_t CSSParser::Media_ruleContext::getRuleIndex() const {
  return CSSParser::RuleMedia_rule;
}

void CSSParser::Media_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMedia_rule(this);
}

void CSSParser::Media_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMedia_rule(this);
}

CSSParser::Media_ruleContext* CSSParser::media_rule() {
  Media_ruleContext *_localctx = _tracker.createInstance<Media_ruleContext>(_ctx, getState());
  enterRule(_localctx, 12, CSSParser::RuleMedia_rule);
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
    setState(159);
    match(CSSParser::AT_MEDIA);
    setState(160);
    media_query_list();
    setState(161);
    match(CSSParser::LBRACE);
    setState(165);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3445869508599072) != 0)) {
      setState(162);
      nested_statement();
      setState(167);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(168);
    match(CSSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Media_query_listContext ------------------------------------------------------------------

CSSParser::Media_query_listContext::Media_query_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::Media_queryContext *> CSSParser::Media_query_listContext::media_query() {
  return getRuleContexts<CSSParser::Media_queryContext>();
}

CSSParser::Media_queryContext* CSSParser::Media_query_listContext::media_query(size_t i) {
  return getRuleContext<CSSParser::Media_queryContext>(i);
}

std::vector<tree::TerminalNode *> CSSParser::Media_query_listContext::COMMA() {
  return getTokens(CSSParser::COMMA);
}

tree::TerminalNode* CSSParser::Media_query_listContext::COMMA(size_t i) {
  return getToken(CSSParser::COMMA, i);
}


size_t CSSParser::Media_query_listContext::getRuleIndex() const {
  return CSSParser::RuleMedia_query_list;
}

void CSSParser::Media_query_listContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMedia_query_list(this);
}

void CSSParser::Media_query_listContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMedia_query_list(this);
}

CSSParser::Media_query_listContext* CSSParser::media_query_list() {
  Media_query_listContext *_localctx = _tracker.createInstance<Media_query_listContext>(_ctx, getState());
  enterRule(_localctx, 14, CSSParser::RuleMedia_query_list);
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
    setState(170);
    media_query();
    setState(175);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSSParser::COMMA) {
      setState(171);
      match(CSSParser::COMMA);
      setState(172);
      media_query();
      setState(177);
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

//----------------- Media_queryContext ------------------------------------------------------------------

CSSParser::Media_queryContext::Media_queryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::Media_typeContext* CSSParser::Media_queryContext::media_type() {
  return getRuleContext<CSSParser::Media_typeContext>(0);
}

std::vector<tree::TerminalNode *> CSSParser::Media_queryContext::AND_KEYWORD() {
  return getTokens(CSSParser::AND_KEYWORD);
}

tree::TerminalNode* CSSParser::Media_queryContext::AND_KEYWORD(size_t i) {
  return getToken(CSSParser::AND_KEYWORD, i);
}

std::vector<CSSParser::Media_expressionContext *> CSSParser::Media_queryContext::media_expression() {
  return getRuleContexts<CSSParser::Media_expressionContext>();
}

CSSParser::Media_expressionContext* CSSParser::Media_queryContext::media_expression(size_t i) {
  return getRuleContext<CSSParser::Media_expressionContext>(i);
}

tree::TerminalNode* CSSParser::Media_queryContext::ONLY_KEYWORD() {
  return getToken(CSSParser::ONLY_KEYWORD, 0);
}

tree::TerminalNode* CSSParser::Media_queryContext::NOT_KEYWORD() {
  return getToken(CSSParser::NOT_KEYWORD, 0);
}


size_t CSSParser::Media_queryContext::getRuleIndex() const {
  return CSSParser::RuleMedia_query;
}

void CSSParser::Media_queryContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMedia_query(this);
}

void CSSParser::Media_queryContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMedia_query(this);
}

CSSParser::Media_queryContext* CSSParser::media_query() {
  Media_queryContext *_localctx = _tracker.createInstance<Media_queryContext>(_ctx, getState());
  enterRule(_localctx, 16, CSSParser::RuleMedia_query);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(197);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::IDENT:
      case CSSParser::ONLY_KEYWORD:
      case CSSParser::NOT_KEYWORD: {
        enterOuterAlt(_localctx, 1);
        setState(179);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == CSSParser::ONLY_KEYWORD

        || _la == CSSParser::NOT_KEYWORD) {
          setState(178);
          _la = _input->LA(1);
          if (!(_la == CSSParser::ONLY_KEYWORD

          || _la == CSSParser::NOT_KEYWORD)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
        }
        setState(181);
        media_type();
        setState(186);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == CSSParser::AND_KEYWORD) {
          setState(182);
          match(CSSParser::AND_KEYWORD);
          setState(183);
          media_expression();
          setState(188);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        break;
      }

      case CSSParser::LPAREN: {
        enterOuterAlt(_localctx, 2);
        setState(189);
        media_expression();
        setState(194);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == CSSParser::AND_KEYWORD) {
          setState(190);
          match(CSSParser::AND_KEYWORD);
          setState(191);
          media_expression();
          setState(196);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
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

//----------------- Media_typeContext ------------------------------------------------------------------

CSSParser::Media_typeContext::Media_typeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Media_typeContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::Media_typeContext::getRuleIndex() const {
  return CSSParser::RuleMedia_type;
}

void CSSParser::Media_typeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMedia_type(this);
}

void CSSParser::Media_typeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMedia_type(this);
}

CSSParser::Media_typeContext* CSSParser::media_type() {
  Media_typeContext *_localctx = _tracker.createInstance<Media_typeContext>(_ctx, getState());
  enterRule(_localctx, 18, CSSParser::RuleMedia_type);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(199);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Media_expressionContext ------------------------------------------------------------------

CSSParser::Media_expressionContext::Media_expressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Media_expressionContext::LPAREN() {
  return getToken(CSSParser::LPAREN, 0);
}

CSSParser::Media_featureContext* CSSParser::Media_expressionContext::media_feature() {
  return getRuleContext<CSSParser::Media_featureContext>(0);
}

tree::TerminalNode* CSSParser::Media_expressionContext::RPAREN() {
  return getToken(CSSParser::RPAREN, 0);
}

tree::TerminalNode* CSSParser::Media_expressionContext::COLON() {
  return getToken(CSSParser::COLON, 0);
}

CSSParser::ExprContext* CSSParser::Media_expressionContext::expr() {
  return getRuleContext<CSSParser::ExprContext>(0);
}


size_t CSSParser::Media_expressionContext::getRuleIndex() const {
  return CSSParser::RuleMedia_expression;
}

void CSSParser::Media_expressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMedia_expression(this);
}

void CSSParser::Media_expressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMedia_expression(this);
}

CSSParser::Media_expressionContext* CSSParser::media_expression() {
  Media_expressionContext *_localctx = _tracker.createInstance<Media_expressionContext>(_ctx, getState());
  enterRule(_localctx, 20, CSSParser::RuleMedia_expression);
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
    setState(201);
    match(CSSParser::LPAREN);
    setState(202);
    media_feature();
    setState(205);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::COLON) {
      setState(203);
      match(CSSParser::COLON);
      setState(204);
      expr();
    }
    setState(207);
    match(CSSParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Media_featureContext ------------------------------------------------------------------

CSSParser::Media_featureContext::Media_featureContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Media_featureContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::Media_featureContext::getRuleIndex() const {
  return CSSParser::RuleMedia_feature;
}

void CSSParser::Media_featureContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMedia_feature(this);
}

void CSSParser::Media_featureContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMedia_feature(this);
}

CSSParser::Media_featureContext* CSSParser::media_feature() {
  Media_featureContext *_localctx = _tracker.createInstance<Media_featureContext>(_ctx, getState());
  enterRule(_localctx, 22, CSSParser::RuleMedia_feature);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(209);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Page_ruleContext ------------------------------------------------------------------

CSSParser::Page_ruleContext::Page_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Page_ruleContext::AT_PAGE() {
  return getToken(CSSParser::AT_PAGE, 0);
}

tree::TerminalNode* CSSParser::Page_ruleContext::LBRACE() {
  return getToken(CSSParser::LBRACE, 0);
}

CSSParser::Declaration_listContext* CSSParser::Page_ruleContext::declaration_list() {
  return getRuleContext<CSSParser::Declaration_listContext>(0);
}

tree::TerminalNode* CSSParser::Page_ruleContext::RBRACE() {
  return getToken(CSSParser::RBRACE, 0);
}

CSSParser::Pseudo_pageContext* CSSParser::Page_ruleContext::pseudo_page() {
  return getRuleContext<CSSParser::Pseudo_pageContext>(0);
}


size_t CSSParser::Page_ruleContext::getRuleIndex() const {
  return CSSParser::RulePage_rule;
}

void CSSParser::Page_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPage_rule(this);
}

void CSSParser::Page_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPage_rule(this);
}

CSSParser::Page_ruleContext* CSSParser::page_rule() {
  Page_ruleContext *_localctx = _tracker.createInstance<Page_ruleContext>(_ctx, getState());
  enterRule(_localctx, 24, CSSParser::RulePage_rule);
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
    setState(211);
    match(CSSParser::AT_PAGE);
    setState(213);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::COLON) {
      setState(212);
      pseudo_page();
    }
    setState(215);
    match(CSSParser::LBRACE);
    setState(216);
    declaration_list();
    setState(217);
    match(CSSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Pseudo_pageContext ------------------------------------------------------------------

CSSParser::Pseudo_pageContext::Pseudo_pageContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Pseudo_pageContext::COLON() {
  return getToken(CSSParser::COLON, 0);
}

tree::TerminalNode* CSSParser::Pseudo_pageContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::Pseudo_pageContext::getRuleIndex() const {
  return CSSParser::RulePseudo_page;
}

void CSSParser::Pseudo_pageContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPseudo_page(this);
}

void CSSParser::Pseudo_pageContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPseudo_page(this);
}

CSSParser::Pseudo_pageContext* CSSParser::pseudo_page() {
  Pseudo_pageContext *_localctx = _tracker.createInstance<Pseudo_pageContext>(_ctx, getState());
  enterRule(_localctx, 26, CSSParser::RulePseudo_page);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(219);
    match(CSSParser::COLON);
    setState(220);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Font_face_ruleContext ------------------------------------------------------------------

CSSParser::Font_face_ruleContext::Font_face_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Font_face_ruleContext::AT_FONT_FACE() {
  return getToken(CSSParser::AT_FONT_FACE, 0);
}

tree::TerminalNode* CSSParser::Font_face_ruleContext::LBRACE() {
  return getToken(CSSParser::LBRACE, 0);
}

CSSParser::Declaration_listContext* CSSParser::Font_face_ruleContext::declaration_list() {
  return getRuleContext<CSSParser::Declaration_listContext>(0);
}

tree::TerminalNode* CSSParser::Font_face_ruleContext::RBRACE() {
  return getToken(CSSParser::RBRACE, 0);
}


size_t CSSParser::Font_face_ruleContext::getRuleIndex() const {
  return CSSParser::RuleFont_face_rule;
}

void CSSParser::Font_face_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFont_face_rule(this);
}

void CSSParser::Font_face_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFont_face_rule(this);
}

CSSParser::Font_face_ruleContext* CSSParser::font_face_rule() {
  Font_face_ruleContext *_localctx = _tracker.createInstance<Font_face_ruleContext>(_ctx, getState());
  enterRule(_localctx, 28, CSSParser::RuleFont_face_rule);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(222);
    match(CSSParser::AT_FONT_FACE);
    setState(223);
    match(CSSParser::LBRACE);
    setState(224);
    declaration_list();
    setState(225);
    match(CSSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Keyframes_ruleContext ------------------------------------------------------------------

CSSParser::Keyframes_ruleContext::Keyframes_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Keyframes_ruleContext::AT_KEYFRAMES() {
  return getToken(CSSParser::AT_KEYFRAMES, 0);
}

tree::TerminalNode* CSSParser::Keyframes_ruleContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

tree::TerminalNode* CSSParser::Keyframes_ruleContext::LBRACE() {
  return getToken(CSSParser::LBRACE, 0);
}

tree::TerminalNode* CSSParser::Keyframes_ruleContext::RBRACE() {
  return getToken(CSSParser::RBRACE, 0);
}

std::vector<CSSParser::Keyframe_blockContext *> CSSParser::Keyframes_ruleContext::keyframe_block() {
  return getRuleContexts<CSSParser::Keyframe_blockContext>();
}

CSSParser::Keyframe_blockContext* CSSParser::Keyframes_ruleContext::keyframe_block(size_t i) {
  return getRuleContext<CSSParser::Keyframe_blockContext>(i);
}


size_t CSSParser::Keyframes_ruleContext::getRuleIndex() const {
  return CSSParser::RuleKeyframes_rule;
}

void CSSParser::Keyframes_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKeyframes_rule(this);
}

void CSSParser::Keyframes_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKeyframes_rule(this);
}

CSSParser::Keyframes_ruleContext* CSSParser::keyframes_rule() {
  Keyframes_ruleContext *_localctx = _tracker.createInstance<Keyframes_ruleContext>(_ctx, getState());
  enterRule(_localctx, 30, CSSParser::RuleKeyframes_rule);
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
    setState(227);
    match(CSSParser::AT_KEYFRAMES);
    setState(228);
    match(CSSParser::IDENT);
    setState(229);
    match(CSSParser::LBRACE);
    setState(233);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 211106266087424) != 0)) {
      setState(230);
      keyframe_block();
      setState(235);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(236);
    match(CSSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Keyframe_blockContext ------------------------------------------------------------------

CSSParser::Keyframe_blockContext::Keyframe_blockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::Keyframe_selectorContext* CSSParser::Keyframe_blockContext::keyframe_selector() {
  return getRuleContext<CSSParser::Keyframe_selectorContext>(0);
}

tree::TerminalNode* CSSParser::Keyframe_blockContext::LBRACE() {
  return getToken(CSSParser::LBRACE, 0);
}

CSSParser::Declaration_listContext* CSSParser::Keyframe_blockContext::declaration_list() {
  return getRuleContext<CSSParser::Declaration_listContext>(0);
}

tree::TerminalNode* CSSParser::Keyframe_blockContext::RBRACE() {
  return getToken(CSSParser::RBRACE, 0);
}


size_t CSSParser::Keyframe_blockContext::getRuleIndex() const {
  return CSSParser::RuleKeyframe_block;
}

void CSSParser::Keyframe_blockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKeyframe_block(this);
}

void CSSParser::Keyframe_blockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKeyframe_block(this);
}

CSSParser::Keyframe_blockContext* CSSParser::keyframe_block() {
  Keyframe_blockContext *_localctx = _tracker.createInstance<Keyframe_blockContext>(_ctx, getState());
  enterRule(_localctx, 32, CSSParser::RuleKeyframe_block);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(238);
    keyframe_selector();
    setState(239);
    match(CSSParser::LBRACE);
    setState(240);
    declaration_list();
    setState(241);
    match(CSSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Keyframe_selectorContext ------------------------------------------------------------------

CSSParser::Keyframe_selectorContext::Keyframe_selectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> CSSParser::Keyframe_selectorContext::PERCENTAGE() {
  return getTokens(CSSParser::PERCENTAGE);
}

tree::TerminalNode* CSSParser::Keyframe_selectorContext::PERCENTAGE(size_t i) {
  return getToken(CSSParser::PERCENTAGE, i);
}

std::vector<tree::TerminalNode *> CSSParser::Keyframe_selectorContext::FROM_KEYWORD() {
  return getTokens(CSSParser::FROM_KEYWORD);
}

tree::TerminalNode* CSSParser::Keyframe_selectorContext::FROM_KEYWORD(size_t i) {
  return getToken(CSSParser::FROM_KEYWORD, i);
}

std::vector<tree::TerminalNode *> CSSParser::Keyframe_selectorContext::TO_KEYWORD() {
  return getTokens(CSSParser::TO_KEYWORD);
}

tree::TerminalNode* CSSParser::Keyframe_selectorContext::TO_KEYWORD(size_t i) {
  return getToken(CSSParser::TO_KEYWORD, i);
}

std::vector<tree::TerminalNode *> CSSParser::Keyframe_selectorContext::COMMA() {
  return getTokens(CSSParser::COMMA);
}

tree::TerminalNode* CSSParser::Keyframe_selectorContext::COMMA(size_t i) {
  return getToken(CSSParser::COMMA, i);
}


size_t CSSParser::Keyframe_selectorContext::getRuleIndex() const {
  return CSSParser::RuleKeyframe_selector;
}

void CSSParser::Keyframe_selectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKeyframe_selector(this);
}

void CSSParser::Keyframe_selectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKeyframe_selector(this);
}

CSSParser::Keyframe_selectorContext* CSSParser::keyframe_selector() {
  Keyframe_selectorContext *_localctx = _tracker.createInstance<Keyframe_selectorContext>(_ctx, getState());
  enterRule(_localctx, 34, CSSParser::RuleKeyframe_selector);
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
    setState(243);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 211106266087424) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(248);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSSParser::COMMA) {
      setState(244);
      match(CSSParser::COMMA);
      setState(245);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 211106266087424) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(250);
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

//----------------- Supports_ruleContext ------------------------------------------------------------------

CSSParser::Supports_ruleContext::Supports_ruleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Supports_ruleContext::AT_SUPPORTS() {
  return getToken(CSSParser::AT_SUPPORTS, 0);
}

CSSParser::Supports_conditionContext* CSSParser::Supports_ruleContext::supports_condition() {
  return getRuleContext<CSSParser::Supports_conditionContext>(0);
}

tree::TerminalNode* CSSParser::Supports_ruleContext::LBRACE() {
  return getToken(CSSParser::LBRACE, 0);
}

tree::TerminalNode* CSSParser::Supports_ruleContext::RBRACE() {
  return getToken(CSSParser::RBRACE, 0);
}

std::vector<CSSParser::Nested_statementContext *> CSSParser::Supports_ruleContext::nested_statement() {
  return getRuleContexts<CSSParser::Nested_statementContext>();
}

CSSParser::Nested_statementContext* CSSParser::Supports_ruleContext::nested_statement(size_t i) {
  return getRuleContext<CSSParser::Nested_statementContext>(i);
}


size_t CSSParser::Supports_ruleContext::getRuleIndex() const {
  return CSSParser::RuleSupports_rule;
}

void CSSParser::Supports_ruleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSupports_rule(this);
}

void CSSParser::Supports_ruleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSupports_rule(this);
}

CSSParser::Supports_ruleContext* CSSParser::supports_rule() {
  Supports_ruleContext *_localctx = _tracker.createInstance<Supports_ruleContext>(_ctx, getState());
  enterRule(_localctx, 36, CSSParser::RuleSupports_rule);
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
    setState(251);
    match(CSSParser::AT_SUPPORTS);
    setState(252);
    supports_condition();
    setState(253);
    match(CSSParser::LBRACE);
    setState(257);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3445869508599072) != 0)) {
      setState(254);
      nested_statement();
      setState(259);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(260);
    match(CSSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Supports_conditionContext ------------------------------------------------------------------

CSSParser::Supports_conditionContext::Supports_conditionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::Supports_negationContext* CSSParser::Supports_conditionContext::supports_negation() {
  return getRuleContext<CSSParser::Supports_negationContext>(0);
}

CSSParser::Supports_conjunctionContext* CSSParser::Supports_conditionContext::supports_conjunction() {
  return getRuleContext<CSSParser::Supports_conjunctionContext>(0);
}

CSSParser::Supports_disjunctionContext* CSSParser::Supports_conditionContext::supports_disjunction() {
  return getRuleContext<CSSParser::Supports_disjunctionContext>(0);
}

CSSParser::Supports_condition_in_parensContext* CSSParser::Supports_conditionContext::supports_condition_in_parens() {
  return getRuleContext<CSSParser::Supports_condition_in_parensContext>(0);
}


size_t CSSParser::Supports_conditionContext::getRuleIndex() const {
  return CSSParser::RuleSupports_condition;
}

void CSSParser::Supports_conditionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSupports_condition(this);
}

void CSSParser::Supports_conditionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSupports_condition(this);
}

CSSParser::Supports_conditionContext* CSSParser::supports_condition() {
  Supports_conditionContext *_localctx = _tracker.createInstance<Supports_conditionContext>(_ctx, getState());
  enterRule(_localctx, 38, CSSParser::RuleSupports_condition);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(266);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(262);
      supports_negation();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(263);
      supports_conjunction();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(264);
      supports_disjunction();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(265);
      supports_condition_in_parens();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Supports_condition_in_parensContext ------------------------------------------------------------------

CSSParser::Supports_condition_in_parensContext::Supports_condition_in_parensContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Supports_condition_in_parensContext::LPAREN() {
  return getToken(CSSParser::LPAREN, 0);
}

CSSParser::Supports_conditionContext* CSSParser::Supports_condition_in_parensContext::supports_condition() {
  return getRuleContext<CSSParser::Supports_conditionContext>(0);
}

tree::TerminalNode* CSSParser::Supports_condition_in_parensContext::RPAREN() {
  return getToken(CSSParser::RPAREN, 0);
}

CSSParser::Supports_declaration_conditionContext* CSSParser::Supports_condition_in_parensContext::supports_declaration_condition() {
  return getRuleContext<CSSParser::Supports_declaration_conditionContext>(0);
}

CSSParser::General_enclosedContext* CSSParser::Supports_condition_in_parensContext::general_enclosed() {
  return getRuleContext<CSSParser::General_enclosedContext>(0);
}


size_t CSSParser::Supports_condition_in_parensContext::getRuleIndex() const {
  return CSSParser::RuleSupports_condition_in_parens;
}

void CSSParser::Supports_condition_in_parensContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSupports_condition_in_parens(this);
}

void CSSParser::Supports_condition_in_parensContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSupports_condition_in_parens(this);
}

CSSParser::Supports_condition_in_parensContext* CSSParser::supports_condition_in_parens() {
  Supports_condition_in_parensContext *_localctx = _tracker.createInstance<Supports_condition_in_parensContext>(_ctx, getState());
  enterRule(_localctx, 40, CSSParser::RuleSupports_condition_in_parens);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(274);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(268);
      match(CSSParser::LPAREN);
      setState(269);
      supports_condition();
      setState(270);
      match(CSSParser::RPAREN);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(272);
      supports_declaration_condition();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(273);
      general_enclosed();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Supports_negationContext ------------------------------------------------------------------

CSSParser::Supports_negationContext::Supports_negationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Supports_negationContext::NOT_KEYWORD() {
  return getToken(CSSParser::NOT_KEYWORD, 0);
}

CSSParser::Supports_condition_in_parensContext* CSSParser::Supports_negationContext::supports_condition_in_parens() {
  return getRuleContext<CSSParser::Supports_condition_in_parensContext>(0);
}


size_t CSSParser::Supports_negationContext::getRuleIndex() const {
  return CSSParser::RuleSupports_negation;
}

void CSSParser::Supports_negationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSupports_negation(this);
}

void CSSParser::Supports_negationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSupports_negation(this);
}

CSSParser::Supports_negationContext* CSSParser::supports_negation() {
  Supports_negationContext *_localctx = _tracker.createInstance<Supports_negationContext>(_ctx, getState());
  enterRule(_localctx, 42, CSSParser::RuleSupports_negation);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(276);
    match(CSSParser::NOT_KEYWORD);
    setState(277);
    supports_condition_in_parens();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Supports_conjunctionContext ------------------------------------------------------------------

CSSParser::Supports_conjunctionContext::Supports_conjunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::Supports_condition_in_parensContext *> CSSParser::Supports_conjunctionContext::supports_condition_in_parens() {
  return getRuleContexts<CSSParser::Supports_condition_in_parensContext>();
}

CSSParser::Supports_condition_in_parensContext* CSSParser::Supports_conjunctionContext::supports_condition_in_parens(size_t i) {
  return getRuleContext<CSSParser::Supports_condition_in_parensContext>(i);
}

std::vector<tree::TerminalNode *> CSSParser::Supports_conjunctionContext::AND_KEYWORD() {
  return getTokens(CSSParser::AND_KEYWORD);
}

tree::TerminalNode* CSSParser::Supports_conjunctionContext::AND_KEYWORD(size_t i) {
  return getToken(CSSParser::AND_KEYWORD, i);
}


size_t CSSParser::Supports_conjunctionContext::getRuleIndex() const {
  return CSSParser::RuleSupports_conjunction;
}

void CSSParser::Supports_conjunctionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSupports_conjunction(this);
}

void CSSParser::Supports_conjunctionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSupports_conjunction(this);
}

CSSParser::Supports_conjunctionContext* CSSParser::supports_conjunction() {
  Supports_conjunctionContext *_localctx = _tracker.createInstance<Supports_conjunctionContext>(_ctx, getState());
  enterRule(_localctx, 44, CSSParser::RuleSupports_conjunction);
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
    setState(279);
    supports_condition_in_parens();
    setState(282); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(280);
      match(CSSParser::AND_KEYWORD);
      setState(281);
      supports_condition_in_parens();
      setState(284); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == CSSParser::AND_KEYWORD);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Supports_disjunctionContext ------------------------------------------------------------------

CSSParser::Supports_disjunctionContext::Supports_disjunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::Supports_condition_in_parensContext *> CSSParser::Supports_disjunctionContext::supports_condition_in_parens() {
  return getRuleContexts<CSSParser::Supports_condition_in_parensContext>();
}

CSSParser::Supports_condition_in_parensContext* CSSParser::Supports_disjunctionContext::supports_condition_in_parens(size_t i) {
  return getRuleContext<CSSParser::Supports_condition_in_parensContext>(i);
}

std::vector<tree::TerminalNode *> CSSParser::Supports_disjunctionContext::OR_KEYWORD() {
  return getTokens(CSSParser::OR_KEYWORD);
}

tree::TerminalNode* CSSParser::Supports_disjunctionContext::OR_KEYWORD(size_t i) {
  return getToken(CSSParser::OR_KEYWORD, i);
}


size_t CSSParser::Supports_disjunctionContext::getRuleIndex() const {
  return CSSParser::RuleSupports_disjunction;
}

void CSSParser::Supports_disjunctionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSupports_disjunction(this);
}

void CSSParser::Supports_disjunctionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSupports_disjunction(this);
}

CSSParser::Supports_disjunctionContext* CSSParser::supports_disjunction() {
  Supports_disjunctionContext *_localctx = _tracker.createInstance<Supports_disjunctionContext>(_ctx, getState());
  enterRule(_localctx, 46, CSSParser::RuleSupports_disjunction);
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
    setState(286);
    supports_condition_in_parens();
    setState(289); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(287);
      match(CSSParser::OR_KEYWORD);
      setState(288);
      supports_condition_in_parens();
      setState(291); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == CSSParser::OR_KEYWORD);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Supports_declaration_conditionContext ------------------------------------------------------------------

CSSParser::Supports_declaration_conditionContext::Supports_declaration_conditionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Supports_declaration_conditionContext::LPAREN() {
  return getToken(CSSParser::LPAREN, 0);
}

CSSParser::DeclarationContext* CSSParser::Supports_declaration_conditionContext::declaration() {
  return getRuleContext<CSSParser::DeclarationContext>(0);
}

tree::TerminalNode* CSSParser::Supports_declaration_conditionContext::RPAREN() {
  return getToken(CSSParser::RPAREN, 0);
}


size_t CSSParser::Supports_declaration_conditionContext::getRuleIndex() const {
  return CSSParser::RuleSupports_declaration_condition;
}

void CSSParser::Supports_declaration_conditionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSupports_declaration_condition(this);
}

void CSSParser::Supports_declaration_conditionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSupports_declaration_condition(this);
}

CSSParser::Supports_declaration_conditionContext* CSSParser::supports_declaration_condition() {
  Supports_declaration_conditionContext *_localctx = _tracker.createInstance<Supports_declaration_conditionContext>(_ctx, getState());
  enterRule(_localctx, 48, CSSParser::RuleSupports_declaration_condition);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(293);
    match(CSSParser::LPAREN);
    setState(294);
    declaration();
    setState(295);
    match(CSSParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- General_enclosedContext ------------------------------------------------------------------

CSSParser::General_enclosedContext::General_enclosedContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::General_enclosedContext::RPAREN() {
  return getToken(CSSParser::RPAREN, 0);
}

tree::TerminalNode* CSSParser::General_enclosedContext::FUNCTION() {
  return getToken(CSSParser::FUNCTION, 0);
}

tree::TerminalNode* CSSParser::General_enclosedContext::LPAREN() {
  return getToken(CSSParser::LPAREN, 0);
}

std::vector<CSSParser::AnyContext *> CSSParser::General_enclosedContext::any() {
  return getRuleContexts<CSSParser::AnyContext>();
}

CSSParser::AnyContext* CSSParser::General_enclosedContext::any(size_t i) {
  return getRuleContext<CSSParser::AnyContext>(i);
}


size_t CSSParser::General_enclosedContext::getRuleIndex() const {
  return CSSParser::RuleGeneral_enclosed;
}

void CSSParser::General_enclosedContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGeneral_enclosed(this);
}

void CSSParser::General_enclosedContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGeneral_enclosed(this);
}

CSSParser::General_enclosedContext* CSSParser::general_enclosed() {
  General_enclosedContext *_localctx = _tracker.createInstance<General_enclosedContext>(_ctx, getState());
  enterRule(_localctx, 50, CSSParser::RuleGeneral_enclosed);
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
    setState(297);
    _la = _input->LA(1);
    if (!(_la == CSSParser::LPAREN

    || _la == CSSParser::FUNCTION)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(301);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4503600692981674) != 0)) {
      setState(298);
      any();
      setState(303);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(304);
    match(CSSParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RulesetContext ------------------------------------------------------------------

CSSParser::RulesetContext::RulesetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::Selector_groupContext* CSSParser::RulesetContext::selector_group() {
  return getRuleContext<CSSParser::Selector_groupContext>(0);
}

tree::TerminalNode* CSSParser::RulesetContext::LBRACE() {
  return getToken(CSSParser::LBRACE, 0);
}

CSSParser::Declaration_listContext* CSSParser::RulesetContext::declaration_list() {
  return getRuleContext<CSSParser::Declaration_listContext>(0);
}

tree::TerminalNode* CSSParser::RulesetContext::RBRACE() {
  return getToken(CSSParser::RBRACE, 0);
}


size_t CSSParser::RulesetContext::getRuleIndex() const {
  return CSSParser::RuleRuleset;
}

void CSSParser::RulesetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRuleset(this);
}

void CSSParser::RulesetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRuleset(this);
}

CSSParser::RulesetContext* CSSParser::ruleset() {
  RulesetContext *_localctx = _tracker.createInstance<RulesetContext>(_ctx, getState());
  enterRule(_localctx, 52, CSSParser::RuleRuleset);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(306);
    selector_group();
    setState(307);
    match(CSSParser::LBRACE);
    setState(308);
    declaration_list();
    setState(309);
    match(CSSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Selector_groupContext ------------------------------------------------------------------

CSSParser::Selector_groupContext::Selector_groupContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::SelectorContext *> CSSParser::Selector_groupContext::selector() {
  return getRuleContexts<CSSParser::SelectorContext>();
}

CSSParser::SelectorContext* CSSParser::Selector_groupContext::selector(size_t i) {
  return getRuleContext<CSSParser::SelectorContext>(i);
}

std::vector<tree::TerminalNode *> CSSParser::Selector_groupContext::COMMA() {
  return getTokens(CSSParser::COMMA);
}

tree::TerminalNode* CSSParser::Selector_groupContext::COMMA(size_t i) {
  return getToken(CSSParser::COMMA, i);
}


size_t CSSParser::Selector_groupContext::getRuleIndex() const {
  return CSSParser::RuleSelector_group;
}

void CSSParser::Selector_groupContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelector_group(this);
}

void CSSParser::Selector_groupContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelector_group(this);
}

CSSParser::Selector_groupContext* CSSParser::selector_group() {
  Selector_groupContext *_localctx = _tracker.createInstance<Selector_groupContext>(_ctx, getState());
  enterRule(_localctx, 54, CSSParser::RuleSelector_group);
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
    setState(311);
    selector();
    setState(316);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSSParser::COMMA) {
      setState(312);
      match(CSSParser::COMMA);
      setState(313);
      selector();
      setState(318);
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

//----------------- SelectorContext ------------------------------------------------------------------

CSSParser::SelectorContext::SelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::Simple_selector_sequenceContext *> CSSParser::SelectorContext::simple_selector_sequence() {
  return getRuleContexts<CSSParser::Simple_selector_sequenceContext>();
}

CSSParser::Simple_selector_sequenceContext* CSSParser::SelectorContext::simple_selector_sequence(size_t i) {
  return getRuleContext<CSSParser::Simple_selector_sequenceContext>(i);
}

std::vector<CSSParser::CombinatorContext *> CSSParser::SelectorContext::combinator() {
  return getRuleContexts<CSSParser::CombinatorContext>();
}

CSSParser::CombinatorContext* CSSParser::SelectorContext::combinator(size_t i) {
  return getRuleContext<CSSParser::CombinatorContext>(i);
}


size_t CSSParser::SelectorContext::getRuleIndex() const {
  return CSSParser::RuleSelector;
}

void CSSParser::SelectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelector(this);
}

void CSSParser::SelectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelector(this);
}

CSSParser::SelectorContext* CSSParser::selector() {
  SelectorContext *_localctx = _tracker.createInstance<SelectorContext>(_ctx, getState());
  enterRule(_localctx, 56, CSSParser::RuleSelector);
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
    setState(319);
    simple_selector_sequence();
    setState(325);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 562949953478656) != 0)) {
      setState(320);
      combinator();
      setState(321);
      simple_selector_sequence();
      setState(327);
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

//----------------- CombinatorContext ------------------------------------------------------------------

CSSParser::CombinatorContext::CombinatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::CombinatorContext::PLUS() {
  return getToken(CSSParser::PLUS, 0);
}

tree::TerminalNode* CSSParser::CombinatorContext::MINUS() {
  return getToken(CSSParser::MINUS, 0);
}

tree::TerminalNode* CSSParser::CombinatorContext::MULTIPLY() {
  return getToken(CSSParser::MULTIPLY, 0);
}

tree::TerminalNode* CSSParser::CombinatorContext::TILDE() {
  return getToken(CSSParser::TILDE, 0);
}


size_t CSSParser::CombinatorContext::getRuleIndex() const {
  return CSSParser::RuleCombinator;
}

void CSSParser::CombinatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCombinator(this);
}

void CSSParser::CombinatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCombinator(this);
}

CSSParser::CombinatorContext* CSSParser::combinator() {
  CombinatorContext *_localctx = _tracker.createInstance<CombinatorContext>(_ctx, getState());
  enterRule(_localctx, 58, CSSParser::RuleCombinator);
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
    setState(328);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 562949953478656) != 0))) {
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

//----------------- Simple_selector_sequenceContext ------------------------------------------------------------------

CSSParser::Simple_selector_sequenceContext::Simple_selector_sequenceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::Type_selectorContext* CSSParser::Simple_selector_sequenceContext::type_selector() {
  return getRuleContext<CSSParser::Type_selectorContext>(0);
}

std::vector<CSSParser::HashContext *> CSSParser::Simple_selector_sequenceContext::hash() {
  return getRuleContexts<CSSParser::HashContext>();
}

CSSParser::HashContext* CSSParser::Simple_selector_sequenceContext::hash(size_t i) {
  return getRuleContext<CSSParser::HashContext>(i);
}

std::vector<CSSParser::ClassContext *> CSSParser::Simple_selector_sequenceContext::class_() {
  return getRuleContexts<CSSParser::ClassContext>();
}

CSSParser::ClassContext* CSSParser::Simple_selector_sequenceContext::class_(size_t i) {
  return getRuleContext<CSSParser::ClassContext>(i);
}

std::vector<CSSParser::AttribContext *> CSSParser::Simple_selector_sequenceContext::attrib() {
  return getRuleContexts<CSSParser::AttribContext>();
}

CSSParser::AttribContext* CSSParser::Simple_selector_sequenceContext::attrib(size_t i) {
  return getRuleContext<CSSParser::AttribContext>(i);
}

std::vector<CSSParser::PseudoContext *> CSSParser::Simple_selector_sequenceContext::pseudo() {
  return getRuleContexts<CSSParser::PseudoContext>();
}

CSSParser::PseudoContext* CSSParser::Simple_selector_sequenceContext::pseudo(size_t i) {
  return getRuleContext<CSSParser::PseudoContext>(i);
}

std::vector<CSSParser::NegationContext *> CSSParser::Simple_selector_sequenceContext::negation() {
  return getRuleContexts<CSSParser::NegationContext>();
}

CSSParser::NegationContext* CSSParser::Simple_selector_sequenceContext::negation(size_t i) {
  return getRuleContext<CSSParser::NegationContext>(i);
}


size_t CSSParser::Simple_selector_sequenceContext::getRuleIndex() const {
  return CSSParser::RuleSimple_selector_sequence;
}

void CSSParser::Simple_selector_sequenceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimple_selector_sequence(this);
}

void CSSParser::Simple_selector_sequenceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimple_selector_sequence(this);
}

CSSParser::Simple_selector_sequenceContext* CSSParser::simple_selector_sequence() {
  Simple_selector_sequenceContext *_localctx = _tracker.createInstance<Simple_selector_sequenceContext>(_ctx, getState());
  enterRule(_localctx, 60, CSSParser::RuleSimple_selector_sequence);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(350);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::MULTIPLY:
      case CSSParser::IDENT:
      case CSSParser::PIPE: {
        enterOuterAlt(_localctx, 1);
        setState(330);
        type_selector();
        setState(338);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2251799813688608) != 0)) {
          setState(336);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case CSSParser::HASH: {
              setState(331);
              hash();
              break;
            }

            case CSSParser::DOT: {
              setState(332);
              class_();
              break;
            }

            case CSSParser::LBRACKET: {
              setState(333);
              attrib();
              break;
            }

            case CSSParser::COLON: {
              setState(334);
              pseudo();
              break;
            }

            case CSSParser::NOT_FUNCTION: {
              setState(335);
              negation();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(340);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        break;
      }

      case CSSParser::LBRACKET:
      case CSSParser::COLON:
      case CSSParser::DOT:
      case CSSParser::HASH:
      case CSSParser::NOT_FUNCTION: {
        enterOuterAlt(_localctx, 2);
        setState(346); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(346);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case CSSParser::HASH: {
              setState(341);
              hash();
              break;
            }

            case CSSParser::DOT: {
              setState(342);
              class_();
              break;
            }

            case CSSParser::LBRACKET: {
              setState(343);
              attrib();
              break;
            }

            case CSSParser::COLON: {
              setState(344);
              pseudo();
              break;
            }

            case CSSParser::NOT_FUNCTION: {
              setState(345);
              negation();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(348); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2251799813688608) != 0));
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

//----------------- Type_selectorContext ------------------------------------------------------------------

CSSParser::Type_selectorContext::Type_selectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::Element_nameContext* CSSParser::Type_selectorContext::element_name() {
  return getRuleContext<CSSParser::Element_nameContext>(0);
}

CSSParser::Namespace_prefixContext* CSSParser::Type_selectorContext::namespace_prefix() {
  return getRuleContext<CSSParser::Namespace_prefixContext>(0);
}

tree::TerminalNode* CSSParser::Type_selectorContext::MULTIPLY() {
  return getToken(CSSParser::MULTIPLY, 0);
}


size_t CSSParser::Type_selectorContext::getRuleIndex() const {
  return CSSParser::RuleType_selector;
}

void CSSParser::Type_selectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterType_selector(this);
}

void CSSParser::Type_selectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitType_selector(this);
}

CSSParser::Type_selectorContext* CSSParser::type_selector() {
  Type_selectorContext *_localctx = _tracker.createInstance<Type_selectorContext>(_ctx, getState());
  enterRule(_localctx, 62, CSSParser::RuleType_selector);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(360);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(353);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx)) {
      case 1: {
        setState(352);
        namespace_prefix();
        break;
      }

      default:
        break;
      }
      setState(355);
      element_name();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(357);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
      case 1: {
        setState(356);
        namespace_prefix();
        break;
      }

      default:
        break;
      }
      setState(359);
      match(CSSParser::MULTIPLY);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Namespace_prefixContext ------------------------------------------------------------------

CSSParser::Namespace_prefixContext::Namespace_prefixContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Namespace_prefixContext::PIPE() {
  return getToken(CSSParser::PIPE, 0);
}

tree::TerminalNode* CSSParser::Namespace_prefixContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

tree::TerminalNode* CSSParser::Namespace_prefixContext::MULTIPLY() {
  return getToken(CSSParser::MULTIPLY, 0);
}


size_t CSSParser::Namespace_prefixContext::getRuleIndex() const {
  return CSSParser::RuleNamespace_prefix;
}

void CSSParser::Namespace_prefixContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNamespace_prefix(this);
}

void CSSParser::Namespace_prefixContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNamespace_prefix(this);
}

CSSParser::Namespace_prefixContext* CSSParser::namespace_prefix() {
  Namespace_prefixContext *_localctx = _tracker.createInstance<Namespace_prefixContext>(_ctx, getState());
  enterRule(_localctx, 64, CSSParser::RuleNamespace_prefix);
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
    setState(363);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::MULTIPLY

    || _la == CSSParser::IDENT) {
      setState(362);
      _la = _input->LA(1);
      if (!(_la == CSSParser::MULTIPLY

      || _la == CSSParser::IDENT)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(365);
    match(CSSParser::PIPE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Element_nameContext ------------------------------------------------------------------

CSSParser::Element_nameContext::Element_nameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Element_nameContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::Element_nameContext::getRuleIndex() const {
  return CSSParser::RuleElement_name;
}

void CSSParser::Element_nameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElement_name(this);
}

void CSSParser::Element_nameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElement_name(this);
}

CSSParser::Element_nameContext* CSSParser::element_name() {
  Element_nameContext *_localctx = _tracker.createInstance<Element_nameContext>(_ctx, getState());
  enterRule(_localctx, 66, CSSParser::RuleElement_name);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(367);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- HashContext ------------------------------------------------------------------

CSSParser::HashContext::HashContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::HashContext::HASH() {
  return getToken(CSSParser::HASH, 0);
}

tree::TerminalNode* CSSParser::HashContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::HashContext::getRuleIndex() const {
  return CSSParser::RuleHash;
}

void CSSParser::HashContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterHash(this);
}

void CSSParser::HashContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitHash(this);
}

CSSParser::HashContext* CSSParser::hash() {
  HashContext *_localctx = _tracker.createInstance<HashContext>(_ctx, getState());
  enterRule(_localctx, 68, CSSParser::RuleHash);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(369);
    match(CSSParser::HASH);
    setState(370);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassContext ------------------------------------------------------------------

CSSParser::ClassContext::ClassContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::ClassContext::DOT() {
  return getToken(CSSParser::DOT, 0);
}

tree::TerminalNode* CSSParser::ClassContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::ClassContext::getRuleIndex() const {
  return CSSParser::RuleClass;
}

void CSSParser::ClassContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClass(this);
}

void CSSParser::ClassContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClass(this);
}

CSSParser::ClassContext* CSSParser::class_() {
  ClassContext *_localctx = _tracker.createInstance<ClassContext>(_ctx, getState());
  enterRule(_localctx, 70, CSSParser::RuleClass);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(372);
    match(CSSParser::DOT);
    setState(373);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttribContext ------------------------------------------------------------------

CSSParser::AttribContext::AttribContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::AttribContext::LBRACKET() {
  return getToken(CSSParser::LBRACKET, 0);
}

std::vector<tree::TerminalNode *> CSSParser::AttribContext::IDENT() {
  return getTokens(CSSParser::IDENT);
}

tree::TerminalNode* CSSParser::AttribContext::IDENT(size_t i) {
  return getToken(CSSParser::IDENT, i);
}

tree::TerminalNode* CSSParser::AttribContext::RBRACKET() {
  return getToken(CSSParser::RBRACKET, 0);
}

tree::TerminalNode* CSSParser::AttribContext::PREFIXMATCH() {
  return getToken(CSSParser::PREFIXMATCH, 0);
}

tree::TerminalNode* CSSParser::AttribContext::SUFFIXMATCH() {
  return getToken(CSSParser::SUFFIXMATCH, 0);
}

tree::TerminalNode* CSSParser::AttribContext::SUBSTRINGMATCH() {
  return getToken(CSSParser::SUBSTRINGMATCH, 0);
}

tree::TerminalNode* CSSParser::AttribContext::EQUALS() {
  return getToken(CSSParser::EQUALS, 0);
}

tree::TerminalNode* CSSParser::AttribContext::INCLUDES() {
  return getToken(CSSParser::INCLUDES, 0);
}

tree::TerminalNode* CSSParser::AttribContext::DASHMATCH() {
  return getToken(CSSParser::DASHMATCH, 0);
}

tree::TerminalNode* CSSParser::AttribContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}


size_t CSSParser::AttribContext::getRuleIndex() const {
  return CSSParser::RuleAttrib;
}

void CSSParser::AttribContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAttrib(this);
}

void CSSParser::AttribContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAttrib(this);
}

CSSParser::AttribContext* CSSParser::attrib() {
  AttribContext *_localctx = _tracker.createInstance<AttribContext>(_ctx, getState());
  enterRule(_localctx, 72, CSSParser::RuleAttrib);
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
    setState(375);
    match(CSSParser::LBRACKET);
    setState(376);
    match(CSSParser::IDENT);
    setState(379);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8257536) != 0)) {
      setState(377);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 8257536) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(378);
      _la = _input->LA(1);
      if (!(_la == CSSParser::STRING

      || _la == CSSParser::IDENT)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(381);
    match(CSSParser::RBRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PseudoContext ------------------------------------------------------------------

CSSParser::PseudoContext::PseudoContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> CSSParser::PseudoContext::COLON() {
  return getTokens(CSSParser::COLON);
}

tree::TerminalNode* CSSParser::PseudoContext::COLON(size_t i) {
  return getToken(CSSParser::COLON, i);
}

tree::TerminalNode* CSSParser::PseudoContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

CSSParser::Functional_pseudoContext* CSSParser::PseudoContext::functional_pseudo() {
  return getRuleContext<CSSParser::Functional_pseudoContext>(0);
}


size_t CSSParser::PseudoContext::getRuleIndex() const {
  return CSSParser::RulePseudo;
}

void CSSParser::PseudoContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPseudo(this);
}

void CSSParser::PseudoContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPseudo(this);
}

CSSParser::PseudoContext* CSSParser::pseudo() {
  PseudoContext *_localctx = _tracker.createInstance<PseudoContext>(_ctx, getState());
  enterRule(_localctx, 74, CSSParser::RulePseudo);
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
    setState(383);
    match(CSSParser::COLON);
    setState(385);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::COLON) {
      setState(384);
      match(CSSParser::COLON);
    }
    setState(389);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::IDENT: {
        setState(387);
        match(CSSParser::IDENT);
        break;
      }

      case CSSParser::FUNCTION: {
        setState(388);
        functional_pseudo();
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

//----------------- Functional_pseudoContext ------------------------------------------------------------------

CSSParser::Functional_pseudoContext::Functional_pseudoContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Functional_pseudoContext::FUNCTION() {
  return getToken(CSSParser::FUNCTION, 0);
}

CSSParser::ExprContext* CSSParser::Functional_pseudoContext::expr() {
  return getRuleContext<CSSParser::ExprContext>(0);
}

tree::TerminalNode* CSSParser::Functional_pseudoContext::RPAREN() {
  return getToken(CSSParser::RPAREN, 0);
}


size_t CSSParser::Functional_pseudoContext::getRuleIndex() const {
  return CSSParser::RuleFunctional_pseudo;
}

void CSSParser::Functional_pseudoContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctional_pseudo(this);
}

void CSSParser::Functional_pseudoContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctional_pseudo(this);
}

CSSParser::Functional_pseudoContext* CSSParser::functional_pseudo() {
  Functional_pseudoContext *_localctx = _tracker.createInstance<Functional_pseudoContext>(_ctx, getState());
  enterRule(_localctx, 76, CSSParser::RuleFunctional_pseudo);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(391);
    match(CSSParser::FUNCTION);
    setState(392);
    expr();
    setState(393);
    match(CSSParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NegationContext ------------------------------------------------------------------

CSSParser::NegationContext::NegationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::NegationContext::NOT_FUNCTION() {
  return getToken(CSSParser::NOT_FUNCTION, 0);
}

CSSParser::Negation_argContext* CSSParser::NegationContext::negation_arg() {
  return getRuleContext<CSSParser::Negation_argContext>(0);
}

tree::TerminalNode* CSSParser::NegationContext::RPAREN() {
  return getToken(CSSParser::RPAREN, 0);
}


size_t CSSParser::NegationContext::getRuleIndex() const {
  return CSSParser::RuleNegation;
}

void CSSParser::NegationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNegation(this);
}

void CSSParser::NegationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNegation(this);
}

CSSParser::NegationContext* CSSParser::negation() {
  NegationContext *_localctx = _tracker.createInstance<NegationContext>(_ctx, getState());
  enterRule(_localctx, 78, CSSParser::RuleNegation);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(395);
    match(CSSParser::NOT_FUNCTION);
    setState(396);
    negation_arg();
    setState(397);
    match(CSSParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Negation_argContext ------------------------------------------------------------------

CSSParser::Negation_argContext::Negation_argContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::Type_selectorContext* CSSParser::Negation_argContext::type_selector() {
  return getRuleContext<CSSParser::Type_selectorContext>(0);
}

CSSParser::HashContext* CSSParser::Negation_argContext::hash() {
  return getRuleContext<CSSParser::HashContext>(0);
}

CSSParser::ClassContext* CSSParser::Negation_argContext::class_() {
  return getRuleContext<CSSParser::ClassContext>(0);
}

CSSParser::AttribContext* CSSParser::Negation_argContext::attrib() {
  return getRuleContext<CSSParser::AttribContext>(0);
}

CSSParser::PseudoContext* CSSParser::Negation_argContext::pseudo() {
  return getRuleContext<CSSParser::PseudoContext>(0);
}


size_t CSSParser::Negation_argContext::getRuleIndex() const {
  return CSSParser::RuleNegation_arg;
}

void CSSParser::Negation_argContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNegation_arg(this);
}

void CSSParser::Negation_argContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNegation_arg(this);
}

CSSParser::Negation_argContext* CSSParser::negation_arg() {
  Negation_argContext *_localctx = _tracker.createInstance<Negation_argContext>(_ctx, getState());
  enterRule(_localctx, 80, CSSParser::RuleNegation_arg);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(404);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::MULTIPLY:
      case CSSParser::IDENT:
      case CSSParser::PIPE: {
        enterOuterAlt(_localctx, 1);
        setState(399);
        type_selector();
        break;
      }

      case CSSParser::HASH: {
        enterOuterAlt(_localctx, 2);
        setState(400);
        hash();
        break;
      }

      case CSSParser::DOT: {
        enterOuterAlt(_localctx, 3);
        setState(401);
        class_();
        break;
      }

      case CSSParser::LBRACKET: {
        enterOuterAlt(_localctx, 4);
        setState(402);
        attrib();
        break;
      }

      case CSSParser::COLON: {
        enterOuterAlt(_localctx, 5);
        setState(403);
        pseudo();
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

//----------------- Declaration_listContext ------------------------------------------------------------------

CSSParser::Declaration_listContext::Declaration_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::DeclarationContext *> CSSParser::Declaration_listContext::declaration() {
  return getRuleContexts<CSSParser::DeclarationContext>();
}

CSSParser::DeclarationContext* CSSParser::Declaration_listContext::declaration(size_t i) {
  return getRuleContext<CSSParser::DeclarationContext>(i);
}

std::vector<tree::TerminalNode *> CSSParser::Declaration_listContext::SEMICOLON() {
  return getTokens(CSSParser::SEMICOLON);
}

tree::TerminalNode* CSSParser::Declaration_listContext::SEMICOLON(size_t i) {
  return getToken(CSSParser::SEMICOLON, i);
}


size_t CSSParser::Declaration_listContext::getRuleIndex() const {
  return CSSParser::RuleDeclaration_list;
}

void CSSParser::Declaration_listContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeclaration_list(this);
}

void CSSParser::Declaration_listContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDeclaration_list(this);
}

CSSParser::Declaration_listContext* CSSParser::declaration_list() {
  Declaration_listContext *_localctx = _tracker.createInstance<Declaration_listContext>(_ctx, getState());
  enterRule(_localctx, 82, CSSParser::RuleDeclaration_list);
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
    setState(407);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::IDENT) {
      setState(406);
      declaration();
    }
    setState(415);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSSParser::SEMICOLON) {
      setState(409);
      match(CSSParser::SEMICOLON);
      setState(411);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == CSSParser::IDENT) {
        setState(410);
        declaration();
      }
      setState(417);
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

//----------------- DeclarationContext ------------------------------------------------------------------

CSSParser::DeclarationContext::DeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::PropertyContext* CSSParser::DeclarationContext::property() {
  return getRuleContext<CSSParser::PropertyContext>(0);
}

tree::TerminalNode* CSSParser::DeclarationContext::COLON() {
  return getToken(CSSParser::COLON, 0);
}

CSSParser::ExprContext* CSSParser::DeclarationContext::expr() {
  return getRuleContext<CSSParser::ExprContext>(0);
}

CSSParser::PrioContext* CSSParser::DeclarationContext::prio() {
  return getRuleContext<CSSParser::PrioContext>(0);
}


size_t CSSParser::DeclarationContext::getRuleIndex() const {
  return CSSParser::RuleDeclaration;
}

void CSSParser::DeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeclaration(this);
}

void CSSParser::DeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDeclaration(this);
}

CSSParser::DeclarationContext* CSSParser::declaration() {
  DeclarationContext *_localctx = _tracker.createInstance<DeclarationContext>(_ctx, getState());
  enterRule(_localctx, 84, CSSParser::RuleDeclaration);
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
    setState(418);
    property();
    setState(419);
    match(CSSParser::COLON);
    setState(420);
    expr();
    setState(422);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::IMPORTANT) {
      setState(421);
      prio();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrioContext ------------------------------------------------------------------

CSSParser::PrioContext::PrioContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::PrioContext::IMPORTANT() {
  return getToken(CSSParser::IMPORTANT, 0);
}


size_t CSSParser::PrioContext::getRuleIndex() const {
  return CSSParser::RulePrio;
}

void CSSParser::PrioContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrio(this);
}

void CSSParser::PrioContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrio(this);
}

CSSParser::PrioContext* CSSParser::prio() {
  PrioContext *_localctx = _tracker.createInstance<PrioContext>(_ctx, getState());
  enterRule(_localctx, 86, CSSParser::RulePrio);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(424);
    match(CSSParser::IMPORTANT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyContext ------------------------------------------------------------------

CSSParser::PropertyContext::PropertyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::PropertyContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::PropertyContext::getRuleIndex() const {
  return CSSParser::RuleProperty;
}

void CSSParser::PropertyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProperty(this);
}

void CSSParser::PropertyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProperty(this);
}

CSSParser::PropertyContext* CSSParser::property() {
  PropertyContext *_localctx = _tracker.createInstance<PropertyContext>(_ctx, getState());
  enterRule(_localctx, 88, CSSParser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(426);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

CSSParser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::TermContext *> CSSParser::ExprContext::term() {
  return getRuleContexts<CSSParser::TermContext>();
}

CSSParser::TermContext* CSSParser::ExprContext::term(size_t i) {
  return getRuleContext<CSSParser::TermContext>(i);
}

std::vector<CSSParser::OperatorContext *> CSSParser::ExprContext::operator_() {
  return getRuleContexts<CSSParser::OperatorContext>();
}

CSSParser::OperatorContext* CSSParser::ExprContext::operator_(size_t i) {
  return getRuleContext<CSSParser::OperatorContext>(i);
}


size_t CSSParser::ExprContext::getRuleIndex() const {
  return CSSParser::RuleExpr;
}

void CSSParser::ExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpr(this);
}

void CSSParser::ExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpr(this);
}

CSSParser::ExprContext* CSSParser::expr() {
  ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, getState());
  enterRule(_localctx, 90, CSSParser::RuleExpr);
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
    setState(428);
    term();
    setState(434);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1065445888) != 0)) {
      setState(429);
      operator_();
      setState(430);
      term();
      setState(436);
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

//----------------- OperatorContext ------------------------------------------------------------------

CSSParser::OperatorContext::OperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::OperatorContext::DIVIDE() {
  return getToken(CSSParser::DIVIDE, 0);
}

tree::TerminalNode* CSSParser::OperatorContext::COMMA() {
  return getToken(CSSParser::COMMA, 0);
}


size_t CSSParser::OperatorContext::getRuleIndex() const {
  return CSSParser::RuleOperator;
}

void CSSParser::OperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOperator(this);
}

void CSSParser::OperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOperator(this);
}

CSSParser::OperatorContext* CSSParser::operator_() {
  OperatorContext *_localctx = _tracker.createInstance<OperatorContext>(_ctx, getState());
  enterRule(_localctx, 92, CSSParser::RuleOperator);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(440);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::DIVIDE: {
        enterOuterAlt(_localctx, 1);
        setState(437);
        match(CSSParser::DIVIDE);
        break;
      }

      case CSSParser::COMMA: {
        enterOuterAlt(_localctx, 2);
        setState(438);
        match(CSSParser::COMMA);
        break;
      }

      case CSSParser::HASH:
      case CSSParser::PLUS:
      case CSSParser::MINUS:
      case CSSParser::STRING:
      case CSSParser::NUMBER:
      case CSSParser::PERCENTAGE:
      case CSSParser::IDENT:
      case CSSParser::DIMENSION:
      case CSSParser::FUNCTION:
      case CSSParser::URI: {
        enterOuterAlt(_localctx, 3);

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

//----------------- TermContext ------------------------------------------------------------------

CSSParser::TermContext::TermContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::TermContext::NUMBER() {
  return getToken(CSSParser::NUMBER, 0);
}

tree::TerminalNode* CSSParser::TermContext::PERCENTAGE() {
  return getToken(CSSParser::PERCENTAGE, 0);
}

tree::TerminalNode* CSSParser::TermContext::DIMENSION() {
  return getToken(CSSParser::DIMENSION, 0);
}

tree::TerminalNode* CSSParser::TermContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}

tree::TerminalNode* CSSParser::TermContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

tree::TerminalNode* CSSParser::TermContext::URI() {
  return getToken(CSSParser::URI, 0);
}

tree::TerminalNode* CSSParser::TermContext::HASH() {
  return getToken(CSSParser::HASH, 0);
}

CSSParser::FunctionContext* CSSParser::TermContext::function() {
  return getRuleContext<CSSParser::FunctionContext>(0);
}

CSSParser::Unary_operatorContext* CSSParser::TermContext::unary_operator() {
  return getRuleContext<CSSParser::Unary_operatorContext>(0);
}


size_t CSSParser::TermContext::getRuleIndex() const {
  return CSSParser::RuleTerm;
}

void CSSParser::TermContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTerm(this);
}

void CSSParser::TermContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTerm(this);
}

CSSParser::TermContext* CSSParser::term() {
  TermContext *_localctx = _tracker.createInstance<TermContext>(_ctx, getState());
  enterRule(_localctx, 94, CSSParser::RuleTerm);
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
    setState(443);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::PLUS

    || _la == CSSParser::MINUS) {
      setState(442);
      unary_operator();
    }
    setState(456);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::NUMBER: {
        setState(445);
        match(CSSParser::NUMBER);
        break;
      }

      case CSSParser::PERCENTAGE: {
        setState(446);
        match(CSSParser::PERCENTAGE);
        break;
      }

      case CSSParser::DIMENSION: {
        setState(447);
        match(CSSParser::DIMENSION);
        break;
      }

      case CSSParser::STRING: {
        setState(448);
        match(CSSParser::STRING);
        break;
      }

      case CSSParser::IDENT: {
        setState(449);
        match(CSSParser::IDENT);
        break;
      }

      case CSSParser::URI: {
        setState(450);
        match(CSSParser::URI);
        break;
      }

      case CSSParser::HASH: {
        setState(451);
        match(CSSParser::HASH);
        setState(453);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx)) {
        case 1: {
          setState(452);
          match(CSSParser::IDENT);
          break;
        }

        default:
          break;
        }
        break;
      }

      case CSSParser::FUNCTION: {
        setState(455);
        function();
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

//----------------- Unary_operatorContext ------------------------------------------------------------------

CSSParser::Unary_operatorContext::Unary_operatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Unary_operatorContext::MINUS() {
  return getToken(CSSParser::MINUS, 0);
}

tree::TerminalNode* CSSParser::Unary_operatorContext::PLUS() {
  return getToken(CSSParser::PLUS, 0);
}


size_t CSSParser::Unary_operatorContext::getRuleIndex() const {
  return CSSParser::RuleUnary_operator;
}

void CSSParser::Unary_operatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnary_operator(this);
}

void CSSParser::Unary_operatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnary_operator(this);
}

CSSParser::Unary_operatorContext* CSSParser::unary_operator() {
  Unary_operatorContext *_localctx = _tracker.createInstance<Unary_operatorContext>(_ctx, getState());
  enterRule(_localctx, 96, CSSParser::RuleUnary_operator);
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
    setState(458);
    _la = _input->LA(1);
    if (!(_la == CSSParser::PLUS

    || _la == CSSParser::MINUS)) {
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

//----------------- FunctionContext ------------------------------------------------------------------

CSSParser::FunctionContext::FunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::FunctionContext::FUNCTION() {
  return getToken(CSSParser::FUNCTION, 0);
}

CSSParser::ExprContext* CSSParser::FunctionContext::expr() {
  return getRuleContext<CSSParser::ExprContext>(0);
}

tree::TerminalNode* CSSParser::FunctionContext::RPAREN() {
  return getToken(CSSParser::RPAREN, 0);
}


size_t CSSParser::FunctionContext::getRuleIndex() const {
  return CSSParser::RuleFunction;
}

void CSSParser::FunctionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunction(this);
}

void CSSParser::FunctionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunction(this);
}

CSSParser::FunctionContext* CSSParser::function() {
  FunctionContext *_localctx = _tracker.createInstance<FunctionContext>(_ctx, getState());
  enterRule(_localctx, 98, CSSParser::RuleFunction);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(460);
    match(CSSParser::FUNCTION);
    setState(461);
    expr();
    setState(462);
    match(CSSParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AnyContext ------------------------------------------------------------------

CSSParser::AnyContext::AnyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::AnyContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

tree::TerminalNode* CSSParser::AnyContext::NUMBER() {
  return getToken(CSSParser::NUMBER, 0);
}

tree::TerminalNode* CSSParser::AnyContext::PERCENTAGE() {
  return getToken(CSSParser::PERCENTAGE, 0);
}

tree::TerminalNode* CSSParser::AnyContext::DIMENSION() {
  return getToken(CSSParser::DIMENSION, 0);
}

tree::TerminalNode* CSSParser::AnyContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}

tree::TerminalNode* CSSParser::AnyContext::URI() {
  return getToken(CSSParser::URI, 0);
}

tree::TerminalNode* CSSParser::AnyContext::HASH() {
  return getToken(CSSParser::HASH, 0);
}

tree::TerminalNode* CSSParser::AnyContext::PLUS() {
  return getToken(CSSParser::PLUS, 0);
}

tree::TerminalNode* CSSParser::AnyContext::MINUS() {
  return getToken(CSSParser::MINUS, 0);
}

tree::TerminalNode* CSSParser::AnyContext::MULTIPLY() {
  return getToken(CSSParser::MULTIPLY, 0);
}

tree::TerminalNode* CSSParser::AnyContext::DIVIDE() {
  return getToken(CSSParser::DIVIDE, 0);
}

tree::TerminalNode* CSSParser::AnyContext::EQUALS() {
  return getToken(CSSParser::EQUALS, 0);
}

tree::TerminalNode* CSSParser::AnyContext::COLON() {
  return getToken(CSSParser::COLON, 0);
}

tree::TerminalNode* CSSParser::AnyContext::SEMICOLON() {
  return getToken(CSSParser::SEMICOLON, 0);
}

tree::TerminalNode* CSSParser::AnyContext::COMMA() {
  return getToken(CSSParser::COMMA, 0);
}

tree::TerminalNode* CSSParser::AnyContext::DOT() {
  return getToken(CSSParser::DOT, 0);
}

CSSParser::FunctionContext* CSSParser::AnyContext::function() {
  return getRuleContext<CSSParser::FunctionContext>(0);
}

tree::TerminalNode* CSSParser::AnyContext::LPAREN() {
  return getToken(CSSParser::LPAREN, 0);
}

tree::TerminalNode* CSSParser::AnyContext::RPAREN() {
  return getToken(CSSParser::RPAREN, 0);
}

std::vector<CSSParser::AnyContext *> CSSParser::AnyContext::any() {
  return getRuleContexts<CSSParser::AnyContext>();
}

CSSParser::AnyContext* CSSParser::AnyContext::any(size_t i) {
  return getRuleContext<CSSParser::AnyContext>(i);
}

tree::TerminalNode* CSSParser::AnyContext::LBRACKET() {
  return getToken(CSSParser::LBRACKET, 0);
}

tree::TerminalNode* CSSParser::AnyContext::RBRACKET() {
  return getToken(CSSParser::RBRACKET, 0);
}

tree::TerminalNode* CSSParser::AnyContext::LBRACE() {
  return getToken(CSSParser::LBRACE, 0);
}

tree::TerminalNode* CSSParser::AnyContext::RBRACE() {
  return getToken(CSSParser::RBRACE, 0);
}

tree::TerminalNode* CSSParser::AnyContext::ANYCHAR() {
  return getToken(CSSParser::ANYCHAR, 0);
}


size_t CSSParser::AnyContext::getRuleIndex() const {
  return CSSParser::RuleAny;
}

void CSSParser::AnyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAny(this);
}

void CSSParser::AnyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAny(this);
}

CSSParser::AnyContext* CSSParser::any() {
  AnyContext *_localctx = _tracker.createInstance<AnyContext>(_ctx, getState());
  enterRule(_localctx, 100, CSSParser::RuleAny);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(506);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::IDENT: {
        enterOuterAlt(_localctx, 1);
        setState(464);
        match(CSSParser::IDENT);
        break;
      }

      case CSSParser::NUMBER: {
        enterOuterAlt(_localctx, 2);
        setState(465);
        match(CSSParser::NUMBER);
        break;
      }

      case CSSParser::PERCENTAGE: {
        enterOuterAlt(_localctx, 3);
        setState(466);
        match(CSSParser::PERCENTAGE);
        break;
      }

      case CSSParser::DIMENSION: {
        enterOuterAlt(_localctx, 4);
        setState(467);
        match(CSSParser::DIMENSION);
        break;
      }

      case CSSParser::STRING: {
        enterOuterAlt(_localctx, 5);
        setState(468);
        match(CSSParser::STRING);
        break;
      }

      case CSSParser::URI: {
        enterOuterAlt(_localctx, 6);
        setState(469);
        match(CSSParser::URI);
        break;
      }

      case CSSParser::HASH: {
        enterOuterAlt(_localctx, 7);
        setState(470);
        match(CSSParser::HASH);
        break;
      }

      case CSSParser::PLUS: {
        enterOuterAlt(_localctx, 8);
        setState(471);
        match(CSSParser::PLUS);
        break;
      }

      case CSSParser::MINUS: {
        enterOuterAlt(_localctx, 9);
        setState(472);
        match(CSSParser::MINUS);
        break;
      }

      case CSSParser::MULTIPLY: {
        enterOuterAlt(_localctx, 10);
        setState(473);
        match(CSSParser::MULTIPLY);
        break;
      }

      case CSSParser::DIVIDE: {
        enterOuterAlt(_localctx, 11);
        setState(474);
        match(CSSParser::DIVIDE);
        break;
      }

      case CSSParser::EQUALS: {
        enterOuterAlt(_localctx, 12);
        setState(475);
        match(CSSParser::EQUALS);
        break;
      }

      case CSSParser::COLON: {
        enterOuterAlt(_localctx, 13);
        setState(476);
        match(CSSParser::COLON);
        break;
      }

      case CSSParser::SEMICOLON: {
        enterOuterAlt(_localctx, 14);
        setState(477);
        match(CSSParser::SEMICOLON);
        break;
      }

      case CSSParser::COMMA: {
        enterOuterAlt(_localctx, 15);
        setState(478);
        match(CSSParser::COMMA);
        break;
      }

      case CSSParser::DOT: {
        enterOuterAlt(_localctx, 16);
        setState(479);
        match(CSSParser::DOT);
        break;
      }

      case CSSParser::FUNCTION: {
        enterOuterAlt(_localctx, 17);
        setState(480);
        function();
        break;
      }

      case CSSParser::LPAREN: {
        enterOuterAlt(_localctx, 18);
        setState(481);
        match(CSSParser::LPAREN);
        setState(485);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 4503600692981674) != 0)) {
          setState(482);
          any();
          setState(487);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(488);
        match(CSSParser::RPAREN);
        break;
      }

      case CSSParser::LBRACKET: {
        enterOuterAlt(_localctx, 19);
        setState(489);
        match(CSSParser::LBRACKET);
        setState(493);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 4503600692981674) != 0)) {
          setState(490);
          any();
          setState(495);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(496);
        match(CSSParser::RBRACKET);
        break;
      }

      case CSSParser::LBRACE: {
        enterOuterAlt(_localctx, 20);
        setState(497);
        match(CSSParser::LBRACE);
        setState(501);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 4503600692981674) != 0)) {
          setState(498);
          any();
          setState(503);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(504);
        match(CSSParser::RBRACE);
        break;
      }

      case CSSParser::ANYCHAR: {
        enterOuterAlt(_localctx, 21);
        setState(505);
        match(CSSParser::ANYCHAR);
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

void CSSParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  cssparserParserInitialize();
#else
  ::antlr4::internal::call_once(cssparserParserOnceFlag, cssparserParserInitialize);
#endif
}
