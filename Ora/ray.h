#pragma once

#include "gfc_vector.h"
#include "gfc_primitives.h"
#include "gfc_matrix.h"

typedef struct Ray_H {
	Vector3D ori;
	Vector3D dir;
}Ray;




/// <summary>
/// returns a point corresponding to the intersect between a ray and plane
/// </summary>
/// <param name="ray">The Ray</param>
/// <param name="plane">The Plane</param>
/// <returns>The 3D world point</returns>
Vector3D rayPlaneIntersect(Ray ray, Plane3D plane);

Ray newRay(Vector3D origin, Vector3D direction);

Vector3D planePointY(Plane3D plane);

Vector3D unprojToPos(Vector3D camPos, Vector3D dir, float depth);


//Vector3D rayToPointFromDistance(Ray ray, float dist);

