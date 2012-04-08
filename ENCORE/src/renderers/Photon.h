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
#include "TPoint3.h"
#include "TVector3.h"

using encore::Color;
using encore::Point3f;
using encore::Vector3f;

// enumeration for axis-aligned planes
enum AXIS	
{
	X = 0,
	Y,
	Z
};

enum REFLECTION_TYPE
{
	NONE = 0,
	SPECULAR,
	DIFFUSE,
	TRANSMISSION,
};

/// Photon class
///
/// The photon data structure 
///
class Photon
{
public:

	Photon();
	Photon(Point3f in_position, Color in_power, Vector3f in_direction);

    /// Destroys the instance.
    /// NON-VIRTUAL destructor assumes that Photon will NOT be inherited
	~Photon(); 

public: // public methods

    /// Get the photon position
    Point3f& Position(){ return m_position; }
    const Point3f& Position() const { return m_position; }
    /// Set the photon position
	void Position(Point3f in_position){ m_position = in_position; }
    /// Get the photon's power
	Color Power(){ return m_power; }
    /// Set the photon's power
	void Power(Color in_power){ m_power = in_power; }
    /// Get the axis that the dividing plane aligns with
	AXIS Plane(){ return m_plane; }
    /// Set the axis that the dividing plane algins with
	void SetPlane(AXIS in_plane){ m_plane = in_plane; }
    /// Get the incoming direction of the photon
	Vector3f Direction(){ return m_incomingDir; }
    /// Set the incoming direction of the photon
	void Direction(Vector3f in_dir){ m_incomingDir = in_dir; }
    /// Get the normal of the absorbing surface
	const Vector3f& SurfNormal() const { return m_surfNormal; }
    /// Stores the normal of the absorbing surface 
	void SurfNormal(Vector3f in_normal){ m_surfNormal = in_normal; }
	/// Set the type of the last reflection
	void LastReflection(REFLECTION_TYPE r){ m_last = r; }
    /// Get the type of the last reflection 
	REFLECTION_TYPE LastReflection(){ return m_last; }
    /// Returns whether the photon is a shadow photon
    bool IsShadowPhoton(){ return m_IsShadowPhoton; }
    /// Change whether the photon is a shadow photon
    void SetShadowPhoton(bool isShadowPhoton){ m_IsShadowPhoton = isShadowPhoton; } 

private:
	/// Point of absorbtion
	Point3f m_position;
	/// Axis the splitting plane is aligned with
	AXIS m_plane;
	/// The incoming direction
	Vector3f m_incomingDir;
	/// Normal of the absorbing surface
	Vector3f m_surfNormal;
    /// Power of the photon in RGB
	Color m_power;
	/// The type of the photon's most recent reflection
	REFLECTION_TYPE m_last;
    /// Whether the photon is a shadow photon
    bool m_IsShadowPhoton;
};


class PhotonDistPair
{
public:
    PhotonDistPair(const Photon& p = Photon(), const float& distance = 0.0)
    {
	    m_Photon = p;
		m_Distance = distance;
	}

public:
    Photon m_Photon;
	float m_Distance;
};

inline bool operator<(const PhotonDistPair& lhs, const PhotonDistPair& rhs)
{
    return (lhs.m_Distance < rhs.m_Distance);
}