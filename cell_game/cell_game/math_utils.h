/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#pragma once

#include "vector2d.h"
#include "cell.h"

namespace chaos {
namespace cell {

const float EPSILON = 1e-6f;
const float PI = 3.1415927f;
const float DOUBLE_PI = 6.2831853f;
const float INVERSE_PI = 0.31830986f;
const float LARGE_FLOAT = 1e19f;

inline float randomFloat(const float minValue = -1.0f, const float maxValue = 1.0f) {
	static const float INVERSE_RAND_MAX_FLOAT = 1.0f / RAND_MAX;
	return minValue + (maxValue - minValue) * INVERSE_RAND_MAX_FLOAT * rand();
}

inline float distance(const Vector &rhs, const Vector &lhs) {
	Vector difference = rhs - lhs;
	return difference.length();
}

inline float distance(const Cell &cell, const Vector &position) {
	float result = LARGE_FLOAT;
	
	if (!cell.isDead()) {
		result = distance(cell.position, position) - cell.radius;
	}

	return result;
}

inline float distance(const Vector &position, const Cell &cell) {
	return distance(cell, position);
}

inline float distance(const Cell &lhs, const Cell &rhs) {
	float result = LARGE_FLOAT;

	if (!lhs.isDead() && !rhs.isDead()) {
		result = distance(lhs.position, rhs.position) - (lhs.radius + rhs.radius);
	}

	return result;
}

inline float dot(const Vector &rhs, const Vector &lhs) {
	return rhs.x * lhs.x + rhs.y * lhs.y;
}

template <typename T>
inline T min(const T &lhs, const T &rhs) {
	return (lhs < rhs) ? lhs : rhs;
}

template <typename T>
inline T max(const T &lhs, const T &rhs) {
	return (rhs < lhs) ? lhs : rhs;
}

template <typename T>
inline T clamp(const T &value, const T &lowerLimit, const T &upperLimit) {
	return max(min(value, upperLimit), lowerLimit);
}

}; // namespace cell
}; // namespace chaos