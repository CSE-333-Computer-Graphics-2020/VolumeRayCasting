
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

bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void Volume::firstIntersect(Ray &r) const{
	Vector3D o = r.getOrigin();
	Vector3D d = r.getDirection();
	double t = (XDIM - o.X())/d.X();
	Vector3D p = o + (t*d);
	if(0<p.Y() && p.Y()<YDIM && 0<p.Z() && p.Z()<ZDIM)
	{
		r.t = t;
		r.hit = true;
	}
	return ;
}

// std::pair<bool,float> Volume::firstIntersectT(Ray &r) const{
// 	Vector3D o = r.getOrigin();
// 	Vector3D d = r.getDirection();

// 	//(camera : XDIM/2,YDIM/2,ZDIM + 300)
// 	// double t = (ZDIM - o.Z())/d.Z();
// 	// Vector3D p = o + (t*d);
// 	// if(0<p.X() && p.X()<XDIM && 0<p.Y() && p.Y()<YDIM)
// 	// {
// 	// 	r.t = t;
// 	// 	r.hit = true;
// 	// }

// 	//(camera: XDIM + 300, YDIM/2,ZDIM)
// 	double t = (XDIM - o.X())/d.X();
// 	Vector3D p = o + (t*d);
// 	if(0<p.Y() && p.Y()<YDIM && 0<p.Z() && p.Z()<ZDIM)
// 	{
// 		return std::make_pair(true,t);
// 	}
// 	return std::make_pair(false,t);	
// }

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
/*
		color alpha
OPT = 0 : s    s
OPT = 1 : s    g
*/

glm::vec4 Volume::getNextIntersectColor(const Ray &r) const{
	Vector3D p = r.getPosition();

	if(INTER_MODE==1) //Trilinear Interpolation
	{
		int gxi = p.X();
		int gyi = p.Y();
		int gzi = p.Z();
		float tx = p.X() - gxi;
		float ty = p.Y() - gyi;
		float tz = p.Z() - gzi;

	// Lets try trilinear interpolation
		const glm::vec4 c000 = getVortexColor(gxi, gyi, gzi); 
		const glm::vec4 c100 = getVortexColor(gxi + 1, gyi, gzi); 
		const glm::vec4 c010 = getVortexColor(gxi, gyi + 1, gzi); 
		const glm::vec4 c110 = getVortexColor(gxi + 1, gyi + 1, gzi); 
		const glm::vec4 c001 = getVortexColor(gxi, gyi, gzi + 1); 
		const glm::vec4 c101 = getVortexColor(gxi + 1, gyi, gzi + 1); 
		const glm::vec4 c011 = getVortexColor(gxi, gyi + 1, gzi + 1); 
		const glm::vec4 c111 = getVortexColor(gxi + 1, gyi + 1, gzi + 1); 

		return 
	    (1.0f - tx) * (1.0f - ty) * (1.0f - tz) * c000 + 
	    tx * (1.0f - ty) * (1.0f - tz) * c100 + 
	    (1.0f - tx) * ty * (1.0f - tz) * c010 + 
	    tx * ty * (1.0f - tz) * c110 + 
	    (1.0f - tx) * (1.0f - ty) * tz * c001 + 
	    tx * (1.0f - ty) * tz * c101 + 
	    (1.0f - tx) * ty * tz * c011 + 
	    tx * ty * tz * c111;
	}
	else	//For Nearest Neighbour
	{
		return getVortexColor(p.X(),p.Y(),p.Z());
	}
}
glm::vec3 Volume::getNextIntersectionGradient(const Ray &r) const{
	Vector3D p = r.getPosition();
	if(INTER_MODE==1) //For Trilinear interpolation
	{
		int gxi = p.X();
		int gyi = p.Y();
		int gzi = p.Z();
		float tx = p.X() - gxi;
		float ty = p.Y() - gyi;
		float tz = p.Z() - gzi;

	// Lets try trilinear interpolation
		const glm::vec3 c000 = getVortexGradient(gxi, gyi, gzi); 
		const glm::vec3 c100 = getVortexGradient(gxi + 1, gyi, gzi); 
		const glm::vec3 c010 = getVortexGradient(gxi, gyi + 1, gzi); 
		const glm::vec3 c110 = getVortexGradient(gxi + 1, gyi + 1, gzi); 
		const glm::vec3 c001 = getVortexGradient(gxi, gyi, gzi + 1); 
		const glm::vec3 c101 = getVortexGradient(gxi + 1, gyi, gzi + 1); 
		const glm::vec3 c011 = getVortexGradient(gxi, gyi + 1, gzi + 1); 
		const glm::vec3 c111 = getVortexGradient(gxi + 1, gyi + 1, gzi + 1); 

		return 
	    (1.0f - tx) * (1.0f - ty) * (1.0f - tz) * c000 + 
	    tx * (1.0f - ty) * (1.0f - tz) * c100 + 
	    (1.0f - tx) * ty * (1.0f - tz) * c010 + 
	    tx * ty * (1.0f - tz) * c110 + 
	    (1.0f - tx) * (1.0f - ty) * tz * c001 + 
	    tx * (1.0f - ty) * tz * c101 + 
	    (1.0f - tx) * ty * tz * c011 + 
	    tx * ty * tz * c111;
	}
	else
	{
		return getVortexGradient(p.X(),p.Y(),p.Z());
	}
}
int Volume::getNextIntersectionScalar(const Ray &r) const{
	Vector3D p = r.getPosition();
	return sampleVolume(p.X(),p.Y(),p.Z());
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
		filterVolumeNxNxN(S_FILTER);
		return true;
	}
	return false;
}

