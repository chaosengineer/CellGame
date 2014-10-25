/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "ast_tree.h"
#include "ast_visitor.h"

namespace chaos { namespace cell {

ASTNode* createNode(ASTNode* parent, const TreeIterator& it);

void buildTree(ASTNode* parent, const TreeIterator& it)
{
	if (auto node = createNode(parent, it))
	{
		for (auto child = it->children.begin(), last = it->children.end(); child != last; ++child)
			buildTree(node, child);
	}
}

////////////////////////////////////////////////////////////////////////////////

ASTTree::ASTTree()
{}

ASTTree::~ASTTree()
{}

void ASTTree::build(const TreeIterator& it, const std::string& filename)
{
	destroy();
	_filename = filename;
	buildTree(this, it);
}

void ASTTree::clear()
{
	auto child = _firstChild;

	_firstChild = _lastChild = nullptr;
	_childCount = 0;

	while (child)
	{
		{
			auto next = child->nextSibling();
			delete child;
			child = next;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// Helper macros

#define BEGIN_NODE_MAP() \
	ASTNode* createNode(ASTNode* parent, const TreeIterator& it) { \
	RuleID rid = static_cast<RuleID>(it->value.id().to_long()); \
	switch (rid) {

#define END_NODE_MAP() default: assert_msg(false, "Unmatched rule: %s (RID: %d)\n", toString(rid), rid); return 0; };}


#if 0//_DEBUG
	#define NODE(ruleID, nodeClass) \
		case ruleID: { \
		const ParsePosition pos = it->value.begin().get_position(); \
		trace("Creating node: %s(%d): %s (rid: %d) -> %s\n", pos.file.c_str(), pos.line, toString(rid), rid, STRINGIZE(nodeClass)); \
		return new nodeClass(parent, it); }
#else
	#define NODE(ruleId, nodeClass) case ruleId: return new nodeClass(parent, it);
#endif


////////////////////////////////////////////////////////////////////////////////

BEGIN_NODE_MAP()
	NODE( RID_IDENTIFIER, IdentifierNode )
	NODE( RID_SYSTEM_IDENTIFIER, SystemIdentifierNode )

	NODE( RID_INTEGER_LITERAL_HEX, IntegerLiteralNode )
	NODE( RID_INTEGER_LITERAL_OCT, IntegerLiteralNode)
	NODE( RID_INTEGER_LITERAL_DEC, IntegerLiteralNode )

	NODE( RID_REAL_LITERAL, RealLiteralNode )
///	NODE( RID_BOOLEAN_LITERAL, BooleanLiteralNode )

	NODE( RID_START_SYMBOL, StartSymbolNode )

	NODE( RID_TRANSLATION_UNIT, TranslationUnitNode )

	NODE( RID_TYPE_SPECIFIER, TypeSpecifierNode )
	NODE( RID_TYPE_MODIFIER, TypeModifierNode )

	NODE( RID_ARGUMENT_LIST, ArgumentListNode )
	NODE( RID_PRIMARY_EXPRESSION, PrimaryExpressionNode )
	NODE( RID_PRIMARY_EXPRESSION_HELPER, PrimaryExpressionHelperNode )
	NODE( RID_PARENTHESIZED_EXPRESSION, ParenthesizedExpressionNode )
	NODE( RID_MEMBER_ACCESS, MemberAccessNode )
	NODE( RID_INVOCATION, InvocationNode )
	NODE( RID_ELEMENT_ACCESS, ElementAccessNode )
	NODE( RID_OBJECT_CREATION_EXPRESSION, ObjectCreationExpressionNode )
	NODE( RID_POSTFIX_EXPRESSION, PostfixExpressionNode )
	NODE( RID_UNARY_EXPRESSION, UnaryExpressionNode )
	NODE( RID_MULTIPLICATIVE_EXPRESSION, MultiplicativeExpressionNode )
	NODE( RID_ADDITIVE_EXPRESSION, AdditiveExpressionNode )
	NODE( RID_SHIFT_EXPRESSION, ShiftExpressionNode )
	NODE( RID_RELATIONAL_EXPRESSION, RelationalExpressionNode )
	NODE( RID_EQUALITY_EXPRESSION, EqualityExpressionNode )
	NODE( RID_AND_EXPRESSION, AndExpressionNode )
	NODE( RID_EXCLUSIVE_OR_EXPRESSION, ExclusiveOrExpressionNode )
	NODE( RID_INCLUSIVE_OR_EXPRESSION, InclusiveOrExpressionNode )
	NODE( RID_CONDITIONAL_AND_EXPRESSION, ConditionalAndExpressionNode )
	NODE( RID_CONDITIONAL_OR_EXPRESSION, ConditionalOrExpressionNode )
	NODE( RID_CONDITIONAL_EXPRESSION, ConditionalExpressionNode )
	NODE( RID_ASSIGNMENT, AssignmentNode )
	NODE( RID_ARRAY_CREATION_EXPRESSION, ArrayCreationExpressionNode )

	NODE( RID_BLOCK, BlockNode )
	NODE( RID_STATEMENT_LIST, StatementListNode )
	NODE( RID_EMPTY_STATEMENT, EmptyStatementNode )
	NODE( RID_VARIABLE_DECLARATION, VariableDeclarationNode )
	NODE( RID_VARIABLE_DECLARATOR, VariableDeclaratorNode )
	NODE( RID_EXPRESSION_STATEMENT, ExpressionStatementNode )
	NODE( RID_IF_STATEMENT, IfStatementNode )
	NODE( RID_ELSE_STATEMENT, ElseStatementNode )
	NODE( RID_WHILE_STATEMENT, WhileStatementNode )
	NODE( RID_QUIT_STATEMENT, QuitStatementNode )
	NODE( RID_STATEMENT_EXPRESSION_LIST, StatementExpressionListNode )
	NODE( RID_QUALIFIED_IDENTIFIER, QualifiedIdentifierNode )

	NODE( RID_ARRAY_SPECIFIER, ArraySpecifierNode )
END_NODE_MAP()

}} // chaos::cell

