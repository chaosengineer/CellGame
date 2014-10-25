/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_error_H
#define __CELL_error_H

#include "spirit.h" // ParsePosition
#include <stdexcept>

namespace chaos { namespace cell {

//! Exceptions thrown by the traversers.
class CellError : public std::runtime_error
{
public:
	CellError(const char* message) : std::runtime_error(message)
	{}

	CellError(const std::string& message) : std::runtime_error(message)
	{}

	// noreturn
	static void raise(const char* format, ...);
	
	// noreturn
	static void raise(const ParsePosition& pos, const char* format, ...);
};

}} // chaos::cell

#endif // __CELL_error_H

