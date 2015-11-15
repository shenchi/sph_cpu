CXX=g++
LDFLAGS=-L/usr/local/lib -framework OpenGL -lglew -lglfw3
CXXFLAGS=-I/usr/local/include

all:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o sph src/*.cpp
