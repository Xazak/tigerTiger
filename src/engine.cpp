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
static const int PLAYER_START_X = 965;
static const int PLAYER_START_Y = 965;

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
	fovRadius(10)
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
	map = new GameMap(MAP_WIDTH, MAP_HEIGHT); // init a new map
	map->init(true);
	// create a player object
	player = new Actor(PLAYER_START_X, PLAYER_START_Y, '@', TCODColor::orange, "player");
	player->sentience = new PlayerSentience();
//	LOGMSG("sentience OK");
//	player->vitality = new Vitality();
//	LOGMSG("vitality OK");
	player->mortality = new PlayerMortality(69, 47, "your corpse");
//	LOGMSG("mortality OK");
	player->tempo = new ActorClock(100);
//	LOGMSG("temporality OK");
	player->container = new Container(26);
//	LOGMSG("inventory OK");
	//player-pickable ?
	//player-attack
	allActors.push(player); // register the player with the engine
	parser.init(); // tell the parser to hook into the player actor
	engine.gui->refreshViewport(); // force a viewport update
	// print the MOTD
	gui->message(TCODColor::orange, "Tiger Tiger, burning bright,\nIn the forests of the night;\nWhat immortal hand or eye,\nCould frame thy fearful symmetry?");
//	chrono = new GameClock(); // create a world clock and set up AP tracking
}
void GameEngine::term() {
	allActors.clearAndDelete(); // delete all actors
	actionQueue.clearAndDelete(); // wipe the action queue
	if (map) delete map; // delete the map if it still exists
	gui->clear(); // wipe the GUI
}
void GameEngine::save() {
/*	OLD METHOD
//	if (player->destructible->isDead()) {
//		TCODSystem::deleteFile("game.sav");
//	} else {
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
//	}
		*/
	// NEW METHOD
	// invoke the scribe
	ERRMSG("BAD SAVE() FXN CALLED!");
}
void GameEngine::load() {
	ERRMSG("DEPRECATED LOAD() CALLED");
/*	engine.gui->menu.clear(); // wipe the menu to ensure no artifacts
	engine.gui->menu.addItem(Menu::NEW_GAME, "New Game");
	// add "Continue" if there's a save game to continue with
	if (TCODSystem::fileExists("game.sav")) {
		engine.gui->menu.addItem(Menu::CONTINUE, "Continue");
	}
	engine.gui->menu.addItem(Menu::EXIT, "Exit");
	// handle menu choice input
	Menu::MenuItemCode menuItem = engine.gui->menu.pick();
	switch (menuItem) {
		case Menu::EXIT:
			LOGMSG(" EXIT ");
			// fall through
		case Menu::NONE:
			LOGMSG(" NONE ");
			exit(0);
			break;
		case Menu::NEW_GAME:
			LOGMSG(" NEW GAME ");
			engine.term();
			engine.init();
			break;
		case Menu::SAVE:
			// save the game to an external file
			LOGMSG(" SAVE ");
			break;
		case Menu::CONTINUE:
			// load the game from an external file
			LOGMSG(" LOAD ");
			break;
		default:
			ERRMSG("Invalid menu option encountered!");
			break;
	}
	*/
		// PREVIOUS ::CONTINUE CODE
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
			ERRMSG("WARNING: Engine is in undefined state!");
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
		Actor *actor = *iter;
		if (actor != player && map->isVisible(actor->xpos, actor->ypos)) {
//			LOGMSG("Rendering actor " << actor->name);
			actor->render();
		}
		player->render();
	}
	gui->blitToScreen();
}
// *** MINOR FUNCTIONS
void GameEngine::mainMenu() {
	engine.gui->menu.clear(); // wipe the menu prior to rebuilding it
	if (currMode != STARTUP) {
		engine.gui->menu.addItem(Menu::NONE, "Return to Game");
	}
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
//	chrono.save(fileBuffer); // world clock state
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
	fovRadius = fileBuffer.getInt();
//	chrono->load(fileBuffer);
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
	LOGMSG("called");
	bool successFlag = false;
	successFlag = TCODSystem::deleteFile("game.sav");
	if (successFlag) {
		LOGMSG("File deleted");
	} else {
		LOGMSG("Could not delete file!");
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
		actionQueue.push(subject);
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
	switch (currMode) {
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
}
