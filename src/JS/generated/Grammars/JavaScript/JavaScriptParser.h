
// Generated from Grammars/JavaScript/JavaScriptParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  JavaScriptParser : public JavaScriptParserBase {
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
    RuleProgram = 0, RuleSourceElement = 1, RuleStatement = 2, RuleBlock = 3, 
    RuleStatementList = 4, RuleImportStatement = 5, RuleImportFromBlock = 6, 
    RuleImportModuleItems = 7, RuleImportAliasName = 8, RuleModuleExportName = 9, 
    RuleImportedBinding = 10, RuleImportDefault = 11, RuleImportNamespace = 12, 
    RuleImportFrom = 13, RuleAliasName = 14, RuleExportStatement = 15, RuleExportFromBlock = 16, 
    RuleExportModuleItems = 17, RuleExportAliasName = 18, RuleDeclaration = 19, 
    RuleVariableStatement = 20, RuleVariableDeclarationList = 21, RuleVariableDeclaration = 22, 
    RuleEmptyStatement_ = 23, RuleExpressionStatement = 24, RuleIfStatement = 25, 
    RuleIterationStatement = 26, RuleVarModifier = 27, RuleContinueStatement = 28, 
    RuleBreakStatement = 29, RuleReturnStatement = 30, RuleYieldStatement = 31, 
    RuleWithStatement = 32, RuleSwitchStatement = 33, RuleCaseBlock = 34, 
    RuleCaseClauses = 35, RuleCaseClause = 36, RuleDefaultClause = 37, RuleLabelledStatement = 38, 
    RuleThrowStatement = 39, RuleTryStatement = 40, RuleCatchProduction = 41, 
    RuleFinallyProduction = 42, RuleDebuggerStatement = 43, RuleFunctionDeclaration = 44, 
    RuleClassDeclaration = 45, RuleClassTail = 46, RuleClassElement = 47, 
    RuleMethodDefinition = 48, RuleFieldDefinition = 49, RuleClassElementName = 50, 
    RulePrivateIdentifier = 51, RuleFormalParameterList = 52, RuleFormalParameterArg = 53, 
    RuleLastFormalParameterArg = 54, RuleFunctionBody = 55, RuleSourceElements = 56, 
    RuleArrayLiteral = 57, RuleElementList = 58, RuleArrayElement = 59, 
    RulePropertyAssignment = 60, RulePropertyName = 61, RuleArguments = 62, 
    RuleArgument = 63, RuleExpressionSequence = 64, RuleSingleExpression = 65, 
    RuleInitializer = 66, RuleAssignable = 67, RuleObjectLiteral = 68, RuleAnonymousFunction = 69, 
    RuleArrowFunctionParameters = 70, RuleArrowFunctionBody = 71, RuleAssignmentOperator = 72, 
    RuleLiteral = 73, RuleTemplateStringLiteral = 74, RuleTemplateStringAtom = 75, 
    RuleNumericLiteral = 76, RuleBigintLiteral = 77, RuleGetter = 78, RuleSetter = 79, 
    RuleIdentifierName = 80, RuleIdentifier = 81, RuleReservedWord = 82, 
    RuleKeyword = 83, RuleLet_ = 84, RuleEos = 85
  };

  explicit JavaScriptParser(antlr4::TokenStream *input);

  JavaScriptParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~JavaScriptParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ProgramContext;
  class SourceElementContext;
  class StatementContext;
  class BlockContext;
  class StatementListContext;
  class ImportStatementContext;
  class ImportFromBlockContext;
  class ImportModuleItemsContext;
  class ImportAliasNameContext;
  class ModuleExportNameContext;
  class ImportedBindingContext;
  class ImportDefaultContext;
  class ImportNamespaceContext;
  class ImportFromContext;
  class AliasNameContext;
  class ExportStatementContext;
  class ExportFromBlockContext;
  class ExportModuleItemsContext;
  class ExportAliasNameContext;
  class DeclarationContext;
  class VariableStatementContext;
  class VariableDeclarationListContext;
  class VariableDeclarationContext;
  class EmptyStatement_Context;
  class ExpressionStatementContext;
  class IfStatementContext;
  class IterationStatementContext;
  class VarModifierContext;
  class ContinueStatementContext;
  class BreakStatementContext;
  class ReturnStatementContext;
  class YieldStatementContext;
  class WithStatementContext;
  class SwitchStatementContext;
  class CaseBlockContext;
  class CaseClausesContext;
  class CaseClauseContext;
  class DefaultClauseContext;
  class LabelledStatementContext;
  class ThrowStatementContext;
  class TryStatementContext;
  class CatchProductionContext;
  class FinallyProductionContext;
  class DebuggerStatementContext;
  class FunctionDeclarationContext;
  class ClassDeclarationContext;
  class ClassTailContext;
  class ClassElementContext;
  class MethodDefinitionContext;
  class FieldDefinitionContext;
  class ClassElementNameContext;
  class PrivateIdentifierContext;
  class FormalParameterListContext;
  class FormalParameterArgContext;
  class LastFormalParameterArgContext;
  class FunctionBodyContext;
  class SourceElementsContext;
  class ArrayLiteralContext;
  class ElementListContext;
  class ArrayElementContext;
  class PropertyAssignmentContext;
  class PropertyNameContext;
  class ArgumentsContext;
  class ArgumentContext;
  class ExpressionSequenceContext;
  class SingleExpressionContext;
  class InitializerContext;
  class AssignableContext;
  class ObjectLiteralContext;
  class AnonymousFunctionContext;
  class ArrowFunctionParametersContext;
  class ArrowFunctionBodyContext;
  class AssignmentOperatorContext;
  class LiteralContext;
  class TemplateStringLiteralContext;
  class TemplateStringAtomContext;
  class NumericLiteralContext;
  class BigintLiteralContext;
  class GetterContext;
  class SetterContext;
  class IdentifierNameContext;
  class IdentifierContext;
  class ReservedWordContext;
  class KeywordContext;
  class Let_Context;
  class EosContext; 

  class  ProgramContext : public antlr4::ParserRuleContext {
  public:
    ProgramContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    antlr4::tree::TerminalNode *HashBangLine();
    SourceElementsContext *sourceElements();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProgramContext* program();

  class  SourceElementContext : public antlr4::ParserRuleContext {
  public:
    SourceElementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    StatementContext *statement();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SourceElementContext* sourceElement();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BlockContext *block();
    VariableStatementContext *variableStatement();
    ImportStatementContext *importStatement();
    ExportStatementContext *exportStatement();
    EmptyStatement_Context *emptyStatement_();
    ClassDeclarationContext *classDeclaration();
    FunctionDeclarationContext *functionDeclaration();
    ExpressionStatementContext *expressionStatement();
    IfStatementContext *ifStatement();
    IterationStatementContext *iterationStatement();
    ContinueStatementContext *continueStatement();
    BreakStatementContext *breakStatement();
    ReturnStatementContext *returnStatement();
    YieldStatementContext *yieldStatement();
    WithStatementContext *withStatement();
    LabelledStatementContext *labelledStatement();
    SwitchStatementContext *switchStatement();
    ThrowStatementContext *throwStatement();
    TryStatementContext *tryStatement();
    DebuggerStatementContext *debuggerStatement();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StatementContext* statement();

  class  BlockContext : public antlr4::ParserRuleContext {
  public:
    BlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OpenBrace();
    antlr4::tree::TerminalNode *CloseBrace();
    StatementListContext *statementList();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlockContext* block();

  class  StatementListContext : public antlr4::ParserRuleContext {
  public:
    StatementListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StatementListContext* statementList();

  class  ImportStatementContext : public antlr4::ParserRuleContext {
  public:
    ImportStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Import();
    ImportFromBlockContext *importFromBlock();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportStatementContext* importStatement();

  class  ImportFromBlockContext : public antlr4::ParserRuleContext {
  public:
    ImportFromBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ImportFromContext *importFrom();
    EosContext *eos();
    ImportNamespaceContext *importNamespace();
    ImportModuleItemsContext *importModuleItems();
    ImportDefaultContext *importDefault();
    antlr4::tree::TerminalNode *StringLiteral();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportFromBlockContext* importFromBlock();

  class  ImportModuleItemsContext : public antlr4::ParserRuleContext {
  public:
    ImportModuleItemsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OpenBrace();
    antlr4::tree::TerminalNode *CloseBrace();
    std::vector<ImportAliasNameContext *> importAliasName();
    ImportAliasNameContext* importAliasName(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportModuleItemsContext* importModuleItems();

  class  ImportAliasNameContext : public antlr4::ParserRuleContext {
  public:
    ImportAliasNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ModuleExportNameContext *moduleExportName();
    antlr4::tree::TerminalNode *As();
    ImportedBindingContext *importedBinding();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportAliasNameContext* importAliasName();

  class  ModuleExportNameContext : public antlr4::ParserRuleContext {
  public:
    ModuleExportNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierNameContext *identifierName();
    antlr4::tree::TerminalNode *StringLiteral();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ModuleExportNameContext* moduleExportName();

  class  ImportedBindingContext : public antlr4::ParserRuleContext {
  public:
    ImportedBindingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Identifier();
    antlr4::tree::TerminalNode *Yield();
    antlr4::tree::TerminalNode *Await();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportedBindingContext* importedBinding();

  class  ImportDefaultContext : public antlr4::ParserRuleContext {
  public:
    ImportDefaultContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AliasNameContext *aliasName();
    antlr4::tree::TerminalNode *Comma();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportDefaultContext* importDefault();

  class  ImportNamespaceContext : public antlr4::ParserRuleContext {
  public:
    ImportNamespaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Multiply();
    std::vector<IdentifierNameContext *> identifierName();
    IdentifierNameContext* identifierName(size_t i);
    antlr4::tree::TerminalNode *As();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportNamespaceContext* importNamespace();

  class  ImportFromContext : public antlr4::ParserRuleContext {
  public:
    ImportFromContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *From();
    antlr4::tree::TerminalNode *StringLiteral();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportFromContext* importFrom();

  class  AliasNameContext : public antlr4::ParserRuleContext {
  public:
    AliasNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierNameContext *> identifierName();
    IdentifierNameContext* identifierName(size_t i);
    antlr4::tree::TerminalNode *As();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AliasNameContext* aliasName();

  class  ExportStatementContext : public antlr4::ParserRuleContext {
  public:
    ExportStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ExportStatementContext() = default;
    void copyFrom(ExportStatementContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  ExportDefaultDeclarationContext : public ExportStatementContext {
  public:
    ExportDefaultDeclarationContext(ExportStatementContext *ctx);

    antlr4::tree::TerminalNode *Export();
    antlr4::tree::TerminalNode *Default();
    SingleExpressionContext *singleExpression();
    EosContext *eos();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExportDeclarationContext : public ExportStatementContext {
  public:
    ExportDeclarationContext(ExportStatementContext *ctx);

    antlr4::tree::TerminalNode *Export();
    EosContext *eos();
    ExportFromBlockContext *exportFromBlock();
    DeclarationContext *declaration();
    antlr4::tree::TerminalNode *Default();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ExportStatementContext* exportStatement();

  class  ExportFromBlockContext : public antlr4::ParserRuleContext {
  public:
    ExportFromBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ImportNamespaceContext *importNamespace();
    ImportFromContext *importFrom();
    EosContext *eos();
    ExportModuleItemsContext *exportModuleItems();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExportFromBlockContext* exportFromBlock();

  class  ExportModuleItemsContext : public antlr4::ParserRuleContext {
  public:
    ExportModuleItemsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OpenBrace();
    antlr4::tree::TerminalNode *CloseBrace();
    std::vector<ExportAliasNameContext *> exportAliasName();
    ExportAliasNameContext* exportAliasName(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExportModuleItemsContext* exportModuleItems();

  class  ExportAliasNameContext : public antlr4::ParserRuleContext {
  public:
    ExportAliasNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ModuleExportNameContext *> moduleExportName();
    ModuleExportNameContext* moduleExportName(size_t i);
    antlr4::tree::TerminalNode *As();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExportAliasNameContext* exportAliasName();

  class  DeclarationContext : public antlr4::ParserRuleContext {
  public:
    DeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VariableStatementContext *variableStatement();
    ClassDeclarationContext *classDeclaration();
    FunctionDeclarationContext *functionDeclaration();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DeclarationContext* declaration();

  class  VariableStatementContext : public antlr4::ParserRuleContext {
  public:
    VariableStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VariableDeclarationListContext *variableDeclarationList();
    EosContext *eos();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VariableStatementContext* variableStatement();

  class  VariableDeclarationListContext : public antlr4::ParserRuleContext {
  public:
    VariableDeclarationListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VarModifierContext *varModifier();
    std::vector<VariableDeclarationContext *> variableDeclaration();
    VariableDeclarationContext* variableDeclaration(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VariableDeclarationListContext* variableDeclarationList();

  class  VariableDeclarationContext : public antlr4::ParserRuleContext {
  public:
    VariableDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AssignableContext *assignable();
    antlr4::tree::TerminalNode *Assign();
    SingleExpressionContext *singleExpression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VariableDeclarationContext* variableDeclaration();

  class  EmptyStatement_Context : public antlr4::ParserRuleContext {
  public:
    EmptyStatement_Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SemiColon();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EmptyStatement_Context* emptyStatement_();

  class  ExpressionStatementContext : public antlr4::ParserRuleContext {
  public:
    ExpressionStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionSequenceContext *expressionSequence();
    EosContext *eos();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionStatementContext* expressionStatement();

  class  IfStatementContext : public antlr4::ParserRuleContext {
  public:
    IfStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *If();
    antlr4::tree::TerminalNode *OpenParen();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *CloseParen();
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);
    antlr4::tree::TerminalNode *Else();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IfStatementContext* ifStatement();

  class  IterationStatementContext : public antlr4::ParserRuleContext {
  public:
    IterationStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    IterationStatementContext() = default;
    void copyFrom(IterationStatementContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  DoStatementContext : public IterationStatementContext {
  public:
    DoStatementContext(IterationStatementContext *ctx);

    antlr4::tree::TerminalNode *Do();
    StatementContext *statement();
    antlr4::tree::TerminalNode *While();
    antlr4::tree::TerminalNode *OpenParen();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *CloseParen();
    EosContext *eos();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  WhileStatementContext : public IterationStatementContext {
  public:
    WhileStatementContext(IterationStatementContext *ctx);

    antlr4::tree::TerminalNode *While();
    antlr4::tree::TerminalNode *OpenParen();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *CloseParen();
    StatementContext *statement();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ForStatementContext : public IterationStatementContext {
  public:
    ForStatementContext(IterationStatementContext *ctx);

    antlr4::tree::TerminalNode *For();
    antlr4::tree::TerminalNode *OpenParen();
    std::vector<antlr4::tree::TerminalNode *> SemiColon();
    antlr4::tree::TerminalNode* SemiColon(size_t i);
    antlr4::tree::TerminalNode *CloseParen();
    StatementContext *statement();
    std::vector<ExpressionSequenceContext *> expressionSequence();
    ExpressionSequenceContext* expressionSequence(size_t i);
    VariableDeclarationListContext *variableDeclarationList();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ForInStatementContext : public IterationStatementContext {
  public:
    ForInStatementContext(IterationStatementContext *ctx);

    antlr4::tree::TerminalNode *For();
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *In();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *CloseParen();
    StatementContext *statement();
    SingleExpressionContext *singleExpression();
    VariableDeclarationListContext *variableDeclarationList();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ForOfStatementContext : public IterationStatementContext {
  public:
    ForOfStatementContext(IterationStatementContext *ctx);

    antlr4::tree::TerminalNode *For();
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *Of();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *CloseParen();
    StatementContext *statement();
    SingleExpressionContext *singleExpression();
    VariableDeclarationListContext *variableDeclarationList();
    antlr4::tree::TerminalNode *Await();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  IterationStatementContext* iterationStatement();

  class  VarModifierContext : public antlr4::ParserRuleContext {
  public:
    VarModifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Var();
    Let_Context *let_();
    antlr4::tree::TerminalNode *Const();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VarModifierContext* varModifier();

  class  ContinueStatementContext : public antlr4::ParserRuleContext {
  public:
    ContinueStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Continue();
    EosContext *eos();
    IdentifierContext *identifier();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ContinueStatementContext* continueStatement();

  class  BreakStatementContext : public antlr4::ParserRuleContext {
  public:
    BreakStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Break();
    EosContext *eos();
    IdentifierContext *identifier();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BreakStatementContext* breakStatement();

  class  ReturnStatementContext : public antlr4::ParserRuleContext {
  public:
    ReturnStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Return();
    EosContext *eos();
    ExpressionSequenceContext *expressionSequence();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ReturnStatementContext* returnStatement();

  class  YieldStatementContext : public antlr4::ParserRuleContext {
  public:
    YieldStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    EosContext *eos();
    antlr4::tree::TerminalNode *Yield();
    antlr4::tree::TerminalNode *YieldStar();
    ExpressionSequenceContext *expressionSequence();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  YieldStatementContext* yieldStatement();

  class  WithStatementContext : public antlr4::ParserRuleContext {
  public:
    WithStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *With();
    antlr4::tree::TerminalNode *OpenParen();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *CloseParen();
    StatementContext *statement();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  WithStatementContext* withStatement();

  class  SwitchStatementContext : public antlr4::ParserRuleContext {
  public:
    SwitchStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Switch();
    antlr4::tree::TerminalNode *OpenParen();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *CloseParen();
    CaseBlockContext *caseBlock();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SwitchStatementContext* switchStatement();

  class  CaseBlockContext : public antlr4::ParserRuleContext {
  public:
    CaseBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OpenBrace();
    antlr4::tree::TerminalNode *CloseBrace();
    std::vector<CaseClausesContext *> caseClauses();
    CaseClausesContext* caseClauses(size_t i);
    DefaultClauseContext *defaultClause();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CaseBlockContext* caseBlock();

  class  CaseClausesContext : public antlr4::ParserRuleContext {
  public:
    CaseClausesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<CaseClauseContext *> caseClause();
    CaseClauseContext* caseClause(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CaseClausesContext* caseClauses();

  class  CaseClauseContext : public antlr4::ParserRuleContext {
  public:
    CaseClauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Case();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *Colon();
    StatementListContext *statementList();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CaseClauseContext* caseClause();

  class  DefaultClauseContext : public antlr4::ParserRuleContext {
  public:
    DefaultClauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Default();
    antlr4::tree::TerminalNode *Colon();
    StatementListContext *statementList();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DefaultClauseContext* defaultClause();

  class  LabelledStatementContext : public antlr4::ParserRuleContext {
  public:
    LabelledStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *Colon();
    StatementContext *statement();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LabelledStatementContext* labelledStatement();

  class  ThrowStatementContext : public antlr4::ParserRuleContext {
  public:
    ThrowStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Throw();
    ExpressionSequenceContext *expressionSequence();
    EosContext *eos();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ThrowStatementContext* throwStatement();

  class  TryStatementContext : public antlr4::ParserRuleContext {
  public:
    TryStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Try();
    BlockContext *block();
    CatchProductionContext *catchProduction();
    FinallyProductionContext *finallyProduction();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TryStatementContext* tryStatement();

  class  CatchProductionContext : public antlr4::ParserRuleContext {
  public:
    CatchProductionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Catch();
    BlockContext *block();
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *CloseParen();
    AssignableContext *assignable();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CatchProductionContext* catchProduction();

  class  FinallyProductionContext : public antlr4::ParserRuleContext {
  public:
    FinallyProductionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Finally();
    BlockContext *block();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FinallyProductionContext* finallyProduction();

  class  DebuggerStatementContext : public antlr4::ParserRuleContext {
  public:
    DebuggerStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Debugger();
    EosContext *eos();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DebuggerStatementContext* debuggerStatement();

  class  FunctionDeclarationContext : public antlr4::ParserRuleContext {
  public:
    FunctionDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Function_();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *CloseParen();
    FunctionBodyContext *functionBody();
    antlr4::tree::TerminalNode *Async();
    antlr4::tree::TerminalNode *Multiply();
    FormalParameterListContext *formalParameterList();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FunctionDeclarationContext* functionDeclaration();

  class  ClassDeclarationContext : public antlr4::ParserRuleContext {
  public:
    ClassDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Class();
    IdentifierContext *identifier();
    ClassTailContext *classTail();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ClassDeclarationContext* classDeclaration();

  class  ClassTailContext : public antlr4::ParserRuleContext {
  public:
    ClassTailContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OpenBrace();
    antlr4::tree::TerminalNode *CloseBrace();
    antlr4::tree::TerminalNode *Extends();
    SingleExpressionContext *singleExpression();
    std::vector<ClassElementContext *> classElement();
    ClassElementContext* classElement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ClassTailContext* classTail();

  class  ClassElementContext : public antlr4::ParserRuleContext {
  public:
    ClassElementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MethodDefinitionContext *methodDefinition();
    antlr4::tree::TerminalNode *Static();
    IdentifierContext *identifier();
    FieldDefinitionContext *fieldDefinition();
    BlockContext *block();
    EmptyStatement_Context *emptyStatement_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ClassElementContext* classElement();

  class  MethodDefinitionContext : public antlr4::ParserRuleContext {
  public:
    MethodDefinitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ClassElementNameContext *classElementName();
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *CloseParen();
    FunctionBodyContext *functionBody();
    antlr4::tree::TerminalNode *Async();
    antlr4::tree::TerminalNode *Multiply();
    FormalParameterListContext *formalParameterList();
    GetterContext *getter();
    SetterContext *setter();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MethodDefinitionContext* methodDefinition();

  class  FieldDefinitionContext : public antlr4::ParserRuleContext {
  public:
    FieldDefinitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ClassElementNameContext *classElementName();
    InitializerContext *initializer();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FieldDefinitionContext* fieldDefinition();

  class  ClassElementNameContext : public antlr4::ParserRuleContext {
  public:
    ClassElementNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyNameContext *propertyName();
    PrivateIdentifierContext *privateIdentifier();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ClassElementNameContext* classElementName();

  class  PrivateIdentifierContext : public antlr4::ParserRuleContext {
  public:
    PrivateIdentifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Hashtag();
    IdentifierNameContext *identifierName();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PrivateIdentifierContext* privateIdentifier();

  class  FormalParameterListContext : public antlr4::ParserRuleContext {
  public:
    FormalParameterListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<FormalParameterArgContext *> formalParameterArg();
    FormalParameterArgContext* formalParameterArg(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);
    LastFormalParameterArgContext *lastFormalParameterArg();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FormalParameterListContext* formalParameterList();

  class  FormalParameterArgContext : public antlr4::ParserRuleContext {
  public:
    FormalParameterArgContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AssignableContext *assignable();
    antlr4::tree::TerminalNode *Assign();
    SingleExpressionContext *singleExpression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FormalParameterArgContext* formalParameterArg();

  class  LastFormalParameterArgContext : public antlr4::ParserRuleContext {
  public:
    LastFormalParameterArgContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Ellipsis();
    SingleExpressionContext *singleExpression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LastFormalParameterArgContext* lastFormalParameterArg();

  class  FunctionBodyContext : public antlr4::ParserRuleContext {
  public:
    FunctionBodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OpenBrace();
    antlr4::tree::TerminalNode *CloseBrace();
    SourceElementsContext *sourceElements();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FunctionBodyContext* functionBody();

  class  SourceElementsContext : public antlr4::ParserRuleContext {
  public:
    SourceElementsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SourceElementContext *> sourceElement();
    SourceElementContext* sourceElement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SourceElementsContext* sourceElements();

  class  ArrayLiteralContext : public antlr4::ParserRuleContext {
  public:
    ArrayLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OpenBracket();
    ElementListContext *elementList();
    antlr4::tree::TerminalNode *CloseBracket();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArrayLiteralContext* arrayLiteral();

  class  ElementListContext : public antlr4::ParserRuleContext {
  public:
    ElementListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);
    std::vector<ArrayElementContext *> arrayElement();
    ArrayElementContext* arrayElement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ElementListContext* elementList();

  class  ArrayElementContext : public antlr4::ParserRuleContext {
  public:
    ArrayElementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SingleExpressionContext *singleExpression();
    antlr4::tree::TerminalNode *Ellipsis();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArrayElementContext* arrayElement();

  class  PropertyAssignmentContext : public antlr4::ParserRuleContext {
  public:
    PropertyAssignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    PropertyAssignmentContext() = default;
    void copyFrom(PropertyAssignmentContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  PropertyExpressionAssignmentContext : public PropertyAssignmentContext {
  public:
    PropertyExpressionAssignmentContext(PropertyAssignmentContext *ctx);

    PropertyNameContext *propertyName();
    antlr4::tree::TerminalNode *Colon();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ComputedPropertyExpressionAssignmentContext : public PropertyAssignmentContext {
  public:
    ComputedPropertyExpressionAssignmentContext(PropertyAssignmentContext *ctx);

    antlr4::tree::TerminalNode *OpenBracket();
    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *CloseBracket();
    antlr4::tree::TerminalNode *Colon();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PropertyShorthandContext : public PropertyAssignmentContext {
  public:
    PropertyShorthandContext(PropertyAssignmentContext *ctx);

    SingleExpressionContext *singleExpression();
    antlr4::tree::TerminalNode *Ellipsis();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PropertySetterContext : public PropertyAssignmentContext {
  public:
    PropertySetterContext(PropertyAssignmentContext *ctx);

    SetterContext *setter();
    antlr4::tree::TerminalNode *OpenParen();
    FormalParameterArgContext *formalParameterArg();
    antlr4::tree::TerminalNode *CloseParen();
    FunctionBodyContext *functionBody();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PropertyGetterContext : public PropertyAssignmentContext {
  public:
    PropertyGetterContext(PropertyAssignmentContext *ctx);

    GetterContext *getter();
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *CloseParen();
    FunctionBodyContext *functionBody();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  FunctionPropertyContext : public PropertyAssignmentContext {
  public:
    FunctionPropertyContext(PropertyAssignmentContext *ctx);

    PropertyNameContext *propertyName();
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *CloseParen();
    FunctionBodyContext *functionBody();
    antlr4::tree::TerminalNode *Async();
    antlr4::tree::TerminalNode *Multiply();
    FormalParameterListContext *formalParameterList();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  PropertyAssignmentContext* propertyAssignment();

  class  PropertyNameContext : public antlr4::ParserRuleContext {
  public:
    PropertyNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierNameContext *identifierName();
    antlr4::tree::TerminalNode *StringLiteral();
    NumericLiteralContext *numericLiteral();
    antlr4::tree::TerminalNode *OpenBracket();
    SingleExpressionContext *singleExpression();
    antlr4::tree::TerminalNode *CloseBracket();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyNameContext* propertyName();

  class  ArgumentsContext : public antlr4::ParserRuleContext {
  public:
    ArgumentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *CloseParen();
    std::vector<ArgumentContext *> argument();
    ArgumentContext* argument(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArgumentsContext* arguments();

  class  ArgumentContext : public antlr4::ParserRuleContext {
  public:
    ArgumentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SingleExpressionContext *singleExpression();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *Ellipsis();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArgumentContext* argument();

  class  ExpressionSequenceContext : public antlr4::ParserRuleContext {
  public:
    ExpressionSequenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionSequenceContext* expressionSequence();

  class  SingleExpressionContext : public antlr4::ParserRuleContext {
  public:
    SingleExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    SingleExpressionContext() = default;
    void copyFrom(SingleExpressionContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  TemplateStringExpressionContext : public SingleExpressionContext {
  public:
    TemplateStringExpressionContext(SingleExpressionContext *ctx);

    SingleExpressionContext *singleExpression();
    TemplateStringLiteralContext *templateStringLiteral();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TernaryExpressionContext : public SingleExpressionContext {
  public:
    TernaryExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *QuestionMark();
    antlr4::tree::TerminalNode *Colon();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  LogicalAndExpressionContext : public SingleExpressionContext {
  public:
    LogicalAndExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *And();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PowerExpressionContext : public SingleExpressionContext {
  public:
    PowerExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *Power();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PreIncrementExpressionContext : public SingleExpressionContext {
  public:
    PreIncrementExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *PlusPlus();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ObjectLiteralExpressionContext : public SingleExpressionContext {
  public:
    ObjectLiteralExpressionContext(SingleExpressionContext *ctx);

    ObjectLiteralContext *objectLiteral();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  MetaExpressionContext : public SingleExpressionContext {
  public:
    MetaExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *New();
    antlr4::tree::TerminalNode *Dot();
    IdentifierContext *identifier();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  InExpressionContext : public SingleExpressionContext {
  public:
    InExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *In();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  LogicalOrExpressionContext : public SingleExpressionContext {
  public:
    LogicalOrExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *Or();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  OptionalChainExpressionContext : public SingleExpressionContext {
  public:
    OptionalChainExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *QuestionMarkDot();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  NotExpressionContext : public SingleExpressionContext {
  public:
    NotExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Not();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PreDecreaseExpressionContext : public SingleExpressionContext {
  public:
    PreDecreaseExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *MinusMinus();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ArgumentsExpressionContext : public SingleExpressionContext {
  public:
    ArgumentsExpressionContext(SingleExpressionContext *ctx);

    SingleExpressionContext *singleExpression();
    ArgumentsContext *arguments();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  AwaitExpressionContext : public SingleExpressionContext {
  public:
    AwaitExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Await();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ThisExpressionContext : public SingleExpressionContext {
  public:
    ThisExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *This();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  FunctionExpressionContext : public SingleExpressionContext {
  public:
    FunctionExpressionContext(SingleExpressionContext *ctx);

    AnonymousFunctionContext *anonymousFunction();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  UnaryMinusExpressionContext : public SingleExpressionContext {
  public:
    UnaryMinusExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Minus();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  AssignmentExpressionContext : public SingleExpressionContext {
  public:
    AssignmentExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *Assign();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PostDecreaseExpressionContext : public SingleExpressionContext {
  public:
    PostDecreaseExpressionContext(SingleExpressionContext *ctx);

    SingleExpressionContext *singleExpression();
    antlr4::tree::TerminalNode *MinusMinus();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TypeofExpressionContext : public SingleExpressionContext {
  public:
    TypeofExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Typeof();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  InstanceofExpressionContext : public SingleExpressionContext {
  public:
    InstanceofExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *Instanceof();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  UnaryPlusExpressionContext : public SingleExpressionContext {
  public:
    UnaryPlusExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Plus();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  DeleteExpressionContext : public SingleExpressionContext {
  public:
    DeleteExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Delete();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ImportExpressionContext : public SingleExpressionContext {
  public:
    ImportExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Import();
    antlr4::tree::TerminalNode *OpenParen();
    SingleExpressionContext *singleExpression();
    antlr4::tree::TerminalNode *CloseParen();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  EqualityExpressionContext : public SingleExpressionContext {
  public:
    EqualityExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *Equals_();
    antlr4::tree::TerminalNode *NotEquals();
    antlr4::tree::TerminalNode *IdentityEquals();
    antlr4::tree::TerminalNode *IdentityNotEquals();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  BitXOrExpressionContext : public SingleExpressionContext {
  public:
    BitXOrExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *BitXOr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  SuperExpressionContext : public SingleExpressionContext {
  public:
    SuperExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Super();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  MultiplicativeExpressionContext : public SingleExpressionContext {
  public:
    MultiplicativeExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *Multiply();
    antlr4::tree::TerminalNode *Divide();
    antlr4::tree::TerminalNode *Modulus();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  BitShiftExpressionContext : public SingleExpressionContext {
  public:
    BitShiftExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *LeftShiftArithmetic();
    antlr4::tree::TerminalNode *RightShiftArithmetic();
    antlr4::tree::TerminalNode *RightShiftLogical();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ParenthesizedExpressionContext : public SingleExpressionContext {
  public:
    ParenthesizedExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *OpenParen();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *CloseParen();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  AdditiveExpressionContext : public SingleExpressionContext {
  public:
    AdditiveExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *Plus();
    antlr4::tree::TerminalNode *Minus();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  RelationalExpressionContext : public SingleExpressionContext {
  public:
    RelationalExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *LessThan();
    antlr4::tree::TerminalNode *MoreThan();
    antlr4::tree::TerminalNode *LessThanEquals();
    antlr4::tree::TerminalNode *GreaterThanEquals();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PostIncrementExpressionContext : public SingleExpressionContext {
  public:
    PostIncrementExpressionContext(SingleExpressionContext *ctx);

    SingleExpressionContext *singleExpression();
    antlr4::tree::TerminalNode *PlusPlus();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  YieldExpressionContext : public SingleExpressionContext {
  public:
    YieldExpressionContext(SingleExpressionContext *ctx);

    YieldStatementContext *yieldStatement();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  BitNotExpressionContext : public SingleExpressionContext {
  public:
    BitNotExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *BitNot();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  NewExpressionContext : public SingleExpressionContext {
  public:
    NewExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *New();
    IdentifierContext *identifier();
    ArgumentsContext *arguments();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  LiteralExpressionContext : public SingleExpressionContext {
  public:
    LiteralExpressionContext(SingleExpressionContext *ctx);

    LiteralContext *literal();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ArrayLiteralExpressionContext : public SingleExpressionContext {
  public:
    ArrayLiteralExpressionContext(SingleExpressionContext *ctx);

    ArrayLiteralContext *arrayLiteral();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  MemberDotExpressionContext : public SingleExpressionContext {
  public:
    MemberDotExpressionContext(SingleExpressionContext *ctx);

    SingleExpressionContext *singleExpression();
    antlr4::tree::TerminalNode *Dot();
    IdentifierNameContext *identifierName();
    antlr4::tree::TerminalNode *QuestionMark();
    antlr4::tree::TerminalNode *Hashtag();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ClassExpressionContext : public SingleExpressionContext {
  public:
    ClassExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Class();
    ClassTailContext *classTail();
    IdentifierContext *identifier();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  MemberIndexExpressionContext : public SingleExpressionContext {
  public:
    MemberIndexExpressionContext(SingleExpressionContext *ctx);

    SingleExpressionContext *singleExpression();
    antlr4::tree::TerminalNode *OpenBracket();
    ExpressionSequenceContext *expressionSequence();
    antlr4::tree::TerminalNode *CloseBracket();
    antlr4::tree::TerminalNode *QuestionMarkDot();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  IdentifierExpressionContext : public SingleExpressionContext {
  public:
    IdentifierExpressionContext(SingleExpressionContext *ctx);

    IdentifierContext *identifier();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  BitAndExpressionContext : public SingleExpressionContext {
  public:
    BitAndExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *BitAnd();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  BitOrExpressionContext : public SingleExpressionContext {
  public:
    BitOrExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *BitOr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  AssignmentOperatorExpressionContext : public SingleExpressionContext {
  public:
    AssignmentOperatorExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    AssignmentOperatorContext *assignmentOperator();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  VoidExpressionContext : public SingleExpressionContext {
  public:
    VoidExpressionContext(SingleExpressionContext *ctx);

    antlr4::tree::TerminalNode *Void();
    SingleExpressionContext *singleExpression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  CoalesceExpressionContext : public SingleExpressionContext {
  public:
    CoalesceExpressionContext(SingleExpressionContext *ctx);

    std::vector<SingleExpressionContext *> singleExpression();
    SingleExpressionContext* singleExpression(size_t i);
    antlr4::tree::TerminalNode *NullCoalesce();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  SingleExpressionContext* singleExpression();
  SingleExpressionContext* singleExpression(int precedence);
  class  InitializerContext : public antlr4::ParserRuleContext {
  public:
    InitializerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Assign();
    SingleExpressionContext *singleExpression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InitializerContext* initializer();

  class  AssignableContext : public antlr4::ParserRuleContext {
  public:
    AssignableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    KeywordContext *keyword();
    ArrayLiteralContext *arrayLiteral();
    ObjectLiteralContext *objectLiteral();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AssignableContext* assignable();

  class  ObjectLiteralContext : public antlr4::ParserRuleContext {
  public:
    ObjectLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OpenBrace();
    antlr4::tree::TerminalNode *CloseBrace();
    std::vector<PropertyAssignmentContext *> propertyAssignment();
    PropertyAssignmentContext* propertyAssignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> Comma();
    antlr4::tree::TerminalNode* Comma(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ObjectLiteralContext* objectLiteral();

  class  AnonymousFunctionContext : public antlr4::ParserRuleContext {
  public:
    AnonymousFunctionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    AnonymousFunctionContext() = default;
    void copyFrom(AnonymousFunctionContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  AnonymousFunctionDeclContext : public AnonymousFunctionContext {
  public:
    AnonymousFunctionDeclContext(AnonymousFunctionContext *ctx);

    antlr4::tree::TerminalNode *Function_();
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *CloseParen();
    FunctionBodyContext *functionBody();
    antlr4::tree::TerminalNode *Async();
    antlr4::tree::TerminalNode *Multiply();
    FormalParameterListContext *formalParameterList();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ArrowFunctionContext : public AnonymousFunctionContext {
  public:
    ArrowFunctionContext(AnonymousFunctionContext *ctx);

    ArrowFunctionParametersContext *arrowFunctionParameters();
    antlr4::tree::TerminalNode *ARROW();
    ArrowFunctionBodyContext *arrowFunctionBody();
    antlr4::tree::TerminalNode *Async();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  NamedFunctionContext : public AnonymousFunctionContext {
  public:
    NamedFunctionContext(AnonymousFunctionContext *ctx);

    FunctionDeclarationContext *functionDeclaration();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  AnonymousFunctionContext* anonymousFunction();

  class  ArrowFunctionParametersContext : public antlr4::ParserRuleContext {
  public:
    ArrowFunctionParametersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyNameContext *propertyName();
    antlr4::tree::TerminalNode *OpenParen();
    antlr4::tree::TerminalNode *CloseParen();
    FormalParameterListContext *formalParameterList();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArrowFunctionParametersContext* arrowFunctionParameters();

  class  ArrowFunctionBodyContext : public antlr4::ParserRuleContext {
  public:
    ArrowFunctionBodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SingleExpressionContext *singleExpression();
    FunctionBodyContext *functionBody();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArrowFunctionBodyContext* arrowFunctionBody();

  class  AssignmentOperatorContext : public antlr4::ParserRuleContext {
  public:
    AssignmentOperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MultiplyAssign();
    antlr4::tree::TerminalNode *DivideAssign();
    antlr4::tree::TerminalNode *ModulusAssign();
    antlr4::tree::TerminalNode *PlusAssign();
    antlr4::tree::TerminalNode *MinusAssign();
    antlr4::tree::TerminalNode *LeftShiftArithmeticAssign();
    antlr4::tree::TerminalNode *RightShiftArithmeticAssign();
    antlr4::tree::TerminalNode *RightShiftLogicalAssign();
    antlr4::tree::TerminalNode *BitAndAssign();
    antlr4::tree::TerminalNode *BitXorAssign();
    antlr4::tree::TerminalNode *BitOrAssign();
    antlr4::tree::TerminalNode *PowerAssign();
    antlr4::tree::TerminalNode *NullishCoalescingAssign();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AssignmentOperatorContext* assignmentOperator();

  class  LiteralContext : public antlr4::ParserRuleContext {
  public:
    LiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NullLiteral();
    antlr4::tree::TerminalNode *BooleanLiteral();
    antlr4::tree::TerminalNode *StringLiteral();
    TemplateStringLiteralContext *templateStringLiteral();
    antlr4::tree::TerminalNode *RegularExpressionLiteral();
    NumericLiteralContext *numericLiteral();
    BigintLiteralContext *bigintLiteral();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LiteralContext* literal();

  class  TemplateStringLiteralContext : public antlr4::ParserRuleContext {
  public:
    TemplateStringLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> BackTick();
    antlr4::tree::TerminalNode* BackTick(size_t i);
    std::vector<TemplateStringAtomContext *> templateStringAtom();
    TemplateStringAtomContext* templateStringAtom(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TemplateStringLiteralContext* templateStringLiteral();

  class  TemplateStringAtomContext : public antlr4::ParserRuleContext {
  public:
    TemplateStringAtomContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TemplateStringAtom();
    antlr4::tree::TerminalNode *TemplateStringStartExpression();
    SingleExpressionContext *singleExpression();
    antlr4::tree::TerminalNode *TemplateCloseBrace();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TemplateStringAtomContext* templateStringAtom();

  class  NumericLiteralContext : public antlr4::ParserRuleContext {
  public:
    NumericLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DecimalLiteral();
    antlr4::tree::TerminalNode *HexIntegerLiteral();
    antlr4::tree::TerminalNode *OctalIntegerLiteral();
    antlr4::tree::TerminalNode *OctalIntegerLiteral2();
    antlr4::tree::TerminalNode *BinaryIntegerLiteral();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NumericLiteralContext* numericLiteral();

  class  BigintLiteralContext : public antlr4::ParserRuleContext {
  public:
    BigintLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BigDecimalIntegerLiteral();
    antlr4::tree::TerminalNode *BigHexIntegerLiteral();
    antlr4::tree::TerminalNode *BigOctalIntegerLiteral();
    antlr4::tree::TerminalNode *BigBinaryIntegerLiteral();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BigintLiteralContext* bigintLiteral();

  class  GetterContext : public antlr4::ParserRuleContext {
  public:
    GetterContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    ClassElementNameContext *classElementName();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GetterContext* getter();

  class  SetterContext : public antlr4::ParserRuleContext {
  public:
    SetterContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    ClassElementNameContext *classElementName();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SetterContext* setter();

  class  IdentifierNameContext : public antlr4::ParserRuleContext {
  public:
    IdentifierNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    ReservedWordContext *reservedWord();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IdentifierNameContext* identifierName();

  class  IdentifierContext : public antlr4::ParserRuleContext {
  public:
    IdentifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Identifier();
    antlr4::tree::TerminalNode *NonStrictLet();
    antlr4::tree::TerminalNode *Async();
    antlr4::tree::TerminalNode *As();
    antlr4::tree::TerminalNode *From();
    antlr4::tree::TerminalNode *Yield();
    antlr4::tree::TerminalNode *Of();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IdentifierContext* identifier();

  class  ReservedWordContext : public antlr4::ParserRuleContext {
  public:
    ReservedWordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    KeywordContext *keyword();
    antlr4::tree::TerminalNode *NullLiteral();
    antlr4::tree::TerminalNode *BooleanLiteral();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ReservedWordContext* reservedWord();

  class  KeywordContext : public antlr4::ParserRuleContext {
  public:
    KeywordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Break();
    antlr4::tree::TerminalNode *Do();
    antlr4::tree::TerminalNode *Instanceof();
    antlr4::tree::TerminalNode *Typeof();
    antlr4::tree::TerminalNode *Case();
    antlr4::tree::TerminalNode *Else();
    antlr4::tree::TerminalNode *New();
    antlr4::tree::TerminalNode *Var();
    antlr4::tree::TerminalNode *Catch();
    antlr4::tree::TerminalNode *Finally();
    antlr4::tree::TerminalNode *Return();
    antlr4::tree::TerminalNode *Void();
    antlr4::tree::TerminalNode *Continue();
    antlr4::tree::TerminalNode *For();
    antlr4::tree::TerminalNode *Switch();
    antlr4::tree::TerminalNode *While();
    antlr4::tree::TerminalNode *Debugger();
    antlr4::tree::TerminalNode *Function_();
    antlr4::tree::TerminalNode *This();
    antlr4::tree::TerminalNode *With();
    antlr4::tree::TerminalNode *Default();
    antlr4::tree::TerminalNode *If();
    antlr4::tree::TerminalNode *Throw();
    antlr4::tree::TerminalNode *Delete();
    antlr4::tree::TerminalNode *In();
    antlr4::tree::TerminalNode *Try();
    antlr4::tree::TerminalNode *Class();
    antlr4::tree::TerminalNode *Enum();
    antlr4::tree::TerminalNode *Extends();
    antlr4::tree::TerminalNode *Super();
    antlr4::tree::TerminalNode *Const();
    antlr4::tree::TerminalNode *Export();
    antlr4::tree::TerminalNode *Import();
    antlr4::tree::TerminalNode *Implements();
    Let_Context *let_();
    antlr4::tree::TerminalNode *Private();
    antlr4::tree::TerminalNode *Public();
    antlr4::tree::TerminalNode *Interface();
    antlr4::tree::TerminalNode *Package();
    antlr4::tree::TerminalNode *Protected();
    antlr4::tree::TerminalNode *Static();
    antlr4::tree::TerminalNode *Yield();
    antlr4::tree::TerminalNode *YieldStar();
    antlr4::tree::TerminalNode *Async();
    antlr4::tree::TerminalNode *Await();
    antlr4::tree::TerminalNode *From();
    antlr4::tree::TerminalNode *As();
    antlr4::tree::TerminalNode *Of();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  KeywordContext* keyword();

  class  Let_Context : public antlr4::ParserRuleContext {
  public:
    Let_Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NonStrictLet();
    antlr4::tree::TerminalNode *StrictLet();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Let_Context* let_();

  class  EosContext : public antlr4::ParserRuleContext {
  public:
    EosContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SemiColon();
    antlr4::tree::TerminalNode *EOF();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EosContext* eos();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool expressionStatementSempred(ExpressionStatementContext *_localctx, size_t predicateIndex);
  bool continueStatementSempred(ContinueStatementContext *_localctx, size_t predicateIndex);
  bool breakStatementSempred(BreakStatementContext *_localctx, size_t predicateIndex);
  bool returnStatementSempred(ReturnStatementContext *_localctx, size_t predicateIndex);
  bool yieldStatementSempred(YieldStatementContext *_localctx, size_t predicateIndex);
  bool throwStatementSempred(ThrowStatementContext *_localctx, size_t predicateIndex);
  bool classElementSempred(ClassElementContext *_localctx, size_t predicateIndex);
  bool methodDefinitionSempred(MethodDefinitionContext *_localctx, size_t predicateIndex);
  bool singleExpressionSempred(SingleExpressionContext *_localctx, size_t predicateIndex);
  bool getterSempred(GetterContext *_localctx, size_t predicateIndex);
  bool setterSempred(SetterContext *_localctx, size_t predicateIndex);
  bool eosSempred(EosContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

