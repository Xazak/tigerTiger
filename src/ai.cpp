#include <stdio.h>
#include <math.h>
#include "main.hpp"

// number of monster pursuit turns
static const int TRACKING_TURNS = 3;

void PlayerAi::update(Actor *owner) {
    if (owner->destructible && owner->destructible->isDead()) { return; }
    int dx = 0;
    int dy = 0;
    switch (engine.lastKey.vk) {
        case TCODK_UP: dy =- 1; break;
        case TCODK_DOWN: dy = 1; break;
        case TCODK_LEFT: dx =- 1; break;
        case TCODK_RIGHT: dx = 1; break;
        default: break;
    }
    if (dx != 0 || dy != 0) {
        engine.gameStatus=Engine::NEW_TURN;
        if (moveOrAttack(owner, owner->x+dx, owner->y+dy)) {
            engine.map->computeFOV();
        }
    }
}
bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) {
    if (engine.map->isWall(targetx, targety)) return false;
    // look for living actors to attack
    for (Actor **iterator = engine.actors.begin();
        iterator != engine.actors.end(); iterator++) {
            Actor *actor = *iterator;
            if (actor->destructible && actor->destructible->isDead()
                && actor->x == targetx && actor->y == targety ) {
                    engine.gui->message(TCODColor::lightGrey, "There's a %s here.\n", actor->name);
            }
    }
    owner->x = targetx;
    owner->y = targety;
    return true;
}
void MonsterAi::update(Actor *owner) {
    if (owner->destructible && owner->destructible->isDead()) {
        return;
    }
    moveOrAttack(owner, engine.player->x, engine.player->y);
}
void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety) {
    int dx = targetx - owner->x;
    int dy = targety - owner->y;
    int stepdx = (dx > 0 ? 1 : -1);
    int stepdy = (dy > 0 ? 1 : -1);
    float distance = sqrtf(dx * dx + dy * dy);
    if (distance < 2 ) {
        // at melee range, attack!
        if (owner->attacker) {
            owner->attacker->attack(owner, engine.player);
        }
        return;
    } else if (engine.map->isInFOV(owner->x, owner->y)) {
        // player is visible, move closer
        dx = (int)(round(dx/distance));
        dy = (int)(round(dy/distance));
        if (engine.map->canWalk(owner->x+dx, owner->y+dy)) {
            owner->x += dx;
            owner->y += dy;
            return;
        } else if (engine.map->canWalk(owner->x + stepdx, owner->y)) {
            owner->x += stepdx;
        } else if (engine.map->canWalk(owner->x, owner->y + stepdy)) {
            owner->y += stepdy;
        }
    }
    // player not visible, follow yer nose
    // find adj cell with highest scentLevel
    unsigned int bestLevel = 0;
    int bestCellIndex = -1;
    static int tdx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    static int tdy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    for (int i = 0; i < 8; i++) {
        int cellx = owner->x + tdx[i];
        int celly = owner->y + tdy[i];
        if (engine.map->canWalk(cellx, celly)) {
            unsigned int cellScent = engine.map->getScent(cellx, celly);
            if (cellScent > engine.map->currentScentValue - SCENT_THRESHOLD && cellScent > bestLevel) {
                bestLevel = cellScent;
                bestCellIndex = i;
            }
        }
        if (bestCellIndex != -1) {
            // the monster smells the player: follow yer nose!
            owner->x += tdx[bestCellIndex];
            owner->y += tdy[bestCellIndex];
        }
    }
}
