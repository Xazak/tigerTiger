/*	 main.cpp
DATE Oct 05 2018
AUTH xazak
DESC Implementation of main game loop and assoc. functions
 */
#include "main.hpp"

typedef unsigned int uint;

CmdInterpreter parser; // create a command parser
GameEngine engine(80, 50); // init game engine with window size

int main() {
	// starts the game engine: brings up main menu and checks for saves
	// RNG, map, time, etc. objects are NOT GUARANTEED until the loop starts!
	engine.load();
	while (!TCODConsole::isWindowClosed()) { // main game loop
		TCODConsole::flush(); // this needs to happen before gathering input
		// wait until player hits a key, and get the input context with it
		parser.lastEvent = TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,
				&parser.lastKey, NULL, true);
		// perform input interpretation and change parser state to match
		parser.translate();
		// check if the game state should be iterated
		if (parser.stateChange == true) {
			engine.update();
			parser.changeAction(CmdInterpreter::Action::IDLE);
		}
		engine.render();
	}
	engine.save();
	return 0; // NOTE: does this need to return exception codes?
}
