
CPPINCS = -I renderers/ \
          -I misc/ \
          -I scene/ \
          -I math/ \
          -I accelerators/

#CPPFLAGS = -Wall


CPPFILES = main.cpp \
           renderers/PhotonMapperCPU.cpp \
           renderers/PhotonMap.cpp \
           renderers/Photon.cpp \
           renderers/Sampler.cpp \
           renderers/Tonemap.cpp \
           renderers/OpenGLRenderer.cpp



main:
	g++ -c $(CPPFILES) $(CPPFLAGS) $(CPPINCS)


default: main
	g++ -o encore main.o
