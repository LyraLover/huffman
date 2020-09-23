cpp = main.cpp compress/compress.cpp decompress/decompress.cpp

all: clean $(cpp)
	mkdir build
	$(CXX) $(cpp) -std=c++20 -o build/huffman

clean:
	rm -rf build/
