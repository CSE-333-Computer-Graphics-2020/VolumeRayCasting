
#include "volume.h"

#include <iostream>

int clip(int n, int lower, int upper);

bool fileExists(string fileName)
{
	ifstream infile(fileName.c_str());

	if (infile.good())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Volume::firstIntersect(Ray &r) const{
	Vector3D o = r.getOrigin();
	Vector3D d = r.getDirection();

	//(camera : XDIM/2,YDIM/2,ZDIM + 300)
	// double t = (ZDIM - o.Z())/d.Z();
	// Vector3D p = o + (t*d);
	// if(0<p.X() && p.X()<XDIM && 0<p.Y() && p.Y()<YDIM)
	// {
	// 	r.t = t;
	// 	r.hit = true;
	// }

	//(camera: XDIM + 300, YDIM/2,ZDIM)
	double t = (XDIM - o.X())/d.X();
	Vector3D p = o + (t*d);
	if(0<p.Y() && p.Y()<YDIM && 0<p.Z() && p.Z()<ZDIM)
	{
		r.t = t;
		r.hit = true;
	}
	return ;
}
bool inbet(double a,double b,double c)
{
	return a<b && b<c;
}
bool Volume::nextIntersect(Ray &r) const {
	Vector3D o = r.getOrigin();
	Vector3D d = r.getDirection();
	Vector3D p = o + (r.t+DT)*d;

	if( inbet(0,p.X(),XDIM) && inbet(0,p.Y(),YDIM) && inbet(0,p.Z(),ZDIM) )
	{
		r.t = (r.t + DT);
		r.hit = true;
		return true;
	}
	else
	{
		r.hit = false;
		return false;
	}
}
glm::vec4 Volume::getNextIntersectColor(const Ray &r) const{
	Vector3D p = r.getPosition();
	return getVortexColor(p.X(),p.Y(),p.Z());
}


void Volume::showVolume(int z)
{
	for (int x=0;x<XDIM;x++){
		for (int y=0;y<YDIM;y++)
			if(sampleVolume(x,y,z)>10)
				std::cout<<sampleVolume(x,y,z)<<" ";
		std::cout<<std::endl;
	}
}

bool Volume::loadVolume(string volume_file)
{
	ifstream infile(volume_file.c_str(), ios_base::binary);
	if (infile.good())
	{
		//read the volume data file
		pData = new GLubyte[XDIM*YDIM*ZDIM];
		infile.read(reinterpret_cast<char*>(pData), XDIM*YDIM*ZDIM * sizeof(GLubyte));
		infile.close();

		scalars.resize(XDIM*YDIM*ZDIM);
		gradients.resize(XDIM*YDIM*ZDIM);

		for (int i = 0; i < scalars.size(); i++)
		{
			scalars[i] = (float)pData[i];
		}
		return true;
	}
	return false;
}

void Volume::generateGradients(int sampleSize,string fileName)
{
	int n = sampleSize;
	glm::vec3 normal = glm::vec3(0.0f);
	glm::vec3 s1, s2;

	int index = 0;
	for (int z = 0; z < ZDIM; z++)
	{
		for (int y = 0; y < YDIM; y++)
		{
			for (int x = 0; x < XDIM; x++)
			{
				s1.x = sampleVolume(x - n, y, z);
				s2.x = sampleVolume(x + n, y, z);
				s1.y = sampleVolume(x, y - n, z);
				s2.y = sampleVolume(x, y + n, z);
				s1.z = sampleVolume(x, y, z - n);
				s2.z = sampleVolume(x, y, z + n);

				gradients[index++] = glm::normalize(s2 - s1);
				if (isnan(gradients[index - 1].x))
				{
					gradients[index - 1] = glm::vec3(0.0f);
				}
			}
		}
	}

	filterNxNxN(3);

	//Now save the gradient
	ofstream fout;
	fout.open(fileName, ios_base::binary);

	glm::vec3 *temp = &(gradients[0]);

	fout.write(reinterpret_cast<char*>(temp), XDIM*YDIM*ZDIM *  sizeof(glm::vec3));
	fout.close();
	cout << "Gradients saved into file\n";
}

void Volume::readGradientsFromFile(string fileName)
{
	//Load the gradients
	ifstream fin;
	fin.open(fileName, ios_base::binary);
	
	glm::vec3 *temp = new glm::vec3[XDIM*YDIM*ZDIM];
	
	fin.read(reinterpret_cast<char*>(temp), XDIM*YDIM*ZDIM * sizeof(glm::vec3));
	fin.close();
	
	gradients = vector<glm::vec3>(temp, temp + XDIM * YDIM*ZDIM);

	cout << "Gradients loaded from file\n";
}



int Volume::sampleVolume(int x, int y, int z)
{
	x = (int)clip(x, 0, XDIM - 1);
	y = (int)clip(y, 0, YDIM - 1);
	z = (int)clip(z, 0, ZDIM - 1);
	return scalars[x + (y * XDIM) + (z * XDIM * YDIM)];
}
glm::vec3 Volume::sampleGradients(int x, int y, int z)
{
	return gradients[x + (y * XDIM) + (z * XDIM * YDIM)];
}
bool Volume::isInBounds(int x, int y, int z) const
{
	return ((x >= 0 && x < XDIM) && (y >= 0 && y < YDIM) && (z >= 0 && z < ZDIM));
}
//filter the gradients with an NxNxN box filter
//Should be an odd number of samples. 3 used by default.
void Volume::filterNxNxN(int n)
{
	int index = 0;
	for (int z = 0; z < ZDIM; z++)
	{
		for (int y = 0; y < YDIM; y++)
		{
			for (int x = 0; x < XDIM; x++)
			{
				gradients[index++] = sampleNxNxN(x, y, z, n);
			}
		}
	}
}

glm::vec3 Volume::sampleNxNxN(int x, int y, int z, int n)
{
	n = (n - 1) / 2;
	glm::vec3 avg = glm::vec3(0.0f);
	int num = 0;

	for (int k = z - n; k <= z + n; k++)
	{
		for (int j = y - n; j <= y + n; j++)
		{
			for (int i = x - n; i <= x + n; i++)
			{
				if (isInBounds(i, j, k))
				{
					avg += sampleGradients(i, j, k);
					num++;
				}
			}
		}
	}
	avg /= (float)num;

	if (avg != glm::vec3(0.0f))
	{
		glm::normalize(avg);
	}

	return avg;
}

int clip(int n, int lower, int upper)
{
	return std::max(lower, std::min(n, upper));
}
