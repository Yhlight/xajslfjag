
// Generated from D:/new/javascript/javascript/JavaScriptLexer.g4 by ANTLR 4.13.2

#pragma once

#include "JavaScriptLexerBase.h"


#include "antlr4-runtime.h"




class  JavaScriptLexer : public JavaScriptLexerBase {
public:
  enum {
    HashBangLine = 1, MultiLineComment = 2, SingleLineComment = 3, RegularExpressionLiteral = 4, 
    OpenBracket = 5, CloseBracket = 6, OpenParen = 7, CloseParen = 8, OpenBrace = 9, 
    TemplateCloseBrace = 10, CloseBrace = 11, SemiColon = 12, Comma = 13, 
    Assign = 14, QuestionMark = 15, QuestionMarkDot = 16, Colon = 17, Ellipsis = 18, 
    Dot = 19, PlusPlus = 20, MinusMinus = 21, Plus = 22, Minus = 23, BitNot = 24, 
    Not = 25, Multiply = 26, Divide = 27, Modulus = 28, Power = 29, NullCoalesce = 30, 
    Hashtag = 31, RightShiftArithmetic = 32, LeftShiftArithmetic = 33, RightShiftLogical = 34, 
    LessThan = 35, MoreThan = 36, LessThanEquals = 37, GreaterThanEquals = 38, 
    Equals_ = 39, NotEquals = 40, IdentityEquals = 41, IdentityNotEquals = 42, 
    BitAnd = 43, BitXOr = 44, BitOr = 45, And = 46, Or = 47, MultiplyAssign = 48, 
    DivideAssign = 49, ModulusAssign = 50, PlusAssign = 51, MinusAssign = 52, 
    LeftShiftArithmeticAssign = 53, RightShiftArithmeticAssign = 54, RightShiftLogicalAssign = 55, 
    BitAndAssign = 56, BitXorAssign = 57, BitOrAssign = 58, PowerAssign = 59, 
    NullishCoalescingAssign = 60, ARROW = 61, NullLiteral = 62, BooleanLiteral = 63, 
    DecimalLiteral = 64, HexIntegerLiteral = 65, OctalIntegerLiteral = 66, 
    OctalIntegerLiteral2 = 67, BinaryIntegerLiteral = 68, BigHexIntegerLiteral = 69, 
    BigOctalIntegerLiteral = 70, BigBinaryIntegerLiteral = 71, BigDecimalIntegerLiteral = 72, 
    Break = 73, Do = 74, Instanceof = 75, Typeof = 76, Case = 77, Else = 78, 
    New = 79, Var = 80, Catch = 81, Finally = 82, Return = 83, Void = 84, 
    Continue = 85, For = 86, Switch = 87, While = 88, Debugger = 89, Function_ = 90, 
    This = 91, With = 92, Default = 93, If = 94, Throw = 95, Delete = 96, 
    In = 97, Try = 98, As = 99, From = 100, Of = 101, Yield = 102, YieldStar = 103, 
    Class = 104, Enum = 105, Extends = 106, Super = 107, Const = 108, Export = 109, 
    Import = 110, Async = 111, Await = 112, Implements = 113, StrictLet = 114, 
    NonStrictLet = 115, Private = 116, Public = 117, Interface = 118, Package = 119, 
    Protected = 120, Static = 121, Identifier = 122, StringLiteral = 123, 
    BackTick = 124, WhiteSpaces = 125, LineTerminator = 126, HtmlComment = 127, 
    CDataComment = 128, UnexpectedCharacter = 129, TemplateStringStartExpression = 130, 
    TemplateStringAtom = 131
  };

  enum {
    ERROR = 2
  };

  enum {
    TEMPLATE = 1
  };

  explicit JavaScriptLexer(antlr4::CharStream *input);

  ~JavaScriptLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  void action(antlr4::RuleContext *context, size_t ruleIndex, size_t actionIndex) override;

  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.
  void OpenBraceAction(antlr4::RuleContext *context, size_t actionIndex);
  void TemplateCloseBraceAction(antlr4::RuleContext *context, size_t actionIndex);
  void CloseBraceAction(antlr4::RuleContext *context, size_t actionIndex);
  void StringLiteralAction(antlr4::RuleContext *context, size_t actionIndex);
  void TemplateStringStartExpressionAction(antlr4::RuleContext *context, size_t actionIndex);

  // Individual semantic predicate functions triggered by sempred() above.
  bool HashBangLineSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool RegularExpressionLiteralSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool TemplateCloseBraceSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool OctalIntegerLiteralSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ImplementsSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool StrictLetSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool NonStrictLetSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PrivateSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PublicSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool InterfaceSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool PackageSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool ProtectedSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);
  bool StaticSempred(antlr4::RuleContext *_localctx, size_t predicateIndex);

};

