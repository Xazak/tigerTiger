/* parser.cpp
DATE Nov 30, 2018
AUTH xazak
DESC Implementation of the parser module.
 */

#include "main.hpp"

// SCREEN COORDINATE POSITIONS
// +=========...
// |(0,0)
// |
// | [y][k][u]
// | [h][z][l]
// | [b][j][n]
// |
// 	        (m,n)
	/* ACTION CONVERSIONS AND CONTEXT
	KEY ACTION  CONTEXT
		IDLE    (unused by player, indicates parser awaiting input, ie between actions)
	z	WAIT    for N turns | N = 1
  hjkl	MOVE    to destination tile @X, Y
		RUN     	(as MOVE, where destination is 1+ tiles away)
		SNEAK   	(as MOVE, but without incurring stealth penalties)
		LEAP    to tile @X, Y
		POUNCE  	(COMPLEX ACTION: LEAP to target tile, then ATTACK creature)
		ATTACK  a nearby creature C
		GRAPPLE 	(as ATTACK, but trying to limit C's movement)
		BITE    	(as ATTACK, using teeth)
		SLASH   	(as ATTACK, using claws)
		RAKE    	(as ATTACK, using feet)
		GRAB    an object T at actor's feet
		CARRY   	(as GRAB, where T is small)
		DRAG    	(as GRAB, where T is large)
		DROP    a carried object
		CONSUME a food item T [at actor's feet]
		LOOK    at a distant tile @X, Y
		RUB     against a nearby object
		SIT     on the actor's occupied tile
		GROOM   self [or another nearby actor]
		OPEN    an openable container
		CLOSE   an openable container
		FORCE   a locked container to open
		CAST    a spell known by the actor
		USE     a carried object's functionality
		DIP     a carried object into another object
		PRAY    to the actor's preferred deity
		WEAR    a carried [or nearby] wearable object
		WIELD   a carried [or nearby] wieldable object
		INVEN.  show the player's list of carried objects
	*/

CmdInterpreter::CmdInterpreter():
	currAction(Sentience::Action::IDLE),
	prevAction(Sentience::Action::IDLE)
{	context = new ActionContext();
}
CmdInterpreter::~CmdInterpreter() {
	delete context;
}
void CmdInterpreter::save(TCODZip &fileBuffer) {
	LOGMSG("called");

}
void CmdInterpreter::load(TCODZip &fileBuffer) {
	LOGMSG("called");
}
void CmdInterpreter::translate() {
	// Reads input data from lastEvent/lastKey and sets the parser's state
	changeAction(keycodeLookup[lastKey.c]); // what action did the player input?
	// **** DEBUG
	// debug commands will be trapped here; don't expect them to be normal...
	switch (lastKey.c) {
		case 'm':	// spawn a monkey
			engine.map->addAnimal((engine.player->xpos - 5), (engine.player->ypos - 5));
			break;
		default:
			break;
	}
	// gather more context depending on the action being taken
	// check for certain metagame inputs: the escape menu, for example
//	Menu::MenuItemCode menuItem = Menu::MenuItemCode::NONE;
	switch (lastKey.vk) {
		case TCODK_ESCAPE:
			LOGMSG("Escape pressed");
			engine.mainMenu();
			break;
		default:
			break;
	}
	engine.player->sentience->currContext->action = currAction;
	switch(currAction) {
		case Sentience::Action::WAIT:
			// player is doing nothing for a full turn
			engine.player->sentience->currContext->echs = 1; // later we can mod this to allow multiturn waits
			break;
		case Sentience::Action::MOVE:
			// PLACE CHECK HERE FOR CONV TO RUN/SNEAK
			switch(lastKey.c) {
				case 'h': // move left
					context->echs = -1;
					break;
				case 'j': // move down
					context->whye = 1;
					break;
				case 'k': // move up
					context->whye = -1;
					break;
				case 'l': // move right
					context->echs = 1;
					break;
				case 'y': // move up-left
					context->echs = -1;
					context->whye = -1;
					break;
				case 'u': // move up-right
					context->echs = 1;
					context->whye = -1;
					break;
				case 'b': // move down-left
					context->echs = -1;
					context->whye = 1;
					break;
				case 'n': // move down-right
					context->echs = 1;
					context->whye = 1;
					break;
				default:
					break;
			}
			break;
		case Sentience::Action::RUN:
			// as MOVE, but uses less AP
			break;
		case Sentience::Action::SNEAK:
			// as MOVE, but uses more AP and decreases stealth penalties
			break;
		case Sentience::Action::CONSUME:
			// CHECK: target is... carried, at feet, nearby
			break;
		case Sentience::Action::LEAP:
			// as MOVE, but takes exactly one turn to move 2+ distance
			// always prompt for target tile
			break;
		case Sentience::Action::POUNCE:
			// as LEAP, but lands NEARBY target actor and converts to ATTACK
			break;
		case Sentience::Action::GRAPPLE:
			break;
		case Sentience::Action::ATTACK:
			break;
		case Sentience::Action::BITE:
			// as ATTACK
			break;
		case Sentience::Action::SLASH:
			// as ATTACK
			break;
		case Sentience::Action::RAKE:
			// as ATTACK
			break;
		case Sentience::Action::LOOK:
			// always prompt for target tile
			break;
		case Sentience::Action::GRAB:
			// CHECK: target is... at feet, nearby
			// ALWAYS converts to CARRY or DRAG by TARGET
			break;
		case Sentience::Action::CARRY:
			// TARGET is small
			break;
		case Sentience::Action::DRAG:
			// TARGET is large
			break;
		case Sentience::Action::DROP:
			// ALWAYS targets carried/dragged object
			break;
		case Sentience::Action::RUB:
			// TARGET is nearby
			break;
		case Sentience::Action::SIT:
			// toggles actor's 'sitting' state
			break;
		case Sentience::Action::GROOM:
			// CHECK: if no target nearby, groom self
			break;
		// IDLE ALWAYS COMES LAST
		case Sentience::Action::IDLE:
			// falls through
		default:
			break;
	}
	stateChange = actionDiegesis[currAction]; // does the action change state?
}
void CmdInterpreter::changeAction(Sentience::Action newAction) {
	prevAction = currAction;
	currAction = newAction;
}
