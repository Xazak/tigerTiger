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
	/* CONVERSION CHECKS:
	MOVE
		RUN
		SNEAK
	LEAP
		POUNCE
	ATTACK
		GRAPPLE
		BITE
		SLASH
		RAKE
	GRAB
		DRAG
		CARRY
	 */
	/* ACTION CONTEXT DETAILS
	IDLE
	WAIT    for N turns | N = 1
	MOVE    to destination tile @X, Y
	RUN     	(as MOVE, where destination is 1+ tiles away)
	SNEAK   	(as MOVE, but without incurring stealth penalties)
	CONSUME a food item T [at actor's feet]
	LEAP    to tile @X, Y
	POUNCE  	(COMPLEX ACTION: LEAP to target tile, then ATTACK creature)
	ATTACK  a nearby creature C
	GRAPPLE 	(as ATTACK, but trying to limit C's movement)
	BITE    	(as ATTACK, using teeth)
	SLASH   	(as ATTACK, using claws)
	RAKE    	(as ATTACK, using feet)
	LOOK    at a distant tile @X, Y
	GRAB    an object T at actor's feet
	CARRY   	(as GRAB, where T is small)
	DRAG    	(as GRAB, where T is large)
	DROP    a carried object
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
	stateChange = actionDiegesis[currAction]; // does the action change state?
}
void CmdInterpreter::changeAction(Action newAction) {
	prevAction = currAction;
	currAction = newAction;
}
