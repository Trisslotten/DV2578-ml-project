CXXFLAGS = `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3` `pkg-config --static --libs glew`


all:
	$(CXX) src/*.cpp $(CXXFLAGS)
