/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#ifndef __CELL_common_H
#define __CELL_common_H

#include <cstdlib> // rand()

#if !defined(_MSC_VER) || (_MSC_VER < 1300)
	#define __noop ((void)0)
#endif

#ifndef STRINGIZE
	#define STRINGIZE_(x) #x
	#define STRINGIZE(x) STRINGIZE_(x)
#endif


#ifdef _DEBUG

#include <cassert>
#include <stdio.h> // printf()

#ifdef __GNUG__
// GCC needs the '##' extension to swallow a trailing comma if no arguments are passed to the ellipsis

#define assert_msg(condition, format, ...) \
	do { if (!(condition)) printf((format), ##__VA_ARGS__); assert(condition); } while (false)

#define trace(format, ...) printf((format), ##__VA_ARGS__)

#else

#define assert_msg(condition, format, ...) \
	do { if (!(condition)) printf((format), __VA_ARGS__); assert(condition); } while (false)

#define trace(format, ...) printf((format), __VA_ARGS__)

#endif // __GNUG__

#else

#define assert_msg(condition, format, ...) __noop
#define trace(format, ...) __noop

#endif // _DEBUG


#ifndef _countof
	#define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif

template<class IteratorT, class DistanceT>
inline IteratorT& safe_advance(IteratorT& it, const IteratorT& last, DistanceT count)
{
	while (it != last && count > 0)
	{
		++it;
		--count;
	}
	return it;
}

#endif // __CELL_common_H

