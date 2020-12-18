#include "world.h"

using namespace std;

glm::vec3 avgShader(Ray &ray,Volume *volume,int SAMPLES);
glm::vec3 compositeShader(Ray &ray,Volume *volume,int SAMPLES);
glm::vec3 compositeShaderPhong(Ray &ray,Volume *volume,const Vector3D &vposition,const glm::vec3 &lightVector,const float &shininess,const float &ks,int SAMPLES);
// glm::vec3 compositeShader2D(Ray &ray,Volume *volume,int SAMPLES);

#include <iostream>
Color World::cast_ray(Ray& ray)
{
	glm::vec3 color;
	if(ASEMBLE_MODE==1)
	{
		color = avgShader(ray,this->volume,this->SAMPLES);		
	}
	else if(ASEMBLE_MODE==2)
	{
		color = compositeShader(ray,this->volume,this->SAMPLES);		
	}
	else
	{
		color = compositeShaderPhong(ray,this->volume,camera->get_position(),lightVector,shininess,spc_cntr,this->SAMPLES);
	}
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
		glm::vec4 c = volume->getNextIntersectColor(ray);

		color = color+(glm::vec3(c[0],c[1],c[2])*c[3]);
		ray.t = ray.t + sampleLength;
	}
	if(ray.hit)
		color = color/(float)SAMPLES;	
	return color;

}

glm::vec3 compositeShader(Ray &ray,Volume *volume,int SAMPLES)
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
		// int sv = volume->getNextIntersectionScalar(ray);
		// glm::vec4 c = volume->transformer->getTransformedColor(sv,sv);
		glm::vec4 c = volume->getNextIntersectColor(ray);

		color = color+(1-a)*c[3]*glm::vec3(c);
		// color = color+(1-a)*glm::vec3(c);
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

// glm::vec4 direction = glm::normalize(glm::vec4(88.0f,88.0f,88.0f,1.0f) - glm::vec4(-112.0f ,-20.0f ,288.0f, 1.0f));

glm::vec3 compositeShaderPhong(Ray &ray,Volume *volume,const Vector3D &vposition,const glm::vec3 &lightVector,const float &shininess,const float &ks,int SAMPLES)
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
		glm::vec4 c = volume->getNextIntersectColor(ray);
		glm::vec3 normal =  volume->getNextIntersectionGradient(ray);
		if(normal!=glm::vec3(0.0f))
			normal = glm::normalize(normal);
		// std::cout<<glm::length(normal)<<std::endl;
		Vector3D vdirection = vposition - ray.getPosition();
		glm::vec3 direction = glm::normalize(glm::vec3(vdirection.X(),vdirection.Y(),vdirection.Z()));


		float directional = glm::clamp(glm::dot(normal, lightVector), 0.0f, 1.0f);
		float specular =  glm::pow( glm::max( glm::dot(glm::reflect(lightVector, normal), direction), 0.0f), shininess) ;

		float tempa = c[3];
		c = 0.95f*(directional)*c + 0.05f*c + ks*specular*glm::vec4(1.0f);
		// c = 0.95f*(directional)*c + 0.05f*c;
		// c = (directional)*c;
		c[3] = tempa;

		// color = color+(1-a)*c[3]*glm::vec3(c);
		// a = a + (1-a)*c[3];
		color = color + (1-a)*c[3]* glm::vec3(c);
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


// glm::vec3 compositeShader2D(Ray &ray,Volume *volume,int SAMPLES)
// {
// 	glm::vec3 color(0.0);
// 	float a=0.0;
// 	int numSamp = 0;
// 	float t0;
// 	float t1;
// 	bool hit0 = volume->aabbIntersectFront(ray,t0);
// 	bool hit1 = volume->aabbIntersectBack(ray,t1);
// 	float sampleLength = (t1-t0)/SAMPLES;

// 	ray.t = t0;
// 	ray.hit = hit0 & hit1;
// 	for(int i=0;i<SAMPLES && ray.hit;i++)
// 	{
// 		int sv = volume->getNextIntersectionScalar(ray);
// 		int grd = glm::clamp(glm::length( volume->getNextIntersectionGradient(ray) ), 0.0f, 1.0f)*256;
// 		glm::vec4 c = volume->transformer->getTransformedColor(sv,grd);
// 		color = color+(1-a)*c[3]*glm::vec3(c);
// 		a = a + (1-a)*c[3];
// 		if(a >=0.99f)
// 		{
// 			a=1.0;
// 			break;
// 		}
// 		ray.t = ray.t + sampleLength;
// 	}
// 	return color;
// }
