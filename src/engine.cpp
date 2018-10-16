#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) :
	gameStatus(STARTUP), player(NULL), map(NULL), fovRadius(10),
	screenWidth(screenWidth), screenHeight(screenHeight) {
	// initialize the console
	TCODConsole::initRoot(80, 50, "libtcod C++ tutorial", false);
	gui = new Gui(); // set up the GUI

}
Engine::~Engine() {
	actors.clearAndDelete(); //TCOD helper fxn
	delete map;
	delete gui;
}
void Engine::init() {
	// create a player object
	player = new Actor(40, 25, '@', "player", TCODColor::white);
	player->destructible = new PlayerDestructible(30, 2, "your cadaver");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push(player);
	// initialize the map, show the MOTD
	map = new Map(80, 43); // init a new map
	map->init(true);
	gui->message(TCODColor::red,
		"Welcome stranger!\nPrepare to perish in the Tombs!");
}
void Engine::update() {
	if (gameStatus == STARTUP) map->computeFOV();
	gameStatus = IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, &mouse);
	player->update();
	if (gameStatus == NEW_TURN) {
		map->currentScentValue++;
		for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
			Actor *actor = *iterator;
			if (actor != player) {
				actor->update();
			}
		}
	}
}
void Engine::render() {
	TCODConsole::root->clear(); // clear any console leftovers
	map->render();				// draw the map
	gui->render();				// update the gui
	// draw the actors
	for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (map->isInFOV(actor->x, actor->y)) {
			actor->render();
		}
		player->render();
		// show the player's stats
		//NOTE: this function has been deprecated, use the new version!
		TCODConsole::root->print(1, screenHeight-2,
			"HP: %d/%d", (int)player->destructible->curHP,
			(int)player->destructible->maxHP);
	}
}
void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}
Actor *Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closest = NULL;
	float bestDistance = 1E6f; //1E6 == 1000000
	for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor != player && actor->destructible && !actor->destructible->isDead()) {
			float distance = actor->getDistance(x, y);
			if (distance < bestDistance && (distance <= range || range == 0.0f )) {
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}
bool Engine::pickATile(int *x, int *y, float maxRange) {
	while (!TCODConsole::isWindowClosed()) {
		render();
		// highlight the possible range
		for (int cx = 0; cx < map->width; cx++) {
			for (int cy = 0; cy < map->height; cy++) {
				if (map->isInFOV(cx, cy) && (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
					col = col * 1.2f;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
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
		}
		TCODConsole::flush();
	}
	return false;
}
Actor *Engine::getActor(int x, int y) const {
	for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) {
			return actor;
		}
	}
	return NULL;
}
void Engine::save() {
	if (player->destructible->isDead()) {
		TCODSystem::deleteFile("game.sav");
	} else {
		TCODZip zip;
		//save the map first
		zip.putInt(map->width);
		zip.putInt(map->height);
		map->save(zip);
		// then the player
		player->save(zip);
		// then the other actors
		zip.putInt(actors.size()-1);
		for (Actor **it = actors.begin(); it != actors.end(); it++) {
			if (*it != player) {
				(*it)->save(zip);
			}
		}
		// then the message log
		gui->save(zip);
		zip.saveToFile("game.sav");
	}
}
void Engine::load() {
	if (TCODSystem::fileExists("game.sav")) {
		TCODZip zip;
		zip.loadFromFile("game.sav");
		// load the map
		int width = zip.getInt();
		int height = zip.getInt();
		map = new Map(width, height);
		map->load(zip);
		// load the player
		player = new Actor(0, 0, 0, NULL, TCODColor::white);
		player->load(zip);
		actors.push(player);
		// load everyone else
		int nbActors = zip.getInt();
		while (nbActors > 0) {
			Actor *actor = new Actor(0, 0, 0, NULL, TCODColor::white);
			actor->load(zip);
			actors.push(actor);
			nbActors--;
		}
		// load the message log
		gui->load(zip);
	} else {
		engine.init();
	}
}
