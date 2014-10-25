/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_ast_node_base_H
#define __CELL_ast_node_base_H

#include "spirit.h" // TreeIterator
#include "rules.h"
#include "types.h"
#include "boost/noncopyable.hpp"

#include <string>
#include <memory> // auto_ptr<>

namespace chaos { namespace cell {

// Forward declarations
class ASTVisitor;
class ASTNode;

//! Transforms an integer constant to a type.
template<int N> struct IntToType { enum { value = N }; };

//! Abstract base class for all AST context classes.
//! This class is designated to hold additional useful information that
//! will be propagated down the tree and back up during the AST traversal.
class ASTContext
{
public:
	ASTContext()
	{}

	virtual ~ASTContext() = 0;
};

//! Empty context.
class NilContext : public ASTContext
{};

//! Downcasts \a ctx to \a Context.
template<class Context>
inline Context context_cast(ASTContext* ctx)
{
	return static_cast<Context>(ctx);
}

//! Defines an interface for visitable classes.
class IAcceptor
{
public:
	//! Destroys the acceptor.
	virtual ~IAcceptor() = 0;

	//! Accepts the specified visitor.
	//! Returns \a true if the specified visitor was accepted, otherwise \a false.
	virtual bool accept(ASTVisitor& visitor, ASTContext* ctx) const = 0;
};

//! \IAcceptor adapter.
template<class Node, bool IsTerminal>
class Acceptor : public IAcceptor
{
public:
	typedef Node NodeType;
	
	//! Accept tags
	typedef IntToType<true> AcceptTerminal;
	typedef IntToType<false> AcceptNonTerminal;

	//! Creates a new acceptor with the specified \a node.
	Acceptor(Node& node) : _node(node)
	{}

	//! Destroys this acceptor.
	virtual ~Acceptor()
	{}

	//! Reimplemented from IAcceptor::accept().
	virtual bool accept(ASTVisitor& visitor, ASTContext* ctx) const
	{
		return accept(visitor, ctx, IntToType<IsTerminal>());
	}

private:
	//! Called for non-terminal nodes.
	bool accept(ASTVisitor& visitor, ASTContext* ctx, AcceptNonTerminal) const
	{
		if (visitor.preVisit(_node, ctx))
		{
			for (auto child = _node.firstChild(); child != nullptr; child = child->nextSibling())
			{
				if (!child->accept(visitor, ctx))
					break;
			}
		}
		return visitor.visit(_node, ctx);
	}

	//! Called for terminal nodes.
	bool accept(ASTVisitor& visitor, ASTContext* ctx, AcceptTerminal) const
	{
		return visitor.visit(_node, ctx);
	}

private:
	//! The wrapped node which would accept the visitor.
	Node& _node;
};

//! Thrown when trying to read an empty tag.
class TagError : public std::logic_error
{
public:
	explicit TagError(const char* message = "empty tag") : std::logic_error(message)
	{}
};

//! Base class for all AST nodes.
class ASTNode : boost::noncopyable
{
public:
	//! Constructs an empty node.
	ASTNode();

	//! Constructs a new node from a Spirit parse iterator.
	ASTNode(ASTNode* parent, const TreeIterator& it);

	//! Class destructor.
	virtual ~ASTNode() = 0;

	//! Gets the text data of this node.
	const std::string& text() const { return _text; }

	//! Gets the index of this node.
	int index() const { return _index; }
	
	//! Determines if this node has child nodes.
	bool empty() const { return _childCount == 0; }

	//! Gets the parent of this node.
	ASTNode* parent() const { return _parent; }

	//! Returns \a true if the node has parent, otherwise \a false.
	bool hasParent() const { return _parent != 0; }

	//! Returns the first child of this node.
	ASTNode* firstChild() const { return _firstChild; }

	//! Returns the last child of this node.
	ASTNode* lastChild() const { return _lastChild; }

	//! Gets the node immediately preceding this node.
	ASTNode* previousSibling() const { return _previousSibling; }

	//! Gets the node immediately following this node.
	ASTNode* nextSibling() const { return _nextSibling; }

	//! Appends the \a node to the list of its children.
	void addChild(ASTNode* node);

