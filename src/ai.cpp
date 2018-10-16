#include <stdio.h>
#include <math.h>
#include "main.hpp"

// number of monster pursuit turns
static const int TRACKING_TURNS = 3;

Ai *Ai::create(TCODZip &zip) {
	AiType type = (AiType)zip.getInt();
	Ai *ai = NULL;
	switch(type) {
		case PLAYER: ai = new PlayerAi(); break;
		case MONSTER: ai = new MonsterAi(); break;
		case CONFUSED_MONSTER: ai = new ConfusedMonsterAi(0, NULL); break;
	}
	ai->load(zip);
	return ai;
}
void PlayerAi::update(Actor *owner) {
    if (owner->destructible && owner->destructible->isDead()) { return; }
    int dx = 0;
    int dy = 0;
    switch (engine.lastKey.vk) {
        case TCODK_UP: dy =- 1; break;
        case TCODK_DOWN: dy = 1; break;
        case TCODK_LEFT: dx =- 1; break;
        case TCODK_RIGHT: dx = 1; break;
        case TCODK_CHAR: handleActionKey(owner, engine.lastKey.c); break;
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
            bool corpseOrItem = (actor->destructible && actor->destructible->isDead()) || actor->pickable;
            if (corpseOrItem && actor->x == targetx && actor->y == targety ) {
                engine.gui->message(TCODColor::lightGrey, "There's a %s here.\n", actor->name);
            }
    }
    owner->x = targetx;
    owner->y = targety;
    return true;
}
void PlayerAi::handleActionKey(Actor *owner, int ascii) {
    switch(ascii) {
        case 'g': { //pickup item
            bool found = false;
            for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
                Actor *actor = *iterator;
                if (actor->pickable && actor->x == owner->x && actor->y == owner->y) {
                    if (actor->pickable->pick(actor, owner)) {
                        found = true;
                        engine.gui->message(TCODColor::lightGrey, "You pick up the %s.", actor->name);
                        break;
                    } else if (!found) {
                        found = true;
                        engine.gui->message(TCODColor::red, "Your inventory is full.");
                    }
                }
            }
            if (!found) {
                engine.gui->message(TCODColor::lightGrey, "There is nothing here to pick up.");
            }
            engine.gameStatus=Engine::NEW_TURN;
            break;
        }
        case 'i': { //display inventory
            Actor *actor = choseFromInventory(owner);
            if (actor) {
                actor->pickable->use(actor, owner);
                engine.gameStatus = Engine::NEW_TURN;
            }
        break;
        }
        case 'd': { //drop item
            Actor *actor = choseFromInventory(owner);
            if (actor) {
                actor->pickable->drop(actor, owner);
                engine.gameStatus = Engine::NEW_TURN;
            }
        break;
        }

    }
}
Actor *PlayerAi::choseFromInventory(Actor *owner) {
    static const int INVENTORY_WIDTH = 50;
    static const int INVENTORY_HEIGHT = 28;
    static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);
    // display the inventory frame
    con.setDefaultForeground(TCODColor(200, 180, 50));
    con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");
    // display the items with their shortcuts
    con.setDefaultForeground(TCODColor::white);
    int shortcut = 'a';
    int y = 1;
    for (Actor **it = owner->container->inventory.begin(); it != owner->container->inventory.end(); it++) {
        Actor *actor = *it;
        con.print(2, y, "(%c) %s", shortcut, actor->name);
        y++;
        shortcut++;
    }
    // blit the inventory console to the root
    TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root, engine.screenWidth / 2 - INVENTORY_WIDTH / 2, engine.screenHeight / 2 - INVENTORY_HEIGHT / 2);
    TCODConsole::flush();
    // wait for a keypress
    TCOD_key_t key;
    TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
    if (key.vk == TCODK_CHAR) {
        int actorIndex = key.c - 'a';
        if (actorIndex >= 0 && actorIndex < owner->container->inventory.size()) {
            return owner->container->inventory.get(actorIndex);
        }
    }
    return NULL;
}
void PlayerAi::load(TCODZip &zip) { }
void PlayerAi::save(TCODZip &zip) {
	zip.putInt(PLAYER);
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
void MonsterAi::load(TCODZip &zip) {
	moveCount = zip.getInt();
}
void MonsterAi::save(TCODZip &zip) {
	zip.putInt(MONSTER);
	zip.putInt(moveCount);
}
ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns, Ai *oldAi) : nbTurns(nbTurns), oldAi(oldAi) { }
void ConfusedMonsterAi::update(Actor *owner) {
    TCODRandom *rng = TCODRandom::getInstance();
    int dx = rng->getInt(-1, 1);
    int dy = rng->getInt(-1, 1);
    int destx = owner->x + dx;
    int desty = owner->y + dy;
    if (engine.map->canWalk(destx, desty)) {
        owner->x = destx;
        owner->y = desty;
    } else {
        Actor *actor=engine.getActor(destx, desty);
        if (actor) {
            owner->attacker->attack(owner, actor);
        }
    }
    nbTurns--;
    if (nbTurns == 0) {
        owner->ai = oldAi;
        delete this;
    }
}
void ConfusedMonsterAi::load(TCODZip &zip) {
	nbTurns = zip.getInt();
	oldAi = Ai::create(zip);
}
void ConfusedMonsterAi::save(TCODZip &zip) {
	zip.putInt(CONFUSED_MONSTER);
	zip.putInt(nbTurns);
	oldAi->save(zip);
}
