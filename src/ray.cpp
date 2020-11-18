#include "ray.h"

bool Ray::setParameter(const float par)
{
	if(par < t && par > SMALLEST_DIST)
	{
		hit = true;
		t = par;
		return true;
	}
	return false;
}
