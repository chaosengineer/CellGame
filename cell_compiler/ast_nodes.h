/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_ast_nodes_H
#define __CELL_ast_nodes_H

#include "spirit.h" // TreeIterator
#include "rules.h"
#include "types.h"
#include "ast_node_base.h"

namespace chaos { namespace cell {

class ASTVisitor;
class ASTNode;

////////////////////////////////////////////////////////////////////////////////
// Node declaration macros

#define DECLARE_NODE( node, base, T )\
	class node : public T<node, base> {\
	public:\
		node(ASTNode*, const TreeIterator&);\
		virtual ~node();\
	};

#define DECLARE_TERMINAL_EX( node, base )\
	DECLARE_NODE( node, base, TerminalNode )

#define DECLARE_NONTERMINAL_EX( node, base )\
	DECLARE_NODE( node, base, NonTerminalNode )

#define DECLARE_TERMINAL( node )\
	DECLARE_NODE( node, ASTNode, TerminalNode )

#define DECLARE_NONTERMINAL( node )\
	DECLARE_NODE( node, ASTNode, NonTerminalNode )

#define DECLARE_BINARY_EXPRESSION( node )\
	DECLARE_NONTERMINAL_EX( node, BinaryExpressionBase )

#define DECLARE_UNARY_EXPRESSION( node )\
	DECLARE_NONTERMINAL_EX( node, UnaryExpressionBase )

//! Base class for all expressions.
class ExpressionBase : public ASTNode
{
public:
	ExpressionBase(ASTNode* parent, const TreeIterator& it);
	virtual ~ExpressionBase() = 0;
	ExpressionOperator getOperator() const { return _operator; }

protected:
	ExpressionOperator _operator;
};

//! Base class for all unary expressions.
class UnaryExpressionBase : public ExpressionBase
{
public:
	UnaryExpressionBase(ASTNode* parent, const TreeIterator& it);
	virtual ~UnaryExpressionBase();
	ASTNode* operand() const { return _firstChild; }
};

//! Base class for all binary expressions.
class BinaryExpressionBase : public ExpressionBase
{
public:
	BinaryExpressionBase(ASTNode* parent, const TreeIterator& it);
	virtual ~BinaryExpressionBase();
	ASTNode* leftOperand()  const { return childAt(0); }
	ASTNode* rightOperand() const { return childAt(1); }
};

//! Base class for identifiers.
class IdentifierNode : public TerminalNode<IdentifierNode>
{
public:
	IdentifierNode(ASTNode* parent, const TreeIterator& it);
	virtual ~IdentifierNode();
	const std::string& id() const { return _id; }

protected:
	std::string _id;
};

DECLARE_TERMINAL_EX( SystemIdentifierNode, IdentifierNode )

//! Base class for all literal nodes.
template<class T>
class LiteralBase : public ASTNode
{
public:
	LiteralBase(ASTNode* parent, const TreeIterator& it) : ASTNode(parent, it), _value(T())
	{ _text.assign(it->value.begin(), it->value.end()); }

