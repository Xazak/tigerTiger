#include <math.h>
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
Tile::Tile(): glyph(43), foreColor(25, 25, 25), backColor(127, 127, 127),
	explored(false), occupant(NULL) { }
//Tile::~Tile() { }
Map::Map(int inputWidth, int inputHeight) : width(inputWidth), height(inputHeight) {
	// pick a random seed between 0 and (a big number)
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}
Map::~Map() {
	delete [] tiles;
	delete visionMap;
}
void Map::computeFOV() {
	visionMap->computeFov(engine.player->xpos, engine.player->ypos, engine.fovRadius);
}
/*	*** MAP::RENDER METHOD
	draw every tile according to its internal values, of those tiles that
	fall within the range of the viewport's vision:
	+---+-
	|   |^
	| P |H
	|   |v
	+---+-
	|<W>|
	where W and H are the viewportWidth and ..Height respectively.
	P should always be located at (W/2, H/2) w/in the viewport, and the tiles
	displayed are generated relative to that by their absolute position on 
	the map.
	METHOD
	Given the player's x,y position (indexing to tiles[x + y * arrayWidth])
	viewportXStart, viewportYStart = 0
	viewportXEnd = viewportWidth
	viewportYEnd = viewportHeight
	viewportTarget = (viewportXIndex, viewportYIndex)
	mapWindowXStart = playerX - viewportXOffset
	mapWindowYStart = playerY - viewportYOffset
	mapWindowXEnd = playerX + viewportXOffset
	mapWindowYEnd = playerY + viewportYOffset
	mapWindowTarget = (mapWindowXIndex, mapWindowYIndex)
	1) iterate across all viewportX
		iterate across all mapXIndex
		2) iterate across all viewportY
			3) draw (mapXIndex, mapYIndex) on (viewportX, viewportY)
*/
void Map::render() const {
	// draw the map onto the viewport console
	static const float beyondFOVMod = 0.5f; // color coefficient
	Tile *target = &tiles[0]; // index pointer, starts at beginning
	engine.gui->refreshViewport(); // update the viewport coords before we start
	// begin map processing
	int mapX = engine.gui->viewportXOrigin; // index to x-position of map tile
	int mapY = engine.gui->viewportYOrigin; // index to y-position of map tile
	for (int viewportX = 0; viewportX < engine.gui->viewport->getWidth(); viewportX++) {
		if (viewportX == 0) mapX = engine.gui->viewportXOrigin; // removing this causes segfaults! WHY???
		for (int viewportY = 0; viewportY < engine.gui->viewport->getHeight(); viewportY++) {
			if (viewportY == 0) mapY = engine.gui->viewportYOrigin; // same for mapX above???
			target = &tiles[mapX + mapY * width];
			if (isVisible(mapX, mapY)) {
				// draw visible tiles first
				engine.gui->viewport->putCharEx(viewportX, viewportY,
					target->glyph, target->foreColor,
					target->backColor);
			} else if (isExplored(mapX, mapY)) {
				// draw not-visible, but explored tiles
				engine.gui->viewport->putCharEx(viewportX, viewportY,
					target->glyph,
					target->foreColor - (target->foreColor * beyondFOVMod),
					target->backColor - (target->backColor * beyondFOVMod));
			}
			mapY++;
		}
		mapX++;
	}
}
void Map::init(bool withActors) {
	// create the map objects
	rng = new TCODRandom(seed, TCOD_RNG_CMWC);
	tiles = new Tile[width * height]; // use x + y * width to locate a tile
	visionMap = new TCODMap(width, height); // invokes the TCOD FOV map object
	// invoke the BSP tree tools to generate a dungeon graph
//	TCODBsp bsp(0, 0, width, height);
//	bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
//	BspListener listener(*this);
//	bsp.traverseInvertedLevelOrder(&listener, (void *)withActors);
	// invoke our custom mapgen code instead
	generateTerrain(true, width, height); // this is my function
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
bool Map::isWall(int x, int y) const {
	return !visionMap->isWalkable(x, y);
}
bool Map::isOccupied(int x, int y) const {
	// returns true if the target tile contains an actor
	return tiles[x + y * width].occupant;
}
bool Map::isObstructed(int x, int y) const {
	// returns true if the target cell contains an obstructing object
	Tile *target = &tiles[x + y * width];
	if (isOccupied(x, y)) {
		return target->occupant->obstructs;
	}
	return false;
}
bool Map::isVisible(int x, int y) const {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
	if (visionMap->isInFov(x, y)) {
		tiles[x + y * width].explored = true;
		return true;
	}
	return false;
}
bool Map::isExplored(int x, int y) const {
//	LOGMSG("Checking location (" << x << ", " << y << ") at [" << (x + y * width) << "]");
	return tiles[x + y * width].explored;
}
bool Map::isHolding(int x, int y) const {
//	return tiles[x + y * width].itemList;
	return false;
}
// TOOLS
Actor *getOccupant (int x, int y) {
	return NULL;
}
// Map Generation
void Map::generateTerrain(bool isNew, int width, int height) {
	// isNew controls whether the map is being made from scratch or not
	// currently all maps are new, and we ARE using the tutorial fxns
	this->createRoom(true, 3, 3, width - 3, height - 3, true);
//	this->dig(engine.player->xpos - 3, engine.player->ypos, engine.player->xpos + 3, engine.player->ypos);
}
void Map::createRoom (bool first, int x1, int y1, int x2, int y2, bool withActors) {
	dig(x1, y1, x2, y2);
	if (!withActors) {
		return;
	}
}
void Map::dig (int x1, int y1, int x2, int y2) {
	// this would be the appropriate place to set a tile's biome and terrain
	// types, but obviously will need to wait until more complex logic is here
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
			// everything that isn't a wall, becomes grass
			visionMap->setProperties(tilex, tiley, true, true);
			Tile *target = &tiles[tilex + tiley * width];
			target->glyph = 46;
			target->foreColor = TCODColor::darkerGreen;
			target->backColor = TCODColor::darkestGreen;
		}
	}
}
void Map::addAnimal(int x, int y) {
	// spawns an animal at the specified x,y coords
	Actor *monkey = new Actor(x, y, 'm', TCODColor::sepia, "monkey");
	monkey->sentience = new AnimalSentience();
	monkey->mortality = new NPCMortality(5, 0, "monkey corpse");
	monkey->container = new Container(1);
	engine.actors.push(monkey);
//	LOGMSG("New animal created at " << x << ", " << y);
}
// from the tutorial
// these are used for BSP dungeon generation
/*void Map::addMonster(int x, int y) {
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
}*/
