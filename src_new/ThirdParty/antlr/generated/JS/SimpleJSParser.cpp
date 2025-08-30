
// Generated from third-party/antlr/Grammars/JavaScript/SimpleJSParser.g4 by ANTLR 4.13.2


#include "SimpleJSParserListener.h"

#include "SimpleJSParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct SimpleJSParserStaticData final {
  SimpleJSParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  SimpleJSParserStaticData(const SimpleJSParserStaticData&) = delete;
  SimpleJSParserStaticData(SimpleJSParserStaticData&&) = delete;
  SimpleJSParserStaticData& operator=(const SimpleJSParserStaticData&) = delete;
  SimpleJSParserStaticData& operator=(SimpleJSParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag simplejsparserParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<SimpleJSParserStaticData> simplejsparserParserStaticData = nullptr;

void simplejsparserParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (simplejsparserParserStaticData != nullptr) {
    return;
  }
#else
  assert(simplejsparserParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<SimpleJSParserStaticData>(
    std::vector<std::string>{
      "program", "statement", "block", "varDeclaration", "varDeclarator", 
      "functionDeclaration", "classDeclaration", "classElement", "methodDefinition", 
      "propertyDefinition", "parameterList", "expressionStatement", "ifStatement", 
      "whileStatement", "forStatement", "returnStatement", "throwStatement", 
      "tryStatement", "switchStatement", "caseClause", "defaultClause", 
      "breakStatement", "continueStatement", "emptyStatement", "expression", 
      "assignmentExpression", "conditionalExpression", "logicalOrExpression", 
      "logicalAndExpression", "equalityExpression", "relationalExpression", 
      "additiveExpression", "multiplicativeExpression", "unaryExpression", 
      "postfixExpression", "leftHandSideExpression", "callExpression", "memberExpression", 
      "primaryExpression", "functionExpression", "arrowFunction", "arguments", 
      "arrayLiteral", "objectLiteral", "propertyAssignment", "literal"
    },
    std::vector<std::string>{
      "", "'break'", "'case'", "'catch'", "'class'", "'const'", "'continue'", 
      "'default'", "'delete'", "'do'", "'else'", "'export'", "'extends'", 
      "'finally'", "'for'", "'function'", "'if'", "'import'", "'in'", "'instanceof'", 
      "'let'", "'new'", "'return'", "'super'", "'switch'", "'this'", "'throw'", 
      "'try'", "'typeof'", "'var'", "'void'", "'while'", "'with'", "'async'", 
      "'await'", "'yield'", "'of'", "'from'", "'as'", "'null'", "'true'", 
      "'false'", "'undefined'", "'='", "'+='", "'-='", "'*='", "'/='", "'%='", 
      "'++'", "'--'", "'+'", "'-'", "'*'", "'/'", "'%'", "'=='", "'!='", 
      "'==='", "'!=='", "'<'", "'<='", "'>'", "'>='", "'&&'", "'||'", "'!'", 
      "'&'", "'|'", "'^'", "'~'", "'<<'", "'>>'", "'>>>'", "'\\u003F'", 
      "':'", "'=>'", "';'", "','", "'.'", "'('", "')'", "'{'", "'}'", "'['", 
      "']'"
    },
    std::vector<std::string>{
      "", "BREAK", "CASE", "CATCH", "CLASS", "CONST", "CONTINUE", "DEFAULT", 
      "DELETE", "DO", "ELSE", "EXPORT", "EXTENDS", "FINALLY", "FOR", "FUNCTION", 
      "IF", "IMPORT", "IN", "INSTANCEOF", "LET", "NEW", "RETURN", "SUPER", 
      "SWITCH", "THIS", "THROW", "TRY", "TYPEOF", "VAR", "VOID", "WHILE", 
      "WITH", "ASYNC", "AWAIT", "YIELD", "OF", "FROM", "AS", "NULL", "TRUE", 
      "FALSE", "UNDEFINED", "ASSIGN", "PLUS_ASSIGN", "MINUS_ASSIGN", "MULT_ASSIGN", 
      "DIV_ASSIGN", "MOD_ASSIGN", "PLUS_PLUS", "MINUS_MINUS", "PLUS", "MINUS", 
      "MULTIPLY", "DIVIDE", "MODULUS", "EQ", "NE", "EQ_STRICT", "NE_STRICT", 
      "LT", "LE", "GT", "GE", "AND", "OR", "NOT", "BIT_AND", "BIT_OR", "BIT_XOR", 
      "BIT_NOT", "LSHIFT", "RSHIFT", "URSHIFT", "QUESTION", "COLON", "ARROW", 
      "SEMICOLON", "COMMA", "DOT", "LPAREN", "RPAREN", "LBRACE", "RBRACE", 
      "LBRACKET", "RBRACKET", "NUMBER", "STRING", "TEMPLATE", "REGEX", "IDENTIFIER", 
      "LINE_COMMENT", "BLOCK_COMMENT", "WS", "ANY"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,94,546,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,1,0,5,0,94,8,0,10,0,12,0,97,9,0,1,0,
  	1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,116,
  	8,1,1,2,1,2,5,2,120,8,2,10,2,12,2,123,9,2,1,2,1,2,1,3,1,3,1,3,1,3,5,3,
  	131,8,3,10,3,12,3,134,9,3,1,3,3,3,137,8,3,1,4,1,4,1,4,3,4,142,8,4,1,5,
  	3,5,145,8,5,1,5,1,5,1,5,1,5,3,5,151,8,5,1,5,1,5,1,5,1,6,1,6,1,6,1,6,3,
  	6,160,8,6,1,6,1,6,5,6,164,8,6,10,6,12,6,167,9,6,1,6,1,6,1,7,1,7,3,7,173,
  	8,7,1,8,1,8,1,8,3,8,178,8,8,1,8,1,8,1,8,1,9,1,9,1,9,3,9,186,8,9,1,9,3,
  	9,189,8,9,1,10,1,10,1,10,5,10,194,8,10,10,10,12,10,197,9,10,1,11,1,11,
  	3,11,201,8,11,1,12,1,12,1,12,1,12,1,12,1,12,1,12,3,12,210,8,12,1,13,1,
  	13,1,13,1,13,1,13,1,13,1,14,1,14,1,14,1,14,3,14,222,8,14,1,14,1,14,3,
  	14,226,8,14,1,14,1,14,3,14,230,8,14,1,14,1,14,1,14,1,14,1,14,1,14,3,14,
  	238,8,14,1,14,1,14,1,14,1,14,1,14,3,14,245,8,14,1,15,1,15,3,15,249,8,
  	15,1,15,3,15,252,8,15,1,16,1,16,1,16,3,16,257,8,16,1,17,1,17,1,17,1,17,
  	1,17,3,17,264,8,17,1,17,1,17,3,17,268,8,17,1,17,1,17,3,17,272,8,17,1,
  	18,1,18,1,18,1,18,1,18,1,18,5,18,280,8,18,10,18,12,18,283,9,18,1,18,3,
  	18,286,8,18,1,18,5,18,289,8,18,10,18,12,18,292,9,18,1,18,1,18,1,19,1,
  	19,1,19,1,19,5,19,300,8,19,10,19,12,19,303,9,19,1,20,1,20,1,20,5,20,308,
  	8,20,10,20,12,20,311,9,20,1,21,1,21,3,21,315,8,21,1,22,1,22,3,22,319,
  	8,22,1,23,1,23,1,24,1,24,1,25,1,25,1,25,1,25,1,25,3,25,330,8,25,1,26,
  	1,26,1,26,1,26,1,26,1,26,3,26,338,8,26,1,27,1,27,1,27,5,27,343,8,27,10,
  	27,12,27,346,9,27,1,28,1,28,1,28,5,28,351,8,28,10,28,12,28,354,9,28,1,
  	29,1,29,1,29,5,29,359,8,29,10,29,12,29,362,9,29,1,30,1,30,1,30,5,30,367,
  	8,30,10,30,12,30,370,9,30,1,31,1,31,1,31,5,31,375,8,31,10,31,12,31,378,
  	9,31,1,32,1,32,1,32,5,32,383,8,32,10,32,12,32,386,9,32,1,33,1,33,1,33,
  	1,33,1,33,1,33,1,33,3,33,395,8,33,1,34,1,34,3,34,399,8,34,1,35,1,35,3,
  	35,403,8,35,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,36,1,
  	36,1,36,1,36,5,36,419,8,36,10,36,12,36,422,9,36,1,37,1,37,1,37,1,37,1,
  	37,3,37,429,8,37,3,37,431,8,37,1,37,1,37,1,37,1,37,1,37,1,37,1,37,1,37,
  	5,37,441,8,37,10,37,12,37,444,9,37,1,38,1,38,1,38,1,38,1,38,1,38,1,38,
  	1,38,1,38,1,38,1,38,1,38,3,38,458,8,38,1,39,3,39,461,8,39,1,39,1,39,3,
  	39,465,8,39,1,39,1,39,3,39,469,8,39,1,39,1,39,1,39,1,40,1,40,1,40,3,40,
  	477,8,40,1,40,3,40,480,8,40,1,40,1,40,1,40,3,40,485,8,40,1,41,1,41,1,
  	41,1,41,5,41,491,8,41,10,41,12,41,494,9,41,3,41,496,8,41,1,41,1,41,1,
  	42,1,42,1,42,1,42,5,42,504,8,42,10,42,12,42,507,9,42,3,42,509,8,42,1,
  	42,1,42,1,43,1,43,1,43,1,43,5,43,517,8,43,10,43,12,43,520,9,43,3,43,522,
  	8,43,1,43,1,43,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,
  	1,44,3,44,538,8,44,1,44,1,44,3,44,542,8,44,1,45,1,45,1,45,0,2,72,74,46,
  	0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,
  	50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,0,10,3,
  	0,5,5,20,20,29,29,2,0,18,18,36,36,1,0,43,48,1,0,56,59,2,0,18,19,60,63,
  	1,0,51,52,1,0,53,55,1,0,49,50,6,0,8,8,28,28,30,30,51,52,66,66,70,70,2,
  	0,39,42,86,89,592,0,95,1,0,0,0,2,115,1,0,0,0,4,117,1,0,0,0,6,126,1,0,
  	0,0,8,138,1,0,0,0,10,144,1,0,0,0,12,155,1,0,0,0,14,172,1,0,0,0,16,174,
  	1,0,0,0,18,182,1,0,0,0,20,190,1,0,0,0,22,198,1,0,0,0,24,202,1,0,0,0,26,
  	211,1,0,0,0,28,244,1,0,0,0,30,246,1,0,0,0,32,253,1,0,0,0,34,258,1,0,0,
  	0,36,273,1,0,0,0,38,295,1,0,0,0,40,304,1,0,0,0,42,312,1,0,0,0,44,316,
  	1,0,0,0,46,320,1,0,0,0,48,322,1,0,0,0,50,329,1,0,0,0,52,331,1,0,0,0,54,
  	339,1,0,0,0,56,347,1,0,0,0,58,355,1,0,0,0,60,363,1,0,0,0,62,371,1,0,0,
  	0,64,379,1,0,0,0,66,394,1,0,0,0,68,396,1,0,0,0,70,402,1,0,0,0,72,404,
  	1,0,0,0,74,430,1,0,0,0,76,457,1,0,0,0,78,460,1,0,0,0,80,479,1,0,0,0,82,
  	486,1,0,0,0,84,499,1,0,0,0,86,512,1,0,0,0,88,541,1,0,0,0,90,543,1,0,0,
  	0,92,94,3,2,1,0,93,92,1,0,0,0,94,97,1,0,0,0,95,93,1,0,0,0,95,96,1,0,0,
  	0,96,98,1,0,0,0,97,95,1,0,0,0,98,99,5,0,0,1,99,1,1,0,0,0,100,116,3,4,
  	2,0,101,116,3,6,3,0,102,116,3,10,5,0,103,116,3,12,6,0,104,116,3,22,11,
  	0,105,116,3,24,12,0,106,116,3,26,13,0,107,116,3,28,14,0,108,116,3,30,
  	15,0,109,116,3,32,16,0,110,116,3,34,17,0,111,116,3,36,18,0,112,116,3,
  	42,21,0,113,116,3,44,22,0,114,116,3,46,23,0,115,100,1,0,0,0,115,101,1,
  	0,0,0,115,102,1,0,0,0,115,103,1,0,0,0,115,104,1,0,0,0,115,105,1,0,0,0,
  	115,106,1,0,0,0,115,107,1,0,0,0,115,108,1,0,0,0,115,109,1,0,0,0,115,110,
  	1,0,0,0,115,111,1,0,0,0,115,112,1,0,0,0,115,113,1,0,0,0,115,114,1,0,0,
  	0,116,3,1,0,0,0,117,121,5,82,0,0,118,120,3,2,1,0,119,118,1,0,0,0,120,
  	123,1,0,0,0,121,119,1,0,0,0,121,122,1,0,0,0,122,124,1,0,0,0,123,121,1,
  	0,0,0,124,125,5,83,0,0,125,5,1,0,0,0,126,127,7,0,0,0,127,132,3,8,4,0,
  	128,129,5,78,0,0,129,131,3,8,4,0,130,128,1,0,0,0,131,134,1,0,0,0,132,
  	130,1,0,0,0,132,133,1,0,0,0,133,136,1,0,0,0,134,132,1,0,0,0,135,137,5,
  	77,0,0,136,135,1,0,0,0,136,137,1,0,0,0,137,7,1,0,0,0,138,141,5,90,0,0,
  	139,140,5,43,0,0,140,142,3,48,24,0,141,139,1,0,0,0,141,142,1,0,0,0,142,
  	9,1,0,0,0,143,145,5,33,0,0,144,143,1,0,0,0,144,145,1,0,0,0,145,146,1,
  	0,0,0,146,147,5,15,0,0,147,148,5,90,0,0,148,150,5,80,0,0,149,151,3,20,
  	10,0,150,149,1,0,0,0,150,151,1,0,0,0,151,152,1,0,0,0,152,153,5,81,0,0,
  	153,154,3,4,2,0,154,11,1,0,0,0,155,156,5,4,0,0,156,159,5,90,0,0,157,158,
  	5,12,0,0,158,160,5,90,0,0,159,157,1,0,0,0,159,160,1,0,0,0,160,161,1,0,
  	0,0,161,165,5,82,0,0,162,164,3,14,7,0,163,162,1,0,0,0,164,167,1,0,0,0,
  	165,163,1,0,0,0,165,166,1,0,0,0,166,168,1,0,0,0,167,165,1,0,0,0,168,169,
  	5,83,0,0,169,13,1,0,0,0,170,173,3,16,8,0,171,173,3,18,9,0,172,170,1,0,
  	0,0,172,171,1,0,0,0,173,15,1,0,0,0,174,175,5,90,0,0,175,177,5,80,0,0,
  	176,178,3,20,10,0,177,176,1,0,0,0,177,178,1,0,0,0,178,179,1,0,0,0,179,
  	180,5,81,0,0,180,181,3,4,2,0,181,17,1,0,0,0,182,185,5,90,0,0,183,184,
  	5,43,0,0,184,186,3,48,24,0,185,183,1,0,0,0,185,186,1,0,0,0,186,188,1,
  	0,0,0,187,189,5,77,0,0,188,187,1,0,0,0,188,189,1,0,0,0,189,19,1,0,0,0,
  	190,195,5,90,0,0,191,192,5,78,0,0,192,194,5,90,0,0,193,191,1,0,0,0,194,
  	197,1,0,0,0,195,193,1,0,0,0,195,196,1,0,0,0,196,21,1,0,0,0,197,195,1,
  	0,0,0,198,200,3,48,24,0,199,201,5,77,0,0,200,199,1,0,0,0,200,201,1,0,
  	0,0,201,23,1,0,0,0,202,203,5,16,0,0,203,204,5,80,0,0,204,205,3,48,24,
  	0,205,206,5,81,0,0,206,209,3,2,1,0,207,208,5,10,0,0,208,210,3,2,1,0,209,
  	207,1,0,0,0,209,210,1,0,0,0,210,25,1,0,0,0,211,212,5,31,0,0,212,213,5,
  	80,0,0,213,214,3,48,24,0,214,215,5,81,0,0,215,216,3,2,1,0,216,27,1,0,
  	0,0,217,218,5,14,0,0,218,221,5,80,0,0,219,222,3,6,3,0,220,222,3,48,24,
  	0,221,219,1,0,0,0,221,220,1,0,0,0,221,222,1,0,0,0,222,223,1,0,0,0,223,
  	225,5,77,0,0,224,226,3,48,24,0,225,224,1,0,0,0,225,226,1,0,0,0,226,227,
  	1,0,0,0,227,229,5,77,0,0,228,230,3,48,24,0,229,228,1,0,0,0,229,230,1,
  	0,0,0,230,231,1,0,0,0,231,232,5,81,0,0,232,245,3,2,1,0,233,234,5,14,0,
  	0,234,237,5,80,0,0,235,238,3,6,3,0,236,238,3,48,24,0,237,235,1,0,0,0,
  	237,236,1,0,0,0,238,239,1,0,0,0,239,240,7,1,0,0,240,241,3,48,24,0,241,
  	242,5,81,0,0,242,243,3,2,1,0,243,245,1,0,0,0,244,217,1,0,0,0,244,233,
  	1,0,0,0,245,29,1,0,0,0,246,248,5,22,0,0,247,249,3,48,24,0,248,247,1,0,
  	0,0,248,249,1,0,0,0,249,251,1,0,0,0,250,252,5,77,0,0,251,250,1,0,0,0,
  	251,252,1,0,0,0,252,31,1,0,0,0,253,254,5,26,0,0,254,256,3,48,24,0,255,
  	257,5,77,0,0,256,255,1,0,0,0,256,257,1,0,0,0,257,33,1,0,0,0,258,259,5,
  	27,0,0,259,267,3,4,2,0,260,261,5,3,0,0,261,263,5,80,0,0,262,264,5,90,
  	0,0,263,262,1,0,0,0,263,264,1,0,0,0,264,265,1,0,0,0,265,266,5,81,0,0,
  	266,268,3,4,2,0,267,260,1,0,0,0,267,268,1,0,0,0,268,271,1,0,0,0,269,270,
  	5,13,0,0,270,272,3,4,2,0,271,269,1,0,0,0,271,272,1,0,0,0,272,35,1,0,0,
  	0,273,274,5,24,0,0,274,275,5,80,0,0,275,276,3,48,24,0,276,277,5,81,0,
  	0,277,281,5,82,0,0,278,280,3,38,19,0,279,278,1,0,0,0,280,283,1,0,0,0,
  	281,279,1,0,0,0,281,282,1,0,0,0,282,285,1,0,0,0,283,281,1,0,0,0,284,286,
  	3,40,20,0,285,284,1,0,0,0,285,286,1,0,0,0,286,290,1,0,0,0,287,289,3,38,
  	19,0,288,287,1,0,0,0,289,292,1,0,0,0,290,288,1,0,0,0,290,291,1,0,0,0,
  	291,293,1,0,0,0,292,290,1,0,0,0,293,294,5,83,0,0,294,37,1,0,0,0,295,296,
  	5,2,0,0,296,297,3,48,24,0,297,301,5,75,0,0,298,300,3,2,1,0,299,298,1,
  	0,0,0,300,303,1,0,0,0,301,299,1,0,0,0,301,302,1,0,0,0,302,39,1,0,0,0,
  	303,301,1,0,0,0,304,305,5,7,0,0,305,309,5,75,0,0,306,308,3,2,1,0,307,
  	306,1,0,0,0,308,311,1,0,0,0,309,307,1,0,0,0,309,310,1,0,0,0,310,41,1,
  	0,0,0,311,309,1,0,0,0,312,314,5,1,0,0,313,315,5,77,0,0,314,313,1,0,0,
  	0,314,315,1,0,0,0,315,43,1,0,0,0,316,318,5,6,0,0,317,319,5,77,0,0,318,
  	317,1,0,0,0,318,319,1,0,0,0,319,45,1,0,0,0,320,321,5,77,0,0,321,47,1,
  	0,0,0,322,323,3,50,25,0,323,49,1,0,0,0,324,330,3,52,26,0,325,326,3,70,
  	35,0,326,327,7,2,0,0,327,328,3,50,25,0,328,330,1,0,0,0,329,324,1,0,0,
  	0,329,325,1,0,0,0,330,51,1,0,0,0,331,337,3,54,27,0,332,333,5,74,0,0,333,
  	334,3,48,24,0,334,335,5,75,0,0,335,336,3,48,24,0,336,338,1,0,0,0,337,
  	332,1,0,0,0,337,338,1,0,0,0,338,53,1,0,0,0,339,344,3,56,28,0,340,341,
  	5,65,0,0,341,343,3,56,28,0,342,340,1,0,0,0,343,346,1,0,0,0,344,342,1,
  	0,0,0,344,345,1,0,0,0,345,55,1,0,0,0,346,344,1,0,0,0,347,352,3,58,29,
  	0,348,349,5,64,0,0,349,351,3,58,29,0,350,348,1,0,0,0,351,354,1,0,0,0,
  	352,350,1,0,0,0,352,353,1,0,0,0,353,57,1,0,0,0,354,352,1,0,0,0,355,360,
  	3,60,30,0,356,357,7,3,0,0,357,359,3,60,30,0,358,356,1,0,0,0,359,362,1,
  	0,0,0,360,358,1,0,0,0,360,361,1,0,0,0,361,59,1,0,0,0,362,360,1,0,0,0,
  	363,368,3,62,31,0,364,365,7,4,0,0,365,367,3,62,31,0,366,364,1,0,0,0,367,
  	370,1,0,0,0,368,366,1,0,0,0,368,369,1,0,0,0,369,61,1,0,0,0,370,368,1,
  	0,0,0,371,376,3,64,32,0,372,373,7,5,0,0,373,375,3,64,32,0,374,372,1,0,
  	0,0,375,378,1,0,0,0,376,374,1,0,0,0,376,377,1,0,0,0,377,63,1,0,0,0,378,
  	376,1,0,0,0,379,384,3,66,33,0,380,381,7,6,0,0,381,383,3,66,33,0,382,380,
  	1,0,0,0,383,386,1,0,0,0,384,382,1,0,0,0,384,385,1,0,0,0,385,65,1,0,0,
  	0,386,384,1,0,0,0,387,395,3,68,34,0,388,389,7,7,0,0,389,395,3,66,33,0,
  	390,391,7,8,0,0,391,395,3,66,33,0,392,393,5,34,0,0,393,395,3,66,33,0,
  	394,387,1,0,0,0,394,388,1,0,0,0,394,390,1,0,0,0,394,392,1,0,0,0,395,67,
  	1,0,0,0,396,398,3,70,35,0,397,399,7,7,0,0,398,397,1,0,0,0,398,399,1,0,
  	0,0,399,69,1,0,0,0,400,403,3,72,36,0,401,403,3,74,37,0,402,400,1,0,0,
  	0,402,401,1,0,0,0,403,71,1,0,0,0,404,405,6,36,-1,0,405,406,3,74,37,0,
  	406,407,3,82,41,0,407,420,1,0,0,0,408,409,10,3,0,0,409,419,3,82,41,0,
  	410,411,10,2,0,0,411,412,5,84,0,0,412,413,3,48,24,0,413,414,5,85,0,0,
  	414,419,1,0,0,0,415,416,10,1,0,0,416,417,5,79,0,0,417,419,5,90,0,0,418,
  	408,1,0,0,0,418,410,1,0,0,0,418,415,1,0,0,0,419,422,1,0,0,0,420,418,1,
  	0,0,0,420,421,1,0,0,0,421,73,1,0,0,0,422,420,1,0,0,0,423,424,6,37,-1,
  	0,424,431,3,76,38,0,425,426,5,21,0,0,426,428,3,74,37,0,427,429,3,82,41,
  	0,428,427,1,0,0,0,428,429,1,0,0,0,429,431,1,0,0,0,430,423,1,0,0,0,430,
  	425,1,0,0,0,431,442,1,0,0,0,432,433,10,3,0,0,433,434,5,84,0,0,434,435,
  	3,48,24,0,435,436,5,85,0,0,436,441,1,0,0,0,437,438,10,2,0,0,438,439,5,
  	79,0,0,439,441,5,90,0,0,440,432,1,0,0,0,440,437,1,0,0,0,441,444,1,0,0,
  	0,442,440,1,0,0,0,442,443,1,0,0,0,443,75,1,0,0,0,444,442,1,0,0,0,445,
  	458,5,25,0,0,446,458,5,23,0,0,447,458,5,90,0,0,448,458,3,90,45,0,449,
  	458,3,84,42,0,450,458,3,86,43,0,451,452,5,80,0,0,452,453,3,48,24,0,453,
  	454,5,81,0,0,454,458,1,0,0,0,455,458,3,78,39,0,456,458,3,80,40,0,457,
  	445,1,0,0,0,457,446,1,0,0,0,457,447,1,0,0,0,457,448,1,0,0,0,457,449,1,
  	0,0,0,457,450,1,0,0,0,457,451,1,0,0,0,457,455,1,0,0,0,457,456,1,0,0,0,
  	458,77,1,0,0,0,459,461,5,33,0,0,460,459,1,0,0,0,460,461,1,0,0,0,461,462,
  	1,0,0,0,462,464,5,15,0,0,463,465,5,90,0,0,464,463,1,0,0,0,464,465,1,0,
  	0,0,465,466,1,0,0,0,466,468,5,80,0,0,467,469,3,20,10,0,468,467,1,0,0,
  	0,468,469,1,0,0,0,469,470,1,0,0,0,470,471,5,81,0,0,471,472,3,4,2,0,472,
  	79,1,0,0,0,473,480,5,90,0,0,474,476,5,80,0,0,475,477,3,20,10,0,476,475,
  	1,0,0,0,476,477,1,0,0,0,477,478,1,0,0,0,478,480,5,81,0,0,479,473,1,0,
  	0,0,479,474,1,0,0,0,480,481,1,0,0,0,481,484,5,76,0,0,482,485,3,48,24,
  	0,483,485,3,4,2,0,484,482,1,0,0,0,484,483,1,0,0,0,485,81,1,0,0,0,486,
  	495,5,80,0,0,487,492,3,48,24,0,488,489,5,78,0,0,489,491,3,48,24,0,490,
  	488,1,0,0,0,491,494,1,0,0,0,492,490,1,0,0,0,492,493,1,0,0,0,493,496,1,
  	0,0,0,494,492,1,0,0,0,495,487,1,0,0,0,495,496,1,0,0,0,496,497,1,0,0,0,
  	497,498,5,81,0,0,498,83,1,0,0,0,499,508,5,84,0,0,500,505,3,48,24,0,501,
  	502,5,78,0,0,502,504,3,48,24,0,503,501,1,0,0,0,504,507,1,0,0,0,505,503,
  	1,0,0,0,505,506,1,0,0,0,506,509,1,0,0,0,507,505,1,0,0,0,508,500,1,0,0,
  	0,508,509,1,0,0,0,509,510,1,0,0,0,510,511,5,85,0,0,511,85,1,0,0,0,512,
  	521,5,82,0,0,513,518,3,88,44,0,514,515,5,78,0,0,515,517,3,88,44,0,516,
  	514,1,0,0,0,517,520,1,0,0,0,518,516,1,0,0,0,518,519,1,0,0,0,519,522,1,
  	0,0,0,520,518,1,0,0,0,521,513,1,0,0,0,521,522,1,0,0,0,522,523,1,0,0,0,
  	523,524,5,83,0,0,524,87,1,0,0,0,525,526,5,90,0,0,526,527,5,75,0,0,527,
  	542,3,48,24,0,528,529,5,87,0,0,529,530,5,75,0,0,530,542,3,48,24,0,531,
  	532,5,86,0,0,532,533,5,75,0,0,533,542,3,48,24,0,534,535,5,90,0,0,535,
  	537,5,80,0,0,536,538,3,20,10,0,537,536,1,0,0,0,537,538,1,0,0,0,538,539,
  	1,0,0,0,539,540,5,81,0,0,540,542,3,4,2,0,541,525,1,0,0,0,541,528,1,0,
  	0,0,541,531,1,0,0,0,541,534,1,0,0,0,542,89,1,0,0,0,543,544,7,9,0,0,544,
  	91,1,0,0,0,67,95,115,121,132,136,141,144,150,159,165,172,177,185,188,
  	195,200,209,221,225,229,237,244,248,251,256,263,267,271,281,285,290,301,
  	309,314,318,329,337,344,352,360,368,376,384,394,398,402,418,420,428,430,
  	440,442,457,460,464,468,476,479,484,492,495,505,508,518,521,537,541
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  simplejsparserParserStaticData = std::move(staticData);
}

}

SimpleJSParser::SimpleJSParser(TokenStream *input) : SimpleJSParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

SimpleJSParser::SimpleJSParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  SimpleJSParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *simplejsparserParserStaticData->atn, simplejsparserParserStaticData->decisionToDFA, simplejsparserParserStaticData->sharedContextCache, options);
}