	const T& value() const { return _value; }

protected:
	T _value;
};

//! Single-precision floating-point constants.
class RealLiteralNode : public TerminalNode<RealLiteralNode, LiteralBase<float> >
{
public:
	RealLiteralNode(ASTNode* parent, const TreeIterator& it);
	virtual ~RealLiteralNode();
};

//! Integer constants.
class IntegerLiteralNode : public TerminalNode<IntegerLiteralNode, LiteralBase<int> >
{
public:
	IntegerLiteralNode(ASTNode* parent, const TreeIterator& it);
	virtual ~IntegerLiteralNode();
	int radix() const { return _radix; }

private:
	int _radix;
};

class TypeSpecifierNode : public TerminalNode<TypeSpecifierNode, IdentifierNode>
{
public:
	TypeSpecifierNode(ASTNode* parent, const TreeIterator& it);
	virtual ~TypeSpecifierNode();
	TypeSpecifier type() const { return _type; }

private:
	TypeSpecifier _type;
};

DECLARE_TERMINAL( TypeModifierNode )
DECLARE_NONTERMINAL( StartSymbolNode )
DECLARE_NONTERMINAL( TranslationUnitNode )
DECLARE_NONTERMINAL( ArgumentListNode )
DECLARE_NONTERMINAL( PrimaryExpressionNode )
DECLARE_NONTERMINAL( PrimaryExpressionHelperNode )
DECLARE_NONTERMINAL( ParenthesizedExpressionNode )

class MemberAccessNode : public NonTerminalNode<MemberAccessNode>
{
public:
	MemberAccessNode(ASTNode* parent, const TreeIterator& it);
	virtual ~MemberAccessNode();
	ASTNode* name() const { return firstChild(); }
};

class InvocationNode : public NonTerminalNode<InvocationNode>
{
public:
	InvocationNode(ASTNode* parent, const TreeIterator& it);
	virtual ~InvocationNode();
	ASTNode* invocationName() const { return childAt(0); }
	ASTNode* invocationArguments() const { return childAt(1); }
};

DECLARE_NONTERMINAL( ElementAccessNode )

DECLARE_NONTERMINAL_EX( ObjectCreationExpressionNode, TypeSpecifierNode )
DECLARE_NONTERMINAL_EX( ArrayCreationExpressionNode, TypeSpecifierNode )

DECLARE_UNARY_EXPRESSION( PostfixExpressionNode )
DECLARE_UNARY_EXPRESSION( UnaryExpressionNode )

DECLARE_BINARY_EXPRESSION( MultiplicativeExpressionNode )
DECLARE_BINARY_EXPRESSION( AdditiveExpressionNode )
DECLARE_BINARY_EXPRESSION( ShiftExpressionNode )
DECLARE_BINARY_EXPRESSION( RelationalExpressionNode )
DECLARE_BINARY_EXPRESSION( EqualityExpressionNode )
DECLARE_BINARY_EXPRESSION( AndExpressionNode )
DECLARE_BINARY_EXPRESSION( ExclusiveOrExpressionNode )
DECLARE_BINARY_EXPRESSION( InclusiveOrExpressionNode )
DECLARE_BINARY_EXPRESSION( ConditionalAndExpressionNode )
DECLARE_BINARY_EXPRESSION( ConditionalOrExpressionNode )
DECLARE_BINARY_EXPRESSION( AssignmentNode )

DECLARE_NONTERMINAL( ConditionalExpressionNode )

DECLARE_NONTERMINAL( BlockNode )
DECLARE_NONTERMINAL( StatementListNode )
DECLARE_TERMINAL( EmptyStatementNode )

DECLARE_NONTERMINAL_EX( VariableDeclarationNode, TypeSpecifierNode )
DECLARE_NONTERMINAL( VariableDeclaratorListNode )
DECLARE_NONTERMINAL_EX( VariableDeclaratorNode, IdentifierNode )
DECLARE_NONTERMINAL( ExpressionStatementNode )

class IfStatementNode : public NonTerminalNode<IfStatementNode>
{
public:
	IfStatementNode(ASTNode* parent, const TreeIterator& it);
	~IfStatementNode();
	virtual ASTNode* condition() const { return childAt(0); }
	virtual ASTNode* thenBody()  const { return childAt(1); }
	virtual ASTNode* elseBody()  const { return childAt(2); }
};

DECLARE_NONTERMINAL( ElseStatementNode )

class WhileStatementNode : public NonTerminalNode<WhileStatementNode>
{
public:
	WhileStatementNode(ASTNode* parent, const TreeIterator& it);
	~WhileStatementNode();
	virtual ASTNode* condition() const { return childAt(0); }
	virtual ASTNode* body() const { return childAt(1); }
};

DECLARE_NONTERMINAL( StatementExpressionListNode )
DECLARE_TERMINAL( QuitStatementNode )
DECLARE_NONTERMINAL_EX( QualifiedIdentifierNode, IdentifierNode )
DECLARE_NONTERMINAL( ArrayDeclaratorNode )
DECLARE_NONTERMINAL( ArraySpecifierNode )

}} // chaos::cell

#endif // __CELL_ast_nodes_H

