
// Generated from Grammars/JavaScript/JavaScriptParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "JavaScriptParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by JavaScriptParser.
 */
class  JavaScriptParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by JavaScriptParser.
   */
    virtual std::any visitProgram(JavaScriptParser::ProgramContext *context) = 0;

    virtual std::any visitSourceElement(JavaScriptParser::SourceElementContext *context) = 0;

    virtual std::any visitStatement(JavaScriptParser::StatementContext *context) = 0;

    virtual std::any visitBlock(JavaScriptParser::BlockContext *context) = 0;

    virtual std::any visitStatementList(JavaScriptParser::StatementListContext *context) = 0;

    virtual std::any visitImportStatement(JavaScriptParser::ImportStatementContext *context) = 0;

    virtual std::any visitImportFromBlock(JavaScriptParser::ImportFromBlockContext *context) = 0;

    virtual std::any visitImportModuleItems(JavaScriptParser::ImportModuleItemsContext *context) = 0;

    virtual std::any visitImportAliasName(JavaScriptParser::ImportAliasNameContext *context) = 0;

    virtual std::any visitModuleExportName(JavaScriptParser::ModuleExportNameContext *context) = 0;

    virtual std::any visitImportedBinding(JavaScriptParser::ImportedBindingContext *context) = 0;

    virtual std::any visitImportDefault(JavaScriptParser::ImportDefaultContext *context) = 0;

    virtual std::any visitImportNamespace(JavaScriptParser::ImportNamespaceContext *context) = 0;

    virtual std::any visitImportFrom(JavaScriptParser::ImportFromContext *context) = 0;

    virtual std::any visitAliasName(JavaScriptParser::AliasNameContext *context) = 0;

    virtual std::any visitExportDeclaration(JavaScriptParser::ExportDeclarationContext *context) = 0;

    virtual std::any visitExportDefaultDeclaration(JavaScriptParser::ExportDefaultDeclarationContext *context) = 0;

    virtual std::any visitExportFromBlock(JavaScriptParser::ExportFromBlockContext *context) = 0;

    virtual std::any visitExportModuleItems(JavaScriptParser::ExportModuleItemsContext *context) = 0;

    virtual std::any visitExportAliasName(JavaScriptParser::ExportAliasNameContext *context) = 0;

    virtual std::any visitDeclaration(JavaScriptParser::DeclarationContext *context) = 0;

    virtual std::any visitVariableStatement(JavaScriptParser::VariableStatementContext *context) = 0;

    virtual std::any visitVariableDeclarationList(JavaScriptParser::VariableDeclarationListContext *context) = 0;

    virtual std::any visitVariableDeclaration(JavaScriptParser::VariableDeclarationContext *context) = 0;

    virtual std::any visitEmptyStatement_(JavaScriptParser::EmptyStatement_Context *context) = 0;

    virtual std::any visitExpressionStatement(JavaScriptParser::ExpressionStatementContext *context) = 0;

    virtual std::any visitIfStatement(JavaScriptParser::IfStatementContext *context) = 0;

    virtual std::any visitDoStatement(JavaScriptParser::DoStatementContext *context) = 0;

    virtual std::any visitWhileStatement(JavaScriptParser::WhileStatementContext *context) = 0;

    virtual std::any visitForStatement(JavaScriptParser::ForStatementContext *context) = 0;

    virtual std::any visitForInStatement(JavaScriptParser::ForInStatementContext *context) = 0;

    virtual std::any visitForOfStatement(JavaScriptParser::ForOfStatementContext *context) = 0;

    virtual std::any visitVarModifier(JavaScriptParser::VarModifierContext *context) = 0;

    virtual std::any visitContinueStatement(JavaScriptParser::ContinueStatementContext *context) = 0;

    virtual std::any visitBreakStatement(JavaScriptParser::BreakStatementContext *context) = 0;

    virtual std::any visitReturnStatement(JavaScriptParser::ReturnStatementContext *context) = 0;

    virtual std::any visitYieldStatement(JavaScriptParser::YieldStatementContext *context) = 0;

    virtual std::any visitWithStatement(JavaScriptParser::WithStatementContext *context) = 0;

    virtual std::any visitSwitchStatement(JavaScriptParser::SwitchStatementContext *context) = 0;

    virtual std::any visitCaseBlock(JavaScriptParser::CaseBlockContext *context) = 0;

    virtual std::any visitCaseClauses(JavaScriptParser::CaseClausesContext *context) = 0;

    virtual std::any visitCaseClause(JavaScriptParser::CaseClauseContext *context) = 0;

    virtual std::any visitDefaultClause(JavaScriptParser::DefaultClauseContext *context) = 0;

    virtual std::any visitLabelledStatement(JavaScriptParser::LabelledStatementContext *context) = 0;

    virtual std::any visitThrowStatement(JavaScriptParser::ThrowStatementContext *context) = 0;

    virtual std::any visitTryStatement(JavaScriptParser::TryStatementContext *context) = 0;

    virtual std::any visitCatchProduction(JavaScriptParser::CatchProductionContext *context) = 0;

    virtual std::any visitFinallyProduction(JavaScriptParser::FinallyProductionContext *context) = 0;

    virtual std::any visitDebuggerStatement(JavaScriptParser::DebuggerStatementContext *context) = 0;

    virtual std::any visitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *context) = 0;

    virtual std::any visitClassDeclaration(JavaScriptParser::ClassDeclarationContext *context) = 0;

    virtual std::any visitClassTail(JavaScriptParser::ClassTailContext *context) = 0;

    virtual std::any visitClassElement(JavaScriptParser::ClassElementContext *context) = 0;

    virtual std::any visitMethodDefinition(JavaScriptParser::MethodDefinitionContext *context) = 0;

    virtual std::any visitFieldDefinition(JavaScriptParser::FieldDefinitionContext *context) = 0;

    virtual std::any visitClassElementName(JavaScriptParser::ClassElementNameContext *context) = 0;

    virtual std::any visitPrivateIdentifier(JavaScriptParser::PrivateIdentifierContext *context) = 0;

    virtual std::any visitFormalParameterList(JavaScriptParser::FormalParameterListContext *context) = 0;

    virtual std::any visitFormalParameterArg(JavaScriptParser::FormalParameterArgContext *context) = 0;

    virtual std::any visitLastFormalParameterArg(JavaScriptParser::LastFormalParameterArgContext *context) = 0;

    virtual std::any visitFunctionBody(JavaScriptParser::FunctionBodyContext *context) = 0;

    virtual std::any visitSourceElements(JavaScriptParser::SourceElementsContext *context) = 0;

    virtual std::any visitArrayLiteral(JavaScriptParser::ArrayLiteralContext *context) = 0;

    virtual std::any visitElementList(JavaScriptParser::ElementListContext *context) = 0;

    virtual std::any visitArrayElement(JavaScriptParser::ArrayElementContext *context) = 0;

    virtual std::any visitPropertyExpressionAssignment(JavaScriptParser::PropertyExpressionAssignmentContext *context) = 0;

    virtual std::any visitComputedPropertyExpressionAssignment(JavaScriptParser::ComputedPropertyExpressionAssignmentContext *context) = 0;

    virtual std::any visitFunctionProperty(JavaScriptParser::FunctionPropertyContext *context) = 0;

    virtual std::any visitPropertyGetter(JavaScriptParser::PropertyGetterContext *context) = 0;

    virtual std::any visitPropertySetter(JavaScriptParser::PropertySetterContext *context) = 0;

    virtual std::any visitPropertyShorthand(JavaScriptParser::PropertyShorthandContext *context) = 0;

    virtual std::any visitPropertyName(JavaScriptParser::PropertyNameContext *context) = 0;

    virtual std::any visitArguments(JavaScriptParser::ArgumentsContext *context) = 0;

    virtual std::any visitArgument(JavaScriptParser::ArgumentContext *context) = 0;

    virtual std::any visitExpressionSequence(JavaScriptParser::ExpressionSequenceContext *context) = 0;

    virtual std::any visitTemplateStringExpression(JavaScriptParser::TemplateStringExpressionContext *context) = 0;

    virtual std::any visitTernaryExpression(JavaScriptParser::TernaryExpressionContext *context) = 0;

    virtual std::any visitLogicalAndExpression(JavaScriptParser::LogicalAndExpressionContext *context) = 0;

    virtual std::any visitPowerExpression(JavaScriptParser::PowerExpressionContext *context) = 0;

    virtual std::any visitPreIncrementExpression(JavaScriptParser::PreIncrementExpressionContext *context) = 0;

    virtual std::any visitObjectLiteralExpression(JavaScriptParser::ObjectLiteralExpressionContext *context) = 0;

    virtual std::any visitMetaExpression(JavaScriptParser::MetaExpressionContext *context) = 0;

    virtual std::any visitInExpression(JavaScriptParser::InExpressionContext *context) = 0;

    virtual std::any visitLogicalOrExpression(JavaScriptParser::LogicalOrExpressionContext *context) = 0;

    virtual std::any visitOptionalChainExpression(JavaScriptParser::OptionalChainExpressionContext *context) = 0;

    virtual std::any visitNotExpression(JavaScriptParser::NotExpressionContext *context) = 0;

    virtual std::any visitPreDecreaseExpression(JavaScriptParser::PreDecreaseExpressionContext *context) = 0;

    virtual std::any visitArgumentsExpression(JavaScriptParser::ArgumentsExpressionContext *context) = 0;

    virtual std::any visitAwaitExpression(JavaScriptParser::AwaitExpressionContext *context) = 0;

    virtual std::any visitThisExpression(JavaScriptParser::ThisExpressionContext *context) = 0;

    virtual std::any visitFunctionExpression(JavaScriptParser::FunctionExpressionContext *context) = 0;

    virtual std::any visitUnaryMinusExpression(JavaScriptParser::UnaryMinusExpressionContext *context) = 0;

    virtual std::any visitAssignmentExpression(JavaScriptParser::AssignmentExpressionContext *context) = 0;

    virtual std::any visitPostDecreaseExpression(JavaScriptParser::PostDecreaseExpressionContext *context) = 0;

    virtual std::any visitTypeofExpression(JavaScriptParser::TypeofExpressionContext *context) = 0;

    virtual std::any visitInstanceofExpression(JavaScriptParser::InstanceofExpressionContext *context) = 0;

    virtual std::any visitUnaryPlusExpression(JavaScriptParser::UnaryPlusExpressionContext *context) = 0;

    virtual std::any visitDeleteExpression(JavaScriptParser::DeleteExpressionContext *context) = 0;

    virtual std::any visitImportExpression(JavaScriptParser::ImportExpressionContext *context) = 0;

    virtual std::any visitEqualityExpression(JavaScriptParser::EqualityExpressionContext *context) = 0;

    virtual std::any visitBitXOrExpression(JavaScriptParser::BitXOrExpressionContext *context) = 0;

    virtual std::any visitSuperExpression(JavaScriptParser::SuperExpressionContext *context) = 0;

    virtual std::any visitMultiplicativeExpression(JavaScriptParser::MultiplicativeExpressionContext *context) = 0;

    virtual std::any visitBitShiftExpression(JavaScriptParser::BitShiftExpressionContext *context) = 0;

    virtual std::any visitParenthesizedExpression(JavaScriptParser::ParenthesizedExpressionContext *context) = 0;

    virtual std::any visitAdditiveExpression(JavaScriptParser::AdditiveExpressionContext *context) = 0;

    virtual std::any visitRelationalExpression(JavaScriptParser::RelationalExpressionContext *context) = 0;

    virtual std::any visitPostIncrementExpression(JavaScriptParser::PostIncrementExpressionContext *context) = 0;

    virtual std::any visitYieldExpression(JavaScriptParser::YieldExpressionContext *context) = 0;

    virtual std::any visitBitNotExpression(JavaScriptParser::BitNotExpressionContext *context) = 0;

    virtual std::any visitNewExpression(JavaScriptParser::NewExpressionContext *context) = 0;

    virtual std::any visitLiteralExpression(JavaScriptParser::LiteralExpressionContext *context) = 0;

    virtual std::any visitArrayLiteralExpression(JavaScriptParser::ArrayLiteralExpressionContext *context) = 0;

    virtual std::any visitMemberDotExpression(JavaScriptParser::MemberDotExpressionContext *context) = 0;

    virtual std::any visitClassExpression(JavaScriptParser::ClassExpressionContext *context) = 0;

    virtual std::any visitMemberIndexExpression(JavaScriptParser::MemberIndexExpressionContext *context) = 0;

    virtual std::any visitIdentifierExpression(JavaScriptParser::IdentifierExpressionContext *context) = 0;

    virtual std::any visitBitAndExpression(JavaScriptParser::BitAndExpressionContext *context) = 0;

    virtual std::any visitBitOrExpression(JavaScriptParser::BitOrExpressionContext *context) = 0;

    virtual std::any visitAssignmentOperatorExpression(JavaScriptParser::AssignmentOperatorExpressionContext *context) = 0;

    virtual std::any visitVoidExpression(JavaScriptParser::VoidExpressionContext *context) = 0;

    virtual std::any visitCoalesceExpression(JavaScriptParser::CoalesceExpressionContext *context) = 0;

    virtual std::any visitInitializer(JavaScriptParser::InitializerContext *context) = 0;

    virtual std::any visitAssignable(JavaScriptParser::AssignableContext *context) = 0;

    virtual std::any visitObjectLiteral(JavaScriptParser::ObjectLiteralContext *context) = 0;

    virtual std::any visitNamedFunction(JavaScriptParser::NamedFunctionContext *context) = 0;

    virtual std::any visitAnonymousFunctionDecl(JavaScriptParser::AnonymousFunctionDeclContext *context) = 0;

    virtual std::any visitArrowFunction(JavaScriptParser::ArrowFunctionContext *context) = 0;

    virtual std::any visitArrowFunctionParameters(JavaScriptParser::ArrowFunctionParametersContext *context) = 0;

    virtual std::any visitArrowFunctionBody(JavaScriptParser::ArrowFunctionBodyContext *context) = 0;

    virtual std::any visitAssignmentOperator(JavaScriptParser::AssignmentOperatorContext *context) = 0;

    virtual std::any visitLiteral(JavaScriptParser::LiteralContext *context) = 0;

    virtual std::any visitTemplateStringLiteral(JavaScriptParser::TemplateStringLiteralContext *context) = 0;

    virtual std::any visitTemplateStringAtom(JavaScriptParser::TemplateStringAtomContext *context) = 0;

    virtual std::any visitNumericLiteral(JavaScriptParser::NumericLiteralContext *context) = 0;

    virtual std::any visitBigintLiteral(JavaScriptParser::BigintLiteralContext *context) = 0;

    virtual std::any visitGetter(JavaScriptParser::GetterContext *context) = 0;

    virtual std::any visitSetter(JavaScriptParser::SetterContext *context) = 0;

    virtual std::any visitIdentifierName(JavaScriptParser::IdentifierNameContext *context) = 0;

    virtual std::any visitIdentifier(JavaScriptParser::IdentifierContext *context) = 0;

    virtual std::any visitReservedWord(JavaScriptParser::ReservedWordContext *context) = 0;

    virtual std::any visitKeyword(JavaScriptParser::KeywordContext *context) = 0;

    virtual std::any visitLet_(JavaScriptParser::Let_Context *context) = 0;

    virtual std::any visitEos(JavaScriptParser::EosContext *context) = 0;


};

