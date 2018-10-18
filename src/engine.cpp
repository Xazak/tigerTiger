#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight):
	gameStatus(STARTUP), player(NULL), map(NULL), fovRadius(10),
	screenWidth(screenWidth), screenHeight(screenHeight) {
	// initialize the console
	TCODConsole::initRoot(screenWidth, screenHeight, "TIGER!TIGER!", false);
	gui = new Gui(); // init the GUI
}
Engine::~Engine() {
	term();
	delete gui;
}
void Engine::term() {
	actors.clearAndDelete(); // delete all actors
	if (map) delete map; // delete the map if it still exists
	gui->clear(); // wipe the GUI
}
void Engine::init() {
	// create a player object
	player = new Actor(40, 25, '@', TCODColor::orange, "player");
	// specific player ability initializations go here
	actors.push(player);
	// initialize the map, show the MOTD
	map = new Map(80, 43); // init a new map
	map->init(true);
	gui->message(TCODColor::orange,
		"Tiger Tiger, burning bright,\nIn the forests of the night;\nWhat immortal hand or eye,\nCould frame thy fearful symmetry?");
	gameStatus=STARTUP;
}
void Engine::update() {
	if (gameStatus == STARTUP) map->computeFOV(); // force FOV update
	gameStatus = IDLE; // await player input
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
	// invoke the main menu when the player hits Esc
	if (lastKey.vk == TCODK_ESCAPE) {
//		save();
//		load();
	}
	// the player gets to go first
	player->update();
	// then the NPCs make their moves
	if (gameStatus == NEW_TURN) {
		for (Actor **iter = actors.begin(); iter != actors.end(); iter++) {
			Actor *actor = *iter;
			if (actor != player) {
				actor->update();
			}
		}
	}
}
void Engine::render() {
	TCODConsole::root->clear(); // clear any console leftovers
	map->render(); // draw the map
	gui->render(); // update the gui
	// draw the actors
	for (Actor **iter=actors.begin(); iter != actors.end(); iter++) {
		Actor *actor = *iter;
		if (map->isInFOV(actor->actorX, actor->actorY)) {
			actor->render();
		}
		player->render();
		// show the player's stats
//		TCODConsole::root->print(1, screenHeight-2, "HP: %d/%d", (int)player->destructible->curHP, (int)player->destructible->maxHP);
	}
}
void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}
Actor *Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closestMonster = NULL;
	float bestDistance = 1E6f; //1E6 == 1000000; how far away are we allowed to select?
	for (Actor **iter = actors.begin(); iter != actors.end(); iter++) {
		Actor *actor = *iter;
//		if (actor != player && actor->destructible && !actor->destructible->isDead()) {
		if (actor != player) {
			float distance = actor->getDistance(x, y);
			if (distance < bestDistance && (distance <= range || range == 0.0f )) {
				bestDistance = distance;
				closestMonster = actor;
			}
		}
	}
	return closestMonster;
}
bool Engine::pickATile(int *x, int *y, float maxRange) {
	while (!TCODConsole::isWindowClosed()) {
		render();
		// highlight the possible range
		for (int cx = 0; cx < map->width; cx++) {
			for (int cy = 0; cy < map->height; cy++) {
				if (map->isInFOV(cx, cy) && (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)) {
					TCODColor color = TCODConsole::root->getCharBackground(cx, cy);
					color = color * 1.2f;
					TCODConsole::root->setCharBackground(cx, cy, color);
				}
			}
		}
		// mouse and ??? handling
/*		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
		if (map->isInFOV(mouse.cx, mouse.cy) && 
				(maxRange == 0 || player->getDistance(mouse.cx, mouse.cy) <= maxRange)) {
			TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);
			if (mouse.lbutton_pressed) {
				*x = mouse.cx;
				*y = mouse.cy;
				return true;
			}
		}
		if (mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
			return false;
		}*/
		TCODConsole::flush();
	}
	return false;
}
Actor *Engine::getActor(int x, int y) const {
	for (Actor **iter = actors.begin(); iter != actors.end(); iter++) {
		Actor *actor = *iter;
//		if (actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) {
		if (actor->actorX == x && actor->actorY == y) {
			return actor;
		}
	}
	return NULL;
}
