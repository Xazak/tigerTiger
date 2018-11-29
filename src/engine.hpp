class GameEngine {
	public:
		struct EngineState {
			virtual void update() {};
		};
		struct Startup: public EngineState {
			void update();
		} static startup;
		struct NewTurn: public EngineState {
			void update();
		} static newTurn;
		struct Ongoing: public EngineState {
			void update();
		} static ongoing;
		struct Idle: public EngineState {
			void update();
		} static idle;
		void switchMode(EngineState *newMode);
		EngineState *currMode = NULL;
		EngineState *prevMode = NULL;

		GameGUI *gui; // points to master GUI object
		int screenWidth = 0;
		int screenHeight = 0;

//		GameClock *chrono; // points to world-scope timekeeping tools

		TCODList<Actor *> allActors; // contains ALL ACTORS
		TCODList<Actor *> actionQueue; // contains only LOCAL and SENTIENT actors
		Actor *player; // always points to player actor
		TCOD_key_t lastKey; // contains last key pressed by player
	//	TCOD_mouse_t mouse;
		int fovRadius;

		GameMap *map; // points to master map object

		// *** MAJOR FXNS
		GameEngine(int screenWidth, int screenHeight);
		~GameEngine();
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
		TCODRandom *rng; // pointer to RNG engine

	private:
		long seed; // RNG seed (stored for reuse)
};
extern GameEngine engine; // 'extern' tells the compiler to look elsewhere for the actual declaration
