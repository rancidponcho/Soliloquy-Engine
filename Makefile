CFLAGS = -std=c++17 -O2 -g -I$(TINYOBJ_PATH)
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

GLSLC = ~/dev/tools/glslc
TINYOBJ_PATH = /home/rancidponcho/dev/tools/tinyobjloader

vertSrc = $(wildcard shaders/*.vert)
vertObj = $(patsubst %.vert, %.vert.spv, $(vertSrc))
fragSrc = $(wildcard shaders/*.frag)
fragObj = $(patsubst %.frag, %.frag.spv, $(fragSrc))

TARGET = SolEngine
$(TARGET): $(vertObj) $(fragObj)
$(TARGET): *.cpp *.hpp
	g++ $(CFLAGS) -o $(TARGET) *.cpp $(LDFLAGS)

%.spv: %
	$(GLSLC) $< -o $@

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f shaders/*.spv