/*	 engine.cpp
DATE Oct 08 2018
AUTH xazak
DESC Implementation of the game engine, which handles update and render calls.
 */
#include "main.hpp"

static const int MAP_HEIGHT = 1000;
static const int MAP_WIDTH = 1000;
//static const int PLAYER_START_X = 30;
//static const int PLAYER_START_Y = 30;
static const int PLAYER_START_X = 299;
static const int PLAYER_START_Y = 299;

/*void GameEngine::Startup::update() {
	// perform any start-of-game bookkeeping, then start a new turn
	// force an FOV update to ensure that the map appears on the first turn
	map->computeFOV();
	// start a new turn
	switchMode(&newTurn);
}
void GameEngine::NewTurn::update() {
	// perform new-turn bookkeeping before asking the first actor to update
	// refresh actor AP pools and put the queue in order
//	engine.time->updateQueue();
//	engine.time->refresh();
//	engine.time->sort();
	actionQueue.clear();
	for (Actor **iter = allActors.begin(); iter != allActors.end(); iter++) {
		Actor *subject = *iter;
		actionQueue.push(subject);
	}
	// start asking actors to update
	switchMode(&ongoing);
}
void GameEngine::Ongoing::update() {
	// perform per-turn processing until the player's turn comes up
	// ask whoever's in line next to update
//	Actor *subject = engine.time->getNextActor();
	Actor *subject = allActors.begin();
	if (subject == player) {
		// if it's the player, change to IDLE and stay there until told otherwise
		switchMode(&idle);
	} else {
		// it's not the player, so ask the NPC to update and move on
		subject->update();
		// when the actor's finished with their turn, move them down the queue
//		engine.time->advanceQueue();
	}
	// if no one else is waiting in line to update, start a new turn
	if (engine.time->isQueueEmpty()) {
		engine.switchMode(&newTurn);
	}
}
void GameEngine::Idle::update() {
	// perform player input processing until they change the game state
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
	// invoke the main menu if the player hit ESC
	if (lastKey.vk == TCODK_ESCAPE) {
		save();
		load();
	} else {
		// ask the player-actor to update()
		if (player->update()) {
			// the player has changed the game state
			switchMode(&ongoing);
		}
	}
}
void VictoryMode::update() {
	// perform victory bookkeeping
	LOGMSG("*** Victory mode engaged");
}
void DefeatMode::update() {
	// perform defeat bookkeeping
	LOGMSG("*** Defeat mode engaged");
}*/
// *** ENGINE MECHANICS
GameEngine::GameEngine(int screenWidth, int screenHeight):
	screenWidth(screenWidth), screenHeight(screenHeight),
	fovRadius(50)
	{
	// initialize the console
	TCODConsole::initRoot(screenWidth, screenHeight, "TIGER!TIGER!", false);
	gui = new GameGUI(); // init the GUI
}
GameEngine::~GameEngine() {
	term();
	delete gui;
}
// *** MAJOR FUNCTIONS
void GameEngine::init() {
	// initialize the map
	// later, we can set this seed to generate randomly or whatever
	seed = 0x0a987654;
	rng = new TCODRandom(seed, TCOD_RNG_MT); // start up the RNG using mersenne
	map = new GameMap(MAP_WIDTH, MAP_HEIGHT); // init a new map
	map->init(true);
	LOGMSG("map initialized");
	chrono = new WorldClock(); // create a world clock
	LOGMSG("world clock started");
	// create a player object
	player = new Actor(PLAYER_START_X, PLAYER_START_Y, 't',
			TCODColor::orange, "player");
	LOGMSG("player-actor created");
	player->sentience = new PlayerSentience();
	LOGMSG("sentience OK");
	player->vitality = new Vitality();
	LOGMSG("vitality OK");
	player->mortality = new Mortality(69, 47, "your corpse");
	LOGMSG("mortality OK");
	player->tempo = new ActorClock(100);
	LOGMSG("temporality OK");
	player->container = new Container(26);
	LOGMSG("inventory OK");
	//player is not portable by other NPCs
	//player-attack
//	LOGMSG("violence OK");
	engine.map->setOccupant(player->xpos, player->ypos, player);
	allActors.push(player); // register the player with the engine
	LOGMSG("player registered with allActors");
	parser.init(); // tell the parser to hook into the player actor
	LOGMSG("parser initialized");
	engine.gui->refreshViewport(); // force a viewport update
	LOGMSG("viewport refreshed");
	// print the MOTD
	gui->message(TCODColor::orange, "Tiger Tiger, burning bright,\nIn the forests of the night;\nWhat immortal hand or eye,\nCould frame thy fearful symmetry?");
	LOGMSG("engine init FINISHED");
}
void GameEngine::term() {
	allActors.clearAndDelete(); // delete all actors
	actionQueue.clearAndDelete(); // wipe the action queue
	if (map) delete map; // delete the map if it still exists
	gui->clear(); // wipe the GUI
	delete rng; // kill the RNG
}
void GameEngine::update() {
	switch(currMode) {
		case STARTUP:
			map->computeFOV(); // force the viewport to update
			engine.gui->menu.addItem(Menu::SAVE, "Save Game"); // enable saving
			switchMode(NEWTURN);
			break;
		case IDLE:
			// the engine is trying to obtain an update from the player
			// the engine doesn't care about metagame operations
			if (parser.stateChange == false) break;
			engine.player->update();
			engine.chrono->advanceTime();
			engine.player->sentience->context->clear();
			switchMode(ONGOING);
			break;
		case ONGOING:
			// process one actor in the action queue:
			// if no one's waiting (queue empty or not enough AP) -> NEWTURN
			if (actionQueue.size() == 0) {
				switchMode(NEWTURN);
				break;
			}
			// look at the next actor in line to update
			currActor = actionQueue.pop();
			// if the actor's the player -> IDLE
			if (currActor == player) {
				switchMode(IDLE);
			} else { // if not, ask the actor to update
				currActor->update();
				// should time advance here? or just on player's turn?
			}
			break;
		case NEWTURN:
			// re-populate the action queue
			updateActionQueue();
			// distribute AP
			refreshAP();
			// sort the entire queue
			switchMode(ONGOING);
			break;
		case VICTORY:
			LOGMSG("WARNING: no state defn for VICTORY");
			break;
		case DEFEAT:
			LOGMSG("WARNING: no state defn for DEFEAT");
			break;
		default:
			ERRMSG("WARNING: Engine state is UNDEFINED!");
			break;
	}
}
void GameEngine::render() {
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
//	LOGMSG(" called");
	gui->render(); // draw the GUI
	map->computeFOV(); // refresh the FOV map
	map->render(); // draw the map
	// draw all visible actors
	for (Actor **iter = allActors.begin(); iter != allActors.end(); iter++) {
		Actor *target = *iter;
		if (target != player && map->isVisible(target->xpos, target->ypos)) {
//			LOGMSG("Rendering actor " << target->name);
			target->render();
		}
		player->render();
	}
	gui->blitToScreen();
}
// *** ACTOR INSTANTIATION
void GameEngine::spawnCreature(int x, int y) {
	// generates a creature of the matching type at x, y
	// eventually these explicit calls will be replaced with a dictionary
	createLangurMonkey(x, y);
	LOGMSG("New monkey created at " << x << ", " << y);
}
void GameEngine::createLangurMonkey(int x, int y) {
	// creates a langur monkey, the debug mascot, at the given coords
	Actor *monkey = new Actor(x, y, 'm', TCODColor::lightGrey, "langur monkey");
	monkey->sentience = new AnimalSentience();
	monkey->mortality = new Mortality(5, 0, "monkey corpse");
	monkey->container = new Container(1);
	monkey->tempo = new ActorClock(100);
	map->setOccupant(x, y, monkey);
	allActors.push(monkey);
}
void GameEngine::createBushFruit(int x, int y) {
	// creates a bush fruit at the specified coordinates
	Actor *fruit = new Actor(0, 0, '%', TCODColor::lime, "fruit");
	// must be able to pick up the fruit
	fruit->portable = new Portable();
	// must be able to eat the fruit (and regain health/nutrition)
//	fruit->edible = new Edible();
}
// *** ITEM INSTANTIATION
// need another item generator that puts directly to a container
void spawnItem(std::string type, int x, int y) {
	// generates an item of the matching type at x, y
}
// *** MINOR FUNCTIONS
void GameEngine::mainMenu() {
	engine.gui->menu.clear(); // wipe the menu prior to rebuilding it
	// add "Return to Game" if there's a game running
	if (currMode != STARTUP) {
		engine.gui->menu.addItem(Menu::NONE, "Return to Game");
	}
	// always let the player start a new game
	engine.gui->menu.addItem(Menu::NEW_GAME, "New Game");
	// add "Continue" if the game just started up AND there's a game to load
	if (currMode == STARTUP && TCODSystem::fileExists("game.sav") ) {
		engine.gui->menu.addItem(Menu::CONTINUE, "Continue");
	}
	// allow saving the game if the game's running and the player's alive
	if (currMode != STARTUP && !(engine.player->mortality->isDead()) ) {
		engine.gui->menu.addItem(Menu::SAVE, "Save Game");
	}
	engine.gui->menu.addItem(Menu::QUIT, "Quit to Desktop");
	// handle menu choice input
	Menu::MenuItemCode menuItem = engine.gui->menu.pick();
	switch (menuItem) {
		case Menu::NEW_GAME:
//			LOGMSG(" NEW GAME ");
			engine.deleteSavedGame();
			engine.term();
			engine.init();
			break;
		case Menu::CONTINUE:
			// load the game from an external file
//			LOGMSG(" LOAD ");
			engine.loadFromFile();
			break;
		case Menu::SAVE:
			// save the game to an external file
//			LOGMSG(" SAVE ");
			engine.saveToFile();
			break;
		case Menu::QUIT:
//			LOGMSG(" QUIT ");
			exit(0);
			break;
		case Menu::NONE:
//			LOGMSG(" NONE ");
			// fall through
		default:
//			LOGMSG(" DEFAULT (return to game) ");
			break;
	}
}
void GameEngine::saveToFile() {
	LOGMSG("*** called");
	// if the player's dead, don't even try to save the game
	if (player->mortality->isDead()) return;
	TCODZip fileBuffer; // create the compression buffer
	fileBuffer.putInt(seed); // RNG seed
	fileBuffer.putInt(fovRadius); // player's FOV radius
	chrono->save(fileBuffer); // world clock state
	map->save(fileBuffer);   // state of world tiles
	player->save(fileBuffer);// player's state
	fileBuffer.putInt(allActors.size() - 1); // quantity of non-player actors
	for (Actor **iter = allActors.begin(); iter != allActors.end(); iter++) {
		// if the actor is NOT the player, ask them to save their data
		if (*iter != player) (*iter)->save(fileBuffer);
	}
	gui->save(fileBuffer);   // message log
	fileBuffer.saveToFile("game.sav");
}
void GameEngine::loadFromFile() {
	LOGMSG("*** called");
	TCODZip fileBuffer; // open an empty file buffer
	engine.term(); // flush the engine
	fileBuffer.loadFromFile("game.sav"); // open the save game file
	// begin retrieving various game state data
	seed = fileBuffer.getInt();
	rng = new TCODRandom(seed, TCOD_RNG_MT); // start up the RNG
	fovRadius = fileBuffer.getInt();
	chrono = new WorldClock();
	chrono->load(fileBuffer);
	int newWidth = fileBuffer.getInt();
	int newHeight = fileBuffer.getInt();
	map = new GameMap(newWidth, newHeight);
	LOGMSG("created map of size " << newWidth << "x" << newHeight);
	map->init(false);
	map->load(fileBuffer);
	player = new Actor(0, 0, 0, TCODColor::white, nullptr);
	player->load(fileBuffer, true);
	allActors.push(player);
	parser.init();
	int npcQuantity = fileBuffer.getInt();
	LOGMSG("npcQuantity: " << npcQuantity);
	while (npcQuantity > 0) {
		Actor *actor = new Actor(0, 0, 0, TCODColor::white, nullptr);
		actor->load(fileBuffer, false);
		allActors.push(actor);
		npcQuantity--;
	}
	gui->load(fileBuffer);
	switchMode(IDLE); // cannot originally trigger a save from any other state
	engine.gui->refreshViewport();
}
void GameEngine::exitGame() {
}
void GameEngine::deleteSavedGame() {
	bool successFlag = false;
	successFlag = TCODSystem::deleteFile("game.sav");
	if (successFlag) {
		LOGMSG("SUCCESS");
	} else {
		LOGMSG("FAILED");
	}
}
void GameEngine::sendToBack(Actor *actor) {
	allActors.remove(actor);
	allActors.insertBefore(actor, 0);
}
Actor *GameEngine::getClosestMonster(int x, int y, float range) const {
	Actor *closestMonster = nullptr;
	float bestDistance = 1E6f; //1E6 == 1000000; how far away are we allowed to select?
	for (Actor **iter = allActors.begin(); iter != allActors.end(); iter++) {
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
bool GameEngine::pickATile(int *x, int *y, float maxRange) {
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
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
		if (map->isVisible(mouse.cx, mouse.cy) && 
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
Actor *GameEngine::getActor(int x, int y) const {
	for (Actor **iter = allActors.begin(); iter != allActors.end(); iter++) {
		Actor *actor = *iter;
//		if (actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) {
		if (actor->xpos == x && actor->ypos == y) {
			return actor;
		}
	}
	return nullptr;
}
void GameEngine::updateActionQueue() {
	// this could be much better if it sorted AND inserted at the same time...
	actionQueue.clear();
	for (Actor **iter = allActors.begin(); iter != allActors.end(); iter++) {
		Actor *subject = *iter;
		if (subject->sentience) actionQueue.push(subject);
	}
}
void GameEngine::refreshAP() {
	for (Actor **iter = actionQueue.begin(); iter != actionQueue.end(); iter++) {
		Actor *subject = *iter;
		subject->tempo->refreshAP();
	}
}
void GameEngine::switchMode(EngineState newMode) {
	prevMode = currMode;
	currMode = newMode;
/*	switch (currMode) {
		case STARTUP:
		LOGMSG("mode switch: " << currMode << ": STARTUP");
		break;
		case IDLE:
		LOGMSG("mode switch: " << currMode << ": IDLE");
		break;
		case ONGOING:
		LOGMSG("mode switch: " << currMode << ": ONGOING");
		break;
		case NEWTURN:
		LOGMSG("mode switch: " << currMode << ": NEWTURN");
		break;
		case VICTORY:
		LOGMSG("mode switch: " << currMode << ": VICTORY");
		break;
		case DEFEAT:
		LOGMSG("mode switch: " << currMode << ": DEFEAT");
		break;
		default:
		break;
	}
	*/
}
