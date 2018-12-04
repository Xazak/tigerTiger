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
	switch(currAction) {
		case WAIT:
			// do nothing
			break;
		default:
			break;
	}
	stateChange = actionDiegesis[currAction]; // does the action change state?
}
void CmdInterpreter::changeAction(Action newAction) {
	prevAction = currAction;
	currAction = newAction;
}
