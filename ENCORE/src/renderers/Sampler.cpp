#include "Sampler.h"

#include "Common.h"

#include "Matrix.h"
#include "TAffineMatrix.h"

Sampler::Sampler(int xDivisions, int zDivisions, int samplesPerCell)
{
    m_Normal = Vector3f(0, 1, 0);

    m_xDivisions  = xDivisions;
    m_zDivisions = zDivisions;

    m_SamplesPerCell = samplesPerCell;

    m_CurrentLength = 1;
    m_CurrentWidth = 1;

    m_PercentJitter = 0.0f;
}

std::vector<Ray> Sampler::GetSampleRaysForHemisphere(Point3f origin, Vector3f normal)
{
#if 0
    std::vector<Ray> samples;

    for(int count = 0; count < m_xDivisions * m_zDivisions; ++count)
    {
        Ray r;
        r.Origin() = origin;
        r.Direction() = randDirectionSuperN(normal);

        samples.push_back(r);
    }
    return samples;
#else
    m_Normal = Vector3f(0, 0, 1);  // set to proper normal for this method

    GenerateHemisphereSamplePoints();
    SphericalToCartesian();
    
    AlignPlaneWithNormal(normal);

    return GetSampleRays(origin);
#endif
}

std::vector<Point3f> Sampler::GetSamplePointsForRect(Point3f center, Vector3f normal, float length, float width)
{
    m_Normal = Vector3f(0, 1, 0);  // set to proper normal
    
    GenerateGridSamplePoints();
    TranslatePoints(Vector3f(-0.5, 0.0, -0.5)); // center points at origin
    ScalePoints(length, width); // resize points to fit rect
        
    AlignPlaneWithNormal(normal); // rotate points so they are aligned with the plane

    TranslatePoints(Vector3f(Point3f(), center)); // move the points to where the plane is

    Vector3f diff = normal * 0.01f; 
    TranslatePoints(diff);

    return m_SamplePoints;
}

void Sampler::GenerateGridSamplePoints()
{
    m_SamplePoints.clear();

    float cellLength = (m_xDivisions != 0) ? (1 / (float) m_xDivisions) : 1;
    float cellWidth = (m_zDivisions != 0) ? (1 / (float) m_zDivisions) : 1;

    float halfCellLength = cellLength / 2.0f;
    float halfCellWidth  = cellWidth / 2.0f;
    
    for(int row = 0; row < m_zDivisions; ++row)
    {
        for(int col = 0; col < m_xDivisions; ++col)
        {
            float cellCenterX = (cellLength / 2.0f) + (cellLength * col);
            float cellCenterZ = (cellWidth / 2.0f) + (cellWidth * row);

            for(int samples = 0; samples < m_SamplesPerCell; ++samples)
            {
                if(m_PercentJitter > 0)
                {
                    float jitterValueX = randNumber(-halfCellLength, halfCellLength) * m_PercentJitter;
                    float jitterValueZ = randNumber(-halfCellWidth, halfCellWidth) * m_PercentJitter;

                    m_SamplePoints.push_back(Point3f(cellCenterX + jitterValueX, 0, cellCenterZ + jitterValueZ));
                }
                else
                {
                    m_SamplePoints.push_back(Point3f(cellCenterX, 0, cellCenterZ));
                }                
            }
        }
    }
}

