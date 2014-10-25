/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_types_H
#define __CELL_types_H

#include "common.h"
#include <ostream>

namespace chaos { namespace cell {

//! Built-in types.
enum TypeSpecifier
{
	TS_NONE,
	TS_INT,
	TS_REAL,
	TS_VECTOR
};

//! Operators.
enum ExpressionOperator
{
	OP_INVALID,

// Unary operators
	OP_NOT,           // '!'
	OP_BITNOT,        // '~'

// Unary operators with side effects
	OP_PLUSPLUS,      // '++'
	OP_MINUSMINUS,    // '--'

// Relational operators
	OP_EQ,            // '=='
	OP_NOTEQ,         // '!='
	OP_LT,            // '<'
	OP_GT,            // '>'
	OP_LTEQ,          // '<='
	OP_GTEQ,          // '>='

// Arithmetic operators
	OP_MUL,           // '*'
	OP_DIV,           // '/'
	OP_MOD,           // '%'
	OP_PLUS,          // '+'
	OP_MINUS,         // '-'
	OP_LSHIFT,        // '<<'
	OP_RSHIFT,        // '>>'
	OP_BITAND,        // '&'
	OP_BITXOR,        // '^'
	OP_BITOR,         // '|'
	OP_AND,           // '&&'
	OP_OR,            // '||'

// Assignment operators
	OP_ASSIGN,        // '='
	OP_MUL_ASSIGN,    // '*='
	OP_DIV_ASSIGN,    // '/='
	OP_MOD_ASSIGN,    // '%='
	OP_PLUS_ASSIGN,   // '+='
	OP_MINUS_ASSIGN,  // '-='
	OP_LSHIFT_ASSIGN, // '<<='
	OP_RSHIFT_ASSIGN, // '>>='
	OP_BITAND_ASSIGN, // '&='
	OP_BITXOR_ASSIGN, // '^='
	OP_BITOR_ASSIGN,  // '|='
};

}} // chaos::cell

#endif // __CELL_types_H

