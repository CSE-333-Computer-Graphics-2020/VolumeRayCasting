#ifndef _VOLUME_H_
#define _VOLUME_H_

#include <glm/glm.hpp>
#include <GL/gl.h>
#include <glm/gtx/intersect.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <fstream>  //ifstream
#include <cstring>	//memcpy
#include <float.h>	//FLT_MAX
class LightSource;
#include "lightsource.h"
#include "ray.h"
#include "transformcolor.h"
#include "vector3D.h"

using namespace std;

bool fileExists(string fileName);

#include <iostream>

const double DT=1.5;

class Face
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
	Face()
	{}
	//v0 and v3 oposite, v1 and v2 are oposite
	Face(glm::vec3 _v0,glm::vec3 _v1,glm::vec3 _v2,glm::vec3 _v3):
		v0(_v0), v1(_v1), v2(_v2), v3(_v3)
	{}
	bool rayFaceIntersect(glm::vec3 o,glm::vec3 d,float &t)
	{
		glm::vec2 beri;
		if(glm::intersectRayTriangle(o,d,v0,v1,v2,beri,t))
			return true;
		if(glm::intersectRayTriangle(o,d,v2,v1,v3,beri,t))
			return true;
		return false;
	}
};

class Volume
{
protected:
	//https://www.linuxquestions.org/questions/programming-9/glubyte-definition-591103/
	GLubyte* pData;
	vector<float> scalars;
	vector<glm::vec3> gradients;

	Face faces[6];
	//https://www.linuxquestions.org/questions/programming-9/glubyte-definition-591103/
public:
	TransformColor *transformer;
	const int XDIM=128;
	const int YDIM=256;
	const int ZDIM=256;

	Volume(TransformColor *_transformer, const string volume_file,bool gen_gradient=false):
		transformer(_transformer)
	{
		std::cout<<"Loading Volume..."<<std::endl;
	    if (!loadVolume(volume_file))
	    {
	        cout << "Volume Loading Error!" << endl;
	    }
	    std::cout<<"Finish Loading Volume!"<<std::endl;
	    std::cout<<"Creating AABB..."<<std::endl;
	    setAABB();
	    std::cout<<"Finish AABB..."<<std::endl;
	    // showVolume(100);
	    if(!gen_gradient && fileExists(volume_file+".grad"))
			readGradientsFromFile(volume_file+".grad");
		else{
			std::cout<<"Generating Gradient..."<<std::endl;
			generateGradients(1,volume_file+".grad");
			std::cout<<"Finish Generating Gradient"<<std::endl;
		}

	}

	void setAABB()
	{
	    glm::vec3 vertices[8];
	    vertices[0] = glm::vec3(0	,0		,0);
	    vertices[1] = glm::vec3(XDIM,0		,0);
	    vertices[2] = glm::vec3(XDIM,YDIM	,0);
	    vertices[3] = glm::vec3(0	,YDIM	,0);
	    vertices[4] = glm::vec3(0	,0		,ZDIM);
	    vertices[5] = glm::vec3(XDIM,0		,ZDIM);
	    vertices[6] = glm::vec3(XDIM,YDIM	,ZDIM);
	    vertices[7] = glm::vec3(0	,YDIM	,ZDIM);
	    //0-2 , 1-3, z=0 
	    //4-6 , 5-7, z=ZDIM
	    //1-6, 2-5,  x=XDIM
	    //3-6, 2-7,  y=YDIM
	    //0-7, 3-4,    x=0
	    //0-5, 1-4,	   y=0

	    faces[0] = Face(vertices[0],vertices[1],vertices[3],vertices[2]);
	    faces[1] = Face(vertices[4],vertices[5],vertices[7],vertices[6]);
	    faces[2] = Face(vertices[1],vertices[2],vertices[5],vertices[6]);
	    faces[3] = Face(vertices[3],vertices[2],vertices[7],vertices[6]);
	    faces[4] = Face(vertices[0],vertices[3],vertices[4],vertices[7]);
	    faces[5] = Face(vertices[0],vertices[1],vertices[4],vertices[5]);
	}

	bool aabbIntersectFront(Ray &r,float &t)
	{
		Vector3D ov = r.getOrigin();
		Vector3D dv = r.getDirection();
		glm::vec3 o(ov.X(),ov.Y(),ov.Z());
		glm::vec3 d(dv.X(),dv.Y(),dv.Z());
		r.hit = aabbIntersectFront(o,d,t);
		return r.hit;
	}
	bool aabbIntersectBack(Ray &r,float &t)
	{
		Vector3D ov = r.getOrigin();
		Vector3D dv = r.getDirection();
		glm::vec3 o(ov.X(),ov.Y(),ov.Z());
		glm::vec3 d(dv.X(),dv.Y(),dv.Z());
		r.hit = aabbIntersectBack(o,d,t);
		return r.hit;
	}
	bool aabbIntersectFront(const glm::vec3 &o,const glm::vec3 &d,float &t)
	{
		float tmin = FLT_MAX;
		bool hit = false;
		for(int i=0;i<6;i++){
			float tm;
			if(faces[i].rayFaceIntersect(o,d,tm)){
				tmin = std::min(tm,tmin);
				hit=true;
			}
		}
		if(hit)
			t = tmin;
		return hit;
	}
	bool aabbIntersectBack(const glm::vec3 &o,const glm::vec3 &d,float &t)
	{
		float tmax = FLT_MIN;
		bool hit = false;
		for(int i=0;i<6;i++){
			float tm;
			if(faces[i].rayFaceIntersect(o,d,tm)){
				tmax = std::max(tm,tmax);
				hit=true;
			}
		}
		if(hit)
			t = tmax;
		return hit;
	}
	glm::vec4 getVortexColor(int x,int y,int z) const
	{
		if(!isInBounds(x,y,z))
			return glm::vec4(0.0);
		float scaleVal = scalars[x + (y*XDIM) + (z*XDIM*YDIM)];
		glm::vec3 grd = gradients[ x + (y*XDIM) + (z*XDIM*YDIM) ];

//		int gradVal =  scalars[x + (y*XDIM) + (z*XDIM*YDIM)]; //needs to change to gradient magnitude
		int gradVal =  glm::length(grd)*256;
		 return this->transformer->getTransformedColor(scaleVal,gradVal);
	}
	glm::vec3 getVortexGradient(int x,int y,int z) const
	{
		return gradients[ x + (y*XDIM) + (z*XDIM*YDIM) ];;		
	}

	// RenderObject function
	void firstIntersect(Ray &r) const;
	bool nextIntersect(Ray &r) const;
	glm::vec4 getNextIntersectColor(const Ray &r) const;
	glm::vec3 getNextIntersectionGradient(const Ray &r) const;
	int getNextIntersectionScalar(const Ray &r) const;

	bool loadVolume(string volume_file);
	void generateGradients(int sampleSize,string fileName);
	void readGradientsFromFile(string fileName);
	int sampleVolume(int x, int y, int z) const;
	glm::vec3 sampleGradients(int x, int y, int z);
	bool isInBounds(int x, int y, int z) const;
	void filterNxNxN(int n);
	glm::vec3 sampleNxNxN(int x, int y, int z, int n);
	void showVolume(int z);
};

#endif
