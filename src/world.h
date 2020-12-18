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
#include "color.h"
#include "ray.h"
#include "camera.h"



class World
{
private:
	Color background; //Background color to shade rays that miss all objects
	Volume *volume;
	float BRIGHTNESS=8.0f;
	float shininess=3.0f;
	float spc_cntr = 0.5f;
	unsigned int SAMPLES = 100;
	/*
	assembling method :
	1 - average
	2 - compositing
	3 - compositing + shading
	*/
	unsigned int ASEMBLE_MODE = 3;


public:
	Camera *camera;
	glm::vec3 lightVector = glm::normalize(glm::vec3(-1.0f,1.0f,0.0f));
	World():
	 background(0)
	{}
	void setBackground(const Color& bk) { background = bk;}
	void setBrightness(const float& brt) { BRIGHTNESS = brt;}
	void setShininess(const float& shininess) { this->shininess = shininess; }
	Color getbackgeound() { return background;}
	void setLight(glm::vec3 lightDirection)
	{
		lightVector = glm::normalize(lightDirection) ;
	}
	void setCamera(Camera *camera)
	{
		this->camera = camera;
	}

	void setVolume(Volume *vol)
	{
		this->volume = vol;
	}
	void setSamples(unsigned int SAMPLES)
	{
		this->SAMPLES = SAMPLES;
	}
	void setAssembling(unsigned int ASEMBLE_MODE)
	{
		this->ASEMBLE_MODE = ASEMBLE_MODE;
	}
	void setSpec(const float spc_cntr)
	{
		this->spc_cntr = spc_cntr;
	}
	Color cast_ray(Ray& ray);
};
#endif
