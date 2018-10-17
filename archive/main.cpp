#include "main.hpp"

Engine engine(80, 50);

int main() {
	engine.load();
	// outermost game loop: runs until player closes game window
	while ( !TCODConsole::isWindowClosed() ) {
		engine.update();
		engine.render();
		TCODConsole::flush();
		}
	engine.save();
	return 0;
}

