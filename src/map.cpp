#include <math.h>
#include <iostream>
#include "main.hpp"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_ITEMS = 2;
static const int MAX_ROOM_MONSTERS = 3;

/* Most of this is code that specifically generates Nethack-style dungeons
 * using the BSP toolkit from libtcod; it will need to be rewritten later...
 */
class BspListener : public ITCODBspCallback {
	private:
		Map &map; // a room to dig
		int roomNum; // room number
		int lastx, lasty; // center of the last room
	public:
		BspListener(Map &map) : map(map), roomNum(0) {}
		bool visitNode(TCODBsp *node, void *userData) {
			if (node->isLeaf()) {
				int x, y, w, h;
				bool withActors = (bool)userData;
				// dig a room
				w = map.rng->getInt(ROOM_MIN_SIZE, node->w-2);
				h = map.rng->getInt(ROOM_MIN_SIZE, node->h-2);
				x = map.rng->getInt(node->x+1, node->x + node->w - w - 1);
				y = map.rng->getInt(node->y+1, node->y + node->h - h - 1);
				map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1, withActors);
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
// The following code is definitely necessary for the map itself
/*bool Tile::occupied(int x, int y) {
	for (Actor **actor = engine.actors.begin(); actor != engine.actors.end(); actor++) {
		if (actor->x == x && actor->y == y) return true;
	}
	return false;
}*/

Map::Map(int width, int height) : width(width), height(height) {
	// pick a random seed between 0 and (a big number)
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}
Map::~Map() {
	delete [] tiles;
	delete map;
}
void Map::computeFOV() {
	map->computeFov(engine.player->xpos, engine.player->ypos, engine.fovRadius);
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (isVisible(x, y)) {
//				int dx = x - engine.player->xpos;
//				int dy = y - engine.player->ypos;
//				long distance = (int)sqrt(dx * dx + dy * dy); // what for?
			}
		}
	}
}
void Map::render() const {
	// actually draw the map
	static const TCODColor darkWall(0, 0, 100);
	static const TCODColor darkGround(50, 50, 150);
	static const TCODColor lightWall(130, 110, 50);
	static const TCODColor lightGround(200, 180, 50);
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (isVisible(x, y)) {
				TCODConsole::root->setCharBackground(x, y, isObstructed(x, y) ? lightWall : TCODColor::lightGrey);
			} else if (isExplored(x, y)) {
				TCODConsole::root->setCharBackground(x, y, isObstructed(x, y) ? darkWall : TCODColor::lightGrey);
			} else if (!isObstructed(x, y)) {
				TCODConsole::root->setCharBackground(x, y, TCODColor::white);
			}
		}
	}
}
void Map::init(bool withActors) {
	// create the map objects
	rng = new TCODRandom(seed, TCOD_RNG_CMWC);
	tiles = new Tile[width*height];		// use x + y * width to locate a tile
	map = new TCODMap(width, height);
	// invoke the BSP tree tools to generate a dungeon graph
//	TCODBsp bsp(0, 0, width, height);
//	bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
//	BspListener listener(*this);
//	bsp.traverseInvertedLevelOrder(&listener, (void *)withActors);
	// invoke our custom mapgen code instead
	generateMap(true, width, height);
}
/*void Map::save(TCODZip &zip) {
	zip.putInt(seed);
	for (int i = 0; i < width*height; i++) {
		zip.putInt(tiles[i].explored);
	}
}
void Map::load(TCODZip &zip) {
	seed = zip.getInt();
	init(false);
	for (int i = 0; i < width*height; i++) {
		tiles[i].explored = zip.getInt();
	}
}*/
// should these be overloaded to use Tile pointers as well?
bool Map::isObstructed(int x, int y) const {
	Tile *target = &tiles[x + y * width];
	if (!map->isWalkable(x, y)) {
		return false; // there is a static object in the way
	} else if (target->occupant && target->occupant->obstructs) {
		// would just the right side be sufficient above?
		return false; // there is an actor in the way
	}
	return true; // didn't find anything obstructive
}
bool Map::isVisible(int x, int y) const {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
	if (map->isInFov(x, y)) {
		tiles[x + y * width].explored = true;
		return true;
	}
	return false;
}
bool Map::isExplored(int x, int y) const {
	return tiles[x + y * width].explored;
}
bool Map::isOccupied(int x, int y) const {
	return tiles[x + y * width].occupant;
}
bool Map::isHolding(int x, int y) const {
//	return tiles[x + y * width].itemList;
	return false;
}
// Map Generation Tools
void Map::generateMap(bool isNew, int width, int height) {
	// isNew controls whether the map is being made from scratch or not
	// currently all maps are new, and we ARE using the tutorial fxns
	this->createRoom(true, 1, 1, width - 1, height - 1, true);
//	this->dig(engine.player->xpos - 3, engine.player->ypos, engine.player->xpos + 3, engine.player->ypos);
}

