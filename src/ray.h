//ray.h
#ifndef _RAY_H_
#define _RAY_H_

#include <float.h>
#include "vector3D.h"

const double SMALLEST_DIST = 1e-5; //Constant used to dismiss intersections very close to previous
class Ray
{
private:
	//const Vector3D origin;
	Vector3D origin;
	Vector3D direction;
	Vector3D normal; //Normal of the hit object

public:  
	Ray(const Vector3D& o, const Vector3D& d):
    		origin(o), direction(d), t(FLT_MAX), hit(false)
	{
		direction.normalize();	
	}

	float t; //Distance travelled alogn the Ray
	bool hit; //has the ray hit something?
	
	void setOrigin(const Vector3D& _o) {this->origin = _o;}
	Vector3D getOrigin() const  {return origin;}
	Vector3D setDirection(const Vector3D& _d) {this->direction = _d;}
	Vector3D getDirection() const  {return direction;}
	Vector3D getPosition() const {return origin + t*direction;}
	Vector3D getNormal() const {return normal;}
	float getParameter() const {return t;}
	bool setParameter(const float par);
	bool didHit() const {return hit;}

};
#endif
