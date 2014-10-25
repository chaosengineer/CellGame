/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#pragma once

#include <vector>

#include "vector2d.h"

namespace chaos {
namespace cell {

class ICellAI;

class Cell {

public:
	float radius;
	Vector position;
	Vector velocity;
	const ICellAI *ai;

	Cell(float cellRadius, const Vector &cellPosition, const Vector &cellVelocity, const ICellAI *cellAI = NULL);

	bool isDead() const;

	float getArea() const;
	void setArea(const float newArea);
};

}; // namespace cell
}; // namespace chaos