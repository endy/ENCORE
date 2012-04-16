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

#include "Color.h"
#include "Point3.h"
#include "Ray.h"

#include <vector>

namespace encore
{
    /// IEmissive class
    ///
    /// Interface to objects that give off light in the scene.  
    /// These can include scene lights or objects
	class IEmissive
	{
	public:

        IEmissive(){};
        virtual ~IEmissive(){};

    public:

        ///  Return a ray that corresponds to the direction of an emitted photon
        virtual Ray EmitPhoton() = 0;
        /// Get the point, or a point of a photon emission
        virtual Point3 GetPointOfEmission() = 0;
        /// Get sample points for soft shadows
        virtual std::vector<Point3> GetSamplePoints(int xDivisions, int yDivisions, int samplesPerDivision){ return std::vector<Point3>(); }

		Color GetEmissiveColor() const { return m_EmissiveColor; }
		void  SetEmissiveColor(Color emissive){ m_EmissiveColor = emissive; } 

	protected:

		/// The color of the emissive surface
		Color m_EmissiveColor;
	
    };
}
