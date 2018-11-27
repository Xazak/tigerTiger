#include "main.hpp"

typedef unsigned int uint;

GameEngine engine(80, 50); // init game engine with window size

int main() {
	engine.load(); // invoke game engine
	// first game loop: runs until window is closed by player
	while (!TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	engine.save();
	return 0; // NOTE: does this need to return exception codes?
}
