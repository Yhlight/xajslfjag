
// Generated from src/CSS/CSS3.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSS3Lexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, Ident = 20, 
    Comment = 21, Space = 22, Cdo = 23, Cdc = 24, Includes = 25, DashMatch = 26, 
    Hash = 27, Important = 28, Plus = 29, Minus = 30, Number = 31, Percentage = 32, 
    Dimension = 33, String_ = 34, Uri = 35, UnicodeRange = 36, MediaOnly = 37, 
    Not = 38, And = 39, Or = 40, From = 41, To = 42, AtKeyword = 43, Media = 44, 
    Page = 45, FontFace = 46, Keyframes = 47, Viewport = 48, Supports = 49, 
    Calc = 50, Var = 51, Function = 52, PseudoNot = 53, Variable = 54, Comma = 55, 
    NewlineToken = 56, UnknownAtRule = 57
  };

  explicit CSS3Lexer(antlr4::CharStream *input);

  ~CSS3Lexer() override;


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

