#ifndef _RENDER_OBJECT_H_
#define _RENDER_OBJECT_H_

#include <glm/glm.hpp>
#include <GL/gl.h>

class World;
#include "world.h"
#include "ray.h"

#include <iostream>
class RenderObject;

class RenderObject
{
protected:
	//https://www.linuxquestions.org/questions/programming-9/glubyte-definition-591103/
	World *world;

public:

	RenderObject(World *_world)
	{
		this->world = _world;
	}
	virtual void firstIntersect(Ray &r) const;
	virtual bool nextIntersect(Ray &r) const;
	virtual void getNextIntersectColor(Ray &r) const;
};
#endif
