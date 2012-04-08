//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Image Analyzer - A tool to compare two images for their difference
// 
// Developed By
//      Brandon Light:   brandon@alum.wpi.edu
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include "TextureManager.h"
#include "MetricRMS.h"

void printUsage()
{
    std::cout << "USAGE USAGE USAGE!" << std::endl;

}

CTexture* loadImage(char *filename)
{
    std::string imageFile = filename;

    size_t periodPos = imageFile.rfind('.');

    int extLength = imageFile.length() - periodPos;

    char *ext = new char[extLength];
    std::fill(ext, ext+extLength, 0);
    imageFile.copy(ext, extLength-1, periodPos+1);

    CTexture *tex = NULL;
    if(strcmp(ext,"eni") == 0)
    {
        tex = CTextureManager::ReadTextureFromENI(imageFile);
    }
    else if(strcmp(ext,"png") == 0)
    {
        tex = CTextureManager::ReadTextureFromPNG(imageFile);
    }

     delete ext; 
     ext = NULL;

     return tex;
}

int main(int argc, char **argv)
{
    CTexture *image1 = NULL;
    CTexture *image2 = NULL;

    if(argc != 3)
    {
        printUsage();
    }
    else
    {
        image1 = loadImage(argv[1]);
        image2 = loadImage(argv[2]);
    }

    CMetricRMS rms;

    CTexture *diff = rms.Analyze(image1, image2);

    CTextureManager::WriteTextureToPNG("diff.png", diff);

    std::fstream errorReport("error.txt", std::ios_base::out);
    errorReport << "ErrorR = " << rms.GetErrorR() << std::endl;
    errorReport << "ErrorG = " << rms.GetErrorG() << std::endl;
    errorReport << "ErrorB = " << rms.GetErrorB() << std::endl;
    errorReport << "Error  = " << rms.GetError() << std::endl;
    errorReport.close();

    return 0;
}