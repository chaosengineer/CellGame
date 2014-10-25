/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "string_utils.h"

std::string& formatStringV(std::string& dest, const char* format, va_list args)
{
	if (!format)
		return dest;

	int result = -1, length = 256;
	char* buffer = nullptr;

	while (result == -1)
	{
		if (buffer)
			delete [] buffer;
		buffer = new char[(length + 1) * sizeof(char)];
		memset(buffer, 0, (length + 1) * sizeof(char));
		result = vsnprintf(buffer, length, format, args);
		length *= 2;
	}

	dest.assign(buffer);
	delete [] buffer;
	return dest;
}

std::string& formatString(std::string& dest, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	formatStringV(dest, format, args);
	va_end(args);
	return dest;
}

std::string& toLower(std::string& s)
{
	for ( auto& ch : s )
		ch = tolower(static_cast<unsigned char>(ch));
	return s;
}
