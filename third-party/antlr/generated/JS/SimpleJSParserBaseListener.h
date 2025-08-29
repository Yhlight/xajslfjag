
// Generated from third-party/antlr/Grammars/JavaScript/SimpleJSParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SimpleJSParserListener.h"


/**
 * This class provides an empty implementation of SimpleJSParserListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  SimpleJSParserBaseListener : public SimpleJSParserListener {
public:

  virtual void enterProgram(SimpleJSParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(SimpleJSParser::ProgramContext * /*ctx*/) override { }

  virtual void enterStatement(SimpleJSParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(SimpleJSParser::StatementContext * /*ctx*/) override { }

  virtual void enterBlock(SimpleJSParser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(SimpleJSParser::BlockContext * /*ctx*/) override { }

  virtual void enterVarDeclaration(SimpleJSParser::VarDeclarationContext * /*ctx*/) override { }
  virtual void exitVarDeclaration(SimpleJSParser::VarDeclarationContext * /*ctx*/) override { }

  virtual void enterVarDeclarator(SimpleJSParser::VarDeclaratorContext * /*ctx*/) override { }
  virtual void exitVarDeclarator(SimpleJSParser::VarDeclaratorContext * /*ctx*/) override { }

  virtual void enterFunctionDeclaration(SimpleJSParser::FunctionDeclarationContext * /*ctx*/) override { }
  virtual void exitFunctionDeclaration(SimpleJSParser::FunctionDeclarationContext * /*ctx*/) override { }

  virtual void enterClassDeclaration(SimpleJSParser::ClassDeclarationContext * /*ctx*/) override { }
  virtual void exitClassDeclaration(SimpleJSParser::ClassDeclarationContext * /*ctx*/) override { }

  virtual void enterClassElement(SimpleJSParser::ClassElementContext * /*ctx*/) override { }
  virtual void exitClassElement(SimpleJSParser::ClassElementContext * /*ctx*/) override { }

  virtual void enterMethodDefinition(SimpleJSParser::MethodDefinitionContext * /*ctx*/) override { }
  virtual void exitMethodDefinition(SimpleJSParser::MethodDefinitionContext * /*ctx*/) override { }

  virtual void enterPropertyDefinition(SimpleJSParser::PropertyDefinitionContext * /*ctx*/) override { }
  virtual void exitPropertyDefinition(SimpleJSParser::PropertyDefinitionContext * /*ctx*/) override { }

  virtual void enterParameterList(SimpleJSParser::ParameterListContext * /*ctx*/) override { }
  virtual void exitParameterList(SimpleJSParser::ParameterListContext * /*ctx*/) override { }

  virtual void enterExpressionStatement(SimpleJSParser::ExpressionStatementContext * /*ctx*/) override { }
  virtual void exitExpressionStatement(SimpleJSParser::ExpressionStatementContext * /*ctx*/) override { }

  virtual void enterIfStatement(SimpleJSParser::IfStatementContext * /*ctx*/) override { }
  virtual void exitIfStatement(SimpleJSParser::IfStatementContext * /*ctx*/) override { }

  virtual void enterWhileStatement(SimpleJSParser::WhileStatementContext * /*ctx*/) override { }
  virtual void exitWhileStatement(SimpleJSParser::WhileStatementContext * /*ctx*/) override { }

  virtual void enterForStatement(SimpleJSParser::ForStatementContext * /*ctx*/) override { }
  virtual void exitForStatement(SimpleJSParser::ForStatementContext * /*ctx*/) override { }

  virtual void enterReturnStatement(SimpleJSParser::ReturnStatementContext * /*ctx*/) override { }
  virtual void exitReturnStatement(SimpleJSParser::ReturnStatementContext * /*ctx*/) override { }

  virtual void enterThrowStatement(SimpleJSParser::ThrowStatementContext * /*ctx*/) override { }
  virtual void exitThrowStatement(SimpleJSParser::ThrowStatementContext * /*ctx*/) override { }

  virtual void enterTryStatement(SimpleJSParser::TryStatementContext * /*ctx*/) override { }
  virtual void exitTryStatement(SimpleJSParser::TryStatementContext * /*ctx*/) override { }

  virtual void enterSwitchStatement(SimpleJSParser::SwitchStatementContext * /*ctx*/) override { }
  virtual void exitSwitchStatement(SimpleJSParser::SwitchStatementContext * /*ctx*/) override { }

  virtual void enterCaseClause(SimpleJSParser::CaseClauseContext * /*ctx*/) override { }
  virtual void exitCaseClause(SimpleJSParser::CaseClauseContext * /*ctx*/) override { }

  virtual void enterDefaultClause(SimpleJSParser::DefaultClauseContext * /*ctx*/) override { }
  virtual void exitDefaultClause(SimpleJSParser::DefaultClauseContext * /*ctx*/) override { }

  virtual void enterBreakStatement(SimpleJSParser::BreakStatementContext * /*ctx*/) override { }
  virtual void exitBreakStatement(SimpleJSParser::BreakStatementContext * /*ctx*/) override { }

  virtual void enterContinueStatement(SimpleJSParser::ContinueStatementContext * /*ctx*/) override { }
  virtual void exitContinueStatement(SimpleJSParser::ContinueStatementContext * /*ctx*/) override { }

  virtual void enterEmptyStatement(SimpleJSParser::EmptyStatementContext * /*ctx*/) override { }
  virtual void exitEmptyStatement(SimpleJSParser::EmptyStatementContext * /*ctx*/) override { }

  virtual void enterExpression(SimpleJSParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(SimpleJSParser::ExpressionContext * /*ctx*/) override { }

  virtual void enterAssignmentExpression(SimpleJSParser::AssignmentExpressionContext * /*ctx*/) override { }
  virtual void exitAssignmentExpression(SimpleJSParser::AssignmentExpressionContext * /*ctx*/) override { }

  virtual void enterConditionalExpression(SimpleJSParser::ConditionalExpressionContext * /*ctx*/) override { }
  virtual void exitConditionalExpression(SimpleJSParser::ConditionalExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalOrExpression(SimpleJSParser::LogicalOrExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalOrExpression(SimpleJSParser::LogicalOrExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalAndExpression(SimpleJSParser::LogicalAndExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalAndExpression(SimpleJSParser::LogicalAndExpressionContext * /*ctx*/) override { }

  virtual void enterEqualityExpression(SimpleJSParser::EqualityExpressionContext * /*ctx*/) override { }
  virtual void exitEqualityExpression(SimpleJSParser::EqualityExpressionContext * /*ctx*/) override { }

  virtual void enterRelationalExpression(SimpleJSParser::RelationalExpressionContext * /*ctx*/) override { }
  virtual void exitRelationalExpression(SimpleJSParser::RelationalExpressionContext * /*ctx*/) override { }

  virtual void enterAdditiveExpression(SimpleJSParser::AdditiveExpressionContext * /*ctx*/) override { }
  virtual void exitAdditiveExpression(SimpleJSParser::AdditiveExpressionContext * /*ctx*/) override { }

  virtual void enterMultiplicativeExpression(SimpleJSParser::MultiplicativeExpressionContext * /*ctx*/) override { }
  virtual void exitMultiplicativeExpression(SimpleJSParser::MultiplicativeExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryExpression(SimpleJSParser::UnaryExpressionContext * /*ctx*/) override { }
  virtual void exitUnaryExpression(SimpleJSParser::UnaryExpressionContext * /*ctx*/) override { }

  virtual void enterPostfixExpression(SimpleJSParser::PostfixExpressionContext * /*ctx*/) override { }
  virtual void exitPostfixExpression(SimpleJSParser::PostfixExpressionContext * /*ctx*/) override { }

  virtual void enterLeftHandSideExpression(SimpleJSParser::LeftHandSideExpressionContext * /*ctx*/) override { }
  virtual void exitLeftHandSideExpression(SimpleJSParser::LeftHandSideExpressionContext * /*ctx*/) override { }

  virtual void enterCallExpression(SimpleJSParser::CallExpressionContext * /*ctx*/) override { }
  virtual void exitCallExpression(SimpleJSParser::CallExpressionContext * /*ctx*/) override { }

  virtual void enterMemberExpression(SimpleJSParser::MemberExpressionContext * /*ctx*/) override { }
  virtual void exitMemberExpression(SimpleJSParser::MemberExpressionContext * /*ctx*/) override { }

  virtual void enterPrimaryExpression(SimpleJSParser::PrimaryExpressionContext * /*ctx*/) override { }
  virtual void exitPrimaryExpression(SimpleJSParser::PrimaryExpressionContext * /*ctx*/) override { }

  virtual void enterFunctionExpression(SimpleJSParser::FunctionExpressionContext * /*ctx*/) override { }
  virtual void exitFunctionExpression(SimpleJSParser::FunctionExpressionContext * /*ctx*/) override { }

  virtual void enterArrowFunction(SimpleJSParser::ArrowFunctionContext * /*ctx*/) override { }
  virtual void exitArrowFunction(SimpleJSParser::ArrowFunctionContext * /*ctx*/) override { }

  virtual void enterArguments(SimpleJSParser::ArgumentsContext * /*ctx*/) override { }
  virtual void exitArguments(SimpleJSParser::ArgumentsContext * /*ctx*/) override { }

  virtual void enterArrayLiteral(SimpleJSParser::ArrayLiteralContext * /*ctx*/) override { }
  virtual void exitArrayLiteral(SimpleJSParser::ArrayLiteralContext * /*ctx*/) override { }

  virtual void enterObjectLiteral(SimpleJSParser::ObjectLiteralContext * /*ctx*/) override { }
  virtual void exitObjectLiteral(SimpleJSParser::ObjectLiteralContext * /*ctx*/) override { }

  virtual void enterPropertyAssignment(SimpleJSParser::PropertyAssignmentContext * /*ctx*/) override { }
  virtual void exitPropertyAssignment(SimpleJSParser::PropertyAssignmentContext * /*ctx*/) override { }

  virtual void enterLiteral(SimpleJSParser::LiteralContext * /*ctx*/) override { }
  virtual void exitLiteral(SimpleJSParser::LiteralContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

