
// Generated from src/CSS/css3LexerFixed.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  css3LexerFixed : public antlr4::Lexer {
public:
  enum {
    Comment = 1, Space = 2, Cdo = 3, Cdc = 4, Includes = 5, DashMatch = 6, 
    Hash = 7, Import = 8, Page = 9, Media = 10, Namespace = 11, Charset = 12, 
    Important = 13, Percentage = 14, Length = 15, Dimension = 16, UnknownDimension = 17, 
    Number = 18, String_ = 19, PseudoNot = 20, Uri = 21, Ident = 22, Function_ = 23, 
    Semicolon = 24, Comma = 25, LeftBrace = 26, RightBrace = 27, LeftParen = 28, 
    RightParen = 29, LeftBracket = 30, RightBracket = 31, Colon = 32, Dot = 33, 
    Plus = 34, Minus = 35, Star = 36, Slash = 37, Equals = 38, Pipe = 39, 
    Tilde = 40, Greater = 41, Dollar = 42, Caret = 43, Only = 44, Not = 45, 
    And = 46, TildeEquals = 47, PipeEquals = 48, CaretEquals = 49, DollarEquals = 50, 
    StarEquals = 51, AnyChar = 52
  };

  enum {
    ERROR = 2
  };

  explicit css3LexerFixed(antlr4::CharStream *input);

  ~css3LexerFixed() override;


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

