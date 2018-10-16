class Engine {
	public:
		enum GameStatus {
			STARTUP,
			IDLE,
			NEW_TURN,
			VICTORY,
			DEFEAT
		} gameStatus;
		TCODList<Actor *> actors; // the list of all actors on the map
		Actor *player; // ptr to the player object
		Map *map; // ptr to the master Map object
		int fovRadius;
		int screenWidth;
		int screenHeight;
		Gui *gui;
		TCOD_key_t lastKey;
		TCOD_mouse_t mouse;

		Engine(int screenWidth, int screenHeight);
		~Engine();
		void update();
		void render();
		void sendToBack(Actor *actor);
		Actor *getClosestMonster(int x, int y, float range) const;
		bool pickATile(int *x, int *y, float maxRange = 0.0f);
		Actor *getActor(int x, int y) const;
		void init();
		void save();
		void load();
};

extern Engine engine; // 'extern' tells compiler that the global var 'engine' has been declared elsewhere
