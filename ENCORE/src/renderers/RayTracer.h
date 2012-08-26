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

#include "Renderer.h"
#include "TextureMaker.h"
#include "SimpleProfiler.h"


/// RayTracer class
///
/// Provides basic ray-tracing functionality.  Uses "bucket rendering" that 
/// is can be configured to render an image in blocks rather than rows of pixels
///
class RayTracer : public Renderer
{
public:

    RayTracer(int pixelRows, int pixelCols);
    virtual ~RayTracer();

public: // Renderer Interface 

    /// Prints the usage information for RayTracer to STDOUT
    virtual void usage();
    /// Configures the renderer's properties
    virtual void configure( Options* pOptions );
    /// Initialize the renderer.  Must be called before calling render()
    virtual void init(Scene* scn, AccelerationStructure* accelStruct, Camera* cam);
    /// Render a single frame
    virtual void render();
    /// Deinitializes the renderer
    virtual void deinit();
    /// Keyboard input handler
    virtual void keyboard( unsigned char key );

    virtual void Refine();


protected: // Ray Tracer methods
    
    /// Render the scene onto a texture, displays intermediate results
    void RenderScene();
    /// Calculates the light traveling in the reverse direction of the eye ray from where the eyeRay hits
    virtual Color CalculateRadiance(Ray eyeRay, HitInfo hit, int recurseLevel = 1){ return m_backgroundColor; };
    /// Renders the scene as a texture on a quad
    void RenderSceneImage();

protected: // data

    // profile data
    unsigned long int m_RenderTime;

    /// Number of 'buckets' in a row
    int m_BucketRows;
    /// Number of 'buckets' in a column
    int m_BucketCols;
    /// The extents of the image to render, by bucket
    int m_StartBucketRow, m_EndBucketRow, m_StartBucketCol, m_EndBucketCol;

    /// The most recently completed bucket  
    int m_CurrentBucketRow, m_CurrentBucketCol;

    /// The number of pixel rows in the image
    int m_pixelRows;
    /// The number of pixel columns in the image
    int m_pixelCols;
    /// The number of rays per pixel
    unsigned int m_RaysPerPixel;
    /// The size of the pixel blocks
    int m_BlockSize;
    /// The number of levels to recurse when ray tracing
    int m_RecurseLevel;
    /// The color to make the background on ray-misses
    Color m_backgroundColor;

    /// The scene texture
    TextureGL<GLfloat> *m_Image;

    /// The object used to create the image texture
    TextureMaker* m_ImageMaker;

private: // methods

    /// return a ray cast from the eye to the given pixel coord
    std::vector<Ray> CalcEyeRaySamples(int sampleCount, float percentJitter, int pixelRow, int pixelCol);

private: // data

};