SimpleJSParser::~SimpleJSParser() {
  delete _interpreter;
}

const atn::ATN& SimpleJSParser::getATN() const {
  return *simplejsparserParserStaticData->atn;
}

std::string SimpleJSParser::getGrammarFileName() const {
  return "SimpleJSParser.g4";
}

const std::vector<std::string>& SimpleJSParser::getRuleNames() const {
  return simplejsparserParserStaticData->ruleNames;
}

const dfa::Vocabulary& SimpleJSParser::getVocabulary() const {
  return simplejsparserParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView SimpleJSParser::getSerializedATN() const {
  return simplejsparserParserStaticData->serializedATN;
}


//----------------- ProgramContext ------------------------------------------------------------------

SimpleJSParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ProgramContext::EOF() {
  return getToken(SimpleJSParser::EOF, 0);
}

std::vector<SimpleJSParser::StatementContext *> SimpleJSParser::ProgramContext::statement() {
  return getRuleContexts<SimpleJSParser::StatementContext>();
}

SimpleJSParser::StatementContext* SimpleJSParser::ProgramContext::statement(size_t i) {
  return getRuleContext<SimpleJSParser::StatementContext>(i);
}


size_t SimpleJSParser::ProgramContext::getRuleIndex() const {
  return SimpleJSParser::RuleProgram;
}

void SimpleJSParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void SimpleJSParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

SimpleJSParser::ProgramContext* SimpleJSParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, SimpleJSParser::RuleProgram);
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
    setState(95);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8452525702365554) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 32851985) != 0)) {
      setState(92);
      statement();
      setState(97);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(98);
    match(SimpleJSParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

SimpleJSParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::BlockContext* SimpleJSParser::StatementContext::block() {
  return getRuleContext<SimpleJSParser::BlockContext>(0);
}

SimpleJSParser::VarDeclarationContext* SimpleJSParser::StatementContext::varDeclaration() {
  return getRuleContext<SimpleJSParser::VarDeclarationContext>(0);
}

SimpleJSParser::FunctionDeclarationContext* SimpleJSParser::StatementContext::functionDeclaration() {
  return getRuleContext<SimpleJSParser::FunctionDeclarationContext>(0);
}

SimpleJSParser::ClassDeclarationContext* SimpleJSParser::StatementContext::classDeclaration() {
  return getRuleContext<SimpleJSParser::ClassDeclarationContext>(0);
}

SimpleJSParser::ExpressionStatementContext* SimpleJSParser::StatementContext::expressionStatement() {
  return getRuleContext<SimpleJSParser::ExpressionStatementContext>(0);
}

SimpleJSParser::IfStatementContext* SimpleJSParser::StatementContext::ifStatement() {
  return getRuleContext<SimpleJSParser::IfStatementContext>(0);
}

SimpleJSParser::WhileStatementContext* SimpleJSParser::StatementContext::whileStatement() {
  return getRuleContext<SimpleJSParser::WhileStatementContext>(0);
}

SimpleJSParser::ForStatementContext* SimpleJSParser::StatementContext::forStatement() {
  return getRuleContext<SimpleJSParser::ForStatementContext>(0);
}

SimpleJSParser::ReturnStatementContext* SimpleJSParser::StatementContext::returnStatement() {
  return getRuleContext<SimpleJSParser::ReturnStatementContext>(0);
}

SimpleJSParser::ThrowStatementContext* SimpleJSParser::StatementContext::throwStatement() {
  return getRuleContext<SimpleJSParser::ThrowStatementContext>(0);
}

SimpleJSParser::TryStatementContext* SimpleJSParser::StatementContext::tryStatement() {
  return getRuleContext<SimpleJSParser::TryStatementContext>(0);
}

SimpleJSParser::SwitchStatementContext* SimpleJSParser::StatementContext::switchStatement() {
  return getRuleContext<SimpleJSParser::SwitchStatementContext>(0);
}

SimpleJSParser::BreakStatementContext* SimpleJSParser::StatementContext::breakStatement() {
  return getRuleContext<SimpleJSParser::BreakStatementContext>(0);
}

SimpleJSParser::ContinueStatementContext* SimpleJSParser::StatementContext::continueStatement() {
  return getRuleContext<SimpleJSParser::ContinueStatementContext>(0);
}

SimpleJSParser::EmptyStatementContext* SimpleJSParser::StatementContext::emptyStatement() {
  return getRuleContext<SimpleJSParser::EmptyStatementContext>(0);
}


size_t SimpleJSParser::StatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleStatement;
}

void SimpleJSParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void SimpleJSParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

SimpleJSParser::StatementContext* SimpleJSParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 2, SimpleJSParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(115);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(100);
      block();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(101);
      varDeclaration();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(102);
      functionDeclaration();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(103);
      classDeclaration();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(104);
      expressionStatement();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(105);
      ifStatement();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(106);
      whileStatement();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(107);
      forStatement();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(108);
      returnStatement();
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(109);
      throwStatement();
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(110);
      tryStatement();
      break;
    }

    case 12: {
      enterOuterAlt(_localctx, 12);
      setState(111);
      switchStatement();
      break;
    }

    case 13: {
      enterOuterAlt(_localctx, 13);
      setState(112);
      breakStatement();
      break;
    }

    case 14: {
      enterOuterAlt(_localctx, 14);
      setState(113);
      continueStatement();
      break;
    }

    case 15: {
      enterOuterAlt(_localctx, 15);
      setState(114);
      emptyStatement();
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

//----------------- BlockContext ------------------------------------------------------------------

SimpleJSParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::BlockContext::LBRACE() {
  return getToken(SimpleJSParser::LBRACE, 0);
}

tree::TerminalNode* SimpleJSParser::BlockContext::RBRACE() {
  return getToken(SimpleJSParser::RBRACE, 0);
}

std::vector<SimpleJSParser::StatementContext *> SimpleJSParser::BlockContext::statement() {
  return getRuleContexts<SimpleJSParser::StatementContext>();
}

SimpleJSParser::StatementContext* SimpleJSParser::BlockContext::statement(size_t i) {
  return getRuleContext<SimpleJSParser::StatementContext>(i);
}


size_t SimpleJSParser::BlockContext::getRuleIndex() const {
  return SimpleJSParser::RuleBlock;
}

void SimpleJSParser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void SimpleJSParser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}

