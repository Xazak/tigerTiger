#include <math.h>
#include "main.hpp"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_ITEMS = 2;

class BspListener : public ITCODBspCallback { 
    private:
        Map &map;   // a room to dig
        int roomNum;// room number
        int lastx, lasty;   // center of the last room
    public:
        BspListener(Map &map) : map(map), roomNum(0) {}
        bool visitNode(TCODBsp *node, void *userData) {
            if (node->isLeaf()) {
                int x, y, w, h;
                // dig a room
                TCODRandom *rng=TCODRandom::getInstance();
                w = rng->getInt(ROOM_MIN_SIZE, node->w-2);
                h = rng->getInt(ROOM_MIN_SIZE, node->h-2);
                x = rng->getInt(node->x+1, node->x + node->w - w - 1);
                y = rng->getInt(node->y+1, node->y + node->h - h - 1);
                map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1);
                if (roomNum != 0) {
                    // dig a corridor from last room
                    map.dig(lastx, lasty, x+w/2, lasty);
                    map.dig(x+w/2, lasty, x+w/2, y+h/2);
                }
                lastx = x+w/2;
                lasty = y+h/2;
                roomNum++;
            }
            return true;
        }
};
Map::Map(int width, int height) : width(width), height(height), currentScentValue(SCENT_THRESHOLD) {
    tiles = new Tile[width*height];
    map = new TCODMap(width, height);
    TCODBsp bsp(0, 0, width, height);
    bsp.splitRecursive(NULL, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
    BspListener listener(*this);
    bsp.traverseInvertedLevelOrder(&listener, NULL);
}
Map::~Map() {
    delete [] tiles;
    delete map;
}
bool Map::isWall(int x, int y) const {
    return !map->isWalkable(x, y);
}
bool Map::isExplored(int x, int y) const {
    return tiles[x + y * width].explored;
}
bool Map::isInFOV(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    if (map->isInFov(x, y)) {
        tiles[x + y * width].explored = true;
        return true;
    }
    return false;
}
bool Map::canWalk(int x, int y) const {
    if (isWall(x, y)) {
        return false;       // there is a wall in the way
    }
    for (Actor **iterator=engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
        Actor *actor = *iterator;
        if (actor->blocks && actor->x == x && actor->y == y) {
            return false;   // there is an actor in the way
        }
    }
    return true; // nothing is in the way
}
void Map::computeFOV() {
    map->computeFov(engine.player->x, engine.player->y, engine.fovRadius);
    // update scent field
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (isInFOV(x, y)) {
                unsigned int oldScent = getScent(x, y);
                int dx = x - engine.player->x;
                int dy = y - engine.player->y;
                long distance = (int)sqrt(dx * dx + dy * dy);
                unsigned int newScent = currentScentValue - distance;
                if (newScent > oldScent) {
                    tiles[x + y * width].scent = newScent;
                }
            }
        }
    }
}
void Map::render() const {
    static const TCODColor darkWall(0, 0, 100);
    static const TCODColor darkGround(50, 50, 150);
    static const TCODColor lightWall(130, 110, 50);
    static const TCODColor lightGround(200, 180, 50);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int scent = SCENT_THRESHOLD - (currentScentValue - getScent(x, y));
            scent = CLAMP(0, 10, scent);
            float sc = scent * 0.1f;
            if (isInFOV(x, y)) {
                TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? lightWall : TCODColor::lightGrey * sc);
            } else if (isExplored(x, y)) {
                TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? darkWall : TCODColor::lightGrey * sc);
            } else if (!isWall(x, y)) {
                TCODConsole::root->setCharBackground(x, y, TCODColor::white * sc);
            }
        }
    }
}
void Map::dig (int x1, int y1, int x2, int y2) {
    if (x2 < x1) {
        int tmp = x2;
        x2 = x1;
        x1 = tmp;
    }
    if (y2 < y1) {
        int tmp = y2;
        y2 = y1;
        y1 = tmp;
    }
    for (int tilex = x1; tilex <= x2; tilex++) {
        for (int tiley = y1; tiley <= y2; tiley++) {
            map->setProperties(tilex, tiley, true, true);
        }
    }
}
void Map::createRoom (bool first, int x1, int y1, int x2, int y2) {
    dig (x1, y1, x2, y2);
    if (first) {
        // spawn the player in the first room
        engine.player->x = (x1+x2)/2;
        engine.player->y = (y1+y2)/2;
    } else {
        TCODRandom *rng = TCODRandom::getInstance();
        int nbMonsters = rng->getInt(0, MAX_ROOM_MONSTERS);
        while (nbMonsters > 0) {
            int x = rng->getInt(x1, x2);
            int y = rng->getInt(y1, y2);
            if (canWalk(x, y)) {
                addMonster(x, y);
            }
            nbMonsters--;
        }
        // add items
        int nbItems = rng->getInt(0, MAX_ROOM_ITEMS);
        while (nbItems > 0) {
            int x = rng->getInt(x1, x2);
            int y = rng->getInt(y1, y2);
            if (canWalk(x, y)) {
                addItem(x, y);
            }
            nbItems--;
        }
    }
}
void Map::addMonster(int x, int y) {
    TCODRandom *rng = TCODRandom::getInstance();
    if (rng->getInt(0, 100) < 80) {
        // create an orc
        Actor *orc = new Actor (x, y, 'o', "orc", TCODColor::desaturatedGreen);
        orc->destructible = new MonsterDestructible(10, 0, "dead orc");
        orc->attacker = new Attacker(3);
        orc->ai = new MonsterAi();
        engine.actors.push(orc);
    } else {
        // create a troll
        Actor *troll = new Actor(x, y, 'T', "troll", TCODColor::darkerGreen);
        troll->destructible = new MonsterDestructible(16, 1, "troll carcass");
        troll->attacker = new Attacker(4);
        troll->ai = new MonsterAi();
        engine.actors.push(troll);
    }
}
void Map::addItem(int x, int y) {
    TCODRandom *rng = TCODRandom::getInstance();
    int dice = rng->getInt(0, 100);
    if (dice < 70) {
        // create a health potion
        Actor *healthPotion = new Actor(x, y, '!', "health potion", TCODColor::violet);
        healthPotion->blocks = false;
        healthPotion->pickable = new Healer(4);
        engine.actors.push(healthPotion);
    } else if (dice < 80) {
        // create a scroll of lightning bolt
        Actor *scrollOfLightningBolt = new Actor(x, y, '?', "scroll of lightning bolt", TCODColor::lightYellow);
        scrollOfLightningBolt->blocks = false;
        scrollOfLightningBolt->pickable = new LightningBolt(5, 20);
        engine.actors.push(scrollOfLightningBolt);
    } else if (dice < 90) {
        // create a scroll of fireball
        Actor *scrollOfFireball = new Actor(x, y, '?', "scroll of fireball", TCODColor::orange);
        scrollOfFireball->blocks = false;
        scrollOfFireball->pickable = new Fireball(3, 12);
        engine.actors.push(scrollOfFireball);
    } else {
        // create a scroll of confusion
        Actor *scrollOfConfusion = new Actor(x, y, '?', "scroll of confusion", TCODColor::lightGreen);
        scrollOfConfusion->blocks = false;
        scrollOfConfusion->pickable = new Confuser(10, 8);
        engine.actors.push(scrollOfConfusion);
    }
}
unsigned int Map::getScent(int x, int y) const {
    return tiles[x + y * width].scent;
}
