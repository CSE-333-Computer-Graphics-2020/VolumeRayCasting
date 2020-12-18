#ifndef _TRANSFORM_COLOR_H_
#define _TRANSFORM_COLOR_H_

#include <glm/glm.hpp>
#include <GL/gl.h>
#include <glm/gtx/string_cast.hpp> //glm::to_string

#include <string>
#include <fstream>

#include "ray.h"
#include "CubicSpline.h"
#include "TransferFunctionControlPoint.h"
#include "CubicSplineSegment.h"

#include <iostream>
using namespace std;

class TransformColor
{
protected:
	glm::vec3 colorTransform[256];
	double alphaTransform[256];

public:
	TransformColor() {}
	TransformColor(string colorFile, string alphaFile) 
	{
		fstream fin;
		fin.open(colorFile.c_str(), ios::in);
		if (fin.fail())
		{
		    cout << "Can't open color transform file" << endl;
		}
		float r,g,b;
		int d;
		vector<TransferFunctionControlPoint> colorKnots;
		while (fin >> r >> g >> b >> d)
		{
			colorKnots.push_back(TransferFunctionControlPoint(r, g, b, d));
		}
		fin.close();

		fin.open(alphaFile.c_str(), ios::in);
		if (fin.fail())
		{
		    cout << "Can't open alpha transform file" << endl;
		}
		float a;
		vector<TransferFunctionControlPoint> alphaKnots;
		while (fin >> a >> d)
		{
			std::cout<<a<<", "<<d<<std::endl;
			alphaKnots.push_back(TransferFunctionControlPoint(a, d));
		}
		computeTransferFunction(colorKnots,alphaKnots);
	}
	void printColorTransform()
	{
		int len = sizeof(colorTransform)/sizeof(glm::vec3);
		for(int i=0;i<len;i++)
			std::cout<<i<<": "<<glm::to_string(colorTransform[i])<<std::endl;
	}
	void printAlphaTransform()
	{
		int len = sizeof(alphaTransform)/sizeof(double);
		for(int i=0;i<len;i++)
			std::cout<<i<<": "<<alphaTransform[i]<<std::endl;
	}
	void computeTransferFunction(const vector<TransferFunctionControlPoint> &colorKnots, const vector<TransferFunctionControlPoint> &alphaKnots)
	{
		glm::vec4 transferFunc[256];

		vector<CubicSplineSegment> colorCubicSpline = CalculateCubicSpline(colorKnots);
		vector<CubicSplineSegment> alphaCubicSpline = CalculateCubicSpline(alphaKnots);

		int numTF = 0; //Each isoVal from 0 to 255 will be mapped to a color and alpha using transfer func

		for (int i = 0; i < colorKnots.size() - 1; i++)
		{
			int steps = colorKnots[i + 1].isoValue - colorKnots[i].isoValue;

			for (int j = 0; j < steps; j++)
			{
				float k = (float)j / (float)(steps - 1);
				glm::vec3 temp( colorCubicSpline[i].GetPointOnSpline(k) );
				temp[0] = glm::clamp(temp[0],0.0f,1.0f); 
				temp[1] = glm::clamp(temp[1],0.0f,1.0f); 
				temp[2] = glm::clamp(temp[2],0.0f,1.0f); 
				colorTransform[numTF++] = temp;
			}
		}

		numTF = 0; //Each isoVal from 0 to 255 will be mapped to a color and alpha using transfer func

		//needs to change to change range ofvalues to 0-1
		for (int i = 0; i < alphaKnots.size() - 1; i++)
		{
			int steps = alphaKnots[i + 1].isoValue - alphaKnots[i].isoValue;

			for (int j = 0; j < steps; j++)
			{
				float k = (float)j / (float)(steps - 1);

				double temp = glm::clamp( alphaCubicSpline[i].GetPointOnSpline(k).w,0.0f,1.0f);
				alphaTransform[numTF++] = temp;
			}
		}

	}

	glm::vec4 getTransformedColor(const int scalVal, const int gradVal)
	{
		return glm::vec4(colorTransform[scalVal],alphaTransform[gradVal]);
	}

};
#endif
