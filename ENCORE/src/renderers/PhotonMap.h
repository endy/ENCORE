//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Project description and license goes here.
// 
// Developed By
//      Chen-Hao Chang:  chocobo7@WPI.EDU
//      Brandon Light:   brandon@alum.wpi.edu
//      Peter Lohrman:   plohrmann@alum.WPI.EDU
//////////////////////////////////////////////////////////////////////////

#pragma once 

#include "Photon.h"

#include <vector>
#include <queue>
#include <utility>

using std::vector;
using std::priority_queue;
using std::pair;


/// PhotonMap class
///
/// A spatial data structure used for storing photons.  This is implemented using
/// a K-d tree.
///
class PhotonMap
{
public:
	// default constructor
	PhotonMap(size_t in_maxPhotonCount);

	// destructor
	virtual ~PhotonMap();

	//  Add a photon to the map
	void AddPhoton(Photon& p);
	
	//  Builds a balanced K-d tree of photons--basis for the photon map--from the current photons
	void BalanceAndConstruct();

    // precomputes irradiance across the photon map, returning a new photon map with the irradiances stored
    //  increment - the space between photons considered (every Nth photon where increment = N)
    PhotonMap* PrecomputeIrradiance(int increment, const int N, const float maxDistance);
	
	// Gets N nearest photons to 'location'
	vector<PhotonDistPair> GetNearestNPhotons(const int N, const float maxDistance, Point3f location, Vector3 normal);

	// Scale the power of every photon by scaleFactor
	void ScalePhotons(const float scaleFactor);

	// is the photon map full?
	bool Full(){ return (m_photons.size() == static_cast<size_t>(MAX_PHOTON_COUNT)); }

    int GetCountStored(){ return (int) m_photons.size(); }

    // get the 
    Color GetRadianceEstimate(const int N, const float maxDistance, Point3f location, Vector3 normal);

    size_t GetMaxPhotons(){ return MAX_PHOTON_COUNT; }

    // temp debug value, remove asap
    void SetCausticsMap(bool isCausticsMap){ m_CausticsMap = isCausticsMap; }
    bool m_CausticsMap;

protected:  // FUNCTIONS

	// Compare x-values of Photons A and B position
	static bool CompareX(const Photon& A, const Photon& B){ return (A.Position().X() < B.Position().X()); }
	// Compare y-values of Photons A and B position
	static bool CompareY(const Photon& A, const Photon& B){ return (A.Position().Y() < B.Position().Y()); }
	//  Compare z-values of Photons A and B position
	static bool CompareZ(const Photon& A,  const Photon& B){ return (A.Position().Z() < B.Position().Z()); }

protected: // DATA

	// collection of photons in the map
	vector<Photon> m_photons;

	// maximum number of photons stored in the map
	const size_t MAX_PHOTON_COUNT;

	// flag states whether the map is balanced. if it isn't, it will need to be reconstructed
	bool m_isBalanced;

private: // FUNCTIONS
	void BalanceAndConstruct(int minIndex, int maxIndex, int nodeIndex);

	void GetNearestNPhotons(const int N, 
                            float rSquared, 
                            const Point3f& location, 
                            const Vector3& normal, 
                            const int minIndex, 
                            const int maxIndex, 
                            vector<PhotonDistPair>& nearest);

private: // DATA
	
	// maximum coordinates of a box surrounding all photons
	Point3f m_BBoxMax;
	// minimum coordinates of a box surrounding all photons
	Point3f m_BBoxMin;

};

