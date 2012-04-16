#pragma once
/*******************************************
  Finds points on a unit plane to sample
  or sphere
********************************************/

#include <vector>

#include "Ray.h"

using encore::Ray;
using encore::Point3;
using encore::Vector3;

class Sampler
{
public:
    Sampler(int xDivisions, int zDivisions, int samplesPerCell);

    std::vector<Point3> GetSamplePoints(){ return m_SamplePoints; }

    // creates rays to sample a hemisphere above a point with the given normal
    std::vector<Ray> GetSampleRaysForHemisphere(Point3 origin, Vector3 normal);

    std::vector<Point3> GetSamplePointsForRect(Point3 center, Vector3 normal, float length, float width);

    void SetJitterPercent(float percentJitter){ m_PercentJitter = percentJitter; }


private:
    // creates sample points on the xz plane from x=0,z=0 to x=1, z=1
    void GenerateGridSamplePoints();

    // creates sample points on a unit hemisphere
    void GenerateHemisphereSamplePoints();

    // convert points from spherical coords to cartesian
    void SphericalToCartesian();

    // move sample points in the direction of the given vector
    void TranslatePoints(Vector3 translationVector);

    // maps the current width/length to the new width/length
    void ScalePoints(float length, float width);

    // rotates points to align with normal
    void AlignPlaneWithNormal(Vector3 normal);

    // get rays from rayOrigin to sample points
    std::vector<Ray> GetSampleRays(Point3 rayOrigin);

private:
    Vector3 m_Normal;

    int m_xDivisions;
    int m_zDivisions;

    int m_SamplesPerCell;
    float m_PercentJitter;

    float m_CurrentWidth;
    float m_CurrentLength;

    std::vector<Point3> m_SamplePoints;
};
