/*	 sentience.cpp
DATE Oct 24 2018
AUTH xazak
DESC Contains action packages that provide decision-making tools for Actors.
 */
#include <stdio.h>
#include <math.h>
#include "main.hpp"

// Player Sentience -- command interpreter, context handlers, etc
bool PlayerSentience::update(Actor *subject) {
//	LOGMSG(" called ");
	// if the player's dead, don't even try to update
	bool stateChange = false;
	if (subject->mortality && subject->mortality->isDead()) {
		ERRMSG(": Player is dead!");
		return stateChange;
	}
	int xdiff = 0; // target x-coord
	int ydiff = 0; // target y-coord
	// check the TCOD keyboard bindings first
	// if it's not a special key, pass it to the action handler
	switch (engine.lastKey.vk) {
		case TCODK_UP:		ydiff =- 1; break;
		case TCODK_DOWN:	ydiff =  1; break;
		case TCODK_LEFT:	xdiff =- 1; break;
		case TCODK_RIGHT:	xdiff =  1; break;
		case TCODK_CHAR:
			stateChange = handleActionInput(subject, engine.lastKey.c);
			break;
		default: break;
	}
	// update if we've changed position
	if (xdiff != 0 || ydiff != 0) {
//		engine.gameStatus=Engine::NEW_TURN;
		if (decideMoveAttack(subject, subject->xpos + xdiff, subject->ypos + ydiff)) {
			engine.map->computeFOV();
			stateChange = true;
		}
	}
	return stateChange;
}
bool PlayerSentience::decideMoveAttack(Actor *subject, int targetx, int targety) {
	// returns false if the player did not move
	// is there something in the way?
//	LOGMSG("TRY: Move player to (" << targetx << ", " << targety << ")");
	if (engine.map->isWall(targetx, targety)) {
		// there's a wall
		engine.gui->message(TCODColor::white, "You bump your nose.");
		return false;
	} else if (engine.map->isOccupied(targetx, targety)) {
		// there's something or someone
		engine.gui->message(TCODColor::white, "There's someone in the way.");
		return false;
	}
	// there's nothing in the way, so move the player
	subject->xpos = targetx;
	subject->ypos = targety;
//	LOGMSG("Player position: " << targetx << ", " << targety);
/*	LOGMSG("MOVE-TO: " << targetx << ", " << targety << " [" \
			<< (engine.map->isWall(targetx, targety) ? "wall" : "open") \
			<< ", " << (engine.map->isOccupied(targetx, targety) ? "actor" : "empty" ) \
			<< "]");
	*/
//	subject->tempo->deductAP(100);
	LOGMSG(subject->name << " deducted 100 AP");
	return true;
}
bool PlayerSentience::handleActionInput(Actor *subject, int inputKeystroke) {
	// handles all player keystroke event translation
	int xdiff, ydiff = 0;
	bool stateChange = false;
	switch(inputKeystroke) {
		case 'y': { // move NW
			xdiff = -1;
			ydiff = -1;
			break;
		}
		case 'u': { // move NE
			xdiff =  1;
			ydiff = -1;
			break;
		}
		case 'b': { // move SW
			xdiff = -1;
			ydiff =  1;
			break;
		}
		case 'n': { // move SE
			xdiff =  1;
			ydiff =  1;
			break;
		}
		case 'k': { // move player up
			ydiff = -1;
			break;
		}
		case 'j': { // move player down
			ydiff =  1;
			break;
		}
		case 'h': { // move player left
			xdiff = -1;
			break;
		}
		case 'l': { // move player right
			xdiff =  1;
			break;
		}
		case 'd': { //DROP item
			Actor *object = chooseFromInventory(subject);
			if (object) {
				object->portable->drop(object, subject);
//				engine.gameStatus = Engine::NEW_TURN;
			}
			stateChange = true;
			break;
		}
		case 'g': { //GET item
/*			bool found = false;
			for (Actor **iter = engine.actors.begin(); iter != engine.actors.end(); iter++) {
				Actor *object = *iter;
				// is the object get-able AND
				// AND is the subject standing over the object?
				if (object->portable && object->xpos == subject->xpos && actor->ypos == subject->ypos) {
					if (actor->portable->pickUp(subject, subject)) {
						found = true;
						engine.gui->message(TCODColor::lightGrey, "You pick up the %s.", object->name);
						break;
					} else if (!found) {
						found = true;
						engine.gui->message(TCODColor::red, "Your inventory is full.");
					}
				}
			}
			if (!found) {
				engine.gui->message(TCODColor::lightGrey, "There is nothing here to pick up.");
			}*/
			// Check the ground under the player's feet for things to GET
			// is there actually anything on this tile?
//			if (engine.map->tiles[subject->xpos + subject->ypos * engine.map->width]->itemList) { }
			// if there is, AND there's just the one thing, pick it up
			// if there's more than one object, invoke a menu
//			engine.gameStatus=Engine::NEW_TURN;
			stateChange = true;
			break;
		}
		case 'i': { //INVENTORY list
			// this method requires the player to be Portable, which makes no
			// sense at all - when would an NPC pick up the Player? - so the
			// logic for invoking the player's inventory self-container should
			// be moved elsewhere, probably into the Container class itself?
			Actor *actor = chooseFromInventory(subject);
			if (actor) {
				actor->portable->use(actor, subject);
//				engine.gameStatus = Engine::NEW_TURN;
			}
		break;
		}
		case 'p': { //DEBUG: Print player position
			LOGMSG("P-LOC abs: " << subject->xpos << ", " << subject->ypos);
			break;
		}
		case 'm': { //DEBUG: Spawn a monkey at a random screen location
			engine.map->addAnimal(subject->xpos - 5, subject->ypos - 5);
			break;
		}
	}
	// update if we've changed position
	if (xdiff != 0 || ydiff != 0) {
//		engine.gameStatus=Engine::NEW_TURN;
		// the if-then statement below prints spurious collision messages...
		engine.map->computeFOV();
		stateChange = true;
	}
	return stateChange;
}
Actor *PlayerSentience::chooseFromInventory(Actor *subject) {
	// set up the inventory selection menu
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);
	// display the inventory menu frame
	con.setDefaultForeground(TCODColor::darkRed);
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "Inventory");
	// display the inventory list
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int index = 1;
	for (Actor **iter = subject->container->inventory.begin();
		iter != subject->container->inventory.end(); iter++) {
			Actor *object = *iter;
			con.printf(2, index, "(%c) %s", shortcut, object->name);
			shortcut++;
			index++;
	}
	// blit the console onto the main screen
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
		TCODConsole::root, (engine.screenWidth / 2) - (INVENTORY_WIDTH / 2),
		(engine.screenHeight / 2 ) - (INVENTORY_HEIGHT / 2));
	TCODConsole::flush();
	// wait for a keypress
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if (key.vk == TCODK_CHAR) {
		int actorIndex = key.c - 'a';
		if (actorIndex >= 0 && actorIndex < subject->container->inventory.size()) {
			return subject->container->inventory.get(actorIndex);
		}
	}
	return NULL;
}
int PlayerSentience::getCheapestActionCost() {
	return 100;
}
// NPC Sentience -- AI routines, context handlers, etc
bool AnimalSentience::update(Actor *subject) {
	// This is the core decision-making function: it will call any other
	// functions needed for an NPC to decide what to do on their turn
	bool stateChange = false;
	// if the subject is dead, don't even try (quietly)
	if (subject->mortality && subject->mortality->isDead()) {
		// might need to add logic that removes dead actors from engine queue
		return stateChange;
	}
	if (engine.map->isVisible(subject->xpos, subject->ypos)) {
		// the player is visible, let's get closer
		if (subject->getDistance(engine.player->xpos, engine.player->ypos) <= 3) {
			engine.gui->message(TCODColor::white, "%s dances and hoots!", subject->name);
			stateChange = true;
		} else {
			stateChange = decideMoveAttack(subject, engine.player->xpos, engine.player->ypos);
		}
	} else {
		// the player's not visible, let's just pick a random wander
		TCODRandom *rng = TCODRandom::getInstance();
		int targetx = subject->xpos + rng->getInt(-1, 1);
		int targety = subject->ypos + rng->getInt(-1, 1);
		decideMoveAttack(subject, targetx, targety);
		stateChange = true;
	}
	return stateChange;
}
bool AnimalSentience::decideMoveAttack(Actor *subject, int targetx, int targety) {
	// FIX: the movement code does NOT prevent actors from occupying the same
	// tile as the player!
	// decides whether the NPC should move to or attack the target square
	int xdiff = targetx - subject->xpos;
	int ydiff = targety - subject->ypos;
	float distance = sqrtf(xdiff * xdiff + ydiff * ydiff);
	if (distance >= 1) { // how far away is the target tile?
		// if it's more than a single step away, move in that direction
		// do not allow the actor to take more than 1 step in any direction
		xdiff = (int)(round(xdiff / distance));
		ydiff = (int)(round(ydiff / distance));
		if (xdiff > 1) {
			xdiff = xdiff / xdiff;
		} else if (xdiff < -1) {
			xdiff = xdiff / -(xdiff);
		}
		if (ydiff > 1) {
			ydiff = ydiff / ydiff;
		} else if (ydiff < -1) {
			ydiff = ydiff / -(ydiff);
		}
	}
	targetx = subject->xpos + xdiff;
	targety = subject->ypos + ydiff;
/*	LOGMSG (subject->name \
			<< " @" << subject->xpos << "," << subject->ypos \
			<< " -> " << targetx << "(" << xdiff \
			<< "), " << targety << "(" << ydiff << ")");
	*/
	if (engine.map->isWall(targetx, targety)) {
		// there's a wall
		engine.gui->message(TCODColor::white, "The %s bumps into a wall.", subject->name);
		return true;
	} else if (engine.map->isOccupied(targetx, targety)) {
		// there's something or someone
		engine.gui->message(TCODColor::white, "The %s can't get past.", subject->name);
		return true;;
	}
/*	LOGMSG(subject->name \
			<< "\n abs(" << subject->xpos << "," << subject->ypos << ")\n" \
			<< " tgt(" << targetx << "," << targety << ")\n" \
			<< "diff(" << xdiff << "," << ydiff << ")");
	*/
	subject->xpos += xdiff;
	subject->ypos += ydiff;
//	subject->tempo->deductAP(100);
	LOGMSG(subject->name << " deducted 100 AP");
	return true;
}
int AnimalSentience::getCheapestActionCost() {
	return 100;
}
