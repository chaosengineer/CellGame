/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_ast_tree_H
#define __CELL_ast_tree_H

#include "ast_nodes.h"

namespace chaos { namespace cell {

//! The root node.
class ASTTree : public NonTerminalNode<ASTTree>
{
public:
	ASTTree();
	virtual ~ASTTree();

	void build(const TreeIterator& it, const std::string& filename = "");
	const std::string& filename() const { return _filename; }
	void clear();

private:
	std::string _filename;
};

}} // chaos::cell

#endif // __CELL_ast_tree_H

