/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "spirit.h"
#include "token_parsers.h"
#include "skip_grammar.h"

#include <cerrno> // errno

namespace chaos { namespace cell {

using std::string;
using namespace spirit_classic;

////////////////////////////////////////////////////////////////////////////////
//  A semantic action policy that applies the += operator.
//
//  Actions (what it does):
//      ref += value;
//      ref += T(first,last);
//      ref += value_ref;
//
//  Policy holder, corresponding helper method:
//      ref_value_actor, append_a( ref );
//      ref_const_ref_actor, append_a( ref, value_ref );
//
//  () operators: both
//

struct AppendAction
{
	template<class T, class ValueT>
	void act(T& ref, const ValueT& value) const
	{
		ref += value;
	}
	
	template<class T, class IteratorT>
	void act(T& ref, const IteratorT& first, const IteratorT& last) const
	{
		ref += T(first, last);
	}
};

template<class T>
inline spirit_classic::ref_value_actor<T, AppendAction> append_a(T& ref)
{
	return spirit_classic::ref_value_actor<T, AppendAction>(ref);
}

template<class T, class ValueT>
inline spirit_classic::ref_const_ref_actor<T, ValueT, AppendAction> append_a(T& ref, const ValueT& value)
{
	return spirit_classic::ref_const_ref_actor<T, ValueT, AppendAction>(ref, value);
}

////////////////////////////////////////////////////////////////////////////////

static const SkipGrammar skip;

////////////////////////////////////////////////////////////////////////////////

void getIntegerLiteral(const string& text, string& result)
{
	result = "";
	rule<> r
		= 
		(		lexeme_d[ '0' >> as_lower_d['x'] >> +xdigit_p ] // hex
			|	lexeme_d[ '0' >> +chset_p("0-7") ]              // oct
			|	lexeme_d[ +digit_p ]                            // dec
		)
		[ assign_a(result) ]
		;
	parse(text.c_str(), r, skip);
}

void getRealLiteral(const string& text, string& result)
{
	result = "";

	rule<> real_literal_1
		=
		lexeme_d
		[
				+digit_p
			>>	chset_p("eE")
			>>	!chset_p("+-")
			>>	+digit_p
			>>	!as_lower_d['f']
		]
		;

	rule<> real_literal_2
		=
		lexeme_d
		[
				*digit_p
			>>	'.'
			>>	+digit_p
			>>	!(
						chset_p("eE")
					>>	!chset_p("+-")
					>>	+digit_p
				)
			>>	!as_lower_d['f']
		]
		;

	rule<> real_literal_3
		=
		lexeme_d
		[
				+digit_p
			>>	'.'
			>>	*digit_p
			>>	!(
						chset_p("eE")
					>>	!chset_p("+-")
					>>	+digit_p
				)
			>>	!as_lower_d['f']
		]
		;

	rule<> r = (real_literal_1 | real_literal_2 | real_literal_3)[ assign_a(result) ];
	parse(text.c_str(), r, skip);
}

void getBoolLiteral(const string& text, string& result)
{
	rule<> r
		= str_p(TRUE_T) [ assign_a(result) ]
		| str_p(FALSE_T)[ assign_a(result) ]
		;
	parse(text.c_str(), r, skip);
}

////////////////////////////////////////////////////////////////////////////////

long int getIntegerLiteralValue(const string& literal, int radix, bool& overflow)
{
	errno = 0;
	long int value = strtol(literal.c_str(), 0, radix);
	overflow = (ERANGE == errno);
	return value;
}

double getRealLiteralValue(const string& literal, bool& overflow)
{
	errno = 0;
	double value = strtod(literal.c_str(), 0);
	overflow = (ERANGE == errno);
	return value;
}

bool getBoolLiteralValue(const string& literal)
{
	return literal == TRUE_T;
}

////////////////////////////////////////////////////////////////////////////////

void getIdentifier(const string& text, string& result)
{
	result = "";
	bool isSystemID = false;
	
	rule<> r
		=
		lexeme_d
		[
				!ch_p(SYSTEM_PREFIX_T)               [ assign_a(isSystemID, true) ]
			>>	((alpha_p | '_') >> *(alnum_p | '_'))[ assign_a(result) ]
		];
	
	parse(text.c_str(), r, skip);
}

TypeSpecifier getTypeSpecifier(const string& text)
{
	TypeSpecifier result = TS_NONE;
	rule<> r
		=
		longest_d
		[
				str_p( INT_T    )[ assign_a( result, TS_INT    ) ]
			|	str_p( REAL_T   )[ assign_a( result, TS_REAL   ) ]
			|	str_p( VECTOR_T )[ assign_a( result, TS_VECTOR ) ]
		]
		;
		parse(text.c_str(), r, skip);
		return result;
}

ExpressionOperator getOperator(const string& text)
{
	ExpressionOperator result = OP_INVALID;
	rule<> r
		=
		longest_d // use longest_d to catch multi-character operators
		[
				str_p( BANG_T       )[ assign_a( result, OP_NOT           ) ]
			|	str_p( TILDE_T      )[ assign_a( result, OP_BITNOT        ) ] // '+' must be before '++'
			|	str_p( PLUS_T       )[ assign_a( result, OP_PLUS          ) ]
			|	str_p( PLUSPLUS_T   )[ assign_a( result, OP_PLUSPLUS      ) ] // '-' must be before '--'
			|	str_p( MINUS_T      )[ assign_a( result, OP_MINUS         ) ]
			|	str_p( MINUSMINUS_T )[ assign_a( result, OP_MINUSMINUS    ) ] // '=' must be before '=='
			|	str_p( EQ_T         )[ assign_a( result, OP_ASSIGN        ) ]
			|	str_p( EQEQ_T       )[ assign_a( result, OP_EQ            ) ]
			|	str_p( NOTEQ_T      )[ assign_a( result, OP_NOTEQ         ) ]
			|	str_p( LT_T         )[ assign_a( result, OP_LT            ) ]
			|	str_p( GT_T         )[ assign_a( result, OP_GT            ) ]
			|	str_p( LTEQ_T       )[ assign_a( result, OP_LTEQ          ) ]
			|	str_p( GTEQ_T       )[ assign_a( result, OP_GTEQ          ) ]
			|	str_p( STAR_T       )[ assign_a( result, OP_MUL           ) ]
			|	str_p( SLASH_T      )[ assign_a( result, OP_DIV           ) ]
			|	str_p( MOD_T        )[ assign_a( result, OP_MOD           ) ]
			|	str_p( LTLT_T       )[ assign_a( result, OP_LSHIFT        ) ]
			|	str_p( GTGT_T       )[ assign_a( result, OP_RSHIFT        ) ]
			|	str_p( AND_T        )[ assign_a( result, OP_BITAND        ) ]
			|	str_p( XOR_T        )[ assign_a( result, OP_BITXOR        ) ]
			|	str_p( OR_T         )[ assign_a( result, OP_BITOR         ) ]
			|	str_p( ANDAND_T     )[ assign_a( result, OP_AND           ) ]
			|	str_p( OROR_T       )[ assign_a( result, OP_OR            ) ]
			|	str_p( STAREQ_T     )[ assign_a( result, OP_MUL_ASSIGN    ) ]
			|	str_p( SLASHEQ_T    )[ assign_a( result, OP_DIV_ASSIGN    ) ]
			|	str_p( MODEQ_T      )[ assign_a( result, OP_MOD_ASSIGN    ) ]
			|	str_p( PLUSEQ_T     )[ assign_a( result, OP_PLUS_ASSIGN   ) ]
			|	str_p( MINUSEQ_T    )[ assign_a( result, OP_MINUS_ASSIGN  ) ]
			|	str_p( LTLTEQ_T     )[ assign_a( result, OP_LSHIFT_ASSIGN ) ]
			|	str_p( GTGTEQ_T     )[ assign_a( result, OP_RSHIFT_ASSIGN ) ]
			|	str_p( ANDEQ_T      )[ assign_a( result, OP_BITAND_ASSIGN ) ]
			|	str_p( XOREQ_T      )[ assign_a( result, OP_BITXOR_ASSIGN ) ]
			|	str_p( OREQ_T       )[ assign_a( result, OP_BITOR_ASSIGN  ) ]
		]
		;
	parse(text.c_str(), r, skip);
	return result;
}

}} // chaos::cell