SimpleJSParser::BlockContext* SimpleJSParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 4, SimpleJSParser::RuleBlock);
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
    match(SimpleJSParser::LBRACE);
    setState(121);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8452525702365554) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 32851985) != 0)) {
      setState(118);
      statement();
      setState(123);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(124);
    match(SimpleJSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDeclarationContext ------------------------------------------------------------------

SimpleJSParser::VarDeclarationContext::VarDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SimpleJSParser::VarDeclaratorContext *> SimpleJSParser::VarDeclarationContext::varDeclarator() {
  return getRuleContexts<SimpleJSParser::VarDeclaratorContext>();
}

SimpleJSParser::VarDeclaratorContext* SimpleJSParser::VarDeclarationContext::varDeclarator(size_t i) {
  return getRuleContext<SimpleJSParser::VarDeclaratorContext>(i);
}

tree::TerminalNode* SimpleJSParser::VarDeclarationContext::VAR() {
  return getToken(SimpleJSParser::VAR, 0);
}

tree::TerminalNode* SimpleJSParser::VarDeclarationContext::LET() {
  return getToken(SimpleJSParser::LET, 0);
}

tree::TerminalNode* SimpleJSParser::VarDeclarationContext::CONST() {
  return getToken(SimpleJSParser::CONST, 0);
}

std::vector<tree::TerminalNode *> SimpleJSParser::VarDeclarationContext::COMMA() {
  return getTokens(SimpleJSParser::COMMA);
}

tree::TerminalNode* SimpleJSParser::VarDeclarationContext::COMMA(size_t i) {
  return getToken(SimpleJSParser::COMMA, i);
}

tree::TerminalNode* SimpleJSParser::VarDeclarationContext::SEMICOLON() {
  return getToken(SimpleJSParser::SEMICOLON, 0);
}


size_t SimpleJSParser::VarDeclarationContext::getRuleIndex() const {
  return SimpleJSParser::RuleVarDeclaration;
}

void SimpleJSParser::VarDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVarDeclaration(this);
}

void SimpleJSParser::VarDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVarDeclaration(this);
}

SimpleJSParser::VarDeclarationContext* SimpleJSParser::varDeclaration() {
  VarDeclarationContext *_localctx = _tracker.createInstance<VarDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 6, SimpleJSParser::RuleVarDeclaration);
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
    setState(126);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 537919520) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(127);
    varDeclarator();
    setState(132);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SimpleJSParser::COMMA) {
      setState(128);
      match(SimpleJSParser::COMMA);
      setState(129);
      varDeclarator();
      setState(134);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(136);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
    case 1: {
      setState(135);
      match(SimpleJSParser::SEMICOLON);
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

//----------------- VarDeclaratorContext ------------------------------------------------------------------

SimpleJSParser::VarDeclaratorContext::VarDeclaratorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::VarDeclaratorContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* SimpleJSParser::VarDeclaratorContext::ASSIGN() {
  return getToken(SimpleJSParser::ASSIGN, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::VarDeclaratorContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}


size_t SimpleJSParser::VarDeclaratorContext::getRuleIndex() const {
  return SimpleJSParser::RuleVarDeclarator;
}

void SimpleJSParser::VarDeclaratorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVarDeclarator(this);
}

void SimpleJSParser::VarDeclaratorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVarDeclarator(this);
}

SimpleJSParser::VarDeclaratorContext* SimpleJSParser::varDeclarator() {
  VarDeclaratorContext *_localctx = _tracker.createInstance<VarDeclaratorContext>(_ctx, getState());
  enterRule(_localctx, 8, SimpleJSParser::RuleVarDeclarator);
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
    setState(138);
    match(SimpleJSParser::IDENTIFIER);
    setState(141);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::ASSIGN) {
      setState(139);
      match(SimpleJSParser::ASSIGN);
      setState(140);
      expression();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionDeclarationContext ------------------------------------------------------------------

SimpleJSParser::FunctionDeclarationContext::FunctionDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::FunctionDeclarationContext::FUNCTION() {
  return getToken(SimpleJSParser::FUNCTION, 0);
}

tree::TerminalNode* SimpleJSParser::FunctionDeclarationContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* SimpleJSParser::FunctionDeclarationContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

tree::TerminalNode* SimpleJSParser::FunctionDeclarationContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

SimpleJSParser::BlockContext* SimpleJSParser::FunctionDeclarationContext::block() {
  return getRuleContext<SimpleJSParser::BlockContext>(0);
}

tree::TerminalNode* SimpleJSParser::FunctionDeclarationContext::ASYNC() {
  return getToken(SimpleJSParser::ASYNC, 0);
}

SimpleJSParser::ParameterListContext* SimpleJSParser::FunctionDeclarationContext::parameterList() {
  return getRuleContext<SimpleJSParser::ParameterListContext>(0);
}


size_t SimpleJSParser::FunctionDeclarationContext::getRuleIndex() const {
  return SimpleJSParser::RuleFunctionDeclaration;
}

void SimpleJSParser::FunctionDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionDeclaration(this);
}

void SimpleJSParser::FunctionDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionDeclaration(this);
}

SimpleJSParser::FunctionDeclarationContext* SimpleJSParser::functionDeclaration() {
  FunctionDeclarationContext *_localctx = _tracker.createInstance<FunctionDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 10, SimpleJSParser::RuleFunctionDeclaration);
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
    setState(144);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::ASYNC) {
      setState(143);
      match(SimpleJSParser::ASYNC);
    }
    setState(146);
    match(SimpleJSParser::FUNCTION);
    setState(147);
    match(SimpleJSParser::IDENTIFIER);
    setState(148);
    match(SimpleJSParser::LPAREN);
    setState(150);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::IDENTIFIER) {
      setState(149);
      parameterList();
    }
    setState(152);
    match(SimpleJSParser::RPAREN);
    setState(153);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassDeclarationContext ------------------------------------------------------------------

SimpleJSParser::ClassDeclarationContext::ClassDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ClassDeclarationContext::CLASS() {
  return getToken(SimpleJSParser::CLASS, 0);
}

std::vector<tree::TerminalNode *> SimpleJSParser::ClassDeclarationContext::IDENTIFIER() {
  return getTokens(SimpleJSParser::IDENTIFIER);
}

tree::TerminalNode* SimpleJSParser::ClassDeclarationContext::IDENTIFIER(size_t i) {
  return getToken(SimpleJSParser::IDENTIFIER, i);
}

tree::TerminalNode* SimpleJSParser::ClassDeclarationContext::LBRACE() {
  return getToken(SimpleJSParser::LBRACE, 0);
}

tree::TerminalNode* SimpleJSParser::ClassDeclarationContext::RBRACE() {
  return getToken(SimpleJSParser::RBRACE, 0);
}

tree::TerminalNode* SimpleJSParser::ClassDeclarationContext::EXTENDS() {
  return getToken(SimpleJSParser::EXTENDS, 0);
}

std::vector<SimpleJSParser::ClassElementContext *> SimpleJSParser::ClassDeclarationContext::classElement() {
  return getRuleContexts<SimpleJSParser::ClassElementContext>();
}

SimpleJSParser::ClassElementContext* SimpleJSParser::ClassDeclarationContext::classElement(size_t i) {
  return getRuleContext<SimpleJSParser::ClassElementContext>(i);
}


size_t SimpleJSParser::ClassDeclarationContext::getRuleIndex() const {
  return SimpleJSParser::RuleClassDeclaration;
}

void SimpleJSParser::ClassDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassDeclaration(this);
}

void SimpleJSParser::ClassDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassDeclaration(this);
}

SimpleJSParser::ClassDeclarationContext* SimpleJSParser::classDeclaration() {
  ClassDeclarationContext *_localctx = _tracker.createInstance<ClassDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 12, SimpleJSParser::RuleClassDeclaration);
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
    setState(155);
    match(SimpleJSParser::CLASS);
    setState(156);
    match(SimpleJSParser::IDENTIFIER);
    setState(159);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::EXTENDS) {
      setState(157);
      match(SimpleJSParser::EXTENDS);
      setState(158);
      match(SimpleJSParser::IDENTIFIER);
    }
    setState(161);
    match(SimpleJSParser::LBRACE);
    setState(165);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SimpleJSParser::IDENTIFIER) {
      setState(162);
      classElement();
      setState(167);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(168);
    match(SimpleJSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassElementContext ------------------------------------------------------------------

SimpleJSParser::ClassElementContext::ClassElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::MethodDefinitionContext* SimpleJSParser::ClassElementContext::methodDefinition() {
  return getRuleContext<SimpleJSParser::MethodDefinitionContext>(0);
}

SimpleJSParser::PropertyDefinitionContext* SimpleJSParser::ClassElementContext::propertyDefinition() {
  return getRuleContext<SimpleJSParser::PropertyDefinitionContext>(0);
}


size_t SimpleJSParser::ClassElementContext::getRuleIndex() const {
  return SimpleJSParser::RuleClassElement;
}

void SimpleJSParser::ClassElementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassElement(this);
}

void SimpleJSParser::ClassElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassElement(this);
}

