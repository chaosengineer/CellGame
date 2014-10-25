/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_ast_dumper_H
#define __CELL_ast_dumper_H

#include "spirit.h"
#include <map>

namespace chaos { namespace cell {

class ASTDumper
{
public:
	ASTDumper(std::ostream& os = std::cout);

	template<class TreeNodeT>
	void dump(const TreeNodeT& tree, const char* inputLine = "") const
	{
		spirit_classic::tree_to_xml(_os, tree, inputLine, _rules);
	}

	template<class TreeNodeT>
	const ASTDumper& operator<<(const TreeNodeT& tree) const
	{
		dump(tree);
		return *this;
	}

private:
	void initialize();

private:
	typedef std::map<spirit_classic::parser_id, std::string> RuleMap;

	static RuleMap _rules;
	std::ostream&  _os;
};

}} // chaos::cell

#endif // __CELL_ast_dumper_H

