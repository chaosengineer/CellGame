/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "ast_dumper.h"
#include "rules.h"

namespace chaos { namespace cell {

ASTDumper::RuleMap ASTDumper::_rules;

ASTDumper::ASTDumper(std::ostream& os)
	: _os(os)
{
	if (_rules.empty())
		initialize();
}

//
// Initializes statics
//
void ASTDumper::initialize()
{
	for (int rid = RID_IDENTIFIER; rid <= RID_LAST; ++rid)
		_rules[rid] = toString(static_cast<RuleID>(rid));
}

}} // chaos::cell

