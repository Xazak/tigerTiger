#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10),
    screenWidth(screenWidth), screenHeight(screenHeight) {
    // initialize the console
    TCODConsole::initRoot(80, 50, "libtcod C++ tutorial", false);
    // create a player object
    player = new Actor(40, 25, '@', "player", TCODColor::white);
    player->destructible = new PlayerDestructible(30, 2, "your cadaver");
    player->attacker = new Attacker(5);
    player->ai = new PlayerAi();
    actors.push(player);
    map = new Map(80, 43); // init a new map
    gui = new Gui(); // set up the GUI
    gui->message(TCODColor::red, "Welcome stranger!\nPrepare to perish in the Tombs!");
}
Engine::~Engine() {
    actors.clearAndDelete(); //TCOD helper fxn
    delete map;
    delete gui;
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
    map->render();              // draw the map
    gui->render();              // update the gui
    // draw the actors
    for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++) {
        Actor *actor = *iterator;
        if (map->isInFOV(actor->x, actor->y)) {
            actor->render();
        }
        player->render();
        // show the player's stats
        //NOTE: this function has been deprecated, use the new version!
        TCODConsole::root->print(1, screenHeight-2, "HP: %d/%d", (int)player->destructible->curHP,
                (int)player->destructible->maxHP);
    }
}
void Engine::sendToBack(Actor *actor) {
    actors.remove(actor);
    actors.insertBefore(actor, 0);
}
