/****************************************************************************
Brandon Light
03/21/2006

PhotonMap.cpp

Note: Photon map based heavily on renderBitch photon map

****************************************************************************/

#include "PhotonMap.h"
#include "TVector3.h"

#include "Common.h"

#include <algorithm>
#include <iostream>

using encore::Vector3f;

static const float LARGE_NUMBER = 1.0e10;

bool IGNORE_NORMAL = false;

PhotonMap::PhotonMap(size_t in_maxPhotonCount):
	MAX_PHOTON_COUNT(in_maxPhotonCount), 
		m_BBoxMax(LARGE_NUMBER, LARGE_NUMBER, LARGE_NUMBER),
	m_BBoxMin(-LARGE_NUMBER, -LARGE_NUMBER, -LARGE_NUMBER)
{
	m_photons.reserve(MAX_PHOTON_COUNT);
    m_CausticsMap = false;
}

PhotonMap::~PhotonMap()
{
	//for(int index = 0; index < (int)m_photons.size(); index++)
	//{
	//	// delete all of the photons stored in the map
	//	delete m_photons[index];
	//	m_photons[index] = NULL;
	//}
}

///////////   PUBLIC MEMBER FUNCTIONS  /////////////////////////////////////////

void PhotonMap::AddPhoton(Photon& p)
{
	if(m_photons.size() < MAX_PHOTON_COUNT)
	{
		m_photons.push_back(p);
		m_isBalanced = false;
	}
}



PhotonMap* PhotonMap::PrecomputeIrradiance(int increment, const int N, const float maxDistance)
{
    std::cout << "Precomputing irradiance" << std::endl;

    PhotonMap *precomputed = new PhotonMap(MAX_PHOTON_COUNT);

    for(int i = 0; i < (int)m_photons.size(); i += increment)
    {
        Photon p = m_photons[i];
        p.Power(GetRadianceEstimate(N, maxDistance, m_photons[i].Position(), m_photons[i].SurfNormal()));

        precomputed->AddPhoton(p);
    }

    precomputed->BalanceAndConstruct();

    return precomputed;
}

vector<PhotonDistPair> PhotonMap::GetNearestNPhotons(const int N, const float maxDistance, Point3f location, Vector3f normal)
{
    if(m_photons.empty())
    {
        return vector<PhotonDistPair>();
    }

	vector<PhotonDistPair> nearest;
    nearest.reserve(N);

    int maxIndex = (int) m_photons.size() - 1;
	if(N > 0 && maxIndex >= 0)
    {   
		GetNearestNPhotons(N, maxDistance*maxDistance, location, normal, 0, ((int)m_photons.size()) - 1, nearest);
        std::make_heap(nearest.begin(), nearest.end());
    }

	return nearest;
}


void PhotonMap::ScalePhotons(const float scaleFactor)
{
	for(int index = 0; index < (int) m_photons.size(); index++)
	{
		Color power = m_photons[index].Power();
		power.R() *= scaleFactor; 
		power.G() *= scaleFactor;
		power.B() *= scaleFactor;

		m_photons[index].Power(power);
	}
}

inline double smoothstep(const double& a, const double& b, const double& x)
{
	if (x <= a)
		return 0;
	else if (x >= b)
		return 1;
	double temp = (x - a) / (b - a);
	return temp*temp*(3-2*temp);
}


Color PhotonMap::GetRadianceEstimate(const int N, const float maxDistance, Point3f location, Vector3f normal)
{
    float R = 0; 
    float G = 0; 
    float B = 0;

    vector<PhotonDistPair> nearest = GetNearestNPhotons(N, maxDistance, location, normal);

    if(nearest.size() == 0)
    {
        return Color(0.0, 0.0, 0.0);
    }

    // get the radius of the furthest photon
    double r = nearest.front().m_Distance;

 
    for(int index = 0; index < nearest.size(); ++index)
    {
        const Photon* p = nearest[index].m_pPhoton;
        

        Color power = p->Power();
        Vector3f photonDir = p->Direction(); 

        double radius = nearest[index].m_Distance;
        double weight = (m_CausticsMap) ? (r - radius)/radius : 1;//smoothstep(0,1,1-radius/r);

        R += power.R() * weight;
        G += power.G() * weight;
        B += power.B() * weight;
        //std::cout << weight << std::endl;

     }

    // filter?
    float densityEstimate = (m_CausticsMap) ? N/(PI*r*r) : 1;

    R *= densityEstimate;
    G *= densityEstimate;
    B *= densityEstimate;

    return Color(R,G,B);
}

