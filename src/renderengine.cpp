#include "renderengine.h"

#include <iostream>
const Color RenderEngine::trace(const double i, const double j)
{
	Vector3D ray_dir = camera->get_ray_direction(i, j);
	Ray ray(camera->get_position(), ray_dir);
//	std::cout<<"trace for pixel: "<<i<<", "<<j<<std::endl;
	return world->cast_ray(ray);
}

bool RenderEngine::renderLoop()
{
	static int i = 0;
	for(int j = 0; j<camera->getHeight(); j++)
	{
		if(!SAMPLING){
			Color color = trace(i, j);
			color.clamp();
			camera->drawPixel(i, j, color);
		}
		else
		{
			Color color(0);
			for(int p=0;p<SN;p++)
				for(int q=0;q<SN;q++)
				{
					double e = randGen(1);
					double x = i + ((p+e)/SN);
					double y = j + ((q+e)/SN);
					Color c = trace(x,y);
					c.clamp();
					color += c;
				}
			camera->drawPixel(i,j,color/(SN*SN));
		}
	}

	if(++i == camera->getWidth())
	{
		i = 0;
		return true;
	}
	return false;
}
