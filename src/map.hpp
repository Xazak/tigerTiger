// public Tile class for map objects; passable by default
struct Tile {
   bool canWalk; // can we walk through this tile?
   Tile() : canWalk(true) {}
};

class Map {
public :
   int width, height;

   Map(int width, int height);
   ~Map();
   bool isWall(int x, int y) const;
   void render() const;
protected :
   Tile *tiles;

   void setWall(int x, int y);
};
