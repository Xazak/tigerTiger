#include "main.hpp"

static const int MAP_HEIGHT = 1000;
static const int MAP_WIDTH = 1000;
static const int PLAYER_START_X = 100;
static const int PLAYER_START_Y = 100;

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
//	int xLoc = (engine.screenWidth - (engine.screenWidth / 4)) / 2;
//	int yLoc = (engine.screenHeight - (engine.screenHeight / 4)) / 2;
//	player = new Actor(xLoc, yLoc, '@', TCODColor::orange, "player");
	player = new Actor(PLAYER_START_X, PLAYER_START_Y, '@', TCODColor::orange, "player");
	player->sentience = new PlayerSentience();
//	LOGMSG("sentience OK");
	player->mortality = new PlayerMortality(69, 47, "your corpse");
//	LOGMSG("mortality OK");
	player->container = new Container(26);
//	LOGMSG("inventory OK");
	// specific player ability initializations go here
	//player-pickable ?
	//player-attack
	actors.push(player);
	// initialize the map, show the MOTD
	map = new Map(MAP_WIDTH, MAP_HEIGHT); // init a new map
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
/* SCREEN DISPLAY LAYER MODEL
	-PLAYER-

	[scentLayer]
	<visual FX: fire, etc>
	<Actors>
	<Objects>
	<Map Tiles>
	<GUI Information>
	<GUI Frames>
	We will draw them on the screen in order from the bottom, so that each new
	layer overwrites the previous with more important detail and context.
 */
	gui->render(); // draw the GUI
	map->render(); // draw the map
	// draw all visible actors
	for (Actor **iter = actors.begin(); iter != actors.end(); iter++) {
		Actor *actor = *iter;
		if (map->isVisible(actor->xpos, actor->ypos)) {
			actor->render();
		}
		player->render();
	}
	gui->blitToScreen();
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
				if (map->isVisible(cx, cy) && (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)) {
//					TCODColor color = TCODConsole::root->getCharBackground(cx, cy);
					TCODColor color = engine.gui->viewport->getCharBackground(cx, cy);
					color = color * 1.2f;
//					TCODConsole::root->setCharBackground(cx, cy, color);
					engine.gui->viewport->setCharBackground(cx, cy, color);
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
		if (actor->xpos == x && actor->ypos == y) {
			return actor;
		}
	}
	return NULL;
}
void Engine::save() {
	/*if (player->destructible->isDead()) {
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
	}*/
}
void Engine::load() {
	engine.gui->menu.clear(); // wipe the menu to ensure no artifacts
	engine.gui->menu.addItem(Menu::NEW_GAME, "New Game");
	// add "Continue" if there's a save game to continue with
	if (TCODSystem::fileExists("game.sav")) {
		engine.gui->menu.addItem(Menu::CONTINUE, "Continue");
	}
	engine.gui->menu.addItem(Menu::EXIT, "Exit");
	// handle menu choice input
	Menu::MenuItemCode menuItem = engine.gui->menu.pick();
	if (menuItem == Menu::EXIT || menuItem == Menu::NONE) {
		// Exit or user closed window
		exit(0);
	} else if (menuItem == Menu::NEW_GAME) {
		// New game
		engine.term();
		engine.init();
	} else {
		/*TCODZip zip;
		// Continue a saved game
		engine.term();
		zip.loadFromFile("game.sav");
		int width = zip.getInt();
		int height = zip.getInt();
		map = new Map(width, height);
		map->load(zip);
		player = new Actor(0, 0, 0, NULL, TCODColor::white);
		actors.push(player);
		player->load(zip);
		int nbActors = zip.getInt();
		while (nbActors > 0) {
			Actor *actor = new Actor(0, 0, 0, NULL, TCODColor::white);
			actor->load(zip);
			actors.push(actor);
			nbActors--;
		}
		// the message log
		gui->load(zip);*/
		// force FOV computation
		gameStatus = STARTUP;
	}
}
