#include "main.hpp"

static const int MAP_HEIGHT = 1000;
static const int MAP_WIDTH = 1000;
//static const int PLAYER_START_X = 30;
//static const int PLAYER_START_Y = 30;
static const int PLAYER_START_X = 965;
static const int PLAYER_START_Y = 965;

// *** ENGINE STATES
/*	ENGINE STATE MACHINE PROCESS
	GIVEN:
	Q: a queue of local actors with full AP
	I: an iterator pointing into Q
	S: the actor under consideration
	G: the current game state ( = NEW_TURN at start)
	A: the number of actions taken during this turn
	METHOD:
I		? is G = NEW_TURN?
		Y:  REFRESH AP (fill AP pools)
			SORT Q by AP
	SET A = 0
	SET I = Q.start
II		SET S = I
	? does S have remaining AP?
		Y- S.update
		>> IF PLAYER, G = IDLE; wait for game state to change; G = (previous)
		>> IF ANIMAL, get NPC update
		SET I = Q.previous
		MOVE S within Q to new position by AP
		SET A+1
		SET I = Q.next
		GOTO <II>
	? did anyone move this turn? (A ?= 0)
		Y- SET G = ONGOING
		N- SET G = NEW_TURN
		GOTO <I>
*/
void StartupTurn::update() {
	// perform any start-of-game bookkeeping, then start a new turn
	// force an FOV update to ensure that the map appears on the first turn
	engine.map->computeFOV();
	// start a new turn
	engine.switchMode(&newTurn);
}
void NewTurn::update() {
	// perform new-turn bookkeeping before asking the first actor to update
	// refresh actor AP pools and put the queue in order
	engine.time->updateQueue();
	engine.time->refresh();
	engine.time->sort();
	// start asking actors to update
	engine.switchMode(&ongoingTurn);
}
void OngoingTurn::update() {
	// perform per-turn processing until the player's turn comes up
	// ask whoever's in line next to update
	Actor *subject = engine.time->actionQueue->next;
	if (subject == engine.player) {
		// if it's the player, change to IDLE and stay there until told otherwise
		engine.switchMode(&idleMode);
	} else {
		// it's not the player, so ask the NPC to update and move on
		subject->update();
		// when the actor's finished with their turn, move them down the queue
		engine.time->actionQueue->adjustActor(subject);
	}
	// if no one else is waiting in line to update, start a new turn
	if (engine.time->actionQueue.empty()) {
		engine.switchMode(&newTurn);
	}
}
void IdleMode::update() {
	// perform player input processing until they change the game state
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &engine.lastKey, NULL);
	// invoke the main menu if the player hit ESC
	if (engine.lastKey.vk == TCODK_ESCAPE) {
		engine.save();
		engine.load();
	} else {
		// ask the player-actor to update()
	}
}
void VictoryMode::update() {
	// perform victory bookkeeping
	LOGMSG("*** Victory mode engaged");
}
void DefeatMode::update() {
	// perform defeat bookkeeping
	LOGMSG("*** Defeat mode engaged");
}
// *** ENGINE MECHANICS
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
	delete time;
}
// *** MAJOR FUNCTIONS
void Engine::init() {
	// create a player object
	player = new Actor(PLAYER_START_X, PLAYER_START_Y, '@', TCODColor::orange, "player");
	player->sentience = new PlayerSentience();
//	LOGMSG("sentience OK");
	player->mortality = new PlayerMortality(69, 47, "your corpse");
//	LOGMSG("mortality OK");
	player->tempo = new ActorClock(100);
//	LOGMSG("temporality OK");
	player->container = new Container(26);
//	LOGMSG("inventory OK");
	// specific player ability initializations go here
	//player-pickable ?
	//player-attack
	actors.push(player);
	// initialize the map, show the MOTD
	map = new Map(MAP_WIDTH, MAP_HEIGHT); // init a new map
	map->init(true);
	engine.gui->refreshViewport(); // force a viewport update
	gui->message(TCODColor::orange,
		"Tiger Tiger, burning bright,\nIn the forests of the night;\nWhat immortal hand or eye,\nCould frame thy fearful symmetry?");
	//INIT TIMEKEEPER HERE
	time = new GameClock(); // create a world clock and set up AP tracking
	gameStatus = STARTUP;
	LOGMSG("gameStatus: " << gameStatus);
}
void Engine::term() {
	actors.clearAndDelete(); // delete all actors
	if (map) delete map; // delete the map if it still exists
	gui->clear(); // wipe the GUI
}
void Engine::update() {
	currMode->update();
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
		if (actor != player && map->isVisible(actor->xpos, actor->ypos)) {
//			LOGMSG("Rendering actor " << actor->name);
			actor->render();
		}
		player->render();
	}
	gui->blitToScreen();
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
		LOGMSG("gameStatus: " << gameStatus);
	}
}
void Engine::switchMode(EngineState *newMode) {
	prevMode = currMode;
	currMode = newMode;
	LOGMSG("mode switch: " << currMode);
}
// *** MINOR FUNCTIONS
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
