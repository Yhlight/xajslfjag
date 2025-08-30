
// Generated from SimpleJSParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SimpleJSParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by SimpleJSParser.
 */
class  SimpleJSParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by SimpleJSParser.
   */
    virtual std::any visitProgram(SimpleJSParser::ProgramContext *context) = 0;

    virtual std::any visitStatement(SimpleJSParser::StatementContext *context) = 0;

    virtual std::any visitBlock(SimpleJSParser::BlockContext *context) = 0;

    virtual std::any visitVarDeclaration(SimpleJSParser::VarDeclarationContext *context) = 0;

    virtual std::any visitVarDeclarator(SimpleJSParser::VarDeclaratorContext *context) = 0;

    virtual std::any visitFunctionDeclaration(SimpleJSParser::FunctionDeclarationContext *context) = 0;

    virtual std::any visitClassDeclaration(SimpleJSParser::ClassDeclarationContext *context) = 0;

    virtual std::any visitClassElement(SimpleJSParser::ClassElementContext *context) = 0;

    virtual std::any visitMethodDefinition(SimpleJSParser::MethodDefinitionContext *context) = 0;

    virtual std::any visitPropertyDefinition(SimpleJSParser::PropertyDefinitionContext *context) = 0;

    virtual std::any visitParameterList(SimpleJSParser::ParameterListContext *context) = 0;

    virtual std::any visitExpressionStatement(SimpleJSParser::ExpressionStatementContext *context) = 0;

    virtual std::any visitIfStatement(SimpleJSParser::IfStatementContext *context) = 0;

    virtual std::any visitWhileStatement(SimpleJSParser::WhileStatementContext *context) = 0;

    virtual std::any visitForStatement(SimpleJSParser::ForStatementContext *context) = 0;

    virtual std::any visitReturnStatement(SimpleJSParser::ReturnStatementContext *context) = 0;

    virtual std::any visitThrowStatement(SimpleJSParser::ThrowStatementContext *context) = 0;

    virtual std::any visitTryStatement(SimpleJSParser::TryStatementContext *context) = 0;

    virtual std::any visitSwitchStatement(SimpleJSParser::SwitchStatementContext *context) = 0;

    virtual std::any visitCaseClause(SimpleJSParser::CaseClauseContext *context) = 0;

    virtual std::any visitDefaultClause(SimpleJSParser::DefaultClauseContext *context) = 0;

    virtual std::any visitBreakStatement(SimpleJSParser::BreakStatementContext *context) = 0;

    virtual std::any visitContinueStatement(SimpleJSParser::ContinueStatementContext *context) = 0;

    virtual std::any visitEmptyStatement(SimpleJSParser::EmptyStatementContext *context) = 0;

    virtual std::any visitExpression(SimpleJSParser::ExpressionContext *context) = 0;

    virtual std::any visitAssignmentExpression(SimpleJSParser::AssignmentExpressionContext *context) = 0;

    virtual std::any visitConditionalExpression(SimpleJSParser::ConditionalExpressionContext *context) = 0;

    virtual std::any visitLogicalOrExpression(SimpleJSParser::LogicalOrExpressionContext *context) = 0;

    virtual std::any visitLogicalAndExpression(SimpleJSParser::LogicalAndExpressionContext *context) = 0;

    virtual std::any visitEqualityExpression(SimpleJSParser::EqualityExpressionContext *context) = 0;

    virtual std::any visitRelationalExpression(SimpleJSParser::RelationalExpressionContext *context) = 0;

    virtual std::any visitAdditiveExpression(SimpleJSParser::AdditiveExpressionContext *context) = 0;

    virtual std::any visitMultiplicativeExpression(SimpleJSParser::MultiplicativeExpressionContext *context) = 0;

    virtual std::any visitUnaryExpression(SimpleJSParser::UnaryExpressionContext *context) = 0;

    virtual std::any visitPostfixExpression(SimpleJSParser::PostfixExpressionContext *context) = 0;

    virtual std::any visitLeftHandSideExpression(SimpleJSParser::LeftHandSideExpressionContext *context) = 0;

    virtual std::any visitCallExpression(SimpleJSParser::CallExpressionContext *context) = 0;

    virtual std::any visitMemberExpression(SimpleJSParser::MemberExpressionContext *context) = 0;

    virtual std::any visitPrimaryExpression(SimpleJSParser::PrimaryExpressionContext *context) = 0;

    virtual std::any visitFunctionExpression(SimpleJSParser::FunctionExpressionContext *context) = 0;

    virtual std::any visitArrowFunction(SimpleJSParser::ArrowFunctionContext *context) = 0;

    virtual std::any visitArguments(SimpleJSParser::ArgumentsContext *context) = 0;

    virtual std::any visitArrayLiteral(SimpleJSParser::ArrayLiteralContext *context) = 0;

    virtual std::any visitObjectLiteral(SimpleJSParser::ObjectLiteralContext *context) = 0;

    virtual std::any visitPropertyAssignment(SimpleJSParser::PropertyAssignmentContext *context) = 0;

    virtual std::any visitLiteral(SimpleJSParser::LiteralContext *context) = 0;


};