//////////  PRIVATE MEMBER FUNCTIONS  ///////////////////////////////////////////
void PhotonMap::BalanceAndConstruct()
{
	if(m_isBalanced)
		return;
	
	BalanceAndConstruct(0, m_photons.size()-1, 0);

	m_isBalanced = true;
}
void PhotonMap::BalanceAndConstruct(int minIndex, int maxIndex, int level)
{
	if(maxIndex - minIndex >= 0)
	{
		// determine which axis-aligned plane to partition the photons with by choosing the axis w/ the biggest
		//   distribution of photons
		AXIS axis = Z;		
		if((m_BBoxMax.X() - m_BBoxMin.X()) > (m_BBoxMax.Y() - m_BBoxMin.Y()) &&
			 (m_BBoxMax.Y() - m_BBoxMin.Y()) > (m_BBoxMax.Z() - m_BBoxMin.Z()))
		{
			axis = X;
		}
		else if((m_BBoxMax.Y() - m_BBoxMin.Y()) > (m_BBoxMax.Z() - m_BBoxMin.Z()))
		{
			axis = Y;
		}

		int medianIndex = (maxIndex + minIndex) / 2;

		// partition using median
		if(axis == X)
			std::nth_element(m_photons.begin() + minIndex, m_photons.begin() + medianIndex, m_photons.begin() + maxIndex + 1, CompareX);
		else if(axis == Y)
			std::nth_element(m_photons.begin() + minIndex, m_photons.begin() + medianIndex, m_photons.begin() + maxIndex + 1,CompareY);
		else
			std::nth_element(m_photons.begin() + minIndex, m_photons.begin() + medianIndex, m_photons.begin() + maxIndex + 1, CompareZ);

		m_photons[medianIndex].SetPlane(axis);

		float tempMin = m_photons[minIndex].Position()[axis];
		float tempMax = m_photons[maxIndex].Position()[axis];

		// construct left subtree
		m_BBoxMin[axis] = m_photons[medianIndex].Position()[axis];
		m_BBoxMax[axis] = m_photons[medianIndex].Position()[axis];
		BalanceAndConstruct(minIndex, medianIndex-1, level + 1);
			
		// construct right subtree
		m_BBoxMin[axis] = m_photons[medianIndex].Position()[axis];
		m_BBoxMax[axis] = m_photons[medianIndex].Position()[axis];
		BalanceAndConstruct(medianIndex+1, maxIndex, level + 1);

		m_BBoxMin[axis] = tempMin;
		m_BBoxMax[axis] = tempMax;
	}
}


void PhotonMap::GetNearestNPhotons(
    const int N, 
    float rSquared, 
    const Point3f& location, 
    const Vector3f& normal, 
    const int minIndex, 
    const int maxIndex,
    vector<PhotonDistPair>& nearest)
{
	if(maxIndex - minIndex >= 0)
	{
		int nodeIndex = (maxIndex + minIndex) / 2;

        // CONSIDER ADDING THIS NODE TO PHOTON HEAP
		const Photon* p = &m_photons[nodeIndex];
        float x = location.X() - p->Position().X();
        float y = location.Y() - p->Position().Y();
        float z = location.Z() - p->Position().Z();
        float distSquared = (x*x)+(y*y)+(z*z);
		
        if(distSquared < rSquared && (IGNORE_NORMAL || p->SurfNormal() == normal))
        {
            if((int)nearest.size() < N - 1)
            {
                nearest.push_back(PhotonDistPair(p, distSquared));
            }
            else if((int)nearest.size() == N - 1)
            {
                nearest.push_back(PhotonDistPair(p, distSquared));
                std::make_heap(nearest.begin(), nearest.end());

                rSquared = nearest[0].m_Distance;
            }
            else
            {
                nearest.push_back(PhotonDistPair(p, distSquared));
                std::make_heap(nearest.begin(), nearest.end());

                if(nearest.size() > N)
                {
                    std::pop_heap(nearest.begin(), nearest.end());
                    nearest.pop_back();
                }
                
                rSquared = nearest[0].m_Distance;
            }
        }
	
        AXIS plane = m_photons[nodeIndex].Plane();
        float distanceToPlane = location[plane] - m_photons[nodeIndex].Position()[plane];

		//// SEARCH PHOTON MAP 
		if(distanceToPlane*distanceToPlane > rSquared)
		{
            if(distanceToPlane > 0)
            {
                // check right tree
			    GetNearestNPhotons(N, rSquared, location, normal, nodeIndex+1, maxIndex, nearest);
                
            }
            else
            {
                // check left tree
			    GetNearestNPhotons(N, rSquared, location, normal, minIndex, nodeIndex-1, nearest);
            }
        }
        else
        {
            // check both
            GetNearestNPhotons(N, rSquared, location, normal, minIndex, nodeIndex-1, nearest);
            GetNearestNPhotons(N, rSquared, location, normal, nodeIndex+1, maxIndex, nearest);
        }                                               
	}
}