	//! Returns the child node at the specified \a index.
	ASTNode* childAt(int index) const;

	//! Gets the number of child nodes.
	int childCount() const { return _childCount; }

	//! Accepts the specified visitor.
	bool accept(ASTVisitor& visitor, ASTContext* ctx = 0);

	//! Determines if this node accepts visitors.
	bool isVisitable() const { return _isVisitable; }

	//! Associates arbitrary data to this node.
	template<class T>
	void setTag(const T& value)
	{
		_tag.reset(makeTag(value));
	}

	//! Returns the attached data.
	template<class T>
	T& tagAs() const
	{
		if (!hasTag())
			throw TagError();
		return *static_cast<T*>(_tag->content());
	}

	//! Returns \a true if this node has attached data, otherwise \a false.
	bool hasTag() const { return _tag.get() != nullptr; }

	//! Return the rule represented by this node.
	RuleID rid() const { return _rid; }

	//! Determines whether a \a node is of the specified \a type.
	static bool instanceof(const ASTNode* node, RuleID rid)
	{
		return (node != nullptr && node->rid() == rid);
	}

	//! Return the entire whole parse position - filename + line number + column number.
	const ParsePosition& parsePosition() const { return _pos; }

protected:
	//! Destroys the node deleting all its child nodes.
	void destroy();

	//! Base interface for node tags.
	struct Tag
	{
		virtual ~Tag() {}
		virtual void* content() = 0;
	};

	//! Creates a tag, boxing the specified value. T must be CopyAssignable.
	template<class T>
	static Tag* makeTag(const T& value)
	{
		struct TagHolder : Tag // hidden implementation of the Tag interface
		{
			T held;
			TagHolder(const T& t) : held(t) {}
			void* content() { return &held; }
		};

		return new TagHolder(value);
	}

protected:
	typedef std::auto_ptr<IAcceptor> AcceptorPtr;
	typedef std::auto_ptr<Tag>       TagPtr;

	AcceptorPtr   _acceptor;
	RuleID        _rid;
	std::string   _text;
	ParsePosition _pos;
	int           _index;
	int           _childCount;
	ASTNode*      _parent;
	ASTNode*      _previousSibling;
	ASTNode*      _nextSibling;
	ASTNode*      _firstChild;
	ASTNode*      _lastChild;
	unsigned      _isVisitable : 1;
	unsigned      _unused      : 31;
	TagPtr        _tag;
};

//! Base mixin.
template<class T, class Base, bool IsTerminal>
class ASTNodeBase : public Base
{
protected:
	//! The concrete acceptor type that will be used for this node.
	typedef Acceptor<T, IsTerminal> AcceptorType;

public:
	//! Default constructor.
	ASTNodeBase()
	{
		makeAcceptor();
	}
	
	//! Constructs a new node from the given spirit node.
	ASTNodeBase(ASTNode* parent, const TreeIterator& it)
		: Base(parent, it)
	{
		makeAcceptor();
	}

private:
	//! Initializes the acceptor of the concrete node.
	void makeAcceptor()
	{
		T& self = static_cast<T&>(*this);
		_acceptor.reset(new AcceptorType(self));
	}
};

//! Base class for terminal nodes.
template<class T, class Base = ASTNode>
class TerminalNode : public ASTNodeBase<T, Base, true>
{
public:
	//! Own type.
	typedef TerminalNode<T, Base> MyType;

	//! Constructs an empty %TerminalNode.
	TerminalNode()
	{}

	//! Constructs a new node from the given spirit node.
	TerminalNode(ASTNode* parent, const TreeIterator& it)
		: ASTNodeBase(parent, it)
	{}
};

//! Base class for non-terminal nodes.
template<class T, class Base = ASTNode>
class NonTerminalNode : public ASTNodeBase<T, Base, false>
{
public:
	//! Own type.
	typedef NonTerminalNode<T, Base> MyType;

	//! Constructs an empty %NonTerminalNode.
	NonTerminalNode()
	{}

	//! Constructs a new node from the given spirit node.
	NonTerminalNode(ASTNode* parent, const TreeIterator& it)
		: ASTNodeBase(parent, it)
	{}
};

}} // chaos::cell

#endif // __CELL_ast_node_base_H

