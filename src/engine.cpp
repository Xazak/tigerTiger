#include "libtcod.hpp"
#include "actor.hpp"
#include "map.hpp"
#include "engine.hpp"

Engine::Engine() : gameStatus(STARTUP), fovRadius(10) {
    // initialize the console
    TCODConsole::initRoot(80, 50, "libtcod C++ tutorial", false);
    // create a player object
    player = new Actor(40, 25, '@', "player", TCODColor::white);
    actors.push(player); // push the player to the map
    map = new Map(80, 45); // init a new map
}
Engine::~Engine() {
    actors.clearAndDelete(); //TCOD helper fxn
    delete map;
}
void Engine::update() {
    TCOD_key_t key;
    if (gameStatus == STARTUP) map->computeFOV();
    gameStatus = IDLE;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
    int dx = 0;
    int dy = 0;
    switch(key.vk) {
        case TCODK_UP: dy =- 1; break;
        case TCODK_DOWN: dy = 1; break;
        case TCODK_LEFT: dx =- 1; break;
        case TCODK_RIGHT: dx = 1; break;
        default: break;
    }
    if (dx != 0 || dy != 0) {
        gameStatus = NEW_TURN;
        if (player->moveOrAttack(player->x + dx, player->y + dy)) {
            map->computeFOV();
        }
    }
    if (gameStatus == NEW_TURN) {
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
    // draw the actors
    for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++) {
        Actor *actor = *iterator;
        if (map->isInFOV(actor->x, actor->y)) {
            actor->render();
        }
    }
}

