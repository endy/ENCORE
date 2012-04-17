
CPPINCS = -I renderers/ \
          -I misc/ \
          -I scene/ \
          -I math/ \
          -I accelerators/ \
          -I ../lib/include

#CPPFLAGS = -Wall


CPPFILES = main.cpp \
           renderers/Renderer.cpp \
           renderers/PhotonMapperCPU.cpp \
           renderers/PhotonMapperGPU.cpp \
           renderers/PhotonMap.cpp \
           renderers/Photon.cpp \
           renderers/Sampler.cpp \
           renderers/Tonemap.cpp \
           renderers/OpenGLRenderer.cpp \
           math/Affine4.cpp \
           math/Ray.cpp \
           accelerators/BasicAS.cpp \
           accelerators/BVH.cpp \
           accelerators/IrradianceCache.cpp \
           accelerators/Kdtree.cpp \
           accelerators/UniformGrid.cpp \
           accelerators/GPUAccelerationStructureData.cpp \
           misc/BatteryPowerWin32.cpp \
           misc/Common.cpp \
           misc/Logfile.cpp \
           misc/Options.cpp \
           misc/Pixel.cpp \
           misc/SimpleProfiler.cpp \
           misc/Texture.cpp \
           misc/TextureMaker.cpp \
           misc/TextureManager.cpp \
           misc/Timer.cpp \
           scene/AABB.cpp \
           scene/Camera.cpp \
           scene/Color.cpp \
           scene/DynamicModel.cpp \
           scene/DynamicScene.cpp \
           scene/HitInfo.cpp \
           scene/IModel.cpp \
           scene/Material.cpp \
           scene/Material.cpp \
           scene/Model.cpp \
           scene/Objloader.cpp \
           scene/ObjModelloader.cpp \
           scene/PointLightPrim.cpp \
           scene/RectangleLight.cpp \
           scene/Scene.cpp \
           scene/Triangle.cpp \
           scene/TriangleLight.cpp \
           scene/TrianglePrim.cpp \
           scene/Vertex.cpp

compile:
	g++ -c $(CPPFILES) $(CPPFLAGS) $(CPPINCS)

link: compile
	g++ -o encore *.o

default:
	link
