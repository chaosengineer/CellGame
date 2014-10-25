/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <stdarg.h>
#include <cstring> // strcmp()
#include <string>

std::string& formatStringV(std::string& dest, const char* format, va_list args);
std::string& formatString(std::string& dest, const char* format, ...);
std::string& toLower(std::string& s);

// Performs case sensitive string comparison
inline bool equals(const char* s1, const char* s2)
{
	return strcmp(s1, s2) == 0;
}

#endif // __STRING_UTILS_H__
