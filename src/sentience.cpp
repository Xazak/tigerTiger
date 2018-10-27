#include <stdio.h>
#include <math.h>
#include "main.hpp"

void PlayerSentience::update(Actor *subject) {
	// if the player's dead, don't even try to update
	if (subject->mortality && subject->mortality->isDead()) {
		ERRMSG(": Player is dead!");
		return;
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
		case TCODK_CHAR: handleActionInput(subject, engine.lastKey.c); break;
		default: break;
	}
	// update if we've changed position
	if (xdiff != 0 || ydiff != 0) {
		engine.gameStatus=Engine::NEW_TURN;
		if (decideMoveAttack(subject, subject->xpos + xdiff, subject->ypos + ydiff)) {
			engine.map->computeFOV();
		}
	}
}
bool PlayerSentience::decideMoveAttack(Actor *subject, int targetx, int targety) {
	// returns false if the player did not move
	// is there something in the way?
//	LOGMSG("TRY: Move player to (" << targetx << ", " << targety << ")");
	if (engine.map->isWall(targetx, targety)) {
		// there's a wall
/*		LOGMSG("FAIL: Target tile is a wall \n ["\
			<< "WALL:" << engine.map->isWall(targetx, targety) << " "\
			<< "VISB:" << engine.map->isVisible(targetx, targety) << " "\
			<< "EXPL:" << engine.map->isExplored(targetx, targety) << " "\
			<< "]");*/
		engine.gui->message(TCODColor::white, "You bump your nose.");
		return false;
	} else if (engine.map->isOccupied(targetx, targety)) {
//		LOGMSG("FAIL: Target occupied-obstructed");
		// there's something or someone
//		Tile *target = &(engine.map->tiles[targetx + targety * width]);
//		engine.gui->message(TCODColor::white, "A %s is in the way.\n", target->occupant->
		return false;
	}
	// there's nothing in the way, so move the player
	subject->xpos = targetx;
	subject->ypos = targety;
	LOGMSG("MOVE-TO: " << targetx << ", " << targety << " [" \
			<< (engine.map->isWall(targetx, targety) ? "wall" : "open") \
			<< ", " << (engine.map->isOccupied(targetx, targety) ? "actor" : "empty" ) \
			<< "]");
	return true;
}
void PlayerSentience::handleActionInput(Actor *subject, int inputKeystroke) {
	// handles all player keystroke event translation
	int xdiff, ydiff = 0;
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
			engine.gameStatus=Engine::NEW_TURN;
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
				engine.gameStatus = Engine::NEW_TURN;
			}
		break;
		}
		case 'p': { //DEBUG: Print player position
			LOGMSG("Player location: (" << subject->xpos << ", " << subject->ypos << ")");
			break;
		}
	}
	// update if we've changed position
	if (xdiff != 0 || ydiff != 0) {
		engine.gameStatus=Engine::NEW_TURN;
		if (decideMoveAttack(subject, subject->xpos + xdiff, subject->ypos + ydiff)) {
			engine.map->computeFOV();
		}
	}
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
