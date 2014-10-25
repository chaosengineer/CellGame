/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include "cell.h"
#include "math_utils.h"

using namespace std;
using namespace chaos::cell;

////////////////////////////////////////////////////////////
// Cell implementation

Cell::Cell(float cellRadius, const Vector &cellPosition, const Vector &cellVelocity, const ICellAI *cellAI) 
	: radius(cellRadius), position(cellPosition), velocity(cellVelocity), ai(cellAI) {
}

bool Cell::isDead() const {
	return radius < EPSILON;
}

float Cell::getArea() const {
	return PI * radius * radius;
}

void Cell::setArea(const float newArea) {
	radius = sqrtf(INVERSE_PI * chaos::cell::max(newArea, 0.0f));
}