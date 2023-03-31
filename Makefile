CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SolEngine: *.cpp *.hpp
	g++ $(CFLAGS) -o SolEngine *.cpp $(LDFLAGS)

.PHONY: test clean

test: SolEngine
	./SolEngine

clean:
	rm -f SolEngine