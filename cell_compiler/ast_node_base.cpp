/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "ast_node_base.h"
#include "ast_visitor.h"
#include "common.h"

namespace chaos { namespace cell {

////////////////////////////////////////////////////////////////////////////////
// ASTContext

ASTContext::~ASTContext()
{}

////////////////////////////////////////////////////////////////////////////////
// IAcceptor

IAcceptor::~IAcceptor()
{}

////////////////////////////////////////////////////////////////////////////////
// ASTNode

ASTNode::ASTNode()
	: _rid(RID_START_SYMBOL)
	, _index(-1)
	, _childCount(0)
	, _parent(nullptr)
	, _previousSibling(nullptr)
	, _nextSibling(nullptr)
	, _firstChild(nullptr)
	, _lastChild(nullptr)
	, _isVisitable(1)
{}

ASTNode::ASTNode(ASTNode* parent, const TreeIterator& it)
	: _rid(static_cast<RuleID>(it->value.id().to_long()))
	, _index(-1)
	, _childCount(0)
	, _parent(parent)
	, _previousSibling(nullptr)
	, _nextSibling(nullptr)
	, _firstChild(nullptr)
	, _lastChild(nullptr)
	, _isVisitable(1)
{
	if (parent)
		parent->addChild(this);
	_pos = it->value.begin().get_position();
}

ASTNode::~ASTNode()
{
	destroy();
}

void ASTNode::destroy()
{
	auto child = _firstChild;
	while (child)
	{
		auto next = child->_nextSibling;
		delete child;
		child = next;
	}
	_childCount = 0;
	_firstChild = _lastChild = nullptr;
}

void ASTNode::addChild(ASTNode* node)
{
	if (node)
	{
		if (!_firstChild)
			_firstChild = node;
		if (_lastChild)
			_lastChild->_nextSibling = node;
		node->_previousSibling = _lastChild;
		_lastChild = node;
		node->_index = _childCount++;
	}
}

ASTNode* ASTNode::childAt(int index) const
{
	for (auto child = _firstChild; child != nullptr; child = child->_nextSibling, --index)
	{
		if (index == 0)
			return child;
	}

	return nullptr;
}

bool ASTNode::accept(ASTVisitor& visitor, ASTContext* ctx)
{
	if (_isVisitable && _acceptor.get())
		return _acceptor->accept(visitor, ctx);

	trace("Skipping node: [%p]\n", this);
	return true;
}

}} // chaos::cell

