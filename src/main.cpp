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
		engine.render();
		TCODConsole::flush(); // this needs to happen before gathering input
		// WAIT until player hits a key, and get the input context with it
//		parser.lastEvent = TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,
//				&parser.lastKey, NULL, true);
		// CHECK to see if a player hit a key, then gather its context
		parser.lastEvent = TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS,
				&parser.lastKey, NULL);
		// perform input interpretation and change parser state to match
		if (parser.lastEvent != TCOD_EVENT_NONE) {
			parser.translate();
		}
		// check if the game state should be iterated
/*		if (parser.stateChange == true) {
			engine.update();
			parser.changeAction(Sentience::Action::IDLE);
		}*/
		engine.update();
	}
	engine.save();
	return 0; // NOTE: does this need to return exception codes?
}
