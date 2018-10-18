static const int MAX_ROOM_MONSTERS = 3;

// public Tile class for map objects; passable by default
struct Tile {
	bool explored; // have we been here already?
	Tile(): explored(false) {} // set tiles passable by default
};

class Map {
public:
	int width, height;

	Map(int width, int height);
	~Map();
	bool isWall(int x, int y) const; // is the tile a wall?
	bool isInFOV(int x, int y) const; // is the tile visible?
	bool isExplored(int x, int y) const; // has the tile been explored?
	bool canWalk(int x, int y) const; // is the tile passable?
	void computeFOV(); // redraw FOV on map
	void render() const;
	void addMonster(int x, int y); // add a monster to the map
	void addItem(int x, int y); // add an item to the map
	void init(bool withActors); // initialize the map; false = no spawns
//	void load(TCODZip &zip);
//	void save(TCODZip &zip);

protected:
	Tile *tiles; // pointer to ?
	TCODMap *map; // pointer to master map object
	long seed; // RNG seed (stored for reuse)
	TCODRandom *rng; // pointer to RNG engine
	friend class BspListener; //?

	void dig(int x1, int y1, int x2, int y2); // creates a space for a room
	// fills in an open space with room furniture, monster spawns, etc
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors);
};
