all: main.cpp
	mkdir -p build
	g++ main.cpp -o build/deluxe -std=c++17

clean:
	rm -rf build