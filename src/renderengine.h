#ifndef _RENDERENGINE_H_
#define _RENDERENGINE_H_

#include "world.h"
#include "camera.h"
#include "helper.h"

#define SN 5
#define SAMPLING false

class RenderEngine
{
private:
	World *world;
	Camera *camera;
	const Color trace(const double i, const double j);

public:
	RenderEngine(World *_world, Camera *_camera):
		world(_world), camera(_camera) {}
	bool renderLoop();
};
#endif
