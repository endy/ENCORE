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
#include "Vector3.h"

using encore::Color;
using encore::Point3;
using encore::Vector3;

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
	Photon(Point3 in_position, Color in_power, Vector3 in_direction);

    /// Destroys the instance.
    /// NON-VIRTUAL destructor assumes that Photon will NOT be inherited
	~Photon(); 

public: // public methods

    /// Get the photon position
    Point3 Position(){ return m_position; }
    const Point3& Position() const { return m_position; }
    /// Set the photon position
	void Position(Point3 in_position){ m_position = in_position; }
    /// Get the photon's power
	Color Power() const { return m_power; }
    /// Set the photon's power
	void Power(Color in_power){ m_power = in_power; }
    /// Get the axis that the dividing plane aligns with
	AXIS Plane(){ return m_plane; }
    /// Set the axis that the dividing plane algins with
	void SetPlane(AXIS in_plane){ m_plane = in_plane; }
    /// Get the incoming direction of the photon
	Vector3 Direction() const { return m_incomingDir; }
    /// Set the incoming direction of the photon
	void Direction(Vector3 in_dir){ m_incomingDir = in_dir; }
    /// Get the normal of the absorbing surface
	const Vector3& SurfNormal() const { return m_surfNormal; }
    /// Stores the normal of the absorbing surface 
	void SurfNormal(Vector3 in_normal){ m_surfNormal = in_normal; }
	/// Set the type of the last reflection
	void LastReflection(REFLECTION_TYPE r){ m_last = r; }
    /// Get the type of the last reflection 
	REFLECTION_TYPE LastReflection(){ return m_last; }
    /// Returns whether the photon is a shadow photon
    bool IsShadowPhoton() const { return m_IsShadowPhoton; }
    /// Change whether the photon is a shadow photon
    void SetShadowPhoton(bool isShadowPhoton){ m_IsShadowPhoton = isShadowPhoton; } 

private:
	/// Point of absorbtion
	Point3 m_position;
	/// Axis the splitting plane is aligned with
	AXIS m_plane;
	/// The incoming direction
	Vector3 m_incomingDir;
	/// Normal of the absorbing surface
	Vector3 m_surfNormal;
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
    PhotonDistPair(const Photon* p = NULL, const float distance = 0.0)
    {
	    m_pPhoton = p;
		m_Distance = distance;
	}

public:
    const Photon* m_pPhoton;
	float m_Distance;
};

inline bool operator<(const PhotonDistPair& lhs, const PhotonDistPair& rhs)
{
    return (lhs.m_Distance < rhs.m_Distance);
}
