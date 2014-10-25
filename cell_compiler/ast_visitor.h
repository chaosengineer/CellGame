/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_ast_visitor_H
#define __CELL_ast_visitor_H

#include "ast_tree.h"

//! Visitor method declaration macros
#define CELL_DECLARE_VISIT(nodeClass) \
	virtual bool preVisit(nodeClass&, ASTContext* ctx); \
	virtual bool    visit(nodeClass&, ASTContext* ctx);

namespace chaos { namespace cell {

//! Abstract base class for all AST visitors.
class ASTVisitor : boost::noncopyable
{
protected:
	//! Thrown in order to stop traversal altogether.
	class StopTraversal : public std::logic_error
	{
	public:
		StopTraversal() : std::logic_error("StopTraversal") {}
	};

public:
	virtual ~ASTVisitor();
	virtual bool traverse(ASTNode& node) = 0;

protected:
	//! Call this to stop traversal.
	static void stopTraversal()
	{ throw StopTraversal(); }

public:
	CELL_DECLARE_VISIT( ASTTree )

	CELL_DECLARE_VISIT( StartSymbolNode )
	CELL_DECLARE_VISIT( TranslationUnitNode )

	CELL_DECLARE_VISIT( IdentifierNode )
	CELL_DECLARE_VISIT( SystemIdentifierNode )

	CELL_DECLARE_VISIT( IntegerLiteralNode )
	CELL_DECLARE_VISIT( RealLiteralNode )

	CELL_DECLARE_VISIT( TypeSpecifierNode )
	CELL_DECLARE_VISIT( TypeModifierNode )

	CELL_DECLARE_VISIT( ArgumentListNode )
	CELL_DECLARE_VISIT( PrimaryExpressionNode )
	CELL_DECLARE_VISIT( PrimaryExpressionHelperNode )
	CELL_DECLARE_VISIT( ParenthesizedExpressionNode )
	CELL_DECLARE_VISIT( MemberAccessNode )
	CELL_DECLARE_VISIT( InvocationNode )
	CELL_DECLARE_VISIT( ElementAccessNode )
	
	CELL_DECLARE_VISIT( ObjectCreationExpressionNode )

	CELL_DECLARE_VISIT( PostfixExpressionNode )
	CELL_DECLARE_VISIT( UnaryExpressionNode )
	CELL_DECLARE_VISIT( MultiplicativeExpressionNode )
	CELL_DECLARE_VISIT( AdditiveExpressionNode )
	CELL_DECLARE_VISIT( ShiftExpressionNode )
	CELL_DECLARE_VISIT( RelationalExpressionNode )
	CELL_DECLARE_VISIT( EqualityExpressionNode )
	CELL_DECLARE_VISIT( AndExpressionNode )
	CELL_DECLARE_VISIT( ExclusiveOrExpressionNode )
	CELL_DECLARE_VISIT( InclusiveOrExpressionNode )
	CELL_DECLARE_VISIT( ConditionalAndExpressionNode )
	CELL_DECLARE_VISIT( ConditionalOrExpressionNode )
	CELL_DECLARE_VISIT( ConditionalExpressionNode )
	CELL_DECLARE_VISIT( AssignmentNode )
	CELL_DECLARE_VISIT( ArrayCreationExpressionNode )

	CELL_DECLARE_VISIT( BlockNode )
	CELL_DECLARE_VISIT( StatementListNode )
	CELL_DECLARE_VISIT( EmptyStatementNode )
	CELL_DECLARE_VISIT( VariableDeclarationNode )
	CELL_DECLARE_VISIT( VariableDeclaratorListNode )
	CELL_DECLARE_VISIT( VariableDeclaratorNode )
	CELL_DECLARE_VISIT( ExpressionStatementNode )
	CELL_DECLARE_VISIT( IfStatementNode )
	CELL_DECLARE_VISIT( ElseStatementNode )
	CELL_DECLARE_VISIT( WhileStatementNode )
	CELL_DECLARE_VISIT( StatementExpressionListNode )
	CELL_DECLARE_VISIT( ArrayDeclaratorNode )
	CELL_DECLARE_VISIT( ArraySpecifierNode )
	CELL_DECLARE_VISIT( QuitStatementNode )
	CELL_DECLARE_VISIT( QualifiedIdentifierNode )

public:
	bool process(ASTNode* node);
	bool process(ASTNode& node);

protected:
	bool process(ASTNode* node, ASTContext* ctx)
	{ return node ? node->accept(*this, ctx) : false; }
	
	bool process(ASTNode& node, ASTContext* ctx)
	{ return node.accept(*this, ctx); }

	//! Returns the context associated with this visitor.
	virtual ASTContext* context();

protected:
	std::string _currentFile;
};

//! Base mixin for visitors.
template<class Context = NilContext>
class ASTVisitorMix : public ASTVisitor
{
public:
	//! Type of the associated context.
	typedef Context ContextType;

	//! Traverses the specified \a node, recursively.
	virtual bool traverse(ASTNode& node)
	{
		try
		{
			return node.accept(*this, &_startContext);
		}
		catch (StopTraversal&)
		{
			return false;
		}
		catch (...)
		{
			throw;
		}
	}

	//! Casts a generic ASTContext to the context type of this visitor.
	static Context* contextFrom(ASTContext* ctx) { return context_cast<Context*>(ctx); }

protected:
	//! The initial context instance.
	Context _startContext;
};

}} // chaos::cell

#endif // __CELL_ast_visitor_H

