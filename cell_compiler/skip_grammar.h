/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_skip_grammar_H
#define __CELL_skip_grammar_H

#include "spirit.h"
#include "tokens.h"

namespace chaos { namespace cell {

//! Whitespace and comment grammar
struct SkipGrammar : public spirit_classic::grammar<SkipGrammar>
{
	template <typename Scanner>
	struct definition
	{
		definition(const SkipGrammar& self)
		{
			skip
				= spirit_classic::space_p
				| spirit_classic::comment_p(COMMENT_LINE_T)                 // line comment
				| spirit_classic::comment_p(COMMENT_START_T, COMMENT_END_T) // multi-line comment
				;
		}

		spirit_classic::rule<Scanner> skip;

		const spirit_classic::rule<Scanner>& start() const { return skip; }
	};
};

}} // chaos::cell

#endif // __CELL_skip_grammar_H

