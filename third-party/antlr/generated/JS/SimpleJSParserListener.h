
// Generated from SimpleJSParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SimpleJSParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by SimpleJSParser.
 */
class  SimpleJSParserListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(SimpleJSParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(SimpleJSParser::ProgramContext *ctx) = 0;

  virtual void enterStatement(SimpleJSParser::StatementContext *ctx) = 0;
  virtual void exitStatement(SimpleJSParser::StatementContext *ctx) = 0;

  virtual void enterBlock(SimpleJSParser::BlockContext *ctx) = 0;
  virtual void exitBlock(SimpleJSParser::BlockContext *ctx) = 0;

  virtual void enterVarDeclaration(SimpleJSParser::VarDeclarationContext *ctx) = 0;
  virtual void exitVarDeclaration(SimpleJSParser::VarDeclarationContext *ctx) = 0;

  virtual void enterVarDeclarator(SimpleJSParser::VarDeclaratorContext *ctx) = 0;
  virtual void exitVarDeclarator(SimpleJSParser::VarDeclaratorContext *ctx) = 0;

  virtual void enterFunctionDeclaration(SimpleJSParser::FunctionDeclarationContext *ctx) = 0;
  virtual void exitFunctionDeclaration(SimpleJSParser::FunctionDeclarationContext *ctx) = 0;

  virtual void enterClassDeclaration(SimpleJSParser::ClassDeclarationContext *ctx) = 0;
  virtual void exitClassDeclaration(SimpleJSParser::ClassDeclarationContext *ctx) = 0;

  virtual void enterClassElement(SimpleJSParser::ClassElementContext *ctx) = 0;
  virtual void exitClassElement(SimpleJSParser::ClassElementContext *ctx) = 0;

  virtual void enterMethodDefinition(SimpleJSParser::MethodDefinitionContext *ctx) = 0;
  virtual void exitMethodDefinition(SimpleJSParser::MethodDefinitionContext *ctx) = 0;

  virtual void enterPropertyDefinition(SimpleJSParser::PropertyDefinitionContext *ctx) = 0;
  virtual void exitPropertyDefinition(SimpleJSParser::PropertyDefinitionContext *ctx) = 0;

  virtual void enterParameterList(SimpleJSParser::ParameterListContext *ctx) = 0;
  virtual void exitParameterList(SimpleJSParser::ParameterListContext *ctx) = 0;

  virtual void enterExpressionStatement(SimpleJSParser::ExpressionStatementContext *ctx) = 0;
  virtual void exitExpressionStatement(SimpleJSParser::ExpressionStatementContext *ctx) = 0;

  virtual void enterIfStatement(SimpleJSParser::IfStatementContext *ctx) = 0;
  virtual void exitIfStatement(SimpleJSParser::IfStatementContext *ctx) = 0;

  virtual void enterWhileStatement(SimpleJSParser::WhileStatementContext *ctx) = 0;
  virtual void exitWhileStatement(SimpleJSParser::WhileStatementContext *ctx) = 0;

  virtual void enterForStatement(SimpleJSParser::ForStatementContext *ctx) = 0;
  virtual void exitForStatement(SimpleJSParser::ForStatementContext *ctx) = 0;

  virtual void enterReturnStatement(SimpleJSParser::ReturnStatementContext *ctx) = 0;
  virtual void exitReturnStatement(SimpleJSParser::ReturnStatementContext *ctx) = 0;

  virtual void enterThrowStatement(SimpleJSParser::ThrowStatementContext *ctx) = 0;
  virtual void exitThrowStatement(SimpleJSParser::ThrowStatementContext *ctx) = 0;

  virtual void enterTryStatement(SimpleJSParser::TryStatementContext *ctx) = 0;
  virtual void exitTryStatement(SimpleJSParser::TryStatementContext *ctx) = 0;

  virtual void enterSwitchStatement(SimpleJSParser::SwitchStatementContext *ctx) = 0;
  virtual void exitSwitchStatement(SimpleJSParser::SwitchStatementContext *ctx) = 0;

  virtual void enterCaseClause(SimpleJSParser::CaseClauseContext *ctx) = 0;
  virtual void exitCaseClause(SimpleJSParser::CaseClauseContext *ctx) = 0;

  virtual void enterDefaultClause(SimpleJSParser::DefaultClauseContext *ctx) = 0;
  virtual void exitDefaultClause(SimpleJSParser::DefaultClauseContext *ctx) = 0;

  virtual void enterBreakStatement(SimpleJSParser::BreakStatementContext *ctx) = 0;
  virtual void exitBreakStatement(SimpleJSParser::BreakStatementContext *ctx) = 0;

  virtual void enterContinueStatement(SimpleJSParser::ContinueStatementContext *ctx) = 0;
  virtual void exitContinueStatement(SimpleJSParser::ContinueStatementContext *ctx) = 0;

  virtual void enterEmptyStatement(SimpleJSParser::EmptyStatementContext *ctx) = 0;
  virtual void exitEmptyStatement(SimpleJSParser::EmptyStatementContext *ctx) = 0;

  virtual void enterExpression(SimpleJSParser::ExpressionContext *ctx) = 0;
  virtual void exitExpression(SimpleJSParser::ExpressionContext *ctx) = 0;

  virtual void enterAssignmentExpression(SimpleJSParser::AssignmentExpressionContext *ctx) = 0;
  virtual void exitAssignmentExpression(SimpleJSParser::AssignmentExpressionContext *ctx) = 0;

  virtual void enterConditionalExpression(SimpleJSParser::ConditionalExpressionContext *ctx) = 0;
  virtual void exitConditionalExpression(SimpleJSParser::ConditionalExpressionContext *ctx) = 0;

  virtual void enterLogicalOrExpression(SimpleJSParser::LogicalOrExpressionContext *ctx) = 0;
  virtual void exitLogicalOrExpression(SimpleJSParser::LogicalOrExpressionContext *ctx) = 0;

  virtual void enterLogicalAndExpression(SimpleJSParser::LogicalAndExpressionContext *ctx) = 0;
  virtual void exitLogicalAndExpression(SimpleJSParser::LogicalAndExpressionContext *ctx) = 0;

  virtual void enterEqualityExpression(SimpleJSParser::EqualityExpressionContext *ctx) = 0;
  virtual void exitEqualityExpression(SimpleJSParser::EqualityExpressionContext *ctx) = 0;

  virtual void enterRelationalExpression(SimpleJSParser::RelationalExpressionContext *ctx) = 0;
  virtual void exitRelationalExpression(SimpleJSParser::RelationalExpressionContext *ctx) = 0;

  virtual void enterAdditiveExpression(SimpleJSParser::AdditiveExpressionContext *ctx) = 0;
  virtual void exitAdditiveExpression(SimpleJSParser::AdditiveExpressionContext *ctx) = 0;

  virtual void enterMultiplicativeExpression(SimpleJSParser::MultiplicativeExpressionContext *ctx) = 0;
  virtual void exitMultiplicativeExpression(SimpleJSParser::MultiplicativeExpressionContext *ctx) = 0;

  virtual void enterUnaryExpression(SimpleJSParser::UnaryExpressionContext *ctx) = 0;
  virtual void exitUnaryExpression(SimpleJSParser::UnaryExpressionContext *ctx) = 0;

  virtual void enterPostfixExpression(SimpleJSParser::PostfixExpressionContext *ctx) = 0;
  virtual void exitPostfixExpression(SimpleJSParser::PostfixExpressionContext *ctx) = 0;

  virtual void enterLeftHandSideExpression(SimpleJSParser::LeftHandSideExpressionContext *ctx) = 0;
  virtual void exitLeftHandSideExpression(SimpleJSParser::LeftHandSideExpressionContext *ctx) = 0;

  virtual void enterCallExpression(SimpleJSParser::CallExpressionContext *ctx) = 0;
  virtual void exitCallExpression(SimpleJSParser::CallExpressionContext *ctx) = 0;

  virtual void enterMemberExpression(SimpleJSParser::MemberExpressionContext *ctx) = 0;
  virtual void exitMemberExpression(SimpleJSParser::MemberExpressionContext *ctx) = 0;

  virtual void enterPrimaryExpression(SimpleJSParser::PrimaryExpressionContext *ctx) = 0;
  virtual void exitPrimaryExpression(SimpleJSParser::PrimaryExpressionContext *ctx) = 0;

  virtual void enterFunctionExpression(SimpleJSParser::FunctionExpressionContext *ctx) = 0;
  virtual void exitFunctionExpression(SimpleJSParser::FunctionExpressionContext *ctx) = 0;

  virtual void enterArrowFunction(SimpleJSParser::ArrowFunctionContext *ctx) = 0;
  virtual void exitArrowFunction(SimpleJSParser::ArrowFunctionContext *ctx) = 0;

  virtual void enterArguments(SimpleJSParser::ArgumentsContext *ctx) = 0;
  virtual void exitArguments(SimpleJSParser::ArgumentsContext *ctx) = 0;

  virtual void enterArrayLiteral(SimpleJSParser::ArrayLiteralContext *ctx) = 0;
  virtual void exitArrayLiteral(SimpleJSParser::ArrayLiteralContext *ctx) = 0;

  virtual void enterObjectLiteral(SimpleJSParser::ObjectLiteralContext *ctx) = 0;
  virtual void exitObjectLiteral(SimpleJSParser::ObjectLiteralContext *ctx) = 0;

  virtual void enterPropertyAssignment(SimpleJSParser::PropertyAssignmentContext *ctx) = 0;
  virtual void exitPropertyAssignment(SimpleJSParser::PropertyAssignmentContext *ctx) = 0;

  virtual void enterLiteral(SimpleJSParser::LiteralContext *ctx) = 0;
  virtual void exitLiteral(SimpleJSParser::LiteralContext *ctx) = 0;


};

