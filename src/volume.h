#ifndef _VOLUME_H_
#define _VOLUME_H_

#include <glm/glm.hpp>
#include <GL/gl.h>

#include <vector>
#include <string>
#include <sstream>
#include <fstream>  //ifstream
#include <cstring>	//memcpy
class LightSource;
#include "lightsource.h"
#include "ray.h"
#include "transformcolor.h"
#include "vector3D.h"

using namespace std;

bool fileExists(string fileName);

#include <iostream>

const double DT=1.5;

class Volume
{
protected:
	//https://www.linuxquestions.org/questions/programming-9/glubyte-definition-591103/
	GLubyte* pData;
	vector<float> scalars;
	vector<glm::vec3> gradients;

	//https://www.linuxquestions.org/questions/programming-9/glubyte-definition-591103/
	TransformColor *transformer;
public:
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
	    // showVolume(100);
	    if(!gen_gradient && fileExists(volume_file+".grad"))
			readGradientsFromFile(volume_file+".grad");
		else{
			std::cout<<"Generating Gradient..."<<std::endl;
			generateGradients(1,volume_file+".grad");
			std::cout<<"Finish Generating Gradient"<<std::endl;
		}

	}

	glm::vec4 getVortexColor(int x,int y,int z) const
	{
		if(!isInBounds(x,y,z))
			return glm::vec4(0.0);
		float scaleVal = scalars[x + (y*XDIM) + (z*XDIM*YDIM)];
		int gradVal =  scalars[x + (y*XDIM) + (z*XDIM*YDIM)]; //needs to change to gradient magnitude
		 return this->transformer->getTransformedColor(scaleVal,gradVal);
	}

	// RenderObject function
	void firstIntersect(Ray &r) const;
	bool nextIntersect(Ray &r) const;
	glm::vec4 getNextIntersectColor(const Ray &r) const;

	bool loadVolume(string volume_file);
	void generateGradients(int sampleSize,string fileName);
	void readGradientsFromFile(string fileName);
	int sampleVolume(int x, int y, int z);
	glm::vec3 sampleGradients(int x, int y, int z);
	bool isInBounds(int x, int y, int z) const;
	void filterNxNxN(int n);
	glm::vec3 sampleNxNxN(int x, int y, int z, int n);
	void showVolume(int z);
};

#endif
