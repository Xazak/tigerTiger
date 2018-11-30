/*	 main.cpp
DATE Oct 05 2018
AUTH xazak
DESC Implementation of main game loop and assoc. functions
 */
#include "main.hpp"

typedef unsigned int uint;

GameEngine engine(80, 50); // init game engine with window size
CmdInterpreter parser; // create a command parser

int main() {
	engine.load(); // invoke game engine such that we can load a saved game
	// first game loop: runs until window is closed by player
	while (!TCODConsole::isWindowClosed()) {
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
