all: 
	g++ src/*.cpp -o tigerGame -Iinclude/libtcod -L. -ltcod -ltcodgui -Wl,-rpath=. -Wall
