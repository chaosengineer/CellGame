/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

//! 2D vector type. It translates to <2 x float>.
typedef float vec __attribute__((ext_vector_type(2)));

//! Simulation context
typedef struct Cell_t
{
	float radius;
	vec position;
	vec velocity;
	void* padding;
} Cell;

#define joinaux(a, b) a##b
#define join(a, b) joinaux(a, b)
#define sassert(e) typedef char join(assert__ln_, __LINE__)[(e) ? 1 : -1]

//! Make sure the size of Cell_t is 32 bytes.
sassert(sizeof(Cell) == 32);

//! Invoked when the compiler sees '#Radius[index]'.
float read_radius(Cell* cells, int index)
{
	return (cells + index)->radius;
}

//! Invoked when the compiler sees '#Position[index]'.
vec read_position(Cell* cells, int index)
{
	return (cells + index)->position;
}

//! Invoked when the compiler sees '#Velocity[index]'.
vec read_velocity(Cell* cells, int index)
{
	return (cells + index)->velocity;
}

//! Invoked when the compiler sees 'sqrt()'.
float cell_sqrt(float x)
{
	return __builtin_sqrtf(x);
}

//! Invoked when the compiler sees 'v.length'.
float cell_length(vec v)
{
	return cell_sqrt(v.x*v.x + v.y*v.y);
}

//! Invoked when the compiler sees 'v.normalized'.
vec cell_normalize(vec v)
{
	float length = cell_length(v);
	
	if (length < 1e-6f) {
		return v;
	} 
	
	return v / length;
}

//! Invoked when the compiler sees 'a ^ b'.
float cell_dot(vec a, vec b)
{
	return a.x*b.x + a.y*b.y;
}

//! Makes a vector from 2 floats.
vec cell_makeVec(float x, float y)
{
	vec v;
	v.x = x;
	v.y = y;
	return v;
}

// TODO: Add more functions :)

//! The main template. This function gets cloned each time the compiler is invoked
//| which then populates its body with generated instructions.
void cell_main_template(Cell* cells, int cellCount, float arenaRadius, vec* force)
{
	return;
}

