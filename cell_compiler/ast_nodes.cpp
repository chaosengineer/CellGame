/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "ast_nodes.h"
#include "ast_visitor.h"
#include "token_parsers.h"

namespace chaos { namespace cell {

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Node implementation macros

#define IMPLEMENT_NODE( node )\
	node::node(ASTNode* parent, const TreeIterator& it) : MyType(parent, it) {}\
	node::~node() {}

////////////////////////////////////////////////////////////////////////////////

ExpressionBase::ExpressionBase(ASTNode* parent, const TreeIterator& it)
	: ASTNode(parent, it)
{
	_text.assign(it->value.begin(), it->value.end());
	_operator = cell::getOperator(_text);
}

ExpressionBase::~ExpressionBase()
{
	// Do nothing
}

////////////////////////////////////////////////////////////////////////////////

UnaryExpressionBase::UnaryExpressionBase(ASTNode* parent, const TreeIterator& it)
	: ExpressionBase(parent, it)
{}

UnaryExpressionBase::~UnaryExpressionBase()
{
	// Do nothing
}

////////////////////////////////////////////////////////////////////////////////

BinaryExpressionBase::BinaryExpressionBase(ASTNode* parent, const TreeIterator& it)
	: ExpressionBase(parent, it)
{}

BinaryExpressionBase::~BinaryExpressionBase()
{
	// Do nothing
}

////////////////////////////////////////////////////////////////////////////////

IdentifierNode::IdentifierNode(ASTNode* parent, const TreeIterator& it)
	: MyType(parent, it)
{
	_text.assign(it->value.begin(), it->value.end());
	getIdentifier(_text, _id);
}

IdentifierNode::~IdentifierNode()
{
	// Do nothing
}

////////////////////////////////////////////////////////////////////////////////

TypeSpecifierNode::TypeSpecifierNode(ASTNode* parent, const TreeIterator& it)
	: MyType(parent, it)
{
	_type = getTypeSpecifier(_id);
}

TypeSpecifierNode::~TypeSpecifierNode()
{
	// Do nothing
}

////////////////////////////////////////////////////////////////////////////////

RealLiteralNode::RealLiteralNode(ASTNode* parent, const TreeIterator& it)
	: MyType(parent, it)
{
	bool overflow;
	_value = (float)getRealLiteralValue(_text, overflow);
}

RealLiteralNode::~RealLiteralNode()
{
	// Do nothing
}

////////////////////////////////////////////////////////////////////////////////

IntegerLiteralNode::IntegerLiteralNode(ASTNode* parent, const TreeIterator& it)
	: MyType(parent, it)
	, _radix(10)
{
	switch (it->value.id().to_long())
	{
	case RID_INTEGER_LITERAL_HEX:
		_radix = 16;
		break;

	case RID_INTEGER_LITERAL_OCT:
		_radix = 8;
		break;

	case RID_INTEGER_LITERAL_DEC:
		_radix = 10;
		break;
	}
	bool overflow;
	_value = getIntegerLiteralValue(_text, _radix, overflow);
}

IntegerLiteralNode::~IntegerLiteralNode()
{
	// Do nothing
}

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE( SystemIdentifierNode )
IMPLEMENT_NODE( StartSymbolNode )
IMPLEMENT_NODE( TranslationUnitNode )
IMPLEMENT_NODE( ArgumentListNode )
IMPLEMENT_NODE( PrimaryExpressionNode )
IMPLEMENT_NODE( PrimaryExpressionHelperNode )
IMPLEMENT_NODE( ParenthesizedExpressionNode )
IMPLEMENT_NODE( MemberAccessNode )
IMPLEMENT_NODE( InvocationNode )
IMPLEMENT_NODE( ElementAccessNode )
IMPLEMENT_NODE( ObjectCreationExpressionNode )
IMPLEMENT_NODE( ArrayCreationExpressionNode )
IMPLEMENT_NODE( PostfixExpressionNode )
IMPLEMENT_NODE( UnaryExpressionNode )
IMPLEMENT_NODE( MultiplicativeExpressionNode )
IMPLEMENT_NODE( AdditiveExpressionNode )
IMPLEMENT_NODE( ShiftExpressionNode )
IMPLEMENT_NODE( RelationalExpressionNode )
IMPLEMENT_NODE( EqualityExpressionNode )
IMPLEMENT_NODE( AndExpressionNode )
IMPLEMENT_NODE( ExclusiveOrExpressionNode )
IMPLEMENT_NODE( InclusiveOrExpressionNode )
IMPLEMENT_NODE( ConditionalAndExpressionNode )
IMPLEMENT_NODE( ConditionalOrExpressionNode )
IMPLEMENT_NODE( AssignmentNode )
IMPLEMENT_NODE( ConditionalExpressionNode )
IMPLEMENT_NODE( BlockNode )
IMPLEMENT_NODE( StatementListNode )
IMPLEMENT_NODE( EmptyStatementNode )
IMPLEMENT_NODE( VariableDeclarationNode )
IMPLEMENT_NODE( VariableDeclaratorListNode )
IMPLEMENT_NODE( VariableDeclaratorNode )
IMPLEMENT_NODE( ExpressionStatementNode )
IMPLEMENT_NODE( IfStatementNode )
IMPLEMENT_NODE( ElseStatementNode )
IMPLEMENT_NODE( WhileStatementNode )
IMPLEMENT_NODE( TypeModifierNode )
IMPLEMENT_NODE( StatementExpressionListNode )
IMPLEMENT_NODE( QuitStatementNode )
IMPLEMENT_NODE( ArrayDeclaratorNode )
IMPLEMENT_NODE( ArraySpecifierNode )
IMPLEMENT_NODE( QualifiedIdentifierNode )

}} // chaos::cell