SimpleJSParser::ClassElementContext* SimpleJSParser::classElement() {
  ClassElementContext *_localctx = _tracker.createInstance<ClassElementContext>(_ctx, getState());
  enterRule(_localctx, 14, SimpleJSParser::RuleClassElement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(172);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(170);
      methodDefinition();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(171);
      propertyDefinition();
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

//----------------- MethodDefinitionContext ------------------------------------------------------------------

SimpleJSParser::MethodDefinitionContext::MethodDefinitionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::MethodDefinitionContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* SimpleJSParser::MethodDefinitionContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

tree::TerminalNode* SimpleJSParser::MethodDefinitionContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

SimpleJSParser::BlockContext* SimpleJSParser::MethodDefinitionContext::block() {
  return getRuleContext<SimpleJSParser::BlockContext>(0);
}

SimpleJSParser::ParameterListContext* SimpleJSParser::MethodDefinitionContext::parameterList() {
  return getRuleContext<SimpleJSParser::ParameterListContext>(0);
}


size_t SimpleJSParser::MethodDefinitionContext::getRuleIndex() const {
  return SimpleJSParser::RuleMethodDefinition;
}

void SimpleJSParser::MethodDefinitionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMethodDefinition(this);
}

void SimpleJSParser::MethodDefinitionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMethodDefinition(this);
}

SimpleJSParser::MethodDefinitionContext* SimpleJSParser::methodDefinition() {
  MethodDefinitionContext *_localctx = _tracker.createInstance<MethodDefinitionContext>(_ctx, getState());
  enterRule(_localctx, 16, SimpleJSParser::RuleMethodDefinition);
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
    setState(174);
    match(SimpleJSParser::IDENTIFIER);
    setState(175);
    match(SimpleJSParser::LPAREN);
    setState(177);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::IDENTIFIER) {
      setState(176);
      parameterList();
    }
    setState(179);
    match(SimpleJSParser::RPAREN);
    setState(180);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyDefinitionContext ------------------------------------------------------------------

SimpleJSParser::PropertyDefinitionContext::PropertyDefinitionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::PropertyDefinitionContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* SimpleJSParser::PropertyDefinitionContext::ASSIGN() {
  return getToken(SimpleJSParser::ASSIGN, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::PropertyDefinitionContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::PropertyDefinitionContext::SEMICOLON() {
  return getToken(SimpleJSParser::SEMICOLON, 0);
}


size_t SimpleJSParser::PropertyDefinitionContext::getRuleIndex() const {
  return SimpleJSParser::RulePropertyDefinition;
}

void SimpleJSParser::PropertyDefinitionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyDefinition(this);
}

void SimpleJSParser::PropertyDefinitionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyDefinition(this);
}

SimpleJSParser::PropertyDefinitionContext* SimpleJSParser::propertyDefinition() {
  PropertyDefinitionContext *_localctx = _tracker.createInstance<PropertyDefinitionContext>(_ctx, getState());
  enterRule(_localctx, 18, SimpleJSParser::RulePropertyDefinition);
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
    setState(182);
    match(SimpleJSParser::IDENTIFIER);
    setState(185);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::ASSIGN) {
      setState(183);
      match(SimpleJSParser::ASSIGN);
      setState(184);
      expression();
    }
    setState(188);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::SEMICOLON) {
      setState(187);
      match(SimpleJSParser::SEMICOLON);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParameterListContext ------------------------------------------------------------------

SimpleJSParser::ParameterListContext::ParameterListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> SimpleJSParser::ParameterListContext::IDENTIFIER() {
  return getTokens(SimpleJSParser::IDENTIFIER);
}

tree::TerminalNode* SimpleJSParser::ParameterListContext::IDENTIFIER(size_t i) {
  return getToken(SimpleJSParser::IDENTIFIER, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::ParameterListContext::COMMA() {
  return getTokens(SimpleJSParser::COMMA);
}

tree::TerminalNode* SimpleJSParser::ParameterListContext::COMMA(size_t i) {
  return getToken(SimpleJSParser::COMMA, i);
}


size_t SimpleJSParser::ParameterListContext::getRuleIndex() const {
  return SimpleJSParser::RuleParameterList;
}

void SimpleJSParser::ParameterListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParameterList(this);
}

void SimpleJSParser::ParameterListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParameterList(this);
}

SimpleJSParser::ParameterListContext* SimpleJSParser::parameterList() {
  ParameterListContext *_localctx = _tracker.createInstance<ParameterListContext>(_ctx, getState());
  enterRule(_localctx, 20, SimpleJSParser::RuleParameterList);
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
    setState(190);
    match(SimpleJSParser::IDENTIFIER);
    setState(195);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SimpleJSParser::COMMA) {
      setState(191);
      match(SimpleJSParser::COMMA);
      setState(192);
      match(SimpleJSParser::IDENTIFIER);
      setState(197);
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

//----------------- ExpressionStatementContext ------------------------------------------------------------------

SimpleJSParser::ExpressionStatementContext::ExpressionStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::ExpressionContext* SimpleJSParser::ExpressionStatementContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::ExpressionStatementContext::SEMICOLON() {
  return getToken(SimpleJSParser::SEMICOLON, 0);
}


size_t SimpleJSParser::ExpressionStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleExpressionStatement;
}

void SimpleJSParser::ExpressionStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpressionStatement(this);
}

void SimpleJSParser::ExpressionStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpressionStatement(this);
}

SimpleJSParser::ExpressionStatementContext* SimpleJSParser::expressionStatement() {
  ExpressionStatementContext *_localctx = _tracker.createInstance<ExpressionStatementContext>(_ctx, getState());
  enterRule(_localctx, 22, SimpleJSParser::RuleExpressionStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(198);
    expression();
    setState(200);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      setState(199);
      match(SimpleJSParser::SEMICOLON);
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

//----------------- IfStatementContext ------------------------------------------------------------------

SimpleJSParser::IfStatementContext::IfStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::IfStatementContext::IF() {
  return getToken(SimpleJSParser::IF, 0);
}

tree::TerminalNode* SimpleJSParser::IfStatementContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::IfStatementContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::IfStatementContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

std::vector<SimpleJSParser::StatementContext *> SimpleJSParser::IfStatementContext::statement() {
  return getRuleContexts<SimpleJSParser::StatementContext>();
}

SimpleJSParser::StatementContext* SimpleJSParser::IfStatementContext::statement(size_t i) {
  return getRuleContext<SimpleJSParser::StatementContext>(i);
}

tree::TerminalNode* SimpleJSParser::IfStatementContext::ELSE() {
  return getToken(SimpleJSParser::ELSE, 0);
}


size_t SimpleJSParser::IfStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleIfStatement;
}

void SimpleJSParser::IfStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIfStatement(this);
}

void SimpleJSParser::IfStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIfStatement(this);
}

SimpleJSParser::IfStatementContext* SimpleJSParser::ifStatement() {
  IfStatementContext *_localctx = _tracker.createInstance<IfStatementContext>(_ctx, getState());
  enterRule(_localctx, 24, SimpleJSParser::RuleIfStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(202);
    match(SimpleJSParser::IF);
    setState(203);
    match(SimpleJSParser::LPAREN);
    setState(204);
    expression();
    setState(205);
    match(SimpleJSParser::RPAREN);
    setState(206);
    statement();
    setState(209);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx)) {
    case 1: {
      setState(207);
      match(SimpleJSParser::ELSE);
      setState(208);
      statement();
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

//----------------- WhileStatementContext ------------------------------------------------------------------

SimpleJSParser::WhileStatementContext::WhileStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::WhileStatementContext::WHILE() {
  return getToken(SimpleJSParser::WHILE, 0);
}

tree::TerminalNode* SimpleJSParser::WhileStatementContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::WhileStatementContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::WhileStatementContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

SimpleJSParser::StatementContext* SimpleJSParser::WhileStatementContext::statement() {
  return getRuleContext<SimpleJSParser::StatementContext>(0);
}


size_t SimpleJSParser::WhileStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleWhileStatement;
}

void SimpleJSParser::WhileStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWhileStatement(this);
}

void SimpleJSParser::WhileStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWhileStatement(this);
}

SimpleJSParser::WhileStatementContext* SimpleJSParser::whileStatement() {
  WhileStatementContext *_localctx = _tracker.createInstance<WhileStatementContext>(_ctx, getState());
  enterRule(_localctx, 26, SimpleJSParser::RuleWhileStatement);

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
    match(SimpleJSParser::WHILE);
    setState(212);
    match(SimpleJSParser::LPAREN);
    setState(213);
    expression();
    setState(214);
    match(SimpleJSParser::RPAREN);
    setState(215);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ForStatementContext ------------------------------------------------------------------

SimpleJSParser::ForStatementContext::ForStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ForStatementContext::FOR() {
  return getToken(SimpleJSParser::FOR, 0);
}

tree::TerminalNode* SimpleJSParser::ForStatementContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

std::vector<tree::TerminalNode *> SimpleJSParser::ForStatementContext::SEMICOLON() {
  return getTokens(SimpleJSParser::SEMICOLON);
}

tree::TerminalNode* SimpleJSParser::ForStatementContext::SEMICOLON(size_t i) {
  return getToken(SimpleJSParser::SEMICOLON, i);
}

tree::TerminalNode* SimpleJSParser::ForStatementContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

SimpleJSParser::StatementContext* SimpleJSParser::ForStatementContext::statement() {
  return getRuleContext<SimpleJSParser::StatementContext>(0);
}

SimpleJSParser::VarDeclarationContext* SimpleJSParser::ForStatementContext::varDeclaration() {
  return getRuleContext<SimpleJSParser::VarDeclarationContext>(0);
}

std::vector<SimpleJSParser::ExpressionContext *> SimpleJSParser::ForStatementContext::expression() {
  return getRuleContexts<SimpleJSParser::ExpressionContext>();
}

SimpleJSParser::ExpressionContext* SimpleJSParser::ForStatementContext::expression(size_t i) {
  return getRuleContext<SimpleJSParser::ExpressionContext>(i);
}

tree::TerminalNode* SimpleJSParser::ForStatementContext::IN() {
  return getToken(SimpleJSParser::IN, 0);
}

tree::TerminalNode* SimpleJSParser::ForStatementContext::OF() {
  return getToken(SimpleJSParser::OF, 0);
}


size_t SimpleJSParser::ForStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleForStatement;
}

void SimpleJSParser::ForStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForStatement(this);
}

void SimpleJSParser::ForStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForStatement(this);
}

SimpleJSParser::ForStatementContext* SimpleJSParser::forStatement() {
  ForStatementContext *_localctx = _tracker.createInstance<ForStatementContext>(_ctx, getState());
  enterRule(_localctx, 28, SimpleJSParser::RuleForStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(244);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(217);
      match(SimpleJSParser::FOR);
      setState(218);
      match(SimpleJSParser::LPAREN);
      setState(221);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case SimpleJSParser::CONST:
        case SimpleJSParser::LET:
        case SimpleJSParser::VAR: {
          setState(219);
          varDeclaration();
          break;
        }

        case SimpleJSParser::DELETE:
        case SimpleJSParser::FUNCTION:
        case SimpleJSParser::NEW:
        case SimpleJSParser::SUPER:
        case SimpleJSParser::THIS:
        case SimpleJSParser::TYPEOF:
        case SimpleJSParser::VOID:
        case SimpleJSParser::ASYNC:
        case SimpleJSParser::AWAIT:
        case SimpleJSParser::NULL_:
        case SimpleJSParser::TRUE:
        case SimpleJSParser::FALSE:
        case SimpleJSParser::UNDEFINED:
        case SimpleJSParser::PLUS_PLUS:
        case SimpleJSParser::MINUS_MINUS:
        case SimpleJSParser::PLUS:
        case SimpleJSParser::MINUS:
        case SimpleJSParser::NOT:
        case SimpleJSParser::BIT_NOT:
        case SimpleJSParser::LPAREN:
        case SimpleJSParser::LBRACE:
        case SimpleJSParser::LBRACKET:
        case SimpleJSParser::NUMBER:
        case SimpleJSParser::STRING:
        case SimpleJSParser::TEMPLATE:
        case SimpleJSParser::REGEX:
        case SimpleJSParser::IDENTIFIER: {
          setState(220);
          expression();
          break;
        }

        case SimpleJSParser::SEMICOLON: {
          break;
        }

      default:
        break;
      }
      setState(223);
      match(SimpleJSParser::SEMICOLON);
      setState(225);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 8452522794582272) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 66)) & 32849937) != 0)) {
        setState(224);
        expression();
      }
      setState(227);
      match(SimpleJSParser::SEMICOLON);
      setState(229);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 8452522794582272) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 66)) & 32849937) != 0)) {
        setState(228);
        expression();
      }
      setState(231);
      match(SimpleJSParser::RPAREN);
      setState(232);
      statement();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(233);
      match(SimpleJSParser::FOR);
      setState(234);
      match(SimpleJSParser::LPAREN);
      setState(237);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case SimpleJSParser::CONST:
        case SimpleJSParser::LET:
        case SimpleJSParser::VAR: {
          setState(235);
          varDeclaration();
          break;
        }

        case SimpleJSParser::DELETE:
        case SimpleJSParser::FUNCTION:
        case SimpleJSParser::NEW:
        case SimpleJSParser::SUPER:
        case SimpleJSParser::THIS:
        case SimpleJSParser::TYPEOF:
        case SimpleJSParser::VOID:
        case SimpleJSParser::ASYNC:
        case SimpleJSParser::AWAIT:
        case SimpleJSParser::NULL_:
        case SimpleJSParser::TRUE:
        case SimpleJSParser::FALSE:
        case SimpleJSParser::UNDEFINED:
        case SimpleJSParser::PLUS_PLUS:
        case SimpleJSParser::MINUS_MINUS:
        case SimpleJSParser::PLUS:
        case SimpleJSParser::MINUS:
        case SimpleJSParser::NOT:
        case SimpleJSParser::BIT_NOT:
        case SimpleJSParser::LPAREN:
        case SimpleJSParser::LBRACE:
        case SimpleJSParser::LBRACKET:
        case SimpleJSParser::NUMBER:
        case SimpleJSParser::STRING:
        case SimpleJSParser::TEMPLATE:
        case SimpleJSParser::REGEX:
        case SimpleJSParser::IDENTIFIER: {
          setState(236);
          expression();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(239);
      _la = _input->LA(1);
      if (!(_la == SimpleJSParser::IN

      || _la == SimpleJSParser::OF)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(240);
      expression();
      setState(241);
      match(SimpleJSParser::RPAREN);
      setState(242);
      statement();
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

//----------------- ReturnStatementContext ------------------------------------------------------------------

SimpleJSParser::ReturnStatementContext::ReturnStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ReturnStatementContext::RETURN() {
  return getToken(SimpleJSParser::RETURN, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::ReturnStatementContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::ReturnStatementContext::SEMICOLON() {
  return getToken(SimpleJSParser::SEMICOLON, 0);
}


size_t SimpleJSParser::ReturnStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleReturnStatement;
}

void SimpleJSParser::ReturnStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReturnStatement(this);
}

void SimpleJSParser::ReturnStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReturnStatement(this);
}

SimpleJSParser::ReturnStatementContext* SimpleJSParser::returnStatement() {
  ReturnStatementContext *_localctx = _tracker.createInstance<ReturnStatementContext>(_ctx, getState());
  enterRule(_localctx, 30, SimpleJSParser::RuleReturnStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(246);
    match(SimpleJSParser::RETURN);
    setState(248);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 22, _ctx)) {
    case 1: {
      setState(247);
      expression();
      break;
    }

    default:
      break;
    }
    setState(251);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      setState(250);
      match(SimpleJSParser::SEMICOLON);
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

//----------------- ThrowStatementContext ------------------------------------------------------------------

SimpleJSParser::ThrowStatementContext::ThrowStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ThrowStatementContext::THROW() {
  return getToken(SimpleJSParser::THROW, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::ThrowStatementContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::ThrowStatementContext::SEMICOLON() {
  return getToken(SimpleJSParser::SEMICOLON, 0);
}


size_t SimpleJSParser::ThrowStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleThrowStatement;
}

void SimpleJSParser::ThrowStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterThrowStatement(this);
}

void SimpleJSParser::ThrowStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitThrowStatement(this);
}

