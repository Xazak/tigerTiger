/*	FILE: map.hpp
CONTENTS: Declarations for Map and Tile classes
	*/
struct Tile {
	enum ProtoType { FLOOR, WALL, TRANSITION } protoType;
	enum BiomeType { MOUNTAIN, GRASSLAND, RAINFOREST, MANGROVE, FARMLAND, CAVE } biomeType;
	enum TerrainType { GRASS, MUD, GRAVEL, WATER, ROCK, TREE, BUILDING } terrain;
	int  glyph;		// contains the ASCII code for the symbol to allow addressing of extended chars
	TCODColor color;
	// Obstruction and Obfuscation are handled by the isWalkable() and
	// isTransparent() functions provided under TCODMap
	// these properties can be changed by calling
	//    TCODMap::setProperties
	//    (int x, int y, bool isTransparent, bool isWalkable)
	bool explored;	// has the player seen this tile?
	// Querying whether these pointers exists serves as a yes/no check
	Actor *occupant; // pointer to the actor occupying this tile
//	Scent *scentTrail; // pointer to scent information
//	Item *itemList;	// pointer to tile's contents
	Tile(): explored(false) {} // set tiles unseen by default
};

class Map {
public:
	int width, height;
//	SYSTEM FXNS
	Map(int width, int height);
	~Map();
	void computeFOV(); // redraw FOV on map
	void render() const;
	void init(bool withActors); // initialize the map; false = no spawns
//	void load(TCODZip &zip);
//	void save(TCODZip &zip);
//	QUERIES
	bool isObstructed(int x, int y) const; // can we walk here?
	bool isVisible(int x, int y) const; // is the tile visible?
	bool isExplored(int x, int y) const; // has the tile been explored?
	bool isOccupied(int x, int y) const; // is the tile occupied by someone?
	bool isHolding(int x, int y) const; // are there items on the tile?
//	TOOLS
	void addMonster(int x, int y); // add a monster to the map
	void addItem(int x, int y); // add an item to the map

protected:
	Tile *tiles; // pointer to tile array
	TCODMap *map; // pointer to master map object
	TCODRandom *rng; // pointer to RNG engine
	long seed; // RNG seed (stored for reuse)
	void generateMap(bool isNew, int width, int height);

	friend class BspListener; // dungeon-generator tool
	void dig(int x1, int y1, int x2, int y2); // creates a space for a room
	// fills in an open space with room furniture, monster spawns, etc
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors);
};
