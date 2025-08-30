
// Generated from SimpleJSLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  SimpleJSLexer : public antlr4::Lexer {
public:
  enum {
    BREAK = 1, CASE = 2, CATCH = 3, CLASS = 4, CONST = 5, CONTINUE = 6, 
    DEFAULT = 7, DELETE = 8, DO = 9, ELSE = 10, EXPORT = 11, EXTENDS = 12, 
    FINALLY = 13, FOR = 14, FUNCTION = 15, IF = 16, IMPORT = 17, IN = 18, 
    INSTANCEOF = 19, LET = 20, NEW = 21, RETURN = 22, SUPER = 23, SWITCH = 24, 
    THIS = 25, THROW = 26, TRY = 27, TYPEOF = 28, VAR = 29, VOID = 30, WHILE = 31, 
    WITH = 32, ASYNC = 33, AWAIT = 34, YIELD = 35, OF = 36, FROM = 37, AS = 38, 
    NULL_ = 39, TRUE = 40, FALSE = 41, UNDEFINED = 42, ASSIGN = 43, PLUS_ASSIGN = 44, 
    MINUS_ASSIGN = 45, MULT_ASSIGN = 46, DIV_ASSIGN = 47, MOD_ASSIGN = 48, 
    PLUS_PLUS = 49, MINUS_MINUS = 50, PLUS = 51, MINUS = 52, MULTIPLY = 53, 
    DIVIDE = 54, MODULUS = 55, EQ = 56, NE = 57, EQ_STRICT = 58, NE_STRICT = 59, 
    LT = 60, LE = 61, GT = 62, GE = 63, AND = 64, OR = 65, NOT = 66, BIT_AND = 67, 
    BIT_OR = 68, BIT_XOR = 69, BIT_NOT = 70, LSHIFT = 71, RSHIFT = 72, URSHIFT = 73, 
    QUESTION = 74, COLON = 75, ARROW = 76, SEMICOLON = 77, COMMA = 78, DOT = 79, 
    LPAREN = 80, RPAREN = 81, LBRACE = 82, RBRACE = 83, LBRACKET = 84, RBRACKET = 85, 
    NUMBER = 86, STRING = 87, TEMPLATE = 88, REGEX = 89, IDENTIFIER = 90, 
    LINE_COMMENT = 91, BLOCK_COMMENT = 92, WS = 93, ANY = 94
  };

  enum {
    COMMENTS = 2
  };

  explicit SimpleJSLexer(antlr4::CharStream *input);

  ~SimpleJSLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