SimpleJSParser::ThrowStatementContext* SimpleJSParser::throwStatement() {
  ThrowStatementContext *_localctx = _tracker.createInstance<ThrowStatementContext>(_ctx, getState());
  enterRule(_localctx, 32, SimpleJSParser::RuleThrowStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(253);
    match(SimpleJSParser::THROW);
    setState(254);
    expression();
    setState(256);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
    case 1: {
      setState(255);
      match(SimpleJSParser::SEMICOLON);
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

//----------------- TryStatementContext ------------------------------------------------------------------

SimpleJSParser::TryStatementContext::TryStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::TryStatementContext::TRY() {
  return getToken(SimpleJSParser::TRY, 0);
}

std::vector<SimpleJSParser::BlockContext *> SimpleJSParser::TryStatementContext::block() {
  return getRuleContexts<SimpleJSParser::BlockContext>();
}

SimpleJSParser::BlockContext* SimpleJSParser::TryStatementContext::block(size_t i) {
  return getRuleContext<SimpleJSParser::BlockContext>(i);
}

tree::TerminalNode* SimpleJSParser::TryStatementContext::CATCH() {
  return getToken(SimpleJSParser::CATCH, 0);
}

tree::TerminalNode* SimpleJSParser::TryStatementContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

tree::TerminalNode* SimpleJSParser::TryStatementContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

tree::TerminalNode* SimpleJSParser::TryStatementContext::FINALLY() {
  return getToken(SimpleJSParser::FINALLY, 0);
}

tree::TerminalNode* SimpleJSParser::TryStatementContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}


size_t SimpleJSParser::TryStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleTryStatement;
}

void SimpleJSParser::TryStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTryStatement(this);
}

void SimpleJSParser::TryStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTryStatement(this);
}

SimpleJSParser::TryStatementContext* SimpleJSParser::tryStatement() {
  TryStatementContext *_localctx = _tracker.createInstance<TryStatementContext>(_ctx, getState());
  enterRule(_localctx, 34, SimpleJSParser::RuleTryStatement);
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
    setState(258);
    match(SimpleJSParser::TRY);
    setState(259);
    block();
    setState(267);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::CATCH) {
      setState(260);
      match(SimpleJSParser::CATCH);
      setState(261);
      match(SimpleJSParser::LPAREN);
      setState(263);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == SimpleJSParser::IDENTIFIER) {
        setState(262);
        match(SimpleJSParser::IDENTIFIER);
      }
      setState(265);
      match(SimpleJSParser::RPAREN);
      setState(266);
      block();
    }
    setState(271);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::FINALLY) {
      setState(269);
      match(SimpleJSParser::FINALLY);
      setState(270);
      block();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SwitchStatementContext ------------------------------------------------------------------

SimpleJSParser::SwitchStatementContext::SwitchStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::SwitchStatementContext::SWITCH() {
  return getToken(SimpleJSParser::SWITCH, 0);
}

tree::TerminalNode* SimpleJSParser::SwitchStatementContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::SwitchStatementContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::SwitchStatementContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

tree::TerminalNode* SimpleJSParser::SwitchStatementContext::LBRACE() {
  return getToken(SimpleJSParser::LBRACE, 0);
}

tree::TerminalNode* SimpleJSParser::SwitchStatementContext::RBRACE() {
  return getToken(SimpleJSParser::RBRACE, 0);
}

std::vector<SimpleJSParser::CaseClauseContext *> SimpleJSParser::SwitchStatementContext::caseClause() {
  return getRuleContexts<SimpleJSParser::CaseClauseContext>();
}

SimpleJSParser::CaseClauseContext* SimpleJSParser::SwitchStatementContext::caseClause(size_t i) {
  return getRuleContext<SimpleJSParser::CaseClauseContext>(i);
}

SimpleJSParser::DefaultClauseContext* SimpleJSParser::SwitchStatementContext::defaultClause() {
  return getRuleContext<SimpleJSParser::DefaultClauseContext>(0);
}


size_t SimpleJSParser::SwitchStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleSwitchStatement;
}

void SimpleJSParser::SwitchStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSwitchStatement(this);
}

void SimpleJSParser::SwitchStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSwitchStatement(this);
}

SimpleJSParser::SwitchStatementContext* SimpleJSParser::switchStatement() {
  SwitchStatementContext *_localctx = _tracker.createInstance<SwitchStatementContext>(_ctx, getState());
  enterRule(_localctx, 36, SimpleJSParser::RuleSwitchStatement);
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
    setState(273);
    match(SimpleJSParser::SWITCH);
    setState(274);
    match(SimpleJSParser::LPAREN);
    setState(275);
    expression();
    setState(276);
    match(SimpleJSParser::RPAREN);
    setState(277);
    match(SimpleJSParser::LBRACE);
    setState(281);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(278);
        caseClause(); 
      }
      setState(283);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx);
    }
    setState(285);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::DEFAULT) {
      setState(284);
      defaultClause();
    }
    setState(290);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SimpleJSParser::CASE) {
      setState(287);
      caseClause();
      setState(292);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(293);
    match(SimpleJSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CaseClauseContext ------------------------------------------------------------------

SimpleJSParser::CaseClauseContext::CaseClauseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::CaseClauseContext::CASE() {
  return getToken(SimpleJSParser::CASE, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::CaseClauseContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::CaseClauseContext::COLON() {
  return getToken(SimpleJSParser::COLON, 0);
}

std::vector<SimpleJSParser::StatementContext *> SimpleJSParser::CaseClauseContext::statement() {
  return getRuleContexts<SimpleJSParser::StatementContext>();
}

SimpleJSParser::StatementContext* SimpleJSParser::CaseClauseContext::statement(size_t i) {
  return getRuleContext<SimpleJSParser::StatementContext>(i);
}


size_t SimpleJSParser::CaseClauseContext::getRuleIndex() const {
  return SimpleJSParser::RuleCaseClause;
}

void SimpleJSParser::CaseClauseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCaseClause(this);
}

void SimpleJSParser::CaseClauseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCaseClause(this);
}

SimpleJSParser::CaseClauseContext* SimpleJSParser::caseClause() {
  CaseClauseContext *_localctx = _tracker.createInstance<CaseClauseContext>(_ctx, getState());
  enterRule(_localctx, 38, SimpleJSParser::RuleCaseClause);
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
    setState(295);
    match(SimpleJSParser::CASE);
    setState(296);
    expression();
    setState(297);
    match(SimpleJSParser::COLON);
    setState(301);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8452525702365554) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 32851985) != 0)) {
      setState(298);
      statement();
      setState(303);
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

//----------------- DefaultClauseContext ------------------------------------------------------------------

SimpleJSParser::DefaultClauseContext::DefaultClauseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::DefaultClauseContext::DEFAULT() {
  return getToken(SimpleJSParser::DEFAULT, 0);
}

tree::TerminalNode* SimpleJSParser::DefaultClauseContext::COLON() {
  return getToken(SimpleJSParser::COLON, 0);
}

std::vector<SimpleJSParser::StatementContext *> SimpleJSParser::DefaultClauseContext::statement() {
  return getRuleContexts<SimpleJSParser::StatementContext>();
}

SimpleJSParser::StatementContext* SimpleJSParser::DefaultClauseContext::statement(size_t i) {
  return getRuleContext<SimpleJSParser::StatementContext>(i);
}


size_t SimpleJSParser::DefaultClauseContext::getRuleIndex() const {
  return SimpleJSParser::RuleDefaultClause;
}

void SimpleJSParser::DefaultClauseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefaultClause(this);
}

void SimpleJSParser::DefaultClauseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefaultClause(this);
}

SimpleJSParser::DefaultClauseContext* SimpleJSParser::defaultClause() {
  DefaultClauseContext *_localctx = _tracker.createInstance<DefaultClauseContext>(_ctx, getState());
  enterRule(_localctx, 40, SimpleJSParser::RuleDefaultClause);
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
    setState(304);
    match(SimpleJSParser::DEFAULT);
    setState(305);
    match(SimpleJSParser::COLON);
    setState(309);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8452525702365554) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 32851985) != 0)) {
      setState(306);
      statement();
      setState(311);
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

//----------------- BreakStatementContext ------------------------------------------------------------------

SimpleJSParser::BreakStatementContext::BreakStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::BreakStatementContext::BREAK() {
  return getToken(SimpleJSParser::BREAK, 0);
}

tree::TerminalNode* SimpleJSParser::BreakStatementContext::SEMICOLON() {
  return getToken(SimpleJSParser::SEMICOLON, 0);
}


size_t SimpleJSParser::BreakStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleBreakStatement;
}

void SimpleJSParser::BreakStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBreakStatement(this);
}

void SimpleJSParser::BreakStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBreakStatement(this);
}

SimpleJSParser::BreakStatementContext* SimpleJSParser::breakStatement() {
  BreakStatementContext *_localctx = _tracker.createInstance<BreakStatementContext>(_ctx, getState());
  enterRule(_localctx, 42, SimpleJSParser::RuleBreakStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(312);
    match(SimpleJSParser::BREAK);
    setState(314);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      setState(313);
      match(SimpleJSParser::SEMICOLON);
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

//----------------- ContinueStatementContext ------------------------------------------------------------------

SimpleJSParser::ContinueStatementContext::ContinueStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ContinueStatementContext::CONTINUE() {
  return getToken(SimpleJSParser::CONTINUE, 0);
}

tree::TerminalNode* SimpleJSParser::ContinueStatementContext::SEMICOLON() {
  return getToken(SimpleJSParser::SEMICOLON, 0);
}


size_t SimpleJSParser::ContinueStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleContinueStatement;
}

void SimpleJSParser::ContinueStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterContinueStatement(this);
}

void SimpleJSParser::ContinueStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitContinueStatement(this);
}

SimpleJSParser::ContinueStatementContext* SimpleJSParser::continueStatement() {
  ContinueStatementContext *_localctx = _tracker.createInstance<ContinueStatementContext>(_ctx, getState());
  enterRule(_localctx, 44, SimpleJSParser::RuleContinueStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(316);
    match(SimpleJSParser::CONTINUE);
    setState(318);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
    case 1: {
      setState(317);
      match(SimpleJSParser::SEMICOLON);
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

//----------------- EmptyStatementContext ------------------------------------------------------------------

SimpleJSParser::EmptyStatementContext::EmptyStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::EmptyStatementContext::SEMICOLON() {
  return getToken(SimpleJSParser::SEMICOLON, 0);
}


size_t SimpleJSParser::EmptyStatementContext::getRuleIndex() const {
  return SimpleJSParser::RuleEmptyStatement;
}

void SimpleJSParser::EmptyStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEmptyStatement(this);
}

void SimpleJSParser::EmptyStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEmptyStatement(this);
}

SimpleJSParser::EmptyStatementContext* SimpleJSParser::emptyStatement() {
  EmptyStatementContext *_localctx = _tracker.createInstance<EmptyStatementContext>(_ctx, getState());
  enterRule(_localctx, 46, SimpleJSParser::RuleEmptyStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(320);
    match(SimpleJSParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

SimpleJSParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::AssignmentExpressionContext* SimpleJSParser::ExpressionContext::assignmentExpression() {
  return getRuleContext<SimpleJSParser::AssignmentExpressionContext>(0);
}


size_t SimpleJSParser::ExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleExpression;
}

void SimpleJSParser::ExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpression(this);
}

void SimpleJSParser::ExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpression(this);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 48, SimpleJSParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(322);
    assignmentExpression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignmentExpressionContext ------------------------------------------------------------------

SimpleJSParser::AssignmentExpressionContext::AssignmentExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::ConditionalExpressionContext* SimpleJSParser::AssignmentExpressionContext::conditionalExpression() {
  return getRuleContext<SimpleJSParser::ConditionalExpressionContext>(0);
}

SimpleJSParser::LeftHandSideExpressionContext* SimpleJSParser::AssignmentExpressionContext::leftHandSideExpression() {
  return getRuleContext<SimpleJSParser::LeftHandSideExpressionContext>(0);
}

SimpleJSParser::AssignmentExpressionContext* SimpleJSParser::AssignmentExpressionContext::assignmentExpression() {
  return getRuleContext<SimpleJSParser::AssignmentExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::AssignmentExpressionContext::ASSIGN() {
  return getToken(SimpleJSParser::ASSIGN, 0);
}

tree::TerminalNode* SimpleJSParser::AssignmentExpressionContext::PLUS_ASSIGN() {
  return getToken(SimpleJSParser::PLUS_ASSIGN, 0);
}

tree::TerminalNode* SimpleJSParser::AssignmentExpressionContext::MINUS_ASSIGN() {
  return getToken(SimpleJSParser::MINUS_ASSIGN, 0);
}

tree::TerminalNode* SimpleJSParser::AssignmentExpressionContext::MULT_ASSIGN() {
  return getToken(SimpleJSParser::MULT_ASSIGN, 0);
}

tree::TerminalNode* SimpleJSParser::AssignmentExpressionContext::DIV_ASSIGN() {
  return getToken(SimpleJSParser::DIV_ASSIGN, 0);
}

tree::TerminalNode* SimpleJSParser::AssignmentExpressionContext::MOD_ASSIGN() {
  return getToken(SimpleJSParser::MOD_ASSIGN, 0);
}


size_t SimpleJSParser::AssignmentExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleAssignmentExpression;
}

void SimpleJSParser::AssignmentExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentExpression(this);
}

void SimpleJSParser::AssignmentExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentExpression(this);
}