// from the tutorial
// these are used for BSP dungeon generation
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
void Map::createRoom (bool first, int x1, int y1, int x2, int y2, bool withActors) {
	dig(x1, y1, x2, y2);
	
	if (!withActors) {
		return;
	}
	if (first) {
		// spawn the player in the first room
		engine.player->xpos = (x1+x2)/2;
		engine.player->ypos = (y1+y2)/2;
	} else {
		TCODRandom *rng = TCODRandom::getInstance();
		int nbMonsters = rng->getInt(0, MAX_ROOM_MONSTERS);
		while (nbMonsters > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (!isObstructed(x, y)) {
				addMonster(x, y);
			}
			nbMonsters--;
		}
		// add items
		int nbItems = rng->getInt(0, MAX_ROOM_ITEMS);
		while (nbItems > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (!isObstructed(x, y)) {
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
		Actor *orc = new Actor (x, y, 'o', TCODColor::desaturatedGreen, "orc");
//		orc->destructible = new MonsterDestructible(10, 0, "dead orc");
//		orc->attacker = new Attacker(3);
//		orc->ai = new MonsterAi();
		engine.actors.push(orc);
	} else {
		// create a troll
		Actor *troll = new Actor(x, y, 'T', TCODColor::darkerGreen, "troll");
//		troll->destructible = new MonsterDestructible(16, 1, "troll carcass");
//		troll->attacker = new Attacker(4);
//		troll->ai = new MonsterAi();
		engine.actors.push(troll);
	}
}
void Map::addItem(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);
	if (dice < 70) {
		// create a health potion
		Actor *healthPotion = new Actor(x, y, '!', TCODColor::violet, "health potion");
//		healthPotion->blocks = false;
//		healthPotion->pickable = new Healer(4);
		engine.actors.push(healthPotion);
	} else if (dice < 80) {
		// create a scroll of lightning bolt
		Actor *scrollOfLightningBolt = new Actor(x, y, '?', TCODColor::lightYellow, "scroll of lightning bolt");
//		scrollOfLightningBolt->blocks = false;
//		scrollOfLightningBolt->pickable = new LightningBolt(5, 20);
		engine.actors.push(scrollOfLightningBolt);
	} else if (dice < 90) {
		// create a scroll of fireball
		Actor *scrollOfFireball = new Actor(x, y, '?', TCODColor::orange, "scroll of fireball");
//		scrollOfFireball->blocks = false;
//		scrollOfFireball->pickable = new Fireball(3, 12);
		engine.actors.push(scrollOfFireball);
	} else {
		// create a scroll of confusion
		Actor *scrollOfConfusion = new Actor(x, y, '?', TCODColor::lightGreen, "scroll of confusion");
//		scrollOfConfusion->blocks = false;
//		scrollOfConfusion->pickable = new Confuser(10, 8);
		engine.actors.push(scrollOfConfusion);
	}
}