void Sampler::GenerateHemisphereSamplePoints()
{
    m_SamplePoints.clear();

    
    float thetaDiv  = (m_xDivisions != 0) ? ((1 / (float) m_xDivisions) * (PI/2.0f)) : PI/2.0f;
    float phiDiv = (m_zDivisions != 0) ? ((1 / (float) m_zDivisions) * (2 * PI)) : 2 * PI;

    float halfThetaDiv = thetaDiv / 2.0f;
    float halfPhiDiv  = phiDiv / 2.0f;

    for(int row = 0; row < m_zDivisions; ++row)
    {
        for(int col = 0; col < m_xDivisions; ++col)
        {
            float theta = (thetaDiv * col) + halfThetaDiv;
            float phi =  (phiDiv * row) + halfPhiDiv;

            for(int samples = 0; samples < m_SamplesPerCell; ++samples)
            {
                if(m_PercentJitter > 0)
                {
                    float jitteredThetaValue = randNumber(-halfThetaDiv, halfThetaDiv) * m_PercentJitter;
                    float jitteredPhiValue = randNumber(-halfPhiDiv, halfPhiDiv) * m_PercentJitter;

                    m_SamplePoints.push_back(Point3f(theta + jitteredThetaValue, 0, phi + jitteredPhiValue));
                }
                else
                {
                    m_SamplePoints.push_back(Point3f(theta, 0, phi));
                }                
            }
        }
    }
}

void Sampler::SphericalToCartesian()
{
    for(int sampleIndex = 0; sampleIndex < (int) m_SamplePoints.size(); ++sampleIndex)
    {
        float theta = m_SamplePoints[sampleIndex].X();
        float phi = m_SamplePoints[sampleIndex].Z();

        // absolute value at y (because we are sampling a hemisphere
        Point3f p(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
        Vector3f vP(Point3f(), p);
        vP.Normalize();
        m_SamplePoints[sampleIndex] = vP.ToPoint();  
    }
}


void Sampler::AlignPlaneWithNormal(Vector3f normal)
{
    normal.Normalize();

    if(m_Normal == normal)
    {
        // already aligned
        return;
    }
    else if(m_Normal == -normal)
    {
        // aligned with -Normal, simple flip is all that is needed
        for(int index = 0; index < (int) m_SamplePoints.size(); ++ index)
        {
            Vector3f pVector(Point3f(), m_SamplePoints[index]);
            pVector += pVector * -2;
            m_SamplePoints[index] = pVector.ToPoint();
        }
        return;
    }
    ////
    // find rotation vector and theta, then multiply by the rotation matrix...
    ////

    float theta = acos(Dot(m_Normal, normal));

    if(theta == 0)
    {
        return;
    }

    Vector3f rv = Cross(m_Normal, normal);
    rv.Normalize();

    TAffineMatrix<float> m = TAffineMatrix<float>::GetRotationMatrix(rv, theta);
    
    for(int sampleIndex = 0; sampleIndex < (int) m_SamplePoints.size(); ++sampleIndex)
    {
        m_SamplePoints[sampleIndex] = m *  m_SamplePoints[sampleIndex];
    }
}

void Sampler::TranslatePoints(Vector3f translationVector)
{
    Point3f transPoint = translationVector.ToPoint();
    for(int sampleIndex = 0; sampleIndex < (int) m_SamplePoints.size(); ++sampleIndex)
    {
        m_SamplePoints[sampleIndex] = transPoint + m_SamplePoints[sampleIndex];
    }
}

void Sampler::ScalePoints(float length, float width)
{
    float lengthRatio = length / m_CurrentLength;
    float widthRatio = width / m_CurrentWidth;

    for(int sampleIndex = 0; sampleIndex < (int) m_SamplePoints.size(); ++sampleIndex)
    {
        m_SamplePoints[sampleIndex].X() *= lengthRatio;
        m_SamplePoints[sampleIndex].Z() *= widthRatio;
    }

    m_CurrentLength = length;
    m_CurrentWidth = width;
}

std::vector<Ray> Sampler::GetSampleRays(Point3f rayOrigin)
{
    std::vector<Ray> sampleRays(m_SamplePoints.size());

    for(int sampleIndex = 0; sampleIndex < (int) m_SamplePoints.size(); ++sampleIndex)
    {
        Vector3f dir(Point3f(0.0f, 0.0f, 0.0f), m_SamplePoints[sampleIndex]);
        dir.Normalize();

        Ray r(rayOrigin, dir);
        sampleRays[sampleIndex] = r;
    }

    return sampleRays;
}