SimpleJSParser::AssignmentExpressionContext* SimpleJSParser::assignmentExpression() {
  AssignmentExpressionContext *_localctx = _tracker.createInstance<AssignmentExpressionContext>(_ctx, getState());
  enterRule(_localctx, 50, SimpleJSParser::RuleAssignmentExpression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(329);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 35, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(324);
      conditionalExpression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(325);
      leftHandSideExpression();
      setState(326);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 554153860399104) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(327);
      assignmentExpression();
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

//----------------- ConditionalExpressionContext ------------------------------------------------------------------

SimpleJSParser::ConditionalExpressionContext::ConditionalExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::LogicalOrExpressionContext* SimpleJSParser::ConditionalExpressionContext::logicalOrExpression() {
  return getRuleContext<SimpleJSParser::LogicalOrExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::ConditionalExpressionContext::QUESTION() {
  return getToken(SimpleJSParser::QUESTION, 0);
}

std::vector<SimpleJSParser::ExpressionContext *> SimpleJSParser::ConditionalExpressionContext::expression() {
  return getRuleContexts<SimpleJSParser::ExpressionContext>();
}

SimpleJSParser::ExpressionContext* SimpleJSParser::ConditionalExpressionContext::expression(size_t i) {
  return getRuleContext<SimpleJSParser::ExpressionContext>(i);
}

tree::TerminalNode* SimpleJSParser::ConditionalExpressionContext::COLON() {
  return getToken(SimpleJSParser::COLON, 0);
}


size_t SimpleJSParser::ConditionalExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleConditionalExpression;
}

void SimpleJSParser::ConditionalExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConditionalExpression(this);
}

void SimpleJSParser::ConditionalExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConditionalExpression(this);
}

SimpleJSParser::ConditionalExpressionContext* SimpleJSParser::conditionalExpression() {
  ConditionalExpressionContext *_localctx = _tracker.createInstance<ConditionalExpressionContext>(_ctx, getState());
  enterRule(_localctx, 52, SimpleJSParser::RuleConditionalExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(331);
    logicalOrExpression();
    setState(337);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx)) {
    case 1: {
      setState(332);
      match(SimpleJSParser::QUESTION);
      setState(333);
      expression();
      setState(334);
      match(SimpleJSParser::COLON);
      setState(335);
      expression();
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

//----------------- LogicalOrExpressionContext ------------------------------------------------------------------

SimpleJSParser::LogicalOrExpressionContext::LogicalOrExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SimpleJSParser::LogicalAndExpressionContext *> SimpleJSParser::LogicalOrExpressionContext::logicalAndExpression() {
  return getRuleContexts<SimpleJSParser::LogicalAndExpressionContext>();
}

SimpleJSParser::LogicalAndExpressionContext* SimpleJSParser::LogicalOrExpressionContext::logicalAndExpression(size_t i) {
  return getRuleContext<SimpleJSParser::LogicalAndExpressionContext>(i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::LogicalOrExpressionContext::OR() {
  return getTokens(SimpleJSParser::OR);
}

tree::TerminalNode* SimpleJSParser::LogicalOrExpressionContext::OR(size_t i) {
  return getToken(SimpleJSParser::OR, i);
}


size_t SimpleJSParser::LogicalOrExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleLogicalOrExpression;
}

void SimpleJSParser::LogicalOrExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalOrExpression(this);
}

void SimpleJSParser::LogicalOrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalOrExpression(this);
}

SimpleJSParser::LogicalOrExpressionContext* SimpleJSParser::logicalOrExpression() {
  LogicalOrExpressionContext *_localctx = _tracker.createInstance<LogicalOrExpressionContext>(_ctx, getState());
  enterRule(_localctx, 54, SimpleJSParser::RuleLogicalOrExpression);

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
    setState(339);
    logicalAndExpression();
    setState(344);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(340);
        match(SimpleJSParser::OR);
        setState(341);
        logicalAndExpression(); 
      }
      setState(346);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LogicalAndExpressionContext ------------------------------------------------------------------

SimpleJSParser::LogicalAndExpressionContext::LogicalAndExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SimpleJSParser::EqualityExpressionContext *> SimpleJSParser::LogicalAndExpressionContext::equalityExpression() {
  return getRuleContexts<SimpleJSParser::EqualityExpressionContext>();
}

SimpleJSParser::EqualityExpressionContext* SimpleJSParser::LogicalAndExpressionContext::equalityExpression(size_t i) {
  return getRuleContext<SimpleJSParser::EqualityExpressionContext>(i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::LogicalAndExpressionContext::AND() {
  return getTokens(SimpleJSParser::AND);
}

tree::TerminalNode* SimpleJSParser::LogicalAndExpressionContext::AND(size_t i) {
  return getToken(SimpleJSParser::AND, i);
}


size_t SimpleJSParser::LogicalAndExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleLogicalAndExpression;
}

void SimpleJSParser::LogicalAndExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalAndExpression(this);
}

void SimpleJSParser::LogicalAndExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalAndExpression(this);
}

SimpleJSParser::LogicalAndExpressionContext* SimpleJSParser::logicalAndExpression() {
  LogicalAndExpressionContext *_localctx = _tracker.createInstance<LogicalAndExpressionContext>(_ctx, getState());
  enterRule(_localctx, 56, SimpleJSParser::RuleLogicalAndExpression);

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
    setState(347);
    equalityExpression();
    setState(352);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(348);
        match(SimpleJSParser::AND);
        setState(349);
        equalityExpression(); 
      }
      setState(354);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EqualityExpressionContext ------------------------------------------------------------------

SimpleJSParser::EqualityExpressionContext::EqualityExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SimpleJSParser::RelationalExpressionContext *> SimpleJSParser::EqualityExpressionContext::relationalExpression() {
  return getRuleContexts<SimpleJSParser::RelationalExpressionContext>();
}

