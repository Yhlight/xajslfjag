
// Generated from Grammars/JavaScript/JavaScriptParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaScriptParserVisitor.h"


/**
 * This class provides an empty implementation of JavaScriptParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  JavaScriptParserBaseVisitor : public JavaScriptParserVisitor {
public:

  virtual std::any visitProgram(JavaScriptParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSourceElement(JavaScriptParser::SourceElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(JavaScriptParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlock(JavaScriptParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatementList(JavaScriptParser::StatementListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportStatement(JavaScriptParser::ImportStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportFromBlock(JavaScriptParser::ImportFromBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportModuleItems(JavaScriptParser::ImportModuleItemsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportAliasName(JavaScriptParser::ImportAliasNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModuleExportName(JavaScriptParser::ModuleExportNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportedBinding(JavaScriptParser::ImportedBindingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportDefault(JavaScriptParser::ImportDefaultContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportNamespace(JavaScriptParser::ImportNamespaceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportFrom(JavaScriptParser::ImportFromContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAliasName(JavaScriptParser::AliasNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportDeclaration(JavaScriptParser::ExportDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportDefaultDeclaration(JavaScriptParser::ExportDefaultDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportFromBlock(JavaScriptParser::ExportFromBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportModuleItems(JavaScriptParser::ExportModuleItemsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportAliasName(JavaScriptParser::ExportAliasNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration(JavaScriptParser::DeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableStatement(JavaScriptParser::VariableStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclarationList(JavaScriptParser::VariableDeclarationListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclaration(JavaScriptParser::VariableDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEmptyStatement_(JavaScriptParser::EmptyStatement_Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpressionStatement(JavaScriptParser::ExpressionStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfStatement(JavaScriptParser::IfStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDoStatement(JavaScriptParser::DoStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhileStatement(JavaScriptParser::WhileStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForStatement(JavaScriptParser::ForStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForInStatement(JavaScriptParser::ForInStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForOfStatement(JavaScriptParser::ForOfStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVarModifier(JavaScriptParser::VarModifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitContinueStatement(JavaScriptParser::ContinueStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBreakStatement(JavaScriptParser::BreakStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReturnStatement(JavaScriptParser::ReturnStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitYieldStatement(JavaScriptParser::YieldStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWithStatement(JavaScriptParser::WithStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSwitchStatement(JavaScriptParser::SwitchStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCaseBlock(JavaScriptParser::CaseBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCaseClauses(JavaScriptParser::CaseClausesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCaseClause(JavaScriptParser::CaseClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDefaultClause(JavaScriptParser::DefaultClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLabelledStatement(JavaScriptParser::LabelledStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitThrowStatement(JavaScriptParser::ThrowStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTryStatement(JavaScriptParser::TryStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchProduction(JavaScriptParser::CatchProductionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFinallyProduction(JavaScriptParser::FinallyProductionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDebuggerStatement(JavaScriptParser::DebuggerStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassDeclaration(JavaScriptParser::ClassDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassTail(JavaScriptParser::ClassTailContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassElement(JavaScriptParser::ClassElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMethodDefinition(JavaScriptParser::MethodDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFieldDefinition(JavaScriptParser::FieldDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassElementName(JavaScriptParser::ClassElementNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrivateIdentifier(JavaScriptParser::PrivateIdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFormalParameterList(JavaScriptParser::FormalParameterListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFormalParameterArg(JavaScriptParser::FormalParameterArgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLastFormalParameterArg(JavaScriptParser::LastFormalParameterArgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionBody(JavaScriptParser::FunctionBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSourceElements(JavaScriptParser::SourceElementsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayLiteral(JavaScriptParser::ArrayLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementList(JavaScriptParser::ElementListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayElement(JavaScriptParser::ArrayElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyExpressionAssignment(JavaScriptParser::PropertyExpressionAssignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComputedPropertyExpressionAssignment(JavaScriptParser::ComputedPropertyExpressionAssignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionProperty(JavaScriptParser::FunctionPropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyGetter(JavaScriptParser::PropertyGetterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertySetter(JavaScriptParser::PropertySetterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyShorthand(JavaScriptParser::PropertyShorthandContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyName(JavaScriptParser::PropertyNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArguments(JavaScriptParser::ArgumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgument(JavaScriptParser::ArgumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpressionSequence(JavaScriptParser::ExpressionSequenceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateStringExpression(JavaScriptParser::TemplateStringExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTernaryExpression(JavaScriptParser::TernaryExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLogicalAndExpression(JavaScriptParser::LogicalAndExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPowerExpression(JavaScriptParser::PowerExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPreIncrementExpression(JavaScriptParser::PreIncrementExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitObjectLiteralExpression(JavaScriptParser::ObjectLiteralExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMetaExpression(JavaScriptParser::MetaExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInExpression(JavaScriptParser::InExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLogicalOrExpression(JavaScriptParser::LogicalOrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOptionalChainExpression(JavaScriptParser::OptionalChainExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNotExpression(JavaScriptParser::NotExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPreDecreaseExpression(JavaScriptParser::PreDecreaseExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgumentsExpression(JavaScriptParser::ArgumentsExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAwaitExpression(JavaScriptParser::AwaitExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitThisExpression(JavaScriptParser::ThisExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionExpression(JavaScriptParser::FunctionExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryMinusExpression(JavaScriptParser::UnaryMinusExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentExpression(JavaScriptParser::AssignmentExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPostDecreaseExpression(JavaScriptParser::PostDecreaseExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeofExpression(JavaScriptParser::TypeofExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInstanceofExpression(JavaScriptParser::InstanceofExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryPlusExpression(JavaScriptParser::UnaryPlusExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeleteExpression(JavaScriptParser::DeleteExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportExpression(JavaScriptParser::ImportExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEqualityExpression(JavaScriptParser::EqualityExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitXOrExpression(JavaScriptParser::BitXOrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSuperExpression(JavaScriptParser::SuperExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMultiplicativeExpression(JavaScriptParser::MultiplicativeExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitShiftExpression(JavaScriptParser::BitShiftExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParenthesizedExpression(JavaScriptParser::ParenthesizedExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAdditiveExpression(JavaScriptParser::AdditiveExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRelationalExpression(JavaScriptParser::RelationalExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPostIncrementExpression(JavaScriptParser::PostIncrementExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitYieldExpression(JavaScriptParser::YieldExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitNotExpression(JavaScriptParser::BitNotExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNewExpression(JavaScriptParser::NewExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLiteralExpression(JavaScriptParser::LiteralExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayLiteralExpression(JavaScriptParser::ArrayLiteralExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemberDotExpression(JavaScriptParser::MemberDotExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassExpression(JavaScriptParser::ClassExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemberIndexExpression(JavaScriptParser::MemberIndexExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifierExpression(JavaScriptParser::IdentifierExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitAndExpression(JavaScriptParser::BitAndExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitOrExpression(JavaScriptParser::BitOrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentOperatorExpression(JavaScriptParser::AssignmentOperatorExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVoidExpression(JavaScriptParser::VoidExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCoalesceExpression(JavaScriptParser::CoalesceExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInitializer(JavaScriptParser::InitializerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignable(JavaScriptParser::AssignableContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitObjectLiteral(JavaScriptParser::ObjectLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamedFunction(JavaScriptParser::NamedFunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnonymousFunctionDecl(JavaScriptParser::AnonymousFunctionDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrowFunction(JavaScriptParser::ArrowFunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrowFunctionParameters(JavaScriptParser::ArrowFunctionParametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrowFunctionBody(JavaScriptParser::ArrowFunctionBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentOperator(JavaScriptParser::AssignmentOperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLiteral(JavaScriptParser::LiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateStringLiteral(JavaScriptParser::TemplateStringLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateStringAtom(JavaScriptParser::TemplateStringAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumericLiteral(JavaScriptParser::NumericLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBigintLiteral(JavaScriptParser::BigintLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGetter(JavaScriptParser::GetterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSetter(JavaScriptParser::SetterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifierName(JavaScriptParser::IdentifierNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier(JavaScriptParser::IdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReservedWord(JavaScriptParser::ReservedWordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitKeyword(JavaScriptParser::KeywordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLet_(JavaScriptParser::Let_Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEos(JavaScriptParser::EosContext *ctx) override {
    return visitChildren(ctx);
  }


};

