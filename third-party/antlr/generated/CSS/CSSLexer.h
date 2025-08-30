
// Generated from CSSLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSSLexer : public antlr4::Lexer {
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
    ERROR = 2
  };

  explicit CSSLexer(antlr4::CharStream *input);

  ~CSSLexer() override;


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