SimpleJSParser::RelationalExpressionContext* SimpleJSParser::EqualityExpressionContext::relationalExpression(size_t i) {
  return getRuleContext<SimpleJSParser::RelationalExpressionContext>(i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::EqualityExpressionContext::EQ() {
  return getTokens(SimpleJSParser::EQ);
}

tree::TerminalNode* SimpleJSParser::EqualityExpressionContext::EQ(size_t i) {
  return getToken(SimpleJSParser::EQ, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::EqualityExpressionContext::NE() {
  return getTokens(SimpleJSParser::NE);
}

tree::TerminalNode* SimpleJSParser::EqualityExpressionContext::NE(size_t i) {
  return getToken(SimpleJSParser::NE, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::EqualityExpressionContext::EQ_STRICT() {
  return getTokens(SimpleJSParser::EQ_STRICT);
}

tree::TerminalNode* SimpleJSParser::EqualityExpressionContext::EQ_STRICT(size_t i) {
  return getToken(SimpleJSParser::EQ_STRICT, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::EqualityExpressionContext::NE_STRICT() {
  return getTokens(SimpleJSParser::NE_STRICT);
}

tree::TerminalNode* SimpleJSParser::EqualityExpressionContext::NE_STRICT(size_t i) {
  return getToken(SimpleJSParser::NE_STRICT, i);
}


size_t SimpleJSParser::EqualityExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleEqualityExpression;
}

void SimpleJSParser::EqualityExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEqualityExpression(this);
}

void SimpleJSParser::EqualityExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEqualityExpression(this);
}

SimpleJSParser::EqualityExpressionContext* SimpleJSParser::equalityExpression() {
  EqualityExpressionContext *_localctx = _tracker.createInstance<EqualityExpressionContext>(_ctx, getState());
  enterRule(_localctx, 58, SimpleJSParser::RuleEqualityExpression);
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
    setState(355);
    relationalExpression();
    setState(360);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(356);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 1080863910568919040) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(357);
        relationalExpression(); 
      }
      setState(362);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RelationalExpressionContext ------------------------------------------------------------------

SimpleJSParser::RelationalExpressionContext::RelationalExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SimpleJSParser::AdditiveExpressionContext *> SimpleJSParser::RelationalExpressionContext::additiveExpression() {
  return getRuleContexts<SimpleJSParser::AdditiveExpressionContext>();
}

SimpleJSParser::AdditiveExpressionContext* SimpleJSParser::RelationalExpressionContext::additiveExpression(size_t i) {
  return getRuleContext<SimpleJSParser::AdditiveExpressionContext>(i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::RelationalExpressionContext::LT() {
  return getTokens(SimpleJSParser::LT);
}

tree::TerminalNode* SimpleJSParser::RelationalExpressionContext::LT(size_t i) {
  return getToken(SimpleJSParser::LT, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::RelationalExpressionContext::LE() {
  return getTokens(SimpleJSParser::LE);
}

tree::TerminalNode* SimpleJSParser::RelationalExpressionContext::LE(size_t i) {
  return getToken(SimpleJSParser::LE, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::RelationalExpressionContext::GT() {
  return getTokens(SimpleJSParser::GT);
}

tree::TerminalNode* SimpleJSParser::RelationalExpressionContext::GT(size_t i) {
  return getToken(SimpleJSParser::GT, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::RelationalExpressionContext::GE() {
  return getTokens(SimpleJSParser::GE);
}

tree::TerminalNode* SimpleJSParser::RelationalExpressionContext::GE(size_t i) {
  return getToken(SimpleJSParser::GE, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::RelationalExpressionContext::INSTANCEOF() {
  return getTokens(SimpleJSParser::INSTANCEOF);
}

tree::TerminalNode* SimpleJSParser::RelationalExpressionContext::INSTANCEOF(size_t i) {
  return getToken(SimpleJSParser::INSTANCEOF, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::RelationalExpressionContext::IN() {
  return getTokens(SimpleJSParser::IN);
}

tree::TerminalNode* SimpleJSParser::RelationalExpressionContext::IN(size_t i) {
  return getToken(SimpleJSParser::IN, i);
}


size_t SimpleJSParser::RelationalExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleRelationalExpression;
}

void SimpleJSParser::RelationalExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRelationalExpression(this);
}

void SimpleJSParser::RelationalExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRelationalExpression(this);
}

SimpleJSParser::RelationalExpressionContext* SimpleJSParser::relationalExpression() {
  RelationalExpressionContext *_localctx = _tracker.createInstance<RelationalExpressionContext>(_ctx, getState());
  enterRule(_localctx, 60, SimpleJSParser::RuleRelationalExpression);
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
    setState(363);
    additiveExpression();
    setState(368);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(364);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & -1152921504606060544) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(365);
        additiveExpression(); 
      }
      setState(370);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AdditiveExpressionContext ------------------------------------------------------------------

SimpleJSParser::AdditiveExpressionContext::AdditiveExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SimpleJSParser::MultiplicativeExpressionContext *> SimpleJSParser::AdditiveExpressionContext::multiplicativeExpression() {
  return getRuleContexts<SimpleJSParser::MultiplicativeExpressionContext>();
}

SimpleJSParser::MultiplicativeExpressionContext* SimpleJSParser::AdditiveExpressionContext::multiplicativeExpression(size_t i) {
  return getRuleContext<SimpleJSParser::MultiplicativeExpressionContext>(i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::AdditiveExpressionContext::PLUS() {
  return getTokens(SimpleJSParser::PLUS);
}

tree::TerminalNode* SimpleJSParser::AdditiveExpressionContext::PLUS(size_t i) {
  return getToken(SimpleJSParser::PLUS, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::AdditiveExpressionContext::MINUS() {
  return getTokens(SimpleJSParser::MINUS);
}

tree::TerminalNode* SimpleJSParser::AdditiveExpressionContext::MINUS(size_t i) {
  return getToken(SimpleJSParser::MINUS, i);
}


size_t SimpleJSParser::AdditiveExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleAdditiveExpression;
}

void SimpleJSParser::AdditiveExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAdditiveExpression(this);
}

void SimpleJSParser::AdditiveExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAdditiveExpression(this);
}

SimpleJSParser::AdditiveExpressionContext* SimpleJSParser::additiveExpression() {
  AdditiveExpressionContext *_localctx = _tracker.createInstance<AdditiveExpressionContext>(_ctx, getState());
  enterRule(_localctx, 62, SimpleJSParser::RuleAdditiveExpression);
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
    setState(371);
    multiplicativeExpression();
    setState(376);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(372);
        _la = _input->LA(1);
        if (!(_la == SimpleJSParser::PLUS

        || _la == SimpleJSParser::MINUS)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(373);
        multiplicativeExpression(); 
      }
      setState(378);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MultiplicativeExpressionContext ------------------------------------------------------------------

SimpleJSParser::MultiplicativeExpressionContext::MultiplicativeExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SimpleJSParser::UnaryExpressionContext *> SimpleJSParser::MultiplicativeExpressionContext::unaryExpression() {
  return getRuleContexts<SimpleJSParser::UnaryExpressionContext>();
}

SimpleJSParser::UnaryExpressionContext* SimpleJSParser::MultiplicativeExpressionContext::unaryExpression(size_t i) {
  return getRuleContext<SimpleJSParser::UnaryExpressionContext>(i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::MultiplicativeExpressionContext::MULTIPLY() {
  return getTokens(SimpleJSParser::MULTIPLY);
}

tree::TerminalNode* SimpleJSParser::MultiplicativeExpressionContext::MULTIPLY(size_t i) {
  return getToken(SimpleJSParser::MULTIPLY, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::MultiplicativeExpressionContext::DIVIDE() {
  return getTokens(SimpleJSParser::DIVIDE);
}

tree::TerminalNode* SimpleJSParser::MultiplicativeExpressionContext::DIVIDE(size_t i) {
  return getToken(SimpleJSParser::DIVIDE, i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::MultiplicativeExpressionContext::MODULUS() {
  return getTokens(SimpleJSParser::MODULUS);
}

tree::TerminalNode* SimpleJSParser::MultiplicativeExpressionContext::MODULUS(size_t i) {
  return getToken(SimpleJSParser::MODULUS, i);
}


size_t SimpleJSParser::MultiplicativeExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleMultiplicativeExpression;
}

void SimpleJSParser::MultiplicativeExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMultiplicativeExpression(this);
}

void SimpleJSParser::MultiplicativeExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMultiplicativeExpression(this);
}

SimpleJSParser::MultiplicativeExpressionContext* SimpleJSParser::multiplicativeExpression() {
  MultiplicativeExpressionContext *_localctx = _tracker.createInstance<MultiplicativeExpressionContext>(_ctx, getState());
  enterRule(_localctx, 64, SimpleJSParser::RuleMultiplicativeExpression);
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
    setState(379);
    unaryExpression();
    setState(384);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(380);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 63050394783186944) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(381);
        unaryExpression(); 
      }
      setState(386);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryExpressionContext ------------------------------------------------------------------

SimpleJSParser::UnaryExpressionContext::UnaryExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::PostfixExpressionContext* SimpleJSParser::UnaryExpressionContext::postfixExpression() {
  return getRuleContext<SimpleJSParser::PostfixExpressionContext>(0);
}

SimpleJSParser::UnaryExpressionContext* SimpleJSParser::UnaryExpressionContext::unaryExpression() {
  return getRuleContext<SimpleJSParser::UnaryExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::PLUS_PLUS() {
  return getToken(SimpleJSParser::PLUS_PLUS, 0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::MINUS_MINUS() {
  return getToken(SimpleJSParser::MINUS_MINUS, 0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::PLUS() {
  return getToken(SimpleJSParser::PLUS, 0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::MINUS() {
  return getToken(SimpleJSParser::MINUS, 0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::NOT() {
  return getToken(SimpleJSParser::NOT, 0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::BIT_NOT() {
  return getToken(SimpleJSParser::BIT_NOT, 0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::TYPEOF() {
  return getToken(SimpleJSParser::TYPEOF, 0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::VOID() {
  return getToken(SimpleJSParser::VOID, 0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::DELETE() {
  return getToken(SimpleJSParser::DELETE, 0);
}

tree::TerminalNode* SimpleJSParser::UnaryExpressionContext::AWAIT() {
  return getToken(SimpleJSParser::AWAIT, 0);
}


size_t SimpleJSParser::UnaryExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleUnaryExpression;
}

void SimpleJSParser::UnaryExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryExpression(this);
}

void SimpleJSParser::UnaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryExpression(this);
}

SimpleJSParser::UnaryExpressionContext* SimpleJSParser::unaryExpression() {
  UnaryExpressionContext *_localctx = _tracker.createInstance<UnaryExpressionContext>(_ctx, getState());
  enterRule(_localctx, 66, SimpleJSParser::RuleUnaryExpression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(394);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SimpleJSParser::FUNCTION:
      case SimpleJSParser::NEW:
      case SimpleJSParser::SUPER:
      case SimpleJSParser::THIS:
      case SimpleJSParser::ASYNC:
      case SimpleJSParser::NULL_:
      case SimpleJSParser::TRUE:
      case SimpleJSParser::FALSE:
      case SimpleJSParser::UNDEFINED:
      case SimpleJSParser::LPAREN:
      case SimpleJSParser::LBRACE:
      case SimpleJSParser::LBRACKET:
      case SimpleJSParser::NUMBER:
      case SimpleJSParser::STRING:
      case SimpleJSParser::TEMPLATE:
      case SimpleJSParser::REGEX:
      case SimpleJSParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(387);
        postfixExpression();
        break;
      }

      case SimpleJSParser::PLUS_PLUS:
      case SimpleJSParser::MINUS_MINUS: {
        enterOuterAlt(_localctx, 2);
        setState(388);
        _la = _input->LA(1);
        if (!(_la == SimpleJSParser::PLUS_PLUS

        || _la == SimpleJSParser::MINUS_MINUS)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(389);
        unaryExpression();
        break;
      }

      case SimpleJSParser::DELETE:
      case SimpleJSParser::TYPEOF:
      case SimpleJSParser::VOID:
      case SimpleJSParser::PLUS:
      case SimpleJSParser::MINUS:
      case SimpleJSParser::NOT:
      case SimpleJSParser::BIT_NOT: {
        enterOuterAlt(_localctx, 3);
        setState(390);
        _la = _input->LA(1);
        if (!(((((_la - 8) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 8)) & 4899942782863409153) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(391);
        unaryExpression();
        break;
      }

      case SimpleJSParser::AWAIT: {
        enterOuterAlt(_localctx, 4);
        setState(392);
        match(SimpleJSParser::AWAIT);
        setState(393);
        unaryExpression();
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

//----------------- PostfixExpressionContext ------------------------------------------------------------------

SimpleJSParser::PostfixExpressionContext::PostfixExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::LeftHandSideExpressionContext* SimpleJSParser::PostfixExpressionContext::leftHandSideExpression() {
  return getRuleContext<SimpleJSParser::LeftHandSideExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::PostfixExpressionContext::PLUS_PLUS() {
  return getToken(SimpleJSParser::PLUS_PLUS, 0);
}

tree::TerminalNode* SimpleJSParser::PostfixExpressionContext::MINUS_MINUS() {
  return getToken(SimpleJSParser::MINUS_MINUS, 0);
}


size_t SimpleJSParser::PostfixExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RulePostfixExpression;
}

void SimpleJSParser::PostfixExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPostfixExpression(this);
}

void SimpleJSParser::PostfixExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPostfixExpression(this);
}

SimpleJSParser::PostfixExpressionContext* SimpleJSParser::postfixExpression() {
  PostfixExpressionContext *_localctx = _tracker.createInstance<PostfixExpressionContext>(_ctx, getState());
  enterRule(_localctx, 68, SimpleJSParser::RulePostfixExpression);
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
    setState(396);
    leftHandSideExpression();
    setState(398);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 44, _ctx)) {
    case 1: {
      setState(397);
      _la = _input->LA(1);
      if (!(_la == SimpleJSParser::PLUS_PLUS

      || _la == SimpleJSParser::MINUS_MINUS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
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

//----------------- LeftHandSideExpressionContext ------------------------------------------------------------------

SimpleJSParser::LeftHandSideExpressionContext::LeftHandSideExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::CallExpressionContext* SimpleJSParser::LeftHandSideExpressionContext::callExpression() {
  return getRuleContext<SimpleJSParser::CallExpressionContext>(0);
}

SimpleJSParser::MemberExpressionContext* SimpleJSParser::LeftHandSideExpressionContext::memberExpression() {
  return getRuleContext<SimpleJSParser::MemberExpressionContext>(0);
}


size_t SimpleJSParser::LeftHandSideExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleLeftHandSideExpression;
}

void SimpleJSParser::LeftHandSideExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLeftHandSideExpression(this);
}

void SimpleJSParser::LeftHandSideExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLeftHandSideExpression(this);
}

SimpleJSParser::LeftHandSideExpressionContext* SimpleJSParser::leftHandSideExpression() {
  LeftHandSideExpressionContext *_localctx = _tracker.createInstance<LeftHandSideExpressionContext>(_ctx, getState());
  enterRule(_localctx, 70, SimpleJSParser::RuleLeftHandSideExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(402);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(400);
      callExpression(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(401);
      memberExpression(0);
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

//----------------- CallExpressionContext ------------------------------------------------------------------

SimpleJSParser::CallExpressionContext::CallExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::MemberExpressionContext* SimpleJSParser::CallExpressionContext::memberExpression() {
  return getRuleContext<SimpleJSParser::MemberExpressionContext>(0);
}

SimpleJSParser::ArgumentsContext* SimpleJSParser::CallExpressionContext::arguments() {
  return getRuleContext<SimpleJSParser::ArgumentsContext>(0);
}

SimpleJSParser::CallExpressionContext* SimpleJSParser::CallExpressionContext::callExpression() {
  return getRuleContext<SimpleJSParser::CallExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::CallExpressionContext::LBRACKET() {
  return getToken(SimpleJSParser::LBRACKET, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::CallExpressionContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::CallExpressionContext::RBRACKET() {
  return getToken(SimpleJSParser::RBRACKET, 0);
}

tree::TerminalNode* SimpleJSParser::CallExpressionContext::DOT() {
  return getToken(SimpleJSParser::DOT, 0);
}

tree::TerminalNode* SimpleJSParser::CallExpressionContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}


size_t SimpleJSParser::CallExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleCallExpression;
}

void SimpleJSParser::CallExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCallExpression(this);
}

void SimpleJSParser::CallExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCallExpression(this);
}


SimpleJSParser::CallExpressionContext* SimpleJSParser::callExpression() {
   return callExpression(0);
}

SimpleJSParser::CallExpressionContext* SimpleJSParser::callExpression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SimpleJSParser::CallExpressionContext *_localctx = _tracker.createInstance<CallExpressionContext>(_ctx, parentState);
  SimpleJSParser::CallExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 72;
  enterRecursionRule(_localctx, 72, SimpleJSParser::RuleCallExpression, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(405);
    memberExpression(0);
    setState(406);
    arguments();
    _ctx->stop = _input->LT(-1);
    setState(420);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(418);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<CallExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleCallExpression);
          setState(408);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(409);
          arguments();
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<CallExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleCallExpression);
          setState(410);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(411);
          match(SimpleJSParser::LBRACKET);
          setState(412);
          expression();
          setState(413);
          match(SimpleJSParser::RBRACKET);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<CallExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleCallExpression);
          setState(415);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(416);
          match(SimpleJSParser::DOT);
          setState(417);
          match(SimpleJSParser::IDENTIFIER);
          break;
        }

        default:
          break;
        } 
      }
      setState(422);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- MemberExpressionContext ------------------------------------------------------------------

SimpleJSParser::MemberExpressionContext::MemberExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SimpleJSParser::PrimaryExpressionContext* SimpleJSParser::MemberExpressionContext::primaryExpression() {
  return getRuleContext<SimpleJSParser::PrimaryExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::MemberExpressionContext::NEW() {
  return getToken(SimpleJSParser::NEW, 0);
}

SimpleJSParser::MemberExpressionContext* SimpleJSParser::MemberExpressionContext::memberExpression() {
  return getRuleContext<SimpleJSParser::MemberExpressionContext>(0);
}

SimpleJSParser::ArgumentsContext* SimpleJSParser::MemberExpressionContext::arguments() {
  return getRuleContext<SimpleJSParser::ArgumentsContext>(0);
}

tree::TerminalNode* SimpleJSParser::MemberExpressionContext::LBRACKET() {
  return getToken(SimpleJSParser::LBRACKET, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::MemberExpressionContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::MemberExpressionContext::RBRACKET() {
  return getToken(SimpleJSParser::RBRACKET, 0);
}

tree::TerminalNode* SimpleJSParser::MemberExpressionContext::DOT() {
  return getToken(SimpleJSParser::DOT, 0);
}

tree::TerminalNode* SimpleJSParser::MemberExpressionContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}


size_t SimpleJSParser::MemberExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleMemberExpression;
}

void SimpleJSParser::MemberExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMemberExpression(this);
}

void SimpleJSParser::MemberExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMemberExpression(this);
}


SimpleJSParser::MemberExpressionContext* SimpleJSParser::memberExpression() {
   return memberExpression(0);
}

SimpleJSParser::MemberExpressionContext* SimpleJSParser::memberExpression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SimpleJSParser::MemberExpressionContext *_localctx = _tracker.createInstance<MemberExpressionContext>(_ctx, parentState);
  SimpleJSParser::MemberExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 74;
  enterRecursionRule(_localctx, 74, SimpleJSParser::RuleMemberExpression, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(430);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SimpleJSParser::FUNCTION:
      case SimpleJSParser::SUPER:
      case SimpleJSParser::THIS:
      case SimpleJSParser::ASYNC:
      case SimpleJSParser::NULL_:
      case SimpleJSParser::TRUE:
      case SimpleJSParser::FALSE:
      case SimpleJSParser::UNDEFINED:
      case SimpleJSParser::LPAREN:
      case SimpleJSParser::LBRACE:
      case SimpleJSParser::LBRACKET:
      case SimpleJSParser::NUMBER:
      case SimpleJSParser::STRING:
      case SimpleJSParser::TEMPLATE:
      case SimpleJSParser::REGEX:
      case SimpleJSParser::IDENTIFIER: {
        setState(424);
        primaryExpression();
        break;
      }

      case SimpleJSParser::NEW: {
        setState(425);
        match(SimpleJSParser::NEW);
        setState(426);
        memberExpression(0);
        setState(428);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 48, _ctx)) {
        case 1: {
          setState(427);
          arguments();
          break;
        }

        default:
          break;
        }
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(442);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 51, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(440);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<MemberExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleMemberExpression);
          setState(432);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(433);
          match(SimpleJSParser::LBRACKET);
          setState(434);
          expression();
          setState(435);
          match(SimpleJSParser::RBRACKET);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<MemberExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleMemberExpression);
          setState(437);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(438);
          match(SimpleJSParser::DOT);
          setState(439);
          match(SimpleJSParser::IDENTIFIER);
          break;
        }

        default:
          break;
        } 
      }
      setState(444);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 51, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- PrimaryExpressionContext ------------------------------------------------------------------

SimpleJSParser::PrimaryExpressionContext::PrimaryExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::PrimaryExpressionContext::THIS() {
  return getToken(SimpleJSParser::THIS, 0);
}

tree::TerminalNode* SimpleJSParser::PrimaryExpressionContext::SUPER() {
  return getToken(SimpleJSParser::SUPER, 0);
}

tree::TerminalNode* SimpleJSParser::PrimaryExpressionContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}

SimpleJSParser::LiteralContext* SimpleJSParser::PrimaryExpressionContext::literal() {
  return getRuleContext<SimpleJSParser::LiteralContext>(0);
}

SimpleJSParser::ArrayLiteralContext* SimpleJSParser::PrimaryExpressionContext::arrayLiteral() {
  return getRuleContext<SimpleJSParser::ArrayLiteralContext>(0);
}

SimpleJSParser::ObjectLiteralContext* SimpleJSParser::PrimaryExpressionContext::objectLiteral() {
  return getRuleContext<SimpleJSParser::ObjectLiteralContext>(0);
}

tree::TerminalNode* SimpleJSParser::PrimaryExpressionContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::PrimaryExpressionContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::PrimaryExpressionContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

SimpleJSParser::FunctionExpressionContext* SimpleJSParser::PrimaryExpressionContext::functionExpression() {
  return getRuleContext<SimpleJSParser::FunctionExpressionContext>(0);
}

SimpleJSParser::ArrowFunctionContext* SimpleJSParser::PrimaryExpressionContext::arrowFunction() {
  return getRuleContext<SimpleJSParser::ArrowFunctionContext>(0);
}


size_t SimpleJSParser::PrimaryExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RulePrimaryExpression;
}

void SimpleJSParser::PrimaryExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryExpression(this);
}

void SimpleJSParser::PrimaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryExpression(this);
}

SimpleJSParser::PrimaryExpressionContext* SimpleJSParser::primaryExpression() {
  PrimaryExpressionContext *_localctx = _tracker.createInstance<PrimaryExpressionContext>(_ctx, getState());
  enterRule(_localctx, 76, SimpleJSParser::RulePrimaryExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(457);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 52, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(445);
      match(SimpleJSParser::THIS);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(446);
      match(SimpleJSParser::SUPER);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(447);
      match(SimpleJSParser::IDENTIFIER);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(448);
      literal();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(449);
      arrayLiteral();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(450);
      objectLiteral();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(451);
      match(SimpleJSParser::LPAREN);
      setState(452);
      expression();
      setState(453);
      match(SimpleJSParser::RPAREN);
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(455);
      functionExpression();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(456);
      arrowFunction();
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

//----------------- FunctionExpressionContext ------------------------------------------------------------------

SimpleJSParser::FunctionExpressionContext::FunctionExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::FunctionExpressionContext::FUNCTION() {
  return getToken(SimpleJSParser::FUNCTION, 0);
}

tree::TerminalNode* SimpleJSParser::FunctionExpressionContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

tree::TerminalNode* SimpleJSParser::FunctionExpressionContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

SimpleJSParser::BlockContext* SimpleJSParser::FunctionExpressionContext::block() {
  return getRuleContext<SimpleJSParser::BlockContext>(0);
}

tree::TerminalNode* SimpleJSParser::FunctionExpressionContext::ASYNC() {
  return getToken(SimpleJSParser::ASYNC, 0);
}

tree::TerminalNode* SimpleJSParser::FunctionExpressionContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}

SimpleJSParser::ParameterListContext* SimpleJSParser::FunctionExpressionContext::parameterList() {
  return getRuleContext<SimpleJSParser::ParameterListContext>(0);
}


size_t SimpleJSParser::FunctionExpressionContext::getRuleIndex() const {
  return SimpleJSParser::RuleFunctionExpression;
}

void SimpleJSParser::FunctionExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionExpression(this);
}

void SimpleJSParser::FunctionExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionExpression(this);
}

SimpleJSParser::FunctionExpressionContext* SimpleJSParser::functionExpression() {
  FunctionExpressionContext *_localctx = _tracker.createInstance<FunctionExpressionContext>(_ctx, getState());
  enterRule(_localctx, 78, SimpleJSParser::RuleFunctionExpression);
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
    setState(460);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::ASYNC) {
      setState(459);
      match(SimpleJSParser::ASYNC);
    }
    setState(462);
    match(SimpleJSParser::FUNCTION);
    setState(464);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::IDENTIFIER) {
      setState(463);
      match(SimpleJSParser::IDENTIFIER);
    }
    setState(466);
    match(SimpleJSParser::LPAREN);
    setState(468);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SimpleJSParser::IDENTIFIER) {
      setState(467);
      parameterList();
    }
    setState(470);
    match(SimpleJSParser::RPAREN);
    setState(471);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArrowFunctionContext ------------------------------------------------------------------

SimpleJSParser::ArrowFunctionContext::ArrowFunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ArrowFunctionContext::ARROW() {
  return getToken(SimpleJSParser::ARROW, 0);
}

tree::TerminalNode* SimpleJSParser::ArrowFunctionContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* SimpleJSParser::ArrowFunctionContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

tree::TerminalNode* SimpleJSParser::ArrowFunctionContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::ArrowFunctionContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

SimpleJSParser::BlockContext* SimpleJSParser::ArrowFunctionContext::block() {
  return getRuleContext<SimpleJSParser::BlockContext>(0);
}

SimpleJSParser::ParameterListContext* SimpleJSParser::ArrowFunctionContext::parameterList() {
  return getRuleContext<SimpleJSParser::ParameterListContext>(0);
}


size_t SimpleJSParser::ArrowFunctionContext::getRuleIndex() const {
  return SimpleJSParser::RuleArrowFunction;
}

void SimpleJSParser::ArrowFunctionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrowFunction(this);
}

void SimpleJSParser::ArrowFunctionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrowFunction(this);
}

SimpleJSParser::ArrowFunctionContext* SimpleJSParser::arrowFunction() {
  ArrowFunctionContext *_localctx = _tracker.createInstance<ArrowFunctionContext>(_ctx, getState());
  enterRule(_localctx, 80, SimpleJSParser::RuleArrowFunction);
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
    setState(479);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SimpleJSParser::IDENTIFIER: {
        setState(473);
        match(SimpleJSParser::IDENTIFIER);
        break;
      }

      case SimpleJSParser::LPAREN: {
        setState(474);
        match(SimpleJSParser::LPAREN);
        setState(476);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == SimpleJSParser::IDENTIFIER) {
          setState(475);
          parameterList();
        }
        setState(478);
        match(SimpleJSParser::RPAREN);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(481);
    match(SimpleJSParser::ARROW);
    setState(484);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 58, _ctx)) {
    case 1: {
      setState(482);
      expression();
      break;
    }

    case 2: {
      setState(483);
      block();
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

//----------------- ArgumentsContext ------------------------------------------------------------------

SimpleJSParser::ArgumentsContext::ArgumentsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ArgumentsContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

tree::TerminalNode* SimpleJSParser::ArgumentsContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

std::vector<SimpleJSParser::ExpressionContext *> SimpleJSParser::ArgumentsContext::expression() {
  return getRuleContexts<SimpleJSParser::ExpressionContext>();
}

SimpleJSParser::ExpressionContext* SimpleJSParser::ArgumentsContext::expression(size_t i) {
  return getRuleContext<SimpleJSParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::ArgumentsContext::COMMA() {
  return getTokens(SimpleJSParser::COMMA);
}

tree::TerminalNode* SimpleJSParser::ArgumentsContext::COMMA(size_t i) {
  return getToken(SimpleJSParser::COMMA, i);
}


size_t SimpleJSParser::ArgumentsContext::getRuleIndex() const {
  return SimpleJSParser::RuleArguments;
}

void SimpleJSParser::ArgumentsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArguments(this);
}

void SimpleJSParser::ArgumentsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArguments(this);
}

SimpleJSParser::ArgumentsContext* SimpleJSParser::arguments() {
  ArgumentsContext *_localctx = _tracker.createInstance<ArgumentsContext>(_ctx, getState());
  enterRule(_localctx, 82, SimpleJSParser::RuleArguments);
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
    setState(486);
    match(SimpleJSParser::LPAREN);
    setState(495);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8452522794582272) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 32849937) != 0)) {
      setState(487);
      expression();
      setState(492);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SimpleJSParser::COMMA) {
        setState(488);
        match(SimpleJSParser::COMMA);
        setState(489);
        expression();
        setState(494);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(497);
    match(SimpleJSParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArrayLiteralContext ------------------------------------------------------------------

SimpleJSParser::ArrayLiteralContext::ArrayLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ArrayLiteralContext::LBRACKET() {
  return getToken(SimpleJSParser::LBRACKET, 0);
}

tree::TerminalNode* SimpleJSParser::ArrayLiteralContext::RBRACKET() {
  return getToken(SimpleJSParser::RBRACKET, 0);
}

std::vector<SimpleJSParser::ExpressionContext *> SimpleJSParser::ArrayLiteralContext::expression() {
  return getRuleContexts<SimpleJSParser::ExpressionContext>();
}

SimpleJSParser::ExpressionContext* SimpleJSParser::ArrayLiteralContext::expression(size_t i) {
  return getRuleContext<SimpleJSParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::ArrayLiteralContext::COMMA() {
  return getTokens(SimpleJSParser::COMMA);
}

tree::TerminalNode* SimpleJSParser::ArrayLiteralContext::COMMA(size_t i) {
  return getToken(SimpleJSParser::COMMA, i);
}


size_t SimpleJSParser::ArrayLiteralContext::getRuleIndex() const {
  return SimpleJSParser::RuleArrayLiteral;
}

void SimpleJSParser::ArrayLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayLiteral(this);
}

void SimpleJSParser::ArrayLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrayLiteral(this);
}

SimpleJSParser::ArrayLiteralContext* SimpleJSParser::arrayLiteral() {
  ArrayLiteralContext *_localctx = _tracker.createInstance<ArrayLiteralContext>(_ctx, getState());
  enterRule(_localctx, 84, SimpleJSParser::RuleArrayLiteral);
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
    setState(499);
    match(SimpleJSParser::LBRACKET);
    setState(508);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8452522794582272) != 0) || ((((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 32849937) != 0)) {
      setState(500);
      expression();
      setState(505);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SimpleJSParser::COMMA) {
        setState(501);
        match(SimpleJSParser::COMMA);
        setState(502);
        expression();
        setState(507);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(510);
    match(SimpleJSParser::RBRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ObjectLiteralContext ------------------------------------------------------------------

SimpleJSParser::ObjectLiteralContext::ObjectLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::ObjectLiteralContext::LBRACE() {
  return getToken(SimpleJSParser::LBRACE, 0);
}

tree::TerminalNode* SimpleJSParser::ObjectLiteralContext::RBRACE() {
  return getToken(SimpleJSParser::RBRACE, 0);
}

std::vector<SimpleJSParser::PropertyAssignmentContext *> SimpleJSParser::ObjectLiteralContext::propertyAssignment() {
  return getRuleContexts<SimpleJSParser::PropertyAssignmentContext>();
}

SimpleJSParser::PropertyAssignmentContext* SimpleJSParser::ObjectLiteralContext::propertyAssignment(size_t i) {
  return getRuleContext<SimpleJSParser::PropertyAssignmentContext>(i);
}

std::vector<tree::TerminalNode *> SimpleJSParser::ObjectLiteralContext::COMMA() {
  return getTokens(SimpleJSParser::COMMA);
}

tree::TerminalNode* SimpleJSParser::ObjectLiteralContext::COMMA(size_t i) {
  return getToken(SimpleJSParser::COMMA, i);
}


size_t SimpleJSParser::ObjectLiteralContext::getRuleIndex() const {
  return SimpleJSParser::RuleObjectLiteral;
}

void SimpleJSParser::ObjectLiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterObjectLiteral(this);
}

void SimpleJSParser::ObjectLiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitObjectLiteral(this);
}

SimpleJSParser::ObjectLiteralContext* SimpleJSParser::objectLiteral() {
  ObjectLiteralContext *_localctx = _tracker.createInstance<ObjectLiteralContext>(_ctx, getState());
  enterRule(_localctx, 86, SimpleJSParser::RuleObjectLiteral);
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
    setState(512);
    match(SimpleJSParser::LBRACE);
    setState(521);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 86) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 86)) & 19) != 0)) {
      setState(513);
      propertyAssignment();
      setState(518);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SimpleJSParser::COMMA) {
        setState(514);
        match(SimpleJSParser::COMMA);
        setState(515);
        propertyAssignment();
        setState(520);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(523);
    match(SimpleJSParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyAssignmentContext ------------------------------------------------------------------

SimpleJSParser::PropertyAssignmentContext::PropertyAssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::PropertyAssignmentContext::IDENTIFIER() {
  return getToken(SimpleJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* SimpleJSParser::PropertyAssignmentContext::COLON() {
  return getToken(SimpleJSParser::COLON, 0);
}

SimpleJSParser::ExpressionContext* SimpleJSParser::PropertyAssignmentContext::expression() {
  return getRuleContext<SimpleJSParser::ExpressionContext>(0);
}

tree::TerminalNode* SimpleJSParser::PropertyAssignmentContext::STRING() {
  return getToken(SimpleJSParser::STRING, 0);
}

tree::TerminalNode* SimpleJSParser::PropertyAssignmentContext::NUMBER() {
  return getToken(SimpleJSParser::NUMBER, 0);
}

tree::TerminalNode* SimpleJSParser::PropertyAssignmentContext::LPAREN() {
  return getToken(SimpleJSParser::LPAREN, 0);
}

tree::TerminalNode* SimpleJSParser::PropertyAssignmentContext::RPAREN() {
  return getToken(SimpleJSParser::RPAREN, 0);
}

SimpleJSParser::BlockContext* SimpleJSParser::PropertyAssignmentContext::block() {
  return getRuleContext<SimpleJSParser::BlockContext>(0);
}

SimpleJSParser::ParameterListContext* SimpleJSParser::PropertyAssignmentContext::parameterList() {
  return getRuleContext<SimpleJSParser::ParameterListContext>(0);
}


size_t SimpleJSParser::PropertyAssignmentContext::getRuleIndex() const {
  return SimpleJSParser::RulePropertyAssignment;
}

void SimpleJSParser::PropertyAssignmentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyAssignment(this);
}

void SimpleJSParser::PropertyAssignmentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyAssignment(this);
}

SimpleJSParser::PropertyAssignmentContext* SimpleJSParser::propertyAssignment() {
  PropertyAssignmentContext *_localctx = _tracker.createInstance<PropertyAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 88, SimpleJSParser::RulePropertyAssignment);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(541);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 66, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(525);
      match(SimpleJSParser::IDENTIFIER);
      setState(526);
      match(SimpleJSParser::COLON);
      setState(527);
      expression();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(528);
      match(SimpleJSParser::STRING);
      setState(529);
      match(SimpleJSParser::COLON);
      setState(530);
      expression();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(531);
      match(SimpleJSParser::NUMBER);
      setState(532);
      match(SimpleJSParser::COLON);
      setState(533);
      expression();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(534);
      match(SimpleJSParser::IDENTIFIER);
      setState(535);
      match(SimpleJSParser::LPAREN);
      setState(537);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == SimpleJSParser::IDENTIFIER) {
        setState(536);
        parameterList();
      }
      setState(539);
      match(SimpleJSParser::RPAREN);
      setState(540);
      block();
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

//----------------- LiteralContext ------------------------------------------------------------------

SimpleJSParser::LiteralContext::LiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SimpleJSParser::LiteralContext::NULL_() {
  return getToken(SimpleJSParser::NULL_, 0);
}

tree::TerminalNode* SimpleJSParser::LiteralContext::TRUE() {
  return getToken(SimpleJSParser::TRUE, 0);
}

tree::TerminalNode* SimpleJSParser::LiteralContext::FALSE() {
  return getToken(SimpleJSParser::FALSE, 0);
}

tree::TerminalNode* SimpleJSParser::LiteralContext::UNDEFINED() {
  return getToken(SimpleJSParser::UNDEFINED, 0);
}

tree::TerminalNode* SimpleJSParser::LiteralContext::NUMBER() {
  return getToken(SimpleJSParser::NUMBER, 0);
}

tree::TerminalNode* SimpleJSParser::LiteralContext::STRING() {
  return getToken(SimpleJSParser::STRING, 0);
}

tree::TerminalNode* SimpleJSParser::LiteralContext::TEMPLATE() {
  return getToken(SimpleJSParser::TEMPLATE, 0);
}

tree::TerminalNode* SimpleJSParser::LiteralContext::REGEX() {
  return getToken(SimpleJSParser::REGEX, 0);
}


size_t SimpleJSParser::LiteralContext::getRuleIndex() const {
  return SimpleJSParser::RuleLiteral;
}

void SimpleJSParser::LiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteral(this);
}

void SimpleJSParser::LiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SimpleJSParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteral(this);
}

SimpleJSParser::LiteralContext* SimpleJSParser::literal() {
  LiteralContext *_localctx = _tracker.createInstance<LiteralContext>(_ctx, getState());
  enterRule(_localctx, 90, SimpleJSParser::RuleLiteral);
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
    setState(543);
    _la = _input->LA(1);
    if (!(((((_la - 39) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 39)) & 2111062325329935) != 0))) {
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

bool SimpleJSParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 36: return callExpressionSempred(antlrcpp::downCast<CallExpressionContext *>(context), predicateIndex);
    case 37: return memberExpressionSempred(antlrcpp::downCast<MemberExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool SimpleJSParser::callExpressionSempred(CallExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 3);
    case 1: return precpred(_ctx, 2);
    case 2: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool SimpleJSParser::memberExpressionSempred(MemberExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 3: return precpred(_ctx, 3);
    case 4: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

void SimpleJSParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  simplejsparserParserInitialize();
#else
  ::antlr4::internal::call_once(simplejsparserParserOnceFlag, simplejsparserParserInitialize);
#endif
}
