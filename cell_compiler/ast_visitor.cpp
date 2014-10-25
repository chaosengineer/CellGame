/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "ast_visitor.h"

//! Implements default visitor behavior
#define IMPLEMENT_VISIT(nodeClass)\
	bool ASTVisitor::preVisit(nodeClass&, ASTContext*) { return true; }\
	bool ASTVisitor::visit(nodeClass&, ASTContext*) { return true; }\

namespace chaos { namespace cell {

ASTVisitor::~ASTVisitor()
{
	// Do nothing
}

bool ASTVisitor::process(ASTNode* node)
{
	return node ? node->accept(*this, context()) : false;
}

bool ASTVisitor::process(ASTNode& node)
{
	return node.accept(*this, context());
}

ASTContext* ASTVisitor::context()
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_VISIT( ASTTree )

IMPLEMENT_VISIT( IdentifierNode )
IMPLEMENT_VISIT( SystemIdentifierNode )

IMPLEMENT_VISIT( IntegerLiteralNode )
IMPLEMENT_VISIT( RealLiteralNode )
//IMPLEMENT_VISIT( BooleanLiteralNode )

IMPLEMENT_VISIT( StartSymbolNode )

IMPLEMENT_VISIT( TranslationUnitNode )

IMPLEMENT_VISIT( TypeSpecifierNode )
IMPLEMENT_VISIT( TypeModifierNode )

IMPLEMENT_VISIT( ArgumentListNode )
IMPLEMENT_VISIT( PrimaryExpressionNode )
IMPLEMENT_VISIT( PrimaryExpressionHelperNode )
IMPLEMENT_VISIT( ParenthesizedExpressionNode )
IMPLEMENT_VISIT( MemberAccessNode )
IMPLEMENT_VISIT( InvocationNode )
IMPLEMENT_VISIT( ElementAccessNode )

IMPLEMENT_VISIT( ObjectCreationExpressionNode )
IMPLEMENT_VISIT( PostfixExpressionNode )
IMPLEMENT_VISIT( UnaryExpressionNode )
IMPLEMENT_VISIT( MultiplicativeExpressionNode )
IMPLEMENT_VISIT( AdditiveExpressionNode )
IMPLEMENT_VISIT( ShiftExpressionNode )
IMPLEMENT_VISIT( RelationalExpressionNode )
IMPLEMENT_VISIT( EqualityExpressionNode )
IMPLEMENT_VISIT( AndExpressionNode )
IMPLEMENT_VISIT( ExclusiveOrExpressionNode )
IMPLEMENT_VISIT( InclusiveOrExpressionNode )
IMPLEMENT_VISIT( ConditionalAndExpressionNode )
IMPLEMENT_VISIT( ConditionalOrExpressionNode )
IMPLEMENT_VISIT( ConditionalExpressionNode )
IMPLEMENT_VISIT( AssignmentNode )
IMPLEMENT_VISIT( ArrayCreationExpressionNode )

IMPLEMENT_VISIT( BlockNode )
IMPLEMENT_VISIT( StatementListNode )
IMPLEMENT_VISIT( EmptyStatementNode )
IMPLEMENT_VISIT( VariableDeclarationNode )
IMPLEMENT_VISIT( VariableDeclaratorListNode )
IMPLEMENT_VISIT( VariableDeclaratorNode )
IMPLEMENT_VISIT( ExpressionStatementNode )
IMPLEMENT_VISIT( IfStatementNode )
IMPLEMENT_VISIT( ElseStatementNode )
IMPLEMENT_VISIT( WhileStatementNode )
IMPLEMENT_VISIT( StatementExpressionListNode )
IMPLEMENT_VISIT( ArrayDeclaratorNode )
IMPLEMENT_VISIT( ArraySpecifierNode )
IMPLEMENT_VISIT( QuitStatementNode )
IMPLEMENT_VISIT( QualifiedIdentifierNode )

}} // chaos::cell

