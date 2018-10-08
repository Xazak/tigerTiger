#include "libtcod.hpp"
#include "actor.hpp"
#include "map.hpp"
#include "engine.hpp"

Engine::Engine() {
    // initialize the console
    TCODConsole::initRoot(80, 50, "libtcod C++ tutorial", false);
    // create a player object
    player = new Actor(40, 25, '@', TCODColor::white);
    // push the player and an NPC actor to the map
    actors.push(player);
    actors.push(new Actor(60, 13, '@', TCODColor::yellow));
    // init a new map
    map = new Map(80, 45);
}

Engine::~Engine() {
    actors.clearAndDelete(); //TCOD helper fxn
    delete map;
}

void Engine::update() {
    TCOD_key_t key;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
    switch(key.vk) {
        case TCODK_UP:
            if (!map->isWall(player->x, player->y-1)) player->y--;
            break;
        case TCODK_DOWN:
            if (!map->isWall(player->x, player->y+1)) player->y++;
            break;
        case TCODK_LEFT:
            if (!map->isWall(player->x-1, player->y)) player->x--;
            break;
        case TCODK_RIGHT:
            if (!map->isWall(player->x+1, player->y)) player->x++;
            break;
        default: break;
    }
}

void Engine::render() {
    TCODConsole::root->clear(); // clear any console leftovers
    map->render();              // draw the map
    // draw the actors
    for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++) {
        (*iterator)->render();
    }
}

