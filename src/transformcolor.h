#ifndef _TRANSFORM_COLOR_H_
#define _TRANSFORM_COLOR_H_

#include <glm/glm.hpp>
#include <GL/gl.h>
#include <glm/gtx/string_cast.hpp> //glm::to_string

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

				colorTransform[numTF++] = glm::vec3( colorCubicSpline[i].GetPointOnSpline(k) ) ;
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

				alphaTransform[numTF++] = alphaCubicSpline[i].GetPointOnSpline(k).w;
			}
		}

	}

	glm::vec4 getTransformedColor(const int scalVal, const int gradVal)
	{
		return glm::vec4(colorTransform[scalVal],alphaTransform[gradVal]);
	}

};
#endif
