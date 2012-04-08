#pragma once

#include <math.h>

#ifdef WIN32
#include <windows.h>
#include <GL/glu.h>
#include <GL/gl.h>
#elif __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include "TVector3.h"

using encore::Point3f;
using encore::Vector3f;

using encore::TVector3;

class Camera
{
public:
	Camera(void);
	Camera( Point3f eyePosition, Point3f lookAt, Vector3f upDirection);
	Camera( Point3f eyePosition, Point3f lookAt, Vector3f upDirection,
		  float viewAngle, float aspectRatio, float nearPlane, float farPlane);

    virtual ~Camera(void);

    //// ACCESSORS ////////////////////////////
	void set(Point3f eye, Point3f look, Vector3f up);

    Point3f GetEye(){ return m_Eye; }
	void SetEye(float x, float y, float z);
    Point3f GetLook(){ return m_Look; }
	void SetLook(float x, float y, float z);
    Vector3f GetUp(){ return m_Up; }
	void SetUp(float x, float y, float z);
	
    void SetViewVolume(float vAng, float asp, float nearD, float farD);

    Vector3f N(){ return m_N; }
    Vector3f U(){ return m_U; }
    Vector3f V(){ return m_V; }

    float GetViewAngle(){ return m_ViewAngle; }
    float GetAspect(){ return m_Aspect; }
    float GetNearClipPlane(){ return m_NearClipPlane; }
    float GetFarClipPlane(){ return m_FarClipPlane; }

    void setViewport(int width, int height){ m_ViewportWidth=width; m_ViewportHeight = height;}
    int GetViewportWidth(){ return m_ViewportWidth; }
    int GetViewportHeight(){ return m_ViewportHeight; }

    float GetWorldWidth(){ return m_WorldWidth; }
    float GetWorldHeight(){ return m_WorldHeight; }

    //// VIEWING  ////////////////////////////
    
    void LoadCameraMatrices();

	void setProjectionMatrix(void);
	void setModelViewMatrix(void);

    //// MOVEMENT ////////////////////////////
	void Roll(float angle);
	void Pitch(float angle);
	void Yaw(float angle);

    void Slide(float deltaX, float deltaY, float deltaZ);

    void rotateAboutLook(float deltaX, float deltaY);

private:
    void CalcCamVectors();

private:
	Point3f m_Eye, m_Look;
	Vector3f m_Up;
	Vector3f m_U, m_V, m_N;
	float m_ViewAngle, m_Aspect, m_NearClipPlane, m_FarClipPlane, m_WorldWidth, m_WorldHeight;
	int m_ViewportWidth, m_ViewportHeight;

};
