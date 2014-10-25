/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "error.h"
#include "string_utils.h"

namespace chaos { namespace cell {

void CellError::raise(const char* format, ...)
{
	std::string message;
	va_list args;
	va_start(args, format);
	formatStringV(message, format, args);
	va_end(args);
	throw CellError(message);
}

void CellError::raise(const ParsePosition& pos, const char* format, ...)
{
	std::string what;
	va_list args;
	va_start(args, format);
	formatStringV(what, format, args);
	va_end(args);

	std::string message;
	formatString(message, "%s (%d) : ", pos.file.c_str(), pos.line);
	message += what;

	throw CellError(message);
}

}} // chaos::cell

