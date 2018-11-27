class GameEngine {
	public:
	struct EngineState {
		virtual void update() {};
	};
	struct Startup: public EngineState {
		void update();
	};
	void switchMode(EngineState *newMode);

	EngineState *currMode = NULL;
	EngineState *prevMode = NULL;
	int screenHeight = 0;
	int screenWidth = 0;

	GameGUI *gui; // points to master GUI object

	GameClock *chrono; // points to world-scope timekeeping tools

	TCODList<Actor *> allActors; // contains ALL ACTORS
	TCODList<Actor *> actionQueue; // contains only LOCAL and SENTIENT actors
	Actor *player; // always points to player actor
	TCOD_key_t lastKey; // contains last key pressed by player
//	TCOD_mouse_t mouse;
	int fovRadius = 0;

	GameMap *map; // points to master map object

	// *** MAJOR FXNS
	Engine(int screenWidth, int screenHeight);
	~Engine();
	void init(); // starts the game engine
	void term(); // stops the game engine
	void save(); // write game state to external file
	void load(); // load game state from external file
	void update(); // processes game state updates
	void render(); // invokes lower-level render requests
	// *** MINOR FXNS
	// pushes the *actor to the rear-most screen layer so that other sigils
	// can be drawn on top of it, ie for non-blocking actors
	void sendToBack(Actor *actor);
	Actor *getClosestMonster(int x, int y, float range) const;
	bool pickATile(int *x, int *y, float maxRange = 0.0f);
	Actor *getActor(int x, int y) const;
};
extern GameEngine engine; // 'extern' tells the compiler to look elsewhere for the actual declaration
