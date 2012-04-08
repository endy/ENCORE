#pragma once
/*******************************************
  Finds points on a unit plane to sample
  or sphere
********************************************/

#include <vector>

#include "Ray.h"

using encore::Ray;
using encore::Point3f;
using encore::Vector3f;

class Sampler
{
public:
    Sampler(int xDivisions, int zDivisions, int samplesPerCell);

    std::vector<Point3f> GetSamplePoints(){ return m_SamplePoints; }

    // creates rays to sample a hemisphere above a point with the given normal
    std::vector<Ray> GetSampleRaysForHemisphere(Point3f origin, Vector3f normal);

    std::vector<Point3f> GetSamplePointsForRect(Point3f center, Vector3f normal, float length, float width);

    void SetJitterPercent(float percentJitter){ m_PercentJitter = percentJitter; }


private:
    // creates sample points on the xz plane from x=0,z=0 to x=1, z=1
    void GenerateGridSamplePoints();

    // creates sample points on a unit hemisphere
    void GenerateHemisphereSamplePoints();

    // convert points from spherical coords to cartesian
    void SphericalToCartesian();

    // move sample points in the direction of the given vector
    void TranslatePoints(Vector3f translationVector);

    // maps the current width/length to the new width/length
    void ScalePoints(float length, float width);

    // rotates points to align with normal
    void AlignPlaneWithNormal(Vector3f normal);

    // get rays from rayOrigin to sample points
    std::vector<Ray> GetSampleRays(Point3f rayOrigin);

private:
    Vector3f m_Normal;

    int m_xDivisions;
    int m_zDivisions;

    int m_SamplesPerCell;
    float m_PercentJitter;

    float m_CurrentWidth;
    float m_CurrentLength;

    std::vector<Point3f> m_SamplePoints;
};