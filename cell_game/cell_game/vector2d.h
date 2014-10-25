/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#pragma once

#include <math.h> // sqrtf

namespace chaos {
namespace cell {

_declspec(align(8))
class Vector {

public:
	float x;
	float y;

	Vector(const float inputX = 0.0f, const float inputY = 0.0f)
		: x(inputX), y(inputY) {
	}

	float length() const {
		return sqrtf(x * x + y * y);
	}

	void normalize() {
		float vectorLength = length();
		if (0.0f < vectorLength) {
			x /= vectorLength;
			y /= vectorLength;
		}
	}

	Vector& operator+=(const float rhs) {
		x += rhs;
		y += rhs;
		return *this;
	}

	Vector& operator+=(const Vector &rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vector& operator-=(const float rhs) {
		x -= rhs;
		y -= rhs;
		return *this;
	}

	Vector& operator-=(const Vector &rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vector& operator*=(const float rhs) {
		x *= rhs;
		y *= rhs;
		return *this;
	}

	Vector& operator*=(const Vector &rhs) {
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	Vector& operator/=(const float rhs) {
		x /= rhs;
		y /= rhs;
		return *this;
	}

	Vector& operator/=(const Vector &rhs) {
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}
};

inline Vector operator-(const Vector &vector) {
	return Vector(-vector.x, -vector.y);
}

inline Vector operator+(const Vector &lhs, const float rhs) {
	return Vector(lhs.x + rhs, lhs.y + rhs);
}

inline Vector operator+(const float lhs, const Vector &rhs) {
	return Vector(lhs + rhs.x, lhs + rhs.y);
}

inline Vector operator+(const Vector &lhs, const Vector &rhs) {
	return Vector(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline Vector operator-(const Vector &lhs, const float rhs) {
	return Vector(lhs.x - rhs, lhs.y - rhs);
}

inline Vector operator-(const Vector &lhs, const Vector &rhs) {
	return Vector(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline Vector operator*(const Vector &lhs, const float rhs) {
	return Vector(lhs.x * rhs, lhs.y * rhs);
}

inline Vector operator*(const float lhs, const Vector &rhs) {
	return Vector(lhs * rhs.x, lhs * rhs.y);
}

inline Vector operator*(const Vector &lhs, const Vector &rhs) {
	return Vector(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline Vector operator/(const Vector &lhs, const float rhs) {
	return Vector(lhs.x / rhs, lhs.y / rhs);
}

inline Vector operator/(const Vector &lhs, const Vector &rhs) {
	return Vector(lhs.x / rhs.x, lhs.y / rhs.y);
}

}; // namespace cell
}; // namespace chaos