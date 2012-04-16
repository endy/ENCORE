
CPPINCS = -I renderers/ \
          -I misc/ \
          -I scene/ \
          -I math/ \
          -I accelerators/ \
          -I ../lib/include

#CPPFLAGS = -Wall


#CPPFILES = main.cpp \
           renderers/PhotonMapperCPU.cpp \
           renderers/PhotonMap.cpp \
           renderers/Photon.cpp \
           renderers/Sampler.cpp \
           renderers/Tonemap.cpp \
           renderers/OpenGLRenderer.cpp

CPPFILES = math/Affine4.cpp \
           math/Ray.cpp \
           accelerators/BasicAS.cpp \
           accelerators/BVH.cpp \
           accelerators/IrradianceCache.cpp \
           accelerators/Kdtree.cpp \
           accelerators/UniformGrid.cpp \
           accelerators/GPUAccelerationStructureData.cpp

main:
	g++ -c $(CPPFILES) $(CPPFLAGS) $(CPPINCS)


default: main
	g++ -o encore main.o
