#ifndef _WORLD_H_
#define _WORLD_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> //glm::to_string

#include "ray.h"
#include "vector3D.h"
#include <cstring>	//memcpy
#include "color.h"
class World;
class Volume;
#include "volume.h"
#include "lightsource.h"
#include "color.h"
#include "ray.h"


#define MODE 'AVG'

class World
{
private:
	Color background; //Background color to shade rays that miss all objects
	Volume *volume;
	float BRIGHTNESS=8.0;
	unsigned int SAMPLES = 100;

public:
	LightSource* lightSource;
	World():
	 background(0)
	{}
	void setBackground(const Color& bk) { background = bk;}
	void setBrightness(const float& brt) { BRIGHTNESS = brt;}
	Color getbackgeound() { return background;}
	void setLight(LightSource* ls)
	{
		lightSource = ls;
	}

	void setVolume(Volume *vol)
	{
		this->volume = vol;
	}
	Color cast_ray(Ray& ray);
};
#endif
