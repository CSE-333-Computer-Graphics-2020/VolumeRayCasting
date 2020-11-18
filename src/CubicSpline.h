#ifndef _CUBIC_SPLINE_H_
#define _CUBIC_SPLINE_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TransferFunctionControlPoint.h"
#include "CubicSplineSegment.h"

std::vector<CubicSplineSegment> CalculateCubicSpline(std::vector<TransferFunctionControlPoint> v);

#endif
