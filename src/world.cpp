#include "world.h"

using namespace std;

glm::vec3 avgShader(Ray &ray,Volume *volume,int SAMPLES);
glm::vec3 compositeShader1D(Ray &ray,Volume *volume,int SAMPLES);
glm::vec3 compositeShader2D(Ray &ray,Volume *volume,int SAMPLES);

#include <iostream>
Color World::cast_ray(Ray& ray)
{
//	glm::vec3 color = avgShader(ray,this->volume,this->SAMPLES);
	glm::vec3 color = compositeShader1D(ray,this->volume,this->SAMPLES);
//	glm::vec3 color = compositeShader2D(ray,this->volume,this->SAMPLES);
	return Color(color[0],color[1],color[2])*this->BRIGHTNESS;
}

glm::vec3 avgShader(Ray &ray,Volume *volume,int SAMPLES)
{
	glm::vec3 color(0.0);
	float a=0.0;
	int numSamp = 0;
	float t0;
	float t1;
	bool hit0 = volume->aabbIntersectFront(ray,t0);
	bool hit1 = volume->aabbIntersectBack(ray,t1);
	float sampleLength = (t1-t0)/SAMPLES;

	ray.t = t0;
	ray.hit = hit0 & hit1;
	for(int i=0;i<SAMPLES && ray.hit;i++)
	{
		int sv = volume->getNextIntersectionScalar(ray);
		glm::vec4 c = volume->transformer->getTransformedColor(sv,sv);

		color = color+(glm::vec3(c[0],c[1],c[2])*c[3]);
		ray.t = ray.t + sampleLength;
	}
	if(ray.hit)
		color = color/(float)SAMPLES;	
	return color;

}

glm::vec3 compositeShader1D(Ray &ray,Volume *volume,int SAMPLES)
{
	glm::vec3 color(0.0);
	float a=0.0;
	int numSamp = 0;
	float t0;
	float t1;
	bool hit0 = volume->aabbIntersectFront(ray,t0);
	bool hit1 = volume->aabbIntersectBack(ray,t1);
	float sampleLength = (t1-t0)/SAMPLES;

	ray.t = t0;
	ray.hit = hit0 & hit1;
	for(int i=0;i<SAMPLES && ray.hit;i++)
	{
		int sv = volume->getNextIntersectionScalar(ray);
		glm::vec4 c = volume->transformer->getTransformedColor(sv,sv);

		color = color+(1-a)*c[3]*glm::vec3(c);
		a = a + (1-a)*c[3];
		if(a >=0.99f)
		{
			a=1.0;
			break;
		}
		ray.t = ray.t + sampleLength;
	}
	return color;
}

glm::vec3 compositeShader2D(Ray &ray,Volume *volume,int SAMPLES)
{
	glm::vec3 color(0.0);
	float a=0.0;
	int numSamp = 0;
	float t0;
	float t1;
	bool hit0 = volume->aabbIntersectFront(ray,t0);
	bool hit1 = volume->aabbIntersectBack(ray,t1);
	float sampleLength = (t1-t0)/SAMPLES;

	ray.t = t0;
	ray.hit = hit0 & hit1;
	for(int i=0;i<SAMPLES && ray.hit;i++)
	{
		int sv = volume->getNextIntersectionScalar(ray);
		int grd = glm::clamp(glm::length( volume->getNextIntersectionGradient(ray) ), 0.0f, 1.0f)*256;
		glm::vec4 c = volume->transformer->getTransformedColor(sv,grd);
		color = color+(1-a)*c[3]*glm::vec3(c);
		a = a + (1-a)*c[3];
		if(a >=0.99f)
		{
			a=1.0;
			break;
		}
		ray.t = ray.t + sampleLength;
	}
	return color;
}
