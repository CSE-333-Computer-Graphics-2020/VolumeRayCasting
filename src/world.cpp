#include "world.h"

using namespace std;

#include <iostream>

Color World::cast_ray(Ray& ray)
{
	glm::vec3 color(0.0);
	this->volume->firstIntersect(ray);
	Vector3D p = ray.getPosition();
	// if(ray.hit)
	// 	std::cout<<p.X()<<", "<<p.Y()<<", "<<p.Z()<<std::endl;
	// if(ray.hit)
	// 	std::cout<<"t:"<<ray.t<<" "<<p.X()<<", "<<p.Y()<<", "<<p.Z()<<std::endl;

	int numSamp = 0;
	while(this->volume->nextIntersect(ray))
	{
		glm::vec4 c = this->volume->getNextIntersectColor(ray);
//		std::cout<<glm::to_string(c)<<std::endl;
		color = color+(glm::vec3(c[0],c[1],c[2]));
//		std::cout<<glm::to_string(color)<<std::endl;
		//----------
		//----------
//		p = ray.getPosition();
//		std::cout<<"in:t:"<<ray.t<<" "<<p.X()<<", "<<p.Y()<<", "<<p.Z()<<std::endl;
		numSamp++;
	}
//	std::cout<<glm::to_string(color)<<std::endl;
	if(numSamp!=0)
		color = color/(float)numSamp;
	return Color(color[0],color[1],color[2]);
}
