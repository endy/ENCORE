
CPPINCS = -I renderers/ \
          -I misc/ \
          -I scene/ \
          -I math/ \
          -I accelerators/

CPPFLAGS = -Wall -fno-implicit-templates

main:
	g++ -c main.cpp $(CPPFLAGS) $(CPPINCS)


default: main
	g++ -o encore main.o
