#include "ray.h"

Vector3D rayPlaneIntersect(Ray ray, Plane3D plane)
{
	Vector3D ret = vector3d(NAN,NAN,NAN), planeNormal, p0;
	float t = NAN;

	

	planeNormal = vector3d(plane.x, plane.y, plane.z);
	p0 = planePointY(plane);




	float denom = vector3d_dot_product(planeNormal, ray.dir);
	if (denom > 1e-6)
	{
		Vector3D p0l0;
		vector3d_sub(p0l0, p0, ray.ori);
		t = vector3d_dot_product(p0l0, planeNormal)/denom;
		t = t >= 0 ? t : NAN;
		 
	}

	if(!isnan(t))
	{
		vector3d_scale(ret, ray.dir, t);
		vector3d_add(ret, ret, ray.ori);
		printf("t validation: %f\n", t);
	}
	return ret;
}

Vector3D unprojToPos(Vector3D camPos, Vector3D dir, float depth)
{
	Ray ray = newRay(camPos, dir);
	Plane3D plane; //todo set plane parallel to camera's close/far plane and adjust based on depth
	plane.x = 0;
	plane.y = 1;
	plane.z = 0;
	plane.d = depth;

	return rayPlaneIntersect(ray, plane);

}


Ray newRay(Vector3D origin, Vector3D direction)
{
	Ray ret;
	ret.ori = origin;
	ret.dir = direction;
	return ret;
}

Vector3D planePointY(Plane3D plane)
{
	return vector3d(0,plane.d / plane.y, 0);
}
