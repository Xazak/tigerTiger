/*	 map.cpp
DATE Oct 08 2018
AUTH xazak
DESC Implementations of Map and Tile classes, incl render methods
 */
#include "main.hpp"
#include <math.h>

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_ITEMS = 2;
static const int MAX_ROOM_MONSTERS = 3;

// **** TILE
// note that when proto = biome = terrain = 0, it is a BEDROCK tile!
Tile::Tile(): proto(WALL), biome(MOUNTAIN), terrain(BEDROCK),
	glyph(43), foreColor(25, 25, 25), backColor(127, 127, 127),
	explored(false), occupant(NULL) { }
//Tile::~Tile() { }
// **** MAP
GameMap::GameMap(int inputWidth, int inputHeight) : width(inputWidth), height(inputHeight) {
	// pick a random seed between 0 and (a big number)
//	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}
GameMap::~GameMap() {
	delete [] tiles;
	delete visionMap;
}
void GameMap::computeFOV() {
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
void GameMap::render() const {
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
void GameMap::init(bool newMap) {
	// create the map objects
	tiles = new Tile[width * height]; // use x + y * width to locate a tile
	LOGMSG("new tilemap created");
	visionMap = new TCODMap(width, height); // invokes the TCOD FOV map object
	LOGMSG("new visionMap created");
	// create a new map if specified to do so
	if (newMap) {
		generateTestMap(true, width, height); // creates the debugging map
		//generateTerrain(true, width, height);
		LOGMSG("generating terrain");
	}
	LOGMSG("map initialized");
}
void GameMap::save(TCODZip &fileBuffer) {
	// save the map dimensions
	fileBuffer.putInt(width);
	fileBuffer.putInt(height);
	// save the tile map info
	for (int index = 0; index < width * height; index++) {
		fileBuffer.putInt((int)tiles[index].proto);
		fileBuffer.putInt((int)tiles[index].biome);
		fileBuffer.putInt((int)tiles[index].terrain);
		fileBuffer.putInt(tiles[index].glyph);
		fileBuffer.putColor(&tiles[index].foreColor);
		fileBuffer.putColor(&tiles[index].backColor);
		fileBuffer.putInt(tiles[index].explored);
		// scent data
		// tile contents
	}
	// save the vision map info
	for (int echs = 0; echs < width; echs++) {
		for (int whye = 0; whye < height; whye++) {
			fileBuffer.putInt(visionMap->isWalkable(echs, whye));
			fileBuffer.putInt(visionMap->isTransparent(echs, whye));
		}
	}
}
void GameMap::load(TCODZip &fileBuffer) {
	// this is called on a map that has already been created with dimensions!!
	LOGMSG("called");
	// fill the tile map
	for (int index = 0; index < width * height; index++) {
		tiles[index].proto = (Tile::ProtoType)fileBuffer.getInt();
		tiles[index].biome = (Tile::BiomeType)fileBuffer.getInt();
		tiles[index].terrain = (Tile::TerraType)fileBuffer.getInt();
		tiles[index].glyph = fileBuffer.getInt();
		tiles[index].foreColor = fileBuffer.getColor();
		tiles[index].backColor = fileBuffer.getColor();
		tiles[index].explored = fileBuffer.getInt();
		// scent data
		// tile contents
	}
	// set up the vision map
	bool walkable = false;
	bool transparent = false;
	for (int echs = 0; echs < width; echs++) {
		for (int whye = 0; whye < height; whye++) {
			walkable = fileBuffer.getInt();
			transparent = fileBuffer.getInt();
			visionMap->setProperties(echs, whye, walkable, transparent);
		}
	}
}
// should these be overloaded to use Tile pointers as well?
// **** MAP QUERIES
bool GameMap::isWall(int x, int y) const {
	return !visionMap->isWalkable(x, y);
}
bool GameMap::isOccupied(int x, int y) const {
	// returns true if the target tile contains an actor
	return tiles[x + y * width].occupant;
}
bool GameMap::isObstructed(int x, int y) const {
	// returns true if the target cell contains an obstructing object
	Tile *target = &tiles[x + y * width];
	if (isOccupied(x, y)) {
		return target->occupant->obstructs;
	}
	return false;
}
bool GameMap::isVisible(int x, int y) const {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
	if (visionMap->isInFov(x, y)) {
		tiles[x + y * width].explored = true;
		return true;
	}
	return false;
}
bool GameMap::isExplored(int x, int y) const {
//	LOGMSG("Checking location (" << x << ", " << y << ") at [" << (x + y * width) << "]");
	return tiles[x + y * width].explored;
}
bool GameMap::isHolding(int x, int y) const {
//	return tiles[x + y * width].itemList;
	return false;
}
Actor *getOccupant (int x, int y) {
	return NULL;
}
// **** GameMap Generation
void GameMap::generateTestMap(bool isNew, int width, int height) {
	// generates the more-or-less statically defined debugging map
	// currently runs at user's request, creating objects on the fly
	// may later include a switch that loads from REXPaint files...
	// This fxn is responsible for:
	// -- setting each cell of tiles[tilex + tiley * width] to correct values
	// PROPERTIES
	// ProtoType: FLOOR | WALL | TRANSITION
	// 98% of a map will be FLOOR; WALL is bedrock tiles, not to be seen
	// BiomeType: MOUNTAIN | GRASSLAND | RAINFOREST|MANGROVE|FARMLAND|CAVE
	// this will be set at a larger level, probably when chunks are implemented
	// TerraType: GRASS|MUD|GRAVEL|WATER|ROCK|BRUSH|BUILDING
	// this value determines the tile's appearance
	// TYPE		GLYPH	FORECOLOR	BACKCOLOR
	// grass	19		darkergreen	darkestgreen
	// dirt		32		none		darkestsepia
	// mud		32		none		darkersepia
	// gravel	178		darkgrey	darkergrey
	// water	247		darkazure	darkerazure
	// rock		32		none		darkestgrey
	// brush	231		darkerlime	darkestlime
	// building	32		none		black
	// explored = false by default
	//
	// -- setting each cell of visionMap->setProperties(x, y, transp?, open?);
	// -- setting each cell of scentMap correctly (NOT YET IMPLEMENTED)

	/* SIMPLE METHOD FOR MAP GENERATION
	01	Create the fundamental map: dig a space, set bedrock/floor tiles
	02	Paint the map with other textures: grass, water, mud, brush, etc
	03	Examine the map for 'interesting' places and add their coords to a list
	04	Using each list of interests, place terrain objects: trees, etc
	*/
	//this->createRoom(true, 3, 3, width - 3, height - 3, true);
	// carve out a simple room: all open tiles will be marked as grass
	// leave a 3-tile border around the edge for bedrock
	Tile *target = nullptr;
	int newWidth = width - 3;
	int newHeight = height - 3;
	int echs = 0;
	int whye = 0;
	dig(3, 3, newWidth, newHeight);
	// paint some rock, dirt across the landscape
	TCODNoise *noiseGen = new TCODNoise(2, engine.rng); // start a 2d noise generator
	float index[2] = {0.0f, 0.0f}; // using x, y coords of map as inputs
	float result = 0.0f; // output value of -1<=n<=1, to be rounded
	float upperBound = 0.5f;
	float lowerBound = -0.5f;
	// 1 = grass (do nothing), 0 = dirt, -1 = rock
//	float foo = noiseGen->get(index);
	for (echs = 0; echs < newWidth; echs++) {
		for (whye = 0; whye < newHeight; whye++) {
			target = &tiles[echs + whye * width];
			index[0] = echs;
			index[1] = whye;
			result = noiseGen->get(index);
//			result = noiseGen->getTurbulence(index, 8.0f);
			// round off result to nearest whole num
			if (result <= lowerBound) { // ~-1
				result = -1.0f;
			} else if (result < upperBound) { // ~0
				result = 0.0f;
			} else { // ~1
				result = 1.0f;
			}
			// set the tilemap at that point by the following switch case
			switch((int)result) {
				case -1: // rock
					target->terrain = Tile::TerraType::ROCK;
					target->backColor = TCODColor::darkestGrey;
					break;
				case 0: // dirt
					target->terrain = Tile::TerraType::DIRT;
					target->backColor = TCODColor::darkestSepia;
					break;
				case 1: // grass
					// do nothing
					break;
				default: // BAD
					break;
			}
		}
	}
	// put a big watering hole in the middle
	// - pick an upper-left coordinate: mapCenterX,Y +- 1d8
	// - take the horiz/vert distance to x/y axis as a, b
	// - using mapCenter as (h, v) and a, b as above, mark all the water tiles
	// normalize x, y and offset to the upper left by 30 and 20 tiles respc.
	echs = (width / 2) - 30;
	whye = (height / 2) - 20;
	// adjust their positions for funsies
	int offset = engine.rng->get(-5, 5);
	echs += offset;
	offset = engine.rng->get(-5, 5);
	whye += offset;
	// get the horiz/vert distance to the axes
	int xDist = (width / 2) - echs;
	int yDist = (height / 2) - whye;
	// eqn to draw an ellipse:
	// (x-h)^2   (y-v)^2
	// ------- + ------- = 1
	//   a^2       b^2
	// ...
	// y = b ( 1 - ((x - h) / a) ) + v
	// where echs = h, whye = v, xDist = a, yDist = b

	// put mud around the watering hole
	// - as above, for +1 to a, b
	// add some trees
	// add some bushes to hide in
	// add some tall grass to hide in

}
void GameMap::addAnimal(int x, int y) {
	// spawns an animal at the specified x,y coords
	Actor *monkey = new Actor(x, y, 'm', TCODColor::sepia, "monkey");
	monkey->sentience = new AnimalSentience();
	monkey->mortality = new Mortality(5, 0, "monkey corpse");
	monkey->container = new Container(1);
	monkey->tempo = new ActorClock(100);
	engine.allActors.push(monkey);
//	LOGMSG("New animal created at " << x << ", " << y);
}
void GameMap::generateTerrain(bool isNew, int width, int height) {
	// isNew controls whether the map is being made from scratch or not
	// currently all maps are new, and we ARE using the tutorial fxns
	this->createRoom(true, 3, 3, width - 3, height - 3, true);
}
void GameMap::createRoom (bool first, int x1, int y1, int x2, int y2, bool withActors) {
	dig(x1, y1, x2, y2);
	if (!withActors) {
		return;
	}
}
void GameMap::dig (int x1, int y1, int x2, int y2) {
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
	Tile *target = nullptr;
	for (int tilex = x1; tilex <= x2; tilex++) {
		for (int tiley = y1; tiley <= y2; tiley++) {
			// everything that isn't a wall, becomes grass
			target = &tiles[tilex + tiley * width];
			target->proto = Tile::ProtoType::FLOOR;
			target->biome = Tile::BiomeType::GRASSLAND;
			target->terrain = Tile::TerraType::GRASS;
			target->glyph = 32;
			target->foreColor = TCODColor::darkerGreen;
			target->backColor = TCODColor::darkestGreen;
			visionMap->setProperties(tilex, tiley, true, true);
		}
	}
}
