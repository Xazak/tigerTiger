class Engine {
	public:
		// changing gameStatus tells the engine to change states
		enum GameStatus {
			STARTUP, IDLE, NEW_TURN, VICTORY, DEFEAT
		} gameStatus;
		TCODList<Actor *> actors; // the list of all actors on the map
		Actor *player; // ptr to the player object
		Map *map; // ptr to the master Map object
		int fovRadius; // should this come from the actor object?
		int screenWidth;
		int screenHeight;
		Gui *gui; // pointer to main GUI object
		TCOD_key_t lastKey; // contains last key pressed by player
//		TCOD_mouse_t mouse;

		Engine(int screenWidth, int screenHeight);
		~Engine();
		void update(); //?
		void render(); //?
		// pushes the *actor to the rear-most screen layer so that other sigils
		// can be drawn on top of it, ie for non-blocking actors
		void sendToBack(Actor *actor);
		Actor *getClosestMonster(int x, int y, float range) const;
		bool pickATile(int *x, int *y, float maxRange = 0.0f);
		Actor *getActor(int x, int y) const;
		void init(); // starts the game engine
		void term(); // stops the game engine
		void save();
		void load();
};

extern Engine engine; // 'extern' tells compiler that the global var 'engine' has been declared elsewhere
