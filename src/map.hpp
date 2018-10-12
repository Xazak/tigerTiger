static const int MAX_ROOM_MONSTERS = 3;

// public Tile class for map objects; passable by default
struct Tile {
   bool explored; // have we been here already?
   unsigned int scent; // amount of player scent on the part
   Tile(): explored(false), scent(0) {}
};

class Map {
public:
   int width, height;

   Map(int width, int height);
   ~Map();
   bool isWall(int x, int y) const;
   bool isInFOV(int x, int y) const;
   bool isExplored(int x, int y) const;
   bool canWalk(int x, int y) const;
   void computeFOV();
   void render() const;
   void addMonster(int x, int y);
   unsigned int getScent(int x, int y) const;
   unsigned int currentScentValue;

protected:
   Tile *tiles;
   TCODMap *map;
   friend class BspListener;

   void dig(int x1, int y1, int x2, int y2);
   void createRoom(bool first, int x1, int y1, int x2, int y2);
};
