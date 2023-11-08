#pragma once

#include <stdbool.h>

#include "object.h"

#define DEFAULTRADIUS 1

bool isColliding(Object* a, Object* b);

bool isColliding(Object* a, Object* b) 
{
	int distance = vector3d_magnitude_between(a->position, b->position);
	return distance < DEFAULTRADIUS;
}