bool Volume::loadVolumePNG(string file,int slices=176)
{
	int components;
	unsigned char *imageData = stbi_load(file.c_str(),
	        &XDIM, &YDIM, &components, STBI_grey);

	ZDIM = slices;
	YDIM = YDIM / ZDIM;

	string v_file = file+'_'+std::to_string(S_FILTER)+".vol";

	if(!fileExists(v_file))
	{

		scalars.resize(XDIM*YDIM*ZDIM);
		gradients.resize(XDIM*YDIM*ZDIM);

		for (int i = 0; i < scalars.size(); i++)
		{
			scalars[i] = (float)imageData[i];
		}
		filterVolumeNxNxN(S_FILTER);

		float *temp = &(scalars[0]);

		//Now save the gradient
		ofstream fout;
		fout.open(v_file, ios_base::binary);

		fout.write(reinterpret_cast<char*>(temp), XDIM*YDIM*ZDIM *  sizeof(float));
		fout.close();
		cout << "Filtered Volume saved into file\n";

		return true;
	}
	else
	{
		ifstream fin;
		fin.open(v_file, ios_base::binary);
		
		float *temp = new float[XDIM*YDIM*ZDIM];
		
		fin.read(reinterpret_cast<char*>(temp), XDIM*YDIM*ZDIM * sizeof(float));
		fin.close();
		
		scalars = vector<float>(temp, temp + XDIM * YDIM*ZDIM);

		cout << "Filtered Volume loaded from file\n";
		return true;
	}
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

				gradients[index++] = (s2 - s1)/ (2.0f*n);
				if (isnan(gradients[index - 1].x) )
				{
					gradients[index - 1].x = 0.0f;
				}
				if( isnan(gradients[index - 1].y) )
				{
					gradients[index - 1].y = 0.0f;
				}
				if(isnan(gradients[index - 1].z))
				{
					gradients[index - 1].z = 0.0f;					
				}
			}
		}
	}

	filterNxNxN(G_FILTER);

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



int Volume::sampleVolume(int x, int y, int z) const
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
void Volume::filterVolumeNxNxN(int n)
{
	std::cout<<"Filtering Volume of size "<<n<<"..."<<std::endl;
	int index = 0;
	vector<float> filt_scalars(scalars.size());

	for (int z = 0; z < ZDIM; z++)
	{
		for (int y = 0; y < YDIM; y++)
		{
			for (int x = 0; x < XDIM; x++)
			{
				filt_scalars[index++] = sampleVolumeNxNxN(x, y, z, n);
			}
		}
	}
	scalars.assign(filt_scalars.begin(),filt_scalars.end());
	std::cout<<"Finish Filtering Volume..."<<std::endl;		
}
float Volume::sampleVolumeNxNxN(int x, int y, int z, int n)
{
	n = (n - 1) / 2;
	float avg = 0.0f;
	int num = 0;

	for (int k = z - n; k <= z + n; k++)
	{
		for (int j = y - n; j <= y + n; j++)
		{
			for (int i = x - n; i <= x + n; i++)
			{
				if (isInBounds(i, j, k))
				{
					avg += sampleVolume(i, j, k);
					num++;
				}
			}
		}
	}
	avg /= (float)num;

	// if (avg != glm::vec3(0.0f))
	// {
	// 	avg = glm::normalize(avg);
	// }

	return avg;
}

//filter the gradients with an NxNxN box filter
//Should be an odd number of samples. 3 used by default.
void Volume::filterNxNxN(int n)
{
	int index = 0;
	vector<glm::vec3> filt_gradients(gradients.size());

	for (int z = 0; z < ZDIM; z++)
	{
		for (int y = 0; y < YDIM; y++)
		{
			for (int x = 0; x < XDIM; x++)
			{
				filt_gradients[index++] = sampleNxNxN(x, y, z, n);
			}
		}
	}
	gradients.assign(filt_gradients.begin(),filt_gradients.end());	
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

	// if (avg != glm::vec3(0.0f))
	// {
	// 	avg = glm::normalize(avg);
	// }

	return avg;
}

int clip(int n, int lower, int upper)
{
	return std::max(lower, std::min(n, upper));
}
