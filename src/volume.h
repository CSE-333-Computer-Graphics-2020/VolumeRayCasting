#ifndef _VOLUME_H_
#define _VOLUME_H_

#include <glm/glm.hpp>
#include <GL/gl.h>
#include <glm/gtx/intersect.hpp>

#include "../depends/stb/stb_image.h"

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
bool has_suffix(const std::string &str, const std::string &suffix);

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
	/*
	Value (Color (RGBA) and gradient) at a location of the grid is computed as :
	1 - for trilinear interpolation with nearest 8 neighbors for the value
	2 - for the nearest neighbor
	*/
	unsigned int INTER_MODE = 1;
	/*
	Scalar Filter Size
	*/
	unsigned int S_FILTER = 1;	
	/*
	Gradient Computation Filter Size
	*/
	unsigned int G_FILTER = 5;
	/*
	Mode of Transformation 
	1 - (Voxel Value, Voxel Value) -> (RGB, A)
	2 - (Voxel Value, Gradient Magnitude) -> (RGB, A)
	*/
	unsigned int TRN_MODE = 1;

	Face faces[6];
	//https://www.linuxquestions.org/questions/programming-9/glubyte-definition-591103/
public:
	TransformColor *transformer;
	int XDIM=128;
	int YDIM=256;
	int ZDIM=256;

	string volume_file;

	Volume(TransformColor *_transformer, const string _volume_file,const int slices, const int _S_FILTER=1):
		transformer(_transformer), volume_file(_volume_file), S_FILTER(_S_FILTER)
	{
		std::cout<<"Loading Volume..."<<std::endl;
	    if (! ( ( has_suffix(volume_file,".png") && loadVolumePNG(volume_file,slices) ) || loadVolume(volume_file) ) )
	    {
	        cout << "Volume Loading Error!" << endl;
	    }
	    std::cout<<"Finish Loading Volume!"<<std::endl;
	    std::cout<<"Creating AABB..."<<std::endl;
	    setAABB();
	    std::cout<<"Finish AABB..."<<std::endl;
	    // showVolume(100);
	}

	void setInterpolation(unsigned int INTER_MODE)
	{
		this->INTER_MODE = INTER_MODE;
	}
	void setGFilterSize(unsigned int G_FILTER)
	{
		this->G_FILTER = G_FILTER;
	}
	void setTransformationDimension(unsigned int TRN_MODE)
	{
		this->TRN_MODE = TRN_MODE;
	}
	void genGradient(bool gen_gradient=false)
	{
		string g_file = volume_file+'_'+std::to_string(G_FILTER)+".grad";
	    if(!gen_gradient && fileExists(g_file))
			readGradientsFromFile(g_file);
		else{
			std::cout<<"Generating Gradient..."<<std::endl;
			generateGradients(1,g_file);
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
/*
		color alpha
OPT = 0 : s    s
OPT = 1 : s    g
*/

	glm::vec4 getVortexColor(int x,int y,int z) const
	{
		if(!isInBounds(x,y,z))
			return glm::vec4(0.0);
		if(TRN_MODE == 1)
		{
			float scaleVal = scalars[x + (y*XDIM) + (z*XDIM*YDIM)];
			return this->transformer->getTransformedColor(scaleVal,scaleVal);			
		}
		else
		{
			float scaleVal = scalars[x + (y*XDIM) + (z*XDIM*YDIM)];
			glm::vec3 grd = gradients[ x + (y*XDIM) + (z*XDIM*YDIM) ];
			int gradVal =  glm::clamp(glm::length(grd),0.0f,1.0f)*256;
			return this->transformer->getTransformedColor(scaleVal,gradVal);						
		}

	}
	glm::vec3 getVortexGradient(int x,int y,int z) const
	{
		if(!isInBounds(x,y,z))
			return glm::vec3(0.0);
		return gradients[ x + (y*XDIM) + (z*XDIM*YDIM) ];;		
	}

	// RenderObject function
	void firstIntersect(Ray &r) const;
	bool nextIntersect(Ray &r) const;
	glm::vec4 getNextIntersectColor(const Ray &r) const;
	glm::vec3 getNextIntersectionGradient(const Ray &r) const;
	int getNextIntersectionScalar(const Ray &r) const;

	bool loadVolume(string volume_file);
	bool loadVolumePNG(string volume_file,int slices);
	void filterVolumeNxNxN(int n);
	float sampleVolumeNxNxN(int x, int y, int z, int n);
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
