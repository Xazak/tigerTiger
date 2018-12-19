/*	 map.hpp
DATE Oct 08 2018
AUTH xazak
DESC Declarations for Map and Tile classes
 */
struct Tile {
	enum ProtoType { FLOOR, WALL, TRANSITION } proto;
	enum BiomeType { MOUNTAIN, GRASSLAND, RAINFOREST, MANGROVE, FARMLAND, CAVE } biome;
	enum TerraType { GRASS, MUD, GRAVEL, WATER, ROCK, TREE, BUILDING } terrain;
	int  glyph;		// contains the ASCII code for the symbol to allow addressing of extended chars
	TCODColor foreColor;
	TCODColor backColor;
	/* Obstruction and Obfuscation are handled by the isWalkable() and
	 * isTransparent() functions provided under TCODMap:
	 *    TCODMap::
	 * these properties can be changed by calling
	 *    TCODMap::setProperties
	 *    (int x, int y, bool isTransparent, bool isWalkable)
	 */
	bool explored;	// has the player seen this tile?
	Actor *occupant; // pointer to the actor occupying this tile
//	Scent *scentTrail; // pointer to scent information
//	Item *itemList;	// pointer to tile's contents
	Tile();
//	~Tile();
};
class GameMap {
	public:
		int width;
		int height;
		enum ObstructionType {
			NOTHING,
			WALL,
			OBJECT,
			CREATURE
		};
		// **** SYSTEM FXNS
		GameMap(int width, int height);
		~GameMap();
		void computeFOV(); // redraw FOV on map
		void render() const;
		void init(bool withActors); // initialize the map; false = no spawns
		void save(TCODZip &fileBuffer);
		void load(TCODZip &fileBuffer);
		// **** QUERIES
		bool isWall(int x, int y) const; // is there a wall here?
		bool isOccupied(int x, int y) const; // is the tile occupied by someone?
		bool isObstructed(int x, int y) const; // is the tile blocked by an actor?
		bool isVisible(int x, int y) const; // is the tile visible?
		bool isExplored(int x, int y) const; // has the tile been explored?
		bool isHolding(int x, int y) const; // are there items on the tile?
		Actor *getOccupant(int x, int y); // get a pointer to the actor standing on the target tile
		// **** TOOLS
		void addAnimal(int x, int y); // spawn an animal on the map

	protected:
		Tile *tiles; // pointer to tile array
		TCODMap *visionMap; // pointer to master map object
		void generateTerrain(bool isNew, int width, int height);
		void dig(int x1, int y1, int x2, int y2); // creates a space for a room
		// fills in an open space with room furniture, monster spawns, etc
		void createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors);
};
