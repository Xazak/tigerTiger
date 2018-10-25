#include <stdio.h>
#include <math.h>
#include "main.hpp"

void PlayerSentience::update(Actor *subject) {
	// if the player's dead, don't even try to update
	if (subject->destructible && subject->destructible->isDead()) {
		clog << "*** PlayerSentience::update() > Player is dead!\n";
		return;
	}
	int dx = 0; // target x-coord
	int dy = 0; // target y-coord
	// discover our target square
	switch (engine.lastKey.vk) {
		case TCODK_UP:		ydiff =- 1; break;
		case TCODK_DOWN:	ydiff =  1; break;
		case TCODK_LEFT:	xdiff =- 1; break;
		case TCODK_RIGHT:	xdiff =  1; break;
		case TCODK_CHAR: handleActionKey(subject, engine.lastKey.c); break;
		default: break;
	}
	// update if we've changed position
	if (dx != 0 || dy != 0) {
		engine.gameStatus=Engine::NEW_TURN;
		if (moveOrAttack(subject, subject->xpos + xdiff, subject->ypos + ydiff)) {
			engine.map->computeFOV();
		}
	}
}
bool PlayerSentience::decideMoveAttack(Actor *subject, int targetx, int targety) {
	// returns false if the player did not move
	if (engine.map->isObstructed(targetx, targety)) {
		if (engine.map->isOccupied(targetx, targety)) {
			// do something about the living creature
		} else {
			// do something about the inanimate object
		}
	}
	// there's nothing in the way
	subject->xpos = targetx;
	subject->ypos = targety;
	return true;
}
void PlayerAi::handleActionInput(Actor *subject, int inputKeystroke) {
	// handles all player keystroke event translation
	switch(inputKeystroke) {
		case 'd': { //DROP item
			Actor *object = choseFromInventory(subject);
			if (object) {
				object->pickable->drop(object, subject);
				engine.gameStatus = Engine::NEW_TURN;
			}
			break;
		}
		case 'g': { //GET item
/*			bool found = false;
			for (Actor **iter = engine.actors.begin(); iter != engine.actors.end(); iter++) {
				Actor *object = *iter;
				// is the object get-able AND
				// AND is the subject standing over the object?
				if (object->pickable && object->xpos == subject->xpos && actor->ypos == subject->ypos) {
					if (actor->pickable->pickUp(owner, subject)) {
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
			if (engine.map->tiles[subject->xpos + subject->ypos * engine.map->width]->itemList) { }
			// if there is, AND there's just the one thing, pick it up
			// if there's more than one object, invoke a menu
			engine.gameStatus=Engine::NEW_TURN;
			break;
		}
		case 'i': { //display inventory
			Actor *actor = choseFromInventory(subject);
			if (actor) {
				actor->pickable->use(actor, subject);
				engine.gameStatus = Engine::NEW_TURN;
			}
		break;
		}

	}
}
