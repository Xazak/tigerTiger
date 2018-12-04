/* parser.cpp
DATE Nov 30, 2018
AUTH xazak
DESC Implementation of the parser module.
 */

#include "main.hpp"

CmdInterpreter::CmdInterpreter() {
}
//~CmdInterpreter::CmdInterpreter() { }
void CmdInterpreter::translate() {
	// reads input data from lastEvent/lastKey and sets the parser's state
	changeAction(keycodeLookup[lastKey.c]); // what action did the player input?
	// action conversion routines should go here
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
	// gather more context depending on the action being taken
	context.action = currAction;
	switch(currAction) {
		case Sentience::Action::WAIT:
			// player is doing nothing for a full turn
			context.echs = 1; // later we can mod this to allow multiturn waits
			break;
		case Sentience::Action::MOVE:
			// CHECK HERE FOR CONV TO RUN/SNEAK
			context.echs = 0; // target xpos
			context.whye = 0; // target ypos
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
